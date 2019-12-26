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

cl_context          cl_get_context ();
cl_command_queue    cl_get_cmd_queue ();

void cl_query_kernel_info (cl_kernel kernel);

#endif /* _UTIL_CL_H_ */
