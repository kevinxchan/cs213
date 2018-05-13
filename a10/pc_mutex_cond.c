#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"
#include "spinlock.h"

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int producer_wait_count;     // # of times producer had to wait
int consumer_wait_count;     // # of times consumer had to wait
int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

struct PC {
  uthread_mutex_t mx;
  uthread_cond_t need_items;
  uthread_cond_t have_items;
  int items;
} typedef PC;

struct PC* createPC() {
  struct PC* p = malloc(sizeof(PC));
  p->mx = uthread_mutex_create();
  p->need_items = uthread_cond_create(p->mx);
  p->have_items = uthread_cond_create(p->mx);
  p->items = 0;
  return p;
}

void* producer (void* v) {
  PC* p = v;
  for (int i=0; i<NUM_ITERATIONS; i++) {
    uthread_mutex_lock(p->mx);
    while (p->items == MAX_ITEMS) {
      producer_wait_count++;
      uthread_cond_wait(p->need_items);
    }
    assert(p->items < MAX_ITEMS);
    p->items++;
    histogram[p->items]++;
    uthread_cond_signal(p->have_items);
    uthread_mutex_unlock(p->mx);
  }
  return NULL;
}

void* consumer (void* v) {
  PC* p = v;
  for (int i=0; i<NUM_ITERATIONS; i++) {
    uthread_mutex_lock(p->mx);
    while (p->items == 0) {
      consumer_wait_count++;
      uthread_cond_wait(p->have_items);
    }
    assert(p->items > 0);
    p->items--;
    histogram[p->items]++;
    uthread_cond_signal(p->need_items);
    uthread_mutex_unlock(p->mx);
  }
  return NULL;
}

int main (int argc, char** argv) {
  uthread_t t[4];

  uthread_init (4);
  PC* p = createPC();

  t[0] = uthread_create(producer, p);
  t[1] = uthread_create(producer, p);
  t[2] = uthread_create(consumer, p);
  t[3] = uthread_create(consumer, p);

  uthread_join(t[0], NULL);
  uthread_join(t[1], NULL);
  uthread_join(t[2], NULL);
  uthread_join(t[3], NULL);
  
  printf ("producer_wait_count=%d, consumer_wait_count=%d\n", producer_wait_count, consumer_wait_count);
  printf ("items value histogram:\n");
  int sum=0;
  for (int i = 0; i <= MAX_ITEMS; i++) {
    printf ("  items=%d, %d times\n", i, histogram [i]);
    sum += histogram [i];
  }
  assert (sum == sizeof (t) / sizeof (uthread_t) * NUM_ITERATIONS);
}
