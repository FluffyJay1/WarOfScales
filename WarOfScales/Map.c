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
	map->fowupdatetimer = 1;
	map->fowcleartimer = 1;
	map->losupdatetimer = 1;
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
	//Node* line;
	Map_GenerateHeightmapFromData(map, "8|90 32 35|140 150 0|100 140 68|40 32 80|20 100 12|73 120 60|10 53 64|-20 130 8", NULL);
	for (i = 0; i < 1000; i++)
	{
		Unit* unit = Unit_Create(GEFlags_Moving, &(Point){-500 - rand() % 1000, rand() % 500 - 250, 0}, &(Point){16, 16, 0}, "res/anim(2x2).png");
		unit->gameelement->movespeed = 100;
		unit->gameelement->play = TRUE;
		unit->gameelement->animationspeed = 10;
		unit->aggrotimer = (rand() % 10) / 10.;
		Map_AddUnit(map, unit, GETeam_A);
	}
	for (i = 0; i < 1000; i++)
	{
		Unit* unit2 = Unit_Create(GEFlags_Moving, &(Point){500 + rand() % 1000, rand() % 500 - 250, 0}, &(Point){16, 16, 0}, "res/anim(2x2).png");
		unit2->gameelement->movespeed = 100;
		unit2->gameelement->frame = 1;
		unit2->aggrotimer = (rand() % 10) / 10.;
		Map_AddUnit(map, unit2, GETeam_B);
	}
	/*
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
	*/
	Map_GenerateLinesFromData(map, "4|-200 -100 0 -100 100 0 3|-200 -100 0 -200 -1000 0 3|100 -100 0 200 100 0 3|-200 1000 0 200 100 0 3", NULL);
	//Map_GenerateLinesFromData(map, "3|-100 -10000 0 -100 200 0 3|0 10000 0 0 -200 0 3|100 -10000 0 100 200 0 3", NULL);
	Map_BakeDiffusionFields(map);
	return map;
}
/*
Format:
First token N is how many lines there are
For each line, the next 7 tokens are the line's p1x, p1y, p1z, p2x, p2y, p2z, not necessarily integers, and level of blocking
*/
void Map_GenerateLinesFromData(Map* map, char* data, char** returnpointer)
{
	char* sc = data;
	int lines = ParseInt(sc, &sc), i, blocking;
	Node* n;
	for (i = 0; i < lines; i++)
	{
		n = Node_Create();
		n->Geom.Line = malloc(sizeof(Line));
		Line_GenerateFromData(n->Geom.Line, sc + 1, &sc);
		blocking = ParseInt(sc + 1, &sc);
		switch (blocking)
		{
		case 1:
			n->flags += NFlag_Blocking1;
			n->Geom.Line->p1.z = LINE_LEVEL1_Z;
			n->Geom.Line->p2.z = LINE_LEVEL1_Z;
			break;
		case 3:
			n->flags += NFlag_Blocking3;
			n->Geom.Line->p1.z = LINE_LEVEL3_Z;
			n->Geom.Line->p2.z = LINE_LEVEL3_Z;
			break;
		default:
			break;
		}
		NodeList_AddNode(map->linelist, n);
	}
	if (returnpointer)
	{
		*returnpointer = sc;
	}
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
	Point gridspace;
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
	Map_MapToGrid(&gridspace, &unit->gameelement->pos, map);
	n->x = gridspace.x;
	n->y = gridspace.y;
	n->w = unit->offenseweight;
	n->h = unit->defenseweight;
	n->Geom.Point = Point_Create(unit->offenselambda, unit->defenselambda, 0);
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
	Point gridspace, heightmapgridspace;
	Map_UpdateFOW(map, frametime); //wait until i know how to sdl surface hackery
	Map_UpdateDiffusionFields(map, frametime);
	Map_UpdatePathfind(map);
	Map_UpdateLOS(map, frametime);
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		if (temp->GameElements.Unit->gameelement->hp <= 0 && temp->GameElements.Unit->gameelement->alive)
		{
			Unit_OnDeath(temp->GameElements.Unit, map);
			temp->GameElements.Unit->gameelement->alive = FALSE;
		}
		Unit_Update(temp->GameElements.Unit, frametime, map->linelist);
		Map_MapToGrid(&gridspace, &temp->GameElements.Unit->gameelement->pos, map);
		temp->x = gridspace.x;
		temp->y = gridspace.y;
		//z position update
		Point_Multiply(&heightmapgridspace, &gridspace, HEIGHTMAP_RESOLUTION);
		Point_ToInt(&heightmapgridspace, &heightmapgridspace);
		temp->GameElements.Unit->gameelement->pos.z = map->heightmap->doublegrid[Grid_PointerArithmetic(&map->heightmap->dim, &heightmapgridspace)];
		Node_SetFlags(temp, NFlag_Visible, temp->GameElements.Unit->gameelement->visible);
		Node_SetFlags(temp, NFlag_Dead, !temp->GameElements.Unit->gameelement->alive);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		if (temp->GameElements.Unit->gameelement->hp <= 0 && temp->GameElements.Unit->gameelement->alive)
		{
			Unit_OnDeath(temp->GameElements.Unit, map);
			temp->GameElements.Unit->gameelement->alive = FALSE;
		}
		Unit_Update(temp->GameElements.Unit, frametime, map->linelist);
		Map_MapToGrid(&gridspace, &temp->GameElements.Unit->gameelement->pos, map);
		temp->x = gridspace.x;
		temp->y = gridspace.y;
		//z position update
		Point_Multiply(&heightmapgridspace, &gridspace, HEIGHTMAP_RESOLUTION);
		Point_ToInt(&heightmapgridspace, &heightmapgridspace);
		temp->GameElements.Unit->gameelement->pos.z = map->heightmap->doublegrid[Grid_PointerArithmetic(&map->heightmap->dim, &heightmapgridspace)];
		Node_SetFlags(temp, NFlag_Visible, temp->GameElements.Unit->gameelement->visible);
		Node_SetFlags(temp, NFlag_Dead, !temp->GameElements.Unit->gameelement->alive);
	}
	Map_UpdateUnitTargets(map);
	Map_CheckUnitCollisions(map);
}

void Map_UpdatePathfind(Map* map)
{
	NodeList visionlines;
	Node *temp, *pointiterator;
	BOOLEAN *coll;
	int index = 0;
	NodeList_Init(&visionlines);
	//each unit has (length of pathpoints) lines, each team has (length of unitlist) units THAT MOVED
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		if (!(temp->flags & NFlag_Dead))
		{
			Node_SetFlags(temp, NFlag_Moved, Unit_HasMoved(temp->GameElements.Unit, map));
			Node_SetFlags(temp, NFlag_Stationary, Unit_IsStationary(temp->GameElements.Unit, map));
			Node_SetFlags(temp, NFlag_Update, ((temp->flags & NFlag_Moved) || (temp->flags & NFlag_Stationary && (temp->GameElements.Unit->pathingtimer > UNIT_PATH_INTERVAL ? (temp->GameElements.Unit->pathingtimer = 0, TRUE) : FALSE))) ? TRUE : FALSE); //ah yes the classic comma ternary combo, who needs readability anyway
			//basically if the unit's stationary, it will check the timer to see if pathing needs to be updated, and if so, it will set the timer to 0 and return true
			Map_GetPathfindLinesForUnit(&visionlines, map, temp->GameElements.Unit, temp->flags & NFlag_Update);
		}
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		if (!(temp->flags & NFlag_Dead))
		{
			Node_SetFlags(temp, NFlag_Moved, Unit_HasMoved(temp->GameElements.Unit, map));
			Node_SetFlags(temp, NFlag_Stationary, Unit_IsStationary(temp->GameElements.Unit, map));
			Node_SetFlags(temp, NFlag_Update, ((temp->flags & NFlag_Moved) || (temp->flags & NFlag_Stationary && (temp->GameElements.Unit->pathingtimer > UNIT_PATH_INTERVAL ? (temp->GameElements.Unit->pathingtimer = 0, TRUE) : FALSE))) ? TRUE : FALSE); //ah yes the classic comma ternary combo, who needs readability anyway
			Map_GetPathfindLinesForUnit(&visionlines, map, temp->GameElements.Unit, temp->flags & NFlag_Update);
		}
	}
	coll = malloc(sizeof(BOOLEAN) * visionlines.listSize);
	Line_CollidesBatch(NULL, coll, NULL, NULL, NULL, NULL, NULL, NULL, &visionlines, map->linelist, FALSE);
	for (index = 0, temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		if (!(temp->flags & NFlag_Dead))
		{
			Map_UpdatePathfindForUnit(temp->GameElements.Unit, map, &index, coll, temp->flags & NFlag_Update);//index gets incremented accordingly in the function
		}
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		if (!(temp->flags & NFlag_Dead))
		{
			Map_UpdatePathfindForUnit(temp->GameElements.Unit, map, &index, coll, temp->flags & NFlag_Update);
		}
	}
	NodeList_Clear(&visionlines, TRUE);
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

/*
Function that checks if units collide with lines and moves they appropriately
*/
void Map_CheckUnitCollisions(Map* map)
{
	int index;
	Node *temp;
	NodeList movelines;
	BOOLEAN* coll;
	Point* collpos;
	//Line** colllines; //an array of pointers
	NodeList_Init(&movelines);
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode) //for each line each unit makes moving from one frame to another, add that line and unit to movelines
	{
		Node* n = Node_Create();
		n->Geom.Line = Line_Create(&temp->GameElements.Unit->gameelement->lastpos, &temp->GameElements.Unit->gameelement->pos);
		NodeList_AddNode(&movelines, n);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		Node* n = Node_Create();
		n->Geom.Line = Line_Create(&temp->GameElements.Unit->gameelement->lastpos, &temp->GameElements.Unit->gameelement->pos);
		NodeList_AddNode(&movelines, n);
	}
	coll = malloc(sizeof(BOOLEAN) * movelines.listSize);
	collpos = malloc(sizeof(Point) * movelines.listSize);
	//colllines = malloc(sizeof(size_t) * movelines.listSize);
	Line_CollidesBatch(NULL, coll, NULL, NULL, collpos, NULL, /*colllines*/ NULL, NULL, &movelines, map->linelist, FALSE);
	for (index = 0, temp = map->unitlista->headNode; temp; index++, temp = temp->nextNode)
	{
		if (coll[index])
		{
			//yeah i don't feel like making this its own function
			/* the "proper" way of doing it, only problem is this can shift the units across other lines, need to figure out how to make it not move across the lines
			Point orientcross, finalcross, vec1, vec2, linevec;
			double projection;
			Point_Subtract(&vec1, &colllines[index]->p1, &temp->GameElements.Unit->gameelement->pos);
			vec1.z = 0;
			Point_Subtract(&vec2, &colllines[index]->p2, &temp->GameElements.Unit->gameelement->pos);
			vec2.z = 0;
			Point_Cross(&orientcross, &vec1, &vec2);
			Point_Subtract(&linevec, &colllines[index]->p2, &colllines[index]->p1);
			Point_Cross(&finalcross, &linevec, &orientcross); //finalcross is the collline normal
			Point_Normalize(&finalcross, &finalcross);
			Point_Dot(&projection, &vec1, &finalcross);
			Point_Multiply(&finalcross, &finalcross, UNIT_LINE_COLLIDE_RADIUS + projection);
			Point_Add(&temp->GameElements.Unit->gameelement->pos, &temp->GameElements.Unit->gameelement->pos, &finalcross);
			*/
			Point_Traverse2D(&temp->GameElements.Unit->gameelement->pos, &collpos[index], &temp->GameElements.Unit->gameelement->pos, -UNIT_LINE_COLLIDE_RADIUS);
			//Point_Copy(&temp->GameElements.Unit->gameelement->pos, &collpos[index]);
		}
	}
	for (temp = map->unitlistb->headNode; temp; index++, temp = temp->nextNode)
	{
		if (coll[index])
		{
			Point_Traverse2D(&temp->GameElements.Unit->gameelement->pos, &collpos[index], &temp->GameElements.Unit->gameelement->pos, -UNIT_LINE_COLLIDE_RADIUS);
		}
	}
	free(coll);
	free(collpos);
	//free(colllines);
	NodeList_Clear(&movelines, TRUE);
	//remember to free all that has been mallocc
}

void Map_UpdateUnitTargets(Map* map)
{
	NodeList targets, targetlines, level3lines; //targets and targetlines are 2 separate lists because we need to destroy the lines but not the units
	int index = 0, *numtargetsa, *numtargetsb, unitindex = 0;
	Node *temp, *targetnode, *linenode;
	BOOLEAN *coll;
	NodeList_Init(&targets);
	NodeList_Init(&targetlines);
	NodeList_Init(&level3lines);
	numtargetsa = malloc(sizeof(int) * map->unitlista->listSize);
	numtargetsb = malloc(sizeof(int) * map->unitlistb->listSize);
	NodeList_GetNodesWithFlags(&level3lines, map->linelist, NFlag_Blocking3, FALSE, NULL, FALSE);
	for (temp = map->unitlista->headNode, unitindex = 0; temp; temp = temp->nextNode, unitindex++) 
	{
		if (temp->GameElements.Unit->gameelement->alive)
		{
			numtargetsa[unitindex] = Map_GetTargetLinesForUnit(temp->GameElements.Unit, map->unitlistb, &targets, &targetlines);
		}
	}
	for (temp = map->unitlistb->headNode, unitindex = 0; temp; temp = temp->nextNode, unitindex++)
	{
		if (temp->GameElements.Unit->gameelement->alive)
		{
			numtargetsb[unitindex] = Map_GetTargetLinesForUnit(temp->GameElements.Unit, map->unitlista, &targets, &targetlines);
		}
	}
	coll = malloc(sizeof(BOOLEAN) * targetlines.listSize);
	Line_CollidesBatch(NULL, coll, NULL, NULL, NULL, NULL, NULL, NULL, &targetlines, &level3lines, FALSE);
	targetnode = targets.headNode;
	linenode = targetlines.headNode;
	for (temp = map->unitlista->headNode, unitindex = 0; temp; temp = temp->nextNode, unitindex++)
	{
		if (temp->GameElements.Unit->gameelement->alive)
		{
			Map_UpdateTargetForUnit(temp->GameElements.Unit, &targetnode, &linenode, &index, coll, numtargetsa[unitindex]);
		}
	}
	for (temp = map->unitlistb->headNode, unitindex = 0; temp; temp = temp->nextNode, unitindex++)
	{
		if (temp->GameElements.Unit->gameelement->alive)
		{
			Map_UpdateTargetForUnit(temp->GameElements.Unit, &targetnode, &linenode, &index, coll, numtargetsb[unitindex]);
		}
	}
	NodeList_Clear(&targets, FALSE);
	NodeList_Clear(&targetlines, TRUE);
	NodeList_Clear(&level3lines, FALSE);
	free(coll);
	free(numtargetsa);
	free(numtargetsb);
}
int Map_GetTargetLinesForUnit(Unit* unit, NodeList* enemies, NodeList* targets, NodeList* lines)
{
	int numtargets = 0;
	Node *temp, *n, *targetsnode;
	if (unit->attacktarget)
	{
		if (!unit->attacktarget->gameelement->alive || !Unit_PointIsInAttackRange(unit, &unit->attacktarget->gameelement->pos))
		{
			unit->attacktarget = NULL;
		}
		else
		{
			n = Node_Create();
			n->Geom.Line = Line_Create(&unit->gameelement->pos, &unit->attacktarget->gameelement->pos);
			NodeList_AddNode(lines, n);
		}
	}
	if (!unit->attacktarget)
	{
		if (unit->aggrotimer > 1) //timer in unit_update
		{
			for (temp = enemies->headNode; temp; temp = temp->nextNode)
			{
				if (temp->GameElements.Unit->gameelement->alive && temp->flags & NFlag_Visible)
				{
					if (Unit_PointIsInAttackRange(unit, &temp->GameElements.Unit->gameelement->pos))
					{
						n = Node_Create();
						n->Geom.Line = Line_Create(&unit->gameelement->pos, &temp->GameElements.Unit->gameelement->pos);
						targetsnode = Node_Create();
						targetsnode->GameElements.Unit = temp->GameElements.Unit;
						NodeList_AddNode(lines, n);
						NodeList_AddNode(targets, targetsnode);
						numtargets++;
					}
				}
			}
		}
	}
	return numtargets;
}
void Map_UpdateTargetForUnit(Unit* unit, Node** targetnode, Node** linenode, int* index, BOOLEAN* coll, int numtargets)
{
	if (unit->attacktarget)
	{
		if (coll[*index])
		{
			unit->attacktarget = NULL;
		}
		(*index)++;
	}
	else if (!unit->attacktarget && unit->aggrotimer > 1)
	{
		//linenode's Line is the line, targetnode's unit is the target
		Node* closest = NULL;
		int endindex = *index + numtargets;
		double mindistance = unit->maxattackrange;
		for (; *index < endindex; (*index)++) //find closest possible target
		{
			if (!coll[*index])
			{
				if ((*linenode)->Geom.Line->length < mindistance)
				{
					closest = *targetnode;
					mindistance = (*linenode)->Geom.Line->length;
				}
			}
			*targetnode = (*targetnode)->nextNode;
			*linenode = (*linenode)->nextNode;
		}
		if (closest)
		{
			unit->attacktarget = closest->GameElements.Unit;
		}
		unit->aggrotimer = unit->aggrotimer - (int)unit->aggrotimer;
	}
}
void Map_UpdateLOS(Map* map, double frametime)
{
	map->losupdatetimer += frametime * LOS_UPDATE_SPEED;
	if (map->losupdatetimer >= 1)
	{
		Node *tempa, *tempb;
		NodeList lines;
		int index;
		BOOLEAN *coll = malloc(sizeof(BOOLEAN) * map->unitlista->listSize * map->unitlistb->listSize);
		NodeList_Init(&lines);
		//reset visibility to false
		for (tempa = map->unitlista->headNode; tempa; tempa = tempa->nextNode)
		{
			tempa->GameElements.Unit->gameelement->visible = FALSE;
		}
		for (tempb = map->unitlistb->headNode; tempb; tempb = tempb->nextNode)
		{
			tempb->GameElements.Unit->gameelement->visible = FALSE;
		}
		for (tempa = map->unitlista->headNode; tempa; tempa = tempa->nextNode)
		{
			for (tempb = map->unitlistb->headNode; tempb; tempb = tempb->nextNode)
			{
				//double distance = Point_Length(&tempa->GameElements.Unit->gameelement->pos, &tempb->GameElements.Unit->gameelement->pos);
				if ((tempa->GameElements.Unit->gameelement->alive && Unit_PointIsInVision(tempa->GameElements.Unit, &tempb->GameElements.Unit->gameelement->pos))
					|| ((tempb->GameElements.Unit->gameelement->alive && Unit_PointIsInVision(tempb->GameElements.Unit, &tempa->GameElements.Unit->gameelement->pos))))
				{
					Line* l = Line_Create(&tempa->GameElements.Unit->gameelement->pos, &tempb->GameElements.Unit->gameelement->pos);
					Node* n = Node_Create();
					n->Geom.Line = l;
					NodeList_AddNode(&lines, n);
				}
			}
		}
		Line_CollidesBatch(NULL, coll, NULL, NULL, NULL, NULL, NULL, NULL, &lines, map->linelist, TRUE);
		for (index = 0, tempa = map->unitlista->headNode; tempa; tempa = tempa->nextNode)
		{
			for (tempb = map->unitlistb->headNode; tempb; tempb = tempb->nextNode)
			{
				//double distance = Point_Length(&tempa->GameElements.Unit->gameelement->pos, &tempb->GameElements.Unit->gameelement->pos);
				int nextindex = index + 1; //how 2 solve fizbuz hire me pls
				if (tempa->GameElements.Unit->gameelement->alive && Unit_PointIsInVision(tempa->GameElements.Unit, &tempb->GameElements.Unit->gameelement->pos))
				{
					if (!coll[index])
					{
						tempb->GameElements.Unit->gameelement->visible = TRUE;
					}
					index = nextindex;
				}
				if (tempb->GameElements.Unit->gameelement->alive &&Unit_PointIsInVision(tempb->GameElements.Unit, &tempa->GameElements.Unit->gameelement->pos))
				{
					if (!coll[index])
					{
						tempa->GameElements.Unit->gameelement->visible = TRUE;
					}
					index = nextindex;
				}
			}
		}
		free(coll);
		NodeList_Clear(&lines, TRUE);
		map->losupdatetimer = 0;
	}
}
void Map_UpdateFOW(Map* map, double frametime)
{
	//TODO sdl surface efficiency
	Node* temp;
	map->fowcleartimer += frametime * FOW_CLEAR_SPEED;
	map->fowupdatetimer += frametime * FOW_UPDATE_SPEED;
	if (map->fowcleartimer >= 1)
	{
		SetArray(map->fowmapa->booleangrid, NULL, map->fowmapa->dim.x * map->fowmapa->dim.y, FALSE);
		SetArray(map->fowmapb->booleangrid, NULL, map->fowmapb->dim.x * map->fowmapb->dim.y, FALSE);
		map->fowcleartimer = 0;
	}
	if (map->fowupdatetimer >= 1 || map->fowcleartimer == 0)
	{
		for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
		{
			if (!(temp->flags & NFlag_Dead))
			{
				Map_UpdateFOWForUnit(map, temp->GameElements.Unit, map->fowmapa);
			}
		}
		for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
		{
			if (!(temp->flags & NFlag_Dead))
			{
				Map_UpdateFOWForUnit(map, temp->GameElements.Unit, map->fowmapb);
			}
		}
		map->fowupdatetimer = 0;
	}
}
void Map_UpdateFOWForUnit(Map* map, Unit* unit, Grid* fowmap)
{
	int x, y;
	Point unitgridspace;
	Map_MapToGrid(&unitgridspace, &unit->gameelement->pos, map);
	Point_ToInt(&unitgridspace, &unitgridspace);
	for (x = Max(unitgridspace.x - unit->visionrange * MAP_TO_GRID_RATIO, 0); x < Min(unitgridspace.x + unit->visionrange * MAP_TO_GRID_RATIO, fowmap->dim.x); x++)
	{
		for (y = Max(unitgridspace.y - unit->visionrange * MAP_TO_GRID_RATIO, 0); y < Min(unitgridspace.y + unit->visionrange * MAP_TO_GRID_RATIO, fowmap->dim.y); y++)
		{
			Point p = (Point) { x, y, 0 };
			if (!fowmap->booleangrid[Grid_PointerArithmetic(&fowmap->dim, &p)])//if not already visible
			{
				Point gridspace, mapspace;
				Point_Multiply(&gridspace, &p, 1. / FOW_RESOLUTION);
				Point_Add(&gridspace, &gridspace, &HALF_POINT_2D);
				Map_GridToMap(&mapspace, &gridspace, map);
				if (Unit_PointIsInVision(unit, &mapspace))
				{
					fowmap->booleangrid[Grid_PointerArithmetic(&fowmap->dim, &p)] = TRUE;
				}
			}
		}
	}
	fowmap->booleangrid[Grid_PointerArithmetic(&fowmap->dim, &unitgridspace)] = TRUE;
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
	for (x = 0; x < map->dim.x * HEIGHTMAP_RESOLUTION; x++)
	{
		for (y = 0; y < map->dim.y * HEIGHTMAP_RESOLUTION; y++)
		{
			Point screenspace, mapspace;
			SDL_Rect dstrect;
			double val = map->heightmap->doublegrid[Grid_PointerArithmetic(&map->heightmap->dim, &(Point){ x, y, 0 })];
			//double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0; //non-linear lighting
			double alpha = val != 0 ? 255 * ((int)val % 100) / 100 : 0;
			Map_GridToMap(&mapspace, &(Point){(double)x / HEIGHTMAP_RESOLUTION, (double)y / HEIGHTMAP_RESOLUTION, 0}, map);
			Camera_MapToScreen(&screenspace, camera, &mapspace); //top down viwe
			//Camera_MapToScreen(&screenspace, camera, &(Point){x * map->mapdim.x / map->dim.x / HEIGHTMAP_RESOLUTION + map->pos.x, -val, 0}); alpha = 255; //side view
			dstrect.x = screenspace.x;
			dstrect.y = screenspace.y;
			dstrect.w = map->mapdim.x / map->dim.x / HEIGHTMAP_RESOLUTION * camera->scale;
			dstrect.h = map->mapdim.y / map->dim.y / HEIGHTMAP_RESOLUTION * camera->scale;
			SDL_SetTextureAlphaMod(dfieldtexture, 255);
			SDL_SetTextureColorMod(dfieldtexture, alpha, alpha, alpha);
			SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		}
	}
	/*
#define X(a) DiffusionField_Draw(renderer, map->a, camera);
	MAP_DFIELD_MAP(X)
#undef X
	*/
	for (temp = map->unitlista->headNode; temp; temp = temp->nextNode)
	{
		Unit_Draw(renderer, temp->GameElements.Unit, camera);
	}
	for (temp = map->unitlistb->headNode; temp; temp = temp->nextNode)
	{
		if (temp->flags & NFlag_Visible)
		{
			Unit_Draw(renderer, temp->GameElements.Unit, camera);
		}
	}
	
	for (x = 0; x < map->fowmapa->dim.x; x++)
	{
		for (y = 0; y < map->fowmapa->dim.y; y++)
		{
			Point screenspace, mapspace;
			SDL_Rect dstrect;
			BOOLEAN val = map->fowmapa->booleangrid[Grid_PointerArithmetic(&map->fowmapa->dim, &(Point){ x, y, 0 })];
			//double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0; //non-linear lighting
			double alpha = val ? 0 : 255;
			Map_GridToMap(&mapspace, &(Point){(double)x / FOW_RESOLUTION, (double)y / FOW_RESOLUTION, 0}, map);
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

/*
unit is not moving or is pathing to the grid point that it is already on
*/
BOOLEAN Unit_IsStationary(Unit* unit, Map* map)
{
	Point posgridspace, pathingpointgridspace;
	if (Point_IsEqual(&unit->gameelement->movevec, &(Point){0, 0, 0}))
	{
		return TRUE;
	}
	Map_MapToGrid(&posgridspace, &unit->gameelement->pos, map);
	Point_ToInt(&posgridspace, &posgridspace);
	Map_MapToGrid(&pathingpointgridspace, &unit->pathingpoint, map);
	Point_ToInt(&pathingpointgridspace, &pathingpointgridspace);
	return Point_IsEqual(&posgridspace, &pathingpointgridspace) ? TRUE : FALSE;
}
/*
unit has moved from one grid space to another
*/
BOOLEAN Unit_HasMoved(Unit* unit, Map* map)
{
	Point posgridspace, lastposgridspace;
	Map_MapToGrid(&posgridspace, &unit->gameelement->pos, map);
	Point_ToInt(&posgridspace, &posgridspace);
	Map_MapToGrid(&lastposgridspace, &unit->gameelement->lastpos, map);
	Point_ToInt(&lastposgridspace, &lastposgridspace);
	return (!Point_IsEqual(&posgridspace, &lastposgridspace)) ? TRUE : FALSE;
}
void Unit_OnDeath(Unit* unit, Map* map)
{
	unit->attacktarget = NULL;
}