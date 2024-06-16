#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

static unsigned int xorResult = 0;

double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

long getFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error getting file size");
        exit(EXIT_FAILURE);
    }
    return st.st_size;
}

unsigned int xorBuffer(unsigned int *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        xorResult ^= buffer[i];
    }
    return xorResult;
}

void readMode(const char *filename, long blockSize, long blockCount) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    long fileSize = getFileSize(filename);
    if (blockSize * blockCount > fileSize) {
        fprintf(stderr, "Error: Block size * Block count > File size\n");
        exit(EXIT_FAILURE);
    }

    double start = getCurrentTime();
    unsigned int *buf = (unsigned int *)malloc(blockSize);
    size_t totalBytesRead = 0;

    for (long i = 0; i < blockCount; i++) {
        ssize_t bytesRead = read(fd, buf, blockSize);
        if (bytesRead == -1) {
            perror("Error reading from file");
            exit(EXIT_FAILURE);
        }
        totalBytesRead += bytesRead;
        xorBuffer(buf, bytesRead / sizeof(unsigned int));
    }

    double end = getCurrentTime();
    free(buf);

    double runtime = end - start;
    double fileSizeRead = (double)totalBytesRead / (1024 * 1024);
    double performance = fileSizeRead / runtime;

    printf("\nXOR: %08x\n", xorResult);
    printf("Time Taken: %.2fs\nPerformance: %.2f MB/s\n\n", runtime, performance);

    close(fd);
}

void writeMode(const char *filename, long blockSize, long blockCount) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    double start = getCurrentTime();
    char *buf = (char *)malloc(blockSize);

    for (long i = 0; i < blockCount; i++) {
        for (long j = 0; j < blockSize; j++) {
            buf[j] = 'A' + rand() % 26;
        }
        if (write(fd, buf, blockSize) == -1) {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }
    }

    free(buf);
    close(fd);

    double end = getCurrentTime();
    double runtime = end - start;
    printf("\nTime Taken: %.2fs\nPerformance: %.2f MB/s\n\n", runtime, (blockCount * blockSize) / (1024 * 1024 * runtime));
}

int main(int argc, const char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <filename> [-r|-w] <block_size> <block_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char mode = argv[2][1];
    long blockSize = atol(argv[3]);
    long blockCount = atol(argv[4]);

    if (blockSize <= 0 || blockCount <= 0) {
        fprintf(stderr, "Error: Block size and block count must be greater than zero\n");
        exit(EXIT_FAILURE);
    }

    if (mode == 'r') {
        readMode(argv[1], blockSize, blockCount);
    } else if (mode == 'w') {
        writeMode(argv[1], blockSize, blockCount);
    } else {
        fprintf(stderr, "Error: Invalid input for r/w; valid values are -r for read and -w for write\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
