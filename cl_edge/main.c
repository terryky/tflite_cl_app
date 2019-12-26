/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include "util_cl.h"
#include "util_debug.h"
#include "util_image_tga.h"

int
main (int argc, char *argv[])
{
    cl_context  ctx;
    cl_command_queue queue;
    cl_kernel   kernel;
    cl_int      ret;

    u_int img_w, img_h, img_buf_size;
    u_char *img_buf_src, *img_buf_dst;

    open_tga_from_file ("food.tga", &img_w, &img_h);
    img_buf_size = img_w * img_h * 4;

    img_buf_src = (u_char *)malloc (img_buf_size);
    img_buf_dst = (u_char *)malloc (img_buf_size);
    decode_tga_from_file ("food.tga", img_buf_src);

    ret = cl_init ();
    CLASSERT(ret);

    ctx   = cl_get_context ();
    queue = cl_get_cmd_queue ();

    kernel = cl_build_kernel_from_file (".", "detect_edge.cl", "detectLine");
    cl_query_kernel_info (kernel);


    cl_mem src_mem = clCreateBuffer(ctx, CL_MEM_READ_WRITE, img_buf_size, NULL, NULL);
    cl_mem dst_mem = clCreateBuffer(ctx, CL_MEM_READ_WRITE, img_buf_size, NULL, NULL);

    ret = clEnqueueWriteBuffer (queue, src_mem, CL_TRUE, 0, img_buf_size, img_buf_src, 0, NULL, NULL);
    CLASSERT(ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dst_mem);
    CLASSERT(ret);

    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &src_mem);
    CLASSERT(ret);

    int size[2] = {img_w, img_h};
    ret = clSetKernelArg(kernel, 2, sizeof(size), size);
    CLASSERT(ret);

    size_t global_work_size[3] = {img_w, img_h, 0};
    size_t local_work_size[3]  = {8, 8, 0};
    cl_adjust_work_size (3, global_work_size, local_work_size);
    fprintf (stderr, "WH(%d, %d)\n", img_w, img_h);
    fprintf (stderr, "global_work_size(%zu, %zu), local_work_size(%zu, %zu)\n",
            global_work_size[0], global_work_size[1], local_work_size[0], local_work_size[1]);

    ret = clEnqueueNDRangeKernel (queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    CLASSERT(ret);

    ret = clEnqueueReadBuffer (queue, dst_mem, CL_TRUE, 0, img_buf_size, img_buf_dst, 0, NULL, NULL);
    CLASSERT(ret);

    save_to_tga_file ("food_edge.tga", img_buf_dst, img_w, img_h);

    return 0;
}
