/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include "util_cl.h"
#include "util_debug.h"


static char s_str_kernel[] ="               \n\
__kernel void vecAdd(__global float* a)     \n\
{                                           \n\
    int gid = get_global_id(0);             \n\
                                            \n\
    a[gid] += a[gid];                       \n\
}                                           \n";


int
main (int argc, char *argv[])
{
    cl_context  ctx;
    cl_command_queue queue;
    cl_kernel   kernel;
    cl_mem      memobj;
    cl_int      ret;
    int i;

    float mem[64];
    for (i = 0; i < 64; i++)
    {
        mem[i] = (float)i;
    }

    ret = cl_init ();
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateBuffer\n");

    ctx   = cl_get_context ();
    queue = cl_get_cmd_queue ();

    kernel = cl_build_kernel (s_str_kernel, "vecAdd");
    cl_query_kernel_info (kernel);

    memobj = clCreateBuffer (ctx, CL_MEM_READ_WRITE, 64 * sizeof(float), NULL, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateBuffer\n");

    ret = clEnqueueWriteBuffer (queue, memobj, CL_TRUE, 0, 64 * sizeof(float), mem, 0, NULL, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clEnqueueWriteBuffer\n");

    ret = clSetKernelArg (kernel, 0, sizeof(cl_mem), (void *)&memobj);
    DBG_ASSERT (ret == CL_SUCCESS, "clSetKernelArg\n");

    size_t global_work_size[3] = {64, 0, 0};
    size_t local_work_size[3]  = {64, 0, 0};
    ret = clEnqueueNDRangeKernel (queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clEnqueueNDRangeKernel");

    ret = clEnqueueReadBuffer (queue, memobj, CL_TRUE, 0, 64 * sizeof(float), mem, 0, NULL, NULL);

    for (i = 0; i < 64; i++)
    {
        fprintf (stderr, "mem[%3d] : %f\n", i, mem[i]);
    }

    return 0;
}
