#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "uthread.h"
#include "uthread_sem.h"


#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int producer_wait_count;     // # of times producer had to wait
int consumer_wait_count;     // # of times consumer had to wait
int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

struct Sem {
  uthread_sem_t mutex;
  uthread_sem_t notMax;
	uthread_sem_t numItems; // sem for number of items
	int items;
};

struct Sem* createSem() {
	struct Sem* s = malloc(sizeof(struct Sem));
	s->mutex = uthread_sem_create(1);
  s->notMax = uthread_sem_create(MAX_ITEMS);
	s->numItems = uthread_sem_create(0);
	s->items = 0;
	return s;
}

void* producer (void* v) {
  struct Sem* s = v;
  // 1. acquire wrap_mutex
  for (int i=0; i<NUM_ITERATIONS; i++) {
    VERBOSE_PRINT("producer wait on s->notMax\n");
    uthread_sem_wait(s->notMax);

    // 2. acquire mutex
    // CRITICAL SECTION
    uthread_sem_wait(s->mutex);
      assert(s->items < MAX_ITEMS);
  	  s->items++;
      VERBOSE_PRINT("producer inc'd s->items: %d\n", s->items);
      VERBOSE_PRINT("producer signaling s->numItems\n");
      uthread_sem_signal(s->numItems);

  	  histogram[s->items]++;
    // END CRITICAL SECTION
    uthread_sem_signal(s->mutex);
  }
  return NULL;
}

void* consumer (void* v) {
  struct Sem* s = v;
  // 1. acquire wrap_mutex
  for (int i=0; i<NUM_ITERATIONS; i++) {  
    VERBOSE_PRINT("consumer waiting on s->numItems\n");
    uthread_sem_wait(s->numItems);
    // 2. acquire mutex
    // CRITICAL SECTION
    uthread_sem_wait(s->mutex);
      assert(s->items > 0);
      s->items--;
      VERBOSE_PRINT("consumer dec'd s->items: %d\n", s->items);
      VERBOSE_PRINT("consumer signaling s->notMax since we have consumed\n");
      uthread_sem_signal(s->notMax);
      histogram[s->items]++;
	  uthread_sem_signal(s->mutex);
  }
  return NULL;
}

int main (int argc, char** argv) {
  uthread_t t[4];

  uthread_init (4);
  struct Sem* s = createSem();

  for (int i = 0; i < NUM_PRODUCERS; i++) {
  	t[i] = uthread_create(producer, s);
  }

  for (int i = 0; i < NUM_CONSUMERS; i++) {
  	t[i + NUM_PRODUCERS] = uthread_create(consumer, s);
  }

  for (int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; i++) {
  	uthread_join(t[i], NULL);
  }

  printf ("producer_wait_count=%d, consumer_wait_count=%d\n", producer_wait_count, consumer_wait_count);
  printf ("items value histogram:\n");
  int sum=0;
  for (int i = 0; i <= MAX_ITEMS; i++) {
    printf ("  items=%d, %d times\n", i, histogram [i]);
    sum += histogram [i];
  }
  assert (sum == sizeof (t) / sizeof (uthread_t) * NUM_ITERATIONS);
}
