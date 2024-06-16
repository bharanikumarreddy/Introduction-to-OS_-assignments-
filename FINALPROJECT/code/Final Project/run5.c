#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

int block_size = 1;

double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    int fd1, fdo;
    char temp_buf[10000];
    double start, end;

    if (argc <= 1) {
        printf("Usual Syntax to run syscall_readfile: ./syscall_readfile <filename>\n");
        return -1;
    }

    char *fname = argv[1];

    fd1 = open(fname, O_RDWR, 0);
    if (fd1 < 0) {
        printf("Unable to open the file\n");
        exit(EXIT_FAILURE);
    }

    // Time calculation for read operation
    printf("--------------- Read Func ------------------------------------------------------------\n");
    for (int fs = 1; fs < 4194305; fs = fs * 4) {
        start = getCurrentTime();
        for (int y = 0; y < fs; y++) {
            read(fd1, temp_buf, block_size);
        }
        end = getCurrentTime();
        double time = end - start;
        double perf_B = fs / time;
        double perf_M = perf_B / (1024 * 1024);
        printf("File size: %d\n", fs);
        printf("Speed in MiB/s: %0.2f \n", perf_M);
        printf("Speed in B/s: %0.2f \n", perf_B);
        printf("Total Time taken: %f \n", time);
        printf("----------------------------------------------\n");
    }

    // Time calculation for lseek operation
    printf("\n--------------- LSEEK Func ----------------------------------------------------------\n");
    for (int fs = 1; fs < 4194305; fs = fs * 4) {
        start = getCurrentTime();
        for (int y = 0; y < fs; y++) {
            lseek(fd1, block_size, SEEK_CUR);
        }
        end = getCurrentTime();
        double time = end - start;
        double perf_B = fs / time;
        double perf_M = perf_B / (1024 * 1024);
        printf("File size: %d\n", fs);
        printf("Speed in MiB/s: %0.2f \n", perf_M);
        printf("Speed in B/s: %0.2f \n", perf_B);
        printf("Total Time taken: %f \n", time);
        printf("----------------------------------------------\n");
    }

    // Time calculation for open operation
    printf("\n--------------- Open Func ----------------------------------------------------------------\n");
    for (int fs = 1; fs < 4194305; fs = fs * 4) {
        start = getCurrentTime();
        for (int y = 0; y < fs; y++) {
            fdo = open(argv[1], O_RDWR);
            if (fdo < 0) {
                perror("Unable to open the file");
                exit(EXIT_FAILURE);
            }
            close(fdo);
        }
        end = getCurrentTime();
        double time = end - start;
        double perf_B = fs / time;
        double perf_M = perf_B / (1024 * 1024);
        printf("File size: %d\n", fs);
        printf("Speed in MiB/s: %0.2f \n", perf_M);
        printf("Speed in B/s: %0.2f \n", perf_B);
        printf("Total Time taken: %f \n", time);
        printf("----------------------------------------------\n");
    }

    close(fd1);

    return 0;
}
