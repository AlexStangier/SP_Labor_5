#include <sys/stat.h>
#include "heap1024.h"
#include "heap1024private.h"

static int instanciated = FALSE;
static int synchronized = FALSE;
static int semaphore;

struct managedheap *heap;

void unlock() {
    if (semop(semaphore, &sembuf_inc, 1) == -1)
    {
        perror("signal failed\n");
        exit(1);
    }
}

void lock() {
    if (semop(semaphore, &sembuf_dec, 1) == -1)
    {
        perror("wait failed\n");
        exit(1);
    }
}

void exit_and_die() {
    shmdt(heap);
//     system("ipcrm -a");
}

int heap_init(key_t key, int syn) {
    atexit(exit_and_die);
    int shmid = 0;

    //use semaphore
    if (syn == TRUE) {
        synchronized = TRUE;

        if ((semaphore = semget(SEMKEY, 1, IPC_CREAT | 0666)) == -1) {
            perror("Failed to get semaphore:\n");
            return EXIT_FAILURE;
        }

        //init semaphore with 0
        struct sembuf sem_operation[1]={0};
        sem_operation[0].sem_op=1;
        sem_operation[0].sem_num = 0;
        int rc;
        if ((rc = semop(semaphore, sem_operation, 1)) == -1) {
            perror("Failed to control semaphore:\n");
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
    int i;
    for (i = 0; i < SEGMENTS; i++) {
        if (heap->segments[i].is_used == FALSE) {
            if(synchronized == TRUE) lock();

            heap->segments[i].is_used = TRUE;
            heap->segments[i].is_freed = FALSE;
            heap->segments[i].ptr = (void *) heap->segments[i].segment;
            heap->segments[i].pid = getpid();

            heap->counter_used_segments++;

            if(synchronized == TRUE) unlock();
            return heap->segments[i].segment;
        }
    }
    return (void *) NULL;
}

void free_1024(void *addr) {
    int i;
    for (i = 0; i < SEGMENTS; i++) {
        if (heap->segments[i].ptr == addr) {
            if(synchronized == TRUE) lock();

            heap->counter_used_segments--;
            if (heap->segments[i].is_used == FALSE) heap->counter_free_errors++;
            heap->segments[i].is_freed = TRUE;
            heap->segments[i].is_used = FALSE;

            if(synchronized == TRUE) unlock();
        }
    }
}

int dbg_get_my_used_mem_count() {
    int count = 0, pid = getpid(), i = 0;
    for (i = 0; i < SEGMENTSIZE; i++) {
        if (heap->segments[i].is_used == TRUE && heap->segments[i].is_freed == FALSE &&
            heap->segments[i].pid == pid) {
            count++;
        }
    }
    return count;
}

int dbg_get_free_errors() {
    return heap->counter_free_errors;;
}
