#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>

typedef struct
{
    char *filename;
    long startOffset;
    long endOffset;
    long blockSize;
    unsigned int threadXor;
} thread_data;

double now()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

long filesize(const char *filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

void *thread_read(void *arg)
{
    thread_data *data = (thread_data *)arg;
    char *filename = data->filename;
    long startOffset = data->startOffset;
    long endOffset = data->endOffset;
    long blockSize = data->blockSize;
    unsigned int threadXOR = 0;
    unsigned int *buf = (unsigned int *)malloc(blockSize);

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("File Not Found.\r\n");
        pthread_exit(NULL);
    }
    lseek(fd, startOffset, SEEK_SET);
    for (long i = 0; i < endOffset; i += blockSize)
    {
        size_t bytes_read = read(fd, buf, blockSize);
        for (size_t i = 0; i < bytes_read / sizeof(unsigned int); i++)
        {
            threadXOR ^= buf[i];
        }
    }

    data->threadXor = threadXOR;

    free(buf);
    close(fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];
    long fsize = filesize(argv[1]);

    int numThreads = 2;
    long blockSize = (long)(6 * pow(fsize, 0.45));
    blockSize = blockSize / numThreads;
    blockSize = blockSize - blockSize % 4;

    long threadFileBlock = (long)fsize / numThreads;
    threadFileBlock = threadFileBlock - threadFileBlock % blockSize;

    pthread_t threads[numThreads];

    thread_data data[numThreads];

    double startT = now();
    for (int i = 0; i < numThreads; i++)
    {
        data[i].filename = filename;
        data[i].startOffset = (i * threadFileBlock);
        if (i == 0)
        {
            data[i].startOffset = 0;
        }
        data[i].endOffset = data[i].startOffset + threadFileBlock;
        if (i == numThreads - 1)
        {
            data[i].endOffset = fsize;
        }
        data[i].blockSize = blockSize;
        data[i].threadXor = 0;
        pthread_create(&threads[i], NULL, thread_read, &data[i]);
    }

    // wait for all threads to complete
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    double endT = now();

    double runtime = endT - startT;

    unsigned int fxor = 0;
    for (int i = 0; i < numThreads; i++)
    {
        fxor ^= data[i].threadXor;
    }
    printf("\nXOR: %08x\n", fxor);
    printf("BlockSize: %ld\nTime Taken: %.6f seconds\n\n", blockSize, runtime);

    return 0;
}
