#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    // memory freed
    free(ptr);

    // using dangling pointer
    printf("%d\n", *ptr);

    return 0;
}