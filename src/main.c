#include <stdio.h>

#include "func.h"

int main(void) {
	char input[MAX_LENGTH];
	while (1) {
		printf("sh> ");
		fflush(stdout);
		fgets(input, MAX_LENGTH, stdin);
		handle_input(input);
	}
	return 0;
}
