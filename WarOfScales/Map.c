#include "Map.h"

void Map_Init(Map* map)
{
	//TODO update this
	map->dim = (Point) { 0, 0, 0 };
	map->mapdim = (Point) { 0, 0, 0 };
	map->pos = (Point) { 0, 0, 0 };
#define X(a) map->a = NULL;
	MAP_DFIELD_MAP(X)
#undef X
	map->heightmap = NULL;
	map->fowmapa = NULL;
	map->fowmapb = NULL;
	map->linelist = NodeList_Create();
	map->unitlista = NodeList_Create();
	map->unitlistb = NodeList_Create();
}
/*
DIMENSIONS SHOULD BE POWERS OF 2 OR ELSE FUCK
*/
Map* Map_Create(Point* dim, Point* pos)
{
	Map* map = malloc(sizeof(Map));
	Map_CreateAt(map, dim, pos);
	return map;
}
void Map_CreateAt(Map* map, Point* dim, Point* pos)
{
	Point heightmapdim, fowmapdim;
	Map_Init(map);
	Point_Copy(&map->pos, pos);
	Point_Copy(&map->dim, dim);
	Point_Multiply(&map->mapdim, dim, MAP_TO_GRID_RATIO);
#define X(a, b) map->a = DiffusionField_Create(dim); Point_Multiply(&map->a->mapdim, dim, MAP_TO_GRID_RATIO); Point_Copy(&map->a->pos, pos); map->a->passesPerSec = b;
	MAP_DFIELD_MAP(X)
#undef X
	Point_Multiply(&heightmapdim, dim, HEIGHTMAP_RESOLUTION);
	Point_Multiply(&fowmapdim, dim, FOW_RESOLUTION);
	map->heightmap = Grid_Create(&heightmapdim, FALSE, TRUE);
	map->fowmapa = Grid_Create(&fowmapdim, TRUE, FALSE);
	map->fowmapb = Grid_Create(&fowmapdim, TRUE, FALSE);
}
Map* Map_CreateFromData(char* data);
Map* Map_CreateTest()
{
	int i;
	Map* map = Map_Create(&(Point) { 128, 128, 0 }, &(Point){-64 * MAP_TO_GRID_RATIO, -64 * MAP_TO_GRID_RATIO, 0});
	Node* line;
	Map_GenerateHeightmapFromData(map, "8|90 32 100|140 150 0|100 140 68|40 32 20|20 100 12|73 120 60|10 53 64|-20 130 8", NULL);
	for (i = 0; i < 500; i++)
	{
		Unit* unit = Unit_Create(GEFlags_Moving, &(Point){-500 - rand() % 500, rand() % 500 - 250, 0}, &(Point){16, 16, 0}, "res/anim(2x2).png");
		unit->gameelement->movespeed = 30 + rand() % 20;
		Map_AddUnit(map, unit, GETeam_A);
	}
	for (i = 0; i < 500; i++)
	{
		Unit* unit2 = Unit_Create(GEFlags_Moving, &(Point){500 + rand() % 500, rand() % 500 - 250, 0}, &(Point){16, 16, 0}, "res/anim(2x2).png");
		unit2->gameelement->movespeed = 15;
		unit2->gameelement->frame = 1;
		Map_AddUnit(map, unit2, GETeam_B);
	}
	line = Node_Create();
	line->Geom.Line = Line_Create(&(Point) { -200, -100, 0 }, &(Point){-100, 100, 0});
	line->flags += NFlag_Blocking3;
	NodeList_AddNode(map->linelist, line);
	line = Node_Create();
	line->Geom.Line = Line_Create(&(Point) { -200, -100, 0 }, &(Point){-200, -1000, 0});
	line->flags += NFlag_Blocking3;
	NodeList_AddNode(map->linelist, line);
	line = Node_Create();
	line->Geom.Line = Line_Create(&(Point) { 100, -100, 0 }, &(Point){200, 100, 0});
	line->flags += NFlag_Blocking3;
	NodeList_AddNode(map->linelist, line);
	line = Node_Create();
	line->Geom.Line = Line_Create(&(Point) { -200, 1000, 0 }, &(Point){200, 100, 0});
	line->flags += NFlag_Blocking3;
	NodeList_AddNode(map->linelist, line);
	Map_BakeDiffusionFields(map);
	return map;
}
/*
Format:
First token N is how many lines there are
For each line, the next 6 tokens are the line's p1x, p1y, p1z, p2x, p2y, p2z, not necessarily integers
*/
void Map_GenerateLinesFromData(Map* map, char* data, char** returnpointer)
{
	Line_GenerateFromData(map->linelist, data, returnpointer);
}
void Map_GenerateHeightmap(Map* map, PointArray* pa)
{
	if (pa) {
		int x, y, i;
		double weight, totalsum, totalquotient;
		for (x = 0; x < map->dim.x * HEIGHTMAP_RESOLUTION; x++)
		{
			for (y = 0; y < map->dim.y * HEIGHTMAP_RESOLUTION; y++)
			{
				totalsum = 0, totalquotient = 0;
				for (i = 0; i < pa->size; i++)
				{
					weight = 1 / (Point_LengthSquared2D(&(Point) { (x + 0.5) / HEIGHTMAP_RESOLUTION, (y + 0.5) / HEIGHTMAP_RESOLUTION, 0 }, &pa->points[i]) + 1);
					totalsum += pa->points[i].z * weight;
					totalquotient += weight;
				}
				map->heightmap->doublegrid[Grid_PointerArithmetic(&map->heightmap->dim, &(Point){ x, y, 0 })] = totalsum / totalquotient;
			}
		}
	}
}
/*
Format:
First token "points" is how many points are
For each point, the next 3 tokens are the point's x y z
*/
void Map_GenerateHeightmapFromData(Map* map, char* data, char** returnpointer)
{
	PointArray* heights = PointArray_CreateFromData(data, returnpointer);
	Map_GenerateHeightmap(map, heights);
}
/*
Compile the lines and set the diffusions fields that way
*/
void Map_BakeDiffusionFields(Map* map)
{
	Node* lineiterator;
	//o god
	map->dfield_patha->offenseagents = map->unitlistb;
	map->dfield_patha->defenseagents = map->unitlista;
	map->dfield_firea->offenseagents = map->unitlistb;
	map->dfield_firea->defenseagents = map->unitlista;
	//not gonna put this in a macro because fuck it
	map->dfield_pathb->offenseagents = map->unitlista;
	map->dfield_pathb->defenseagents = map->unitlistb;
	map->dfield_fireb->offenseagents = map->unitlista;
	map->dfield_fireb->defenseagents = map->unitlistb;
	for (lineiterator = map->linelist->headNode; lineiterator; lineiterator = lineiterator->nextNode)
	{
		Line_Rasterize(map->dfield_patha->diffusable, NULL, FALSE, TRUE, &map->dim, &map->mapdim, &map->pos, lineiterator->Geom.Line);
		Line_Rasterize(map->dfield_pathb->diffusable, NULL, FALSE, TRUE, &map->dim, &map->mapdim, &map->pos, lineiterator->Geom.Line);
		if (lineiterator->flags & NFlag_Blocking3)
		{
			Line_Rasterize(map->dfield_firea->diffusable, NULL, FALSE, TRUE, &map->dim, &map->mapdim, &map->pos, lineiterator->Geom.Line);
			Line_Rasterize(map->dfield_fireb->diffusable, NULL, FALSE, TRUE, &map->dim, &map->mapdim, &map->pos, lineiterator->Geom.Line);
		}
	}
}
void Map_AddUnit(Map* map, Unit* unit, int team)
{
	//add unit and initialize a few things
	int x, y;
	double radius = Max(unit->pathsearchradius / MAP_TO_GRID_RATIO, 1.42);
	Node *n = Node_Create(), *pointnode;
	for (x = -radius; x <= radius; x++) //get the set of points that the unit will have to check for pathing NOT SURE IF THIS IS FASTER THAN ALTERNATE SOLUTION
	{
		for (y = -radius; y <= radius; y++)
		{
			Point* p = Point_Create(x, y, 0);
			if (Point_Magnitude(p) <= radius)
			{
				pointnode = Node_Create();
				pointnode->Geom.Point = p;
				NodeList_AddNode(unit->pathpoints, pointnode);
			}
		}
	}
	unit->gameelement->team = team;
	n->GameElements.Unit = unit;
	n->w = unit->offenseweight;
	n->h = unit->defenseweight;
	switch (team)
	{
	case GETeam_A:
		unit->dfield_path = map->dfield_patha;
		unit->dfield_fire = map->dfield_firea;
		NodeList_AddNode(map->unitlista, n);
		break;
	case GETeam_B:
		unit->dfield_path = map->dfield_pathb;
		unit->dfield_fire = map->dfield_fireb;
		NodeList_AddNode(map->unitlistb, n);
		break;
	}
}
void Map_Update(Map* map, double frametime)
{
	Node* temp;
	Point gridspace;
	//Map_UpdateFOW(map);
	Map_UpdateDiffusionFields(map, frametime);
	Map_UpdatePathfind(map);
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Unit_Update(temp->GameElements.Unit, frametime, map->linelist);
		Map_MapToGrid(&gridspace, &temp->GameElements.Unit->gameelement->pos, map);
		temp->x = gridspace.x;
		temp->y = gridspace.y;
		temp->flags = temp->GameElements.Unit->gameelement->visible ? temp->flags | NFlag_Visible : temp->flags & (~0 - NFlag_Visible);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Unit_Update(temp->GameElements.Unit, frametime, map->linelist);
		Map_MapToGrid(&gridspace, &temp->GameElements.Unit->gameelement->pos, map);
		temp->x = gridspace.x;
		temp->y = gridspace.y;
		temp->flags = temp->GameElements.Unit->gameelement->visible ? temp->flags | NFlag_Visible : temp->flags & (~0 - NFlag_Visible);
	}
}
void Map_UpdatePathfind(Map* map)
{
	NodeList *visionlines = NodeList_Create();
	Node *temp, *pointiterator;
	BOOLEAN *coll;
	int index = 0;
	//each unit has (length of pathpoints) lines, each team has (length of unitlist) units THAT MOVED
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Node_SetFlags(temp, NFlag_Moved, Unit_HasMoved(temp->GameElements.Unit, map));
		Node_SetFlags(temp, NFlag_Stationary, Unit_IsStationary(temp->GameElements.Unit, map));
		Map_GetPathfindLinesForUnit(visionlines, map, temp->GameElements.Unit, temp->flags & (NFlag_Moved + NFlag_Stationary));
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Node_SetFlags(temp, NFlag_Moved, Unit_HasMoved(temp->GameElements.Unit, map));
		Node_SetFlags(temp, NFlag_Stationary, Unit_IsStationary(temp->GameElements.Unit, map));
		Map_GetPathfindLinesForUnit(visionlines, map, temp->GameElements.Unit, temp->flags & (NFlag_Moved + NFlag_Stationary));
	}
	coll = malloc(sizeof(BOOLEAN) * visionlines->listSize);
	Line_CollidesBatch(NULL, coll, NULL, NULL, NULL, NULL, visionlines, map->linelist, FALSE);
	for (index = 0, temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Map_UpdatePathfindForUnit(temp->GameElements.Unit, map, &index, coll, temp->flags & (NFlag_Moved + NFlag_Stationary));//index gets incremented accordingly in the function
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Map_UpdatePathfindForUnit(temp->GameElements.Unit, map, &index, coll, temp->flags & (NFlag_Moved + NFlag_Stationary));
	}
	NodeList_Destroy(visionlines);
	free(coll);
	//REMEMBER TO DESTROY ALL THAT HAS BEEN CREATED
}
/*
helper function for Map_UpdatePathFind
Creates nodes, remember to free them
*/
void Map_GetPathfindLinesForUnit(NodeList* list, Map* map, Unit* unit, BOOLEAN update)
{
	if (update) //if the unit has moved from its previous grid square, or is not moving
	{
		Node *pointiterator;
		Point p2gridspace, p2mapspace;
		for (pointiterator = unit->pathpoints->headNode; pointiterator; pointiterator = pointiterator->nextNode)
		{
			Node* linenode = Node_Create();

			Map_MapToGrid(&p2gridspace, &unit->gameelement->pos, map);
			Point_Add(&p2gridspace, &p2gridspace, pointiterator->Geom.Point);
			Point_ToInt(&p2gridspace, &p2gridspace);
			//at this point, p2gridspace is fully calculated
			if (p2gridspace.x >= 0 && p2gridspace.x < map->dim.x &&  p2gridspace.y >= 0 && p2gridspace.y < map->dim.y) //point within bounds
			{

				Point_Add(&p2gridspace, &p2gridspace, &HALF_POINT_2D); //center it in the middle of the grid
				Map_GridToMap(&p2mapspace, &p2gridspace, map);
				linenode->Geom.Line = Line_Create(&unit->gameelement->pos, &p2mapspace);
				Node_DisableFlags(pointiterator, NFlag_Disabled);
			}
			else
			{
				Node_EnableFlags(pointiterator, NFlag_Disabled); //nodes can be disabled for having lines that are out of bounds
			}
			NodeList_AddNode(list, linenode); //this is outside that scope just to keep # of nodes consistent for later calculations
		}
	}
}
/*
helper function for Map_UpdatePathfind
*/
void Map_UpdatePathfindForUnit(Unit* unit, Map* map, int* index, BOOLEAN* coll, BOOLEAN update)
{
	Node *pointiterator;
	Point p2gridspace, highestpointgridspace;
	double highestval = 0;
	if (update)
	{
		for (pointiterator = unit->pathpoints->headNode; pointiterator; pointiterator = pointiterator->nextNode, (*index)++)
		{
			if (!coll[*index] && !(pointiterator->flags & NFlag_Disabled)) //if the line didn't collide
			{
				Map_MapToGrid(&p2gridspace, &unit->gameelement->pos, map);
				Point_Add(&p2gridspace, &p2gridspace, pointiterator->Geom.Point);
				Point_ToInt(&p2gridspace, &p2gridspace);
				if (p2gridspace.x >= 0 && p2gridspace.x < map->dim.x &&  p2gridspace.y >= 0 && p2gridspace.y < map->dim.y) //point within bounds
				{
#define X(a, b) if (unit->a->field[Grid_PointerArithmetic(&unit->a->dim, &p2gridspace)] * unit->b > highestval) \
{highestval = unit->a->field[Grid_PointerArithmetic(&unit->a->dim, &p2gridspace)] * unit->b; Point_Copy(&highestpointgridspace, &p2gridspace);}
					UNIT_DFIELD_MAP(X)
#undef X
				}
				//pointiterator->val = 0;
			}
		}
		if (highestval == 0) //if theres literally nothing
		{
			Point_Copy(&unit->pathingpoint, &unit->gameelement->pos);
		}
		else
		{
			Point_Add(&highestpointgridspace, &highestpointgridspace, &HALF_POINT_2D);
			Map_GridToMap(&unit->pathingpoint, &highestpointgridspace, map);
		}
	}
}
void Map_CheckUnitCollisions(Map* map)
{

}
void Map_UpdateLOS(Map* map)
{
	//TODO code that calculates visibility
}
void Map_UpdateFOW(Map* map)
{
	Node* temp;
	
	SetArray(map->fowmapa->booleangrid, NULL, map->fowmapa->dim.x * map->fowmapa->dim.y, FALSE);
	SetArray(map->fowmapb->booleangrid, NULL, map->fowmapb->dim.x * map->fowmapb->dim.y, FALSE);
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Map_UpdateFOWForUnit(map, temp->GameElements.Unit, map->fowmapa);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Map_UpdateFOWForUnit(map, temp->GameElements.Unit, map->fowmapb);
	}
}
void Map_UpdateFOWForUnit(Map* map, Unit* unit, Grid* fowmap)
{
	int x, y;
	for (x = 0; x < fowmap->dim.x; x++)
	{
		for (y = 0; y < fowmap->dim.y; y++)
		{
			Point p = (Point) { x, y, 0 };
			if (!fowmap->booleangrid[Grid_PointerArithmetic(&fowmap->dim, &p)])//if not already visible
			{
				Point gridspace, mapspace;
				Point_Multiply(&gridspace, &p, 1. / FOW_RESOLUTION);
				Point_Add(&gridspace, &gridspace, &(Point){0.5, 0.5, 0});
				Map_GridToMap(&mapspace, &gridspace, map);
				if (Unit_PointIsInVision(unit, &mapspace))
				{
					fowmap->booleangrid[Grid_PointerArithmetic(&fowmap->dim, &p)] = TRUE;
				}
			}
		}
	}
}
void Map_UpdateDiffusionFields(Map* map, double frametime)
{
#define X(a) DiffusionField_Update(map->a, frametime);
	MAP_DFIELD_MAP(X)
#undef X
}
void Map_Destroy(Map* map);
void Map_MapToGrid(Point* out, Point* in, Map* map)
{
	out->x = (in->x - map->pos.x) * map->dim.x / map->mapdim.x;
	out->y = (in->y - map->pos.y) * map->dim.y / map->mapdim.y;
	out->z = in->z;
}
void Map_GridToMap(Point* out, Point* in, Map* map)
{
	out->x = in->x * map->mapdim.x / map->dim.x + map->pos.x;
	out->y = in->y * map->mapdim.y / map->dim.y + map->pos.y;
	out->z = in->z;
}

void Map_Draw(SDL_Renderer* renderer, Map* map, Camera* camera)
{
	int x, y;
	Node* temp;
	/*
	for (x = 0; x < map->dim.x * HEIGHTMAP_RESOLUTION; x++)
	{
		for (y = 0; y < map->dim.y * HEIGHTMAP_RESOLUTION; y++)
		{
			Point screenspace, mapspace;
			SDL_Rect dstrect;
			double val = map->heightmap->doublegrid[DiffusionField_PointerArithmetic(&map->heightmap->dim, &(Point){ x, y, 0 })];
			//double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0; //non-linear lighting
			double alpha = val != 0 ? 255 * ((int)val % 100) / 100 : 0;
			Map_GridToMap(&mapspace, &(Point){(double)x / HEIGHTMAP_RESOLUTION, (double)y / HEIGHTMAP_RESOLUTION, 0}, &map->dim, &map->mapdim, &map->pos);
			Camera_MapToScreen(&screenspace, camera, &mapspace); //top down viwe
			//Camera_MapToScreen(&screenspace, camera, &(Point){x * map->mapdim.x / map->dim.x / HEIGHTMAP_RESOLUTION + map->pos.x, -val, 0}); alpha = 5; //side view
			dstrect.x = screenspace.x;
			dstrect.y = screenspace.y;
			dstrect.w = map->mapdim.x / map->dim.x / HEIGHTMAP_RESOLUTION * camera->scale;
			dstrect.h = map->mapdim.y / map->dim.y / HEIGHTMAP_RESOLUTION * camera->scale;
			SDL_SetTextureAlphaMod(dfieldtexture, 255);
			SDL_SetTextureColorMod(dfieldtexture, alpha, alpha, alpha);
			SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		}
	}
	*/
#define X(a) DiffusionField_Draw(renderer, map->a, camera);
	MAP_DFIELD_MAP(X)
#undef X
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Unit_Draw(renderer, temp->GameElements.Unit, camera);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Unit_Draw(renderer, temp->GameElements.Unit, camera);
	}
	/*
	for (x = 0; x < map->fowmapa->dim.x; x++)
	{
		for (y = 0; y < map->fowmapa->dim.y; y++)
		{
			Point screenspace, mapspace;
			SDL_Rect dstrect;
			BOOLEAN val = map->fowmapa->booleangrid[DiffusionField_PointerArithmetic(&map->fowmapa->dim, &(Point){ x, y, 0 })];
			//double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0; //non-linear lighting
			double alpha = val ? 0 : 255;
			Map_GridToMap(&mapspace, &(Point){(double)x / FOW_RESOLUTION, (double)y / FOW_RESOLUTION, 0}, &map->dim, &map->mapdim, &map->pos);
			Camera_MapToScreen(&screenspace, camera, &mapspace); //top down view
			dstrect.x = screenspace.x;
			dstrect.y = screenspace.y;
			dstrect.w = map->mapdim.x / map->dim.x / FOW_RESOLUTION * camera->scale;
			dstrect.h = map->mapdim.y / map->dim.y / FOW_RESOLUTION * camera->scale;
			SDL_SetTextureAlphaMod(dfieldtexture, alpha);
			SDL_SetTextureColorMod(dfieldtexture, 0, 0, 0);
			SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		}
	}
	*/
	for (temp = map->linelist->headNode; temp; temp = temp->nextNode)
	{
		Point p1, p2;
		Camera_MapToScreen(&p1, camera, &temp->Geom.Line->p1);
		Camera_MapToScreen(&p2, camera, &temp->Geom.Line->p2);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
	}
	
	


	//DiffusionField_Draw(renderer, map->dfield_pathb, camera);
}

BOOLEAN Unit_IsStationary(Unit* unit, Map* map)
{
	Point posgridspace, pathingpointgridspace;
	Map_MapToGrid(&posgridspace, &unit->gameelement->pos, map);
	Point_ToInt(&posgridspace, &posgridspace);
	Map_MapToGrid(&pathingpointgridspace, &unit->pathingpoint, map);
	Point_ToInt(&pathingpointgridspace, &pathingpointgridspace);
	return (Point_IsEqual(&unit->gameelement->movevec, &(Point){0, 0, 0}) || Point_IsEqual(&posgridspace, &pathingpointgridspace)) ? TRUE : FALSE;
}
BOOLEAN Unit_HasMoved(Unit* unit, Map* map)
{
	Point posgridspace, lastposgridspace;
	Map_MapToGrid(&posgridspace, &unit->gameelement->pos, map);
	Point_ToInt(&posgridspace, &posgridspace);
	Map_MapToGrid(&lastposgridspace, &unit->gameelement->lastpos, map);
	Point_ToInt(&lastposgridspace, &lastposgridspace);
	return (!Point_IsEqual(&posgridspace, &lastposgridspace)) ? TRUE : FALSE;
}