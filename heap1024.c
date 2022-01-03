#include <sys/stat.h>
#include "heap1024.h"
#include "heap1024private.h"

static int instanciated = FALSE;
int semaphore;
struct managedheap *heap;
extern int errno;

void error_and_die() {
    shmdt(heap);
    system("ipcrm -M 1337");
}

int heap_init(key_t key, int syn) {
    atexit(error_and_die);
    int shmid;

    //use semaphore
    if (syn == TRUE) {
        //kill existing seamphore
        system("ipcrm -S 1338");

        if ((semaphore = semget(SEMKEY, 1, IPC_CREAT | 0777)) == -1) {
            perror("Failed to get semaphore:\n");
            printf("%d\n", errno);
            return EXIT_FAILURE;
        }
    }

    if (instanciated == FALSE) {
        //allocate new shared memory
        shmid = shmget(key, sizeof(mheap), IPC_CREAT | 0666);
        if (shmid == -1) {
            perror("Failed to allocate shm: \n");
            return EXIT_FAILURE;
        }
        instanciated = TRUE;

        //attach to shm
        if ((heap = shmat(shmid, NULL, 0)) == (void *) -1) {
            perror("Failed to attach shm: \n");
            return EXIT_FAILURE;
        }

        //initialize managment variables
        heap->shmid = shmid;
        heap->counter_used_segments = 0;
    } else {
        //attach shared memory to pointer
        if ((heap = shmat(shmid, NULL, 0)) == (void *) -1) {
            perror("Failed to attach to shm: \n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

void *malloc_1024() {
    //find unused segment in heap
    for (int i = 0; i < SEGMENTS; i++) {
        if (heap->segments[i].is_used == FALSE) {
            //perist process_id
            heap->segments[i].process_id = getpid();
            heap->segments[i].is_used = TRUE;
            heap->segments[i].ptr = (void *) heap->segments[i].segment;

            heap->counter_used_segments++;
            return heap->segments[i].ptr;
        }
    }
    return (void *) NULL;
}

void free_1024(void *addr) {
    for (int i = 0; i < SEGMENTS; i++) {
        if (heap->segments[i].ptr == addr) {
            heap->counter_used_segments--;
            if (heap->segments[i].is_used == FALSE) heap->counter_free_errors++;
            heap->segments[i].is_freed = TRUE;
        }
    }

}

int dbg_get_my_used_mem_count() {
    int count = 0;
    int pid = getpid();
    for (int i = 0; i < heap->counter_used_segments; i++) {
        if (heap->segments[i].is_used == TRUE && heap->segments[i].is_freed == FALSE &&
            heap->segments[i].process_id == pid) {
            count++;
        }
    }
    return count;
}

int dbg_get_free_errors() {
    return heap->counter_free_errors;;
}
