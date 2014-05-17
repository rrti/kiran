#ifndef KIRAN_PLANE_HDR
#define KIRAN_PLANE_HDR

#include "./vec3fwd.hpp"
#include "./vec3.hpp"

namespace math {
	// represents a plane ax + by + cz + d = 0 in Hessian normal form
	// normal vector is (a, b, c), unit normal vector is (nx, ny, nz)
	// where
	//    nx = a / sqrt(a * a + b * b + c * c)
	//    ny = b / sqrt(a * a + b * b + c * c)
	//    nz = c / sqrt(a * a + b * b + c * c)
	//     p = d / sqrt(a * a + b * b + c * c)
	struct Plane {
	public:
		Plane(): n(math::NVECf), d(0.0f) {
		}
		Plane(const math::vec3f& abc, float _d): n(abc.norm()), d(_d) {
			p = d / abc.len3D();
		}

		float PointDistance(const math::vec3f& point) const {
			return (n.dot3D(point) - p);
		}

		const math::vec3f& GetNormal() const { return n; }
		float GetDistance() const { return d; }

	private:
		math::vec3f n; // unit normal vector; defines the front-face
		float d;       // orthogonal distance from origin along <n>
		float p;       // normalized distance along <n>
	};
};

#endif
