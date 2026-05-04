#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N      5
#define ROUNDS 3 /* each philosopher eats this many times */

static sem_t fork_sem[N];

/* ── Philosopher ─────────────────────────────────────────────────────────── */
void *philosopher(void *arg)
{
    int id    = *(int *)arg;
    int left  = id;
    int right = (id + 1) % N;

    for (int round = 0; round < ROUNDS; round++) {
        /* Think */
        printf("[phil %d] thinking (round %d)\n", id, round + 1);
        usleep((id + 1) * 5000); /* stagger think times slightly */

        /* Pick up forks — reversed order for the last philosopher */
        if (id == N - 1) {
            sem_wait(&fork_sem[right]); /* right first — breaks the cycle */
            sem_wait(&fork_sem[left]);
        } else {
            sem_wait(&fork_sem[left]);
            sem_wait(&fork_sem[right]);
        }

        /* Eat */
        printf("[phil %d] eating   (round %d)\n", id, round + 1);
        usleep(10000);

        /* Put down forks */
        sem_post(&fork_sem[left]);
        sem_post(&fork_sem[right]);
    }

    printf("[phil %d] finished all rounds\n", id);
    return NULL;
}

int main(void)
{
    for (int i = 0; i < N; i++)
        sem_init(&fork_sem[i], 0, 1);

    int ids[N];
    pthread_t ph[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&ph[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) pthread_join(ph[i], NULL);

    for (int i = 0; i < N; i++) sem_destroy(&fork_sem[i]);

    printf("\nAll philosophers ate %d rounds — no deadlock.\n", ROUNDS);
    return 0;
}
