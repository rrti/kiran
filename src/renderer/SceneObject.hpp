#ifndef KIRAN_SCENEOBJECT_HDR
#define KIRAN_SCENEOBJECT_HDR

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"
#include "../math/Ray.hpp"
#include "../math/Plane.hpp"

class Material;

struct ISceneObject {
public:
	ISceneObject(): pos(math::NVECf), objID(-1), material(0) {}
	ISceneObject(const math::vec3f& p): pos(p), objID(-1), material(0) {}

	// must return intersection point and surface normal
	// to be able to spawn reflection and refraction rays
	// (we re-use the RaySegment structure to store these)
	virtual bool IntersectRay(const math::RaySegment&, math::RayIntersection*) const { return false; }
	virtual bool IntersectCell(const math::vec3f&, const math::vec3f&) const { return false; }

	virtual void CalculateBoundingSphere() { bsRadius = 0.0f; }
	virtual void CalculateBoundingBox() { bbSize = math::NVECf; }

	const math::vec3f& GetPos() const { return pos; }

	int GetID() const { return objID; }
	void SetID(int id) { objID = id; }

	void SetMaterial(const Material* m) { material = m; }
	const Material* GetMaterial() const { return material; }

protected:
	math::vec3f pos;        // object center-position (world-space)

	float bsRadius;         // radius of bounding sphere
	float bsRadiusSq;       // squared radius of bounding sphere
	math::vec3f bbSize;     // {x, y, z}-size of AA bounding box

	int objID;

	const Material* material;
};



struct PlaneSceneObject: public ISceneObject {
public:
	PlaneSceneObject(const math::Plane& p): ISceneObject(math::NVECf), sur(p) {
	}

	bool IntersectRay(const math::RaySegment&, math::RayIntersection*) const;
	bool IntersectCell(const math::vec3f&, const math::vec3f&) const;

private:
	math::Plane sur;
};

struct EllipseSceneObject: public ISceneObject {
public:
	EllipseSceneObject(const math::vec3f& p, const math::vec3f& s): ISceneObject(p), size(s) {
		CalculateBoundingSphere();
		CalculateBoundingBox();

		spherical =
			(((size.x / size.y) == 1.0f) &&
			 ((size.y / size.z) == 1.0f));
	}

	bool IntersectRay(const math::RaySegment&, math::RayIntersection*) const;
	bool IntersectCell(const math::vec3f&, const math::vec3f&) const;

	void CalculateBoundingSphere() {
		bsRadius = std::max(size.x, std::max(size.y, size.z));
	}
	void CalculateBoundingBox() {
		bbSize.x = size.x * 2.0f;
		bbSize.y = size.y * 2.0f;
		bbSize.z = size.z * 2.0f;
	}

private:
	math::vec3f size;
	bool spherical;
};


struct BoxSceneObject: public ISceneObject {
public:
	BoxSceneObject(const math::vec3f& p, const math::vec3f& s): ISceneObject(p), size(s) {
		CalculateBoundingSphere();
		CalculateBoundingBox();
	}

	bool IntersectRay(const math::RaySegment&, math::RayIntersection*) const;
	bool IntersectCell(const math::vec3f&, const math::vec3f&) const;

	void CalculateBoundingSphere() {
		bsRadiusSq =
			((size.x * 0.5f) * (size.x * 0.5f)) +
			((size.y * 0.5f) * (size.y * 0.5f)) +
			((size.z * 0.5f) * (size.z * 0.5f));
		bsRadius = sqrtf(bsRadiusSq);
	}
	void CalculateBoundingBox() {
		bbSize = size;
	}

private:
	math::vec3f size;
};


struct CylinderSceneObject: public ISceneObject {
public:
	CylinderSceneObject(const math::vec3f& p, const math::vec3f& s, math::COOR_AXIS a): ISceneObject(p), size(s), axis(a) {
		CalculateBoundingSphere();
		CalculateBoundingBox();
	}

	bool IntersectRay(const math::RaySegment&, math::RayIntersection*) const;
	bool IntersectCell(const math::vec3f&, const math::vec3f&) const;

	void CalculateBoundingSphere() {
		bsRadiusSq =
			((size.x * 0.5f) *
			 (size.x * 0.5f)) +
			(std::max(size.y, size.z) *
			 std::max(size.y, size.z));
		bsRadius = sqrtf(bsRadiusSq);
	}
	void CalculateBoundingBox() {
		switch (axis) {
			case math::COOR_AXIS_X: {
				bbSize.x = size.x;
				bbSize.y = size.y * 2.0f;
				bbSize.z = size.z * 2.0f;
			} break;
			case math::COOR_AXIS_Y: {
				bbSize.x = size.y * 2.0f;
				bbSize.y = size.x;
				bbSize.z = size.z * 2.0f;
			} break;
			case math::COOR_AXIS_Z: {
				bbSize.x = size.y * 2.0f;
				bbSize.y = size.z * 2.0f;
				bbSize.z = size.x;
			} break;
		}
	}

private:
	math::vec3f size;
	math::COOR_AXIS axis;
};

#endif
