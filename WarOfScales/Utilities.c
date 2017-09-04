#include "Utilities.h"


void SetArray(BOOLEAN* booleanarray, double* doublearray, int elements, double val)
{
	global_item_size = elements;
	local_item_size = 1;
	if (booleanarray != NULL)
	{
		BOOLEAN temp = val;
		arrmemobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(BOOLEAN) * elements, NULL, &ret);
		CL_PRDEBUG;
		valmemobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(BOOLEAN), NULL, &ret);
		CL_PRDEBUG;

		ret = clEnqueueWriteBuffer(command_queue, valmemobj, CL_TRUE, 0,
			sizeof(BOOLEAN), &temp, 0, NULL, NULL); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[SetArrayBoolean], 0, sizeof(cl_mem), (void *)&arrmemobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[SetArrayBoolean], 1, sizeof(cl_mem), (void *)&valmemobj); CL_PRDEBUG;
		ret = clEnqueueNDRangeKernel(command_queue, kernels[SetArrayBoolean], 1, NULL,
			&global_item_size, &local_item_size, 0, NULL, NULL);
		CL_PRDEBUG;
		ret = clEnqueueReadBuffer(command_queue, arrmemobj, CL_TRUE, 0,
				elements * sizeof(BOOLEAN), booleanarray, 0, NULL, NULL);
		CL_PRDEBUG;
	}
	else if (doublearray != NULL)
	{
		double temp = val;
		arrmemobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(double) * elements, NULL, &ret);
		CL_PRDEBUG;
		valmemobj = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(double), NULL, &ret);
		CL_PRDEBUG;

		ret = clEnqueueWriteBuffer(command_queue, valmemobj, CL_TRUE, 0,
			sizeof(double), &temp, 0, NULL, NULL);
		ret = clSetKernelArg(kernels[SetArrayDouble], 0, sizeof(cl_mem), (void *)&arrmemobj); CL_PRDEBUG;
		ret = clSetKernelArg(kernels[SetArrayDouble], 1, sizeof(cl_mem), (void *)&valmemobj); CL_PRDEBUG;
		ret = clEnqueueNDRangeKernel(command_queue, kernels[SetArrayDouble], 1, NULL,
			&global_item_size, &local_item_size, 0, NULL, NULL);
		CL_PRDEBUG;
		ret = clEnqueueReadBuffer(command_queue, arrmemobj, CL_TRUE, 0,
			elements * sizeof(double), doublearray, 0, NULL, NULL);
		CL_PRDEBUG;
	}
	ret = clReleaseMemObject(arrmemobj);
	ret = clReleaseMemObject(valmemobj);
}
BOOLEAN float_IsEqual(float left, float right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
BOOLEAN double_IsEqual(double left, double right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
int Round(double input)
{
	int flooredInput = input;

	if (input - flooredInput > 0.5)
	{
		return flooredInput + 1;
	}
	else
	{
		return flooredInput;
	}
}
int ParseInt(char* in)
{
	int sd = 0, i, out = 0;
	while (in[sd] >= 48 && in[sd] <= 57)
	{
		out *= 10;
		out += in[sd] - 48;
		sd++;
	}
	return out;
}
void Fuck(int** one, int* two)
{
	*one = two;
}