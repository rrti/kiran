#include "../system/Defines.hpp"

#include "./MaterialReflectionModel.hpp"
#include "./Material.hpp"
#include "./SceneLight.hpp"

math::vec3f PhongMaterialReflectionModel::GetIntensity(
	const Material* mat,
	const ISceneLight* light,
	const math::RaySegment& viewRay,
	const math::RaySegment& lightRay,
	const math::RayIntersection* rayInt) const
{
	math::vec3f ill;
	math::vec3f irr;

	const math::vec3f& V = viewRay.GetDir();  // view-ray direction
	const math::vec3f& D = light->GetDir();   // emission direction
	const math::vec3f& L = lightRay.GetDir(); // point-to-light direction
	const math::vec3f& N = rayInt->GetNrm();  // surface normal direction
	      math::vec3f  R;                     // specular-reflection direction

	const float fov = light->GetFOV();

	if (RAD2DEG(acosf(D.dot3D(-L))) < (fov * 0.5f)) {
		irr = light->GetScaledPower() * std::min(1.0f, std::max(0.0f, N.dot3D(L)));
		irr *= (1.0f / (rayInt->GetPos() - light->GetPos()).sqLen3D());
		ill += (mat->GetDiffuseReflectiveness() * irr);
		R = L.reflect(N);
	}

	if (mat->GetSpecularExponent() > 0.0f) {
		if (V.dot3D(R) > 0.0f) {
			irr = light->GetScaledPower() * std::pow(V.dot3D(R), mat->GetSpecularExponent());
			irr *= (1.0f / (rayInt->GetPos() - light->GetPos()).sqLen3D());
			ill += (mat->GetSpecularReflectiveness() * irr);
		}
	}

	return ill;
}
