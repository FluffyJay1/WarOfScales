#include "Unit.h"

void Unit_Init(Unit* unit)
{
	unit->visionrange = 100;
	unit->visionradians = PI;
	unit->attacktimer = 0;
	unit->attackspeed = 1;
	unit->attackdamage = 1;
	unit->maxattackrange = 200;
	unit->baseattackrange = 100;
	unit->attackrangeheightcoeff = 1;
	unit->pathweight = 1;
	unit->fireweight = 0.5;
	unit->pathsearchradius = 64;
	unit->offenseweight = 100;
	unit->defenseweight = 0;
	unit->pathpoints = NULL;
	unit->aimvec = (Point) { 1, 0, 0 };
	unit->pathingpoint = (Point) { 0, 0, 0 };
#define X(a) unit->a = NULL;
	UNIT_DFIELD_MAP(X)
#undef X
	unit->collision.hulltype = CHType_Circle;
	unit->collision.circleRadius = 10;
	unit->collision.polygon = NULL;
}
Unit* Unit_Create(int flags, Point* pos, Point* dim, char* path)
{
	Unit* unit = malloc(sizeof(Unit));
	Unit_Init(unit);
	unit->pathpoints = NodeList_Create();
	unit->gameelement = GameElement_Create(GEClass_Unit, flags, pos, dim, path);
	return unit;
}
void Unit_SetToData(Unit* unit, char* data, char** returnpointer);
void Unit_Destroy(Unit* unit)
{
	GameElement_Destroy(unit->gameelement);
	if (unit->collision.polygon != NULL)
	{
		free(unit->collision.polygon);
	}
	if (unit->pathpoints)
	{
		NodeList_Destroy(unit->pathpoints);
	}
	free(unit);
}
void Unit_Draw(SDL_Renderer* renderer, Unit* unit, Camera* camera)
{
	/*
	Node* temp;
	for (temp = unit->pathpoints->headNode; temp; temp = temp->nextNode)
	{
		Point screenspace, elementscreenspace, mapspace;
		SDL_Rect dstrect;
		//Point_Add(&mapspace, temp->Geom.Point, &HALF_POINT_2D);
		Point_Multiply(&mapspace, temp->Geom.Point, 8);
		Point_Add(&mapspace, &mapspace, &unit->gameelement->pos);
		Camera_MapToScreen(&screenspace, camera, &mapspace); //top down view
		Camera_MapToScreen(&elementscreenspace, camera, &unit->gameelement->pos);
		dstrect.x = screenspace.x - 4;
		dstrect.y = screenspace.y - 4;
		dstrect.w = 8;
		dstrect.h = 8;
		SDL_SetTextureAlphaMod(dfieldtexture, 255);
		switch ((int)temp->val)
		{
		case 69:
			SDL_SetTextureColorMod(dfieldtexture, 255, 0, 0);
			break;
		case 420:
			SDL_SetTextureColorMod(dfieldtexture, 0, 0, 255);
			break;
		default:
			SDL_SetTextureAlphaMod(dfieldtexture, 0);
			break;
		}
		SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		//SDL_RenderDrawLine(renderer, elementscreenspace.x, elementscreenspace.y, screenspace.x, screenspace.y);
	}
	*/
	Point screenspace, elementscreenspace;
	SDL_Rect dstrect;
	Camera_MapToScreen(&screenspace, camera, &unit->pathingpoint); //top down view
	Camera_MapToScreen(&elementscreenspace, camera, &unit->gameelement->pos);
	dstrect.x = screenspace.x - 4;
	dstrect.y = screenspace.y - 4;
	dstrect.w = 8;
	dstrect.h = 8;
	SDL_SetTextureAlphaMod(dfieldtexture, 255);
	SDL_SetTextureColorMod(dfieldtexture, 0, 255, 0);
	SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
	
	GameElement_Draw(renderer, unit->gameelement, camera);
	SDL_RenderDrawLine(renderer, elementscreenspace.x, elementscreenspace.y, screenspace.x, screenspace.y);
}
void Unit_Update(Unit* unit, double frametime)
{
	Unit_UpdateMove(unit, frametime);
	GameElement_Update(unit->gameelement, frametime);
	while (unit->gameelement->thinktimer >= 1)
	{
		Unit_Think(unit);
		unit->gameelement->thinktimer -= 1;
	}
	Point_Copy(&unit->aimvec, &unit->gameelement->movevec);
}
void Unit_UpdateMove(Unit* unit, double frametime)
{
	Point_Subtract(&unit->gameelement->movevec, &unit->pathingpoint, &unit->gameelement->pos);
	unit->gameelement->movevec.z = 0; //make 2d
	Point_Normalize(&unit->gameelement->movevec, &unit->gameelement->movevec);
}
void Unit_Think(Unit* unit)
{
	
}
void Unit_UpdateTarget(Unit* unit, double frametime, NodeList* unitlist);
void Unit_AcquireTarget(Unit* unit, NodeList* unitlist);
BOOLEAN Unit_PointIsInVision(Unit* unit, Point* mapspace)
{
	Point vec;
	double dot;
	if (Point_LengthSquared2D(mapspace, &unit->gameelement->pos) > unit->visionrange * unit->visionrange)
	{
		return FALSE;
	}
	Point_Subtract(&vec, mapspace, &unit->gameelement->pos);
	vec.z = 0;//make 2d
	Point_Normalize(&vec, &vec);
	Point_Dot(&dot, &unit->aimvec, &vec);
	return cos(unit->visionradians / 2) < dot ? TRUE : FALSE;
}