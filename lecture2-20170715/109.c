// 10^9
// how much time does 10^9 simple operations consume?

#include <stdio.h>

int main(void){ 
    unsigned long sum = 0;

    /*
    */
    for (int i = 0; i < 1000000000; ++i)
        sum += i;

    /*
    for (int i = 0; i < 100000; ++i)
        for (int j = 0; j < 10000; ++j)
            sum += i * 10000 + j;
    */

    printf("sum = %lu\n", sum);
    return 0;
}
