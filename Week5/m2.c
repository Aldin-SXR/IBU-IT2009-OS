#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 5 chars + '\0'
    char *src = "Hello";             
    // missing space for '\0'            
    char *dest = malloc(strlen(src));

    // writes 6 bytes → overflow
    strcpy(dest, src);                           

    printf("%s\n", dest);
    free(dest);
    return 0;
}

