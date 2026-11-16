#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int source = open(argv[1], O_RDONLY);

    if (source == -1) {
        perror("open source");
        return EXIT_FAILURE;
    }

    int destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (destination == -1) {
        perror("open destination");
        close(source);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int status = EXIT_SUCCESS;

    while ((bytes_read = read(source, buffer, sizeof(buffer))) > 0) {
        ssize_t total_written = 0;

        while (total_written < bytes_read) {
            ssize_t written = write(destination,
                                    buffer + total_written,
                                    (size_t)(bytes_read - total_written));

            if (written == -1) {
                perror("write");
                status = EXIT_FAILURE;
                goto cleanup;
            }

            total_written += written;
        }
    }

    if (bytes_read == -1) {
        perror("read");
        status = EXIT_FAILURE;
    }

cleanup:
    if (close(source) == -1) {
        perror("close source");
        status = EXIT_FAILURE;
    }

    if (close(destination) == -1) {
        perror("close destination");
        status = EXIT_FAILURE;
    }

    return status;
}
