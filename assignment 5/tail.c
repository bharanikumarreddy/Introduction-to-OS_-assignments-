#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int i;

void displayTail(int fileDescriptor, char *fileName, int linesToDisplay) {
    int bytesRead, totalLines = 0, lineCount = 0;

    int temporaryFileDescriptor = open("tempfile", O_CREATE | O_RDWR);

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        write(temporaryFileDescriptor, buffer, bytesRead);

        for ( i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                totalLines++;
            }
        }
    }

    close(temporaryFileDescriptor);

    if (bytesRead < 0) {
        printf(1, "tail: error reading file\n");
        exit();
    }

    temporaryFileDescriptor = open("tempfile", 0);

    while ((bytesRead = read(temporaryFileDescriptor, buffer, sizeof(buffer))) > 0) {
        for (i = 0; i < bytesRead; i++) {
            if (lineCount >= (totalLines - linesToDisplay) || totalLines < linesToDisplay) {
                printf(1, "%c", buffer[i]);
            } else if (buffer[i] == '\n') {
                lineCount++;
            }
        }
    }

    close(temporaryFileDescriptor);
    unlink("tempfile");
}

int main(int argc, char *argv[]) {
    int fileDescriptor = 0; // Default: read from standard input
    int linesToPrint = 10;  // Default: print last 10 lines
    char *fileName = "";

    if (argc <= 1) {
        displayTail(0, "", 10); // Default behavior: read from user input and print last 10 lines
        exit();
    } else {
        for (i = 1; i < argc; i++) {
            char currentChar = *argv[i];

            if (currentChar == '-') {
                argv[i]++;
                linesToPrint = atoi(argv[i]++);
            } else {
                if ((fileDescriptor = open(argv[i], 0)) < 0) {
                    printf(1, "tail: cannot open %s\n", argv[i]);
                    exit();
                }
            }
        }

        displayTail(fileDescriptor, fileName, linesToPrint);
        close(fileDescriptor);
        exit();
    }
}
