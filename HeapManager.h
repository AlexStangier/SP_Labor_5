#ifndef HeapManager
#define HeapManager

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

int heap_init(int key, int syn);

void *malloc_1024();

void free_1024(void *addr);

int dbg_get_my_used_mem_count();

int dbg_get_free_errors();

void error_and_die();

#endif
