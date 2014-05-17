#ifndef KIRAN_SCENELIGHT_HDR
#define KIRAN_SCENELIGHT_HDR

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

struct ISceneLight {
public:
	ISceneLight(const math::vec3f& _pos, const math::vec3f& _dir, const math::vec3f& pwr, float _fov, unsigned int np) {
		pos = _pos;
		dir = _dir;

		power = pwr;
		numPhotons = np;

		fov = _fov;
	}

	virtual const math::vec3f& GetPos() const { return pos; }
	virtual const math::vec3f& GetDir() const { return dir; }
	virtual const math::vec3f& GetPower() const { return power; }
	virtual const math::vec3f  GetScaledPower() const { return power; }

	virtual unsigned int GetNumPhotons() const { return numPhotons; }
	virtual float GetFOV() const { return fov; }
	virtual float GetRadius() const = 0;

protected:
	// note: point- and area-lights are always positional
	// (directional lights do not exist in nature anyway;
	// would make <pos> and <fov> meaningless)
	//
	// both types can still have their own FOV, so we need
	// to have a "main direction" that defines the light-cone
	math::vec3f pos;
	math::vec3f dir;

	// emission power (per color-band); in Watt
	math::vec3f power;

	// number of photons this light will emit
	unsigned int numPhotons;

	// angle (in degrees) of light's FOV cone
	float fov;
};



// diffuse PSL: emits photons uniformly in all directions
struct PointSceneLight: public ISceneLight {
public:
	PointSceneLight(const math::vec3f& pos, const math::vec3f& dir, const math::vec3f& pwr, unsigned int np, float fov):
		ISceneLight(pos, dir, pwr, fov, np) {
	}

	const math::vec3f  GetScaledPower() const { return (power / (4.0f * M_PI)); }
	float GetRadius() const { return 0.0f; }
};

struct AreaSceneLight: public ISceneLight {
public:
	AreaSceneLight(const math::vec3f& pos, const math::vec3f& dir, const math::vec3f& pwr, unsigned int np, float fov, float _radius):
		ISceneLight(pos, dir, pwr, fov, np), radius(_radius) {
	}

	const math::vec3f  GetScaledPower() const { return (power / (4.0f * M_PI * radius)); }
	float GetRadius() const { return radius; }

protected:
	float radius;
};

#endif
