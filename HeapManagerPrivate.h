#ifndef LAB5_HEAPMANAGERPRIVATE_H
#define LAB5_HEAPMANAGERPRIVATE_H

#define SHMKEY          1337
#define SEMKEY          1338
#define SEGMENTS        10000
#define SEGMENTSIZE     1024
#define TRUE            1
#define FALSE           0

typedef struct segment_instace {
    int is_used;
    int is_freed;
    int process_id;
    char segment[SEGMENTSIZE];  // bei free kommt pointer => pointer zu array element!
} segment;

typedef struct managedheap {
    int counter_used_segments;
    int shmid;
    segment segments[SEGMENTS];
} mheap;

#endif //LAB5_HEAPMANAGERPRIVATE_H
