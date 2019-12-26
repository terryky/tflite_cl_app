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
    CLASSERT(ret);

    ret = clGetDeviceIDs (platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &s_dev, &num_devices);
    CLASSERT(ret);

    s_ctx = clCreateContext (NULL, 1, &s_dev, NULL, NULL, &ret);
    CLASSERT(ret);

    s_cmdq = clCreateCommandQueue (s_ctx, s_dev, 0, &ret);
    CLASSERT(ret);

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
    CLASSERT(ret);

    ret = clBuildProgram (prog, 1, &s_dev, NULL, NULL, NULL);
    if (ret != CL_SUCCESS)
    {
        CLASSERT(ret);

        ret = clGetProgramBuildInfo (prog, s_dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        CLASSERT(ret);

        char *buffer = calloc (len, sizeof(char));
        ret = clGetProgramBuildInfo (prog, s_dev, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        CLASSERT(ret);

        fprintf (stderr, "Error: failed to compile kernel program.\n");
        fprintf (stderr, "----------------------------------------\n");
        fprintf (stderr, "%s\n", buffer);
        fprintf (stderr, "----------------------------------------\n");

        free (buffer);

        return 0; /* return invalid kernel */
    }

    kernel = clCreateKernel (prog, entry_point, &ret);
    CLASSERT(ret);

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
    CLASSERT(ret);
    fprintf (stderr, "CL_KERNEL_PRIVATE_MEM_SIZE                  : %zu\n", val_sz);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(val_sz3), &val_sz3, NULL);
    CLASSERT(ret);
    fprintf (stderr, "CL_KERNEL_COMPILE_WORK_GROUP_SIZE           : (%zu, %zu, %zu)\n", val_sz3[0], val_sz3[1], val_sz3[2]);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(val_ul), &val_ul, NULL);
    CLASSERT(ret);
    fprintf (stderr, "CL_KERNEL_LOCAL_MEM_SIZE                    : %lu\n", val_ul);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(val_sz), &val_sz, NULL);
    CLASSERT(ret);
    fprintf (stderr, "CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %zu\n", val_sz);

    ret = clGetKernelWorkGroupInfo (kernel, s_dev, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(val_ul), &val_ul, NULL);
    CLASSERT(ret);
    fprintf (stderr, "CL_KERNEL_PRIVATE_MEM_SIZE                  : %lu\n", val_ul);
}



/* ----------------------------------------------------------- *
 *   error handling.
 * ----------------------------------------------------------- */
#define CASE_RETURN_ERRSTRING(err) case err: return #err
const char *
cl_get_error_string (cl_int error)
{
    switch(error){
    CASE_RETURN_ERRSTRING (CL_SUCCESS);
    CASE_RETURN_ERRSTRING (CL_DEVICE_NOT_FOUND);
    CASE_RETURN_ERRSTRING (CL_DEVICE_NOT_AVAILABLE);
    CASE_RETURN_ERRSTRING (CL_COMPILER_NOT_AVAILABLE);
    CASE_RETURN_ERRSTRING (CL_MEM_OBJECT_ALLOCATION_FAILURE);
    CASE_RETURN_ERRSTRING (CL_OUT_OF_RESOURCES);
    CASE_RETURN_ERRSTRING (CL_OUT_OF_HOST_MEMORY);
    CASE_RETURN_ERRSTRING (CL_PROFILING_INFO_NOT_AVAILABLE);
    CASE_RETURN_ERRSTRING (CL_MEM_COPY_OVERLAP);
    CASE_RETURN_ERRSTRING (CL_IMAGE_FORMAT_MISMATCH);
    CASE_RETURN_ERRSTRING (CL_IMAGE_FORMAT_NOT_SUPPORTED);
    CASE_RETURN_ERRSTRING (CL_BUILD_PROGRAM_FAILURE);
    CASE_RETURN_ERRSTRING (CL_MAP_FAILURE);
    CASE_RETURN_ERRSTRING (CL_MISALIGNED_SUB_BUFFER_OFFSET);
    CASE_RETURN_ERRSTRING (CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
    CASE_RETURN_ERRSTRING (CL_COMPILE_PROGRAM_FAILURE);
    CASE_RETURN_ERRSTRING (CL_LINKER_NOT_AVAILABLE);
    CASE_RETURN_ERRSTRING (CL_LINK_PROGRAM_FAILURE);
    CASE_RETURN_ERRSTRING (CL_DEVICE_PARTITION_FAILED);
    CASE_RETURN_ERRSTRING (CL_KERNEL_ARG_INFO_NOT_AVAILABLE);

    CASE_RETURN_ERRSTRING (CL_INVALID_VALUE);
    CASE_RETURN_ERRSTRING (CL_INVALID_DEVICE_TYPE);
    CASE_RETURN_ERRSTRING (CL_INVALID_PLATFORM);
    CASE_RETURN_ERRSTRING (CL_INVALID_DEVICE);
    CASE_RETURN_ERRSTRING (CL_INVALID_CONTEXT);
    CASE_RETURN_ERRSTRING (CL_INVALID_QUEUE_PROPERTIES);
    CASE_RETURN_ERRSTRING (CL_INVALID_COMMAND_QUEUE);
    CASE_RETURN_ERRSTRING (CL_INVALID_HOST_PTR);
    CASE_RETURN_ERRSTRING (CL_INVALID_MEM_OBJECT);
    CASE_RETURN_ERRSTRING (CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
    CASE_RETURN_ERRSTRING (CL_INVALID_IMAGE_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_SAMPLER);
    CASE_RETURN_ERRSTRING (CL_INVALID_BINARY);
    CASE_RETURN_ERRSTRING (CL_INVALID_BUILD_OPTIONS);
    CASE_RETURN_ERRSTRING (CL_INVALID_PROGRAM);
    CASE_RETURN_ERRSTRING (CL_INVALID_PROGRAM_EXECUTABLE);
    CASE_RETURN_ERRSTRING (CL_INVALID_KERNEL_NAME);
    CASE_RETURN_ERRSTRING (CL_INVALID_KERNEL_DEFINITION);
    CASE_RETURN_ERRSTRING (CL_INVALID_KERNEL);
    CASE_RETURN_ERRSTRING (CL_INVALID_ARG_INDEX);
    CASE_RETURN_ERRSTRING (CL_INVALID_ARG_VALUE);
    CASE_RETURN_ERRSTRING (CL_INVALID_ARG_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_KERNEL_ARGS);
    CASE_RETURN_ERRSTRING (CL_INVALID_WORK_DIMENSION);

    CASE_RETURN_ERRSTRING (CL_INVALID_WORK_GROUP_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_WORK_ITEM_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_GLOBAL_OFFSET);
    CASE_RETURN_ERRSTRING (CL_INVALID_EVENT_WAIT_LIST);
    CASE_RETURN_ERRSTRING (CL_INVALID_EVENT);
    CASE_RETURN_ERRSTRING (CL_INVALID_OPERATION);
    CASE_RETURN_ERRSTRING (CL_INVALID_GL_OBJECT);
    CASE_RETURN_ERRSTRING (CL_INVALID_BUFFER_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_MIP_LEVEL);
    CASE_RETURN_ERRSTRING (CL_INVALID_GLOBAL_WORK_SIZE);
    CASE_RETURN_ERRSTRING (CL_INVALID_PROPERTY);
    CASE_RETURN_ERRSTRING (CL_INVALID_IMAGE_DESCRIPTOR);
    CASE_RETURN_ERRSTRING (CL_INVALID_COMPILER_OPTIONS);
    CASE_RETURN_ERRSTRING (CL_INVALID_LINKER_OPTIONS);
    CASE_RETURN_ERRSTRING (CL_INVALID_DEVICE_PARTITION_COUNT);

    // extension errors
#if defined (CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR)
    CASE_RETURN_ERRSTRING (CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR);
#endif

#if defined (CL_PLATFORM_NOT_FOUND_KHR)
    CASE_RETURN_ERRSTRING (CL_PLATFORM_NOT_FOUND_KHR);
#endif

#if defined (CL_INVALID_D3D10_DEVICE_KHR)
    CASE_RETURN_ERRSTRING (CL_INVALID_D3D10_DEVICE_KHR);
#endif

#if defined (CL_INVALID_D3D10_RESOURCE_KHR)
    CASE_RETURN_ERRSTRING (CL_INVALID_D3D10_RESOURCE_KHR);
#endif

#if defined (CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR)
    CASE_RETURN_ERRSTRING (CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR);
#endif

#if defined (CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR)
    CASE_RETURN_ERRSTRING (CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR);
#endif

    default: return "Unknown OpenCL error";
    }
}


void
cl_assert (int err, char *fname, int nline)
{
    if (err != CL_SUCCESS)
    {
        fprintf (stderr, "[CL ASSERT ERR] \"%s\"(%d):0x%04x(%s)\n",
                    fname, nline, err, cl_get_error_string (err));
    }
}

