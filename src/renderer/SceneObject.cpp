#include <algorithm>
#include <cfloat>

#include "./SceneObject.hpp"

bool PlaneSceneObject::IntersectRay(const math::RaySegment& ray, math::RayIntersection* rayInt) const {
	if (sur.PointDistance(ray.GetPos()) < 0.0f) {
		// ray origin is behind the plane; behind is defined by
		// the negated plane normal (ie. the plane's back-face)
		// we disregard this case for logical consistency
		return false;
	}

	// if d is 0, ray is parallel to plane and cannot intersect
	const float n = -(ray.GetPos()).dot3D(sur.GetNormal()) + sur.GetDistance();
	const float d =  (ray.GetDir()).dot3D(sur.GetNormal());
	const float t =  (d != 0.0f)? (n / d): -1.0f;

	if (t > 0.0f) {
		rayInt->SetPos(ray.GetPos() + (ray.GetDir() * t));
		rayInt->SetNrm(sur.GetNormal());
		return true;
	}

	return false;
}

bool PlaneSceneObject::IntersectCell(const math::vec3f& cellPos, const math::vec3f& cellDim) const {
	static const math::vec3f cornerOffsets[8] = {
		math::vec3f( 0.5,  0.5f,  0.5f),
		math::vec3f( 0.5,  0.5f, -0.5f),
		math::vec3f( 0.5, -0.5f,  0.5f),
		math::vec3f( 0.5, -0.5f, -0.5f),
		math::vec3f(-0.5,  0.5f,  0.5f),
		math::vec3f(-0.5,  0.5f, -0.5f),
		math::vec3f(-0.5, -0.5f,  0.5f),
		math::vec3f(-0.5, -0.5f, -0.5f),
	};

	float r = 0.0f;
	float d = 0.0f;

	#define OPPOSITE_SIGN(d, r) (d > 0.0f && r <= 0.0f) || (d >= 0.0f && r < 0.0f) || (d < 0.0f && r >= 0.0f) || (d <= 0.0f && r > 0.0f)
	// #define OPPOSITE_SIGN(d, r) ((d * r) < 0.0f)

	// there is an intersection unless all cell corners
	// lie on one side of the planar surface (ie. share
	// the same sign)
	r = sur.PointDistance(cellPos + (cornerOffsets[0] * cellDim));
	d = sur.PointDistance(cellPos + (cornerOffsets[1] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[2] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[3] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[4] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[5] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[6] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }
	d = sur.PointDistance(cellPos + (cornerOffsets[7] * cellDim)); if (OPPOSITE_SIGN(d, r)) { return true; }

	#undef OPPOSITE_SIGN
	return false;
}



bool EllipseSceneObject::IntersectRay(const math::RaySegment& ray, math::RayIntersection* rayInt) const {
	const math::vec3f  rayPos = ray.GetPos() - pos;
	const math::vec3f& rayDir = ray.GetDir();

	const float aa = size.x * size.x;
	const float bb = size.y * size.y;
	const float cc = size.z * size.z;

	const float A =
		((rayDir.x * rayDir.x) / aa) +
		((rayDir.y * rayDir.y) / bb) +
		((rayDir.z * rayDir.z) / cc);
	const float B =
		(2.0f * rayPos.x * rayDir.x) / aa +
		(2.0f * rayPos.y * rayDir.y) / bb +
		(2.0f * rayPos.z * rayDir.z) / cc;
	const float C =
		((rayPos.x * rayPos.x) / aa) +
		((rayPos.y * rayPos.y) / bb) +
		((rayPos.z * rayPos.z) / cc) -
		1.0f;
	const float D = (B * B) - (4.0f * A * C);

	if (D < 0.0f) {
		return false;
	} else {
		const float Dr = sqrtf(D);
		const float t0 = (-B - Dr) / (2.0f * A);
		const float t1 = (-B + Dr) / (2.0f * A);

		if (t0 > 0.0f) {
			const math::vec3f intPos(rayPos + (rayDir * t0));
			const math::vec3f intNor((2.0f * (intPos.x / aa)), (2.0f * (intPos.y / bb)), (2.0f * (intPos.z / cc)));

			rayInt->SetPos(intPos + pos);
			rayInt->SetNrm(intNor.norm());
			rayInt->SetDistance(t0);
			return true;
		}
		if (t1 > 0.0f) {
			const math::vec3f intPos(rayPos + (rayDir * t1));
			const math::vec3f intNor((2.0f * (intPos.x / aa)), (2.0f * (intPos.y / bb)), (2.0f * (intPos.z / cc)));

			rayInt->SetPos(intPos + pos);
			rayInt->SetNrm(intNor.norm());
			rayInt->SetDistance(t1);
			return true;
		}
	}

	return false;
}

bool EllipseSceneObject::IntersectCell(const math::vec3f& cellPos, const math::vec3f& cellDim) const {
	const math::vec3f dCellPos = cellPos - pos;
	const math::vec3f hCellDim = cellDim * 0.5f;

	// note: this is conservative (the sphere's distance to one of the
	// cell corners can be smaller than bsRadius without intersecting)
	if (dCellPos.x > (hCellDim.x + bsRadius) || dCellPos.x < -(hCellDim.x + bsRadius)) { return false; }
	if (dCellPos.y > (hCellDim.y + bsRadius) || dCellPos.y < -(hCellDim.y + bsRadius)) { return false; }
	if (dCellPos.z > (hCellDim.z + bsRadius) || dCellPos.z < -(hCellDim.z + bsRadius)) { return false; }

	return true;
}



bool BoxSceneObject::IntersectRay(const math::RaySegment& ray, math::RayIntersection* rayInt) const {
	EllipseSceneObject obj(pos, math::vec3f(bsRadius, bsRadius, bsRadius));

	if (!obj.IntersectRay(ray, rayInt)) {
		return false;
	}

	const math::vec3f  rpos = ray.GetPos() - pos;
	const math::vec3f& rdir = ray.GetDir();

	float tn = -FLT_MAX;
	float tf =  FLT_MAX;
	float t0 = 0.0f;
	float t1 = 0.0f;
	float t2 = 0.0f;

	#define INTERSECTION_EPS 0.001f
	#define SET_INTERSECTION_NORMAL(p, n)                                       \
		     if (p.x >= (( size.x * 0.5f) - INTERSECTION_EPS)) { n.x =  1.0f; } \
		else if (p.x <= ((-size.x * 0.5f) + INTERSECTION_EPS)) { n.x = -1.0f; } \
		else if (p.y >= (( size.y * 0.5f) - INTERSECTION_EPS)) { n.y =  1.0f; } \
		else if (p.y <= ((-size.y * 0.5f) + INTERSECTION_EPS)) { n.y = -1.0f; } \
		else if (p.z >= (( size.z * 0.5f) - INTERSECTION_EPS)) { n.z =  1.0f; } \
		else if (p.z <= ((-size.z * 0.5f) + INTERSECTION_EPS)) { n.z = -1.0f; }

	#define CHECK_INTERSECTION(p)                                               \
		if (rdir.p > -INTERSECTION_EPS && rdir.p < INTERSECTION_EPS) {          \
			if ((rpos.p < (-size.p * 0.5f) || rpos.p > (size.p * 0.5f))) {      \
				return false;                                                   \
			}                                                                   \
		} else {                                                                \
			if (rdir.p > 0.0f) {                                                \
				t0 = ((-size.p * 0.5f) - rpos.p) / rdir.p;                      \
				t1 = (( size.p * 0.5f) - rpos.p) / rdir.p;                      \
			} else {                                                            \
				t1 = ((-size.p * 0.5f) - rpos.p) / rdir.p;                      \
				t0 = (( size.p * 0.5f) - rpos.p) / rdir.p;                      \
			}                                                                   \
                                                                                \
			if (t0 > t1) { t2 = t1; t1 = t0; t0 = t2; }                         \
			if (t0 > tn) { tn = t0; }                                           \
			if (t1 < tf) { tf = t1; }                                           \
			if (tn > tf) { return false; }                                      \
			if (tf < 0.0f) { return false; }                                    \
		}

	CHECK_INTERSECTION(x);
	CHECK_INTERSECTION(y);
	CHECK_INTERSECTION(z);

	if (tn > 0.0f) {
		math::vec3f intPnt = rpos + (rdir * tn);
		math::vec3f intNor = math::NVECf;

		SET_INTERSECTION_NORMAL(intPnt, intNor);

		rayInt->SetPos(pos + intPnt);
		rayInt->SetNrm(intNor);
		return true;
	}
	if (tf > 0.0f) {
		math::vec3f intPnt = rpos + (rdir * tf);
		math::vec3f intNor = math::NVECf;

		SET_INTERSECTION_NORMAL(intPnt, intNor);

		rayInt->SetPos(pos + intPnt);
		rayInt->SetNrm(intNor);
		return true;
	}

	#undef SET_INTERSECTION_NORMAL
	#undef CHECK_INTERSECTION
	#undef INTERSECTION_EPS

	return false;
}

bool BoxSceneObject::IntersectCell(const math::vec3f& cellPos, const math::vec3f& cellDim) const {
	const math::vec3f dCellPos = cellPos - pos;
	const math::vec3f hCellDim = cellDim * 0.5f;

	if (dCellPos.x > (hCellDim.x + bsRadius) || dCellPos.x < -(hCellDim.x + bsRadius)) { return false; }
	if (dCellPos.y > (hCellDim.y + bsRadius) || dCellPos.y < -(hCellDim.y + bsRadius)) { return false; }
	if (dCellPos.z > (hCellDim.z + bsRadius) || dCellPos.z < -(hCellDim.z + bsRadius)) { return false; }

	return true;
}



bool CylinderSceneObject::IntersectRay(const math::RaySegment& ray, math::RayIntersection* rayInt) const {
	EllipseSceneObject obj(pos, math::vec3f(bsRadius, bsRadius, bsRadius));

	if (!obj.IntersectRay(ray, rayInt)) {
		return false;
	}

	// size.xyz stores height, a-radius, b-radius
	const float aa = size.y * size.y;
	const float bb = size.z * size.z;

	float A = 0.0f, B = 0.0f, C = 0.0f, D = 0.0f;

	// surface intersection, cap intersection
	bool intersectSur = false;
	bool intersectCap = false;
	bool useSurNormal = false;

	// transform the ray to (cylinder) object-space
	const math::vec3f  rayPos = ray.GetPos() - pos;
	const math::vec3f& rayDir = ray.GetDir();

	// surface normal, end-cap normal
	math::vec3f surNor, capNor;
	math::RayIntersection capInt;


	#define SET_CYLINDER_SURFACE_EQUATION_PARAMS(p, q) \
		A =                                            \
			((rayDir.p * rayDir.p) / aa) +             \
			((rayDir.q * rayDir.q) / bb);              \
		B =                                            \
			(2.0f * rayPos.p * rayDir.p) / aa +        \
			(2.0f * rayPos.q * rayDir.q) / bb;         \
		C =                                            \
			((rayPos.p * rayPos.p) / aa) +             \
			((rayPos.q * rayPos.q) / bb) -             \
			1.0f;                                      \
		D = (B * B) - (4.0f * A * C);

	#define SET_CYLINDER_SURFACE_NORMAL_PARAMS(p, q)  \
		surNor.p = (2.0f * (intPos.p / aa));          \
		surNor.q = (2.0f * (intPos.q / bb));

	// we intersect an endcap if
	//    the ray intersects the plane in which the cap lies
	//    the ray points toward the cap surface, not away from it
	//    the intersection point is inside the elliptical boundary
	#define HAVE_CYLINDER_ENDCAP_INTERSECTION(cap, p, q      )                   \
		(cap.IntersectRay(math::RaySegment(rayPos, rayDir), &capInt)        &&   \
		(((capInt.GetNrm()).dot3D(rayDir)) < 0.0f)                          &&   \
		((((capInt.GetPos()).p / size.y) * ((capInt.GetPos()).p / size.y))  +    \
		((((capInt.GetPos()).q / size.z) * ((capInt.GetPos()).q / size.z))) <=   \
		1.0f))

	// we potentially hit the cylinder surface if the
	// intersection position is between the end-caps
	// (the ray might hit a cap first)
	#define HAVE_CYLINDER_SURFACE_INTERSECTION(p)  \
		(intPos.p >= -(size.x * 0.5f)) &&          \
		(intPos.p <=  (size.x * 0.5f))

	#define SET_INTERSECTION_NORMAL()                           \
		if (intersectCap) {                                     \
			useSurNormal =                                      \
				(intersectSur &&                                \
				((capInt.GetPos() - rayPos).sqLen3D() >         \
				(intPos - rayPos).sqLen3D()));                  \
		} else {                                                \
			useSurNormal = intersectSur;                        \
		}                                                       \
		                                                        \
		if (useSurNormal) {                                     \
			rayInt->SetPos(intPos + pos);                       \
			rayInt->SetNrm(surNor.norm());                      \
		}                                                       \
		                                                        \
		return (intersectCap || intersectSur);                  \

	#define CHECK_INTERSECTION_POINT_CAP()                             \
		switch (axis) {                                                \
			case math::COOR_AXIS_X: {                                  \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap0, y, z)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(capNor);                            \
					intersectCap = true;                               \
				}                                                      \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap1, y, z)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(-capNor);                           \
					intersectCap = true;                               \
				}                                                      \
			} break;                                                   \
			case math::COOR_AXIS_Y: {                                  \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap0, x, z)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(capNor);                            \
					intersectCap = true;                               \
				}                                                      \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap1, x, z)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(-capNor);                           \
					intersectCap = true;                               \
				}                                                      \
			} break;                                                   \
			case math::COOR_AXIS_Z: {                                  \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap0, x, y)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(capNor);                            \
					intersectCap = true;                               \
				}                                                      \
				if (HAVE_CYLINDER_ENDCAP_INTERSECTION(cap1, x, y)) {   \
					rayInt->SetPos(capInt.GetPos() + pos);             \
					rayInt->SetNrm(-capNor);                           \
					intersectCap = true;                               \
				}                                                      \
			} break;                                                   \
		}

	#define CHECK_INTERSECTION_POINT(t)                                            \
		if (t > 0.0f) {                                                            \
			const math::vec3f intPos = rayPos + (rayDir * t);                      \
		                                                                           \
			switch (axis) {                                                        \
				case math::COOR_AXIS_X: {                                          \
					if ((intersectSur = HAVE_CYLINDER_SURFACE_INTERSECTION(x))) {  \
						SET_CYLINDER_SURFACE_NORMAL_PARAMS(y, z);                  \
					}                                                              \
				} break;                                                           \
				case math::COOR_AXIS_Y: {                                          \
					if ((intersectSur = HAVE_CYLINDER_SURFACE_INTERSECTION(y))) {  \
						SET_CYLINDER_SURFACE_NORMAL_PARAMS(x, z);                  \
					}                                                              \
				} break;                                                           \
				case math::COOR_AXIS_Z: {                                          \
					if ((intersectSur = HAVE_CYLINDER_SURFACE_INTERSECTION(z))) {  \
						SET_CYLINDER_SURFACE_NORMAL_PARAMS(x, y);                  \
					}                                                              \
				} break;                                                           \
			}                                                                      \
		                                                                           \
			CHECK_INTERSECTION_POINT_CAP();                                        \
			SET_INTERSECTION_NORMAL();                                             \
		}


	switch (axis) {
		case math::COOR_AXIS_X: {
			// a is y-radius, b is z-radius
			SET_CYLINDER_SURFACE_EQUATION_PARAMS(y, z); capNor = math::XVECf;
		} break;
		case math::COOR_AXIS_Y: {
			// a is x-radius, b is z-radius
			SET_CYLINDER_SURFACE_EQUATION_PARAMS(x, z); capNor = math::YVECf;
		} break;
		case math::COOR_AXIS_Z: {
			// a is x-radius, b is y-radius
			SET_CYLINDER_SURFACE_EQUATION_PARAMS(x, y); capNor = math::ZVECf;
		} break;
	}

	// object-space cylinder end-cap planes
	PlaneSceneObject cap0(math::Plane( capNor, size.x * 0.5f));
	PlaneSceneObject cap1(math::Plane(-capNor, size.x * 0.5f));

	if (D < 0.0f) {
		// if the ray does not hit the (infinite) surface, it can
		// only pierce an end-cap if it is exactly parallel to the
		// cylinder's major axis and starts inside the elliptical
		// boundary
		// only if the ray *does* hit the infinite surface, it may
		// happen that a cap is intersected; if so, then this point
		// will be closer than the actual surface intersection
		CHECK_INTERSECTION_POINT_CAP();
		return intersectCap;
	} else {
		const float Dr = sqrtf(D);
		const float t0 = (-B - Dr) / (A + A);
		const float t1 = (-B + Dr) / (A + A);

		CHECK_INTERSECTION_POINT(t0);
		CHECK_INTERSECTION_POINT(t1);
	}

	#undef SET_CYLINDER_SURFACE_EQUATION_PARAMS
	#undef SET_CYLINDER_SURFACE_NORMAL_PARAMS
	#undef HAVE_CYLINDER_ENDCAP_INTERSECTION
	#undef HAVE_CYLINDER_SURFACE_INTERSECTION
	#undef SET_INTERSECTION_NORMAL
	#undef CHECK_INTERSECTION_POINT_CAP
	#undef CHECK_INTERSECTION_POINT

	return false;
}

bool CylinderSceneObject::IntersectCell(const math::vec3f& cellPos, const math::vec3f& cellDim) const {
	const math::vec3f dCellPos = cellPos - pos;
	const math::vec3f hCellDim = cellDim * 0.5f;

	if (dCellPos.x > (hCellDim.x + bsRadius) || dCellPos.x < -(hCellDim.x + bsRadius)) { return false; }
	if (dCellPos.y > (hCellDim.y + bsRadius) || dCellPos.y < -(hCellDim.y + bsRadius)) { return false; }
	if (dCellPos.z > (hCellDim.z + bsRadius) || dCellPos.z < -(hCellDim.z + bsRadius)) { return false; }

	return true;
}
