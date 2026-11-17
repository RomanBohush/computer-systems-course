#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 64

/* Знаходить ">" в argv, прибирає його й запам'ятовує ім'я файлу. Повертає -1 при синтаксичній помилці. */
static int split_redirect(char **argv, char **outfile)
{
    *outfile = NULL;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], ">") == 0) {
            if (argv[i + 1] == NULL) {
                fprintf(stderr, "myshell: syntax error: missing file after >\n");
                return -1;
            }

            *outfile = argv[i + 1];
            argv[i] = NULL;
            break;
        }
    }

    if (argv[0] == NULL) {
        fprintf(stderr, "myshell: syntax error: empty command\n");
        return -1;
    }

    return 0;
}

/* Запускає команду в child. in_fd/out_fd >= 0 означає підключити до stdin/stdout.
   close_a/close_b: кінці pipe, які child має закрити. */
static pid_t spawn(char **argv, int in_fd, int out_fd,
                   int close_a, int close_b, const char *outfile)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        signal(SIGINT, SIG_DFL);

        if (in_fd >= 0 && dup2(in_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        if (out_fd >= 0 && dup2(out_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        if (close_a >= 0) {
            close(close_a);
        }

        if (close_b >= 0) {
            close(close_b);
        }

        if (outfile != NULL) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd == -1) {
                perror("open");
                _exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                _exit(EXIT_FAILURE);
            }

            close(fd);
        }

        execvp(argv[0], argv);

        perror("execvp");
        _exit(EXIT_FAILURE);
    }

    return pid;
}

static void wait_for(pid_t pid)
{
    if (pid > 0 && waitpid(pid, NULL, 0) == -1) {
        perror("waitpid");
    }
}

int main(void)
{
    char line[1024];

    signal(SIGINT, SIG_IGN);

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        char *tokens[MAX_ARGS];
        int n = 0;
        char *tok = strtok(line, " \t");

        while (tok != NULL && n < MAX_ARGS - 1) {
            tokens[n++] = tok;
            tok = strtok(NULL, " \t");
        }

        tokens[n] = NULL;

        if (n == 0) {
            continue;
        }

        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        if (strcmp(tokens[0], "cd") == 0) {
            const char *dir = tokens[1] != NULL ? tokens[1] : getenv("HOME");

            if (dir == NULL || chdir(dir) == -1) {
                perror("cd");
            }

            continue;
        }

        int pipe_index = -1;

        for (int i = 0; i < n; i++) {
            if (strcmp(tokens[i], "|") == 0) {
                pipe_index = i;
                break;
            }
        }

        if (pipe_index == -1) {
            char *outfile;

            if (split_redirect(tokens, &outfile) == -1) {
                continue;
            }

            pid_t pid = spawn(tokens, -1, -1, -1, -1, outfile);

            wait_for(pid);
            continue;
        }

        tokens[pipe_index] = NULL;

        char **left = tokens;
        char **right = tokens + pipe_index + 1;
        char *left_out;
        char *right_out;

        if (split_redirect(left, &left_out) == -1 ||
            split_redirect(right, &right_out) == -1) {
            continue;
        }

        int pipefd[2];

        if (pipe(pipefd) == -1) {
            perror("pipe");
            continue;
        }

        pid_t child1 = spawn(left, -1, pipefd[1], pipefd[0], pipefd[1], left_out);
        pid_t child2 = spawn(right, pipefd[0], -1, pipefd[0], pipefd[1], right_out);

        close(pipefd[0]);
        close(pipefd[1]);

        wait_for(child1);
        wait_for(child2);
    }

    return EXIT_SUCCESS;
}
