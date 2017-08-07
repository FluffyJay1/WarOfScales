#include "UIElement.h"
#include "LinkedList.h"
#include "Game.h"

void UIElement_Init(UIElement* element)
{
	element->nullptr = NULL;
	element->class = Basic;
	element->flags = 0;
	element->frame = 0;
	element->outlineWidth = 1;
	element->play = FALSE;
	element->refreshTexture = TRUE;
	element->visible = TRUE;
	element->pressed = FALSE;
	element->breakMouse = TRUE;
	element->outline = FALSE;
	element->frametimer = 0;
	element->animationspeed = 1;
	element->pos = *Point_Create(0, 0, 0);
	element->lastpos = *Point_Create(0, 0, 0);
	element->dim = *Point_Create(0, 0, 0);
	element->framedim = *Point_Create(1, 1, 0);
	element->texturedim = *Point_Create(0, 0, 0);
	element->outlineColor = (SDL_Color){ 255, 255, 255 };
	element->children = malloc(sizeof(NodeList));
	NodeList_Init(element->children);
	element->parent = NULL;
	element->texture = NULL;
	element->path = NULL;
}
UIElement* UIElement_Create(int class, int flags, Point* pos, Point* dim, char* path)
{
	UIElement *element = malloc(sizeof(UIElement));
	UIElement_Init(element);
	element->class = class;
	element->flags = flags;
	element->pos = *pos;
	Point_Copy(&element->lastpos, pos);
	element->dim = *dim;
	element->path = path;
	return element;
}
//deprecated, don't use this
void UIElement_Set(UIElement* element, int class, int flags, Point pos, Point dim, char* path)
{
	element->class = class;
	element->flags = flags;
	element->frame = 0;
	element->pos = pos;
	element->dim = dim;
	element->children = malloc(sizeof(NodeList));
	NodeList_Init(element->children);
	element->parent = NULL;
	element->texture = NULL;
	element->path = path;
}
void UIElement_SetTexture(UIElement* element, char* path)
{
	element->path = path;
	element->refreshTexture = TRUE;
	element->frame = 0;
	element->frametimer = 0;
}
void UIElement_EnableFlags(UIElement* element, int flags)
{
	element->flags = element->flags | flags;
}
void UIElement_DisableFlags(UIElement* element, int flags)
{
	element->flags = element->flags & (~0 - flags);
}
void UIElement_ToggleFlags(UIElement* element, int flags)
{
	element->flags = element->flags ^ flags;
}
void UIElement_Destroy(UIElement* element)
{
	Node* temp = element->children->headNode;
	while (temp != NULL)
	{
		UIElement_Destroy(temp->UIElement);
	}
	free(element);
}
void UIElement_GetFinalPos(Point* out, UIElement* element)
{
	if (element->parent != NULL)
	{
		Point* parentfinal = malloc(sizeof(Point));
		UIElement_GetFinalPos(parentfinal, element->parent);
		Point_Add(out, parentfinal, &element->pos);
		free(parentfinal);
	}
	else
	{
		Point_Copy(out, &element->pos);
	}
}
void UIElement_Parent(UIElement* parent, UIElement* child)
{
	if (parent != NULL && child != NULL)
	{
		if (parent->parent != child)
		{
			if (child->parent != parent)
			{
				if (child->parent == NULL)
				{
					if (child->parent != child)
					{
						Node* n = Node_Create();
						n->UIElement = child;
						NodeList_AddNode(parent->children, n);
						child->parent = parent;
					}
					else
					{
						printf("Error in function UIElement_Parent: tried to parent an element to itself!\n");
					}
				}
				else
				{
					Node* n = Node_Create();
					Node* oldNode;
					n->UIElement = child;
					NodeList_FindUIElement(&oldNode, child->parent->children, child);
					NodeList_RemoveNode(child->parent->children, oldNode, FALSE);
					NodeList_AddNode(parent->children, n);
					child->parent = parent;
					printf("Warning in function UIElement_Parent: child already has different parent!\n");
				}
			}
			else
			{
				printf("Error in function UIElement_Parent: tried to parent again to the same parent!\n");
			}
		}
		else
		{
			printf("Error in function UIElement_Parent: tried to parent an element to its child!\n");
		}
	}
	else
	{
		printf("Error in function UIElement_Parent: either parent or child was null!\n");
	}
}
void UIElement_HandleEvents(UIElement* element, SDL_Event* event)
{
	switch (event->type) {
	case SDL_MOUSEMOTION:
		if (element->pressed && element->flags & draggable && mousestate & SDL_BUTTON(SDL_BUTTON_LEFT)) //draggable
		{
			element->pos.x += event->motion.xrel;
			element->pos.y += event->motion.yrel;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (UIElement_SDL_PointIsInside(element, &mousepos) && event->button.button == SDL_BUTTON_LEFT) //for both dragging and button
		{
			element->pressed = TRUE;
			if (element->flags & button)
			{
				element->frame = 2;
				UIElement_OnButtonPress(element);
			}
			Game_BreakLoop = TRUE; //break out of Game_HandleEvent loop
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			element->pressed = FALSE;
			if (element->flags & button) //button
			{
				if (UIElement_SDL_PointIsInside(element, &mousepos))
				{
					element->frame = 1;
				}
				else
				{
					element->frame = 0;
				}
			}
		}
		break;
	}
}
void UIElement_Update(UIElement* element, double frametime)
{
	UIElement_UpdateAnimation(element, frametime);
	
}
void UIElement_UpdateReverse(UIElement* element)
{
	if (element->flags & button) { //button
		if (element->pressed == FALSE)
		{
			if (UIElement_SDL_PointIsInside(element, &mousepos) && Game_BreakMouse == FALSE)
			{
				element->frame = 1;
			}
			else
			{
				element->frame = 0;
			}
		}
	}
	if (UIElement_SDL_PointIsInside(element, &mousepos) && element->breakMouse) //break mouse
	{
		Game_BreakMouse = TRUE;
	}
}
void UIElement_UpdateAnimation(UIElement* element, double frametime)
{
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
void UIElement_UpdateLastPos(UIElement* element)
{
	Point_Copy(&element->lastpos, &element->pos);
}
void UIElement_UpdateParentRelationship(UIElement* element)
{
	/*
	if (element->children->listSize > 0)
	{
		Point diff;
		Node* temp = element->children->headNode;
		Point_Subtract(&diff,&element->pos, &element->lastpos);
		while (temp != NULL)
		{
			Point_Add(&temp->UIElement->pos, &temp->UIElement->pos, &diff);
			temp = temp->nextNode;
		}
	}
	UIElement_UpdateLastPos(element);
	*/
}
void UIElement_Message(UIElement* element, char* message)
{

}
void UIElement_OnButtonPress(UIElement* element)
{

}
void UIElement_Draw(SDL_Renderer* renderer, UIElement* element) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	Point finalpos;
	UIElement_GetFinalPos(&finalpos, element);
	dstrect.x = finalpos.x - element->dim.x / 2;
	dstrect.y = finalpos.y - element->dim.y / 2;
	dstrect.w = element->dim.x;
	dstrect.h = element->dim.y;
	if (element->texture == NULL || element->refreshTexture)
	{
		Game_GetTexture(&element->texture, &element->framedim, &element->texturedim, renderer, element->path);
	}
	srcrect.x = (element->frame % Round(element->framedim.x)) * element->texturedim.x / element->framedim.x;
	srcrect.y = (int)(element->frame / element->framedim.y) * element->texturedim.y / element->framedim.y;
	srcrect.w = (element->frame % Round(element->framedim.x) + 1) * element->texturedim.x / element->framedim.x;
	srcrect.h = (int)(element->frame / element->framedim.y + 1) * element->texturedim.y / element->framedim.y;
	SDL_RenderCopy(renderer, element->texture, &srcrect, &dstrect); //render different frames
	if (element->outline && element->outlineWidth > 0)
	{
		UIElement_DrawOutline(renderer, element);
	}
}
void UIElement_DrawOutline(SDL_Renderer* renderer, UIElement* element)
{
	Point finalpos;
	UIElement_GetFinalPos(&finalpos, element);
	SDL_SetRenderDrawColor(renderer, element->outlineColor.r, element->outlineColor.g, element->outlineColor.b, element->outlineColor.a);
	if (element->flags & circular)
	{
		int i;
		for (i = -element->outlineWidth - element->dim.y/2; i <= element->outlineWidth + element->dim.y/2; i++) {
			double delta = (element->dim.x/2 + element->outlineWidth) * sqrt(1 - pow(i/(element->dim.y/2 + element->outlineWidth), 2));
			if (abs(i) <= element->dim.y / 2)
			{
				double delta2 = element->dim.x / 2 * sqrt(1 - pow(i / (element->dim.y / 2), 2));
				SDL_RenderDrawLine(renderer, finalpos.x - delta, finalpos.y + i, finalpos.x - delta2, finalpos.y + i);
				SDL_RenderDrawLine(renderer, finalpos.x + delta2, finalpos.y + i, finalpos.x + delta, finalpos.y + i);
			}
			else
			{
				SDL_RenderDrawLine(renderer, finalpos.x - delta, finalpos.y + i, finalpos.x + delta, finalpos.y + i);
			}
		}
	}
	else
	{
		int i;
		for (i = -element->outlineWidth - element->dim.y / 2; i <= element->outlineWidth + element->dim.y / 2; i++) {
			if (abs(i) <= element->dim.y / 2)
			{
				SDL_RenderDrawLine(renderer, finalpos.x - element->dim.x / 2 - element->outlineWidth, finalpos.y + i, finalpos.x - element->dim.x / 2, finalpos.y + i);
				SDL_RenderDrawLine(renderer, finalpos.x + element->dim.x / 2, finalpos.y + i, finalpos.x + element->dim.x / 2 + element->outlineWidth, finalpos.y + i);
			}
			else
			{
				SDL_RenderDrawLine(renderer, finalpos.x - element->dim.x / 2 - element->outlineWidth, finalpos.y + i, finalpos.x + element->dim.x / 2 + element->outlineWidth, finalpos.y + i);
			}
		}
	}
}
BOOLEAN UIElement_SDL_PointIsInside(UIElement* element, SDL_Point* p)
{
	Point finalpos;
	UIElement_GetFinalPos(&finalpos, element);
	if (element->flags & circular)
	{
		return (p->x - finalpos.x) * (p->x - finalpos.x) / (element->dim.x * element->dim.x / 4) + (p->y - finalpos.y) * (p->y - finalpos.y) / (element->dim.y * element->dim.y / 4) <= 1 ? TRUE : FALSE;
	}
	else
	{
		return (p->x >= finalpos.x - element->dim.x / 2 && p->x <= finalpos.x + element->dim.x / 2 && p->y >= finalpos.y - element->dim.y / 2 && p->y <= finalpos.y + element->dim.y / 2) ? TRUE : FALSE;
	}
}