#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

volatile int done = 0; // state variable

void *child(void *arg) {
    printf("[child] running\n");
    usleep(1000000); /* simulate some work */
    done = 1;
    printf("[child] set done = 1, exiting\n");
    return NULL;
}

int main() {
    printf("[parent] begin\n");

    pthread_t c;
    pthread_create(&c, NULL, child, NULL);

    /* PROBLEM: burns CPU in a tight loop doing nothing useful */
    while (done == 0)
        ; /* spin */

    printf("[parent] child is done\n");
    pthread_join(c, NULL);
    return 0;
}
