#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_TIME 5
#define MAX_TIME 15

long long findFileSize(const char *filename, int block_size, double *timeTaken) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    long long fileSize = 0;
    double elapsed_time = 0.0;

    clock_t start = clock();
    while (elapsed_time < MIN_TIME) {
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
        elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;

        free(buffer);
    }

    while (elapsed_time < MAX_TIME) {
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
        elapsed_time = (double)(clock() - start) / CLOCKS_PER_SEC;

        free(buffer);
    }

    close(fd);

    if (timeTaken != NULL) {
        *timeTaken = elapsed_time;
    }

    return fileSize;
}

void printPerformance(long long fileSize, double elapsed_time, int block_size) {
    double performance = (double)fileSize / (1024 * 1024) / elapsed_time; // Convert bytes to MiB
    printf("Performance: %.2f MiB/s (Block Size: %d bytes)\n", performance, block_size);
    double performanceBps = (double)fileSize / elapsed_time; // Performance in B/s
    printf("Performance: %.2f B/s (Block Size: %d bytes)\n", performanceBps, block_size);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <block_size>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int block_size = atoi(argv[2]);

    double elapsed_time;
    long long fileSize = findFileSize(filename, block_size, &elapsed_time);
    if (fileSize == -1) {
        return 1;
    }

    printPerformance(fileSize, elapsed_time, block_size);

    return 0;
}
