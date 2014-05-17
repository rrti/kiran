#ifndef KIRAN_RAYTRACER_HDR
#define KIRAN_RAYTRACER_HDR

#include "../math/vec3fwd.hpp"

namespace boost {
	class mutex;
	class barrier;
}

namespace math {
	struct RaySegment;
	struct RayIntersection;
}

struct LuaParser;
struct SDLWindow;
class Scene;
class Profiler;
class RNGflt64;

namespace PhotonMap {
	class Map;
	struct Photon;
};

class RayTracer {
public:
	enum {
		RAY_TYPE_PRIMARY    = 0,
		RAY_TYPE_PRIMARY_AA = 1,
		RAY_TYPE_PRIMARY_FP = 2,
		RAY_TYPE_REFLECT    = 3,
		RAY_TYPE_REFRACT    = 4,
		RAY_TYPE_SHADOW     = 5,
		RAY_TYPE_LAST       = 6,
	};
	enum {
		PHOTON_MATINT_REFLECTION_DIFFUSE  = 0,
		PHOTON_MATINT_REFLECTION_SPECULAR = 1,
		PHOTON_MATINT_REFRACTION          = 2,
		PHOTON_MATINT_ABSORPTION          = 3,
		PHOTON_MATINT_LAST                = 4,
	};

	RayTracer(LuaParser&, const Scene&);
	~RayTracer();

	void Render(SDLWindow&, const Scene&);
	unsigned int GetNumThreads() const { return numThreads; }

private:
	math::vec3f GatherIrradianceEstimate(unsigned int, const math::RayIntersection*, const Scene&, RNGflt64*);
	math::vec3f SampleDirectIllumination(unsigned int, const Scene&, const math::RaySegment&, const math::RayIntersection&, RNGflt64*, unsigned int) const;
	math::vec3f ShadeRayPM(unsigned int, const math::RaySegment&, const math::RayIntersection&, const Scene&, RNGflt64*, unsigned int);
	math::vec3f ShadeRayRT(unsigned int, const math::RaySegment&, const math::RayIntersection&, const Scene&, RNGflt64*, unsigned int);
	math::vec3f TraceRay(unsigned int, const math::RaySegment&, const Scene&, RNGflt64*, unsigned int, unsigned int);
	void TracePhoton(unsigned int, const Scene&, PhotonMap::Map*, PhotonMap::Photon*, RNGflt64*, unsigned int, bool);

	void TraceRayThread(unsigned int, SDLWindow&, const Scene&, RNGflt64*);
	void TracePhotonThread(unsigned int, boost::barrier*, const Scene&, PhotonMap::Map*, RNGflt64*);
	void RenderThread(unsigned int, boost::barrier*, SDLWindow&, const Scene&, RNGflt64*);

	unsigned int numThreads;
	unsigned int maxRayDepth;
	unsigned int maxPhotonDepth;
	bool antiAliasing;
	bool incrementalRender;

	bool photonMapping;
	unsigned int photonSearchCount;
	float photonSearchRadius;

	// stores all light-paths matching L(S|D)*D
	PhotonMap::Map* photonMap;
	// total number of photons emitted by all lights
	unsigned int mapNumPhotons;

	Profiler* profiler;
};

#endif
