#include "Unit.h"

void Unit_Init(Unit* unit)
{
	unit->visionrange = 1000;
	unit->visionradianscos = cos(PI/2);
	unit->attacktimer = 0;
	unit->attackspeed = 1;
	unit->attackdamage = 1;
	unit->maxattackrange = 200;
	unit->baseattackrange = 100;
	unit->attackrangeheightcoeff = 5;
	unit->pathweight = 1;
	unit->fireweight = 0.5;
	unit->pathsearchradius = 64;
	unit->offenselambda = 1;
	unit->defenselambda = 0.5;
	unit->offenseweight = 100;
	unit->defenseweight = -1;
	unit->pathingtimer = 0;
	unit->aggrotimer = 0;
	unit->aggrospeed = 4;
	unit->pathpoints = NULL;
	unit->aimvec = (Point) { 1, 0, 0 };
	unit->pathingpoint = (Point) { 0, 0, 0 };
#define X(a) unit->a = NULL;
	UNIT_DFIELD_MAP(X)
#undef X
	unit->collision.hulltype = CHType_Circle;
	unit->collision.circleRadius = 10;
	unit->collision.polygon = NULL;
	unit->attacktarget = NULL;
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
	/*
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
	*/
	Point enemyscreenspace, elementscreenspace;
	
	Camera_MapToScreen(&elementscreenspace, camera, &unit->gameelement->pos);
	if (unit->attacktarget)
	{
		Camera_MapToScreen(&enemyscreenspace, camera, &unit->attacktarget->gameelement->pos); //top down view
		switch (unit->gameelement->team) {
		case GETeam_A:
			SDL_SetRenderDrawColor(renderer, 10, 170, 0, 100);
			break;
		case GETeam_B:
			SDL_SetRenderDrawColor(renderer, 170, 80, 0, 100);
			break;
		}
		SDL_RenderDrawLine(renderer, elementscreenspace.x, elementscreenspace.y, enemyscreenspace.x, enemyscreenspace.y);
	}
	
	GameElement_Draw(renderer, unit->gameelement, camera);
	if (unit->gameelement->hp < unit->gameelement->maxhp && unit->gameelement->alive)
	{
		Unit_DrawHP(renderer, unit, camera);
	}
	//SDL_RenderDrawLine(renderer, elementscreenspace.x, elementscreenspace.y, screenspace.x, screenspace.y);
}
void Unit_DrawHP(SDL_Renderer* renderer, Unit* unit, Camera* camera)
{
	Point hpbarp1, hpbarp2;
	int i;
	//mapspace
	Point_Add(&hpbarp1, &unit->gameelement->pos, &(Point){-UNIT_HP_BAR_WIDTH / 2 * unit->gameelement->hp / unit->gameelement->maxhp, UNIT_HP_BAR_OFFSET, 0});
	Point_Add(&hpbarp2, &unit->gameelement->pos, &(Point){UNIT_HP_BAR_WIDTH / 2 * unit->gameelement->hp / unit->gameelement->maxhp, UNIT_HP_BAR_OFFSET, 0});
	//transition to screenspace
	Camera_MapToScreen(&hpbarp1, camera, &hpbarp1);
	Camera_MapToScreen(&hpbarp2, camera, &hpbarp2);
	switch (unit->gameelement->team) {
	case GETeam_A:
		SDL_SetRenderDrawColor(renderer, 10, 170, 0, 100);
		break;
	case GETeam_B:
		SDL_SetRenderDrawColor(renderer, 170, 80, 0, 100);
		break;
	}
	
	for (i = 0; i < UNIT_HP_BAR_HEIGHT * camera->scale; i++)
	{
		SDL_RenderDrawLine(renderer, hpbarp1.x, hpbarp1.y + i, hpbarp2.x, hpbarp2.y + i);
	}
}
void Unit_Update(Unit* unit, double frametime)
{
	Unit_UpdateMove(unit, frametime);
	GameElement_Update(unit->gameelement, frametime);
	if (unit->gameelement->alive)
	{
		while (unit->gameelement->thinktimer >= 1)
		{
			Unit_Think(unit);
			unit->gameelement->thinktimer -= 1;
		}
		if (unit->aggrotimer >= 1)
		{
			unit->aggrotimer = 0;
		}
		unit->aggrotimer += frametime * unit->aggrospeed; //this order is intentional to allow map to see if the unit needs to aggro
		if (unit->attacktarget)
		{
			unit->attacktarget->gameelement->hp -= frametime;
		}
		if (!Point_IsEqual(&unit->gameelement->movevec, &(Point){0, 0, 0}))
		{
			Point_Copy(&unit->aimvec, &unit->gameelement->movevec);
		}
		unit->pathingtimer += frametime;
	}
	else
	{
		unit->gameelement->frame = 2;
	}
}
void Unit_UpdateMove(Unit* unit, double frametime)
{
	if (!unit->gameelement->alive || unit->attacktarget)
	{
		unit->gameelement->movevec = (Point) { 0, 0, 0 };
	}
	else
	{
		Point_Subtract(&unit->gameelement->movevec, &unit->pathingpoint, &unit->gameelement->pos);
		unit->gameelement->movevec.z = 0; //make 2d
		Point_Normalize(&unit->gameelement->movevec, &unit->gameelement->movevec);
	}
	
	//unit->gameelement->movevec = (Point){ 1, 0, 0 };
}
void Unit_Think(Unit* unit)
{
	
}
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
	return unit->visionradianscos < dot ? TRUE : FALSE;
}
BOOLEAN Unit_PointIsInAttackRange(Unit* unit, Point* mapspace)
{
	double distance = Point_Length2D(&unit->gameelement->pos, mapspace);
	if (distance > unit->maxattackrange)
	{
		return FALSE;
	}
	double heightdiff = mapspace->z - unit->gameelement->pos.z;
	if (distance > unit->baseattackrange - heightdiff * unit->attackrangeheightcoeff)
	{
		return FALSE;
	}
	return TRUE;
}