#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static int done = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

void *child(void *arg) {
    printf("[child] working...\n");
    usleep(1000000);
    printf("[child] calling correct_thr_exit\n");

    done = 1;
    pthread_cond_signal(&cond); /* no lock held — race with broken_thr_join */

    return NULL;
}

int main() {
    printf("[parent] starting child, then waiting without mutex\n");

    pthread_t c;
    pthread_create(&c, NULL, child, NULL);

    if (done == 0)              /* check without lock */
        pthread_cond_wait(&cond, &mutex); /* wait without having locked first */
    /* If signal fires between the 'if' and the 'wait', we sleep forever */

    pthread_join(c, NULL);

    printf("[parent] woke up\n");

    return 0;
}
