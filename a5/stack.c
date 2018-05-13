#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Element {
  char   name[200];
  struct Element *next;
};

struct Element *top = 0;

void push (char* aName) {
  struct Element* e = malloc (sizeof (*e));    // Not the bug: sizeof (*e) == sizeof(struct Element)
  strncpy (e->name, aName, sizeof (e->name));  // Not the bug: sizeof (e->name) == 200
  e->next  = top;
  top = e;
}

void pop(char* ret) {
  struct Element* e = top;
  strncpy(ret, e->name, 200);
  top = e->next;
  free (e);
}

int main (int argc, char** argv) {
  char name[200];
  push ("A");
  push ("B");

  char* w = malloc(sizeof(name)); 
  char* x = malloc(sizeof(name)); 
  char* y = malloc(sizeof(name)); 
  char* z = malloc(sizeof(name));

  pop(w);

  push ("C");
  push ("D");

  pop(x);
  pop(y);
  pop(z);

  printf ("%s %s %s %s\n", w, x, y, z);

  free(w);
  free(x);
  free(y);
  free(z);
}
