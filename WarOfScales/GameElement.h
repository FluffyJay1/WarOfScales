#pragma once
#include "LinkedList.h"
#include "Point.h"
#include "Camera.h"

enum GameElementClass
{
	GEClass_Base,
	GEClass_Unit,
};
enum GameElementFlags
{
	GEFlags_Thinking = 1,
	GEFlags_Moving = 2
};
enum GameElementTeam
{
	GETeam_A,
	GETeam_B,
};
//Handles position, movement, drawing
typedef struct GameElement {
	int* nullptr;
	int class, flags, frame, team;
	BOOLEAN play, refreshTexture, visible, alive, remove; //alive and remove are different because there is possibility of HEROES NEVER DIE happening
	double frametimer, animationspeed, thinktimer, thinkspeed, movespeed;
	Point pos, movevec, lastpos, dim, framedim, texturedim;
	char* path;
	SDL_Texture* texture;
	struct UIElement* UIElement;
	/* commented out because game elements do not need parenting
	struct GameElement* parent;
	NodeList* children;
	*/
	union Sub {
		struct Unit* unit;
	}sub;
}GameElement;

void GameElement_Init(GameElement* element);
GameElement* GameElement_Create(int class, int flags, Point* pos, Point* dim, char* path);
void GameElement_EnableFlags(GameElement* element, int flags);
void GameElement_DisableFlags(GameElement* element, int flags);
void GameElement_ToggleFlags(GameElement* element, int flags);
void GameElement_Destroy(GameElement* element);
void GameElement_Update(GameElement* element, double frametime);
void GameElement_Draw(SDL_Renderer* renderer, GameElement* element, Camera* camera);