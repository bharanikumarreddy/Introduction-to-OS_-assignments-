#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

static unsigned int fxor = 0;

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

unsigned int xorbuf(unsigned int *buffer, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    fxor ^= buffer[i];
  }
  return fxor;
}

int main(int argc, const char *argv[])
{
  int fd, modeFlags;
  long blockSize, blockCount, blocksRead = 0;
  int fileSizeRequested = blockSize * blockCount;
  double startT, endT;

  if (argc != 5)
  {
    printf("usage: ./run <filename> [-r|-w] <block_size> <block_count>\n");
    exit(1);
  }

  char mode = argv[2][1];
  if (mode != 'r' && mode != 'w')
  {
    printf("invalid input for r/w; valid values are -r for read and -w for write\n");
    exit(1);
  }

  if ((blockSize = atol(argv[3])) <= 0)
  {
    printf("must enter a valid block size\n");
    exit(1);
  }

  if ((blockCount = atol(argv[4])) <= 0)
  {
    printf("must enter a valid block count\n");
    exit(1);
  }

  if (mode == 'r')
  {
    modeFlags = O_RDONLY;
    int fd = open(argv[1], modeFlags);
    if (fd == -1)
    {
      printf("File Not Found.\r\n");
      exit(1);
    }

    long actualFileSize = filesize(argv[1]);
    if (fileSizeRequested > actualFileSize)
    {
      printf("Input block size * block count > input file size.\r\n");
      exit(1);
    }

    startT = now();
    unsigned int *buf = (unsigned int *)malloc(blockSize);
    size_t bytes_read;
    size_t total_bytes_read = 0;
    while (blocksRead < blockCount && (bytes_read = read(fd, buf, blockSize)) > 0)
    {
      xorbuf(buf, bytes_read / sizeof(unsigned int));
      total_bytes_read += bytes_read;
      blocksRead++;
    }
    endT = now();

    free(buf);
    double runtime = endT - startT;
    double fileSizeRead = ((double)total_bytes_read / (1024 * 1024));
    double performance = fileSizeRead / runtime;

    printf("\nXOR: %08x\n", fxor);
    printf("Performance: %.2f MB/s \n\n", performance);
    close(fd);
  }
  else if (mode == 'w')
  {
    char *buf;
    startT = now();
    modeFlags = O_WRONLY | O_CREAT | O_TRUNC;
    fd = open(argv[1], modeFlags);
    for (long i = 0; i < blockCount; i++)
    {
      buf = (char *)malloc(blockSize);
      for (long j = 0; j < blockSize; j++)
      {
        buf[j] = 'A' + (random() % 26);
        // printf("%s",&buf[j]);
      }
      // printf("\n");
      write(fd, buf, blockSize);
    }
    close(fd);
    endT = now();
    double runtime = endT - startT;
    // printf("wrote %f MB in %f \n", (double)(blockCount * blockSize) / (double)(1024 * 1024), t1);
    printf("\nPerformance: %.2f MB/s\n\n", (blockCount * blockSize) / (1024 * 1024 * runtime));
  }

  return 0;
}