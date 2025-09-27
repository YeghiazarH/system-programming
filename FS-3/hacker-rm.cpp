#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Input error");
        exit(EXIT_FAILURE);
    }

    const char *path = argv[1];
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Error while opening file");
        exit(EXIT_FAILURE);
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == (off_t)-1) {
        perror("Error getting size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Error seeking file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    const size_t BUFSZ = 4096;
    char buf[BUFSZ];
    for (size_t i = 0; i < BUFSZ; ++i) buf[i] = '\0';

    off_t remaining = size;
    while (remaining > 0) {
        size_t to_write = (remaining > (off_t)BUFSZ) ? BUFSZ : (size_t)remaining;
        size_t written_total = 0;
        while (written_total < to_write) {
            ssize_t w = write(fd, buf + written_total, to_write - written_total);
            if (w == -1) {
                perror("Error writing zeros");
                close(fd);
                exit(EXIT_FAILURE);
            }
            written_total += (size_t)w;
        }
        remaining -= (off_t)to_write;
    }

    if (close(fd) == -1) {
        perror("Warning: close failed");
    }

    if (unlink(path) == -1) {
        perror("Error deleting file");
        exit(EXIT_FAILURE);
    }

    return 0;
}

