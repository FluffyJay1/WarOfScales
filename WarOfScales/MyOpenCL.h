#pragma once
#include "CL\cl.h"
#include <stdio.h>
#define MAX_SOURCE_SIZE (0x100000)
#define DEBUG 0
#define CL_PRDEBUG //if(DEBUG)printf("ret at %d is %d\n", __LINE__, ret);

#define KERNEL_MAP(X) \
X(Diffuse, "diffuse") \
X(LineCollision, "linecollision")

#define X(k, v) k,
enum KernelType
{
	KERNEL_MAP(X)
};
#undef X

cl_device_id device_id;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;
cl_platform_id *platforms;
cl_context context;
cl_command_queue command_queue;
cl_program program;
cl_kernel* kernels;

void InitializeMyOpenCL();
void LoadKernels();
void CleanUpMyOpenCL();