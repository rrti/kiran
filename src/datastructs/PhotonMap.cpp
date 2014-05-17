#include <boost/thread/mutex.hpp>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iostream>

// include first, so the preprocessor
// substitutes in subsequent headers
#include "../system/Defines.hpp"

#include "./PhotonMap.hpp"
#include "./KDTree.hpp"
#include "./UniformGrid.hpp"
#include "./SortedList.hpp"

PhotonMap::Map::Map(unsigned int maxPhotons, PhotonMapType mapType): type(mapType), finalized(false) {
	photonArray.reserve(maxPhotons + 1);
	photonArray.push_back(Photon()); // dummy

	#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
	photonTree = new KDTree<PhotonMap::Photon*>(maxPhotons);
	photonTree->SetNode(0, &photonArray[0]);
	#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
	photonGridCellCount = math::UVECi * powf(maxPhotons, 0.333333f);
	photonGrid = new UniformGrid<const PhotonMap::Photon*>(photonGridCellCount);
	#endif

	minPhotonPower = math::vec3f( FLT_MAX,  FLT_MAX,  FLT_MAX);
	maxPhotonPower = math::vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	numPhotons = 0;
	lastScaledPhoton = 0;

	Photon::InitDirectionTables();

	std::cout << "[PhotonMap::Map::Map]" << std::endl;
	std::cout << "\tPM_DATASTRUCT:                   " << PM_DATASTRUCT                   << std::endl;
	std::cout << "\tUSE_SPHERE_COMPRESSION:          " << USE_SPHERE_COMPRESSION          << std::endl;
	std::cout << "\tSPHERE_COMPRESSION_RATIO:        " << SPHERE_COMPRESSION_RATIO        << std::endl;
	std::cout << "\tPRECOMPUTE_IRRADIANCE_ESTIMATES: " << PRECOMPUTE_IRRADIANCE_ESTIMATES << std::endl;
	std::cout << "\tUSE_FURTHEST_PHOTON_DIST:        " << USE_FURTHEST_PHOTON_DIST        << std::endl;
	std::cout << "\tFILTER_RADIANCE_ESTIMATE:        " << FILTER_RADIANCE_ESTIMATE        << std::endl;
	std::cout << "\tFILTER_CONSTANT:                 " << FILTER_CONSTANT                 << std::endl;
	std::cout << "\tFILTER_NORMALIZER:               " << FILTER_NORMALIZER               << std::endl;
	std::cout << std::endl;
	std::cout << "\tmaxPhotons: " << maxPhotons     << std::endl;
}

PhotonMap::Map::~Map() {
	photonArray.clear();

	#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
	delete photonTree;
	#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
	delete photonGrid;
	#endif
}

void PhotonMap::Map::Finalize() {
	assert(!finalized);

	if (numPhotons > 0) {
		#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
		photonTree->Balance(photonArray.size() == photonArray.capacity());
		#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
		for (unsigned int i = 1; i <= numPhotons; i++) {
			photonGrid->AddNode(&photonArray[i]);
		}
		#endif

		avgPhotonPower /= numPhotons;
	}

	finalized = true;

	std::cout << "[PhotonMap::Map::Finalize]" << std::endl;
	std::cout << "\tstored photons: " << numPhotons           << std::endl;
	std::cout << "\tminPhotonPower: " << minPhotonPower.str() << std::endl;
	std::cout << "\tmaxPhotonPower: " << maxPhotonPower.str() << std::endl;
	std::cout << "\tavgPhotonPower: " << avgPhotonPower.str() << std::endl;
}

#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
void PhotonMap::Map::PrecomputeIrradianceEstimates(
	unsigned int numThreads,
	unsigned int threadNum,
	float searchRadius,
	unsigned int searchCount
) {
	assert(finalized);

	if (numPhotons > 0) {
		static boost::mutex progressMutex;

		const unsigned int photonsPerThread = numPhotons / numThreads;
		const unsigned int photonsRemaining = numPhotons % photonsPerThread;

		const unsigned int photonIdxL = (photonsPerThread * threadNum) + 1;
		const unsigned int photonIdxR =
			(photonsPerThread * (threadNum + 1)) +
			((threadNum == (numThreads - 1))? photonsRemaining: 0);
		const float photonRange = photonIdxR - photonIdxL;

		unsigned int prevProgress = 0;
		unsigned int currProgress = 0;

		for (unsigned int photonIdx = photonIdxL; photonIdx <= photonIdxR; photonIdx++) {
			Photon* p = &photonArray[photonIdx];
				p->SetIrr(GetIrradianceEstimate(p->GetPos(), p->GetNrm(), searchRadius, searchCount, true));

			currProgress = ((photonIdx - photonIdxL) / photonRange) * 100;

			if ((currProgress > prevProgress) && ((currProgress % 10) == 0)) {
				prevProgress = currProgress;

				boost::mutex::scoped_lock lock(progressMutex);

				std::cout << "[PhotonMap::Map::PrecomputeIrradiance]";
				std::cout << " thread: " << threadNum << ", progress: " << currProgress << "%";
				std::cout << " (photon " << (photonIdx - photonIdxL) << " of " << (photonIdxR - photonIdxL + 1) << ")";
				std::cout << std::endl;
			}
		}
	}
}
#endif

bool PhotonMap::Map::AddPhoton(PhotonMap::Photon* p) {
	static boost::mutex addPhotonMutex;

	boost::mutex::scoped_lock lock(addPhotonMutex);

	if (photonArray.size() >= photonArray.capacity()) {
		return false;
	}

	photonArray.push_back(*p);

	#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
	photonTree->SetNode(photonArray.size() - 1, &photonArray[photonArray.size() - 1]);
	photonTree->SetMins(p);
	photonTree->SetMaxs(p);
	#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
	photonGrid->SetMins(p);
	photonGrid->SetMaxs(p);
	#endif

	minPhotonPower.x = std::min(minPhotonPower.x, (p->GetPwr()).x);
	minPhotonPower.y = std::min(minPhotonPower.y, (p->GetPwr()).y);
	minPhotonPower.z = std::min(minPhotonPower.z, (p->GetPwr()).z);
	maxPhotonPower.x = std::max(maxPhotonPower.x, (p->GetPwr()).x);
	maxPhotonPower.y = std::max(maxPhotonPower.y, (p->GetPwr()).y);
	maxPhotonPower.z = std::max(maxPhotonPower.z, (p->GetPwr()).z);
	avgPhotonPower += p->GetPwr();

	numPhotons = photonArray.size() - 1;
	return true;
}

void PhotonMap::Map::ScalePhotonPower(const math::vec3f& scale) {
	// only executed by one thread
	for (++lastScaledPhoton; lastScaledPhoton <= numPhotons; lastScaledPhoton++) {
		photonArray[lastScaledPhoton].SetPwr(photonArray[lastScaledPhoton].GetPwr() * scale);
	}

	lastScaledPhoton--;
}






#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
math::vec3f PhotonMap::Map::GetIrradianceEstimateTree(
	const math::vec3f& searchPos, const math::vec3f& searchNrm,
	float searchRadius, unsigned int searchCount,
	bool precompute
) const {
	math::vec3f irr;

	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 0)
	assert(!precompute);
	#else
	if (precompute)
	#endif
	{
		// assemble volume query structure with
		// the <count> photons nearest to <p>
		NodeVolumeQuery<PhotonMap::Photon*> q(searchCount, searchPos, searchNrm, searchRadius);
		photonTree->GetNodes(&q, 1);

		for (unsigned int i = 1; i <= q.GetNumNodes(); i++) {
			const Photon* photon = q.GetNode(i);

			// we are only dealing with Lambertian surfaces,
			// so we can replace the BRDF evaluation with a
			// dot-product to exclude photons that impacted
			// the back-side of a surface
			if ((photon->GetDirection()).dot3D(searchNrm) < 0.0f) {
				#if (FILTER_RADIANCE_ESTIMATE == 1)
				const float dst = (photon->GetPos() - searchPos).sqLen3D();
				const float wgt = 1.0f - (sqrtf(dst) / (FILTER_CONSTANT * searchRadius));
				irr += (photon->GetPwr() * wgt);
				#else
				irr += (photon->GetPwr());
				#endif
			}
		}

		if (q.GetNumNodes() > 1) {
			#if (USE_FURTHEST_PHOTON_DIST == 1)
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * q.GetMaxNodeDist() * FILTER_NORMALIZER)));
			#else
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * searchRadius * searchRadius * FILTER_NORMALIZER)));
			#endif
		}
	}
	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
	else {
		// get the single nearest photon
		NodeVolumeQuery<PhotonMap::Photon*> q(1, searchPos, searchNrm, searchRadius);
		photonTree->GetNodes(&q, 1);

		assert(q.GetNumNodes() <= 1);

		if (q.GetNumNodes() == 1) {
			irr = (q.GetNode(1))->GetIrr();
		}
	}
	#endif

	return irr;
}
#endif



#if (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
math::vec3f PhotonMap::Map::GetIrradianceEstimateGrid(
	const math::vec3f& searchPos, const math::vec3f& searchNrm,
	float searchRadius, unsigned int searchCount,
	bool precompute
) const {
	math::vec3f irr;

	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 0)
	assert(!precompute);
	#else
	if (precompute)
	#endif
	{
		#if (USE_SPHERE_COMPRESSION == 1)
		/*
		// FIXME: define rotations for sphere compression
		const float xyLength = sqrtf(nrm.x * nrm.x + nrm.y * nrm.y);
		const float vecLength = nrm.len3D();
		const float zAngleDeg = RAD2DEG(acosf(nrm.y / xyLength));
		const float xAngleDeg = RAD2DEG(acosf(nrm.y / vecLength));
		const float rad = searchRadius * searchRadius;
		*/
		#endif

		NodeVolumeQuery<const PhotonMap::Photon*> q(searchCount, searchPos, searchNrm, searchRadius);
		photonGrid->GetNodes(&q);

		for (unsigned int i = 1; i <= q.GetNumNodes(); i++) {
			const Photon* photon = q.GetNode(i);

			if ((photon->GetDirection()).dot3D(searchNrm) < 0.0f) {
				#if (FILTER_RADIANCE_ESTIMATE == 1)
				const float dst = (photon->GetPos() - searchPos).sqLen3D();
				const float wgt = 1.0f - (sqrtf(dst) / (FILTER_CONSTANT * searchRadius));
				irr += (photon->GetPwr() * wgt);
				#else
				irr += (photon->GetPwr());
				#endif
			}
		}

		if (q.GetNumNodes() > 1) {
			#if (USE_FURTHEST_PHOTON_DIST == 1)
			// use the distance of the furthest photon
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * q.GetMaxNodeDist() * FILTER_NORMALIZER)));
			#else
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * searchRadius * searchRadius * FILTER_NORMALIZER)));
			#endif
		}
	}
	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
	else {
		NodeVolumeQuery<const PhotonMap::Photon*> q(1, searchPos, searchNrm, searchRadius);
		photonGrid->GetNodes(&q);

		assert(q.GetNumNodes() <= 1);

		if (q.GetNumNodes() == 1) {
			irr = (q.GetNode(1))->GetIrr();
		}
	}
	#endif

	return irr;
}
#endif



#if (PM_DATASTRUCT == PM_DATASTRUCT_FLAT)
math::vec3f PhotonMap::Map::GetIrradianceEstimateFlat(
	const math::vec3f& searchPos, const math::vec3f& searchNrm,
	float searchRadius, unsigned int searchCount,
	bool precompute
) const {
	math::vec3f irr;

	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 0)
	assert(!precompute);
	#else
	if (precompute)
	#endif
	{
		NodeVolumeQuery<const PhotonMap::Photon*> q(searchCount, searchPos, searchNrm, searchRadius);

		// examine all photons (unavoidable without spatial partitioning)
		for (unsigned int i = 1; i <= numPhotons; i++) {
			q.AddNode(&photonArray[i]);
		}

		for (unsigned int i = 1; i <= q.GetNumNodes(); i++) {
			const Photon* photon = q.GetNode(i);

			if ((photon->GetDirection()).dot3D(searchNrm) < 0.0f) {
				#if (FILTER_RADIANCE_ESTIMATE == 1)
				const float dst = (photon->GetPos() - searchPos).sqLen3D();
				const float wgt = 1.0f - (sqrtf(dst) / (FILTER_CONSTANT * searchRadius));
				irr += (photon->GetPwr() * wgt);
				#else
				irr += (photon->GetPwr());
				#endif
			}
		}

		if (q.GetNumNodes() > 1) {
			#if (USE_FURTHEST_PHOTON_DIST == 1)
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * q.GetMaxNodeDist() * FILTER_NORMALIZER)));
			#else
			irr *= (math::vec3f(1.0f, 1.0f, 1.0f) * (1.0f / (M_PI * searchRadius * searchRadius * FILTER_NORMALIZER)));
			#endif
		}
	}
	#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
	else {
		NodeVolumeQuery<const PhotonMap::Photon*> q(1, searchPos, searchNrm, searchRadius);

		for (unsigned int i = 1; i <= numPhotons; i++) {
			q.AddNode(&photonArray[i]);
		}

		assert(q.GetNumNodes() <= 1);

		if (q.GetNumNodes() == 1) {
			irr = (q.GetNode(1))->GetIrr();
		}
	}
	#endif

	return irr;
}
#endif



// return (an estimate of) the irradiance at
// position <pos> with surface normal <nrm>
// in a sphere of radius <rad>
math::vec3f PhotonMap::Map::GetIrradianceEstimate(
	const math::vec3f& searchPos, const math::vec3f& searchNrm,
	float searchRadius, unsigned int searchCount,
	bool precompute
) const {
	if (numPhotons == 0) {
		return math::NVECf;
	}

	assert(searchRadius > 0.0f && searchCount > 0);

	#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
	return (GetIrradianceEstimateTree(searchPos, searchNrm, searchRadius, searchCount, precompute));
	#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
	return (GetIrradianceEstimateGrid(searchPos, searchNrm, searchRadius, searchCount, precompute));
	#elif (PM_DATASTRUCT == PM_DATASTRUCT_FLAT)
	return (GetIrradianceEstimateFlat(searchPos, searchNrm, searchRadius, searchCount, precompute));
	#else
	return math::NVECf;
	#endif
}






float PhotonMap::Photon::costheta[Photon::NUM_DIRECTIONS] = {0.0f};
float PhotonMap::Photon::sintheta[Photon::NUM_DIRECTIONS] = {0.0f};
float PhotonMap::Photon::cosphi[Photon::NUM_DIRECTIONS] = {0.0f};
float PhotonMap::Photon::sinphi[Photon::NUM_DIRECTIONS] = {0.0f};

void PhotonMap::Photon::InitDirectionTables() {
	static bool initialized = false;

	if (!initialized) {
		initialized = true;

		for (int i = 0; i < Photon::NUM_DIRECTIONS; i++) {
			const float angle = float(i) * (1.0f / Photon::NUM_DIRECTIONS) * M_PI;

			costheta[i] = cosf(angle);
			sintheta[i] = sinf(angle);
			cosphi[i] = cosf(2.0f * angle);
			sinphi[i] = sinf(2.0f * angle);
		}
	}
}

void PhotonMap::Photon::SetDirection(const math::vec3f& dir) {
	// convert the Euclidean direction vector to spherical coordinates
	const int ntheta = std::min(int(acosf(dir.z) * (256.0f / M_PI)), 255);
	const int nphi = std::min(int(atan2(dir.y, dir.x) * (128.0f / M_PI)), 255);

	theta = ubyte8(ntheta);
	phi = (nphi < 0)? ubyte8(nphi + 256): ubyte8(nphi);
}

math::vec3f PhotonMap::Photon::GetDirection() const {
	return math::vec3f(
		sintheta[theta] * cosphi[phi],
		sintheta[theta] * sinphi[phi],
		costheta[theta]
	);
}
