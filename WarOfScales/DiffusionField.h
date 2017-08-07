#pragma once
#include "Point.h"
#include "LinkedList.h"

#define MAX_PASSES_PER_UPDATE 100
#define DIAGONAL_WEIGHT 0.10355339059327376220042218105242
//#define DIAGONAL_WEIGHT 0.14644660940672623779957781894758
#define ADJACENT_WEIGHT 0.14644660940672623779957781894758
#define DEFAULT_D 1
typedef struct DiffusionField {
	double *field, *D, *lambda, *next;
	BOOLEAN* diffusable;
	double passesPerSec, passTimer;
	int iterationsPerPass;
	Point dim, fielddim;
	NodeList* agents;
} DiffusionField;
SDL_Texture* dfieldtexture;

void DiffusionField_Init(DiffusionField* dfield);
DiffusionField* DiffusionField_Create(Point* dim);
void DiffusionField_Destroy(DiffusionField* dfield);
void DiffusionField_Update(DiffusionField* dfield, double frametime);
void DiffusionField_Iterate(DiffusionField* dfield, int times);
void DiffusionField_SetArray(double* doubleArray, BOOLEAN* booleanArray, Point* dim, double value);
void DiffusionField_Clear(DiffusionField* dfield);
void DiffusionField_Scan(Point* output, DiffusionField* dfield, Point* pos, double radius);
void DiffusionField_Point_MapToField(Point* output, DiffusionField* dfield, Point* in);
void DiffusionField_Point_FieldToMap(Point* output, DiffusionField* dfield, Point* in);
inline int DiffusionField_PointerArithmetic(Point* dim, Point* pos);
inline void DiffusionField_ArithmeticPointer(int* outx, int* outy, Point* dim, int ptr);
void DiffusionField_Draw(SDL_Renderer* renderer, DiffusionField* field);