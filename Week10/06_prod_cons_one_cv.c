#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_MAX  1
#define PRODUCER_LOOPS    6
#define CONSUMER_LOOPS    3

static int buffer    = 0; /* number of items currently in buffer */
static int value     = 0; /* the item */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER; /* single CV — the bug */

/* ── Buffer operations ───────────────────────────────────────────────────── */
static void put(int v) { value = v; buffer++; }
static int  get()  { buffer--; return value; }

/* ── Producer ────────────────────────────────────────────────────────────── */
void *producer(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < PRODUCER_LOOPS; i++) {
        pthread_mutex_lock(&mutex);
        while (buffer == BUF_MAX) {
            printf("[producer %d] buffer full, waiting\n", id);
            pthread_cond_wait(&cond, &mutex);
        }
        put(i);
        printf("[producer %d] put %d\n", id, i);
        pthread_cond_signal(&cond); /* BUG: might wake the other producer */
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/* ── Consumer ────────────────────────────────────────────────────────────── */
void *consumer(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < CONSUMER_LOOPS; i++) {
        pthread_mutex_lock(&mutex);
        while (buffer == 0) {
            printf("[consumer %d] buffer empty, waiting\n", id);
            pthread_cond_wait(&cond, &mutex);
        }
        int v = get();
        printf("[consumer %d] got %d\n", id, v);
        pthread_cond_signal(&cond); /* BUG: might wake the other consumer */
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    int pids[1] = {1};
    int cids[2] = {1, 2};
    pthread_t p1, c1, c2;

    pthread_create(&p1, NULL, producer, &pids[0]);
    pthread_create(&c1, NULL, consumer, &cids[0]);
    pthread_create(&c2, NULL, consumer, &cids[1]);

    pthread_join(p1, NULL); 
    pthread_join(c1, NULL); 
    pthread_join(c2, NULL);

    printf("Result: completed without deadlock (lucky scheduling this run)\n");

    return 0;
}