#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>

#include <cassert>
#include <cmath>
#include <iostream>

#include <SDL/SDL_timer.h>

#include "../system/Defines.hpp"

#include "./RayTracer.hpp"
#include "./Scene.hpp"
#include "./SceneLight.hpp"
#include "./SceneObject.hpp"
#include "./Material.hpp"
#include "./MaterialReflectionModel.hpp"
#include "./Camera.hpp"
#include "../datastructs/PhotonMap.hpp"
#include "../math/Ray.hpp"
#include "../system/LuaParser.hpp"
#include "../system/Profiler.hpp"
#include "../system/SDLWindow.hpp"
#include "../system/RNG.hpp"

RayTracer::RayTracer(LuaParser& parser, const Scene& scene): numThreads(1), mapNumPhotons(0) {
	const LuaTable* rootTable = parser.GetRootTbl();
	const LuaTable* tracerTable = rootTable->GetTblVal("raytracer");

	const std::list<ISceneLight*>& lights = scene.GetLights();

	photonMapping = true;

	for (std::list<ISceneLight*>::const_iterator it = lights.begin(); it != lights.end(); it++) {
		mapNumPhotons += (*it)->GetNumPhotons();
		photonMapping = (photonMapping && ((*it)->GetNumPhotons() > 0));
	}

	numThreads = uint(tracerTable->GetFltVal("numThreads", boost::thread::hardware_concurrency()));
	antiAliasing = bool(tracerTable->GetFltVal("antiAliasing", 0));
	incrementalRender = bool(tracerTable->GetFltVal("incrementalRender", 1.0f));

	assert(numThreads >= 1);

	maxRayDepth = uint(tracerTable->GetFltVal("maxRayDepth", 1));
	maxPhotonDepth = uint(tracerTable->GetFltVal("maxPhotonDepth", 0));

	if (photonMapping) {
		photonSearchCount = uint(tracerTable->GetFltVal("photonSearchCount", 1));
		photonSearchRadius = tracerTable->GetFltVal("photonSearchRadius", 1.0f);

		photonMap = new PhotonMap::Map(mapNumPhotons, PhotonMap::PHOTONMAP_GLOBAL);
	} else {
		photonSearchCount = 0;
		photonSearchRadius = 0.0f;

		photonMap = NULL;
	}

	profiler = new Profiler(numThreads, maxRayDepth, maxPhotonDepth, RAY_TYPE_LAST, PHOTON_MATINT_LAST);

	std::cout << "[RayTracer::RayTracer]" << std::endl;
	std::cout << "\tnumThreads:        " << numThreads        << std::endl;
	std::cout << "\tantiAliasing:      " << antiAliasing      << std::endl;
	std::cout << "\tincrementalRender: " << incrementalRender << std::endl;
	std::cout << std::endl;
	std::cout << "\tMONTE_CARLO_SOFT_SHADOWS:              " << MONTE_CARLO_SOFT_SHADOWS              << std::endl;
	std::cout << "\tNUM_MONTE_CARLO_LIGHT_SAMPLES:         " << NUM_MONTE_CARLO_LIGHT_SAMPLES         << std::endl;
	std::cout << "\tPHOTON_ENERGY_CONSERVATION:            " << PHOTON_ENERGY_CONSERVATION            << std::endl;
	std::cout << "\tPHOTON_MAP_INDIRECT_ILLUMINATION_ONLY: " << PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY << std::endl;
	std::cout << "\tIRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY: " << IRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY << std::endl;
	std::cout << "\tNUM_IRRADIANCE_GATHER_RAYS:            " << NUM_IRRADIANCE_GATHER_RAYS            << std::endl;
	std::cout << "\tIRRADIANCE_GATHER_RAY_WEIGHT:          " << IRRADIANCE_GATHER_RAY_WEIGHT          << std::endl;
	std::cout << std::endl;
	std::cout << "\tmaxRayDepth:        " << maxRayDepth        << std::endl;
	std::cout << "\tmaxPhotonDepth:     " << maxPhotonDepth     << std::endl;
	std::cout << "\tmapNumPhotons:      " << mapNumPhotons      << std::endl;
	std::cout << "\tphotonSearchCount:  " << photonSearchCount  << std::endl;
	std::cout << "\tphotonSearchRadius: " << photonSearchRadius << std::endl;
}

RayTracer::~RayTracer() {
	if (photonMapping) {
		delete photonMap;
	}

	delete profiler;
}



math::vec3f RayTracer::SampleDirectIllumination(
	unsigned int threadNum,
	const Scene& scene,
	const math::RaySegment& ray,
	const math::RayIntersection& rayInt,
	RNGflt64* rng,
	unsigned int rayDepth
) const {
	math::vec3f irr;

	if (ray.IsInside()) {
		return irr;
	}

	#if (MONTE_CARLO_SOFT_SHADOWS == 0)
	const static int NUM_AREA_LIGHT_SURFACE_POS_OFFSETS = 6;
	const static math::vec3f areaLightSurfacePosOffsets[NUM_AREA_LIGHT_SURFACE_POS_OFFSETS] = {
		math::XVECf, -math::XVECf,
		math::YVECf, -math::YVECf,
		math::ZVECf, -math::ZVECf
	};
	#endif

	const ISceneObject*             obj          = rayInt.GetObj();
	const Material*                 objMat       = obj->GetMaterial();
	const IMaterialReflectionModel* objMatRefMod = objMat->GetReflectionModel();

	/*
	 * add each light's contribution
	 * a light contributes to the pixel color if and only if
	 *  1. the intersected surface is oriented toward the light
	 *  2. the intersection point has line-of-sight to the light
	 *  3. the intersection point is within the light's FOV cone
	 */
	const std::list<ISceneLight*>& lights = scene.GetLights();

	// if ray is inside an object, don't sample light-sources (?)
	for (std::list<ISceneLight*>::const_iterator it = lights.begin(); it != lights.end(); it++) {
		const ISceneLight* light = *it;
		// vector from intersection position to the light
		const math::vec3f L = (light->GetPos() - rayInt.GetPos()).norm();
		const math::vec3f P = rayInt.GetPos() + L * 0.01f;
		const math::RaySegment lightRay(P, L);

		math::RayIntersection lightRayInt;

		if (light->GetRadius() > 0.0f) {
			// area light, so must probe visibility to
			// various points on the light's "surface"
			unsigned int hitLightSamples = 0;

			#if (MONTE_CARLO_SOFT_SHADOWS == 0)
			const unsigned int numLightSamples = NUM_AREA_LIGHT_SURFACE_POS_OFFSETS;
			#else
			const unsigned int numLightSamples = NUM_MONTE_CARLO_LIGHT_SAMPLES;
			#endif

			math::vec3f areaLightPos;
			math::vec3f areaLightDir;
			math::vec3f emissionDir;

			for (unsigned int i = 0; i < numLightSamples; i++) {
				lightRayInt.SetObj(NULL);

				#if (MONTE_CARLO_SOFT_SHADOWS == 0)
				areaLightPos = light->GetPos() + (areaLightSurfacePosOffsets[i] * light->GetRadius());
				#else
				areaLightPos = light->GetPos() + (emissionDir.rrandomize(rng) * light->GetRadius());
				#endif

				// vector toward the light surface position
				areaLightDir = (areaLightPos - rayInt.GetPos()).norm();

				const math::RaySegment areaLightRay(rayInt.GetPos() + areaLightDir * 0.01f, areaLightDir);

				// note: areaLightPos must fall within the scene bounds
				// note: take the light's own radius into account here?
				const bool haveShadowCaster = (scene.GetOccludingObject(threadNum, areaLightRay, &lightRayInt, light->GetPos()) != NULL);
				const bool fakeShadowCaster = haveShadowCaster &&
					((lightRayInt.GetPos() - rayInt.GetPos()).sqLen3D() >
					(light->GetPos() - rayInt.GetPos()).sqLen3D());

				if (!haveShadowCaster || fakeShadowCaster) {
					hitLightSamples++;
				}

				profiler->IncCounter(Profiler::COUNTER_RAY, threadNum, rayDepth, RAY_TYPE_SHADOW);
			}

			// scale the intensity by how many light "surface"
			// positions are visible from the intersection point
			irr += (objMatRefMod->GetIntensity(objMat, light, ray, lightRay, &rayInt) * (float(hitLightSamples) / numLightSamples));

			#if (DEBUG_ASSERTS_RAYTRACER == 1)
			assert(irr.x != M_INF() && irr.x != M_NAN());
			assert(irr.y != M_INF() && irr.y != M_NAN());
			assert(irr.z != M_INF() && irr.z != M_NAN());
			#endif
		} else {
			/*
				* note: finding out whether the caster is further
				* away than the light-source is necessary because
				* lights are not treated as first-class objects
				*
				* note: also because of this, the usual speed-up
				* of probing whether any object occludes the ray
				* towards the light-source cannot be applied
				*/
			lightRayInt.SetObj(NULL);

			const bool haveShadowCaster = (scene.GetOccludingObject(threadNum, lightRay, &lightRayInt, light->GetPos()) != NULL);
			const bool fakeShadowCaster = haveShadowCaster &&
				((lightRayInt.GetPos() - rayInt.GetPos()).sqLen3D() >
				(light->GetPos() - rayInt.GetPos()).sqLen3D());

			if (!haveShadowCaster || fakeShadowCaster) {
				// evaluate non-approximate direct illumination under
				// point-lights with ordinary shadow rays [Jensen, ch9]
				// note: could also use "shadow photons" for this step
				irr += objMatRefMod->GetIntensity(objMat, light, ray, lightRay, &rayInt); 

				#if (DEBUG_ASSERTS_RAYTRACER == 1)
				assert(irr.x != M_INF() && irr.x != M_NAN());
				assert(irr.y != M_INF() && irr.y != M_NAN());
				assert(irr.z != M_INF() && irr.z != M_NAN());
				#endif
			}
		}

		profiler->IncCounter(Profiler::COUNTER_RAY, threadNum, rayDepth, RAY_TYPE_SHADOW);
	}

	return irr;
}

math::vec3f RayTracer::GatherIrradianceEstimate(unsigned int threadNum, const math::RayIntersection* rayInt, const Scene& scene, RNGflt64* rng) {
	math::vec3f irr;
	math::vec3f est;

	const ISceneObject* obj    = rayInt->GetObj();
	const Material*     objMat = obj->GetMaterial();

	#if (NUM_IRRADIANCE_GATHER_RAYS <= 0)
		threadNum = -1;
		rng = rng;

		est = photonMap->GetIrradianceEstimate(rayInt->GetPos(), rayInt->GetNrm(), photonSearchRadius, photonSearchCount);
		#if (IRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY == 1)
		est *= objMat->GetDiffuseReflectiveness();
		#endif
		irr = est;
	#else
		// note: a (weighted) average over multiple diffuse rays
		// reduces noise, but destroys caustics since these are
		// stored in the same photon-map
		unsigned int numSpecularObjects = 0;

		for (unsigned int i = 0; i < NUM_IRRADIANCE_GATHER_RAYS; i++) {
			math::vec3f gatherRayDir = -rayInt->GetNrm();

			while (gatherRayDir.dot3D(rayInt->GetNrm()) < 0.0f) {
				gatherRayDir.rrandomize(rng);
			}

			math::RaySegment gatherRay(rayInt->GetPos() + (gatherRayDir * 0.01f), gatherRayDir, false);
			math::RayIntersection gatherRayInt;

			if (scene.GetClosestObject(threadNum, gatherRay, &gatherRayInt) != NULL) {
				const ISceneObject* gObj    = gatherRayInt.GetObj();
				const Material*     gObjMat = gObj->GetMaterial();

				if (!gObjMat->IsSpecularlyReflective()) {
					est = photonMap->GetIrradianceEstimate(gatherRayInt.GetPos(), gatherRayInt.GetNrm(), photonSearchRadius, photonSearchCount);
					#if (IRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY == 1)
					est *= gObjMat->GetDiffuseReflectiveness();
					#endif
					irr += est;
				} else {
					numSpecularObjects += 1;
				}
			}
		}

		irr *= (IRRADIANCE_GATHER_RAY_WEIGHT);
		est = photonMap->GetIrradianceEstimate(rayInt->GetPos(), rayInt->GetNrm(), photonSearchRadius, photonSearchCount);
		#if (IRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY == 1)
		est *= objMat->GetDiffuseReflectiveness();
		#endif
		est *= (1.0f - IRRADIANCE_GATHER_RAY_WEIGHT);
		irr += est;
		irr /= (NUM_IRRADIANCE_GATHER_RAYS + 1 - numSpecularObjects);
	#endif

	return irr;
}






math::vec3f RayTracer::ShadeRayPM(
	unsigned int threadNum,
	const math::RaySegment& ray,
	const math::RayIntersection& rayInt,
	const Scene& scene,
	RNGflt64* rng,
	unsigned int rayDepth
) {
	math::vec3f irr;

	const ISceneObject* obj    = rayInt.GetObj();
	const Material*     objMat = obj->GetMaterial();

	#if (PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY == 1)
	// add the direct illumination from the lights
	// (if the photon-map only stores the indirect
	// illumination, then any surface visible from
	// the light-source will not be counted twice)
	if (objMat->IsSpecularlyReflective()) {
		irr += SampleDirectIllumination(threadNum, scene, ray, rayInt, rng, rayDepth);
	}
	#endif

	if (!objMat->IsSpecularlyReflective()) {
		// completely non-specular surface, use the irradiance estimate
		irr += GatherIrradianceEstimate(threadNum, &rayInt, scene, rng);
	} else {
		// note: lights are not treated as intersectable objects, so
		// when PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY is 0 specular
		// highlights can only come from (diffusely reflected) local
		// concentrations of photons on diffuse surfaces
		const math::vec3f& N = (ray.IsInside())? (-rayInt.GetNrm()): (rayInt.GetNrm());
		const math::vec3f  R = (ray.GetDir()).reflect(N);
		const math::vec3f  P = rayInt.GetPos() + R * 0.01f;
		const math::RaySegment reflectRay(P, R, ray.IsInside());

		// evaluate via standard raytracing
		irr += (TraceRay(threadNum, reflectRay, scene, rng, rayDepth + 1, RAY_TYPE_REFLECT) * objMat->GetSpecularReflectiveness());

		if (objMat->IsSpecularlyRefractive()) {
			// if going out of an object, switch refr. indices and invert normal
			const math::vec3f& N = (ray.IsInside())? (-rayInt.GetNrm()): (rayInt.GetNrm());
			const float n1 = (ray.IsInside())? (objMat->GetRefractionIndex()): (1.0f);
			const float n2 = (ray.IsInside())? (1.0f): (objMat->GetRefractionIndex());

			const math::vec3f R = (ray.GetDir()).refract(N, n1, n2);
			const math::vec3f P = rayInt.GetPos() + R * 0.01f;
			const math::RaySegment refractRay(P, R, !ray.IsInside());

			if (R != N) {
				if (objMat->GetBeerCoefficient() > 0.0f) {
					const math::vec3f absorbance = objMat->GetDiffuseReflectiveness() * objMat->GetBeerCoefficient() * -(rayInt.GetDistance());
					const math::vec3f refractIrr = (TraceRay(threadNum, refractRay, scene, rng, rayDepth + 1, RAY_TYPE_REFRACT) * objMat->GetSpecularRefractiveness());

					math::vec3f transparency;
						transparency.x = expf(absorbance.x);
						transparency.y = expf(absorbance.y);
						transparency.z = expf(absorbance.z);

					if (transparency.sqLen3D() > 0.001f) {
						irr += (refractIrr * transparency.norm());
					}
				} else {
					irr += (TraceRay(threadNum, refractRay, scene, rng, rayDepth + 1, RAY_TYPE_REFRACT) * objMat->GetSpecularRefractiveness());
				}

				#if (DEBUG_ASSERTS_RAYTRACER == 1)
				assert(irr.x != M_INF() && irr.x != M_NAN());
				assert(irr.y != M_INF() && irr.y != M_NAN());
				assert(irr.z != M_INF() && irr.z != M_NAN());
				#endif
			} else {
				// total internal reflection; spawn a
				// dedicated internal reflection ray
				// unless mat->IsSpecularlyReflective()?
			}
		}
	}

	return irr;
}



math::vec3f RayTracer::ShadeRayRT(
	unsigned int threadNum,
	const math::RaySegment& ray,
	const math::RayIntersection& rayInt,
	const Scene& scene,
	RNGflt64* rng,
	unsigned int rayDepth
) {
	const ISceneObject* obj    = rayInt.GetObj();
	const Material*     objMat = obj->GetMaterial();

	math::vec3f irr = SampleDirectIllumination(threadNum, scene, ray, rayInt, rng, rayDepth);

	if (objMat->IsSpecularlyReflective()) {
		const math::vec3f& N = (ray.IsInside())? (-rayInt.GetNrm()): (rayInt.GetNrm());
		const math::vec3f  R = (ray.GetDir()).reflect(N);
		const math::vec3f  P = rayInt.GetPos() + R * 0.01f;
		const math::RaySegment reflectRay(P, R, ray.IsInside());

		irr += (TraceRay(threadNum, reflectRay, scene, rng, rayDepth + 1, RAY_TYPE_REFLECT) * objMat->GetSpecularReflectiveness());

		#if (DEBUG_ASSERTS_RAYTRACER == 1)
		assert(irr.x != M_INF() && irr.x != M_NAN());
		assert(irr.y != M_INF() && irr.y != M_NAN());
		assert(irr.z != M_INF() && irr.z != M_NAN());
		#endif
	}

	if (objMat->IsSpecularlyRefractive()) {
		// if going out of an object, switch refr. indices and invert normal
		const math::vec3f& N = (ray.IsInside())? (-rayInt.GetNrm()): (rayInt.GetNrm());
		const float n1 = (ray.IsInside())? (objMat->GetRefractionIndex()): (1.0f);
		const float n2 = (ray.IsInside())? (1.0f): (objMat->GetRefractionIndex());

		const math::vec3f R = (ray.GetDir()).refract(N, n1, n2);
		const math::vec3f P = rayInt.GetPos() + R * 0.01f;
		const math::RaySegment refractRay(P, R, !ray.IsInside());

		if (R != N) {
			/*
			* apply Beer's law instead of the diffuse color, ie.
			*    c_out = c_in * exp(-(e * c * d))
			* where
			*    e is a constant that specifies the material absorbance
			*    c is the amount of material the ray passes through
			*    d is the path length for the ray
			* for the simplified version, we care only about
			*    out = in * exp(-(d * C))
			* where C is a constant specifying the material density
			*
			* note: distance should be the difference between the
			* entrance and exit point of the refracted ray through
			* an object, not the distance from ray position to the
			* entrance point itself
			* note: all colors are in [0, 1] but the transparency
			* (as derived from the absorbance factor) is not, yet
			* this gets multiplied by the color of the refracted
			* ray directly
			*/

			if (objMat->GetBeerCoefficient() > 0.0f) {
				const math::vec3f absorbance = objMat->GetDiffuseReflectiveness() * objMat->GetBeerCoefficient() * -(rayInt.GetDistance());
				const math::vec3f refractIrr = (TraceRay(threadNum, refractRay, scene, rng, rayDepth + 1, RAY_TYPE_REFRACT) * objMat->GetSpecularRefractiveness());

				math::vec3f transparency;
					transparency.x = expf(absorbance.x);
					transparency.y = expf(absorbance.y);
					transparency.z = expf(absorbance.z);

				if (transparency.sqLen3D() > 0.001f) {
					irr += (refractIrr * transparency.norm());
				}
			} else {
				irr += (TraceRay(threadNum, refractRay, scene, rng, rayDepth + 1, RAY_TYPE_REFRACT) * objMat->GetSpecularRefractiveness());
			}

			#if (DEBUG_ASSERTS_RAYTRACER == 1)
			assert(irr.x != M_INF() && irr.x != M_NAN());
			assert(irr.y != M_INF() && irr.y != M_NAN());
			assert(irr.z != M_INF() && irr.z != M_NAN());
			#endif
		} else {
			// total internal reflection; see ShadeRayPM
		}
	}

	return irr;
}



math::vec3f RayTracer::TraceRay(
	unsigned int threadNum,
	const math::RaySegment& ray,
	const Scene& scene,
	RNGflt64* rng,
	unsigned int rayDepth,
	unsigned int rayType
) {
	math::vec3f irr;

	if (rayDepth < maxRayDepth) {
		profiler->IncCounter(Profiler::COUNTER_RAY, threadNum, rayDepth, rayType);

		// get the closest object this ray intersects (if any)
		math::RayIntersection rayInt;

		if (scene.GetClosestObject(threadNum, ray, &rayInt) != NULL) {
			if (photonMapping) {
				#if (DEBUG_RENDER_PHOTON_MAP == 1)
				irr = photonMap->GetIrradianceEstimate(rayInt.GetPos(), rayInt.GetNrm(), 0.05f, 1);
				irr = ((irr.sqLen3D() > 0.0f)? math::UVECf: math::NVECf);
				#else
				irr += ShadeRayPM(threadNum, ray, rayInt, scene, rng, rayDepth);
				#endif
			} else {
				irr += ShadeRayRT(threadNum, ray, rayInt, scene, rng, rayDepth);
			}

			#if (DEBUG_ASSERTS_RAYTRACER == 1)
			assert(irr.x != M_INF() && irr.x != M_NAN());
			assert(irr.y != M_INF() && irr.y != M_NAN());
			assert(irr.z != M_INF() && irr.z != M_NAN());
			#endif
		}
	}

	return irr;
}



void RayTracer::TraceRayThread(unsigned int threadNum, SDLWindow& window, const Scene& scene, RNGflt64* rng) {
	static boost::mutex progressMutex;

	const unsigned int rows = window.GetSizeY() / numThreads;
	const unsigned int rest = (threadNum == (numThreads - 1))? (window.GetSizeY() % numThreads): 0;
	const unsigned int ymin = threadNum * rows;
	const unsigned int ymax = ymin + rows + rest;

	const float yrange = ymax - ymin;
	const Camera* camera = scene.GetCamera();

	unsigned int prevProgress = 0;
	unsigned int currProgress = 0;

	if (camera->RenderDOF()) {
		// Depth of Field (use more advanced camera model)
		// note: the focal-plane faces *toward* the camera
		// so we must displace it by a *negative* distance
		// along the viewing direction
		const PlaneSceneObject focalPlane(math::Plane(-camera->GetZDir(), -camera->GetFocalPlaneDistance()));

		const int   maxLensAperture = camera->GetLensAperture();
		const int   minLensAperture = -maxLensAperture;
		const float dofNormalizer   = float((maxLensAperture * 2.0f + 1.0f) * (maxLensAperture * 2.0f + 1.0f));

		for (unsigned int y = ymin; y < ymax; y++) {
			for (unsigned int x = 0; x < window.GetSizeX(); x++) {
				if (antiAliasing) {
					// TODO: combine AA with DOF
				} else {
					math::RaySegment pxlRay(camera->GetPos(), camera->GetPixelDir(window, x, y));
					math::RayIntersection fplaneInt;

					math::vec3f pxlIrr(math::NVECf);

					if (focalPlane.IntersectRay(pxlRay, &fplaneInt)) {
						for (int n = minLensAperture; n <= maxLensAperture; n++) {
							for (int m = minLensAperture; m <= maxLensAperture; m++) {
								const math::vec3f pxlLocation(camera->GetPixelPos(window, x + m, y + n));

								pxlRay.SetPos(pxlLocation);
								pxlRay.SetDir((fplaneInt.GetPos()).norm());
								pxlIrr += TraceRay(threadNum, pxlRay, scene, rng, 0, RAY_TYPE_PRIMARY_FP);
							}
						}
					}

					window.SetPixel(x, y, pxlIrr / dofNormalizer);
				}
			}

			if (incrementalRender) {
				window.SwapBuffers(numThreads);
			}

			currProgress = ((y - ymin) / yrange) * 100;

			if ((currProgress > prevProgress) && ((currProgress % 10) == 0)) {
				prevProgress = currProgress;

				boost::mutex::scoped_lock lock(progressMutex);

				std::cout << "[RayTracer::TraceRayThread]";
				std::cout << " thread: " << threadNum << ", progress: " << currProgress << "%";
				std::cout << " (row " << (y - ymin) << " of " << (ymax - ymin) << ")";
				std::cout << std::endl;
			}
		}
	} else {
		// use Pinhole camera
		for (unsigned int y = ymin; y < ymax; y++) {
			for (unsigned int x = 0; x < window.GetSizeX(); x++) {
				if (antiAliasing) {
					math::RaySegment pxlRay(camera->GetPos(), camera->GetPixelDir(window, x, y));
					math::vec3f pxlIrr(TraceRay(threadNum, pxlRay, scene, rng, 0, RAY_TYPE_PRIMARY));

					for (int i = -1; i <= 1; i++) {
						for (int j = -1; j <= 1; j++) {
							if (i == 0 && j == 0)
								continue;

							pxlRay.SetDir((((pxlRay.GetDir() + camera->GetPixelDir(window, x + i, y + j))) * 0.5f).norm());
							pxlIrr += TraceRay(threadNum, pxlRay, scene, rng, 0, RAY_TYPE_PRIMARY_AA);
						}
					}

					window.SetPixel(x, y, pxlIrr / 9.0f);
				} else {
					const math::RaySegment pxlRay(camera->GetPos(), camera->GetPixelDir(window, x, y));
					const math::vec3f& pxlIrr = TraceRay(threadNum, pxlRay, scene, rng, 0, RAY_TYPE_PRIMARY);

					window.SetPixel(x, y, pxlIrr);
				}
			}

			if (incrementalRender) {
				window.SwapBuffers(numThreads);
			}

			currProgress = ((y - ymin) / yrange) * 100;

			if ((currProgress > prevProgress) && ((currProgress % 10) == 0)) {
				prevProgress = currProgress;

				boost::mutex::scoped_lock lock(progressMutex);

				std::cout << "[RayTracer::TraceRayThread]";
				std::cout << " thread: " << threadNum << ", progress: " << currProgress << "%";
				std::cout << " (row " << (y - ymin) << " of " << (ymax - ymin) << ")";
				std::cout << std::endl;
			}
		}
	}
}






void RayTracer::TracePhoton(
	unsigned int threadNum,
	const Scene& scene,
	PhotonMap::Map* map,
	PhotonMap::Photon* photon,
	RNGflt64* rng,
	unsigned int photonDepth,
	bool inside
) {
	if (photonDepth < maxPhotonDepth) {
		math::RaySegment ray(photon->GetPos(), photon->GetDirection());
		math::RayIntersection rayInt;

		if (scene.GetClosestObject(threadNum, ray, &rayInt) != NULL) {
			const ISceneObject* obj    = rayInt.GetObj();
			const Material*     objMat = obj->GetMaterial();

			const float r = (*rng)();

			const math::vec3f& diffReflectiveness = objMat->GetDiffuseReflectiveness();
			const math::vec3f& specReflectiveness = objMat->GetSpecularReflectiveness();
			const math::vec3f& specRefractiveness = objMat->GetSpecularRefractiveness();
			const float diffReflectivenessAvg = (diffReflectiveness.x + diffReflectiveness.y + diffReflectiveness.z) / 3.0f;
			const float specReflectivenessAvg = (specReflectiveness.x + specReflectiveness.y + specReflectiveness.z) / 3.0f;
			const float specRefractivenessAvg = (specRefractiveness.x + specRefractiveness.y + specRefractiveness.z) / 3.0f;

			// perform Russian Roulette with the photon's fate (5.2.4)
			if ((r > 0.0f) && (r < diffReflectivenessAvg)) {
				// diffuse reflection
				profiler->IncCounter(Profiler::COUNTER_PHOTON, threadNum, photonDepth, PHOTON_MATINT_REFLECTION_DIFFUSE);

				// generate random direction on hemisphere above surface
				// (taking the surface normal orientation into account)
				// http.developer.nvidia.com/GPUGems/gpugems_ch17.html
				math::vec3f reflectDir = -rayInt.GetNrm();

				while (reflectDir.dot3D(rayInt.GetNrm()) < 0.0f) {
					reflectDir.rrandomize(rng);
				}

				#if (PHOTON_ENERGY_CONSERVATION == 1)
				const math::vec3f pwr = photon->GetPwr() * (diffReflectiveness / diffReflectivenessAvg);
				#else
				const math::vec3f pwr = photon->GetPwr() * (diffReflectiveness);
				#endif

				photon->SetPos(rayInt.GetPos());
				photon->SetDirection(ray.GetDir());
				photon->SetPwr(pwr);

				// NOTE: only store if surface is non-specular (in all three bands)
				// NOTE:
				//     specular objects give a poor estimation since RR will result
				//     in more specular reflection  where we won't store the photon.
				//     Therefore direct illumination is needed to estimate the color
				//     cq. energy of specular objects and it is not needed to store
				//     photons on these surfaces.
				if (!objMat->IsSpecularlyReflective()) {
					if (!scene.PosInBounds(photon->GetPos())) {
						return;
					}

					#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1 || USE_SPHERE_COMPRESSION == 1)
					photon->SetNrm(rayInt.GetNrm());
					#endif

					#if (PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY == 1)
					if (photonDepth > 0) {
						map->AddPhoton(photon);
					}
					#else
					map->AddPhoton(photon);
					#endif
				}

				photon->SetDirection(reflectDir.norm());
				photon->SetPos(photon->GetPos() + (photon->GetDirection() * 0.01f));

				TracePhoton(threadNum, scene, map, photon, rng, photonDepth + 1, inside);
			} else if ((r >= diffReflectivenessAvg) && (r < (diffReflectivenessAvg + specReflectivenessAvg))) {
				// specular reflection
				profiler->IncCounter(Profiler::COUNTER_PHOTON, threadNum, photonDepth, PHOTON_MATINT_REFLECTION_SPECULAR);

				#if (PHOTON_ENERGY_CONSERVATION == 1)
				const math::vec3f pwr = photon->GetPwr() * (specReflectiveness / specReflectivenessAvg);
				#else
				const math::vec3f pwr = photon->GetPwr() * (specReflectiveness);
				#endif

				photon->SetPos(rayInt.GetPos());
				photon->SetDirection((ray.GetDir()).reflect(rayInt.GetNrm()));
				photon->SetPwr(pwr);

				TracePhoton(threadNum, scene, map, photon, rng, photonDepth + 1, inside);

			} else if ((r >= (diffReflectivenessAvg + specReflectivenessAvg)) && (r < (diffReflectivenessAvg + specReflectivenessAvg + specRefractivenessAvg))) {
				// refraction
				// note: temporary, refractions should be handled as S-reflections
				profiler->IncCounter(Profiler::COUNTER_PHOTON, threadNum, photonDepth, PHOTON_MATINT_REFRACTION);

				// if going out of an object, switch refr. indices and invert normal
				const math::vec3f& N = (inside)? (-rayInt.GetNrm()): (rayInt.GetNrm());
				const float n1 = (inside)? (objMat->GetRefractionIndex()): (1.0f);
				const float n2 = (inside)? (1.0f): (objMat->GetRefractionIndex());

				const math::vec3f R = (ray.GetDir()).refract(N, n1, n2);
				const math::vec3f P = rayInt.GetPos() + R * 0.01f;

				photon->SetDirection(R);
				photon->SetPos(P);

				if (R != N) {
					TracePhoton(threadNum, scene, map, photon, rng, photonDepth + 1, !inside);
				}
			} else {
				// absorption
				profiler->IncCounter(Profiler::COUNTER_PHOTON, threadNum, photonDepth, PHOTON_MATINT_ABSORPTION);

				photon->SetPos(rayInt.GetPos());
				photon->SetDirection(ray.GetDir());

				if (!objMat->IsSpecularlyReflective()) {
					if (!scene.PosInBounds(photon->GetPos())) {
						return;
					}

					#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1 || USE_SPHERE_COMPRESSION == 1)
					photon->SetNrm(rayInt.GetNrm());
					#endif

					#if (PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY == 1)
					if (photonDepth > 0) {
						map->AddPhoton(photon);
					}
					#else
					map->AddPhoton(photon);
					#endif
				}
			}
		}
	}
}

void RayTracer::TracePhotonThread(
	unsigned int threadNum,
	boost::barrier* barrier,
	const Scene& scene,
	PhotonMap::Map* map,
	RNGflt64* rng
) {
	const std::list<ISceneLight*>& lights = scene.GetLights();

	/*
	 *  note: we distinguish absorption and three types of transmission events:
	 *      1. (diffuse) reflection
	 *     2A. (specular) reflection
	 *     2B. (specular) refraction
	 *  this is because for both S-reflection and S-refraction the direction of
	 *  the incoming photon is changed anisotropically; if an individual photon
	 *  is neither diffusely reflected nor absorbed, we must make the decision
	 *  whether to S-reflect or S-refract separately
	 */

	for (std::list<ISceneLight*>::const_iterator it = lights.begin(); it != lights.end(); it++) {
		const ISceneLight* light = *it;

		const unsigned int photonsPerThread = light->GetNumPhotons() / numThreads;
		const unsigned int photonsRemaining = light->GetNumPhotons() % photonsPerThread;

		const unsigned int numThreadPhotons = photonsPerThread + ((threadNum == (numThreads - 1))? photonsRemaining: 0);

		math::vec3f emissionPos; // surface emission-position (world-space)
		math::vec3f emissionVec; // normalized vector from pos to emissionPos
		math::vec3f emissionDir; // actual emission direction from emissionPos

		if (light->GetRadius() <= 0.0f) {
			for (unsigned int n = 0; n < numThreadPhotons; n++) {
				emissionPos = light->GetPos();
				emissionDir = emissionDir.rrandomize(rng);

				PhotonMap::Photon photon(emissionPos, emissionDir, light->GetPower());
				TracePhoton(threadNum, scene, map, &photon, rng, 0, false);
			}
		} else {
			// note: direction probability needs to be proportional to cos(angle)
			for (unsigned int n = 0; n < numThreadPhotons; n++) {
				emissionPos = light->GetPos() + (emissionPos.rrandomize(rng) * light->GetRadius());
				emissionVec = (emissionPos - light->GetPos()) / light->GetRadius();
				emissionDir = -emissionVec;

				while (emissionDir.dot3D(emissionVec) < 0.0f) {
					emissionDir.rrandomize(rng);
				}

				PhotonMap::Photon photon(emissionPos, emissionDir, light->GetPower());
				TracePhoton(threadNum, scene, map, &photon, rng, 0, false);
			}
		}

		// all threads need to be done tracing photons
		// (for this light-source) before we can scale
		barrier->wait();

		if (threadNum == 0) {
			// scale the power of all photons for this light-source
			map->ScalePhotonPower(math::UVECf * (1.0f / light->GetNumPhotons()));
		}

		// wait until first thread has scaled the photons, so that it
		// can proceed to Finalize safely after the last light-source
		barrier->wait();
	}

	if (threadNum == 0) {
		map->Finalize();
	}

	// wait until first thread has finalized the map
	barrier->wait();

	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
	map->PrecomputeIrradianceEstimates(numThreads, threadNum, photonSearchRadius, photonSearchCount);
	// wait until all irradiance values are precomputed
	barrier->wait();
	#endif
}






void RayTracer::RenderThread(
	unsigned int threadNum,
	boost::barrier* barrier,
	SDLWindow& window,
	const Scene& scene,
	RNGflt64* rng
) {
	if (photonMapping) {
		TracePhotonThread(threadNum, barrier, scene, photonMap, rng);
	}

	TraceRayThread(threadNum, window, scene, rng);
}

void RayTracer::Render(SDLWindow& window, const Scene& scene) {
	profiler->StartTask("[Render]", SDL_GetTicks());

	std::vector<boost::thread*> threads(numThreads, NULL);
	std::vector<RNGflt64*> rngs(numThreads, NULL);

	boost::barrier threadBarrier(numThreads);

	for (unsigned int threadNum = 0; threadNum < numThreads; threadNum++) {
		#define threadFunc boost::bind(&RayTracer::RenderThread, this,  threadNum, &threadBarrier, boost::ref(window), boost::cref(scene), rngs[threadNum])

		// note: each RNG must use a different seed
		rngs[threadNum] = new RNGflt64(SDL_GetTicks() % random());
		threads[threadNum] = new boost::thread(threadFunc);

		#undef threadFunc
	}

	for (unsigned int threadNum = 0; threadNum < numThreads; threadNum++) {
		threads[threadNum]->join();
		delete threads[threadNum];
		delete rngs[threadNum];
	}

	threads.clear();
	rngs.clear();

	window.NormalizeBuffers();
	profiler->StopTask("[Render]", SDL_GetTicks());
}
