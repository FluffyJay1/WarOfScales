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
BOOLEAN Point_IsInside2D(Point* dim, Point* p)
{
	return p->x >= 0 && p->x < dim->x && p->y >= 0 && p->y < dim->y ? TRUE : FALSE;
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

inline PointArray* PointArray_Create(int size)
{
	PointArray* pa = malloc(sizeof(PointArray));
	pa->points = malloc(sizeof(Point) * size);
	pa->size = size;
	return pa;
}
void PointArray_Destroy(PointArray* pa)
{
	free(pa->points);
	free(pa);
}
void PointArray_GetLine(Line* out, PointArray* pa, int lineindex)
{
	out = Line_Create(&pa->points[lineindex % pa->size], &pa->points[(lineindex + 1) % pa->size]);
}

inline Line* Line_Create(Point* p1, Point* p2) //compile the properties of a line so we don't have to repeatedly recalculate it
{
	Line* l = malloc(sizeof(Line));
	Point_Copy(&l->p1, p1);
	Point_Copy(&l->p2, p2);
	if (p1->x < p2->x)
	{
		l->bound_left = p1->x;
		l->bound_right = p2->x;
		l->p1right = FALSE;
	}
	else
	{
		l->bound_left = p2->x;
		l->bound_right = p1->x;
		l->p1right = TRUE;
	}
	if (p1->y < p2->y)
	{
		l->bound_up = p1->y;
		l->bound_down = p2->y;
		l->p1down = FALSE;
	}
	else
	{
		l->bound_up = p2->y;
		l->bound_down = p1->y;
		l->p1down = TRUE;
	}
	if (double_IsEqual(p1->x, p2->x) || Point_IsEqual(p1, p2))
	{
		l->vertical = TRUE;
		l->slope = INFINITY;
	}
	else
	{
		l->vertical = FALSE;
		l->slope = (p2->y - p1->y) / (p2->x - p1->x);
	}
	l->length = Point_Length(p1, p2);
	return l;
}
BOOLEAN Line_Collides(Point* out, Line* l1, Line* l2)
{
	Point ret;
	if (l1->bound_left > l2->bound_right || l1->bound_right < l2->bound_left || l1->bound_up > l2->bound_down || l1->bound_down < l2->bound_up)
	{
		return FALSE;
	}
	if (l1->vertical && l2->vertical) //same slopes, both vertical
	{
		if (double_IsEqual(l1->p1.x, l2->p1.x))
		{
			out->x = l1->p1.x;
			out->y = abs((l1->bound_up > l2->bound_up ? l1->bound_up : l2->bound_up) - l1->p1.y) < abs((l1->bound_down < l2->bound_down ? l1->bound_down : l2->bound_down) - l1->p1.y) ? (l1->bound_up > l2->bound_up ? l1->bound_up : l2->bound_up) : (l1->bound_down < l2->bound_down ? l1->bound_down : l2->bound_down);
			return TRUE;
		}
		return FALSE;
	}
	else if (l1->vertical == FALSE && l2->vertical == FALSE && double_IsEqual(l1->slope, l2->slope)) //same slopes
	{
		if (double_IsEqual(l1->p1.y - l1->slope * l1->p1.x, l2->p1.y - l2->slope * l2->p1.x)) //same intercepts
		{
			if (abs((l1->bound_up > l2->bound_up ? l1->bound_up : l2->bound_up) - l1->p1.y) < abs((l1->bound_down < l2->bound_down ? l1->bound_down : l2->bound_down) - l1->p1.y))
			{
				if (l1->bound_up > l2->bound_up)
				{
					Point_Copy(out, l1->p1down ? &l1->p2 : &l1->p1);
				}
				else
				{
					Point_Copy(out, l2->p1down ? &l2->p2 : &l2->p1);
				}
			}
			else
			{
				if (l1->bound_down < l2->bound_down)
				{
					Point_Copy(out, l1->p1down ? &l1->p1 : &l1->p2);
				}
				else
				{
					Point_Copy(out, l2->p1down ? &l2->p1 : &l2->p2);
				}
			}
			return TRUE;
		}
		return FALSE;
	}
	if (l1->vertical)
	{
		ret.x = l1->p1.x;
		ret.y = l2->slope * (ret.x - l2->p1.x) + l2->p1.y;
	}
	else if(l2->vertical)
	{
		ret.x = l2->p1.x;
		ret.y = l1->slope * (ret.x - l1->p1.x) + l1->p1.y;
	}
	else
	{
		ret.x = (l2->p1.y + l1->slope * l1->p1.x - l1->p1.y - l2->slope * l2->p1.x) / (l1->slope - l2->slope);
		ret.y = l1->slope * (ret.x - l1->p1.x) + l1->p1.y;
	}
	if (ret.x >= l1->bound_left && ret.x >= l2->bound_left && ret.x <= l1->bound_right && ret.x <= l2->bound_right
		&& ret.y >= l1->bound_up && ret.y >= l2->bound_up && ret.y <= l1->bound_down && ret.y <= l2->bound_down)
	{
		Point_Copy(out, &ret);
		return TRUE;
	}
	return FALSE;
}