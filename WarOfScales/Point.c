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
void Point_Multiply2D(Point* output, Point* p, double d)
{
	output->x = p->x * d;
	output->y = p->y * d;
	output->z = p->z;
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
void Point_Normalize(Point* output, Point* p)
{
	if (Point_Magnitude(p) > 0)
	{
		Point_Multiply(output, p, 1 / Point_Magnitude(p));
	}
	else
	{
		*output = (Point) { 0, 0, 0 };
	}
}
void Point_Normalize2D(Point* output, Point* p)
{
	if (Point_Magnitude2D(p) > 0)
	{
		Point_Multiply2D(output, p, 1 / Point_Magnitude2D(p));
	}
	else
	{
		*output = (Point) { 0, 0, p->z };
	}
}
void Point_MapToGrid(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in)
{
	output->x = (in->x - pos->x) * griddim->x / mapdim->x;
	output->y = (in->y - pos->y) * griddim->y / mapdim->y;
	output->z = in->z;
}
void Point_GridToMap(Point* output, Point* griddim, Point* mapdim, Point* pos, Point* in)
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
/*
Format:
Next 3 tokens are the points x y z
*/
void Point_GenerateFromData(Point* out, char* data, char** returnpointer)
{
	char* sc = data;
	out->x = ParseDouble(sc, &sc);
	out->y = ParseDouble(sc + 1, &sc);
	out->z = ParseDouble(sc + 1, &sc);
	if (returnpointer)
	{
		*returnpointer = sc;
	}
}
BOOLEAN Point_IsLessThan2D(Point* p1, Point* p2)
{
	return p1->x < p2->x && p1->y < p2->y ? TRUE : FALSE;
}
inline BOOLEAN Point_IsInsideCircle(Point* p, Point* circlepos, double radius)
{
	return Point_Length(p, circlepos) < radius;
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
inline double Point_Length(Point* p1, Point* p2)
{
	return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y) + (p2->z - p1->z) * (p2->z - p1->z));
}
double Point_Length2D(Point* p1, Point* p2)
{
	return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y));
}
inline double Point_LengthSquared(Point* p1, Point* p2)
{
	return (p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y) + (p2->z - p1->z) * (p2->z - p1->z);
}
inline double Point_LengthSquared2D(Point* p1, Point* p2)
{
	return (p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y);
}
double Point_Magnitude(Point* p)
{
	return sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
}
double Point_Magnitude2D(Point* p)
{
	return sqrt(p->x * p->x + p->y * p->y);
}
void Point_Print(Point* p)
{
	printf("Point %i: (%f, %f, %f)\n", p, p->x, p->y, p->z);
}

PointArray* PointArray_Create(int size)
{
	PointArray* pa = malloc(sizeof(PointArray));
	pa->points = malloc(sizeof(Point) * size);
	pa->size = size;
	return pa;
}
void PointArray_Copy(PointArray* out, PointArray* in)
{
	int i;
	for (i = 0; i < in->size && i < out->size; i++)
	{
		Point_Copy(&out->points[i], &in->points[i]);
	}
}
void PointArray_Destroy(PointArray* pa)
{
	free(pa->points);
	free(pa);
}
/*
Format:
First token "points" is how many points are
For each point, the next 3 tokens are the point's x y z
*/
PointArray* PointArray_CreateFromData(char* data, char** returnpointer)
{
	char* sc;
	int i, points = ParseInt(data, &sc);
	Point p;
	PointArray* pa = PointArray_Create(points);
	for (i = 0; i < points; i++)
	{
		Point_GenerateFromData(&p, sc + 1, &sc);
		pa->points[i] = p;
	}
	if (returnpointer)
	{
		*returnpointer = sc;
	}
	return pa;
}
inline Line* PointArray_GetLine(PointArray* pa, int lineindex)
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
		n->Geom.Line = PointArray_GetLine(pa, i);
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
	Line_CreateAt(l, p1, p2);
	return l;
}
void Line_CreateAt(Line* l, Point* p1, Point* p2)
{
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
}
void Line_Copy(Line* out, Line* in)
{
	memcpy(out, in, sizeof(Line));
}
/*
Format:
Next 6 tokens are the line's p1x, p1y, p1z, p2x, p2y, p2z, not necessarily integers
*/
void Line_GenerateFromData(Line* out, char* data, char** returnpointer)
{
	char* sc = data;
	Point p1, p2;
	Point_GenerateFromData(&p1, sc, &sc);
	Point_GenerateFromData(&p2, sc + 1, &sc);
	Line_CreateAt(out, &p1, &p2);
	if (returnpointer)
	{
		*returnpointer = sc;
	}
}
/*
Format:
First token "times" is how many lines there are
For each line, the next 6 tokens are the line's p1x, p1y, p1z, p2x, p2y, p2z, not necessarily integers
*/
void Line_GenerateListFromData(NodeList* n, char* data, char** returnpointer)
{
	Node* linenode;
	char* sc;
	double p1x, p1y, p1z, p2x, p2y, p2z;
	int i, times = ParseInt(data, &sc);
	for (i = 0; i < times; i++)
	{	
		linenode = Node_Create();
		Line_GenerateFromData(linenode->Geom.Line, sc + 1, &sc);
		NodeList_AddNode(n, linenode);
	}
	if (returnpointer)
	{
		*returnpointer = sc;
	}
}
BOOLEAN Line_Collides(Point* out, Line* l1, Line* l2, BOOLEAN considerHeight)
{
	Point ret;
	if ( !l1 || !l2 //either line is null
		|| l1->bound_left > l2->bound_right || l1->bound_right < l2->bound_left || l1->bound_up > l2->bound_down || l1->bound_down < l2->bound_up 
		|| (considerHeight && l1->p1.z < l2->p1.z && l1->p2.z < l2->p1.z && l1->p1.z < l2->p2.z && l1->p2.z < l2->p2.z))
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
		&& ret.y >= l1->bound_up && ret.y >= l2->bound_up && ret.y <= l1->bound_down && ret.y <= l2->bound_down
		&& (!considerHeight || (Line_HeightAt(l1, &ret) <= Line_HeightAt(l2, &ret))))
	{
		Point_Copy(out, &ret);
		return TRUE;
	}
	return FALSE;
}
/*
outputs will be calculated if the pointers given aren't null

outcoll is the entire list of collisions, grouped such that l1 collisions are together
outcoll 1 is for if each line1 collides
outcoll 2 is for if each line2 collides
outpos is the entire list of collision locations
outpos1 gives the collision location closest to l1's p1
outpos2 gives the collision location closest to l2's p1
*/
void Line_CollidesBatch(BOOLEAN* outcoll, BOOLEAN* outcoll1, BOOLEAN* outcoll2, Point* outpos, Point* outpos1, Point* outpos2, NodeList* nl1, NodeList* nl2, BOOLEAN considerHeight)
{
	if (usegpu)
	{
		Node *iterator, *iterator1, *iterator2;
		int i, ind1, ind2;
		//int index;
		BOOLEAN* outputcollides;
		double *outputx, *outputy, *l1p1x, *l1p1y, *l1p2x, *l1p2y, *l2p1x, *l2p1y, *l2p2x, *l2p2y, *l1slope, *l2slope,
			*l1bound_up, *l1bound_down, *l1bound_left, *l1bound_right, *l2bound_up, *l2bound_down, *l2bound_left, *l2bound_right, *l1p1z, *l1p2z, *l2p1z, *l2p2z;
		BOOLEAN *l1p1down, *l1vertical, *l2p1down, *l2vertical, *l1null, *l2null;
		size_t l1sized = sizeof(double) * nl1->listSize;
		size_t l1sizeb = sizeof(BOOLEAN) * nl1->listSize;
		size_t l2sized = sizeof(double) * nl2->listSize;
		size_t l2sizeb = sizeof(BOOLEAN) * nl2->listSize;
		opencl_global_item_size = nl1->listSize * nl2->listSize;
		opencl_local_item_size = nl2->listSize;

		//malloc everything
		outputcollides = malloc(sizeof(BOOLEAN) * nl1->listSize * nl2->listSize);
		outputx = malloc(sizeof(double) * nl1->listSize * nl2->listSize);
		outputy = malloc(sizeof(double) * nl1->listSize * nl2->listSize);
#define X(k, v, j) k = malloc(v);
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		l1null = malloc(l1sizeb);
		l2null = malloc(l2sizeb);

		//write to arrays
#define X(k, v, j, l) k[iterator->index] = iterator->Geom.Line->l;
		iterator = nl1->headNode;
		//index = 0;
		while (iterator != NULL)
		{
			if (iterator->Geom.Line)
			{
				LINECOLLISION_L1MAP(X);
				l1null[iterator->index] = FALSE;
			}
			else
			{
				l1null[iterator->index] = TRUE;
			}
			iterator = iterator->nextNode;
			//index++;
		}
		iterator = nl2->headNode;
		//index = 0;
		while (iterator != NULL)
		{
			if (iterator->Geom.Line)
			{
				LINECOLLISION_L2MAP(X);
				l2null[iterator->index] = FALSE;
			}
			else
			{
				l2null[iterator->index] = TRUE;
			}
			iterator = iterator->nextNode;
			//index++;
		}
#undef X

		//create memobjs
		outputcollides_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(BOOLEAN) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
		outputx_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(double) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
		outputy_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(double) * nl1->listSize * nl2->listSize, NULL, &ret); CL_PRDEBUG;
#define X(k, v, j) l_mem[j] = clCreateBuffer(context, CL_MEM_READ_ONLY, v, NULL, &ret); CL_PRDEBUG;
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		considerHeight_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(BOOLEAN), NULL, &ret);
		l1null_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY, l1sizeb, NULL, &ret);
		l2null_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY, l2sizeb, NULL, &ret);

		//write to memobjs
#define X(k, v, j) ret = clEnqueueWriteBuffer(command_queue, l_mem[j], CL_TRUE, 0, v, k, 0, NULL, NULL);
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		ret = clEnqueueWriteBuffer(command_queue, considerHeight_memobj, CL_TRUE, 0, sizeof(BOOLEAN), &considerHeight, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, l1null_memobj, CL_TRUE, 0, l1sizeb, l1null, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, l2null_memobj, CL_TRUE, 0, l2sizeb, l2null, 0, NULL, NULL);

		//set args
		ret = clSetKernelArg(kernels[LineCollision], 0, sizeof(cl_mem), (void *)&outputcollides_memobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[LineCollision], 1, sizeof(cl_mem), (void *)&outputx_memobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[LineCollision], 2, sizeof(cl_mem), (void *)&outputy_memobj); CL_PRDEBUG;
#define X(k, v, j, l, i) ret = clSetKernelArg(kernels[LineCollision], i, sizeof(cl_mem), (void *)&l_mem[j]); CL_PRDEBUG;
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		ret = clSetKernelArg(kernels[LineCollision], 29, sizeof(cl_mem), (void *)&considerHeight_memobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[LineCollision], 30, sizeof(cl_mem), (void *)&l1null_memobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[LineCollision], 31, sizeof(cl_mem), (void *)&l2null_memobj); CL_PRDEBUG;
		//do the shit
		ret = clEnqueueNDRangeKernel(command_queue, kernels[LineCollision], 1, NULL,
			&opencl_global_item_size, &opencl_local_item_size, 0, NULL, NULL);
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

		if (outcoll != NULL)
		{
			memcpy(outcoll, outputcollides, sizeof(BOOLEAN) * nl1->listSize * nl2->listSize);
		}
		if (outcoll1 != NULL)
		{
			SetArray(outcoll1, NULL, nl1->listSize, FALSE);
		}
		if (outcoll2 != NULL)
		{
			SetArray(outcoll2, NULL, nl2->listSize, FALSE);
		}
		/* //commented out for redundancy
		if (outpos != NULL)
		{
			for (i = 0; i < nl1->listSize * nl2->listSize; i++)
			{
				if (outputcollides[i])
				{
					outpos[i] = (Point) { outputx[i], outputy[i], 0 };
				}
			}
		}
		*/

		for (iterator1 = nl1->headNode, ind1 = 0; iterator1 != NULL; iterator1 = iterator1->nextNode, ind1++)
		{
			for (iterator2 = nl2->headNode, ind2 = 0; iterator2 != NULL; iterator2 = iterator2->nextNode, ind2++)
			{
				i = ind1 * nl2->listSize + ind2;
				if (outputcollides[i])
				{
					if (outpos != NULL)
					{
						outpos[i] = (Point) { outputx[i], outputy[i], 0 };
					}
					if (outcoll1 != NULL)
					{
						if (outpos1 != NULL)
						{
							if (outcoll1[ind1] == FALSE || //if is first time we see collision, or if it's closer to p1 than the one previously there
								Point_LengthSquared2D(&(Point) { l1p1x[ind1], l1p1y[ind1], 0 }, &(Point) { outputx[i], outputy[i], 0 }) < Point_LengthSquared2D(&(Point) { l1p1x[ind1], l1p1y[ind1], 0 }, &outpos1[ind1]))
							{
								outpos1[ind1] = (Point) { outputx[i], outputy[i], 0 };
							}
						}
						outcoll1[ind1] = TRUE;

					}
					if (outcoll2 != NULL)
					{
						if (outpos2 != NULL)
						{
							if (outcoll2[ind2] == FALSE || //if is first time we see collision, or if it's closer to p1 than the one previously there
								Point_LengthSquared2D(&(Point) { l2p1x[ind2], l2p1y[ind2], 0 }, &(Point) { outputx[i], outputy[i], 0 }) < Point_LengthSquared2D(&(Point) { l2p1x[ind2], l2p1y[ind2], 0 }, &outpos2[ind2]))
							{
								outpos2[ind2] = (Point) { outputx[i], outputy[i], 0 };
							}
						}
						outcoll2[ind2] = TRUE;

					}
				}
			}
		}
		ret = clReleaseMemObject(outputcollides_memobj);
		ret = clReleaseMemObject(outputx_memobj);
		ret = clReleaseMemObject(outputy_memobj);
#define X(k, v, j) ret = clReleaseMemObject(l_mem[j]);
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		ret = clReleaseMemObject(considerHeight_memobj);
		ret = clReleaseMemObject(l1null_memobj);
		ret = clReleaseMemObject(l2null_memobj);
		free(outputcollides);
		free(outputx);
		free(outputy);
#define X(k) free(k);
		LINECOLLISION_L1MAP(X);
		LINECOLLISION_L2MAP(X);
#undef X
		free(l1null);
		free(l2null);
	}
	else //no gpu
	{
		Node *iterator1, *iterator2;
		int ind1, ind2;
		Point p;
		BOOLEAN collides;
		if (outcoll1 != NULL)
		{
			SetArray(outcoll1, NULL, nl1->listSize, FALSE);
		}
		if (outcoll2 != NULL)
		{
			SetArray(outcoll2, NULL, nl2->listSize, FALSE);
		}
		for (iterator1 = nl1->headNode, ind1 = 0; iterator1 != NULL; iterator1 = iterator1->nextNode, ind1++)
		{
			for (iterator2 = nl2->headNode, ind2 = 0; iterator2 != NULL; iterator2 = iterator2->nextNode, ind2++)
			{
				collides = Line_Collides(&p, iterator1->Geom.Line, iterator2->Geom.Line, considerHeight);
				if (outcoll != NULL)
				{
					outcoll[ind1 * nl2->listSize + ind2] = collides;
				}
				if (collides)
				{
					if (outpos != NULL)
					{
						outpos[ind1 * nl2->listSize + ind2] = p;
					}
					if (outcoll1 != NULL) 
					{
						if (outpos1 != NULL && (outcoll1[ind1] == FALSE || //if is first time we see collision, or if it's closer to p1 than the one previously there
							Point_LengthSquared2D(&p, &iterator1->Geom.Line->p1) < Point_LengthSquared2D(&outpos1[ind1], &iterator1->Geom.Line->p1)))
						{
							outpos1[ind1] = p;
						}
						outcoll1[ind1] = TRUE;
					}
					if (outcoll2 != NULL)
					{
						if (outpos2 != NULL && (outcoll2[ind2] == FALSE || //if is first time we see collision, or if it's closer to p1 than the one previously there
							Point_LengthSquared2D(&p, &iterator2->Geom.Line->p1) < Point_LengthSquared2D(&outpos2[ind2], &iterator2->Geom.Line->p1)))
						{
							outpos2[ind2] = p;
						}
						outcoll1[ind2] = TRUE;
					}
				}
			}
		}
	}
}
BOOLEAN Line_CollidesCircle(Point* out, Line* line, Point* circlepos, double circleradius)
{
	double intercept, x1, x2, y1, y2, f, g, h, det; //used for quadratic equation
	if (line->bound_left > circlepos->x + circleradius || line->bound_right < circlepos->x - circleradius //check bounding boxes
		|| line->bound_up > circlepos->y + circleradius || line->bound_down < circlepos->y - circleradius)
	{
		return FALSE;
	}
	if (out == NULL) //if we don't need the location of collision, we can take some shortcuts
	{
		if (Point_IsInsideCircle(&line->p1, circlepos, circleradius) + Point_IsInsideCircle(&line->p2, circlepos, circleradius) == 1)//if only 1 point is inside the circle
		{
			return TRUE;
		}
	}
	if (!line->vertical)
	{
		intercept = line->p1.y - line->p1.x * line->slope;
		f = 1 + line->slope * line->slope;
		g = line->slope * (intercept - circlepos->y) - circlepos->x;
		h = circlepos->x * circlepos->x + circlepos->y * circlepos->y + intercept * intercept - 2 * circlepos->y * intercept - circleradius * circleradius;
		det = g * g - f * h;
		if (out == NULL) //despite what it seems this is not redundant
		{
			return det >= 0 ? TRUE : FALSE;
		}
		if (det >= 0)
		{
			x1 = (-g + sqrt(det)) / f;
			x2 = (-g - sqrt(det)) / f;
			y1 = line->slope * x1 + intercept;
			y2 = line->slope * x2 + intercept;
			if (Point_LengthSquared(&(Point) { x1, y1, 0 }, &line->p1) < Point_LengthSquared(&(Point) { x2, y2, 0 }, &line->p2) //point associated with x1 is closer to p1
				&& x1 < line->bound_right + EPSILON && x1 > line->bound_left - EPSILON) //make sure the point is on the line
			{
				out->x = x1;
				out->y = y1;
			}
			else if (x2 < line->bound_right + EPSILON && x2 > line->bound_left - EPSILON)
			{
				out->x = x2;
				out->y = y2;
			}
			else
			{
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}
	//if we get to this point it's a vertical line
	det = (line->p1.x - circlepos->x) * (line->p1.x - circlepos->x) - circleradius * circleradius;
	if (out == NULL) //despite what it seems this is not redundant
	{
		return det >= 0 ? TRUE : FALSE;
	}
	if (det >= 0)
	{
		y1 = circlepos->y + sqrt(det);
		y2 = circlepos->y - sqrt(det);
		out->x = line->p1.x;
		if (abs(y1 - line->p1.y) < abs(y2 - line->p2.y) //y1 is closer to p1.y or not
			&& y1 < line->bound_down + EPSILON && y1 > line->bound_up - EPSILON)
		{
			out->y = y1;
		}
		else if(y2 < line->bound_down + EPSILON && y2 > line->bound_up - EPSILON)
		{
			out->y = y2;
		}
		else
		{
			return FALSE; //i think theres a better way to do the if else logic tree here but whatevs
		}
		return TRUE;
	}
	return FALSE;
}
void Line_Rasterize(BOOLEAN* booleangrid, double* doublegrid, double result, BOOLEAN thick, Point* griddim, Point* mapdim, Point* pos, Line* line)
{
	//output->x = (int)((in->x - dfield->pos.x) * dfield->dim.x / dfield->mapdim.x)
	int i;
	Point gridp1, gridp2;
	Line* gridline;
	Point_MapToGrid(&gridp1, griddim, mapdim, pos, &line->p1);
	Point_MapToGrid(&gridp2, griddim, mapdim, pos, &line->p2);
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
				if (x >= 0 && x < griddim->x && x >= (int)gridline->bound_left && x <= (int)gridline->bound_right)
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
				if (y >= 0 && y < griddim->y && y >= (int)gridline->bound_up && y <= (int)gridline->bound_down)
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

double Line_HeightAt(Line *l, Point *p)
{
	if (l->length != 0)
	{
		if (!l->vertical)
		{
			return l->p1.z + ((p->x - l->p1.x) / (l->p2.x - l->p1.x)) * (l->p2.z - l->p1.z);
		}
		else
		{
			return l->p1.z + ((p->y - l->p1.y) / (l->p2.y - l->p1.y)) * (l->p2.z - l->p1.z);
		}
	}
	return Max(l->p1.z, l->p2.z);
}

Grid* Grid_Create(Point* dim, BOOLEAN booleangrid, BOOLEAN doublegrid)
{
	Grid* g = malloc(sizeof(Grid));
	Point_Copy(&g->dim, dim);
	if (booleangrid)
	{
		g->booleangrid = malloc(dim->x * dim->y * sizeof(BOOLEAN));
	}
	else
	{
		g->booleangrid = NULL;
	}
	if (doublegrid)
	{
		g->doublegrid = malloc(dim->x * dim->y * sizeof(double));
	}
	else
	{
		g->doublegrid = NULL;
	}
	return g;
}
void Grid_Destroy(Grid* grid)
{
	if (grid->booleangrid)
	{
		free(grid->booleangrid);
	}
	if (grid->doublegrid)
	{
		free(grid->doublegrid);
	}
	free(grid);
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