//
// Created by tim on 12/5/15.
//

#include "util.h"

int
is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int
is_executable(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return (stat(path, &statbuf) == 0 && statbuf.st_mode & S_IXUSR);
}

