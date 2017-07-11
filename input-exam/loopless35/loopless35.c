#include <stdio.h>
#include <stdlib.h>

#define BOUNDARY 1000

int f(int sum, int x) {
    if (x < 0) {
        fprintf(stderr, "X should be (0..1000)\n");
        exit(1);
    }
    if (x == BOUNDARY)
        return sum;
    if (x > BOUNDARY) {
        fprintf(stderr, "Algorithm error. x > %d. x: %d.\n", BOUNDARY, x);
        exit(2);
    }

    if ((x % 3) == 0)
        sum += x;
    else if ((x % 5) == 0)
        sum += x;

    return f(sum, x+1);
}

int main(void) {
    printf("%d\n", f(0, 1));
}
