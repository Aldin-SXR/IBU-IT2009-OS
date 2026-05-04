#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static int done = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

void *child(void *arg) {
    printf("[child] running\n");
    usleep(1000000);
    printf("[child] finished, signalling parent\n");

    /* Called by the child when it finishes */
    pthread_mutex_lock(&mutex);
    done = 1;                        /* set state BEFORE signalling */
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    printf("[parent] begin\n");

    pthread_t c;
    pthread_create(&c, NULL, child, NULL);

    /* Called by the parent to wait for the child */
    pthread_mutex_lock(&mutex);
    while (done == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("[parent] child has finished\n");
    pthread_join(c, NULL);
    return 0;
}
