#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CL/cl.h>

#define PRINT_PLATFORM_INFO_STR(pf_id, param_name)              \
    do                                                          \
    {                                                           \
        char strbuf[1024];                                      \
        clGetPlatformInfo (pf_id, param_name, sizeof(strbuf), strbuf, NULL); \
        fprintf (stderr, " %-24s: %s\n", #param_name, strbuf);  \
    } while (0)

#define PRINT_DEVINFO_STR(dev_id, param_name)                  \
    do                                                         \
    {                                                          \
        char strbuf[1024];                                     \
        clGetDeviceInfo(dev_id, param_name, sizeof (strbuf), strbuf, NULL); \
        fprintf (stderr, "  %-40s: %s\n", #param_name, strbuf);\
    } while (0)

#define PRINT_DEVINFO_UINT(dev_id, param_name)                 \
    do                                                         \
    {                                                          \
        cl_uint value;                                         \
        clGetDeviceInfo(dev_id, param_name, sizeof (value), &value, NULL); \
        fprintf (stderr, "  %-40s: %u\n", #param_name, value); \
    } while (0)

#define PRINT_DEVINFO_SIZE(dev_id, param_name)                 \
    do                                                         \
    {                                                          \
        size_t value;                                          \
        clGetDeviceInfo(dev_id, param_name, sizeof (value), &value, NULL); \
        fprintf (stderr, "  %-40s: %zu\n", #param_name, value);\
    } while (0)

#define PRINT_DEVINFO_BOOL(dev_id, param_name)                 \
    do                                                         \
    {                                                          \
        cl_bool value;                                         \
        clGetDeviceInfo(dev_id, param_name, sizeof (value), &value, NULL); \
        if (value)                                             \
            fprintf (stderr, "  %-40s: true\n", #param_name);  \
        else                                                   \
            fprintf (stderr, "  %-40s: false\n", #param_name); \
    } while (0)

#define CLASSERT_AND_RET(retval) \
    do \
    {\
        if (retval != CL_SUCCESS)  \
        {           \
            fprintf (stderr, "ERR: %s(%d):%d\n", __FILE__, __LINE__, retval); \
            return -1;\
        }           \
    } while (0)

void
query_cldev (cl_device_id device) 
{
    PRINT_DEVINFO_STR(device, CL_DEVICE_NAME);
    PRINT_DEVINFO_STR(device, CL_DEVICE_VENDOR);
    PRINT_DEVINFO_STR(device, CL_DRIVER_VERSION);
    PRINT_DEVINFO_STR(device, CL_DEVICE_OPENCL_C_VERSION);
    PRINT_DEVINFO_STR(device, CL_DEVICE_PROFILE);

    PRINT_DEVINFO_BOOL(device, CL_DEVICE_AVAILABLE);
    PRINT_DEVINFO_BOOL(device, CL_DEVICE_COMPILER_AVAILABLE);

    cl_device_type type;
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    if (type & CL_DEVICE_TYPE_CPU)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_CPU");
    if (type & CL_DEVICE_TYPE_GPU)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_GPU");
    if (type & CL_DEVICE_TYPE_ACCELERATOR)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_ACCELERATOR");
    if (type & CL_DEVICE_TYPE_DEFAULT)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_TYPE", "CL_DEVICE_TYPE_DEFAULT");

    PRINT_DEVINFO_STR(device, CL_DEVICE_EXTENSIONS);

    PRINT_DEVINFO_UINT(device, CL_DEVICE_VENDOR_ID);
    PRINT_DEVINFO_UINT(device, CL_DEVICE_MAX_COMPUTE_UNITS);
    PRINT_DEVINFO_UINT(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);

    size_t workitem_size[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    fprintf (stderr, "  %-40s: %zu / %zu / %zu \n", "CL_DEVICE_MAX_WORK_ITEM_SIZES", 
        workitem_size[0], workitem_size[1], workitem_size[2]);

    PRINT_DEVINFO_SIZE(device, CL_DEVICE_MAX_WORK_GROUP_SIZE);

    cl_uint clock_frequency;
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
    fprintf (stderr, "  %-40s: %u MHz\n", "CL_DEVICE_MAX_CLOCK_FREQUENCY", clock_frequency);

    PRINT_DEVINFO_UINT(device, CL_DEVICE_ADDRESS_BITS);
    
    cl_ulong max_mem_alloc_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
    fprintf (stderr, "  %-40s: %u MByte\n", "CL_DEVICE_MAX_MEM_ALLOC_SIZE", (unsigned int)(max_mem_alloc_size / (1024 * 1024)));

    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    fprintf (stderr, "  %-40s: %u MByte\n", "CL_DEVICE_GLOBAL_MEM_SIZE", (unsigned int)(mem_size / (1024 * 1024)));

    PRINT_DEVINFO_BOOL(device, CL_DEVICE_ENDIAN_LITTLE);
    PRINT_DEVINFO_BOOL(device, CL_DEVICE_HOST_UNIFIED_MEMORY);
    PRINT_DEVINFO_BOOL(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT);

    cl_device_local_mem_type mem_type;
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(mem_type), &mem_type, NULL);
    fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_LOCAL_MEM_TYPE", mem_type == 1 ? "local" : "global");

    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    fprintf (stderr, "  %-40s: %u KByte\n", "CL_DEVICE_LOCAL_MEM_SIZE", (unsigned int)(mem_size / 1024));

    clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
    fprintf (stderr, "  %-40s: %u KByte\n", "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", (unsigned int)(mem_size / 1024));

    cl_command_queue_properties queue_properties;
    clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
    if (queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_QUEUE_PROPERTIES", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
    if (queue_properties & CL_QUEUE_PROFILING_ENABLE)
        fprintf (stderr, "  %-40s: %s\n", "CL_DEVICE_QUEUE_PROPERTIES", "CL_QUEUE_PROFILING_ENABLE");

    PRINT_DEVINFO_BOOL(device, CL_DEVICE_IMAGE_SUPPORT);

    PRINT_DEVINFO_UINT(device, CL_DEVICE_MAX_READ_IMAGE_ARGS );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);

    PRINT_DEVINFO_SIZE(device, CL_DEVICE_IMAGE2D_MAX_WIDTH );
    PRINT_DEVINFO_SIZE(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT);
    PRINT_DEVINFO_SIZE(device, CL_DEVICE_IMAGE3D_MAX_WIDTH );
    PRINT_DEVINFO_SIZE(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT);
    PRINT_DEVINFO_SIZE(device, CL_DEVICE_IMAGE3D_MAX_DEPTH );
    
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR  );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT   );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG  );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
    PRINT_DEVINFO_UINT(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF  );

    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR  );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT   );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG  );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT );
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
    PRINT_DEVINFO_UINT(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF  );
}




int
main (int argc, const char** argv) 
{
    cl_platform_id* pf_ids;
    cl_uint i, j, num_platforms;
    cl_int ret;


    ret = clGetPlatformIDs (0, NULL, &num_platforms);
    CLASSERT_AND_RET(ret);

    if (num_platforms == 0) 
    {
        fprintf (stderr, "No OpenCL platform found.\n");
        return -1;
    }

    pf_ids = (cl_platform_id*)malloc (num_platforms * sizeof (cl_platform_id));
    if (pf_ids == NULL) 
    {
        fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    ret = clGetPlatformIDs (num_platforms, pf_ids, NULL);
    CLASSERT_AND_RET(ret);

    for (i = 0; i < num_platforms; i++)
    {
        cl_platform_id pf_id = pf_ids[i];
        cl_uint      num_devices;
        cl_device_id *devices;

        fprintf (stderr, "\n");
        fprintf (stderr, "--------------------------------------------------------\n");
        fprintf (stderr, " OpenCL Platform [%d/%d]\n", i, num_platforms);
        fprintf (stderr, "--------------------------------------------------------\n");

        PRINT_PLATFORM_INFO_STR(pf_id, CL_PLATFORM_NAME);
        PRINT_PLATFORM_INFO_STR(pf_id, CL_PLATFORM_VENDOR);
        PRINT_PLATFORM_INFO_STR(pf_id, CL_PLATFORM_VERSION);
        PRINT_PLATFORM_INFO_STR(pf_id, CL_PLATFORM_PROFILE);
        PRINT_PLATFORM_INFO_STR(pf_id, CL_PLATFORM_EXTENSIONS);

        ret = clGetDeviceIDs (pf_id, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        CLASSERT_AND_RET(ret);

        if (num_devices == 0) 
        {
            fprintf (stderr, " No devices found \n");
            continue;
        }

        devices = (cl_device_id *)malloc (sizeof(cl_device_id) * num_devices);
        if (devices == NULL)
        {
            fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
            return -1;
        }

        ret = clGetDeviceIDs (pf_id, CL_DEVICE_TYPE_ALL, num_devices, devices, &num_devices);
        CLASSERT_AND_RET(ret);

        for (j = 0; j < num_devices; j++)
        {
            fprintf (stderr, "\n");
            fprintf (stderr, " ----------------------------------\n");
            fprintf (stderr, "  CL Device [%d/%d]\n", j, num_devices);
            fprintf (stderr, " ----------------------------------\n");

            query_cldev (devices[j]);
        }
    }
}
