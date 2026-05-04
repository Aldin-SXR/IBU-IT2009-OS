#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static sem_t done; /* initialised to 0 */

/* ── Child ───────────────────────────────────────────────────────────────── */
void *child(void *arg)
{
    (void)arg;
    printf("[child] running\n");
    usleep(80000); /* simulate work */
    printf("[child] finished, posting semaphore\n");
    sem_post(&done); /* increment: wakes parent (or records completion) */
    return NULL;
}

int main(void)
{
    sem_init(&done, 0, 0); /* 0 = parent will block on wait() */

    pthread_t c;
    pthread_create(&c, NULL, child, NULL);

    printf("[parent] waiting for child via semaphore\n");
    sem_wait(&done); /* blocks until child calls sem_post() */
    printf("[parent] child is done\n");

    pthread_join(c, NULL);
    sem_destroy(&done);
    return 0;
}
