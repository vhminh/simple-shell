#include <stdio.h>
#include <string.h>

#include "func.h"

char history_commands[HISTORY_SIZE][MAX_LENGTH];
int history_count = 0;  // number of commands executed so far
int last_exec = -1;     // index of last executed command

void print_history() {
	if (history_count == 0) {
		// empty history
		printf("No commands in history\n");
	} else if (history_count < HISTORY_SIZE) {
		// if there are less than HISTORY_SIZE commands, print it backward
		for (int i = last_exec; i >= 0; --i) {
			printf("%d %s\n", last_exec - i + 1, history_commands[i]);
		}
	} else {
		// print backward in a circular list, until meet the last_exec again
		int ind = last_exec;
		int i = 1;
		do {
			printf("%d %s\n", i++, history_commands[ind]);
			ind--;
			if (ind < 0) ind += HISTORY_SIZE;
		} while (ind != last_exec);
	}
}

void add_to_history(char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1],
	                int n_tokens) {
	// point last_exec to the next position
	++history_count;
	++last_exec;
	if (last_exec >= HISTORY_SIZE) {
		last_exec -= HISTORY_SIZE;
	}
	// store the command at history_commmands[last_exec]
	strncpy(history_commands[last_exec], tokens[0], MAX_LENGTH);
	for (int i = 1; i < n_tokens; ++i) {
		strcat(history_commands[last_exec], " ");
		strcat(history_commands[last_exec], tokens[i]);
	}
}

char* get_last_executed() {
	if (last_exec == -1) {
		return 0;
	}
	return history_commands[last_exec];
}
