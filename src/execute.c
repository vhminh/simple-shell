#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "func.h"

pid_t exec_command_async(char* args[], int infd, int outfd,
	                     int fd_pipes[MAX_LENGTH / 2][2], int n_pipes) {
	// extract stdin, stdout redirection path
	// and remove them from args
	char *in_path = NULL, *out_path = NULL;
	int i, j;
	for (i = 0, j = 0; args[i]; ++i) {
		if (strcmp(args[i], ">") == 0) {
			out_path = args[++i];
			continue;
		}
		if (strcmp(args[i], "<") == 0) {
			in_path = args[++i];
			continue;
		}
		args[j++] = args[i];
	}
	args[j] = NULL;
	// run the command in a child process
	pid_t pid = fork();
	if (pid < 0) {
		printf("Error: Unable to create child process\n");
		return pid;
	} else if (pid == 0) {
		// redirect stdin
		int in_fd, out_fd;
		if (in_path) {
			in_fd = open(in_path, O_RDONLY, 0666);
			if (in_fd == -1) {
				printf("Error: cannot open file \"%s\"\n", in_path);
				exit(EXIT_FAILURE);
			}
			dup2(in_fd, STDIN_FILENO);
		} else if (infd != STDIN_FILENO) {
			dup2(infd, STDIN_FILENO);
		}
		// redirect stdout
		if (out_path) {
			out_fd = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (out_fd == -1) {
				printf("Error: cannot open file \"%s\"\n", out_path);
				exit(EXIT_FAILURE);
			}
			dup2(out_fd, STDOUT_FILENO);
		} else if (outfd != STDOUT_FILENO) {
			dup2(outfd, STDOUT_FILENO);
		}
		// close the pipes in the child process
		// if not, the processes will hang
		close_fds(fd_pipes, n_pipes);
		// execute the command
		int status = execvp(args[0], args);
		if (status < 0) {
			exit(COMMAND_NOT_FOUND);
		}
		exit(EXIT_SUCCESS);
	}
	return pid;
}

int exec_command(char* args[], int infd, int outfd) {
	pid_t pid = exec_command_async(args, infd, outfd, 0, 0);
	int status;
	while (wait(&status) != pid)
		;
	if (WEXITSTATUS(status) == COMMAND_NOT_FOUND) {
		printf("Error: command \"%s\" not found\n", args[0]);
	}
	return status;
}

int create_fds(int fd_pipes[MAX_LENGTH / 2][2], int n_pipes) {
	for (int i = 0; i < n_pipes; ++i) {
		if (pipe(fd_pipes[i]) == -1) {
			printf("Error: cannot create pipe\n");
			return -1;
		}
	}
	return 0;
}

void close_fds(int fd_pipes[MAX_LENGTH / 2][2], int n_pipes) {
	for (int i = 0; i < n_pipes; ++i) {
		close(fd_pipes[i][0]);
		close(fd_pipes[i][1]);
	}
}

void exec_combined_commands(char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1],
	                        int n_tokens) {
	int fd_pipes[MAX_LENGTH / 2][2];
	char* args[MAX_LENGTH / 2 + 1];
	int command_count = 0;
	int n_pipes = 1;
	// count the number of pipe character '|'
	// to find how many pipes are needed
	for (int i = 0; i < n_tokens; ++i) {
		n_pipes += strcmp(tokens[i], "|") == 0;
	}
	// create those pipes
	create_fds(fd_pipes, n_pipes);
	// split commands by '|' character and run them one by one
	for (int i = 0, j = 0; i < n_tokens; ++i, j = 0) {
		while (i < n_tokens && strcmp(tokens[i], "|") != 0) {
			args[j] = tokens[i++];
			args[++j] = NULL;
		}
		if (command_count == 0 && i >= n_tokens) {
			// if it is the only command, then just run it directly
			exec_command(args, STDIN_FILENO, STDOUT_FILENO);
			return;
		}
		if (command_count == 0) {
			// first command reads from stdin
			exec_command_async(args, STDIN_FILENO, fd_pipes[command_count][1],
			                   fd_pipes, n_pipes);
		} else if (i >= n_tokens) {
			// last command prints to stdout
			exec_command_async(args, fd_pipes[command_count - 1][0],
			                   STDOUT_FILENO, fd_pipes, n_pipes);
		} else {
			exec_command_async(args, fd_pipes[command_count - 1][0],
			                   fd_pipes[command_count][1], fd_pipes, n_pipes);
		}
		++command_count;
	}
	close_fds(fd_pipes, n_pipes);
	while (wait(NULL) > 0)
		;
}

void handle_input(char* input) {
	char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1];
	// parse the input
	int n_tokens = parse(input, tokens);
	if (n_tokens == 0) {
		return;
	}
	// check exit
	if (strcmp(tokens[0], "exit") == 0) {
		exit(0);
	}
	// check run previous command
	if (strcmp(tokens[0], "!!") == 0) {
		char* last_executed = get_last_executed();
		if (last_executed == 0) {
			printf("No commands in history\n");
			return;
		}
		handle_input(last_executed);
		return;
	}
	// check if history
	if (strcmp(tokens[0], "history") == 0) {
		print_history();
		return;
	}
	// save to history
	add_to_history(tokens, n_tokens);
	// check if we need to wait for child process
	int async = strcmp(tokens[n_tokens - 1], "&") == 0;
	if (async) {
		tokens[n_tokens - 1][0] = 0;
		--n_tokens;
	}
	// execute a line in a child process
	pid_t pid = fork();
	if (pid < 0) {
		printf("Error: Unable to create child process\n");
	} else if (pid == 0) {
		exec_combined_commands(tokens, n_tokens);
		exit(EXIT_SUCCESS);
	} else {
		if (!async) {
			int status;
			while (wait(&status) != pid)
				;
		}
	}
}
