#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>   // basename
#include <sys/mman.h> // mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int ret42(void) {
    return 42;
}

int square(int a) {
    return a * a;
}

int add2(int a, int b) {
    return a + b;
}

int add3_x(int a, int b, int c) {
    return add2(a, b) + c;
}

int add3(int a, int b, int c) {
    return a + b + c;
}

int mul2(int a, int b) {
    return a * b;
}

int mul3(int a, int b, int c) {
    return a * b * c;
}

int main(int argc, char *argv[]) {
    int r; // return value

    if (argc < 3) {
        fprintf(stderr, "Usage: %s filename offset arg1 .. argn\n", basename(argv[0]));
        fprintf(stderr, "       NOTE That the arguments are not neccessary at all.\n");
        exit(1);
    }

    const char *filename = argv[1];
    printf("filename: %s.\n", filename);

    unsigned offset = strtol(argv[2], NULL, 0);
    printf("offset: %u (0x%x)\n", offset, offset);

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open the file (%s): %s (errno: %d).\n", filename, strerror(errno), errno);
        goto open_failed;
    }

    struct stat statbuf;
    r = fstat(fd, &statbuf);
    if (r == -1) {
        fprintf(stderr, "Failed to stat %s: %s (errno: %d).\n", filename, strerror(errno), errno);
        goto fstat_failed;
    }

    printf("file (%s) size: %lu.\n", filename, statbuf.st_size);

    void *addr = mmap(0, statbuf.st_size, PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    if (addr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap: %s (errno: %d).\n", strerror(errno), errno);
        goto mmap_failed;
    }
    printf("mmaped addr: %p.\n", addr);

    // calculate the number of args
    argc -= 3; // - argv[0] - filename - offset
    argv += 3;

    int (*fun)() = addr + offset;
    const unsigned char *faddr = addr + offset;
    printf("fun addr: %p.\n", fun);
    printf("the first 32 bytes of fun:\n");
    for (int i = 0; i < 16; ++i)
        printf("%02x ", faddr[i]);
    printf("\n");
    for (int i = 0; i < 16; ++i)
        printf("%02x ", faddr[16+i]);
    printf("\n");

    switch (argc) {
        case 0:
            r = fun();
            printf("case 0: %d\n", r);
            break;
        case 1:
            r = fun(atoi(argv[0]));
            printf("case 1: %d\n", r);
            break;
        case 2:
            r = fun(atoi(argv[0]), atoi(argv[1]));
            printf("case 2: %d\n", r);
            break;
        case 3:
            r = fun(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
            printf("case 3: %d\n", r);
            break;
        default:
            printf("invalid case. argc: %d.\n", argc);
            break;
    }

    r = munmap(addr, statbuf.st_size);
    if (r == -1) {
        fprintf(stderr, "Failed to munmap(%p, %lu): %s (errno: %d).\n", addr, statbuf.st_size, strerror(errno), errno);
        goto munmap_failed;
    }

    goto exit_successfully;

munmap_failed:
fstat_failed:
mmap_failed:
    close(fd);

open_failed:
    return 1;

exit_successfully:
    return 0;
}
