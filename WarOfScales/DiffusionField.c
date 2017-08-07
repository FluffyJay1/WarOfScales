#include "DiffusionField.h"

void DiffusionField_Init(DiffusionField* dfield)
{
	dfield->field = NULL;
	dfield->D = NULL;
	dfield->lambda = NULL;
	dfield->next = NULL;
	dfield->diffusable = NULL;
	dfield->passesPerSec = 1;
	dfield->passTimer = 0;
	dfield->iterationsPerPass = 1;
	dfield->dim = *Point_Create(0, 0, 0);
	dfield->fielddim = *Point_Create(0, 0, 0);
	dfield->agents = NodeList_Create();
}
DiffusionField* DiffusionField_Create(Point* dim)
{
	DiffusionField* dfield = malloc(sizeof(DiffusionField));
	DiffusionField_Init(dfield);
	Point_Copy(&dfield->dim, dim);
	dfield->field = malloc(sizeof(*dfield->field) * dim->x * dim->y);
	dfield->D = malloc(sizeof(*dfield->D) * dim->x * dim->y);
	dfield->lambda = malloc(sizeof(double) * dim->x * dim->y);
	dfield->next = malloc(sizeof(double) * dim->x * dim->y);
	dfield->diffusable = malloc(sizeof(BOOLEAN) * dim->x * dim->y);
	DiffusionField_SetArray(dfield->field, NULL, dim, 0);
	DiffusionField_SetArray(dfield->D, NULL, dim, DEFAULT_D);
	DiffusionField_SetArray(dfield->lambda, NULL, dim, 1);
	DiffusionField_SetArray(NULL, dfield->diffusable, dim, TRUE);
	return dfield;
}
void DiffusionField_Destroy(DiffusionField* dfield)
{
	free(dfield->field);
	free(dfield->diffusable);
	free(dfield);
}
void DiffusionField_Update(DiffusionField* dfield, double frametime)
{
	int iterations = 0;
	dfield->passTimer += frametime * dfield->passesPerSec;
	while (dfield->passTimer >= 1 && iterations < MAX_PASSES_PER_UPDATE)
	{
		DiffusionField_Iterate(dfield, dfield->iterationsPerPass);
		dfield->passTimer--;
		iterations++;
	}
}
void DiffusionField_Iterate(DiffusionField* dfield, int times)
{
	int i, ptr;
	int x, y, dx, dy;
	int deltaptr;
	double multiplier;
	//double* next = malloc(sizeof(double) * dfield->dim.x * dfield->dim.y);
	for (i = 0; i < times; i++)
	{
		for (ptr = 0; ptr < dfield->dim.x * dfield->dim.y; ptr++) //for each point on the field
		{
			//Point* p;
			dfield->next[ptr] = dfield->field[ptr];
			DiffusionField_ArithmeticPointer(&x, &y, &dfield->dim, ptr);
			if (dfield->diffusable[ptr] == TRUE) {
				for (dx = -1; dx <= 1; dx++) //for all nearby field elements
				{
					for (dy = -1; dy <= 1; dy++)
					{
						if (dx != 0 || dy != 0)
						{
							if (dx * dy == 0)
							{
								multiplier = ADJACENT_WEIGHT;
							}
							else
							{
								multiplier = DIAGONAL_WEIGHT;
							}
							//p = Point_Create(x + dx, y + dy, 0);
							deltaptr = DiffusionField_PointerArithmetic(&dfield->dim, &(Point){x + dx, y + dy, 0});
							if (x + dx >= 0 && x + dx < dfield->dim.x && y + dy >= 0 && y + dy < dfield->dim.y
								&& dfield->diffusable[deltaptr])
							{
								dfield->next[ptr] += multiplier * dfield->D[ptr] * (dfield->field[deltaptr] - dfield->field[ptr]);
							}
							//free(p);
						}
					}
				}
			}
			dfield->next[ptr] *= dfield->lambda[ptr];
		}
		memcpy(dfield->field, dfield->next, sizeof(double) * dfield->dim.x * dfield->dim.y);
	}
	//free(next);
}
void DiffusionField_SetArray(double* doubleArray, BOOLEAN* booleanArray, Point* dim, double value)
{
	int i;
	if (doubleArray != NULL)
	{
		for (i = 0; i < dim->x * dim->y; i++)
		{
			doubleArray[i] = value;
		}
	}
	if (booleanArray != NULL)
	{
		for (i = 0; i < dim->x * dim->y; i++)
		{
			booleanArray[i] = value;
		}
	}
}
void DiffusionField_Clear(DiffusionField* dfield);
void DiffusionField_Scan(Point* output, DiffusionField* dfield, Point* pos, double radius);
void DiffusionField_Point_MapToField(Point* output, DiffusionField* dfield, Point* in);
void DiffusionField_Point_FieldToMap(Point* output, DiffusionField* dfield, Point* in);
inline int DiffusionField_PointerArithmetic(Point* dim, Point* pos)
{
	return pos->y * dim->x + pos->x;
}
inline void DiffusionField_ArithmeticPointer(int* outx, int* outy, Point* dim, int ptr)
{
	*outx = ptr % Round(dim->x);
	*outy = (int)(ptr / dim->x);
}
void DiffusionField_Draw(SDL_Renderer* renderer, DiffusionField* dfield)
{
	int x, y;
	for (x = 0; x < dfield->dim.x; x++)
	{
		for (y = 0; y < dfield->dim.y; y++)
		{
			SDL_Rect dstrect;
			double val = dfield->field[DiffusionField_PointerArithmetic(&dfield->dim, &(Point){ x, y, 0 })];
			double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0;
			dstrect.x = x * WINDOW_WIDTH / dfield->dim.x;
			dstrect.y = y * WINDOW_HEIGHT / dfield->dim.y;
			dstrect.w = WINDOW_WIDTH / dfield->dim.x;
			dstrect.h = WINDOW_HEIGHT / dfield->dim.y;
			SDL_SetTextureAlphaMod(dfieldtexture, alpha);
			SDL_SetTextureColorMod(dfieldtexture, alpha, alpha, alpha);
			SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		}
	}
}