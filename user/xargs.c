#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LEN 120
#define MAXAGR 120

void copy_argument(char **dest, const char *src) {
    *dest = malloc(strlen(src) + 1);
    if (*dest == 0) {
        fprintf(2, "Error: Memory allocation failed\n");
        exit(1);
    }
    strcpy(*dest, src);
}

int parse_line(char *paramv[], int start_index) {
    char buf[MAX_LEN];
    int buf_index = 0;

    // Read input until a newline or EOF
    while (read(0, &buf[buf_index], 1) > 0) {
        if (buf[buf_index] == '\n') {
            buf[buf_index] = '\0';
            break;
        }
        buf_index++;
        if (buf_index >= MAX_LEN) {
            fprintf(2, "Error: Argument too long\n");
            exit(1);
        }
    }

    // Check if we reached EOF with no input
    if (buf_index == 0) {
        return -1; // Signal EOF or no input
    }

    // Parse the line into arguments
    int param_index = start_index;
    int token_start = 0;

    while (token_start < buf_index) {
        // Skip leading spaces
        while (buf[token_start] == ' ' && token_start < buf_index) {
            token_start++;
        }

        // Find the end of the token
        int token_end = token_start;
        while (token_end < buf_index && buf[token_end] != ' ') {
            token_end++;
        }

        if (token_start < token_end) { // Valid token found
            if (param_index >= MAXAGR - 1) {
                fprintf(2, "Error: Too many arguments\n");
                exit(1);
            }
            buf[token_end] = '\0'; // Null-terminate the token
            copy_argument(&paramv[param_index], &buf[token_start]);
            param_index++;
        }

        token_start = token_end + 1; // Move to the next token
    }

    return param_index;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "[Usage] xargs <command_name> <arg1> ...\n");
        exit(1);
    }

    char *paramv[MAXAGR];
    for (int i = 1; i < argc; i++) {
        copy_argument(&paramv[i - 1], argv[i]);
    }

    int param_start = argc - 1; // Start filling arguments after provided ones
    int param_end;

    while ((param_end = parse_line(paramv, param_start)) != -1) {
        paramv[param_end] = 0; // Null-terminate the argument list

        if (fork() == 0) {
            exec(paramv[0], paramv);
            fprintf(2, "Error: exec failed\n");
            exit(1);
        } else {
            wait(0); // Wait for the child process to finish
        }

        // Free dynamically allocated arguments after execution
        for (int i = param_start; i < param_end; i++) {
            free(paramv[i]);
        }
    }

    // Free initial arguments
    for (int i = 0; i < param_start; i++) {
        free(paramv[i]);
    }

    exit(0);
}
