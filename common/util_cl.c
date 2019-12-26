/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

cl_kernel
cl_build_kernel_from_file (char *dir_name, char *src_fname, char *entry_point)
{
    cl_kernel kernel;
    int  fd;
    void *buf;
    struct stat st;
    char src_path[128];

    snprintf (src_path, sizeof(src_path), "%s/%s", dir_name, src_fname);

    fd = open (src_path, O_RDONLY | O_CLOEXEC);
    DBG_ASSERT (fd >= 0, "failed to open %s\n", src_path);

    fstat (fd, &st);
    buf = mmap (0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close (fd);
    if (buf == MAP_FAILED)
        return 0;

    kernel = cl_build_kernel (buf, entry_point);
    munmap (buf, st.st_size);

    return kernel;
}

/* ----------------------------------------------------------- *
 *   adjust global work size.
 * ----------------------------------------------------------- */
void
cl_adjust_work_size (int numdim, size_t *global_work_size, size_t *local_work_size)
{
    for (int i = 0; i < numdim; i ++)
    {
        int gsize = global_work_size[i];
        int lsize = local_work_size[i];
        if (lsize == 0) lsize = 1;

        int num = (gsize + lsize - 1) / lsize;  /* ceil */
        global_work_size[i] = num * lsize;
        local_work_size [i] = lsize;
    }
}


void
cl_query_kernel_info (cl_kernel kernel)
{
    size_t   val_sz;
    size_t   val_sz3[3];
    cl_ulong val_ul;
    cl_int   ret;

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(val_sz), &val_sz, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetKernelWorkGroupInfo\n");
    fprintf (stderr, "CL_KERNEL_PRIVATE_MEM_SIZE                  : %zu\n", val_sz);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(val_sz3), &val_sz3, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetKernelWorkGroupInfo\n");
    fprintf (stderr, "CL_KERNEL_COMPILE_WORK_GROUP_SIZE           : (%zu, %zu, %zu)\n", val_sz3[0], val_sz3[1], val_sz3[2]);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(val_ul), &val_ul, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetKernelWorkGroupInfo\n");
    fprintf (stderr, "CL_KERNEL_LOCAL_MEM_SIZE                    : %lu\n", val_ul);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(val_sz), &val_sz, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetKernelWorkGroupInfo\n");
    fprintf (stderr, "CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %zu\n", val_sz);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(val_ul), &val_ul, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clGetKernelWorkGroupInfo\n");
    fprintf (stderr, "CL_KERNEL_PRIVATE_MEM_SIZE                  : %lu\n", val_ul);
}
 