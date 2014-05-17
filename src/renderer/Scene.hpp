#ifndef KIRAN_SCENE_HDR
#define KIRAN_SCENE_HDR

#include <string>
#include <map>
#include <list>

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

namespace math {
	struct RaySegment;
	struct RayIntersection;
}

struct LuaParser;
struct LuaTable;
struct Camera;

class Material;
class ISceneLight;
class ISceneObject;

template<typename T> class UniformGrid;

class Scene {
public:
	Scene(LuaParser&, Camera*);
	~Scene();

	const std::list<ISceneLight*> GetLights() const { return lights; }
	const std::list<ISceneObject*> GetObjects() const { return objects; }

	const ISceneLight* GetClosestLight(const math::RaySegment&) const;
	const ISceneObject* GetOccludingObject(unsigned int, const math::RaySegment&, math::RayIntersection*, const math::vec3f&) const;
	const ISceneObject* GetClosestObject(unsigned int, const math::RaySegment&, math::RayIntersection*) const;
	const ISceneObject* StepRayThroughGrid(unsigned int, const math::RaySegment&, math::RayIntersection*, float) const;

	Camera* GetCamera() const { return camera; }

	const math::vec3f& GetMinBounds() const { return minBounds; }
	const math::vec3f& GetMaxBounds() const { return maxBounds; }
	bool PosInBounds(const math::vec3f& pos) const {
		if (pos.x < minBounds.x || pos.y < minBounds.y || pos.z < minBounds.z) { return false; }
		if (pos.x > maxBounds.x || pos.y > maxBounds.y || pos.z > maxBounds.z) { return false; }
		return true;
	}

	void SetNumThreads(unsigned int n) { numThreads = n; }

private:
	void AddMaterial(const LuaTable*);
	void AddLight(const LuaTable*);
	void AddObject(const LuaTable*);
	void AddObjectsToGrid();

	std::map<std::string, Material*> materials;
	std::list<ISceneLight*> lights;
	std::list<ISceneObject*> objects;

	math::vec3i objectGridCellCount;
	UniformGrid<const ISceneObject*>* objectGrid;

	Camera* camera;

	math::vec3f minBounds;
	math::vec3f maxBounds;

	// set to the value of RayTracer::numThreads
	unsigned int numThreads;

	enum {
		RAY_INTERSECTION_UNTESTED    = -1,
		RAY_INTERSECTION_TESTED_NONE = -2,
	};
};

#endif
