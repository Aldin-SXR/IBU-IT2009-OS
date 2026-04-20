#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int counter = 0;
const int loops = 1000000;

void* worker(void *arg) {
    for (int i = 0; i < loops; i++) {
        counter++;
    }
    return NULL;
 }

int main() {
    pthread_t p1, p2;
    printf("Initial value : %d\n", counter);

    pthread_create(&p1, NULL, worker, NULL);
    pthread_create(&p2, NULL, worker, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    
    printf("Final value : %d\n", counter);

    return 0;
}