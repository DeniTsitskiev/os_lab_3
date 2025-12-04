#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <sys/types.h>

#define SHM_SIZE 1024

typedef struct {
    char buffer[SHM_SIZE];
    size_t size;
    int exit_flag;
} shared_data;

#endif