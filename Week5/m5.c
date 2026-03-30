#include <stdio.h>
#include <stdlib.h>

int main() {
    // memory allocated
    int *arr = malloc(100 * sizeof(int));

    arr[0] = 42;
    printf("First element: %d\n", arr[0]);

    // forgot to call free(arr)

    return 0;
}