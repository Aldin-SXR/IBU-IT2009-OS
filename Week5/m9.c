#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));

    free(ptr);

    // freeing the same memory again
    free(ptr);

    return 0;
}