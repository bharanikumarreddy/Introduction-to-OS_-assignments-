#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void performReadWrite(const char *filename, char mode, int block_size, int block_count) {
    int fd;

    // Open file based on read or write mode
    if (mode == 'r') {
        fd = open(filename, O_RDONLY);
    } else if (mode == 'w') {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    } else {
        fprintf(stderr, "Invalid mode: %c\n", mode);
        exit(EXIT_FAILURE);
    }

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Check if reading and the file has enough bytes
    if (mode == 'r') {
        off_t fileSize = lseek(fd, 0, SEEK_END);
        if (fileSize < (off_t)block_size * block_count) {
            fprintf(stderr, "Error: File size is not sufficient for reading\n");
            exit(EXIT_FAILURE);
        }
        // Reset the file pointer to the beginning
        lseek(fd, 0, SEEK_SET);
    }

    // Allocate buffer
    char *buffer = malloc(block_size * sizeof(char));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Read or write the specified number of blocks
    for (int i = 0; i < block_count; ++i) {
        if (mode == 'r') {
            int bytesRead = read(fd, buffer, block_size);
            if (bytesRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            // Process the read data if needed
        } else if (mode == 'w') {
            // Fill the buffer with some data for writing (you can modify this)
            for (int j = 0; j < block_size; ++j) {
                buffer[j] = 'A' + (i % 26);  // Writing 'A' to 'Z' in a loop
            }
            int bytesWritten = write(fd, buffer, block_size);
            if (bytesWritten == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Clean up
    free(buffer);

    // Close the file
    close(fd);
}

int main(int argc, char *argv[]) {
    // Check command-line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <filename> [-r|-w] <block_size> <block_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract parameters
    const char *filename = argv[1];
    char mode = argv[2][1];  // Extracting the mode character
    int block_size = atoi(argv[3]);
    int block_count = atoi(argv[4]);

    // Perform read or write operations
    performReadWrite(filename, mode, block_size, block_count);

    return 0;
}
