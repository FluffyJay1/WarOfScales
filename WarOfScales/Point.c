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
void Point_Rotate(Point* output, Point* p, double radians)
{
	if (output != p) {
		output->x = p->x * cos(radians) - p->y * sin(radians);
		output->y = p->x * sin(radians) + p->y * cos(radians);
		output->z = p->z;
	}
	else
	{
		Point temp;
		temp.x = p->x * cos(radians) - p->y * sin(radians);
		temp.y = p->x * sin(radians) + p->y * cos(radians);
		temp.z = p->z;
		Point_Copy(output, &temp);
	}
}
void Point_MapToField(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in)
{
	output->x = (in->x - pos->x) * griddim->x / mapdim->x;
	output->y = (in->y - pos->y) * griddim->y / mapdim->y;
	output->z = in->z;
}
void Point_FieldToMap(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in)
{
	output->x = in->x * mapdim->x / griddim->x + pos->x;
	output->y = in->y * mapdim->y / griddim->y + pos->y;
	output->z = in->z;
}
void Point_ToInt(Point* output, Point* in)
{
	output->x = (int)in->x;
	output->y = (int)in->y;
	output->z = (int)in->z;
}
inline BOOLEAN Point_IsLessThan2D(Point* p1, Point* p2)
{
	return p1->x < p2->x && p1->y < p2->y ? TRUE : FALSE;
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
inline Point* PointArray_GetLine(PointArray* pa, int lineindex)
{
	return Line_Create(&pa->points[lineindex % pa->size], &pa->points[(lineindex + 1) % pa->size]);
}
void PointArray_GetLines(NodeList* out, PointArray* pa) 
{
	int i;
	if (out == NULL)
	{
		printf("Error in function PointArray_GetLines: output nodelist is null!");
	}
	for (i = 0; i < pa->size; i++)
	{
		Node* n = Node_Create();
		n->Line = PointArray_GetLine(pa, i);
		NodeList_AddNode(out, n);
	}
}
void PointArray_Rotate(PointArray* out, PointArray* in, double radians)
{
	int i;
	if (out == NULL)
	{
		printf("Error in function PointArray_Rotate: output pointarray is null!");
	}
	if (out->size < in->size) {
		printf("Error in function PointArray_Rotate: output pointarray is smaller than input!");
	}
	for (i = 0; i < in->size; i++) {
		Point_Rotate(&out->points[i], &in->points[i], radians);
	}
}

Line* Line_Create(Point* p1, Point* p2) //compile the properties of a line so we don't have to repeatedly recalculate it
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
void Line_Copy(Line* out, Line* in)
{
	memcpy(out, in, sizeof(Line));
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
void Line_CollidesBatch(NodeList* nl1, NodeList* nl2)
{
	Node* iterator;
	//int index;
	BOOLEAN* outputcollides;
	double *outputx, *outputy, *l1p1x, *l1p1y, *l1p2x, *l1p2y, *l2p1x, *l2p1y, *l2p2x, *l2p2y, *l1slope, *l2slope,
		*l1bound_up, *l1bound_down, *l1bound_left, *l1bound_right, *l2bound_up, *l2bound_down, *l2bound_left, *l2bound_right;
	BOOLEAN *l1p1down, *l1vertical, *l2p1down, *l2vertical;
	size_t l1sized = sizeof(double) * nl1->listSize;
	size_t l1sizeb = sizeof(BOOLEAN) * nl1->listSize;
	size_t l2sized = sizeof(double) * nl2->listSize;
	size_t l2sizeb = sizeof(BOOLEAN) * nl2->listSize;
	global_item_size = nl1->listSize * nl2->listSize;
	local_item_size = nl1->listSize;

	//malloc everything
	outputcollides = malloc(sizeof(BOOLEAN) * nl1->listSize * nl2->listSize);
	outputx = malloc(sizeof(double) * nl1->listSize * nl2->listSize);
	outputy = malloc(sizeof(double) * nl1->listSize * nl2->listSize);
#define X(k, v, j) k = malloc(v);
	LINECOLLISION_L1MAP(X);
	LINECOLLISION_L2MAP(X);
#undef X

	//write to arrays
#define X(k, v, j, l) k[iterator->index] = iterator->Line->l;
	iterator = nl1->headNode;
	//index = 0;
	while (iterator != NULL)
	{
		LINECOLLISION_L1MAP(X);
		//index++;
	}
	iterator = nl2->headNode;
	//index = 0;
	while (iterator != NULL)
	{
		LINECOLLISION_L2MAP(X);
		//index++;
	}
#undef X

	//create memobjs
	outputcollides_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(BOOLEAN) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
	outputx = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(double) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
	outputy = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		sizeof(double) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
#define X(k, v, j) l_mem[j] = clCreateBuffer(context, CL_MEM_READ_ONLY, v, NULL, &ret); CL_PRDEBUG;
	LINECOLLISION_L1MAP(X);
	LINECOLLISION_L2MAP(X);
#undef X

	//write to memobjs
#define X(k, v, j) ret = clEnqueueWriteBuffer(command_queue, l_mem[j], CL_TRUE, 0, v, k, 0, NULL, NULL);
	LINECOLLISION_L1MAP(X);
	LINECOLLISION_L2MAP(X);
#undef X

	//set args
	ret = clSetKernelArg(kernels[LineCollision], 0, sizeof(cl_mem), (void *)&outputcollides_memobj); CL_PRDEBUG;
	ret = clSetKernelArg(kernels[LineCollision], 1, sizeof(cl_mem), (void *)&outputx); CL_PRDEBUG;
	ret = clSetKernelArg(kernels[LineCollision], 2, sizeof(cl_mem), (void *)&outputy); CL_PRDEBUG;
#define X(k, v, j, l, i) ret = clSetKernelArg(kernels[LineCollision], i, sizeof(cl_mem), (void *)&l_mem[j]); CL_PRDEBUG;
	LINECOLLISION_L1MAP(X);
	LINECOLLISION_L2MAP(X);
#undef X
	//do the shit
	ret = clEnqueueNDRangeKernel(command_queue, kernels[LineCollision], 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);
	CL_PRDEBUG;

	//read the stuff
	ret = clEnqueueReadBuffer(command_queue, outputcollides_memobj, CL_TRUE, 0,
		sizeof(BOOLEAN) * nl1->listSize * nl2->listSize, outputcollides, 0, NULL, NULL);
	CL_PRDEBUG;
	ret = clEnqueueReadBuffer(command_queue, outputx_memobj, CL_TRUE, 0,
		sizeof(double) * nl1->listSize * nl2->listSize, outputx, 0, NULL, NULL);
	CL_PRDEBUG;
	ret = clEnqueueReadBuffer(command_queue, outputy_memobj, CL_TRUE, 0,
		sizeof(double) * nl1->listSize * nl2->listSize, outputy, 0, NULL, NULL);
	CL_PRDEBUG;
}
void Line_Rasterize(BOOLEAN* booleangrid, double* doublegrid, double result, BOOLEAN thick, Point* griddim, Point* mapdim, Point* pos, Line* line)
{
	//output->x = (int)((in->x - dfield->pos.x) * dfield->dim.x / dfield->mapdim.x)
	int i;
	Point gridp1, gridp2;
	Line* gridline;
	Point_MapToField(&gridp1, griddim, mapdim, pos, &line->p1);
	Point_MapToField(&gridp2, griddim, mapdim, pos, &line->p2);
	gridline = Line_Create(&gridp1, &gridp2);
	if (gridline->vertical)
	{
		for (i = gridline->bound_up > 0 ? gridline->bound_up : 0; i <= gridline->bound_down && i < griddim->y; i++)
		{
			if (booleangrid != NULL)
			{
				booleangrid[Grid_PointerArithmetic(griddim, &(Point){(int)gridp1.x, i, 0 })] = result;
			}
			if (doublegrid != NULL)
			{
				doublegrid[Grid_PointerArithmetic(griddim, &(Point){(int)gridp1.x, i, 0 })] = result;
			}
		}
	}
	else if (gridline->slope < 1 && gridline->slope > -1) //slope is between -1 and 1
	{
		for (i = gridline->bound_left > 0 ? gridline->bound_left : 0; i <= gridline->bound_right && i < griddim->x; i++) { //initial line
			int y = gridline->slope * (i + 0.5 - gridline->p1.x) + gridline->p1.y;
			if (y >= 0 && y < griddim->y)
			{
				if (booleangrid != NULL)
				{
					booleangrid[Grid_PointerArithmetic(griddim, &(Point){i, y, 0 })] = result;
				}
				if (doublegrid != NULL)
				{
					doublegrid[Grid_PointerArithmetic(griddim, &(Point){i, y, 0 })] = result;
				}
			}
		}
		if (thick) //thicc line
		{
			for (i = gridline->bound_up > 0 ? gridline->bound_up : 0; i <= gridline->bound_down && i < griddim->y; i++) {
				int x = (i - gridline->p1.y) / gridline->slope + gridline->p1.x;
				if (x >= 0 && x < griddim->x && x > gridline->bound_left && x < gridline->bound_right)
				{
					if (booleangrid != NULL)
					{
						booleangrid[Grid_PointerArithmetic(griddim, &(Point){x, i, 0 })] = result;
						if (i > 0)
						{
							booleangrid[Grid_PointerArithmetic(griddim, &(Point){x, i - 1, 0 })] = result;
						}
					}
					if (doublegrid != NULL)
					{
						doublegrid[Grid_PointerArithmetic(griddim, &(Point){x, i, 0 })] = result;
						if (i > 0)
						{
							doublegrid[Grid_PointerArithmetic(griddim, &(Point){x, i - 1, 0 })] = result;
						}
					}
				}
			}
		}
	}
	else //magnitude of slope is greater than 1
	{
		for (i = gridline->bound_up > 0 ? gridline->bound_up : 0; i <= gridline->bound_down && i < griddim->y; i++) {
			int x = (i + 0.5 - gridline->p1.y) / gridline->slope + gridline->p1.x;
			if (x >= 0 && x < griddim->x)
			{
				if (booleangrid != NULL)
				{
					booleangrid[Grid_PointerArithmetic(griddim, &(Point){x, i, 0 })] = result;
				}
				if (doublegrid != NULL)
				{
					doublegrid[Grid_PointerArithmetic(griddim, &(Point){x, i, 0 })] = result;
				}
			}
		}
		if (thick) //thicc line
		{
			for (i = gridline->bound_left > 0 ? gridline->bound_left : 0; i <= gridline->bound_right && i < griddim->x; i++) {
				int y = gridline->slope * (i - gridline->p1.x) + gridline->p1.y;
				if (y >= 0 && y < griddim->y && y > gridline->bound_up && y < gridline->bound_down)
				{
					if (booleangrid != NULL)
					{
						booleangrid[Grid_PointerArithmetic(griddim, &(Point){i, y, 0 })] = result;
						if (i > 0)
						{
							booleangrid[Grid_PointerArithmetic(griddim, &(Point){i - 1, y, 0 })] = result;
						}
					}
					if (doublegrid != NULL)
					{
						doublegrid[Grid_PointerArithmetic(griddim, &(Point){i, y, 0 })] = result;
						if (i > 0)
						{
							doublegrid[Grid_PointerArithmetic(griddim, &(Point){i - 1, y, 0 })] = result;
						}
					}
				}
			}
		}
	}
	free(gridline);
}

Grid* Grid_Create(Point* dim, BOOLEAN booleangrid, BOOLEAN doublegrid)
{
	Grid* g = malloc(sizeof(Grid));
	Point_Copy(&g->dim, dim);
	if (booleangrid)
	{
		g->booleangrid = malloc(dim->x * dim->y * sizeof(BOOLEAN));
	}
	if (doublegrid)
	{
		g->doublegrid = malloc(dim->x * dim->y * sizeof(double));
	}
	return g;
}
inline int Grid_PointerArithmetic(Point* dim, Point* pos)
{
	return pos->y * dim->x + pos->x;
}
inline void Grid_ArithmeticPointer(int* outx, int* outy, Point* dim, int ptr)
{
	*outx = ptr % Round(dim->x);
	*outy = (int)(ptr / dim->x);
}