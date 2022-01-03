#ifndef HeapManager
#define HeapManager

int heap_init(int key, int syn);

void *malloc_1024();

void free_1024(void *addr);

int dbg_get_my_used_mem_count();

int dbg_get_free_errors();

void error_and_die();

#endif
