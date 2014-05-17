#ifndef KIRAN_VEC3_HDR
#define KIRAN_VEC3_HDR

#include <cassert>
#include <cmath>
#include <string>

#include "./fmath.hpp"

class RNGflt64;

namespace math {
	template<typename T> struct vec3 {
		vec3<T>(                ): x(T(0)), y(T(0)), z(T(0)) {}
		vec3<T>(T _x, T _y, T _z): x(_x),   y(_y),   z(_z)   {}
		vec3<T>(const vec3<T>& v): x(v.x),  y(v.y),  z(v.z)  {}

		inline bool operator == (const vec3<T>& v) const {
			#define EPS    T(0.0000001f)
			#define ABS(x) (((x) > T(0))? (x): -(x))
			return (ABS(x - v.x) <= EPS && ABS(y - v.y) <= EPS && ABS(z - v.z) <= EPS);
			#undef EPS
			#undef ABS
		}
		inline bool operator != (const vec3<T>& v) const {
			return !(*this == v);
		}

		inline T operator [] (const int i) const { return (&x)[i]; }
		inline T& operator [] (const int i) { return (&x)[i]; }


		inline vec3<T> operator + (const vec3<T>& v) const { return vec3<T>(x + v.x, y + v.y, z + v.z); }
		inline vec3<T> operator - (const vec3<T>& v) const { return vec3<T>(x - v.x, y - v.y, z - v.z); }
		inline vec3<T> operator * (const vec3<T>& v) const { return vec3<T>(x * v.x, y * v.y, z * v.z); }
		inline vec3<T> operator / (const vec3<T>& v) const { return vec3<T>(x / v.x, y / v.y, z / v.z); }

		inline vec3<T> operator + (const T s) const { return vec3<T>(x + s, y + s, z + s); }
		inline vec3<T> operator - (const T s) const { return vec3<T>(x - s, y - s, z - s); }
		inline vec3<T> operator * (const T s) const { return vec3<T>(x * s, y * s, z * s); }
		inline vec3<T> operator / (const T s) const { return vec3<T>(x / s, y / s, z / s); }

		inline vec3<T>& operator =  (const vec3<T>& v) { x  = v.x; y  = v.y; z  = v.z; return *this; }
		inline vec3<T>& operator += (const vec3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		inline vec3<T>& operator -= (const vec3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		inline vec3<T>& operator *= (const vec3<T>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		inline vec3<T>& operator /= (const vec3<T>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

		inline vec3<T>& operator += (const T s) { x += s; y += s; z += s; return *this; }
		inline vec3<T>& operator -= (const T s) { x -= s; y -= s; z -= s; return *this; }
		inline vec3<T>& operator *= (const T s) { x *= s; y *= s; z *= s; return *this; }
		inline vec3<T>& operator /= (const T s) { x /= s; y /= s; z /= s; return *this; }

		inline vec3<T> operator - () const { return vec3<T>(-x, -y, -z); }


		inline T dot2D(const vec3<T>& v) const { return T(x * v.x +           z * v.z); }
		inline T dot3D(const vec3<T>& v) const { return T(x * v.x + y * v.y + z * v.z); }
		inline T sqLen2D() const { return (dot2D(*this)); }
		inline T sqLen3D() const { return (dot3D(*this)); }
		inline T len2D() const { return T(sqrtf(sqLen2D())); }
		inline T len3D() const { return T(sqrtf(sqLen3D())); }
		inline T flen2D() const { return T(math::sqrt1(sqLen2D())); }
		inline T flen3D() const { return T(math::sqrt1(sqLen3D())); }


		inline vec3<T> abs() const {
			#define ABS(x) (((x) > T(0))? (x): -(x))
			return vec3<T>(ABS(x), ABS(y), ABS(z));
			#undef ABS
		}

		// cross this vector with <v>
		inline vec3<T> cross(const vec3<T>& v) const {
			return vec3<T>((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x));
		}
		// cross this vector with <v> (in-place)
		inline vec3<T>& icross(const vec3<T>& v) {
			x = (y * v.z) - (z * v.y);
			y = (z * v.x) - (x * v.z);
			z = (x * v.y) - (y * v.x);
			return *this;
		}


		// normalize this vector
		inline vec3<T> norm() const {
			assert(x != T(0) || y != T(0) || z != T(0));

			T dp = sqLen3D();
			T rt = (dp == T(1))?  dp:  T(1) / T(sqrtf(dp));

			return vec3<T>(x * rt, y * rt, z * rt);
		}
		// normalize this vector (in-place)
		inline vec3<T>& inorm() {
			assert(x != T(0) || y != T(0) || z != T(0));

			T dp = sqLen3D();
			T rt = (dp == T(1))?  dp:  T(1) / T(sqrtf(dp));

			x *= rt;
			y *= rt;
			z *= rt;

			return *this;
		}
		// normalize this vector quickly
		inline vec3<T>& ifnorm() {
			assert(x != T(0) || y != T(0) || z != T(0));

			T dp = sqLen3D();
			T rt = (dp == T(1))?  dp:  T(math::isqrt1(dp));

			x *= rt;
			y *= rt;
			z *= rt;

			return *this;
		}


		// reflect this vector about the vector <N>
		inline vec3<T> reflect(const vec3<T>& N) const {
			const vec3<T>& V = *this;
			const vec3<T>  R = N * (N.dot3D(V) * T(2));
			return (V - R);
		}

		// refract this vector about the vector <N>
		//   n1 is the refr. index of the material being exited
		//   n2 is the refr. index of the material being entered
		inline vec3<T> refract(const vec3<T>& N, T n1, T n2) const {
			const vec3<T>& V = *this;

			const float n  = n1 / n2;
			const float ct = -(N.dot3D(V));
			const float st = T(1) - (n * n * (T(1) - ct * ct));

			if (st > T(0)) {
				return ((V * n) + (N * (n * ct - sqrtf(st))));
			} else {
				// total internal reflection
				return N;
			}
		}

		inline vec3<T>& randomize(RNGflt64*);
		inline vec3<T>& rrandomize(RNGflt64*);

		inline vec3<T> rotateX(T a) const { return vec3<T>(x, cos(a) * y + -sin(a) * z,     sin(a) * y + cos(a) * z   ); }
		inline vec3<T> rotateY(T a) const { return vec3<T>(   cos(a) * x +  sin(a) * z, y, -sin(a) * x + cos(a) * z   ); }
		inline vec3<T> rotateZ(T a) const { return vec3<T>(   cos(a) * x + -sin(a) * y,     sin(a) * x + cos(a) * y, z); }

		std::string str() const;
		unsigned int hash() const {
			unsigned int hx = *(int*) &x;
			unsigned int hy = *(int*) &y;
			unsigned int hz = *(int*) &z;
			return (hx ^ hy ^ hz);
		}


		T x;
		T y;
		T z;
	};

	const vec3<float> EVECf = vec3<float>(-1.0f, -1.0f, -1.0f); // error-vector
	const vec3<float> NVECf = vec3<float>( 0.0f,  0.0f,  0.0f); // null-vector
	const vec3<float> XVECf = vec3<float>( 1.0f,  0.0f,  0.0f); // world-coordinate x-axis
	const vec3<float> YVECf = vec3<float>( 0.0f,  1.0f,  0.0f); // world-coordinate y-axis
	const vec3<float> ZVECf = vec3<float>( 0.0f,  0.0f,  1.0f); // world-coordinate z-axis
	const vec3<float> UVECf = vec3<float>( 1.0f,  1.0f,  1.0f); // unity / identity vector

	const vec3<int> UVECi = vec3<int>(1, 1, 1);

	enum COOR_AXIS {
		COOR_AXIS_X = 0,
		COOR_AXIS_Y = 1,
		COOR_AXIS_Z = 2,
	};
}

#endif
