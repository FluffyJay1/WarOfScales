#pragma once
#include "GameElement.h"
#include "DiffusionField.h"

#define UNIT_DFIELD_MAP(X) \
X(dfield_path, pathweight) \
X(dfield_fire, fireweight) 

typedef struct Unit {
	GameElement* gameelement; //imagine this as the super class
	double visionrange, visionradians, attacktimer, attackspeed, attackdamage, maxattackrange, baseattackrange, attackrangeheightcoeff,
		pathweight, fireweight, pathsearchradius, offenseweight, defenseweight;
	//pathweight and fireweight are each unit's weights for pathfinding, offenseweight and defenseweight are their impacts on the dfield
	NodeList* pathpoints; //calculated by map, point is position
	Point aimvec, pathingpoint;
	DiffusionField *dfield_path, *dfield_fire;
	CollisionHull collision;
	struct Unit* attacktarget;
}Unit;

void Unit_Init(Unit* unit);
Unit* Unit_Create(int flags, Point* pos, Point* dim, char* path);
void Unit_SetToData(Unit* unit, char* data, char** returnpointer);
void Unit_Destroy(Unit* unit);
void Unit_Draw(SDL_Renderer* renderer, Unit* unit, Camera* camera);
void Unit_Update(Unit* unit, double frametime);
void Unit_UpdateMove(Unit* unit, double frametime);
void Unit_Think(Unit* unit);
void Unit_UpdateTarget(Unit* unit, double frametime, NodeList* unitlist);
void Unit_UpdatePathweights(Unit* unit, NodeList* lines);
//void Unit_CheckMoveCollision(Unit* unit, NodeList* lines);//BATCH LINE COLLIDES
void Unit_AcquireTarget(Unit* unit, NodeList* unitlist);
BOOLEAN Unit_PointIsInVision(Unit* unit, Point* mapspace);