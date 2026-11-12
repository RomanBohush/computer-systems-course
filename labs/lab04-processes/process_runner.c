#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <program>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Parent PID: %d\n", getpid());
    printf("Creating child...\n");
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        printf("Child PPID: %d\n", getppid());
        printf("Executing: %s\n\n", argv[1]);
        fflush(stdout);

        execlp(argv[1], argv[1], NULL);

        perror("execlp");
        _exit(EXIT_FAILURE);
    }

    int status;

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    printf("\nChild %d completed\n", pid);

    if (WIFEXITED(status)) {
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Child killed by signal: %d\n", WTERMSIG(status));
    }

    printf("Parent continues\n");

    return EXIT_SUCCESS;
}
