#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_MAX 1
#define PRODUCER_LOOPS    6
#define CONSUMER_LOOPS    3

static int buf[BUF_MAX];
static int fill_pos = 0;
static int use_pos  = 0;
static int count    = 0;

static pthread_mutex_t mutex    = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cv_empty = PTHREAD_COND_INITIALIZER; /* producers wait */
static pthread_cond_t  cv_fill  = PTHREAD_COND_INITIALIZER; /* consumers wait */

/* ── Buffer operations ───────────────────────────────────────────────────── */
static void put(int v)
{
    buf[fill_pos] = v;
    fill_pos = (fill_pos + 1) % BUF_MAX;
    count++;
}

static int get(void)
{
    int v = buf[use_pos];
    use_pos = (use_pos + 1) % BUF_MAX;
    count--;
    return v;
}

/* ── Producer ────────────────────────────────────────────────────────────── */
void *producer(void *arg)
{
    int id = *(int *)arg;
    for (int i = 0; i < PRODUCER_LOOPS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == BUF_MAX)
            pthread_cond_wait(&cv_empty, &mutex); /* wait on EMPTY */

        put(i);
        printf("[producer %d] put %d  (count=%d)\n", id, i, count);

        pthread_cond_signal(&cv_fill);  /* signal a consumer */
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

        while (count == 0)
            pthread_cond_wait(&cv_fill, &mutex);  /* wait on FILL */

        int v = get();
        printf("[consumer %d] got %d  (count=%d)\n", id, v, count);

        pthread_cond_signal(&cv_empty); /* signal a producer */
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void)
{
    int pids[2] = {1};
    int cids[2] = {1, 2};
    pthread_t p1, c1, c2;

    pthread_create(&p1, NULL, producer, &pids[0]);
    pthread_create(&c1, NULL, consumer, &cids[0]);
    pthread_create(&c2, NULL, consumer, &cids[1]);

    pthread_join(p1, NULL);
    pthread_join(c1, NULL); 
    pthread_join(c2, NULL);

    printf("All done — no deadlock.\n");
    return 0;
}
