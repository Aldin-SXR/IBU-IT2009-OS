#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/* ── Shared state ────────────────────────────────────────────────────────── */
static int buffer = -1;   /* -1 = empty */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

/* ── Producer ────────────────────────────────────────────────────────────── */
void *producer(void *arg)
{
    (void)arg;
    usleep(30000); /* let both consumers reach their wait first */
    pthread_mutex_lock(&mutex);
    buffer = 42;
    printf("[producer] put 42 in buffer, broadcasting to all consumers\n");
    pthread_cond_broadcast(&cond); /* intentionally wake both */
    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* ── Consumer BROKEN: uses 'if' ──────────────────────────────────────────── */
void *consumer_bad(void *arg)
{
    int id = *(int *)arg;
    pthread_mutex_lock(&mutex);

    if (buffer == -1)                 /* BUG: 'if' does not re-check after wake */
        pthread_cond_wait(&cond, &mutex);

    /* The second consumer reaches here with buffer already -1 */
    if (buffer != -1) {
        printf("[consumer %d BAD]  consumed %d\n", id, buffer);
        buffer = -1;
    } else {
        printf("[consumer %d BAD]  woke up but buffer is EMPTY — bug!\n", id);
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* ── Consumer CORRECT: uses 'while' ─────────────────────────────────────── */
void *consumer_good(void *arg)
{
    int id = *(int *)arg;
    pthread_mutex_lock(&mutex);

    while (buffer == -1)              /* CORRECT: re-checks condition on every wake */
        pthread_cond_wait(&cond, &mutex);

    printf("[consumer %d GOOD] consumed %d\n", id, buffer);
    buffer = -1;

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* ── Helpers ─────────────────────────────────────────────────────────────── */
static void run_bad(void)
{
    printf("--- BAD (if): one item, two consumers, broadcast ---\n");
    buffer = -1;

    int ids[2] = {1, 2};
    pthread_t prod, c1, c2;
    pthread_create(&c1,   NULL, consumer_bad, &ids[0]);
    pthread_create(&c2,   NULL, consumer_bad, &ids[1]);
    pthread_create(&prod, NULL, producer,     NULL);
    pthread_join(prod, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
}

static void run_good(void)
{
    /*
     * With 'while', the second consumer goes back to sleep after finding
     * the buffer empty. We give it a second item so both consumers can
     * finish; otherwise the second would wait forever.
     */
    printf("\n--- GOOD (while): two items, two consumers, two signals ---\n");
    buffer = -1;

    /* Producer puts two items sequentially */
    int ids[2] = {1, 2};
    pthread_t c1, c2;
    pthread_create(&c1, NULL, consumer_good, &ids[0]);
    pthread_create(&c2, NULL, consumer_good, &ids[1]);

    /* Give both consumers item one after the other */
    usleep(30000);
    pthread_mutex_lock(&mutex);
    buffer = 10;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    usleep(30000);
    pthread_mutex_lock(&mutex);
    buffer = 20;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
}

int main(void)
{
    run_bad();
    run_good();
    return 0;
}
