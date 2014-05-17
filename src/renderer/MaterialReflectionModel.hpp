#ifndef KIRAN_MATERIALREFLECTIONMODEL_HDR
#define KIRAN_MATERIALREFLECTIONMODEL_HDR

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"
#include "../math/Ray.hpp"

struct Material;
struct ISceneLight;

struct IMaterialReflectionModel {
public:
	virtual math::vec3f GetIntensity(
		const Material*,
		const ISceneLight*,
		const math::RaySegment&,
		const math::RaySegment&,
		const math::RayIntersection*
	) const = 0;
};


struct PhongMaterialReflectionModel: public IMaterialReflectionModel {
public:
	math::vec3f GetIntensity(
		const Material*,
		const ISceneLight*,
		const math::RaySegment&,
		const math::RaySegment&,
		const math::RayIntersection*
	) const;
};

#endif
