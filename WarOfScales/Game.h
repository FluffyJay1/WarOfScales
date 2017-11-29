#pragma once
//#include "main.h"
#include "SDL.h"
#include "LinkedList.h"
#include "Point.h"
#include "UIElement.h"
#include "DiffusionField.h"
#include "Camera.h"
#include "Map.h"

Camera camera;
SDL_Point mousepos;
Uint32 mousestate;
NodeList UIElementList;
BOOLEAN Game_BreakLoop;
BOOLEAN Game_BreakMouse;
void Game_Init();
void Game_LoadTextures(SDL_Renderer* renderer);
void Game_AddUIElement(UIElement* element);
void Game_GetTexture(SDL_Texture** outputTexture, Point* outputFramedim, Point* outputTextureDim, SDL_Renderer* renderer, char* path);
void Game_Update(double frametime, Uint8 *keystate);
void Game_UpdateReverse(Uint8 *keystate);
void Game_UpdateParentRelationship();
void Game_UpdateLastPos();
void Game_Draw(SDL_Renderer* renderer);
void Game_HandleEvent(SDL_Event* event);
void DrawCircle(SDL_Renderer *renderer, SDL_Point point, int radius, SDL_Color color);
void DrawLine(SDL_Renderer *renderer, SDL_Point point1, SDL_Point point2, int thickness, SDL_Color color);