/*
 * 11_sem_deadlock_order.c
 * BROKEN: Acquiring the mutex BEFORE the slot semaphore → deadlock
 *
 * The wrong order:
 *   sem_wait(&mutex);        // producer holds the lock
 *   sem_wait(&empty_slots);  // buffer is full → producer sleeps HOLDING mutex
 *   ...
 *   // Consumer tries sem_wait(&mutex) → blocked because producer holds it
 *   // Producer waits for consumer to free a slot → blocked on empty_slots
 *   // DEADLOCK: each waits for the other
 *
 * The correct order (example 10):
 *   sem_wait(&empty_slots);  // wait for room FIRST (may sleep without lock)
 *   sem_wait(&mutex);        // THEN take the lock (buffer not full here)
 *
 * This file runs the BROKEN version and uses a watcher thread to
 * detect and confirm the deadlock after 0.5 s.
 *
 * Compile: gcc -Wall -o 11_sem_deadlock_order 11_sem_deadlock_order.c -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUF_MAX 1  /* size 1 makes deadlock immediate */

static int buf      = 0;
static int fill_pos = 0;
static int use_pos  = 0;

static sem_t empty_slots;
static sem_t full_slots;
static sem_t mutex;

static volatile int deadlocked = 0;

/* ── Buffer ops ──────────────────────────────────────────────────────────── */
static void put(int v) { buf = v; fill_pos = (fill_pos + 1) % BUF_MAX; }
static int  get(void)  { int v = buf; use_pos = (use_pos + 1) % BUF_MAX; return v; }

/* ── BROKEN producer: locks mutex FIRST ─────────────────────────────────── */
void *producer_broken(void *arg)
{
    (void)arg;
    for (int i = 0; i < 4; i++) {
        sem_wait(&mutex);        /* BUG: acquires lock before checking for space */
        sem_wait(&empty_slots);  /* if buffer full: sleeps WHILE HOLDING mutex  */
        put(i);
        printf("[producer BROKEN] put %d\n", i);
        sem_post(&mutex);
        sem_post(&full_slots);
        if (deadlocked) return NULL;
    }
    return NULL;
}

/* ── BROKEN consumer: same wrong order ──────────────────────────────────── */
void *consumer_broken(void *arg)
{
    (void)arg;
    for (int i = 0; i < 4; i++) {
        sem_wait(&mutex);       /* BUG: tries to lock, but producer holds it */
        sem_wait(&full_slots);
        int v = get();
        printf("[consumer BROKEN] got %d\n", v);
        sem_post(&mutex);
        sem_post(&empty_slots);
        if (deadlocked) return NULL;
    }
    return NULL;
}

/* ── Watcher ─────────────────────────────────────────────────────────────── */
void *watcher(void *arg)
{
    (void)arg;
    usleep(500000);
    deadlocked = 1;
    printf("[watcher] timeout — deadlock confirmed (wrong sem order)\n");
    /* Release everything to unblock threads */
    sem_post(&mutex);
    sem_post(&empty_slots);
    sem_post(&full_slots);
    sem_post(&mutex);
    sem_post(&empty_slots);
    sem_post(&full_slots);
    return NULL;
}

int main(void)
{
    sem_init(&empty_slots, 0, BUF_MAX);
    sem_init(&full_slots,  0, 0);
    sem_init(&mutex,       0, 1);

    pthread_t p, c, w;
    pthread_create(&w, NULL, watcher,         NULL);
    pthread_create(&p, NULL, producer_broken, NULL);
    pthread_create(&c, NULL, consumer_broken, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);
    pthread_join(w, NULL);

    printf("\nFix: always wait on empty/full BEFORE acquiring the mutex.\n");
    printf("See 10_sem_bounded_buffer.c for the correct version.\n");

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);
    return 0;
}
