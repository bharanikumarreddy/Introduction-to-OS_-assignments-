#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

static unsigned int xorResult = 0;

double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int xorBuffer(unsigned int *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        xorResult ^= buffer[i];
    }
    return xorResult;
}

long getFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error getting file size");
        exit(EXIT_FAILURE);
    }
    return st.st_size;
}

int main(int argc, const char *argv[]) {
    double startT, currT, endT;
    long blockSize;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <blockSize>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((blockSize = atol(argv[2])) <= 0) {
        fprintf(stderr, "Must enter a valid block size\n");
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    long fileSize = getFileSize(argv[1]);

    startT = getCurrentTime();
    currT = getCurrentTime();
    unsigned int *buf = (unsigned int *)malloc(blockSize);
    size_t totalBytesRead = 0;
    long blockCount = 0;

    while (read(fd, buf, blockSize) > 0) {
        xorBuffer(buf, blockSize / sizeof(unsigned int));
        currT = getCurrentTime();
        totalBytesRead += blockSize;
        blockCount++;
    }

    endT = getCurrentTime();
    free(buf);

    double runtime = endT - startT;
    double fileSizeRead = (double)totalBytesRead / (1024 * 1024);
    

    printf("\nXOR: %08x\n", xorResult);
    printf("Time Taken: %.2fs\nBlock Count: %ld\nFile Size: %.2f MB\n\n", runtime, blockCount, fileSizeRead);

    close(fd);
    return 0;
}
