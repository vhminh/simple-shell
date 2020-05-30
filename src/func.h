#ifndef _FUNC_H_
#define _FUNC_H_

#include <sys/types.h>

#define MAX_LENGTH 80  // The maximum length of a command
#define COMMAND_NOT_FOUND 6

///      History feature. Implement in history.c      ///
#define HISTORY_SIZE 10  // History store 10 most recent commands
/**
 * @brief Print out command history to stdout
 *
 * Print out the history
 * from newest to oldest command
 * It does not include "history" and "exit" command
 * The command !! is replaced with the actual command being run
 *
 * Example:
 *     If we execute "make", "history", "cat", "!!", "gcc --version", "history"
 *     Then the last history command will output:
 *         1 gcc --versio of the commandn
 *         2 cat
 *         3 cat
 *         4 make
 */
void print_history();

/**
 * @brief Add a command into history
 *
 * Add a command into history
 * A command should be passed in the function as an array of tokens
 * To parse an array represented as string into tokens, see function parse
 *
 * @param tokens An array of token
 * @param n_tokens Number of token
 */
void add_to_history(char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1],
                    int n_tokens);
/**
 * @brief Get last executed command
 *
 * Get last executed command
 *
 * @return A char pointer points to the last executed command,
 * or NULL if there is no commands
 */
char* get_last_executed();

///      Parse command. Implement in parse.c      ///
/**
 * @brief Parse input command into tokens
 *
 * Parse input command into tokens using '|' and whitespace as delimiter
 * The character '|' is kept as a token in the result array
 *
 * @param input The input string
 * @param tokens Output array of tokens
 * @return Number of tokens
 *
 * Example:
 *     If @p input = "  echo helloworld | grep  word|cat| exit";
 *     parse(input, tokens) will return 9
 *     After execution @p tokens is
 *         ["echo", "helloworld", "|", "grep", "word", "|", "cat", "|", "exit"]
 */
int parse(char* input, char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1]);

///      Functions to execute command. Implement in execute.c      ///
/**
 * @brief Create file descriptors for pipes
 * 
 * Create multiple pipes and return their file descriptors
 * 
 * @param fd_pipes Output array of file descriptors
 * @param n_pipes Number of pipes
 * @return -1 if not successful, 0 otherwise
 */
int create_fds(int fd_pipes[MAX_LENGTH / 2][2], int n_pipes);

/**
 * @brief Close file descriptors of pipes
 * 
 * Close file descriptors of pipes
 * 
 * @param fd_pipes Array of file descriptor for every pipes
 * @param n_pipes Number of pipes
 */
void close_fds(int fd_pipes[MAX_LENGTH / 2][2], int n_pipes);

/**
 * @brief Execute command asynchronously
 * 
 * Execute command in a new child process
 * and return immediately without waiting for the child process to finish
 * The command should redirect input and output base on the given file descriptor
 * Command must not contain '&', '|', '<', '>'
 * 
 * @param args Arguments of the command
 * @param infd File descriptor of the input stream
 * @param outfd File descriptor of the output stream
 * @param fd_pipes Unused file descriptor of the pipes need to be closed
 * @param n_pipes Number of pipes need to close
 */
pid_t exec_command_async(char* args[], int infd, int outfd,
                         int fd_pipes[MAX_LENGTH / 2][2], int n_pipes);

/**
 * @brief Execute command synchronously
 * 
 * Execute command in a new child process
 * and wait for it to finish
 * The command should redirect input and output base on the given file descriptor
 * Command must not contain '&', '|', '<', '>'
 * 
 * @param args Arguments of the command
 * @param infd File descriptor of the input stream
 * @param outfd File descriptor of the output stream
 */
int exec_command(char* args[], int infd, int outfd);

/**
 * @brief Execute combined command
 * 
 * Execute combined command
 * Combined command must be parsed into tokens
 * Commands are separated with pipe '|' character
 * Command does not contain "history", "exit", or "!!"
 * Does not support asynchronous command with character '&'
 * 
 * For example:
 *     ["ls", "-l"]
 *     ["ls", "-l" , "|", "grep", "-i", "make"]
 *     ["ls", "-l", ">", "out.txt"]
 * 
 * @param tokens The input command parsed into tokens
 * @param n_tokens Number of tokens
 */
void exec_combined_commands(char tokens[MAX_LENGTH / 2 + 1][MAX_LENGTH / 2 + 1],
                            int n_tokens);

/**
 * @brief Handle input
 * 
 * Handle input
 * Expect an input command and execute it
 * The input command may contain '&', IO redirection, and pipes
 * Example:
 *     input = "  gcc    --version | grep  -i free |tr a-zA-Z A-Za-z|cat > out.txt  "
 * 
 * @param input The input command
 */
void handle_input(char* input);

#endif
