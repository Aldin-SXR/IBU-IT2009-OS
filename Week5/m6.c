#include <stdio.h>

int main() {
    int x;
    // uninitialized read
    printf("%d\n", x);
    return 0;
}