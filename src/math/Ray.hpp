#ifndef KIRAN_RAY_HDR
#define KIRAN_RAY_HDR

#include "./vec3fwd.hpp"
#include "./vec3.hpp"

struct ISceneObject;

namespace math {
	// note: represents a semi-infinite segment
	struct RaySegment {
	public:
		RaySegment(const math::vec3f& p, const math::vec3f& d, bool b = false): pos(p), dir(d), ins(b) {
		}

		const math::vec3f& GetPos() const { return pos; }
		const math::vec3f& GetDir() const { return dir; }
		bool IsInside() const { return ins; }

		void SetPos(const math::vec3f& p) { pos = p; }
		void SetDir(const math::vec3f& d) { dir = d; }

	private:
		math::vec3f pos; // start-position
		math::vec3f dir; // segment direction

		bool ins;        // true if ray origin is inside an object
	};

	struct RayIntersection {
	public:
		RayIntersection(): p(math::NVECf), n(math::NVECf), o(0) {
		}

		const math::vec3f& GetPos() const { return p; }
		const math::vec3f& GetNrm() const { return n; }
		const ISceneObject* GetObj() const { return o; }

		void SetPos(const math::vec3f& pos) { p = pos; }
		void SetNrm(const math::vec3f& nor) { n = nor; }
		void SetObj(const ISceneObject* obj) { o = obj; }

		void SetDistance(float d) { distance = d; }
		float GetDistance() const { return distance; }
	private:
		math::vec3f p; // position of intersection
		math::vec3f n; // surface normal at intersection
		float distance;
		const ISceneObject* o;
	};
};

#endif
