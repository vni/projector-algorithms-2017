#include <stdio.h>
#include <stdlib.h>

/*
int add(int a, int b)
{
    return a + b;
}
*/

const unsigned char add[] = { 0x8d, 0x04, 0x37, 0xc3 };


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("WRONG USAGE!\n");
        return 1;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    printf("%d + %d = %d\n", a, b,  ((int (*)(int, int))add)(a, b));

    return 0;
}
