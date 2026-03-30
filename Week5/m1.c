#include <stdio.h>
#include <string.h>

int main() {
    // Pointer declared, but no memory allocated
    char *dest;         
    char *src = "Hello, this is a long string.";

    // Writing to unallocated memory
    strcpy(dest, src);

    printf("%s\n", dest);
    return 0;
}

