#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static sem_t write_lock;    /* exclusive access for writers */
static sem_t read_mutex;    /* protects reader_count */
static int   reader_count = 0;
static int   shared_data  = 0;

/* ── Reader lock / unlock ────────────────────────────────────────────────── */
void reader_lock(void)
{
    sem_wait(&read_mutex);
    reader_count++;
    if (reader_count == 1)
        sem_wait(&write_lock); /* first reader locks out writers */
    sem_post(&read_mutex);
}

void reader_unlock(void)
{
    sem_wait(&read_mutex);
    reader_count--;
    if (reader_count == 0)
        sem_post(&write_lock); /* last reader lets writers in */
    sem_post(&read_mutex);
}

/* ── Writer lock / unlock ────────────────────────────────────────────────── */
void writer_lock(void)  { sem_wait(&write_lock); }
void writer_unlock(void){ sem_post(&write_lock); }

/* ── Thread functions ────────────────────────────────────────────────────── */
void *reader(void *arg)
{
    int id = *(int *)arg;
    reader_lock();
    printf("[reader %d] read shared_data = %d\n", id, shared_data);
    usleep(30000); /* simulate reading time */
    reader_unlock();
    return NULL;
}

void *writer(void *arg)
{
    int val = *(int *)arg;
    writer_lock();
    shared_data = val;
    printf("[writer]   wrote shared_data = %d\n", val);
    usleep(20000); /* simulate writing time */
    writer_unlock();
    return NULL;
}

int main(void)
{
    sem_init(&write_lock, 0, 1);
    sem_init(&read_mutex, 0, 1);
    reader_count = 0;
    shared_data  = 0;

    int rids[4] = {1, 2, 3, 4};
    int wval1 = 42, wval2 = 99;
    pthread_t r1, r2, r3, r4, w1, w2;

    /* Phase 1: two readers concurrently (both should proceed together) */
    printf("--- Phase 1: two concurrent readers ---\n");
    pthread_create(&r1, NULL, reader, &rids[0]);
    pthread_create(&r2, NULL, reader, &rids[1]);
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);

    /* Phase 2: writer updates the value */
    printf("--- Phase 2: writer ---\n");
    pthread_create(&w1, NULL, writer, &wval1);
    pthread_join(w1, NULL);

    /* Phase 3: two more readers see the new value */
    printf("--- Phase 3: two readers after write ---\n");
    pthread_create(&r3, NULL, reader, &rids[2]);
    pthread_create(&r4, NULL, reader, &rids[3]);
    pthread_join(r3, NULL);
    pthread_join(r4, NULL);

    /* Phase 4: second writer */
    printf("--- Phase 4: second writer ---\n");
    pthread_create(&w2, NULL, writer, &wval2);
    pthread_join(w2, NULL);

    sem_destroy(&write_lock);
    sem_destroy(&read_mutex);
    return 0;
}
