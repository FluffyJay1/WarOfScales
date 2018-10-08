#pragma once
#include "Utilities.h"
#include "LinkedList.h"
#include "MyOpenCL.h"
#include <malloc.h>
//#define Vector Point
#define HALF_POINT_2D (Point){0.5, 0.5, 0}

typedef struct Point {
	double x, y, z;
}Point;
typedef struct PointArray {
	Point* points;
	int size;
}PointArray;
typedef struct Line { //idk why i'm making this a struct someone send help
	Point p1, p2;
	double slope, length, bound_up, bound_down, bound_left, bound_right;
	BOOLEAN p1right, p1down, vertical;
}Line;
typedef struct Grid { //aaaaaaaaaaaaaaaaaaaa
	BOOLEAN* booleangrid;
	double* doublegrid;
	Point dim;
}Grid;
enum CollisionHullTypes {
	CHType_Circle,
	CHType_Polygon,
};
typedef struct CollisionHull {
	int hulltype;
	double circleRadius;
	PointArray* polygon;
}CollisionHull;
//array name, size of array, index in l_mem, member in line, argument # in kernel
#define LINECOLLISION_L1MAP(X) \
X(l1p1x, l1sized, 0, p1.x, 3) \
X(l1p1y, l1sized, 1, p1.y, 4) \
X(l1p2x, l1sized, 2, p2.x, 5) \
X(l1p2y, l1sized, 3, p2.y, 6) \
X(l1slope, l1sized, 4, slope, 11) \
X(l1bound_up, l1sized, 5, bound_up, 13) \
X(l1bound_down, l1sized, 6, bound_down, 14) \
X(l1bound_left, l1sized, 7, bound_left, 15) \
X(l1bound_right, l1sized, 8, bound_right, 16) \
X(l1p1down, l1sizeb, 9, p1down, 21) \
X(l1vertical, l1sizeb, 10, vertical, 22) \
X(l1p1z, l1sized, 22, p1.z, 25) \
X(l1p2z, l1sized, 23, p2.z, 26) 

//lay off of the 29th argument
#define LINECOLLISION_L2MAP(X) \
X(l2p1x, l2sized, 11, p1.x, 7) \
X(l2p1y, l2sized, 12, p1.y, 8) \
X(l2p2x, l2sized, 13, p2.x, 9) \
X(l2p2y, l2sized, 14, p2.y, 10) \
X(l2slope, l2sized, 15, slope, 12) \
X(l2bound_up, l2sized, 16, bound_up, 17) \
X(l2bound_down, l2sized, 17, bound_down, 18) \
X(l2bound_left, l2sized, 18, bound_left, 19) \
X(l2bound_right, l2sized, 19, bound_right, 20) \
X(l2p1down, l2sizeb, 20, p1down, 23) \
X(l2vertical, l2sizeb, 21, vertical, 24) \
X(l2p1z, l2sized, 24, p1.z, 27) \
X(l2p2z, l2sized, 25, p2.z, 28)

cl_mem outputcollides_memobj;
cl_mem outputx_memobj;
cl_mem outputy_memobj;
#define X() + 1
cl_mem l_mem[0 + LINECOLLISION_L1MAP(X) + LINECOLLISION_L2MAP(X)];
#undef X
cl_mem considerHeight_memobj;
cl_mem l1null_memobj;
cl_mem l2null_memobj;

void Point_Init(Point* p);
inline Point* Point_Create(double x, double y, double z);
void SDL_Point_to_Point(Point* out, SDL_Point* point);
void Point_Copy(Point* output, Point* in);
void Point_Add(Point* output, Point* p1, Point* p2);
void Point_Subtract(Point* output, Point* p1, Point* p2);
void Point_Multiply(Point* output, Point* p, double d);
void Point_Multiply2D(Point* output, Point* p, double d);
void Point_Dot(double* output, Point* p1, Point* p2);
void Point_Cross(Point* output, Point* p1, Point* p2);
void Point_Rotate(Point* output, Point* p, double radians);
void Point_Normalize(Point* output, Point* p);
void Point_Normalize2D(Point* output, Point* p);
void Point_Traverse(Point* output, Point* p1, Point* p2, double distance);
void Point_Traverse2D(Point* output, Point* p1, Point* p2, double distance);
void Point_MapToGrid(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in);
void Point_GridToMap(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in);
void Point_ToInt(Point* output, Point* in);
void Point_GenerateFromData(Point* out, char* data, char** returnpointer);
BOOLEAN Point_IsLessThan2D(Point* p1, Point* p2);
inline BOOLEAN Point_IsInsideCircle(Point* p, Point* circlepos, double radius);
BOOLEAN SDL_Point_IsEqual(SDL_Point* left, SDL_Point* right);
BOOLEAN Point_IsEqual(Point* left, Point* right);
BOOLEAN Point_IsInside2D(Point* dim, Point* p);
double SDL_Point_Length(SDL_Point* p1, SDL_Point* p2);
inline double Point_Length(Point* p1, Point* p2);
double Point_Length2D(Point* p1, Point* p2);
inline double Point_LengthSquared(Point* p1, Point* p2);
inline double Point_LengthSquared2D(Point* p1, Point* p2);
double Point_Magnitude(Point* p);
double Point_Magnitude2D(Point* p);
void Point_Print(Point* p);

PointArray* PointArray_Create(int size);
void PointArray_Copy(PointArray* out, PointArray* in);
void PointArray_Destroy(PointArray* pa);
PointArray* PointArray_CreateFromData(char* data, char** returnpointer);
inline Line* PointArray_GetLine(PointArray* pa, int lineindex);
void PointArray_GetLines(NodeList* out, PointArray* pa);
void PointArray_Rotate(PointArray* out, PointArray* in, double radians);

Line* Line_Create(Point* p1, Point* p2);
void Line_CreateAt(Line* l, Point* p1, Point* p2);
void Line_Copy(Line* out, Line* in);
void Line_GenerateFromData(Line* out, char* data, char** returnpointer);
void Line_GenerateListFromData(NodeList* n, char* data, char** returnpointer);
BOOLEAN Line_Collides(Point* out, Line* l1, Line* l2, BOOLEAN considerHeight);
void Line_CollidesBatch(BOOLEAN* outcoll, BOOLEAN* outcoll1, BOOLEAN* outcoll2, Point* outpos, Point* outpos1, Point* outpos2, Line** outline1, Line** outline2, NodeList* nl1, NodeList* nl2, BOOLEAN considerHeight);
BOOLEAN Line_CollidesCircle(Point* out, Line* line, Point* circlepos, double circleradius);
void Line_Rasterize(BOOLEAN* booleangrid, double* doublegrid, double result, BOOLEAN thick, Point* griddim, Point* mapdim, Point* pos, Line* line);
double Line_HeightAt(Line *l, Point *p);

Grid* Grid_Create(Point* dim, BOOLEAN booleangrid, BOOLEAN doublegrid);
inline int Grid_PointerArithmetic(Point* dim, Point* pos);
inline void Grid_ArithmeticPointer(int* outx, int* outy, Point* dim, int ptr);