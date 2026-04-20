#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread1(void* arg) {
    printf("A\n");
    return NULL;
}

void* thread2(void* arg) {
    printf("B\n");
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("main: begin\n");

    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("main: end\n");

    return 0;
}