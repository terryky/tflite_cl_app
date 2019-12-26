/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2019 terryky1220@gmail.com
 * ------------------------------------------------ */
#ifndef _UTIL_CL_H_
#define _UTIL_CL_H_

#include <CL/cl.h>


cl_int      cl_init ();
cl_kernel   cl_build_kernel (const char *text, char *entry_point);
cl_kernel   cl_build_kernel_from_file (char *dir, char *fname, char *entry_point);

void cl_adjust_work_size (int numdim, size_t *global_work_size, size_t *local_work_size);

cl_context          cl_get_context ();
cl_command_queue    cl_get_cmd_queue ();

void cl_query_kernel_info (cl_kernel kernel);


#if 1
    void cl_assert (int err, char *fname, int nLine);
    #define CLASSERT(err) cl_assert (err, __FILE__, __LINE__)
#else
    #define CLASSERT(err) ((void)0)
#endif

#endif /* _UTIL_CL_H_ */
