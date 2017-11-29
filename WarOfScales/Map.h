#pragma once
#include "DiffusionField.h"
#include "Utilities.h"
#include "GameElement.h"
#include "Unit.h"

#define FOW_RESOLUTION 1 //sqrt of how many fow pixels per diffusion field pixels
#define HEIGHTMAP_RESOLUTION 2 //sqrt of how many heightmap pixels per diffusion field pixels
#define MAP_TO_GRID_RATIO 32 //scale between map and grid

//member name, passes per sec
#define MAP_DFIELD_MAP(X) \
X(dfield_patha, 30) \
X(dfield_firea, 5) \
X(dfield_pathb, 30) \
X(dfield_fireb, 5) 
/*
Line blocking hiearchy: (which it blocks first)
1:Pathing->2:Low-Ground Vision->3:Bullets and All Vision
1:low z->2:actual z->3:high z

if a line blocks vision, it blocks paths
if a line blocks bullets, it blocks vision and paths
if a line can be seen over, it must also can be shot over

pathfield is subject to all levels of blocking (1,2,3)
fowmap and LOS are subject to levels 2-3 of blocking
firefield is subject only to level 3 blocking
*/
typedef struct Map {
	Point dim, mapdim, pos;
	DiffusionField *dfield_patha, *dfield_firea, *dfield_pathb, *dfield_fireb; //path field blocked by terrain, fire field not blocked by terrain you can shoot over
	Grid *heightmap, *fowmapa, *fowmapb;
	NodeList *linelist, *unitlista, *unitlistb;
	/*
	For linelist, flags is the blocking level
	For unitlist, xy is position, w is offense weight (dfield), h is defense weight (dfield), flags is visible or not
	*/
}Map;

void Map_Init(Map* map);
Map* Map_Create(Point* dim);
void Map_CreateAt(Map* map, Point* dim);
Map* Map_CreateFromData(char* data);
Map* Map_CreateTest();
void Map_GenerateLinesFromData(Map* map, char* data, char** returnpointer);
void Map_GenerateHeightmap(Map* map, PointArray* pa);
void Map_GenerateHeightmapFromData(Map* map, char* data, char** returnpointer);
void Map_BakeDiffusionFields(Map* map);
void Map_AddUnit(Map* map, Unit* unit, int team);
void Map_Update(Map* map, double frametime);
void Map_UpdatePathfind(Map* map);
void Map_GetPathfindLinesForUnit(NodeList* list, Map* map, Unit* unit, BOOLEAN update);
void Map_UpdatePathfindForUnit(Unit* unit, Map* map, int* index, BOOLEAN* coll, BOOLEAN update);
void Map_CheckUnitCollisions(Map* map);
void Map_UpdateLOS(Map* map);
void Map_UpdateFOW(Map* map);
void Map_UpdateFOWForUnit(Map* map, Unit* unit, Grid* fowmap);
void Map_UpdateDiffusionFields(Map* map, double frametime);
void Map_Destroy(Map* map);
void Map_MapToGrid(Point* out, Point* in, Map* map);
void Map_GridToMap(Point* out, Point* in, Map* map);

void Map_Draw(SDL_Renderer* renderer, Map* map, Camera* camera);

BOOLEAN Unit_IsStationary(Unit* unit, Map* map);
BOOLEAN Unit_HasMoved(Unit* unit, Map* map);