#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    char *line = NULL;
    char **argz;
    pid_t pid, wpid;
    size_t bufsize = 0;
    int status;

    while(1) {
        // Step 1: Print prompt
        printf("$ ");
        fflush(stdout);  // Ensure prompt displays immediately

        // Step 2: Read a line from stdin using getline
        if (getline(&line, &bufsize, stdin) == -1) {
            perror("getline");
            break;
        }

        // Step 3: Remove the trailing newline
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        // Handle empty input
        if (strlen(line) == 0) {
            continue;
        }

        // Step 4: Fork a child process
        pid = fork();

        if (pid == 0) {
            // Child process
            // Step 5: Create argument vector for execv
            argz = malloc(2 * sizeof(char*));
            if (argz == NULL) {
                perror("malloc");
                exit(1);
            }
            argz[0] = line;
            argz[1] = NULL;

            // Step 6: Execute the command
            if (execv(line, argz) == -1) {
                perror("nsh");
            }
            // If execv succeeds, this code never executes
            exit(1);

        } else if (pid > 0) {
            // Parent process
            // Wait for child to complete
            wpid = waitpid(pid, &status, 0);
            if (wpid == -1) {
                perror("waitpid");
            }
            // Note: argz was allocated in child process and freed when child exits

        } else {
            // fork() failed
            perror("fork");
        }
    }

    // Clean up before exit
    free(line);
    return 0;
}
