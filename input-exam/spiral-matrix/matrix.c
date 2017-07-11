#include <stdio.h>
#include <stdlib.h>
#include <errno.h>  // errno
#include <string.h> // strerror

typedef enum {
    NONE,
    RIGHT,
    DOWN,
    LEFT,
    UP
} Direction;

const char *Direction_toString(Direction d) {
    static const char *directions[] = {
        "NONE", "RIGHT", "DOWN", "LEFT", "UP"
    };

    if (d < 0 || d > UP) {
        fprintf(stderr, "Bad direction: %d. Exiting...\n", d);
        exit(4);
    }

    return directions[d];
}

Direction nextstep(int *x, int *y, Direction d, int *n, int *N) {
    //printf("start-of nextstep Direction: %s, *n: %d, *N: %d\n", Direction_toString(d), *n, *N);

    switch (d) {
        case NONE:
            // initial case
            d = RIGHT;
            *x = *y = 0;
            *n = *N - 1;
            break;
        case RIGHT:
            if (*n > 0) {
                ++*x;
                --*n;
            }
            else {
                d = DOWN;
                --*N;
                if (*N == 0)
                    d = NONE;
                ++*y;
                *n = *N - 1;
            }
            break;
        case DOWN:
            if (*n > 0) {
                ++*y;
                --*n;
            }
            else {
                d = LEFT;
                --*x;
                *n = *N - 1;
            }
            break;
        case LEFT:
            if (*n > 0) {
                --*x;
                --*n;
            }
            else {
                d = UP;
                --*N;
                if (*N == 0)
                    d = NONE;
                --*y;
                *n = *N - 1;
            }
            break;
        case UP:
            if (*n > 0) {
                --*y;
                --*n;
            }
            else {
                d = RIGHT;
                ++*x;
                *n = *N - 1;
            }
            break;
        default:
            fprintf(stderr, "INVALID VALUE OF DIRECTION: %d\n", (int)d);
            exit(3);
            break;
    }

    //printf("end-of nextstep Direction: %s, *n: %d, *N: %d\n", Direction_toString(d), *n, *N);

    return d;
}

typedef struct {
    int N;
    int *data;
} Matrix;

int readin_matrix_size(void) {
    int n;
    scanf("%d", &n);

    if (n < 1 || n > 100) {
        fprintf(stderr, "Invalid array size: %d. Expected to be e [1..100].\n", n);
        exit(2);
    }

    return n;
}

void Matrix_create(Matrix *m, int n) {
    m->N = n;
    if (n < 1 || n > 100) {
        fprintf(stderr, "Invalid array size: %d. Expected to be e [1..100].\n", n);
        exit(2);
    }

    m->data = malloc(sizeof(int) * (m->N * m->N));
    if (m->data == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(1);
    }
}

void Matrix_destroy(Matrix *m) {
    if (m->data) {
        free(m->data);
        m->data = NULL;
    }
}

void Matrix_readin(Matrix *m) {
    int n = m->N * m->N;
    int r;
    for (int i = 0; i < n; ++i) {
        r = scanf("%d", &m->data[i]);
        if (r != 1) {
            fprintf(stderr, "scanf returned != 1 (r: %d). Error: %s (%d). Exiting...\n", r, strerror(errno), errno);
            exit(5);
        }
        //printf("readin [i: %d]: %d\n", i, m->data[i]);
    }
    //printf("\n");
}

void Matrix_print(Matrix *m) {
    for (int i = 0; i < m->N; ++i) {
        for (int j = 0; j < m->N; ++j)
            printf("(i:%d, j:%d, i*N+j: %d) %2d ", i, j, (i*(m->N))+j, m->data[(i*(m->N))+j]);
        printf("\n");
    }
}

void Matrix_print_raw(Matrix *m) {
    for (int i = 0; i < m->N*m->N; ++i) {
        printf("%d ", m->data[i]);
    }
    printf("\n");
}

void Matrix_print_spiral(Matrix *m) {
    int x, y, n, N;
    n = N = m->N;
    Direction d = NONE;
    while ((d = nextstep(&x, &y, d, &n, &N)) != NONE) {
        //printf("Direction: %s (%d)\n", Direction_toString(d), d);
        //printf("x: %d, y: %d\n", x, y);
        printf("%d ", m->data[(y*(m->N))+x]);
        //printf("(y:%d, x:%d, y*N+x: %d) %d \n", y, x, (y*(m->N))+x, m->data[(y*(m->N))+x]);
    }
    printf("\n");
}

int main(void) {
    Matrix m;

    int n = readin_matrix_size();
    //printf("matrix_size: %d\n", n);
    //printf(" ");

    Matrix_create(&m, n);
    Matrix_readin(&m);
    //printf("print raw\n");
    //Matrix_print_raw(&m);
    //printf("MATRIX print\n");
    //Matrix_print(&m);
    //printf("MATRIX print_spiral\n");
    Matrix_print_spiral(&m);
    Matrix_destroy(&m);

    return 0;
}
