#include "Game.h"
#include "SDL.h"
#include "UIElement.h"
#include "Utilities.h"
#include <string.h>
#include "Point.h"
#include <math.h>
#include <stdio.h>

NodeList TextureList;
SDL_Surface *image;
DiffusionField *fuck;
void Game_Init()
{
	int i;
	UIElement *anim = UIElement_Create(Basic, button + draggable + circular, &(Point) { 800, 450, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
	UIElement *anim2 = UIElement_Create(Basic, button + circular, &(Point) { 800, 850, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
	UIElement* last = anim;
	Line* l;
	camera.pos = (Point) { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0 };
	camera.scale = 1;
	Game_BreakLoop = FALSE;
	Game_BreakMouse = FALSE;
	NodeList_Init(&UIElementList);
	NodeList_Init(&TextureList);
	/*
	anim->play = TRUE;
	anim->animationspeed = 20;
	anim2->play = TRUE;
	anim2->animationspeed = 1;
	*/
	UIElement_EnableFlags(anim2, draggable);
	Game_AddUIElement(anim);
	Game_AddUIElement(anim2);
	//NodeList_Print(&UIElementList);
	mousepos = (SDL_Point) { 0, 0 };
	for (i = 0; i < 150; i++)
	{
		UIElement *fuk = UIElement_Create(Basic, button + draggable + circular, &(Point) { rand() % 30 - 15, 15, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
		//Game_AddUIElement(fuk);
		//UIElement_Parent(fuk, last);
		//fuk->outline = TRUE;
		//last = fuk;
		UIElement_Destroy(fuk);
	}
	//anim2->outline = TRUE;
	fuck = DiffusionField_Create(&(Point) { WINDOW_WIDTH / 16, WINDOW_HEIGHT / 16, 0 });
	fuck->mapdim = (Point) { WINDOW_WIDTH, WINDOW_HEIGHT, 0 };
	fuck->field[DiffusionField_PointerArithmetic(&fuck->dim, &(Point){WINDOW_WIDTH / 32, WINDOW_HEIGHT / 32})] = 10000000;
	fuck->passesPerSec = 5;
	l = Line_Create(&(Point) { 400, 400, 0 }, &(Point){600, 1200, 0});
	Line_Rasterize(fuck->diffusable, NULL, FALSE, TRUE, &fuck->dim, &fuck->mapdim, &fuck->pos, l);
	free(l);
	l = Line_Create(&(Point) { 400, 400, 0 }, &(Point){150, 800, 0});
	Line_Rasterize(fuck->diffusable, NULL, FALSE, TRUE, &fuck->dim, &fuck->mapdim, &fuck->pos, l);
}
void Game_LoadTextures(SDL_Renderer* renderer) //see if we can load everything at once
{
	Game_GetTexture(&dfieldtexture, NULL, NULL, renderer, "res/pixelwhite.png");
	Game_GetTexture(&dfieldnexttexture, NULL, NULL, renderer, "res/pixelred.png");
}
void Game_AddUIElement(UIElement* element)
{
	Node *n = Node_Create();
	n->UIElement = element;
	NodeList_AddNode(&UIElementList, n);
}
void Game_GetTexture(SDL_Texture** outputTexture, Point* outputFrameDim, Point* outputTextureDim, SDL_Renderer* renderer, char* path)
{
	//first we try to find this texture, unless we find a suitable place to insert it
	Node* temp = TextureList.headNode, *in;
	//Point outFd, outTd;
	char *p1, *x;
	while (temp != NULL)
	{
		int comp = strcmp(temp->string, path);
		if (comp == 0) //if equal
		{
			*outputTexture = temp->texture;
			if (outputFrameDim != NULL)
			{
				outputFrameDim->x = temp->x;
				outputFrameDim->y = temp->y;
			}
			if (outputTextureDim != NULL)
			{
				outputTextureDim->x = temp->w;
				outputTextureDim->y = temp->h;
			}
			return;
		}
		else if (comp == 1) //we passed the place where it could be, let's INSERT
		{
			in = Node_Create();
			image = IMG_Load(path);
			in->w = image->w;
			in->h = image->h;
			in->texture = SDL_CreateTextureFromSurface(renderer, image);
			in->string = path;
			p1 = strchr(path, '(');
			if (p1 != NULL)
			{
				x = strchr(p1 + 1, 'x');
				if (x != NULL)
				{
					in->x = ParseInt(p1 + 1);
					in->y = ParseInt(x + 1);
				}
				else
				{
					in->x = 1;
					in->y = 1;
				}
			}
			else
			{
				in->x = 1;
				in->y = 1;
			}
			SDL_FreeSurface(image);
			NodeList_InsertNode(&TextureList, temp->previousNode, in);
			*outputTexture = in->texture;
			if (outputFrameDim != NULL)
			{
				outputFrameDim->x = temp->x;
				outputFrameDim->y = temp->y;
			}
			if (outputTextureDim != NULL)
			{
				outputTextureDim->x = temp->w;
				outputTextureDim->y = temp->h;
			}
			return;
		}
		temp = temp->nextNode;
	}
	//if we reach the end or there's nothing loaded yet
	in = Node_Create();
	image = IMG_Load(path);
	in->w = image->w;
	in->h = image->h;
	in->texture = SDL_CreateTextureFromSurface(renderer, image);
	in->string = path;
	p1 = strchr(path, '(');
	if (p1 != NULL)
	{
		x = strchr(p1 + 1, 'x');
		if (x != NULL)
		{
			in->x = ParseInt(p1 + 1);
			in->y = ParseInt(x + 1);
		}
		else
		{
			in->x = 1;
			in->y = 1;
		}
	}
	else
	{
		in->x = 1;
		in->y = 1;
	}
	SDL_FreeSurface(image);
	NodeList_AddNode(&TextureList, in);
	*outputTexture = in->texture;
	if (outputFrameDim != NULL)
	{
		outputFrameDim->x = in->x;
		outputFrameDim->y = in->y;
	}
	if (outputTextureDim != NULL)
	{
		outputTextureDim->x = in->w;
		outputTextureDim->y = in->h;
	}
}
void Game_Update(double frametime, Uint8 *keystate)
{
	Node* temp = UIElementList.headNode;
	Game_BreakMouse = FALSE;
	while (temp != NULL)
	{
		UIElement_Update(temp->UIElement, frametime);
		temp = temp->nextNode;
	}
	DiffusionField_Update(fuck, frametime); //AAAAAAAAAAAAAAAAAA
	if (keystate[SDL_SCANCODE_W])
	{
		camera.pos.y -= frametime * 100;
	}
	if (keystate[SDL_SCANCODE_S])
	{
		camera.pos.y += frametime * 100;
	}
	if (keystate[SDL_SCANCODE_A])
	{
		camera.pos.x -= frametime * 100;
	}
	if (keystate[SDL_SCANCODE_D])
	{
		camera.pos.x += frametime * 100;
	}
	if (keystate[SDL_SCANCODE_UP])
	{
		camera.scale += frametime * camera.scale;
	}
	if (keystate[SDL_SCANCODE_DOWN])
	{
		camera.scale -= frametime * camera.scale;
	}
	
}
void Game_UpdateReverse(Uint8 *keystate)
{
	Node* temp = UIElementList.lastNode;
	Game_BreakMouse = FALSE;
	while (temp != NULL)
	{
		UIElement_UpdateReverse(temp->UIElement);
		temp = temp->previousNode;
	}
}
void Game_UpdateParentRelationship()
{
	Node* temp = UIElementList.headNode;
	while (temp != NULL)
	{
		UIElement_UpdateParentRelationship(temp->UIElement);
		temp = temp->nextNode;
	}
}
void Game_UpdateLastPos()
{
	Node* temp = UIElementList.headNode;
	while (temp != NULL)
	{
		UIElement_UpdateLastPos(temp->UIElement);
		temp = temp->nextNode;
	}
}
void Game_Draw(SDL_Renderer* renderer)
{
	Node* temp = UIElementList.headNode;
	Point c, coll;
	Line *m, *o;
	while (temp != NULL)
	{
		UIElement_Draw(renderer, temp->UIElement);
		temp = temp->nextNode;
	}
	DiffusionField_Draw(renderer, fuck, &camera);
	DrawLine(renderer, (SDL_Point) { 50, 50 }, mousepos, 2, (SDL_Color) { 255, 255, 0, 255 });
	DrawLine(renderer, (SDL_Point) { 500, 300 }, (SDL_Point) { 500, 400 }, 2, (SDL_Color) { 255, 255, 0, 255 });

	SDL_Point_to_Point(&c, &mousepos);
	m = Line_Create(&(Point) { 50, 50, 0 }, &c);
	o = Line_Create(&(Point) { 500, 300, 0 }, &(Point){500, 400, 0});
	if (Line_Collides(&coll, m, o))
	{
		DrawCircle(renderer, (SDL_Point) { coll.x, coll.y}, 10, (SDL_Color) { 255, 0, 0, 255 });
	}
	free(m);
	free(o);
}
void Game_HandleEvent(SDL_Event* event)
{
	Node* temp = UIElementList.lastNode;
	Game_BreakLoop = FALSE;
	while (temp != NULL && Game_BreakLoop == FALSE)
	{
		UIElement_HandleEvents(temp->UIElement, event);
		temp = temp->previousNode;
	}
	switch (event->type) {
	case SDL_MOUSEMOTION:
		mousepos.x = event->motion.x;
		mousepos.y = event->motion.y;
		break;
	}
}
void DrawCircle(SDL_Renderer *renderer, SDL_Point point, int radius, SDL_Color color)
{
	int i;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (i = -radius; i <= radius; i++) {
		double delta = sqrt(radius * radius - i * i);
		SDL_RenderDrawLine(renderer, point.x - delta, point.y + i, point.x + delta, point.y + i);
	}
}
void DrawLine(SDL_Renderer *renderer, SDL_Point point1, SDL_Point point2, int thickness, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (SDL_Point_IsEqual(&point1, &point2))
	{
		DrawCircle(renderer, point1, thickness, color);
	}
	else
	{
		double i;
		double deltay = (double)(point2.x - point1.x) / SDL_Point_Length(&point1, &point2);
		double deltax = (double)(point2.y - point1.y) / SDL_Point_Length(&point1, &point2);
		for (i = -thickness; i <= thickness; i += 0.5)
		{
			SDL_RenderDrawLine(renderer, point1.x - i * deltax, point1.y + i * deltay, point2.x - i * deltax, point2.y + i * deltay);
		}
	}
}