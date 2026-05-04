#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_MAX 4
#define LOOPS   8

static int buf[BUF_MAX];
static int fill_pos = 0;
static int use_pos  = 0;

static sem_t empty_slots; /* free slots available */
static sem_t full_slots;  /* filled slots available */
static sem_t mutex;       /* protects buf, fill_pos, use_pos */

/* ── Buffer operations (called with mutex held) ──────────────────────────── */
static void put(int v)
{
    buf[fill_pos] = v;
    fill_pos = (fill_pos + 1) % BUF_MAX;
}

static int get(void)
{
    int v = buf[use_pos];
    use_pos = (use_pos + 1) % BUF_MAX;
    return v;
}

/* ── Producer ────────────────────────────────────────────────────────────── */
void *producer(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < LOOPS; i++) {
        sem_wait(&empty_slots); /* wait for a free slot — BEFORE mutex */
        sem_wait(&mutex);       /* acquire exclusive access to buf */

        put(i);
        printf("[producer %d] put %d\n", id, i);

        sem_post(&mutex);       /* release buf access */
        sem_post(&full_slots);  /* announce one more filled slot */
    }
    return NULL;
}

/* ── Consumer ────────────────────────────────────────────────────────────── */
void *consumer(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < LOOPS; i++) {
        sem_wait(&full_slots);  /* wait for a filled slot — BEFORE mutex */
        sem_wait(&mutex);       /* acquire exclusive access to buf */

        int v = get();
        printf("[consumer %d] got %d\n", id, v);

        sem_post(&mutex);       /* release buf access */
        sem_post(&empty_slots); /* announce one more free slot */
    }
    return NULL;
}

int main(void)
{
    sem_init(&empty_slots, 0, BUF_MAX); /* all slots free initially */
    sem_init(&full_slots,  0, 0);       /* no filled slots initially */
    sem_init(&mutex,       0, 1);       /* unlocked */

    int pids[2] = {1, 2};
    int cids[2] = {1, 2};
    pthread_t p1, p2, c1, c2;

    pthread_create(&p1, NULL, producer, &pids[0]);
    pthread_create(&p2, NULL, producer, &pids[1]);
    pthread_create(&c1, NULL, consumer, &cids[0]);
    pthread_create(&c2, NULL, consumer, &cids[1]);

    pthread_join(p1, NULL); pthread_join(p2, NULL);
    pthread_join(c1, NULL); pthread_join(c2, NULL);

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);

    printf("All done.\n");
    return 0;
}
