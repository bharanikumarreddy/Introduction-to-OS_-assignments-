#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>

static unsigned int fxor = 0;

double now()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int xorbuf(unsigned int *buffer, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    fxor ^= buffer[i];
  }
  return fxor;
}

long filesize(const char *filename)
{
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

int main(int argc, const char *argv[])
{
  double startT, currT, endT, resonableTime = 10;
  long blockSize;
  if (argc != 3)
  {
    printf("Usage: ./run2 <filename> <blockSize>\n");
    exit(1);
  }
  if ((blockSize = atol(argv[2])) <= 0)
  {
    printf("must enter a valid block size\n");
    exit(1);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1)
  {
    printf("File Not Found.\r\n");
    exit(1);
  }
  long fsize = filesize(argv[1]);

  startT = now();
  currT = now();
  unsigned int *buf = (unsigned int *)malloc(blockSize);
  size_t bytes_read;
  size_t total_bytes_read=0;
  long blockCount=0;
  while (currT-startT<resonableTime && (bytes_read = read(fd, buf, blockSize)) > 0)
  {
    xorbuf(buf, bytes_read / sizeof(unsigned int));
    currT=now();
    total_bytes_read+=bytes_read;
    blockCount++;
  }
  endT = now();

  free(buf);
  double runtime = endT - startT;
  double fileSizeRead=((double)total_bytes_read/(1024*1024));
  double performance = fileSizeRead / runtime;

  printf("\nXOR: %08x\n", fxor);
  printf("TimeTaken: %.2fs\nBlockCount: %ld\nFileSize: %.2f MB\nPerformance: %.2f MB/s \n\n", runtime, blockCount ,fileSizeRead, performance);
  close(fd);
  return 0;
}