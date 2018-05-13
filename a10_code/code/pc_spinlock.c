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

int items = 0;
spinlock_t lock;

void* producer (void* v) {
  for (int i=0; i<NUM_ITERATIONS; i++) {
    spinlock_lock(&lock);
    while (items >= MAX_ITEMS) {
    	spinlock_unlock(&lock);
 	    producer_wait_count++;
    	spinlock_lock(&lock);
    }
    assert(items < MAX_ITEMS);
    items++;
    histogram[items]++;
    spinlock_unlock(&lock);
  }
  return NULL;
}

void* consumer (void* v) {
  for (int i=0; i<NUM_ITERATIONS; i++) {
    spinlock_lock(&lock);
    while (items <= 0) {
      spinlock_unlock(&lock);
    	consumer_wait_count++;
    	spinlock_lock(&lock); // release lock for producer
    }
    assert(items > 0);
    items--;
    histogram[items]++;
    spinlock_unlock(&lock);
  }
  
  return NULL;
}

int main (int argc, char** argv) {
  uthread_t t[4];

  uthread_init (4);
  spinlock_create(&lock);

  t[0] = uthread_create(producer, NULL);
  t[1] = uthread_create(producer, NULL);
  t[2] = uthread_create(consumer, NULL);
  t[3] = uthread_create(consumer, NULL);

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
