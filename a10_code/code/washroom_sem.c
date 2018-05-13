#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"
#include "uthread_sem.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

#define MAX_OCCUPANCY      3
#define NUM_ITERATIONS     100
#define NUM_PEOPLE         20
#define FAIR_WAITING_COUNT 4

/**
 * You might find these declarations useful.
 */
enum GenderIdentity {MALE = 0, FEMALE = 1};
const static enum GenderIdentity otherGender [] = {FEMALE, MALE};

struct Washroom {
  // TODO
};

struct Washroom* createWashroom() {
  struct Washroom* washroom = malloc (sizeof (struct Washroom));
  // TODO
  return washroom;
}

struct Washroom* washroom;

#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_PEOPLE)
int             entryTicker;                                          // incremented with each entry
int             waitingHistogram         [WAITING_HISTOGRAM_SIZE];
int             waitingHistogramOverflow;
uthread_sem_t waitingHistogramMutex;
int             occupancyHistogram       [2] [MAX_OCCUPANCY + 1];

void enterWashroom (enum GenderIdentity g) {
  // TODO
}

void leaveWashroom() {
  // TODO
}

void recordWaitingTime (int waitingTime) {
  uthread_sem_wait (waitingHistogramMutex);
  if (waitingTime < WAITING_HISTOGRAM_SIZE)
     waitingHistogram [waitingTime] ++;
  else
     waitingHistogramOverflow ++;
  uthread_sem_signal (waitingHistogramMutex);
}

//
// TODO
// You will probably need to create some additional produres etc.
//

int main (int argc, char** argv) {
  uthread_init (1);
  washroom = createWashroom();
  uthread_t pt [NUM_PEOPLE];
  waitingHistogramMutex = uthread_sem_create (1);

  // TODO

  printf ("Times with 1 person who identifies as male   %d\n", occupancyHistogram [MALE]   [1]);
  printf ("Times with 2 people who identifies as male   %d\n", occupancyHistogram [MALE]   [2]);
  printf ("Times with 3 people who identifies as male   %d\n", occupancyHistogram [MALE]   [3]);
  printf ("Times with 1 person who identifies as female %d\n", occupancyHistogram [FEMALE] [1]);
  printf ("Times with 2 people who identifies as female %d\n", occupancyHistogram [FEMALE] [2]);
  printf ("Times with 3 people who identifies as female %d\n", occupancyHistogram [FEMALE] [3]);
  printf ("Waiting Histogram\n");
  for (int i=0; i<WAITING_HISTOGRAM_SIZE; i++)
  if (waitingHistogram [i])
  printf ("  Number of times people waited for %d %s to enter: %d\n", i, i==1?"person":"people", waitingHistogram [i]);
  if (waitingHistogramOverflow)
  printf ("  Number of times people waited more than %d entries: %d\n", WAITING_HISTOGRAM_SIZE, waitingHistogramOverflow);
}
