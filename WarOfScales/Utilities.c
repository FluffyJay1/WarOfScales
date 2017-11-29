#include "Utilities.h"


void SetArray(BOOLEAN* booleanarray, double* doublearray, int elements, double val)
{
	if (usegpu)
	{
		opencl_global_item_size = elements;
		opencl_local_item_size = 1;
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
				&opencl_global_item_size, &opencl_local_item_size, 0, NULL, NULL);
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
				&opencl_global_item_size, &opencl_local_item_size, 0, NULL, NULL);
			CL_PRDEBUG;
			ret = clEnqueueReadBuffer(command_queue, arrmemobj, CL_TRUE, 0,
				elements * sizeof(double), doublearray, 0, NULL, NULL);
			CL_PRDEBUG;
		}
		ret = clReleaseMemObject(arrmemobj);
		ret = clReleaseMemObject(valmemobj);
	}
	else
	{
		int i;
		for (i = 0; i < elements; i++)
		{
			if (booleanarray != NULL)
			{
				booleanarray[i] = val;
			}
			if (doublearray != NULL)
			{
				doublearray[i] = val;
			}
		}
	}
}
BOOLEAN float_IsEqual(float left, float right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
BOOLEAN double_IsEqual(double left, double right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
double Min(double a, double b) 
{
	return a < b ? a : b;
}
double Max(double a, double b)
{
	return a > b ? a : b;
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
int ParseInt(char* in, char** returnpointer)
{
	int sd = 0, i, out = 0;
	BOOLEAN negative = FALSE;
	if (in[sd] == '-')
	{
		negative = TRUE;
		sd++;
	}
	while (in[sd] >= 48 && in[sd] <= 57)
	{
		out *= 10;
		out += in[sd] - 48;
		sd++;
	}
	if (returnpointer)
	{
		*returnpointer = &in[sd];
	}
	return negative ? -out : out;
}
double ParseDouble(char* in, char** returnpointer)
{
	int sd = 0, decimal = 1, i;
	double out = 0, d;
	BOOLEAN negative = FALSE;
	if (in[sd] == '-')
	{
		negative = TRUE;
		sd++;
	}
	for (; in[sd] >= 48 && in[sd] <= 57; sd++) //while input is a number
	{
		out *= 10;
		out += in[sd] - 48;
	}
	if (in[sd] == 46)
	{
		for (sd++; in[sd] >= 48 && in[sd] <= 57; sd++, decimal++) //while input is a number
		{
			d = in[sd] - 48;
			for (i = 0; i < decimal; i++)
			{
				d /= 10;
			}
			out += d;
		}
	}
	if (returnpointer)
	{
		*returnpointer = &in[sd];
	}
	//for whatever reason returning out isn't necessary, black magic
	return negative ? -out : out;
}
//This gives the rounded down sqrt of a power of 2
int SqrtPower2(int in)
{
	int comp = 1, ret = in;
	while (comp < in) {
		comp <<= 1;
		ret >>= 1;
	}
	return ret;
}
void Fuck(int** one, int* two)
{
	*one = two;
}