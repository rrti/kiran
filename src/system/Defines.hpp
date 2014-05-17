#ifndef KIRAN_DEFINES_HDR
#define KIRAN_DEFINES_HDR

// Scene
#define SCENEOBJECT_GRID_PARTITIONING      0

// PhotonMap
#define PM_DATASTRUCT_TREE 0   // kd-tree
#define PM_DATASTRUCT_GRID 1   // uniform grid
#define PM_DATASTRUCT_FLAT 2   // no partitioning

//! which spatial-partitioning data-structure should
//! be used by the photon-map class (flat means none)
#define PM_DATASTRUCT                      PM_DATASTRUCT_TREE
//! whether to compress the volume queries for the
//! irradiance estimates along the normal vector of
//! the surface at which the query is made
#define USE_SPHERE_COMPRESSION             1
//! if the cosine of the angle between the query surface
//! normal and the normal of the surface impacted by the
//! photon is smaller than this ratio, then don't add the
//! photon to the query for the irradiance estimate
#define SPHERE_COMPRESSION_RATIO           0.9f
//! whether irradiance estimates should be precomputed for
//! stored photon positions after the photon-map has been
//! finalized (if true, run-time estimates are returned of
//! the single photon nearest to the query-position)
#define PRECOMPUTE_IRRADIANCE_ESTIMATES    0
//! whether the irradiance estimate should be based on the
//! distance of the furthest photon found in the query or
//! on the search-radius
#define USE_FURTHEST_PHOTON_DIST           1
//! whether photons closer to the query search-position
//! should be given a greater weight when estimating the
//! irradiance
#define FILTER_RADIANCE_ESTIMATE           1
#define FILTER_CONSTANT                    1.25f

#if (FILTER_RADIANCE_ESTIMATE == 1)
#define FILTER_NORMALIZER (1.0f - (2.0f / (3.0f * FILTER_CONSTANT)))
#else
#define FILTER_NORMALIZER (1.0f)
#endif



// RayTracer
//! whether area lights should generate soft-shadows
//! via Monte-Carlo sampling or via a fixed number of
//! pre-determined surface position offsets
#define MONTE_CARLO_SOFT_SHADOWS                1
#define NUM_MONTE_CARLO_LIGHT_SAMPLES          32
//! whether a photon's reflected power should also be divided
//! by the average diffuse (or specular) reflectance value of
//! the material it struck when performing Russian-Roulette
#define PHOTON_ENERGY_CONSERVATION              1
//! whether the photon-map only stores indirect illumination
//! (photons that have bounced at least once) and ray-tracing
//! is used to calculate direct illumination
#define PHOTON_MAP_INDIRECT_ILLUMINATION_ONLY   0
//! whether the irradiance estimates gathered during ray-tracing
//! should be multiplied by the diffuse reflectance value of the
//! material struck by a ray
#define IRRADIANCE_ESTIMATE_MATERIAL_MULTIPLY   1
//! how many (Monte-Carlo) sample-rays should be fired into the
//! scene to gather the final irradiance from the photon-map at
//! a given ray-surface intersection position
#define NUM_IRRADIANCE_GATHER_RAYS              0
#define IRRADIANCE_GATHER_RAY_WEIGHT            0.25f


// SDLWindow
#define WHITE_BALANCE_RANGE_LIMITING  1
// #define WHITE_BALANCE_RANGE_R(x)      (0.5f * logf(x))
// #define WHITE_BALANCE_RANGE_G(x)      (0.5f * logf(x))
// #define WHITE_BALANCE_RANGE_B(x)      (0.5f * logf(x))
#define WHITE_BALANCE_RANGE_R(x)      (1.5f * x)
#define WHITE_BALANCE_RANGE_G(x)      (1.5f * x)
#define WHITE_BALANCE_RANGE_B(x)      (1.5f * x)


// math
#define DEG2RAD(a) ((a) * (3.141592653f / 180.0f))
#define RAD2DEG(a) ((a) * (180.0f / 3.141592653f))


// debugging
#define DEBUG_ASSERTS_SDLWINDOW 0
#define DEBUG_ASSERTS_RAYTRACER 0
#define DEBUG_RENDER_PHOTON_MAP 0


// #define M_INF(x) std::isinf(x)
// #define M_NAN(x) std::isnan(x)
#define M_INF(x) std::numeric_limits<float>::infinity()
#define M_NAN(x) std::numeric_limits<float>::quiet_NaN()

#endif
