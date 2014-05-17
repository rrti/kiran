#ifndef KIRAN_PHOTON_MAP_HDR
#define KIRAN_PHOTON_MAP_HDR

#include <vector>

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

template<typename T> class KDTree;
template<typename T> class UniformGrid;

namespace PhotonMap {
	struct Photon {
	public:
		typedef unsigned char ubyte8;
		enum {
			AXIS_X = 0,
			AXIS_Y = 1,
			AXIS_Z = 2,
		};

		Photon(): pos(math::NVECf), pwr(math::NVECf), axis(AXIS_X) {
			SetDirection(math::NVECf);
		}
		Photon(const math::vec3f& pos, const math::vec3f& dir, const math::vec3f& pwr): pos(pos), pwr(pwr), axis(AXIS_X) {
			SetDirection(dir);
		}

		static void InitDirectionTables();

		void SetPos(const math::vec3f& p) { pos = p; }
	//	void SetDir(const math::vec3f& d) { dir = d; }
		void SetPwr(const math::vec3f& p) { pwr = p; }

		#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1 || USE_SPHERE_COMPRESSION == 1)
		void SetNrm(const math::vec3f& n) { nrm = n; }
		void SetIrr(const math::vec3f& i) { irr = i; }
		#endif

		void SetAxis(ubyte8 _axis) { axis = _axis % 3; }
		// sets <theta> and <phi> based on <dir>
		void SetDirection(const math::vec3f&);

		const math::vec3f& GetPos() const { return pos; }
	//	const math::vec3f& GetDir() const { return dir; }
		const math::vec3f& GetPwr() const { return pwr; }

		#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1 || USE_SPHERE_COMPRESSION == 1)
		const math::vec3f& GetNrm() const { return nrm; }
		const math::vec3f& GetIrr() const { return irr; }
		#endif

		// returns tabular direction based on <theta> and <phi>
		math::vec3f GetDirection() const;

		ubyte8 GetAxis() const { return axis; }

		ubyte8 GetTheta() const { return theta; }
		ubyte8 GetPhi() const { return phi; }

	private:
		math::vec3f pos;
	//	math::vec3f dir;
		math::vec3f pwr;

		#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1 || USE_SPHERE_COMPRESSION == 1)
		math::vec3f irr;
		math::vec3f nrm;
		#endif

		// set during balancing
		ubyte8 axis;

		// spherical direction cosines
		ubyte8 theta;
		ubyte8 phi;

		// direction lookup tables
		static const int NUM_DIRECTIONS = 256;
		static float costheta[NUM_DIRECTIONS];
		static float sintheta[NUM_DIRECTIONS];
		static float cosphi[NUM_DIRECTIONS];
		static float sinphi[NUM_DIRECTIONS];
	};

	enum PhotonMapType {
		PHOTONMAP_GLOBAL  = 0,
		PHOTONMAP_DIFFUSE = 1,
		PHOTONMAP_CAUSTIC = 2,
	};

	class Map {
	public:
		Map(unsigned int, PhotonMapType);
		~Map();

		bool AddPhoton(PhotonMap::Photon*);
		void ScalePhotonPower(const math::vec3f&);

		// turn the flat array of photons into a
		// left-balanced max-heap (which is also
		// represented in array-form) after all
		// photons have been added
		void Finalize();

		#if (PRECOMPUTE_IRRADIANCE_ESTIMATES == 1)
		void PrecomputeIrradianceEstimates(unsigned int, unsigned int, float, unsigned int);
		#endif

		PhotonMapType GetMapType() const { return type; }
		unsigned int GetMapSize() const { return (photonArray.size() - 1); }
		unsigned int GetMapCapacity() const { return (photonArray.capacity() - 1); }

		math::vec3f GetIrradianceEstimate(const math::vec3f&, const math::vec3f&, float, unsigned int, bool = false) const;

	private:
		math::vec3f GetIrradianceEstimateGrid(const math::vec3f&, const math::vec3f&, float, unsigned int, bool) const;
		math::vec3f GetIrradianceEstimateTree(const math::vec3f&, const math::vec3f&, float, unsigned int, bool) const;
		math::vec3f GetIrradianceEstimateFlat(const math::vec3f&, const math::vec3f&, float, unsigned int, bool) const;

		// NOTE: each Photon* in photonTree::nodes (except the first)
		// points to an element of photonArray, which functions as a
		// backing store
		// (this only works because the array is resized just once,
		// any further resizing would invalidate the node pointers
		// in photonTree)
		std::vector<PhotonMap::Photon> photonArray;

		#if (PM_DATASTRUCT == PM_DATASTRUCT_TREE)
		KDTree<PhotonMap::Photon*>* photonTree;
		#elif (PM_DATASTRUCT == PM_DATASTRUCT_GRID)
		math::vec3i photonGridCellCount;
		UniformGrid<const PhotonMap::Photon*>* photonGrid;
		#endif

		PhotonMapType type;

		unsigned int numPhotons;
		unsigned int lastScaledPhoton;
		bool finalized;

		math::vec3f minPhotonPower;
		math::vec3f maxPhotonPower;
		math::vec3f avgPhotonPower;
	};
};

#endif
