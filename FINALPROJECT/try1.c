#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

// Function to read a file with specified block size
void readFile(const char* filename, int block_size, int block_count) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    return;
  }

  // Create buffer to hold data
  char buffer[block_size];

  // Read file data in blocks
  for (int i = 0; i < block_count; ++i) {
    int bytes_read = read(fd, buffer, block_size);
    if (bytes_read == -1) {
      perror("Error reading file");
      break;
    }

    // Process data here, e.g., calculate checksum or store in a data structure
  }

  close(fd);
}

// Function to write a file with specified block size
void writeFile(const char* filename, int block_size, int block_count) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("Error opening file");
    return;
  }

  // Create buffer to generate data
  char buffer[block_size];

  // Fill the buffer with random data
  for (int i = 0; i < block_size; ++i) {
    buffer[i] = rand() % 256;
  }

  // Write data to file in blocks
  for (int i = 0; i < block_count; ++i) {
    int bytes_written = write(fd, buffer, block_size);
    if (bytes_written == -1) {
      perror("Error writing file");
      break;
    }
  }

  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    printf("Usage: ./run <filename> [-r|-w] <block_size> <block_count>\n");
    return 1;
  }

  char* filename = argv[1];
  char* mode = argv[2];
  int block_size = atoi(argv[3]);
  int block_count = atoi(argv[4]);

  if (strcmp(mode, "-r") == 0) {
    readFile(filename, block_size, block_count);
  } else if (strcmp(mode, "-w") == 0) {
    writeFile(filename, block_size, block_count);
  } else {
    printf("Invalid mode: %s\n", mode);
    return 1;
  }

  return 0;
}
