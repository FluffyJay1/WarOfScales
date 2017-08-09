#include "MyOpenCL.h"
#include <malloc.h>

void InitializeMyOpenCL()
{
	FILE *fp;
	char *source_str;
	size_t source_size;
	size_t log_size;
	char *log;
	int i;
	fopen_s(&fp, "kernels.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	// Get platform and device information



	ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
	platforms = NULL;
	platforms = malloc(ret_num_platforms * sizeof(cl_platform_id));

	ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
	CL_PRDEBUG

	ret = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 1,
		&device_id, &ret_num_devices);
	CL_PRDEBUG
	// Create an OpenCL context
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	CL_PRDEBUG

	// Create a command queue
	command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
	CL_PRDEBUG

	// Create a program from the kernel source
	program = clCreateProgramWithSource(context, 1,
		(const char **)&source_str, (const size_t *)&source_size, &ret);
	CL_PRDEBUG

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	CL_PRDEBUG

	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	log = malloc(log_size);
	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
	printf("%s\n", log);
}
void LoadKernels()
{
	int i = 0;
#define X(k, v) + 1
	int count = 0 + KERNEL_MAP(X);
#undef X
	kernels = malloc(count * sizeof(cl_kernel));
#define X(k, v) kernels[k] = clCreateKernel(program, v, &ret);
	for (i = 0; i < count; i++)
	{
		KERNEL_MAP(X)
	}
#undef X
}

void CleanUpMyOpenCL()
{
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
#define X(k, v) ret = clReleaseKernel(kernels[k]);
	KERNEL_MAP(X)
#undef X
		ret = clReleaseProgram(program);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
}

