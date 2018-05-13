#include <stdio.h>
#include <stdlib.h>

int l;

int q2 (int i, int j, int k) {
  static const void* jumpTable[9] = { &&L10, &&DEFAULT, &&L12, &&DEFAULT, &&L14, &&DEFAULT, &&L16, &&DEFAULT, &&L18 };
  
  if (i < 10 || i > 18) goto DEFAULT;
  goto *jumpTable [i-10];
L10: // i = 10
  k = k + j;
  l = k;
  goto CONT;
L12: // i = 12
  k = j - k;
  l = k;
  goto CONT;
L14: // i = 14
  k = j - k;
  if (k > 0) 
  	l = 1;
  else 
  	l = 0;
  goto CONT;
L16: // i = 16
  j = k - j;
  if (j > 0)
  	l = 1;
  else
  	l = 0;
  goto CONT;
L18: // i = 18
  k = j - k;
  if (k == 0) {
  	l = 1;
  }
  else {
  	l = 0;
  }
  goto CONT;
DEFAULT:
  l = 0;
  goto CONT;
CONT:
  return l;
}

int main(int argc, char** argv) {
  for (int i=10; i<=18; i++)
    printf ("%d: %d\n", i, q2(i, 11, 11));
}
