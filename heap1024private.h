#ifndef LAB5_HEAP1024PRIVATE_H
#define LAB5_HEAP1024PRIVATE_H

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define SHMKEY          1337
#define SEMKEY          1338
#define SEGMENTS        10000
#define SEGMENTSIZE     1024
#define TRUE            1
#define FALSE           0

typedef struct segment_instace {
    int is_used;
    int is_freed;
    void *ptr;
    int pid;
    char segment[SEGMENTSIZE];  // bei free kommt pointer => pointer zu array element!
} segment;

typedef struct managedheap {
    int counter_used_segments;
    int counter_free_errors;
    int shmid;
    segment segments[SEGMENTS];
} mheap;

struct sembuf sembuf_inc = {
        .sem_num = 0,
        .sem_op = +1,
        .sem_flg = 0
};

struct sembuf sembuf_dec = {
        .sem_num = 0,
        .sem_op = -1,
        .sem_flg = 0
};

union semun {
    int val;                /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
    ushort *array;          /* array for GETALL & SETALL */
    struct seminfo *__buf;  /* buffer for IPC_INFO */
    void *__pad;
};

void signal();

void wait();

#endif //LAB5_HEAP1024PRIVATE_H
