#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "list.h"

// cc -std=gnu11 trunc.c list.c
// test usage: ./a.out 4 3 5 2 3 apple peach banana grape plum

/*
 * print functions for testing
 */

void print_string (element_t ev) {
  char* e = (char*) ev;
  if (e != NULL) 
  	printf ("%s\n", e);
  else
  	return;
}

void print_int (element_t ev) {
  intptr_t e = (intptr_t) ev;
  printf ("%ld\n", e);
}

/*
 * helper functions for main
 */

void string_to_int(element_t* rv, element_t av) {
	char* ep;
	char* a = (char*) av;
	intptr_t* r = (intptr_t*) rv;
	*r = strtol(a, &ep, 10);
	if (*ep)
		*r = -1;
}

void get_strings(element_t* rv, element_t av, element_t bv) {
	intptr_t a = (intptr_t) av;
	char* b = (char*) bv;
	char** r = (char**) rv;
	if (a != -1) {
		*r = NULL;
	}
	else {
		*r = b;
	}
}

int isPositive(element_t av) {
	intptr_t a = (intptr_t) av;
	return ! (a == -1);
}

int isNotNull(element_t av) {
	char* a = (char*) av;
	return ! (a == NULL);
}

void pairwise(element_t* rv, element_t av, element_t bv) {
	intptr_t a = (intptr_t) av;
	char* b = (char*) bv;
	char** r = (char**) rv;
	*r = strndup(b, a);
}

void max(element_t* rv, element_t av, element_t bv) {
	intptr_t a = (intptr_t) av;
	intptr_t b = (intptr_t) bv;
	intptr_t* r = (intptr_t*) rv;

	*r = a > b ? a : b;
}

int main(int argc, char** argv) {
	int n;
	n = argc - 1;
	// create new list to hold command line args
	struct list* args = list_create();
	// list to hold numbers after parsing
	struct list* int_args = list_create();
	// loop to iterate through command line args
	for (int i = 0; i < n; i++) {
		// 1. iterate through args, append to end of list
		list_append(args, argv[i+1]);
	}

	// 2. map all list elements to ints
	list_map1(string_to_int, int_args, args);

	// 3. list to hold string args only
	struct list* string_args = list_create();
	list_map2(get_strings, string_args, int_args, args);

	// 4. filtered list for negatives
	struct list* no_negs = list_create();
	list_filter(isPositive, no_negs, int_args);

	// 5. filter list to remove all NULLs
	struct list* no_nulls = list_create();
	list_filter(isNotNull, no_nulls, string_args);

	// 6. truncated list of strings
	struct list* trunc = list_create();
	list_map2(pairwise, trunc, no_negs, no_nulls);

	// 7. print truncated list
	list_foreach(print_string, trunc);

	// 8. max value
	intptr_t s = 0;
	list_foldl(max, (element_t*) &s, no_negs);
	printf("%d\n", s);

	list_foreach(free, trunc);

	list_destroy(args);
	list_destroy(int_args);
	list_destroy(string_args);
	list_destroy(no_negs);
	list_destroy(no_nulls);
	list_destroy(trunc);

}

