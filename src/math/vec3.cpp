#include <cstdlib>
#include <string>
#include <sstream>

#include "./vec3.hpp"
#include "../system/RNG.hpp"

namespace math {
	// specialization for floats
	template<> vec3<float>& vec3<float>::randomize(RNGflt64* rng) {
		x = (*rng)();
		y = (*rng)();
		z = (*rng)();
		return *this;
	}

	template<> vec3<float>& vec3<float>::rrandomize(RNGflt64* rng) {
		x = 1.0f;
		y = 1.0f;
		z = 1.0f;

		// generate a random vector in the unit-sphere via
		// rejection-sampling; this is used when emitting
		// photons in uniformly distributed directions from
		// (diffuse) point-lights, as well as to pick random
		// surface positions for area-lights for soft-shadows
		while (sqLen3D() > 1.0f) {
			x = 2.0f * (*rng)() - 1.0f;
			y = 2.0f * (*rng)() - 1.0f;
			z = 2.0f * (*rng)() - 1.0f;
		}

		return (inorm());
	}

	template<> std::string vec3<float>::str() const {
		std::string s;
		std::stringstream ss;
			ss << "<";
			ss << x << ", ";
			ss << y << ", ";
			ss << z;
			ss << ">";
		s = ss.str();
		return s;
	}
}
