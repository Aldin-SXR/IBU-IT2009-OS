#include <stdlib.h>

int main() {
    int x = 10;
    // freeing memory not allocated with malloc
    free(&x);

    return 0;
}