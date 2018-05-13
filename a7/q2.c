#include <stdlib.h>
#include <stdio.h>

int x[8] = {1,2,3,-1,-2,0,184,340057058};
int y[8];

int f(int eval) {
	int j = 0;
	while (eval != 0) {
		if (eval & 0x80000000)
			j++;
		eval *= 2;
	}
	return j;
}


int main(int argc, char** argv) {
	for (int i = 7; i >= 0; i--) {
		y[i] = f(x[i]);
	}
	for (int i = 0; i < sizeof(x)/sizeof(x[0]); i++)
		printf("%d\n", x[i]);
	for (int i = 0; i < sizeof(x)/sizeof(x[0]); i++)
		printf("%d\n", y[i]);
}