#include <ctype.h>
#include <string.h>

#include "func.h"

int parse(char* input, char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1]) {
	int n_tokens = 0;
	int i = 0;
	tokens[0][0] = 0;
	while (input[i]) {
		// skip leading whitespaces
		while (input[i] && isspace(input[i])) {
			++i;
		}
		// add next token to array
		int token_length = 0;
		if (input[i] == '\"') {
			++i;
			// check if token starts with a quote
			while (input[i] && input[i] != '\"') {
				tokens[n_tokens][token_length++] = input[i++];
				tokens[n_tokens][token_length] = 0;
			}
			++i;
		}
		else {
			while (input[i] && !isspace(input[i]) && input[i] != '|') {
				tokens[n_tokens][token_length++] = input[i++];
				tokens[n_tokens][token_length] = 0;
			}
		}
		if (token_length > 0) {
			tokens[++n_tokens][0] = 0;
		}
		// handle pipe characters
		while (input[i] && input[i] == '|') {
			tokens[n_tokens][0] = '|';
			tokens[n_tokens][1] = 0;
			tokens[++n_tokens][0] = 0;
			++i;
		}
	}
	return n_tokens;
}
