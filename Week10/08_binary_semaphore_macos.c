#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define ITERATIONS 100000
#define SEM_NAME   "/counter_mutex"

static sem_t *mutex;
static int counter = 0;

void *increment(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(mutex);
        counter++;
        sem_post(mutex);
    }
    printf("[thread %d] done\n", id);
    return NULL;
}

int main(void)
{
    /* Remove any stale semaphore from a previous run */
    sem_unlink(SEM_NAME);

    /* O_CREAT | O_EXCL: create fresh, initial value = 1 (unlocked) */
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    int ids[2] = {1, 2};
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, &ids[0]);
    pthread_create(&t2, NULL, increment, &ids[1]);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("counter = %d  (expected %d)\n", counter, 2 * ITERATIONS);

    sem_close(mutex);
    sem_unlink(SEM_NAME);   /* clean up the named semaphore */
    return 0;
}