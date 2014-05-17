#include <cassert>
#include <cmath>

#include "../system/Defines.hpp"

#include "./Camera.hpp"
#include "../math/Ray.hpp"
#include "../system/SDLWindow.hpp"
#include "../system/LuaParser.hpp"

Camera::Camera(LuaParser& parser, SDLWindow& window):
	pos(math::NVECf),
	vrp(math::NVECf),
	xdir(math::XVECf),
	ydir(math::YVECf),
	zdir(math::ZVECf)
{
	const LuaTable* rootTable = parser.GetRootTbl();
	const LuaTable* sceneTable = rootTable->GetTblVal("scene");
	const LuaTable* cameraTable = sceneTable->GetTblVal("camera");

	const float camFOV = cameraTable->GetFltVal("vfov", 90.0f);
	const math::vec3f camPos = cameraTable->GetVec<math::vec3f>("pos", 3);
	const math::vec3f camVRP = cameraTable->GetVec<math::vec3f>("vrp", 3);

	// get the view-plane dimensions in world-space
	const math::vec3f camVP = cameraTable->GetVec<math::vec3f>("vplane", 3);

	SetExternalParameters(camPos, camVRP);
	SetInternalParameters(camFOV, window.GetSizeX(), window.GetSizeY());

	// TODO: move to SetInternalParameters
	viewPlane.x = camVP.x;
	viewPlane.y = camVP.y;
	viewPlane.z = (camVP.y * 0.5f) / tanf(DEG2RAD(camFOV * 0.5f));

	// note: focal-plane distance is relative to position,
	// so must always be further away than the view-plane
	// (also note that fplaneDistance is absolute)
	renderDOF = bool(cameraTable->GetFltVal("renderDOF", 0));
	fplaneDistance = cameraTable->GetFltVal("fplaneDist", viewPlane.z * 2.0f);
	lensAperture = int(cameraTable->GetFltVal("lensAperture", 4));

	assert(fplaneDistance > viewPlane.z);
}

bool Camera::AABBInOriginPlane(const math::vec3f& plane, const math::vec3f& mins, const math::vec3f& maxs) const {
	math::vec3f fp;
		fp.x = (plane.x > 0.0f)? mins.x: maxs.x;
		fp.y = (plane.y > 0.0f)? mins.y: maxs.y;
		fp.z = (plane.z > 0.0f)? mins.z: maxs.z;
	return (plane.dot3D(fp - pos) < 0.0f);
}

// returns true iif all corners of bounding-box defined
// by <mins, maxs> lie on the "positive" side of each of
// the frustum's half-spaces (?)
bool Camera::InView(const math::vec3f& mins, const math::vec3f& maxs) const {
	return
		AABBInOriginPlane(frustumR, mins, maxs) &&
		AABBInOriginPlane(frustumL, mins, maxs) &&
		AABBInOriginPlane(frustumB, mins, maxs) &&
		AABBInOriginPlane(frustumT, mins, maxs);
}
bool Camera::InView(const math::vec3f& p, float radius) const {
	const math::vec3f t = (p - pos);

	return
		(t.dot3D(frustumR) < radius) &&
		(t.dot3D(frustumL) < radius) &&
		(t.dot3D(frustumB) < radius) &&
		(t.dot3D(frustumT) < radius);
}

void Camera::SetExternalParameters(const math::vec3f& p, const math::vec3f& t) {
	pos = p;
	vrp = t;
}
void Camera::SetInternalParameters(float vfov, unsigned int wx, unsigned int wy) {
	hAspRat = wx / float(wy);
	vAspRat = wy / float(wx);

	// argument x of atan(x) is ratio between two
	// triangle sides; vFOVrad however is an angle
	// so we need a nested tanf() call
	//
	// derive horizontal FOV from vFOV and aspect
	vFOVrad = DEG2RAD(vfov);
	hFOVrad = (atanf(hAspRat * tanf(vFOVrad * 0.5f)) * 2.0f);

	// set the horizontal and vertical half-FOV's
	hhFOVrad = hFOVrad * 0.5f;
	hvFOVrad = vFOVrad * 0.5f;

	// set the tangents of the half-FOV's
	thhFOVrad = tanf(hhFOVrad);
	thvFOVrad = tanf(hvFOVrad);

	// note: with a full vertical FOV of 90.0 degrees, thvFOVrad
	// will simply be tan(DEG2RAD(45.0)) == 1.0 (because the 'O'
	// and 'A' triangle sides are then equal in length, so their
	// ratio will be 1.0)
	// these numbers do not actually represent focal lengths!
	ithhFOVrad = 1.0f / thhFOVrad;
	ithvFOVrad = 1.0f / thvFOVrad;
}


// get the direction of a virtual world-space
// ray through the screen-space pixel at (x,y)
// note: could precompute this for every pixel
//
// assume origin is in center of plane; this
// means pos + zdir points at pixel w/2, h/2
// note: pos is starting point of a ray, but
// not added to final direction here so that
// vectors are position-independent
// (origin of the SDL window is in top-left)
//
// note: independent of the position (z-distance) or
// size of the view-plane; this code assumes that a
// 1:1 mapping exists between the *screen*-size (in
// pixels) and the world-space view-plane dimensions
//
// moving image plane further away makes objects
// bigger (since FOV is being narrowed) and vv.
//

/*
const math::vec3f Camera::GetPixelDir(const SDLWindow& w, unsigned int x, unsigned int y) const {
	// map {x, y} to [-W, W] and [-H, H] first, then
	// normalize to [-1, 1] and scale the directions
	// by the {h, v}FOV angle tangents
	const int   xr = (x << 1) - w.GetSizeX();
	const int   yr = (y << 1) - w.GetSizeY();
	const float dx = (xr / float(w.GetSizeX())) * (thhFOVrad * 2.0f) *  1.0f;
	const float dy = (yr / float(w.GetSizeY())) * (thvFOVrad * 2.0f) * -1.0f;
	const math::vec3f dir  = ((zdir * ithvFOVrad) + (xdir * dx) + (ydir * dy));

	return (dir.norm());
}
*/
const math::vec3f Camera::GetPixelDir(const SDLWindow& w, unsigned int x, unsigned int y) const {
	const float xr = ((int(x) - int(w.GetHSizeX())) / float(w.GetSizeX())) *  1.0f;
	const float yr = ((int(y) - int(w.GetHSizeY())) / float(w.GetSizeY())) * -1.0f;
	const math::vec3f dir =
		(zdir * ithvFOVrad)      +
		(xdir *    hAspRat) * xr +
		(ydir             ) * yr;

	return (dir.norm());
}

const math::vec3f Camera::GetPixelPos(const SDLWindow& w, unsigned int x, unsigned int y) const {
	const math::RaySegment ray(pos, GetPixelDir(w, x, y));

	// intersect the pixel-ray with the view-plane (which
	// faces the camera orthogonally; the distance to the
	// view-plane may not be negative)
	const float n = -(ray.GetPos()).dot3D(-zdir) + viewPlane.z;
	const float d =  (ray.GetDir()).dot3D(-zdir);
	const float t =  (n / d);

	return (pos + (ray.GetDir() * t));
}



void Camera::UpdateCoorSys() {
	zdir = (vrp - pos).inorm();
	xdir = (zdir.cross(ydir)).inorm();
	ydir = (xdir.cross(zdir)).inorm();
	vrp  = pos + zdir;
}

void Camera::UpdateFrustum() {
	const math::vec3f zdirY = (-zdir * (    thvFOVrad           ));
	const math::vec3f zdirX = (-zdir * (tanf(hvFOVrad * hAspRat)));

	frustumT = (zdirY + ydir).inorm();
	frustumB = (zdirY - ydir).inorm();
	frustumR = (zdirX + xdir).inorm();
	frustumL = (zdirX - xdir).inorm();
}

void Camera::Update() {
	UpdateCoorSys();
	UpdateFrustum();
}
