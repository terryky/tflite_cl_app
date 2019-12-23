/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include "util_cl.h"
#include "util_debug.h"



cl_context          s_ctx;
cl_device_id        s_dev;
cl_command_queue    s_cmdq;

int
cl_init ()
{
    cl_int          ret;
    cl_platform_id  platform_id;
    cl_uint         num_devices;
    
    ret = clGetPlatformIDs (1, &platform_id, &num_devices);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetPlatformIDs\n");

    ret = clGetDeviceIDs (platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &s_dev, &num_devices);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetDeviceIDs\n");

    
    s_ctx = clCreateContext (NULL, 1, &s_dev, NULL, NULL, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateContext\n");

    s_cmdq = clCreateCommandQueue (s_ctx, s_dev, 0, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateCommandQueue\n");

    return CL_SUCCESS;
}
 


cl_context
cl_get_context ()
{
    return s_ctx;
}
 
cl_command_queue
cl_get_cmd_queue ()
{
    return s_cmdq;
}
 

/* ----------------------------------------------------------- *
 *   create & compile shader
 * ----------------------------------------------------------- */
cl_kernel
cl_build_kernel (const char *text, char *entry_point)
{
    cl_program prog;
    cl_kernel  kernel;
    size_t len = strlen (text);
    cl_int ret;

    prog = clCreateProgramWithSource (s_ctx, 1, &text, &len, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateProgramWithSource\n");

    ret = clBuildProgram (prog, 1, &s_dev, NULL, NULL, NULL);
    if (ret != CL_SUCCESS)
    {
        ret = clGetProgramBuildInfo (prog, s_dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        DBG_ASSERT (ret == CL_SUCCESS, "clGetProgramBuildInfo\n");

        char *buffer = calloc (len, sizeof(char));
        ret = clGetProgramBuildInfo (prog, s_dev, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        DBG_ASSERT (ret == CL_SUCCESS, "clGetProgramBuildInfo\n");

        fprintf (stderr, "Error: failed to compile kernel program.\n");
        fprintf (stderr, "----------------------------------------\n");
        fprintf (stderr, "%s\n", buffer);
        fprintf (stderr, "----------------------------------------\n");

        free (buffer);

        return 0; /* return invalid kernel */
    }

    kernel = clCreateKernel (prog, entry_point, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clBuildProgram\n");

    return kernel;
}


 