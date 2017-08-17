#include "DiffusionField.h"

void DiffusionField_Initialize()
{

}
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
	dfield->mapdim = *Point_Create(0, 0, 0);
	dfield->pos = *Point_Create(0, 0, 0);
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
	free(dfield->D);
	free(dfield->lambda);
	free(dfield->next);
	free(dfield->diffusable);
	NodeList_Clear(dfield->agents);
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
	global_item_size = dfield->dim.x * dfield->dim.y;
	local_item_size = dfield->dim.x;
	outputfield_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		dfield->dim.x * dfield->dim.y * sizeof(double), NULL, &ret);
	CL_PRDEBUG
	inputfield_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			dfield->dim.x * dfield->dim.y * sizeof(double), NULL, &ret);
	CL_PRDEBUG
	d_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			dfield->dim.x * dfield->dim.y * sizeof(double), NULL, &ret);
	CL_PRDEBUG
	lambda_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			dfield->dim.x * dfield->dim.y * sizeof(double), NULL, &ret);
	CL_PRDEBUG
	diffusable_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			dfield->dim.x * dfield->dim.y * sizeof(BOOLEAN), NULL, &ret);
	CL_PRDEBUG
	width_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(double), NULL, &ret);
	CL_PRDEBUG
	height_memobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(double), NULL, &ret);
	CL_PRDEBUG
	//double* next = malloc(sizeof(double) * dfield->dim.x * dfield->dim.y);
	for (i = 0; i < times; i++)
	{
		ret = clEnqueueWriteBuffer(command_queue, inputfield_memobj, CL_TRUE, 0,
			dfield->dim.x * dfield->dim.y * sizeof(double), dfield->field, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueWriteBuffer(command_queue, d_memobj, CL_TRUE, 0,
			dfield->dim.x * dfield->dim.y * sizeof(double), dfield->D, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueWriteBuffer(command_queue, lambda_memobj, CL_TRUE, 0,
			dfield->dim.x * dfield->dim.y * sizeof(double), dfield->lambda, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueWriteBuffer(command_queue, diffusable_memobj, CL_TRUE, 0,
			dfield->dim.x * dfield->dim.y * sizeof(BOOLEAN), dfield->diffusable, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueWriteBuffer(command_queue, width_memobj, CL_TRUE, 0,
			sizeof(double), &dfield->dim.x, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueWriteBuffer(command_queue, height_memobj, CL_TRUE, 0,
			sizeof(double), &dfield->dim.y, 0, NULL, NULL);
		CL_PRDEBUG

		ret = clSetKernelArg(kernels[Diffuse], 0, sizeof(cl_mem), (void *)&outputfield_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 1, sizeof(cl_mem), (void *)&inputfield_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 2, sizeof(cl_mem), (void *)&d_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 3, sizeof(cl_mem), (void *)&lambda_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 4, sizeof(cl_mem), (void *)&diffusable_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 5, sizeof(cl_mem), (void *)&width_memobj); CL_PRDEBUG
		ret = clSetKernelArg(kernels[Diffuse], 6, sizeof(cl_mem), (void *)&height_memobj);CL_PRDEBUG
			
		ret = clEnqueueNDRangeKernel(command_queue, kernels[Diffuse], 1, NULL,
			&global_item_size, &local_item_size, 0, NULL, NULL);
		CL_PRDEBUG
		ret = clEnqueueReadBuffer(command_queue, outputfield_memobj, CL_TRUE, 0,
			dfield->dim.x * dfield->dim.y * sizeof(double), dfield->next, 0, NULL, NULL);
		CL_PRDEBUG
		/*
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
		*/
		memcpy(dfield->field, dfield->next, sizeof(double) * dfield->dim.x * dfield->dim.y);
	}
	ret = clReleaseMemObject(outputfield_memobj);
	ret = clReleaseMemObject(inputfield_memobj);
	ret = clReleaseMemObject(d_memobj);
	ret = clReleaseMemObject(lambda_memobj);
	ret = clReleaseMemObject(diffusable_memobj);
	ret = clReleaseMemObject(width_memobj);
	ret = clReleaseMemObject(height_memobj);
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
void DiffusionField_Clear(DiffusionField* dfield)
{
	DiffusionField_SetArray(dfield->field, NULL, &dfield->dim, 0);
	DiffusionField_SetArray(dfield->D, NULL, &dfield->dim, DEFAULT_D);
	DiffusionField_SetArray(dfield->lambda, NULL, &dfield->dim, 1);
	DiffusionField_SetArray(NULL, dfield->diffusable, &dfield->dim, TRUE);
	NodeList_Clear(dfield->agents);
}
void DiffusionField_Scan(Point* output, DiffusionField* dfield, Point* pos, double radius)
{
	int dx, dy;
	double largestVal = 0;
	Point largestPos;
	Point_Copy(&largestPos, pos);
	for (dx = -radius; dx <= radius; dx++)
	{
		for (dy = -radius; dy <= radius; dy++)
		{
			if (dx * dx + dy * dy <= radius * radius)
			{
				Point p = (Point) { dx + pos->x, dy + pos->y, 0 };
				if(Point_IsInside2D(&dfield->dim, &p) && dfield->field[DiffusionField_PointerArithmetic(&dfield->dim, &p)] > largestVal)
				{
					Point_Copy(&largestPos, &p);
					largestVal = dfield->field[DiffusionField_PointerArithmetic(&dfield->dim, &p)];
				}
			}
		}
	}
	Point_Copy(output, &largestPos);
}
void DiffusionField_Point_MapToField(Point* output, DiffusionField* dfield, Point* in)
{
	output->x = (int)((in->x - dfield->pos.x) * dfield->dim.x / dfield->mapdim.x);
	output->y = (int)((in->y - dfield->pos.y) * dfield->dim.y / dfield->mapdim.y);
	output->z = in->z;
}
void DiffusionField_Point_FieldToMap(Point* output, DiffusionField* dfield, Point* in)
{
	output->x = in->x * dfield->mapdim.x / dfield->dim.x + dfield->pos.x;
	output->y = in->y * dfield->mapdim.y / dfield->dim.y + dfield->pos.y;
	output->z = in->z;
}
inline int DiffusionField_PointerArithmetic(Point* dim, Point* pos)
{
	return pos->y * dim->x + pos->x;
}
inline void DiffusionField_ArithmeticPointer(int* outx, int* outy, Point* dim, int ptr)
{
	*outx = ptr % Round(dim->x);
	*outy = (int)(ptr / dim->x);
}
void DiffusionField_Draw(SDL_Renderer* renderer, DiffusionField* dfield, Camera* camera)
{
	int x, y;
	for (x = 0; x < dfield->dim.x; x++)
	{
		for (y = 0; y < dfield->dim.y; y++)
		{
			SDL_Rect dstrect;
			double val = dfield->field[DiffusionField_PointerArithmetic(&dfield->dim, &(Point){ x, y, 0 })];
			double alpha = val != 0 ? 255. * (1. - pow(1.1, -val)) : 0;
			dstrect.x = (x * dfield->mapdim.x / dfield->dim.x - camera->pos.x) * camera->scale + WINDOW_WIDTH/2;
			dstrect.y = (y * dfield->mapdim.y / dfield->dim.y - camera->pos.y) * camera->scale + WINDOW_HEIGHT/2;
			dstrect.w = dfield->mapdim.x / dfield->dim.x * camera->scale;
			dstrect.h = dfield->mapdim.y / dfield->dim.y * camera->scale;
			SDL_SetTextureAlphaMod(dfieldtexture, alpha);
			SDL_SetTextureColorMod(dfieldtexture, alpha, alpha, alpha);
			SDL_RenderCopy(renderer, dfieldtexture, NULL, &dstrect);
		}
	}
}