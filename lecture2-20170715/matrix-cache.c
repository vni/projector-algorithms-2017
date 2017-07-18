#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define N 70000
//#define N 5000
#define N 5000
int (*matrix)[N][N];

int main(void) {
    srand(time(NULL));

    void *mem = malloc(sizeof(int) * N * N);
    matrix = mem;
    if (!mem) {
        printf("MEM***\n");
        exit(1);
    }

    struct timespec tstart, tend, duration;

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            (*matrix)[i][j] = i*N+j;

    printf("FIRST ROW BASED\n");
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    int rsum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            rsum += (*matrix)[i][j];
    clock_gettime(CLOCK_MONOTONIC, &tend);
    duration.tv_sec = tend.tv_sec - tstart.tv_sec;
    duration.tv_nsec = tend.tv_nsec - tstart.tv_nsec;
    if (duration.tv_nsec < 0) {
        duration.tv_nsec += 1000000000;
        duration.tv_sec--;
    }
    printf("rsum: %d. duration: %ld.%09ld\n", rsum, duration.tv_sec, duration.tv_nsec);

    printf("SECOND COLUMN BASED\n");
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    int csum = 0;
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
            csum += (*matrix)[i][j];
    clock_gettime(CLOCK_MONOTONIC, &tend);
    duration.tv_sec = tend.tv_sec - tstart.tv_sec;
    duration.tv_nsec = tend.tv_nsec - tstart.tv_nsec;
    if (duration.tv_nsec < 0) {
        duration.tv_nsec += 1000000000;
        duration.tv_sec--;
    }
    printf("csum: %d. duration: %ld.%09ld\n", csum, duration.tv_sec, duration.tv_nsec);

    return 0;
}
