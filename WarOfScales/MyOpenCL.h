#pragma once
#include "CL\cl.h"
#include <stdio.h>

#define MAX_SOURCE_SIZE (0x100000)
#define DEBUG 0
#define CL_PRDEBUG if(DEBUG)printf("ret at %d is %d\n", __LINE__, ret);
#define USE_GPU 0

#define KERNEL_MAP(X) \
X(SetArrayBoolean, "setarrayboolean") \
X(SetArrayDouble, "setarraydouble") \
X(Diffuse, "diffuse") \
X(LineCollision, "linecollision")

#define X(k, v) k,
enum KernelType
{
	KERNEL_MAP(X)
};
#undef X

cl_device_id device_id;
cl_device_id temp_device_id;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;
cl_platform_id *platforms;
cl_context context;
cl_command_queue command_queue;
cl_program program;
cl_kernel* kernels;

size_t opencl_global_item_size;
size_t opencl_local_item_size;
size_t* opencl_global_item_size_2d;
size_t* opencl_local_item_size_2d;
size_t* opencl_offset_2d;
cl_mem arrmemobj;
cl_mem valmemobj;
size_t opencl_max_work_group_size;
size_t sqrt_opencl_max_work_group_size;

char usegpu;

void InitializeMyOpenCL();
void LoadKernels();
void CleanUpMyOpenCL();