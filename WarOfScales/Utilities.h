#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "MyOpenCL.h"
//#include "main.h"
/*
#include "SDL_render.h"
#include "SDL_surface.h"
*/

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define PI 3.1415926535897932384626433832795
#define DEG_TO_RAD (PI / 180.0)

#define EPSILON 0.0001f

#define TRUE 1
#define FALSE 0
#define BOOLEAN char



// Typedefs for consistency
typedef unsigned char u8;
typedef char s8;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long long u64;
typedef long long s64;
typedef float f32;
typedef double f64;
size_t global_item_size;
size_t local_item_size;
cl_mem arrmemobj;
cl_mem valmemobj;

void SetArray(BOOLEAN* booleanarray, double* doublearray, int elements, double val);
BOOLEAN float_IsEqual(float left, float right);
BOOLEAN double_IsEqual(double left, double right);
int Round(double input);
int ParseInt(char* in);
void Fuck(int** one, int* two);