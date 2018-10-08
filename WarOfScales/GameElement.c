#include "GameElement.h"


void GameElement_Init(GameElement* element)
{
	element->nullptr = NULL;
	element->class = GEClass_Base;
	element->flags = 0;
	element->frame = 0;
	element->team = GETeam_A;
	element->play = FALSE;
	element->refreshTexture = TRUE;
	element->visible = TRUE;
	element->alive = TRUE;
	element->remove = FALSE;
	element->frametimer = 0;
	element->animationspeed = 1;
	element->thinktimer = 0;
	element->thinkspeed = 1;
	element->movespeed = 0;
	element->hp = 1;
	element->maxhp = 1;
	element->pos = (Point) { 0, 0, 0 };
	element->movevec = (Point) { 0, 0, 0 };
	element->lastpos = (Point) { 0, 0, 0 };
	element->dim = (Point) { 0, 0, 0 };
	element->framedim = (Point) { 1, 1, 0 };
	element->texturedim = (Point) { 0, 0, 0 };
	element->path = NULL;
	element->texture = NULL;
	element->UIElement = NULL;
}
GameElement* GameElement_Create(int class, int flags, Point* pos, Point* dim, char* path)
{
	GameElement* element = malloc(sizeof(GameElement));
	GameElement_Init(element);
	element->class = class;
	element->flags = flags;
	Point_Copy(&element->pos, pos);
	Point_Copy(&element->dim, dim);
	element->path = path;
	return element;
}
void GameElement_EnableFlags(GameElement* element, int flags)
{
	element->flags = element->flags | flags;
}
void GameElement_DisableFlags(GameElement* element, int flags)
{
	element->flags = element->flags &(~0 - flags);
}
void GameElement_ToggleFlags(GameElement* element, int flags)
{
	element->flags = element->flags ^ flags;
}
void GameElement_Destroy(GameElement* element)
{
	/* commented out because game elements are never parented
	Node* temp;
	for (temp = element->children->headNode; temp; temp = temp->nextNode)
	{
		GameElement_Destroy(temp->GameElement); //recurse
	}
	NodeList_Clear(element->children, FALSE); //free the nodes
	free(element->children); //free the list
	*/
	free(element); //free the element
	//free your mind
}
//think timer, animation, and movement
void GameElement_Update(GameElement* element, double frametime)
{
	Point_Copy(&element->lastpos, &element->pos);
	if (element->alive)
	{
		if (element->flags & GEFlags_Thinking)
		{
			element->thinktimer += frametime * element->thinkspeed;
		}
		if (element->flags & GEFlags_Moving)
		{
			Point move;

			Point_Multiply(&move, &element->movevec, element->movespeed * frametime);
			Point_Add(&element->pos, &element->pos, &move);
		}
	}
	if (element->play == TRUE)
	{
		element->frametimer += frametime * element->animationspeed;
		while (element->frametimer >= 1)
		{
			element->frame++;
			element->frametimer -= 1;
		}
		element->frame = element->frame % Round(element->framedim.x * element->framedim.y);
	}
}
void GameElement_Draw(SDL_Renderer* renderer, GameElement* element, Camera* camera)
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	Point screenspace;
	Camera_MapToScreen(&screenspace, camera, &element->pos);
	dstrect.x = screenspace.x - element->dim.x * camera->scale / 2;
	dstrect.y = screenspace.y - element->dim.y * camera->scale / 2;
	dstrect.w = element->dim.x * camera->scale;
	dstrect.h = element->dim.y * camera->scale;
	if (element->texture == NULL || element->refreshTexture)
	{
		Game_GetTexture(&element->texture, &element->framedim, &element->texturedim, renderer, element->path);
	}
	srcrect.x = (element->frame % Round(element->framedim.x)) * element->texturedim.x / element->framedim.x;
	srcrect.y = (int)(element->frame / element->framedim.y) * element->texturedim.y / element->framedim.y;
	srcrect.w = (element->frame % Round(element->framedim.x) + 1) * element->texturedim.x / element->framedim.x;
	srcrect.h = (int)(element->frame / element->framedim.y + 1) * element->texturedim.y / element->framedim.y;
	SDL_RenderCopy(renderer, element->texture, &srcrect, &dstrect); //render different frames
}