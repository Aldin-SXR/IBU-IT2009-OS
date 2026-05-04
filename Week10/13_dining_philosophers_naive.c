#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

static sem_t fork_sem[N];
static volatile int rescued = 0;

/* ── Philosopher: always grabs left fork first ───────────────────────────── */
void *philosopher(void *arg)
{
    int id    = *(int *)arg;
    int left  = id;
    int right = (id + 1) % N;

    printf("[phil %d] thinking\n", id);
    usleep(10000); /* think a little so all reach fork pickup together */

    printf("[phil %d] picking up LEFT fork %d\n", id, left);
    sem_wait(&fork_sem[left]);

    if (rescued) {
        sem_post(&fork_sem[left]);
        return NULL;
    }

    printf("[phil %d] picking up RIGHT fork %d — may deadlock here\n", id, right);
    sem_wait(&fork_sem[right]); /* all five block here → deadlock */

    /* If we somehow get here (lucky scheduling) */
    printf("[phil %d] eating!\n", id);
    usleep(5000);

    sem_post(&fork_sem[right]);
    sem_post(&fork_sem[left]);
    printf("[phil %d] done eating\n", id);
    return NULL;
}

/* ── Watcher ─────────────────────────────────────────────────────────────── */
void *watcher(void *arg)
{
    (void)arg;
    usleep(500000);
    rescued = 1;
    printf("[watcher] timeout — deadlock confirmed! releasing all forks.\n");
    for (int i = 0; i < N; i++)
        sem_post(&fork_sem[i]);
    return NULL;
}

int main(void)
{
    for (int i = 0; i < N; i++)
        sem_init(&fork_sem[i], 0, 1);

    int ids[N];
    pthread_t ph[N], w;

    pthread_create(&w, NULL, watcher, NULL);
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&ph[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) pthread_join(ph[i], NULL);
    pthread_join(w, NULL);

    for (int i = 0; i < N; i++) sem_destroy(&fork_sem[i]);

    printf("\nFix: break the circular wait — see 14_dining_philosophers_dijkstra.c\n");
    return 0;
}
