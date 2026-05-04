#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

/* Child: signals immediately, no state variable set */
void *child(void *arg) {
    printf("[child] running and signalling immediately\n");

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);  /* signal fires — but potentially nobody is waiting yet */
    pthread_mutex_unlock(&mutex);

    printf("[child] signal sent, exiting\n");
    return NULL;
}

int main() {
    pthread_t child_t;

    /* Let child run and signal first */
    pthread_create(&child_t, NULL, child, NULL);
    pthread_join(child_t, NULL);

    printf("[parent] calling wait — but the signal already fired!\n");

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    printf("[parent] woke up\n");

    return 0;
}