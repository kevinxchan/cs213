#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_ITERATIONS 1000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

struct Agent {
  uthread_mutex_t mutex;
  uthread_cond_t  match;
  uthread_cond_t  paper;
  uthread_cond_t  tobacco;
  uthread_cond_t  smoke;

  uthread_cond_t smoke_match;
  uthread_cond_t smoke_paper;
  uthread_cond_t smoke_tobacco;

  uthread_cond_t has_smoked_match;
  uthread_cond_t has_smoked_paper;
  uthread_cond_t has_smoked_tobacco;

  int MATCH;
  int PAPER;
  int TOBACCO;
};

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent));
  agent->mutex   = uthread_mutex_create();
  agent->paper   = uthread_cond_create (agent->mutex);
  agent->match   = uthread_cond_create (agent->mutex);
  agent->tobacco = uthread_cond_create (agent->mutex);
  agent->smoke   = uthread_cond_create (agent->mutex);

  agent->smoke_match = uthread_cond_create(agent->mutex);
  agent->smoke_paper = uthread_cond_create(agent->mutex);
  agent->smoke_tobacco = uthread_cond_create(agent->mutex);

  agent->has_smoked_match = uthread_cond_create(agent->mutex);
  agent->has_smoked_paper = uthread_cond_create(agent->mutex);
  agent->has_smoked_tobacco = uthread_cond_create(agent->mutex);

  agent->MATCH = 0;
  agent->PAPER = 0;
  agent->TOBACCO = 0;

  return agent;
}

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked

void* tobaccoNotify(void* av) {
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex);
	VERBOSE_PRINT("tobaccoNotify called\n");
	while (1) {
		VERBOSE_PRINT("tobaccoNotify waiting for a->tobacco\n");
		uthread_cond_wait(a->tobacco);
		a->TOBACCO = 1;

		if (a->MATCH && a->TOBACCO) {
			a->MATCH = 0;
			a->TOBACCO = 0;
			VERBOSE_PRINT("tobaccoNotify telling paper to smoke\n");
			uthread_cond_signal(a->smoke_paper);
			uthread_cond_wait(a->has_smoked_paper);
			uthread_cond_signal(a->smoke);
		}
		else if (a->PAPER && a->TOBACCO) {
			a->PAPER = 0;
			a->TOBACCO = 0;
			VERBOSE_PRINT("tobaccoNotify telling match to smoke\n");
			uthread_cond_signal(a->smoke_match);
			uthread_cond_wait(a->has_smoked_match);
			uthread_cond_signal(a->smoke);
		}
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

void* tobaccoSmoke(void* av) {
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex);
	while (1) {
		VERBOSE_PRINT("tobaccoSmoke waiting for a->smoke_tobacco\n");
		uthread_cond_wait(a->smoke_tobacco);
		VERBOSE_PRINT("tobacco smoking\n");
		smoke_count[TOBACCO]++;
		uthread_cond_signal(a->has_smoked_tobacco);
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

void* paperNotify(void* av){
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex);
	VERBOSE_PRINT("paperNotify called\n");
	while(1){
		VERBOSE_PRINT("paperNotify waiting for a->paper\n");
		uthread_cond_wait(a->paper);
		a->PAPER = 1;
		if (a->TOBACCO && a->PAPER) {
			a->TOBACCO = 0;
			a->PAPER = 0;
			VERBOSE_PRINT("paperNotify telling match to smoke\n");
			uthread_cond_signal(a->smoke_match);
			uthread_cond_wait(a->has_smoked_match);
			uthread_cond_signal(a->smoke);
		}
		else if (a->MATCH && a->PAPER) {
			a->MATCH = 0;
			a->PAPER = 0;
			VERBOSE_PRINT("paperNotify telling tobacco to smoke\n");
			uthread_cond_signal(a->smoke_tobacco);
			uthread_cond_wait(a->has_smoked_tobacco);
			uthread_cond_signal(a->smoke);
		}
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

void* paperSmoke(void* av) {
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex);
	while (1) {
		VERBOSE_PRINT("paperSmoke waiting for a->smoke_paper\n");
		uthread_cond_wait(a->smoke_paper);
		VERBOSE_PRINT("paper smoking\n");
		smoke_count[PAPER]++;
		uthread_cond_signal(a->has_smoked_paper);
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

void* matchNotify(void* av) {
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex);
	VERBOSE_PRINT("matchNotify called\n");
	while (1) {
		VERBOSE_PRINT("matchNotify waiting for a->match\n");
		uthread_cond_wait(a->match);
		a->MATCH = 1;
		if (a->PAPER && a->MATCH) {
			a->PAPER = 0;
			a->MATCH = 0;
			VERBOSE_PRINT("matchNotify telling tobacco to smoke\n");
			uthread_cond_signal(a->smoke_tobacco);
			uthread_cond_wait(a->has_smoked_tobacco);
			uthread_cond_signal(a->smoke);
		}
		else if (a->TOBACCO && a->MATCH) {
			a->TOBACCO = 0;
			a->MATCH = 0;
			VERBOSE_PRINT("matchNotify telling paper to smoke\n");
			uthread_cond_signal(a->smoke_paper);
			uthread_cond_wait(a->has_smoked_paper);
			uthread_cond_signal(a->smoke);
		}
		
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

void* matchSmoke(void* av) {
	struct Agent* a = av;
	uthread_mutex_lock(a->mutex); // acquire lock
	while (1) {
		VERBOSE_PRINT("matchSmoke waiting for a->smoke_match\n");
		uthread_cond_wait(a->smoke_match); // wait for smoke signal
		VERBOSE_PRINT("match smoking\n");
		smoke_count[MATCH]++;
		uthread_cond_signal(a->has_smoked_match);
	}
	uthread_mutex_unlock(a->mutex);
	return NULL;
}

/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can re-write it if you like, but be sure that all it does
 * is choose 2 random reasources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* av) {
  struct Agent* a = av;
  static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
  static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};
  
  uthread_mutex_lock (a->mutex);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
      int r = random() % 3; // random number between 0 and 3
      signal_count [matching_smoker [r]] ++;
      int c = choices [r];
      if (c & MATCH) {
        VERBOSE_PRINT ("match available\n");
        uthread_cond_signal (a->match);
      }
      if (c & PAPER) {
        VERBOSE_PRINT ("paper available\n");
        uthread_cond_signal (a->paper);
      }
      if (c & TOBACCO) {
        VERBOSE_PRINT ("tobacco available\n");
        uthread_cond_signal (a->tobacco);
      }
      VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
      uthread_cond_wait (a->smoke);
    }
  uthread_mutex_unlock (a->mutex);
  return NULL;
}

int main (int argc, char** argv) {
  uthread_t t[7];
  uthread_init (7);
  struct Agent*  a = createAgent();

  t[0] = uthread_create(tobaccoNotify, a);
  t[1] = uthread_create(paperNotify, a);
  t[2] = uthread_create(matchNotify, a);

  t[3] = uthread_create(agent, a);

  t[4] = uthread_create(tobaccoSmoke, a);
  t[5] = uthread_create(paperSmoke, a);
  t[6] = uthread_create(matchSmoke, a);


  uthread_join (t[3], 0);

  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);
  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
}