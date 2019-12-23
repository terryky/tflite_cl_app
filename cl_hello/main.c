/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include "util_cl.h"
#include "util_debug.h"


static char s_str_kernel[] ="              \n\
__kernel void hello(__global char* string) \n\
{                                          \n\
   string[0] = 'H';                        \n\
   string[1] = 'e';                        \n\
   string[2] = 'l';                        \n\
   string[3] = 'l';                        \n\
   string[4] = 'o';                        \n\
   string[5] = ',';                        \n\
   string[6] = ' ';                        \n\
   string[7] = 'W';                        \n\
   string[8] = 'o';                        \n\
   string[9] = 'r';                        \n\
   string[10] = 'l';                       \n\
   string[11] = 'd';                       \n\
   string[12] = '!';                       \n\
   string[13] = '\\0';                     \n\
}                                           ";

int
main (int argc, char *argv[])
{
    cl_context  ctx;
    cl_command_queue queue;
    cl_kernel   kernel;
    cl_mem      memobj;
    cl_int      ret;

    ret = cl_init ();
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateBuffer\n");

    ctx   = cl_get_context ();
    queue = cl_get_cmd_queue ();

    kernel = cl_build_kernel (s_str_kernel, "hello");
    cl_query_kernel_info (kernel);

    memobj = clCreateBuffer (ctx, CL_MEM_READ_WRITE, 128, NULL, &ret);
    DBG_ASSERT (ret == CL_SUCCESS, "clCreateBuffer\n");

    ret = clSetKernelArg (kernel, 0, sizeof(cl_mem), (void *)&memobj);
    DBG_ASSERT (ret == CL_SUCCESS, "clSetKernelArg\n");

    ret = clEnqueueTask (queue, kernel, 0, NULL, NULL);
    DBG_ASSERT (ret == CL_SUCCESS, "clEnqueueTask");

    char strbuf[128];
    ret = clEnqueueReadBuffer (queue, memobj, CL_TRUE, 0, 128, strbuf, 0, NULL, NULL);

    puts (strbuf);

    return 0;
}
