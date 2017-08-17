#pragma once
#include "LinkedList.h"
#include "Point.h"
#include "DiffusionField.h"

enum GameElementClass
{
	BasicSoldier,
};
enum GameElementFlags
{

};

typedef struct GameElement {
	int* nullptr;
	int class, flags, frame;
	BOOLEAN play, refreshTexture, visible, alive;
	double frametimer, animationspeed, thinktimer, thinkspeed, visionrange, movespeed, attackspeed, attackdamage, maxattackrange, baseattackrange, attackrangeheightcoeff,
		dmod, dmodrange;
	Point pos, movedir, lastpos, dim, framedim, texturedim;
	char* path;
	SDL_Texture* texture;
	DiffusionField* dfield_defened;
	DiffusionField* dfield_attack;
	struct GameElement* attacktarget;
	struct UIElement* UIElement;
	// do we even need parenting for game elements
	struct GameElement* parent;
	NodeList* children;
};