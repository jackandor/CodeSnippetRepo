
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <CL/cl.h>

const char *prog =
"__kernel void matvec_mult(__global float4* matrix, __global float4* vector, __global float* result) {\n"
"  int i = get_global_id(0);\n"
"  result[i] = dot(matrix[i], vector[0]);\n"
"}\n";

int main()
{
	cl_platform_id *platforms;
	cl_uint num_platforms;
	clGetPlatformIDs(0, NULL, &num_platforms);
	platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
	clGetPlatformIDs(num_platforms, platforms, NULL);
	cl_device_id device;
	clGetDeviceIDs(platforms[2], CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context;
	cl_int err;
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	cl_program program;
	size_t sz = strlen(prog);
	program = clCreateProgramWithSource(context, 1, &prog, &sz, &err);
	clBuildProgram(program, 1, &device, "-cl-std=CL1.1 -Werror", NULL, NULL);
	cl_build_status bs;
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &bs, NULL);
	if (bs < 0) {
		size_t sz = 0;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &sz);
		char *log = (char *)malloc(sz + 1);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sz + 1, log, NULL);
		printf("%s\n", log);
		free(log);
	}
	cl_kernel kernel;
	clCreateKernelsInProgram(program, 1, &kernel, NULL);
	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, NULL, &err);
	float mat[16];
	for (int i = 0; i < 16; i++)
		mat[i] = i * 2.0f;
	cl_mem mat_buff;
	mat_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 16, mat, &err);
	float vec[4];
	for (int i = 0; i < 4; i++)
		vec[i] = i * 3.0f;
	cl_mem vec_buff;
	vec_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 4, vec, &err);
	cl_mem res_buff;
	res_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * 4, NULL, &err);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &mat_buff);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &vec_buff);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &res_buff);
	size_t work_uints_per_kernel = 4;
	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_uints_per_kernel, NULL, 0, NULL, NULL);
	float result[4];
	clEnqueueReadBuffer(queue, res_buff, CL_TRUE, 0, sizeof(float) * 4, result, 0, NULL, NULL);
	printf("result:(%f,%f,%f,%f)", result[0], result[1], result[2], result[3]);
	clReleaseMemObject(res_buff);
	clReleaseMemObject(vec_buff);
	clReleaseMemObject(mat_buff);
	clReleaseCommandQueue(queue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseContext(context);
    return 0;
}
