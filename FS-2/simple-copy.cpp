#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong input");
        exit(EXIT_FAILURE);
    }

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        perror("opening source error");
        exit(EXIT_FAILURE);
    }

    int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("Error while  opening/creating destination file");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(fd_in, buffer, sizeof(buffer))) > 0) {
        if (write(fd_out, buffer, bytes) != bytes) {
            perror("Error while writing to file");
            close(fd_in);
            close(fd_out);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes == -1) {
        perror("Error reading file");
    }

    close(fd_in);
    close(fd_out);
    return 0;
}

