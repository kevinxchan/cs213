#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

#define MAX_OCCUPANCY      3
#define NUM_ITERATIONS     100
#define NUM_PEOPLE         20
#define FAIR_WAITING_COUNT 4
#define SECOND_LAST_PERSON NUM_ITERATIONS * NUM_PEOPLE - 1

/**
 * You might find these declarations useful.
 */
enum GenderIdentity {MALE = 0, FEMALE = 1};
const static enum GenderIdentity otherGender [] = {FEMALE, MALE};

struct Washroom {
  uthread_mutex_t mutex;
  uthread_cond_t canEnter;
  uthread_cond_t female;
  uthread_cond_t male;
  uthread_cond_t cap;
  uthread_cond_t fcap;
  uthread_cond_t mcap;
  
  int numIn;
  enum GenderIdentity genderIn;
  int fwait;
  int mwait;
  int count;
  int BLOCK;
  int firstRound;
};

struct Washroom* createWashroom() {
  struct Washroom* washroom = malloc (sizeof (struct Washroom));
  washroom->mutex = uthread_mutex_create();

  washroom->canEnter = uthread_cond_create(washroom->mutex);
  washroom->female = uthread_cond_create(washroom->mutex);
  washroom->male = uthread_cond_create(washroom->mutex);
  washroom->cap = uthread_cond_create(washroom->mutex);
  washroom->mcap = uthread_cond_create(washroom->mutex);
  washroom->fcap = uthread_cond_create(washroom->mutex);

  washroom->numIn = 0;
  washroom->genderIn = -1;
  washroom->fwait = 0;
  washroom->mwait = 0;
  washroom->count = 0; 
  washroom->BLOCK = 0;
  washroom->firstRound = 1;
  return washroom;
}

struct Washroom* washroom;

#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_PEOPLE)
int             entryTicker;                                          // incremented with each entry
int             waitingHistogram         [WAITING_HISTOGRAM_SIZE];
int             waitingHistogramOverflow;
uthread_mutex_t waitingHistogrammutex;
int             occupancyHistogram       [2] [MAX_OCCUPANCY + 1];

void recordWaitingTime (int waitingTime) {
  uthread_mutex_lock (waitingHistogrammutex);
  if (waitingTime < WAITING_HISTOGRAM_SIZE)
    waitingHistogram [waitingTime] ++;
  else
    waitingHistogramOverflow ++;
  uthread_mutex_unlock (waitingHistogrammutex);
}

void waitGender(struct Washroom* w, enum GenderIdentity g) {
  if (g == FEMALE) {
    uthread_cond_wait(w->female);
  }
  else if (g == MALE) {
    uthread_cond_wait(w->male);
  }
}

void decWait(struct Washroom* w, enum GenderIdentity g) {
  if (g == FEMALE && w->fwait > 0) {
    w->fwait--;
    VERBOSE_PRINT("decWait female decrementing w->fwait, current #: %d\n", w->fwait);
  } else if (g == MALE && w->mwait > 0) {
    w->mwait--;
    VERBOSE_PRINT("decWait male decrementing w->mwait, current #: %d\n", w->mwait);
  }
}

void flush(struct Washroom* w, int count, enum GenderIdentity g) {
  entryTicker++;
  VERBOSE_PRINT("flush entryTicker after inc: %d\n", entryTicker);
  if (w->numIn == 0) {
    VERBOSE_PRINT("w->numIn == 0, setting w->genderIn to g. g = %d\n", g);
    w->genderIn = g;
  }
  w->numIn++; 
  VERBOSE_PRINT("flush w->numIn after inc: %d\n", w->numIn);
  VERBOSE_PRINT("flush updating occupancyHistogram\n");
  occupancyHistogram[w->genderIn][w->numIn]++;
}

void* enterWashroom (struct Washroom* w, enum GenderIdentity g) {
  uthread_mutex_lock(w->mutex);
  int once = 1;
  int tmp = 0;
  int onBlock = 0;
  int onCap = 0;
  int onGender = 0;
  while (1) {
    // no one is allowed in the washroom
    if (w->BLOCK) {
      if (g == FEMALE) {
        w->fwait++; 
        VERBOSE_PRINT("enterWashroom FEMALE blocked from entering, w->fwait: %d\n", w->fwait);
      } else if (g == MALE) {
        w->mwait++;
        VERBOSE_PRINT("enterWashroom MALE blocked from entering, w->mwait: %d\n", w->mwait);
      }
      VERBOSE_PRINT("enterWashroom someone waiting to enter, but blocked...setting onBlock to 1\n");
      onBlock = 1;
    }
    else if (!w->BLOCK && w->numIn == MAX_OCCUPANCY) {
      if (g == FEMALE) {
        w->fwait++;
        VERBOSE_PRINT("enterWashroom FEMALE at MAX_OCCUPANCY, w->fwait: %d\n", w->fwait);
      } else if (g == MALE) {
        w->mwait++;
        VERBOSE_PRINT("enterWashroom MALE at MAX_OCCUPANCY, w->mwait: %d\n", w->mwait);
     } 
     VERBOSE_PRINT("enterWashroom someone waiting to enter but MAX_OCCUPANCY...setting onCap to 1\n");
     onCap = 1;
     }
    else if (!w->BLOCK && w->genderIn != g && w->genderIn != -1 && w->numIn < MAX_OCCUPANCY && w->numIn >= 0) {
      if (g == FEMALE) {
        w->fwait++;
        VERBOSE_PRINT("enterWashroom FEMALE waiting on different gender, w->fwait: %d\n", w->fwait);
      } else if (g == MALE) {
        w->mwait++;
        VERBOSE_PRINT("enterWashroom MALE waiting on different gender, w->mwait: %d\n", w->mwait);
      }
      VERBOSE_PRINT("enterWashroom someone waiting to enter, but opposite gender currently in...setting onGender to 1\n");
      onGender = 1;
    }
    else if ((!w->BLOCK && w->numIn == 0 && w->firstRound) || (!w->BLOCK && w->numIn == 0 && w->genderIn == g) 
    	|| (!w->BLOCK && w->numIn < MAX_OCCUPANCY && w->genderIn == g) || (!w->BLOCK && w->numIn == 0 && w->genderIn == -1)) {
      VERBOSE_PRINT("enterWashroom conditions are good, we're busting out of this while loop! gender = %d\n", g);
      break;
    }
    if (once) {
      tmp = entryTicker;
      once = 0;
    }
    if (onBlock) {
      VERBOSE_PRINT("enterWashroom waiting on w->canEnter (blocked)!\n");
      uthread_cond_wait(w->canEnter);
      onBlock = 0;
    } else if (onCap) {
      if (g == FEMALE) {
      	VERBOSE_PRINT("enterWashroom FEMALE waiting on w->fcap!\n");
      	uthread_cond_wait(w->fcap);
      } else if (g == MALE) {
      	VERBOSE_PRINT("enterWashroom MALE waiting on w->mcap!\n");
      	uthread_cond_wait(w->mcap);
      }
      onCap = 0;
    } else if (onGender) {
      VERBOSE_PRINT("enterWashroom waiting on gender!\n");
      waitGender(w, g);
      onGender = 0;
    }
  }


  VERBOSE_PRINT("WE ARE NOW IN THE WASHROOM\n");


  if (w->numIn == 0 && w->firstRound) {
    VERBOSE_PRINT("w->numIn == 0, setting w->genderIn to g. g = %d\n", g);
    w->genderIn = g;
  }

  assert(w->genderIn == g);
  assert(w->numIn < MAX_OCCUPANCY);
  entryTicker++;
  VERBOSE_PRINT("enterWashroom entryTicker after inc: %d\n", entryTicker);
  w->numIn++;
  VERBOSE_PRINT("enterWashroom inc'd w->numIn: %d\n", w->numIn);
  occupancyHistogram[w->genderIn][w->numIn]++;

  uthread_mutex_unlock(w->mutex);
  return NULL;
}

void* leaveWashroom(struct Washroom* w, enum GenderIdentity g) {
  uthread_mutex_lock(w->mutex);
  w->numIn--;
  VERBOSE_PRINT("leaveWashroom dec w->numIn, current after #: %d\n", w->numIn);
  if (w->genderIn == FEMALE) {
    if (w->mwait > FAIR_WAITING_COUNT) {
      w->BLOCK = 1;
      if (w->numIn == 0) {
        VERBOSE_PRINT("leaveWashroom w->numIn is EMPTY, flipping gender to MALE\n");
        w->genderIn = MALE;
        w->mwait = 0;
        w->BLOCK = 0;
        w->firstRound = 0;
        VERBOSE_PRINT("leaveWashroom broadcasting male, canEnter, mcap!!!\n");
        uthread_cond_broadcast(w->male);
        uthread_cond_broadcast(w->canEnter);
        uthread_cond_broadcast(w->mcap);
      }
    }
    else {
      VERBOSE_PRINT("leaveWashroom currently FEMALE, not enough MALES waiting\n");
      VERBOSE_PRINT("leaveWashroom broadcasting w->female, w->male, w->fcap and w->mcap!!!\n");
      uthread_cond_broadcast(w->female);
      uthread_cond_broadcast(w->male);
      uthread_cond_broadcast(w->fcap);
      uthread_cond_broadcast(w->mcap);
      if (entryTicker == SECOND_LAST_PERSON) {
      	VERBOSE_PRINT("leaveWashroom SECOND_LAST_PERSON is FEMALE, flipping gender to MALE\n");
      	w->genderIn = MALE;
      }
    }
  }
  else if (w->genderIn == MALE) {
    if (w->fwait > FAIR_WAITING_COUNT) {
      w->BLOCK = 1;
      if (w->numIn == 0) {
        VERBOSE_PRINT("leaveWashroom w->numIn is EMPTY, flipping gender to FEMALE\n");
        w->genderIn = FEMALE;
        w->fwait = 0;
        w->BLOCK = 0;
        w->firstRound = 0;
        VERBOSE_PRINT("leaveWashroom broadcasting female, canEnter and fcap!!!\n");
        uthread_cond_broadcast(w->female);
        uthread_cond_broadcast(w->canEnter);
        uthread_cond_broadcast(w->fcap);
      }
    }
    else {
      VERBOSE_PRINT("leaveWashroom currently MALE, not enough FEMALES waiting\n");
      VERBOSE_PRINT("leaveWashroom broadcasting w->female, w->male, w->fcap and w->mcap!!!\n");
      uthread_cond_broadcast(w->female);
      uthread_cond_broadcast(w->male);
      uthread_cond_broadcast(w->fcap);
      uthread_cond_broadcast(w->mcap);
      if (entryTicker == SECOND_LAST_PERSON) {
      	VERBOSE_PRINT("leaveWashroom SECOND_LAST_PERSON is MALE, flipping gender to FEMALE\n");
      	w->genderIn = FEMALE;
      }
    }
  }
  uthread_mutex_unlock(w->mutex);
  return NULL;
}

//
// TODO
// You will probably need to create some additional produres etc.
//

void* create(void* wv) {
  struct Washroom* w = wv;
  enum GenderIdentity g = random() % 2;
  if (g == FEMALE) { 
    VERBOSE_PRINT("INIT FEMALE\n");
  }
  else {
    VERBOSE_PRINT("INIT MALE\n");
  }
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    enterWashroom(w, g);
    for (int j = 0; j < NUM_PEOPLE; j++)
      uthread_yield();
    leaveWashroom(w, g);
    for (int j = 0; j < NUM_PEOPLE; j++)
      uthread_yield();
  }
  return NULL;
}

int main (int argc, char** argv) {
  uthread_init (1);
  washroom = createWashroom();
  uthread_t pt [NUM_PEOPLE];
  waitingHistogrammutex = uthread_mutex_create ();

  for (int i = 0; i < NUM_PEOPLE; i++) {
    pt[i] = uthread_create(create, washroom);
  }
  
  for (int i = 0; i < NUM_PEOPLE; i++)
    uthread_join(pt[i], NULL);

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
