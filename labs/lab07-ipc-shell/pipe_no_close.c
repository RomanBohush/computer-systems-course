#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Без -DFIXED: write end лишається відкритим (баг). З -DFIXED: усе закрито правильно. */
int main(void)
{
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
#ifdef FIXED
        close(pipefd[1]);
#endif
        char buffer[128];
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            printf("Child got %zd bytes\n", n);
            fflush(stdout);
        }

        printf("Child: EOF reached\n");
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);

    const char message[] = "data\n";

    write(pipefd[1], message, strlen(message));
#ifdef FIXED
    close(pipefd[1]);
#endif

    waitpid(pid, NULL, 0);
    printf("Parent: child finished\n");

    return EXIT_SUCCESS;
}
