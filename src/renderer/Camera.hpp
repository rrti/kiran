#ifndef KIRAN_CAMERA_HDR
#define KIRAN_CAMERA_HDR

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

struct LuaParser;
struct SDLWindow;
struct Camera {
public:
	Camera(LuaParser&, SDLWindow&);

	bool AABBInOriginPlane(const math::vec3f& plane, const math::vec3f& mins, const math::vec3f& maxs) const;
	bool InView(const math::vec3f& mins, const math::vec3f& maxs) const;
	bool InView(const math::vec3f& pos, float radius = 0.0f) const;

	void SetExternalParameters(const math::vec3f&, const math::vec3f&);
	void SetInternalParameters(float, unsigned int, unsigned int);
	const math::vec3f GetPixelDir(const SDLWindow&, unsigned int, unsigned int) const;
	const math::vec3f GetPixelPos(const SDLWindow&, unsigned int, unsigned int) const;
	const math::vec3f& GetPos() const { return pos; }
	const math::vec3f& GetXDir() const { return xdir; }
	const math::vec3f& GetYDir() const { return ydir; }
	const math::vec3f& GetZDir() const { return zdir; }

	void SetPos(const math::vec3f& p) { pos = p; }
	void SetTgt(const math::vec3f& t) { vrp = t; }

	bool RenderDOF() const { return renderDOF; }
	int GetLensAperture() const { return lensAperture; }
	float GetFocalPlaneDistance() const { return fplaneDistance; }

	void Update();

private:
	void UpdateCoorSys();
	void UpdateFrustum();

	math::vec3f  pos;        // world-space camera location (prp)
	math::vec3f  vrp;        // point relative to pos determining zdir
	math::vec3f  xdir;       // "right"   dir in world-coors
	math::vec3f  ydir;       // "up"      dir in world-coors
	math::vec3f  zdir;       // "forward" dir in world-coors

	math::vec3f frustumR;    // right view-frustrum plane
	math::vec3f frustumL;    // left view-frustrum plane
	math::vec3f frustumB;    // bottom view-frustrum plane
	math::vec3f frustumT;    // top view-frustrum plane

	math::vec3f viewPlane;   // xsize, ysize, zdist (all in world-coors)

	float hFOVrad;     // horizontal FOV angle (in radians)
	float vFOVrad;     // vertical FOV angle (in radians)
	float hhFOVrad;    // half of hVOFrad angle (in radians)
	float hvFOVrad;    // half of vVOFrad angle (in radians)
	float thhFOVrad;   // tangent of hhFOVrad (side ratio, not "in radians")
	float thvFOVrad;   // tangent of hvFOVrad (side ratio, not "in radians")
	float ithhFOVrad;  // reciprocal of thhFOVrad
	float ithvFOVrad;  // reciprocal of thvFOVrad
	float hAspRat;     // horizontal viewport aspect ratio (W / H)
	float vAspRat;     // vertical viewport aspect ratio (H / W)

	bool renderDOF;
	float fplaneDistance;
	int lensAperture;
};

#endif
