#include <cfloat>
#include <cstring>
#include <iostream>

#include "./Scene.hpp"
#include "./SceneLight.hpp"
#include "./SceneObject.hpp"
#include "./Material.hpp"
#include "./MaterialReflectionModel.hpp"
#include "./Camera.hpp"
#include "../datastructs/UniformGrid.hpp"
#include "../system/Defines.hpp"
#include "../system/LuaParser.hpp"

Scene::Scene(LuaParser& parser, Camera* cam) {
	camera = cam;

	const LuaTable* rootTable  = parser.GetRootTbl();           assert(rootTable  != NULL);
	const LuaTable* sceneTable = rootTable->GetTblVal("scene"); assert(sceneTable != NULL);

	const LuaTable* materialsTable = sceneTable->GetTblVal("materials"); assert(materialsTable != NULL);
	const LuaTable* lightsTable    = sceneTable->GetTblVal("lights");    assert(lightsTable    != NULL);
	const LuaTable* objectsTable   = sceneTable->GetTblVal("objects");   assert(objectsTable   != NULL);

	minBounds = sceneTable->GetVec<math::vec3f>("minBounds", 3);
	maxBounds = sceneTable->GetVec<math::vec3f>("maxBounds", 3);

	#if (SCENEOBJECT_GRID_PARTITIONING == 1)
	// derive the number of grid-cells from the scene-bounds (this
	// might become excessive, add configurable resolution scale?)
	//
	// NOTE: the number of cells (x * y * z) must be < (1 << 31),
	// otherwise the intersection-test cache mechanism will break
	objectGridCellCount.x = (maxBounds.x - minBounds.x) * 0.2f;
	objectGridCellCount.y = (maxBounds.y - minBounds.y) * 0.2f;
	objectGridCellCount.z = (maxBounds.z - minBounds.z) * 0.2f;
	objectGrid = new UniformGrid<const ISceneObject*>(objectGridCellCount);
	#else
	objectGrid = NULL;
	#endif

	std::cout << "[Scene::Scene]" << std::endl;
	std::cout << "\tminBounds:      " << minBounds.str()      << std::endl;
	std::cout << "\tmaxBounds:      " << maxBounds.str()      << std::endl;

	std::list<int> materialIDs;
	std::list<int> lightIDs;
	std::list<int> objectIDs;

	materialsTable->GetIntTblKeys(&materialIDs);
	lightsTable->GetIntTblKeys(&lightIDs);
	objectsTable->GetIntTblKeys(&objectIDs);

	for (std::list<int>::const_iterator it = materialIDs.begin(); it != materialIDs.end(); it++) {
		AddMaterial(materialsTable->GetTblVal(*it, NULL));
	}

	{
		// add a default material for objects
		// that do not specify one themselves
		// (assumes there is no other material
		// named "default")
		LuaTable defaultMaterialTable;
		AddMaterial(&defaultMaterialTable);
	}

	for (std::list<int>::const_iterator it = lightIDs.begin(); it != lightIDs.end(); it++) {
		AddLight(lightsTable->GetTblVal(*it, NULL));
	}
	for (std::list<int>::const_iterator it = objectIDs.begin(); it != objectIDs.end(); it++) {
		AddObject(objectsTable->GetTblVal(*it, NULL));
	}

	#if (SCENEOBJECT_GRID_PARTITIONING == 1)
	AddObjectsToGrid();
	#endif
}

Scene::~Scene() {
	for (std::map<std::string, Material*>::iterator it = materials.begin(); it != materials.end(); it++) {
		delete (it->second)->GetReflectionModel();
		delete (it->second);
	}

	for (std::list<ISceneLight*>::iterator it = lights.begin(); it != lights.end(); it++) {
		delete (*it);
	}

	for (std::list<ISceneObject*>::iterator it = objects.begin(); it != objects.end(); it++) {
		delete *it;
	}

	#if (SCENEOBJECT_GRID_PARTITIONING == 1)
	delete objectGrid;
	#endif

	materials.clear();
	lights.clear();
	objects.clear();
}


void Scene::AddMaterial(const LuaTable* matTable) {
	Material* mat = new Material();
		mat->SetTexture(matTable->GetStrVal("texture", ""));
		mat->SetRefractionIndex(matTable->GetFltVal("refractionIndex", 100.0f) / 100.0f);
		mat->SetBeerCoefficient(matTable->GetFltVal("beerCoefficient",   0.0f) / 100.0f);
		mat->SetSpecularExponent(matTable->GetFltVal("specularExponent", 100.0f) / 100.0f);
		mat->SetDiffuseReflectiveness(matTable->GetVec<math::vec3f>("diffuseReflectiveness", 3) / 100.0f);
		mat->SetSpecularReflectiveness(matTable->GetVec<math::vec3f>("specularReflectiveness", 3) / 100.0f);
		mat->SetSpecularRefractiveness(matTable->GetVec<math::vec3f>("specularRefractiveness", 3) / 100.0f);
		mat->SetID(materials.size());
		mat->SetReflectionModel(new PhongMaterialReflectionModel()); // for now, hardcoded

	materials[matTable->GetStrVal("type", "default")] = mat;
}

void Scene::AddLight(const LuaTable* lgtTable) {
	ISceneLight* light = NULL;

	if (lgtTable->GetFltVal("radius", 0.0f) <= 0.0f) {
		light = new PointSceneLight(
			lgtTable->GetVec<math::vec3f>("position", 3),
			lgtTable->GetVec<math::vec3f>("direction", 3),
			lgtTable->GetVec<math::vec3f>("power", 3),
			lgtTable->GetFltVal("numPhotons", 0.0f),
			lgtTable->GetFltVal("fov", 90.0f)
		);
	} else {
		light = new AreaSceneLight(
			lgtTable->GetVec<math::vec3f>("position", 3),
			lgtTable->GetVec<math::vec3f>("direction", 3),
			lgtTable->GetVec<math::vec3f>("power", 3),
			lgtTable->GetFltVal("numPhotons", 0.0f),
			lgtTable->GetFltVal("fov", 360.0f),
			lgtTable->GetFltVal("radius", 0.0f)
		);
	}

	lights.push_back(light);
}

void Scene::AddObject(const LuaTable* objTable) {
	const std::string type     = objTable->GetStrVal("type", "");
	const std::string material = objTable->GetStrVal("material", "");

	ISceneObject* object = NULL;
	Material* mat = NULL;

	std::map<std::string, Material*>::iterator matIt = materials.find(material);

	if (matIt == materials.end()) {
		mat = materials["default"];
	} else {
		mat = matIt->second;
	}

	if (type == "ellipse") {
		object = new EllipseSceneObject(
			objTable->GetVec<math::vec3f>("position", 3),
			objTable->GetVec<math::vec3f>("size", 3)
		);
	}
	else if (type == "plane") {
		object = new PlaneSceneObject(
			math::Plane(objTable->GetVec<math::vec3f>("normal", 3) / 100.0f, objTable->GetFltVal("distance", 0.0f))
		);
	}
	else if (type == "box") {
		object = new BoxSceneObject(
			objTable->GetVec<math::vec3f>("position", 3),
			objTable->GetVec<math::vec3f>("size", 3)
		);
	}
	else if (type == "cylinder") {
		object = new CylinderSceneObject(
			objTable->GetVec<math::vec3f>("position", 3),
			objTable->GetVec<math::vec3f>("size", 3),
			math::COOR_AXIS(objTable->GetFltVal("axis", 0.0f))
		);
	}
	else if (type == "mesh") {
		// object = new MeshSceneObject(pos);
	}

	assert(object != NULL);

	object->SetID(objects.size());
	object->SetMaterial(mat);
	objects.push_back(object);
}



#if (SCENEOBJECT_GRID_PARTITIONING == 1)
void Scene::AddObjectsToGrid() {
	const ISceneObject minsObject(minBounds);
	const ISceneObject maxsObject(maxBounds);

	// set the spatial position of each cell
	// this requires the grid extends to be
	// known, so use two temporary "virtual"
	// objects
	objectGrid->SetMins(&minsObject);
	objectGrid->SetMaxs(&maxsObject);
	objectGrid->SetCellPositions();

	std::vector<UniformGrid<const ISceneObject*>::GridCell>& cells = objectGrid->GetCells();
	std::vector<UniformGrid<const ISceneObject*>::GridCell>::iterator cellIt;

	// note: we have to visit each cell to handle
	// plane-objects (for which it would be hard
	// to enumerate the overlapped cells)
	// this implies that infinite planes "end" at
	// the grid's boundaries, which might or might
	// not be desirable
	for (cellIt = cells.begin(); cellIt != cells.end(); ++cellIt) {
		UniformGrid<const ISceneObject*>::GridCell& cell = *cellIt;

		for (std::list<ISceneObject*>::const_iterator objIt = objects.begin(); objIt != objects.end(); ++objIt) {
			const ISceneObject* object = *objIt;

			// for any object that is not an infinite plane,
			// test if its bounding sphere overlaps the cell
			// (so that rotated objects are treated properly)
			if (object->IntersectCell(cell.pos, objectGrid->GetCellSize())) {
				cell.nodes.push_back(object);
			}
		}
	}
}
#endif



#if (SCENEOBJECT_GRID_PARTITIONING == 1)
const ISceneObject* Scene::StepRayThroughGrid(unsigned int threadNum, const math::RaySegment& r, math::RayIntersection* i, float maxObjDst) const {
	// statically allocate the intersection-test cache
	// (Scene::Scene() does not yet know <numThreads>)
	// note that this is a minor memory-leak
	static const unsigned int numObjects = objects.size();
	static std::vector<int> intersectionCache(numThreads * numObjects);

	// reset the cache for this thread
	memset(&intersectionCache[threadNum * numObjects], RAY_INTERSECTION_UNTESTED, numObjects * sizeof(int));

	math::vec3f pos = r.GetPos();
	math::vec3f dir = r.GetDir();

	math::RayIntersection objInt;

	float minObjDst = FLT_MAX;
	float curObjDst = 0.0f;

	// TODO: handle case where camera (starting-position of all primary rays) is outside grid
	while (objectGrid->PosInBounds(pos)) {
		const UniformGrid<const ISceneObject*>::GridCell& cell = objectGrid->GetCell(pos);
		const std::list<const ISceneObject*>& objs = cell.nodes;

		// const math::vec3f& cellPos = cell.pos;
		const math::vec3f& cellSize = objectGrid->GetCellSize();
		const math::vec3f cellCornerPos = cell.pos + (cellSize * 0.5f);
		const math::vec3f cellCornerDist = cellCornerPos - pos;

		math::vec3f t;

		if (!objs.empty()) {
			// test each object in this cell for intersection
			for (std::list<const ISceneObject*>::const_iterator it = objs.begin(); it != objs.end(); ++it) {
				const ISceneObject* obj    = *it;
				const unsigned int  objIdx = (threadNum * numObjects) + obj->GetID();

				bool haveIntersection = false;
				bool delayedIntersection = false;

				switch (intersectionCache[objIdx]) {
					case RAY_INTERSECTION_TESTED_NONE: {
						// object was already tested (against this ray)
						// in a previous cell and no intersection found
						// (so do nothing)
						// continue;
					} break;

					case RAY_INTERSECTION_UNTESTED: {
						// object was not yet tested against this ray
						if (obj->IntersectRay(r, &objInt)) {
							const math::vec3f& intPos = objInt.GetPos();
							const math::vec3i& intIdx = objectGrid->GetCellIdx(intPos, true);

							haveIntersection = true;
							delayedIntersection = (intIdx != cell.idx);
						} else {
							// no need to test this object for intersection
							// again should we encounter it in another cell
							intersectionCache[objIdx] = RAY_INTERSECTION_TESTED_NONE;
						}
					} break;

					default: {
						// already tested and intersection found in a different cell
						// evaluate only if current cell is equal to the cached cell
						//
						// HACK: because the intersection position is not saved (in
						// the delayedIntersection branch below), when we reach the
						// default case here in a subsequent iteration we no longer
						// know what objInt.GetPos() was anymore and *must* recover
						// it through a second intersection test
						if (INDEX_1D(cell.idx, objectGrid->GetGridSize()) == intersectionCache[objIdx]) {
							obj->IntersectRay(r, &objInt);
							haveIntersection = true;
						}
					} break;
				}

				if (haveIntersection) {
					if (!delayedIntersection) {
						// we want the closest of all objects
						// that were intersected in this cell
						curObjDst = (objInt.GetPos() - r.GetPos()).sqLen3D();

						if (maxObjDst > 0.0f) {
							// distance-limit for finding an occludee
							if (curObjDst < maxObjDst) {
								i->SetPos(objInt.GetPos());
								i->SetNrm(objInt.GetNrm());
								i->SetObj(obj);
								break;
							}
						} else {
							if (i->GetObj() == NULL || curObjDst < minObjDst) {
								minObjDst = curObjDst;

								i->SetPos(objInt.GetPos());
								i->SetNrm(objInt.GetNrm());
								i->SetObj(obj);
							}
						}
					} else {
						// save the index of the cell where the intersection occurred
						// note: we should *also* save the intersection position itself
						intersectionCache[objIdx] = INDEX_1D(objectGrid->GetCellIdx(objInt.GetPos(), true), objectGrid->GetGridSize());
					}
				}
			}

			// if we found an intersection in the last
			// visited cell, we can stop grid-stepping
			//
			// NOTE: this is true IIF the intersection point
			// lies in this cell, which might not be the case
			// when objects are rotated
			// if not, then we want to remember the index of
			// the cell where the intersection was found, so
			// the object is not re-tested later [done]
			if (i->GetObj() != NULL) {
				break;
			}
		}

		if (dir.x < 0.0f) { t.x = (cellSize.x - cellCornerDist.x) / -dir.x; } else { t.x = cellCornerDist.x / (dir.x + 0.001f); }
		if (dir.y < 0.0f) { t.y = (cellSize.y - cellCornerDist.y) / -dir.y; } else { t.y = cellCornerDist.y / (dir.y + 0.001f); }
		if (dir.z < 0.0f) { t.z = (cellSize.z - cellCornerDist.z) / -dir.z; } else { t.z = cellCornerDist.z / (dir.z + 0.001f); }
		// clamping is needed because the addition of 0.001f can cause <pos> to go
		// infinitesimally out-of-bounds, such that the cornerDists (and hence the
		// intercept-ratios) become negative on the next iteration
		t.x = std::max(0.0f, std::min(cellSize.x, t.x));
		t.y = std::max(0.0f, std::min(cellSize.y, t.y));
		t.z = std::max(0.0f, std::min(cellSize.z, t.z));

		// we need to add some epsilon-distance to avoid infinite loops
		// in case <t> is zero, but we should not move too far into the
		// next cell
		// note: this value very heavily influences grid-traversal speeds,
		// as well as how wide artefact lines caused by advancing the ray
		// too far (such that objects end up "behind" it and are missed;
		// especially visible for infinite planes if they lie exactly on
		// faces of grid cells) will be
		pos += (dir * (std::min(t.x, std::min(t.y, t.z)) + 0.1f));
	}

	return (i->GetObj());
}
#endif



const ISceneObject* Scene::GetOccludingObject(unsigned int threadNum, const math::RaySegment& r, math::RayIntersection* i, const math::vec3f& pos) const {
	#if (SCENEOBJECT_GRID_PARTITIONING == 1)
	return (StepRayThroughGrid(threadNum, r, i, (r.GetPos() - pos).sqLen3D()));
	#else
	const float maxObjDst = (r.GetPos() - pos).sqLen3D();
	      float curObjDst = 0.0f;

	threadNum = -1;
	math::RayIntersection objInt;

	for (std::list<ISceneObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->IntersectRay(r, &objInt)) {
			curObjDst = (objInt.GetPos() - r.GetPos()).sqLen3D();

			if (curObjDst < maxObjDst) {
				i->SetPos(objInt.GetPos());
				i->SetNrm(objInt.GetNrm());
				i->SetObj(*it);
				break;
			}
		}
	}

	return (i->GetObj());
	#endif
}

const ISceneObject* Scene::GetClosestObject(unsigned int threadNum, const math::RaySegment& r, math::RayIntersection* i) const {
	#if (SCENEOBJECT_GRID_PARTITIONING == 1)
	return (StepRayThroughGrid(threadNum, r, i, -1.0f));
	#else
	float minObjDst = FLT_MAX;
	float curObjDst = 0.0f;

	threadNum = -1;
	math::RayIntersection objInt;

	for (std::list<ISceneObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->IntersectRay(r, &objInt)) {
			curObjDst = (objInt.GetPos() - r.GetPos()).sqLen3D();

			if (i->GetObj() == NULL || curObjDst < minObjDst) {
				minObjDst = curObjDst;

				i->SetPos(objInt.GetPos());
				i->SetNrm(objInt.GetNrm());
				i->SetObj(*it);
			}
		}
	}

	return (i->GetObj());
	#endif
}
