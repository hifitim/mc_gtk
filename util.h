//
// Created by tim on 12/5/15.
//

#ifndef MC_GUI_UTIL_H
#define MC_GUI_UTIL_H

#include <sys/stat.h>

/*  needed for lstat  */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

int is_directory(const char *path);
int is_executable(const char *path);



#endif //MC_GUI_UTIL_H
