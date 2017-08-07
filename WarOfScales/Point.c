#include "Point.h"
#include <stdio.h>

void Point_Init(Point* p)
{
	p->x = 0;
	p->y = 0;
	p->z = 0;
}
inline Point* Point_Create(double x, double y, double z)
{
	Point* p = malloc(sizeof(Point));
	p->x = x;
	p->y = y;
	p->z = z;
	return p;
}
void SDL_Point_to_Point(Point* out, SDL_Point* point)
{
	out->x = point->x;
	out->y = point->y;
	out->z = 0;
}
void Point_Copy(Point* output, Point* in)
{
	/*
	Point input = *in;
	*output = input;
	*/
	memcpy(output, in, sizeof(Point));
}
void Point_Add(Point* output, Point* p1, Point* p2)
{
	output->x = p1->x + p2->x;
	output->y = p1->y + p2->y;
	output->z = p1->z + p2->z;
}
void Point_Subtract(Point* output, Point* p1, Point* p2)
{
	output->x = p1->x - p2->x;
	output->y = p1->y - p2->y;
	output->z = p1->z - p2->z;
}
void Point_Multiply(Point* output, Point* p, double d)
{
	output->x = p->x * d;
	output->y = p->y * d;
	output->z = p->z * d;
}
void Point_Dot(double* output, Point* p1, Point* p2)
{
	*output = p1->x * p2->x +
		p1->y * p2->y +
		p1->z * p2->z;
}
void Point_Cross(Point* output, Point* p1, Point* p2)
{
	Point *p = Point_Create(p1->y * p2->z - p1->z * p2->y,
		p1->z * p2->x - p1->x * p2->z,
		p1->x * p2->y - p1->y * p2->x);
	Point_Copy(output, p);
}
BOOLEAN SDL_Point_IsEqual(SDL_Point* left, SDL_Point* right)
{
	return (left->x == right->x) && (left->y == right->y) ? TRUE : FALSE;
}
BOOLEAN Point_IsEqual(Point* left, Point* right)
{
	return double_IsEqual(left->x, right->x) && double_IsEqual(left->y, right->y) && double_IsEqual(left->z, right->z) ? TRUE : FALSE;
}
double SDL_Point_Length(SDL_Point* p1, SDL_Point* p2)
{
	return sqrt((double)(p2->x - p1->x) * (double)(p2->x - p1->x) + (double)(p2->y - p1->y) * (double)(p2->y - p1->y));
}
double Point_Length(Point* p1, Point* p2)
{
	return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y) + (p2->z - p1->z) * (p2->z - p1->z));
}
double Point_Magnitude(Point* p)
{
	return sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
}
void Point_Print(Point* p)
{
	printf("Point %i: (%f, %f, %f)\n", p, p->x, p->y, p->z);
}