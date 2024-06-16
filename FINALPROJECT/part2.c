#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_TIME 5
#define MAX_TIME 15

long long findFileSize(const char *filename, int block_size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    long long fileSize = 0;
    double timeTaken = 0.0;

    clock_t start = clock();
    while (timeTaken < MIN_TIME) {
        char *buffer = (char *)malloc(block_size);
        if (buffer == NULL) {
            perror("malloc");
            close(fd);
            return -1;
        }

        ssize_t bytesRead = read(fd, buffer, block_size);
        if (bytesRead == -1) {
            perror("read");
            free(buffer);
            close(fd);
            return -1;
        }

        fileSize += bytesRead;
        timeTaken = (double)(clock() - start) / CLOCKS_PER_SEC;

        free(buffer);
    }

    while (timeTaken < MAX_TIME) {
        char *buffer = (char *)malloc(block_size);
        if (buffer == NULL) {
            perror("malloc");
            close(fd);
            return -1;
        }

        ssize_t bytesRead = read(fd, buffer, block_size);
        if (bytesRead == 0) {
            break;
        } else if (bytesRead == -1) {
            perror("read");
            free(buffer);
            close(fd);
            return -1;
        }

        fileSize += bytesRead;
        timeTaken = (double)(clock() - start) / CLOCKS_PER_SEC;

        free(buffer);
    }

    close(fd);

    printf("Time taken: %f seconds\n", timeTaken);

    return fileSize;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <block_size>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int block_size = atoi(argv[2]);

    long long fileSize = findFileSize(filename, block_size);
    if (fileSize == -1) {
        return 1;
    }

    printf("File size: %lld bytes\n", fileSize);

    return 0;
}
