#include <stdlib.h>
#include <stdio.h>

int *aptr;

void adderHelper(int add, int index) {
	aptr[index] += add;
}

int main(int argc, char** argv) {
	aptr = malloc(sizeof(int)*10);
	adderHelper(3, 4);
	adderHelper(1, 2);
	for (int i = 0; i < 10; i++)
		printf("%d\n", aptr[i]);
}
