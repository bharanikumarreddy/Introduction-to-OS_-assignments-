#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char inputBuf[1000000];
char *inputArgs;

int strCmp(const char *str1, const char *str2) {
    int i;
    for (i = 0; str1[i] && str2[i]; ++i) {
        char char1 = str1[i];
        char char2 = str2[i];
        if (char1 >= 'A' && char1 <= 'Z') {
            char1 += 'a' - 'A'; // Convert to lowercase
        }
        if (char2 >= 'A' && char2 <= 'Z') {
            char2 += 'a' - 'A'; // Convert to lowercase
        }
        if (char1 != char2) {
            return char1 - char2;
        }
    }
    return str1[i] - str2[i];
}

int finalCount = 0;
int k;
int x;
int y;
int z;
int currentIndex = 0;
int m = 0;
int lineCount = 0;
int b;
int bytesRead;
int i;
int charCount = 0;
int duplicateCount = 0;

int uniqueLines(int fileDescriptor, char *filename, int countLines, int printDuplicates, int ignoreCase) {
    char currentLine[10000];
    char *outputLines[10000];
    int lineRepeats[1000];

    while ((bytesRead = read(fileDescriptor, inputBuf, sizeof(inputBuf))) > 0) {
        for (b = 0; b < bytesRead; b++) {
            if (inputBuf[b] == '\n')
                lineCount++;
        }
        for (i = 0; inputBuf[i] != '\n'; i++) {
            charCount++;
            currentLine[i] = inputBuf[i];
        }

        currentLine[i] = '\0';
        lineRepeats[0] = 1;
        outputLines[0] = (char *)malloc((charCount + 1) * sizeof(char *));
        for (i = 0; i < charCount + 1; i++) {
            outputLines[currentIndex][i] = currentLine[i];
        }
        outputLines[currentIndex][i] = '\0';
        k = i;

        while (finalCount < lineCount - 1) {
            finalCount++;
            duplicateCount = 0;
            for (i = k; inputBuf[i] != '\n'; i++) {
                duplicateCount++;
                currentLine[m++] = inputBuf[i];
            }
            currentLine[m] = '\0';
            k = k + duplicateCount + 1;
            m = 0;

            int compareResult = ignoreCase ? strCmp(outputLines[currentIndex], currentLine) : strcmp(outputLines[currentIndex], currentLine);

            if (compareResult != 0) {
                currentIndex = currentIndex + 1;
                outputLines[currentIndex] = (char *)malloc((duplicateCount + 1) * sizeof(char *));
                for (x = 0; x < duplicateCount; x++) {
                    outputLines[currentIndex][x] = currentLine[x];
                }
                outputLines[currentIndex][x] = '\0';
                lineRepeats[currentIndex] = 1;
            } else {
                lineRepeats[currentIndex] = lineRepeats[currentIndex] + 1;
            }
        }
    }

    if (countLines) {
        for (y = 0; y < currentIndex + 1; y++) {
            printf(1, "%d %s \n", lineRepeats[y], outputLines[y]);
        }
    } else if (printDuplicates) {
        for (z = 0; z < currentIndex + 1; z++) {
            if (lineRepeats[z] > 1) {
                printf(1, "%s \n", outputLines[z]);
            }
        }
    } else {
        for (z = 0; z < currentIndex + 1; z++) {
            printf(1, "%s \n", outputLines[z]);
        }
    }

    for (int j = 0; j <= currentIndex; j++) {
        free(outputLines[j]);
    }

    return 0;
}

int main(int argc, char **argv) {
    int fileDescriptor, r;
    if (argc <= 1) {
        uniqueLines(0, "", 0, 0, 0); // Default behavior: no options
        exit();
    } else if (argc == 2) {
        for (r = 1; r < argc; r++) {
            if ((fileDescriptor = open(argv[r], 0)) < 0) {
                printf(1, "uniqueLines: cannot open %s\n", argv[r]);
                exit();
            }
            uniqueLines(fileDescriptor, argv[r], 0, 0, 0); // Default behavior: no options
            close(fileDescriptor);
        }
        exit();
    } else {
        // Parse options
        int countLines = 0;
        int printDuplicates = 0;
        int ignoreCase = 0;

        for (r = 1; r < argc; r++) {
            if (strcmp(argv[r], "-c") == 0 || strcmp(argv[r], "-C") == 0) {
                countLines = 1;
            } else if (strcmp(argv[r], "-d") == 0 || strcmp(argv[r], "-D") == 0) {
                printDuplicates = 1;
            } else if (strcmp(argv[r], "-i") == 0 || strcmp(argv[r], "-I") == 0) {
                ignoreCase = 1;
            } else {
                break;
            }
        }

        for (; r < argc; r++) {
            if ((fileDescriptor = open(argv[r], 0)) < 0) {
                printf(1, "uniqueLines: cannot open %s\n", argv[r]);
                exit();
            }
            inputArgs = argv[r];
            uniqueLines(fileDescriptor, argv[r], countLines, printDuplicates, ignoreCase);
            close(fileDescriptor);
        }
        exit();
    }
}
