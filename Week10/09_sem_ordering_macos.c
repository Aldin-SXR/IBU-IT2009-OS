#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static sem_t *done; /* initialised to 0 */
#define SEM_NAME   "/counter_mutex"

/* ── Child ───────────────────────────────────────────────────────────────── */
void *child(void *arg)
{
    (void)arg;
    printf("[child] running\n");
    usleep(80000); /* simulate work */
    printf("[child] finished, posting semaphore\n");
    sem_post(done); /* increment: wakes parent (or records completion) */
    return NULL;
}

int main(void) {
    /* Remove any stale semaphore from a previous run */
    sem_unlink(SEM_NAME);

    /* O_CREAT | O_EXCL: create fresh, initial value = 0 */
    done = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (done == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    pthread_t c;
    pthread_create(&c, NULL, child, NULL);

    printf("[parent] waiting for child via semaphore\n");
    sem_wait(done); /* blocks until child calls sem_post() */
    printf("[parent] child is done\n");

    pthread_join(c, NULL);

    sem_close(done);
    sem_unlink(SEM_NAME);   /* clean up the named semaphore */
    return 0;
}
