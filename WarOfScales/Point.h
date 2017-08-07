#pragma once
#include "Utilities.h"
#include <malloc.h>
#define Vector Point
typedef struct Point
{
	double x, y, z;
}Point;

void Point_Init(Point* p);
inline Point* Point_Create(double x, double y, double z);
void SDL_Point_to_Point(Point* out, SDL_Point* point);
void Point_Copy(Point* output, Point* in);
void Point_Add(Point* output, Point* p1, Point* p2);
void Point_Subtract(Point* output, Point* p1, Point* p2);
void Point_Multiply(Point* output, Point* p, double d);
void Point_Dot(double* output, Point* p1, Point* p2);
void Point_Cross(Point* output, Point* p1, Point* p2);
BOOLEAN SDL_Point_IsEqual(SDL_Point* left, SDL_Point* right);
BOOLEAN Point_IsEqual(Point* left, Point* right);
double SDL_Point_Length(SDL_Point* p1, SDL_Point* p2);
double Point_Length(Point* p1, Point* p2);
double Point_Magnitude(Point* p);
void Point_Print(Point* p);
