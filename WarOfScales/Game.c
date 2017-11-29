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
//DiffusionField *fuck;
Map *map;
/*
PointArray *somepointarray;
NodeList* pointarraylines;
*/
void Game_Init()
{
	int i;
	Node* temp;
	UIElement *anim = UIElement_Create(UIEClass_Base, UIEFlag_Button + UIEFlag_Draggable + UIEFlag_Circular, &(Point) { 800, 450, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
	UIElement *anim2 = UIElement_Create(UIEClass_Base, UIEFlag_Button + UIEFlag_Circular, &(Point) { 800, 850, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
	UIElement* last = anim;
	Line* l;
	camera.pos = (Point) { 0, 0, 0 };
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
	UIElement_EnableFlags(anim2, UIEFlag_Draggable);
	Game_AddUIElement(anim);
	Game_AddUIElement(anim2);
	mousepos = (SDL_Point) { 0, 0 };
	for (i = 0; i < 150; i++)
	{
		UIElement *fuk = UIElement_Create(UIEClass_Base, UIEFlag_Button + UIEFlag_Draggable + UIEFlag_Circular, &(Point) { rand() % 30 - 15, 15, 0 }, &(Point) { 200, 200, 0 }, "res/anim(2x2).png");
		//Game_AddUIElement(fuk);
		//UIElement_Parent(fuk, last);
		//fuk->outline = TRUE;
		//last = fuk;
		UIElement_Destroy(fuk);
	}
	//anim2->outline = TRUE;
	/*
	fuck = DiffusionField_Create(&(Point) { 128, 128, 0 });
	fuck->mapdim = (Point) { 1024, 1024, 0 };
	fuck->pos = (Point) { WINDOW_WIDTH/2 - 512, WINDOW_HEIGHT/2 - 512 };
	fuck->field[DiffusionField_PointerArithmetic(&fuck->dim, &(Point){64, 64, 0})] = 100000;
	fuck->passesPerSec = 5;
	fuck->iterationsPerPass = 1;
	*/
	map = Map_CreateTest();
	/*
	somepointarray = PointArray_Create(24);
	somepointarray->points[0] = (Point) { 200, 200, 0 };
	somepointarray->points[1] = (Point) { 300, 200, 0 };
	somepointarray->points[2] = (Point) { 400, 700, 0 };
	somepointarray->points[3] = (Point) { 450, 650, 0 };
	somepointarray->points[4] = (Point) { 300, 400, 0 };
	somepointarray->points[5] = (Point) { 300, 300, 0 };
	
	for (i = 0; i < 24; i++) {
		somepointarray->points[i] = (Point) { rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT, (int)(rand() % 100) - 50 };
	}
	pointarraylines = NodeList_Create();
	PointArray_GetLines(pointarraylines, somepointarray);
	temp = pointarraylines->headNode;
	while (temp != NULL)
	{
		//Line_Rasterize(fuck->diffusable, NULL, FALSE, TRUE, &fuck->dim, &fuck->mapdim, &fuck->pos, temp->Geom.Line);
		temp = temp->nextNode;
	}
	*/
	/*
	l = Line_Create(&(Point) { 400, 400, 0 }, &(Point){600, 1200, 0});
	Line_Rasterize(fuck->diffusable, NULL, FALSE, TRUE, &fuck->dim, &fuck->mapdim, &fuck->pos, l);
	free(l);
	l = Line_Create(&(Point) { 400, 400, 0 }, &(Point){150, 800, 0});
	Line_Rasterize(fuck->diffusable, NULL, FALSE, TRUE, &fuck->dim, &fuck->mapdim, &fuck->pos, l);
	*/
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
	Node* temp, *in;
	//Point outFd, outTd;
	char *p1, *x;
	for (temp = TextureList.headNode; temp != NULL; temp = temp->nextNode)
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
					in->x = ParseInt(p1 + 1, NULL);
					in->y = ParseInt(x + 1, NULL);
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
			in->x = ParseInt(p1 + 1, NULL);
			in->y = ParseInt(x + 1, NULL);
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
	Node* temp;
	Game_BreakMouse = FALSE;
	for (temp = UIElementList.headNode; temp != NULL; temp = temp->nextNode)
	{
		UIElement_Update(temp->UIElement, frametime);
	}
	//DiffusionField_Update(fuck, frametime); //AAAAAAAAAAAAAAAAAA
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
	Map_Update(map, frametime);
}
void Game_UpdateReverse(Uint8 *keystate)
{
	Node* temp;
	Game_BreakMouse = FALSE;
	for (temp = UIElementList.lastNode; temp != NULL; temp = temp->previousNode)
	{
		UIElement_UpdateReverse(temp->UIElement);
	}
}
void Game_UpdateParentRelationship()
{
	Node* temp;
	for (temp = UIElementList.headNode; temp != NULL; temp = temp->nextNode)
	{
		UIElement_UpdateParentRelationship(temp->UIElement);
	}
}
void Game_UpdateLastPos()
{
	Node* temp;
	for (temp = UIElementList.headNode; temp != NULL; temp = temp->nextNode)
	{
		UIElement_UpdateLastPos(temp->UIElement);
	}
}
void Game_Draw(SDL_Renderer* renderer)
{
	int i = 0; //blank comments mean is test code
	Node* temp;
	Point c, coll;
	Line *m, *o;
	//BOOLEAN *bcoll = malloc(sizeof(BOOLEAN) * somepointarray->size);//
	//Point *pos = malloc(sizeof(Point) * somepointarray->size);//
	BOOLEAN bcoll1;
	Point pos1;
	//NodeList line;//
	//Node* asdf = Node_Create();//
	//NodeList_Init(&line);//
	
	for (temp = UIElementList.headNode; temp != NULL; temp = temp->nextNode)
	{
		UIElement_Draw(renderer, temp->UIElement);
	}
	//temp = pointarraylines->headNode;//
	/*
	while (temp != NULL)//
	{
		DrawLine(renderer, (SDL_Point) { temp->Geom.Line->p1.x, temp->Geom.Line->p1.y}, (SDL_Point) { temp->Geom.Line->p2.x, temp->Geom.Line->p2.y }, 2, (SDL_Color) { 255, 255, 0, 255 });
		temp = temp->nextNode;
	}
	*/
	//DiffusionField_Draw(renderer, fuck, &camera);
	Map_Draw(renderer, map, &camera);
	DrawLine(renderer, (SDL_Point) { 50, 50 }, mousepos, 2, (SDL_Color) { 255, 255, 0, 255 });
	DrawLine(renderer, (SDL_Point) { 500, 300 }, (SDL_Point) { 500, 400 }, 2, (SDL_Color) { 255, 255, 0, 255 });

	SDL_Point_to_Point(&c, &mousepos);
	m = Line_Create(&(Point) { 50, 50, 0 }, &c);
	o = Line_Create(&(Point) { 500, 300, -50 }, &(Point){500, 400, 50});
	if (Line_Collides(&coll, m, o, TRUE))
	{
		DrawCircle(renderer, (SDL_Point) { coll.x, coll.y}, 10, (SDL_Color) { 255, 0, 0, 255 });
	}
	//asdf->Geom.Line = m;//
	//NodeList_AddNode(&line, asdf);//
	
	//Line_CollidesBatch(bcoll, &bcoll1, NULL, pos, &pos1, NULL, &line, pointarraylines, TRUE);//
	/*
	for (i = 0; i < pointarraylines->listSize; i++)//
	{
		if (bcoll[i])
		{
			DrawCircle(renderer, (SDL_Point) { (int)pos[i].x, (int)pos[i].y }, 10, (SDL_Color) { 255, 0, 0, 255 });
		}
	}
	if (bcoll1)
	{
		DrawCircle(renderer, (SDL_Point) { (int)pos1.x, (int)pos1.y }, 10, (SDL_Color) { 0, 255, 255, 255 });
	}
	*/
	free(m);
	free(o);

	//free(pos);//
	//free(bcoll);//
	//NodeList_Clear(&line, FALSE);//
}
void Game_HandleEvent(SDL_Event* event)
{
	Node* temp;
	PointArray* pa;
	Point mapspace, gridspace;
	Game_BreakLoop = FALSE;
	for (temp = UIElementList.lastNode; temp != NULL && Game_BreakLoop == FALSE; temp = temp->previousNode)
	{
		UIElement_HandleEvents(temp->UIElement, event);
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