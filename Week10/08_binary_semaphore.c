#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define ITERATIONS 100000

static sem_t mutex;
static int counter = 0;

/* ── Thread function ─────────────────────────────────────────────────────── */
void *increment(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&mutex);   /* lock — decrement to 0, others sleep */
        counter++;
        sem_post(&mutex);   /* unlock — increment back to 1 */
    }
    printf("[thread %d] done\n", id);
    return NULL;
}

int main(void)
{
    sem_init(&mutex, 0, 1); /* init to 1 = "unlocked" */

    int ids[2] = {1, 2};
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, &ids[0]);
    pthread_create(&t2, NULL, increment, &ids[1]);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("counter = %d  (expected %d)\n", counter, 2 * ITERATIONS);

    sem_destroy(&mutex);
    return 0;
}
