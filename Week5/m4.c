#include <stdio.h>
#include <stdlib.h>

int main() {
    // allocated but NOT initialized
    int *arr = malloc(5 * sizeof(int));

    int sum = 0;

    for (int i = 0; i < 5; i++) {
         // reading garbage values
        sum += arr[i];
    }

    printf("Sum = %d\n", sum);

    free(arr);
    return 0;
}

