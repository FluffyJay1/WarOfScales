#include "Utilities.h"
#include "Game.h"
#include "MyOpenCL.h"



const Uint8 *keystate;
SDL_Surface *image;




int main(int argc, char* argv[])
{
	int i, j;
	int k = 0;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture* texture;
	SDL_Event event;
	BOOLEAN quit = FALSE;
	
	Uint32 miliseconds = SDL_GetTicks();
	int past100frames[100] = {0}; // Initializes array to 0
	int past100framestime = 0; // Records the total time it took to process previous 100 frames (in ms)

	srand(time(NULL));
	keystate = SDL_GetKeyboardState(NULL);
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"An SDL2 window",			//window title
		SDL_WINDOWPOS_UNDEFINED,	//initial x pos
		SDL_WINDOWPOS_UNDEFINED,	//initial y pos
		WINDOW_WIDTH,				//width
		WINDOW_HEIGHT,				//height
		SDL_WINDOW_OPENGL			//flags
	);

	if (window == NULL) { //in case if window could not be made
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	IMG_Init(IMG_INIT_PNG);
	Game_LoadTextures(renderer);
	/*
	image = IMG_Load("res/anim(2x2).png");
	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);
	*/
	Game_GetTexture(&texture, NULL, NULL, renderer, "res/circle64.png");
	InitializeMyOpenCL();
	LoadKernels();
	Game_Init();
	while (!quit)
	{
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				quit = TRUE;
				break;
			default:
				Game_HandleEvent(&event);
				break;
			}
		}
		mousestate = SDL_GetMouseState(&mousepos.x, &mousepos.y);
		//if (SDL_GetTicks() != miliseconds) {
			Game_Update((double)(SDL_GetTicks() - miliseconds) * 0.001, keystate);
			Game_UpdateReverse(keystate);
			Game_UpdateParentRelationship();
			Game_UpdateLastPos();
			past100framestime -= past100frames[k % 100];
			past100frames[k % 100] = (SDL_GetTicks() - miliseconds);
			past100framestime += past100frames[k % 100];
			k++;
			printf("%f\n", 100.0 * 1000.0 / past100framestime); //print fps
			
			
			miliseconds = SDL_GetTicks();
		//}
		//SDL_SetRenderDrawColor(renderer, rand() * 255, rand() * 255, rand() * 255, SDL_ALPHA_OPAQUE);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		//SDL_RenderCopy(renderer, texture, NULL, NULL);
		Game_Draw(renderer);
		SDL_RenderPresent(renderer);
	}
	//cleanup
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	CleanUpMyOpenCL();
	return 0;
}
