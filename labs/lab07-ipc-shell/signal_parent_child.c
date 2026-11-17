#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t got_term = 0;

static void handle_sigterm(int signal_number)
{
    (void)signal_number;
    got_term = 1;
}

int main(int argc, char *argv[])
{
    int use_handler = (argc == 2 && strcmp(argv[1], "handler") == 0);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        if (use_handler) {
            struct sigaction action = {0};

            action.sa_handler = handle_sigterm;
            sigemptyset(&action.sa_mask);

            if (sigaction(SIGTERM, &action, NULL) == -1) {
                _exit(EXIT_FAILURE);
            }
        }

        while (!got_term) {
            pause();
        }

        printf("CHILD: SIGTERM caught, exiting normally\n");
        fflush(stdout);
        _exit(7);
    }

    sleep(2);

    printf("PARENT: sending SIGTERM to %d (mode: %s)\n",
           pid, use_handler ? "handler" : "default");
    fflush(stdout);

    kill(pid, SIGTERM);

    int status;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (WIFSIGNALED(status)) {
        printf("PARENT: child killed by signal %d\n", WTERMSIG(status));
    } else if (WIFEXITED(status)) {
        printf("PARENT: child exited with status %d\n", WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}
