#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


char buf[5120000];
char *args;

// Define strcasecmp function if it's not available
int strcasecmp(const char *s1, const char *s2) {
    int i;
    for (i = 0; s1[i] && s2[i]; ++i) {
        char c1 = s1[i];
        char c2 = s2[i];
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 'a' - 'A'; // Convert to lowercase
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 'a' - 'A'; // Convert to lowercase
        }
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return s1[i] - s2[i];
}

int uniq(int fd, char *name, int count_lines, int print_duplicates, int ignore_case) {
    int final = 0, k, p, q, index = 0, m = 0, l = 0, b, n, i, count = 0, count1 = 0;
    char pilot[10000];
    char *output[10000];
    int repeat[1000];

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (b = 0; b < n; b++) {
            if (buf[b] == '\n')
                l++;
        }
        for (i = 0; buf[i] != '\n'; i++) {
            count++;
            pilot[i] = buf[i];
        }

        pilot[i] = '\0';
        repeat[0] = 1;
        output[0] = (char *)malloc((count + 1) * sizeof(char *));
        for (i = 0; i < count + 1; i++) {
            output[index][i] = pilot[i];
        }
        output[index][i] = '\0';
        k = i;

        while (final < l - 1) {
            final++;
            count1 = 0;
            for (i = k; buf[i] != '\n'; i++) {
                count1++;
                pilot[m++] = buf[i];
            }
            pilot[m] = '\0';
            k = k + count1 + 1;
            m = 0;

            int compare_result = ignore_case ? strcasecmp(output[index], pilot) : strcmp(output[index], pilot);

            if (compare_result != 0) {
                index = index + 1;
                output[index] = (char *)malloc((count1 + 1) * sizeof(char *));
                for (p = 0; p < count1; p++) {
                    output[index][p] = pilot[p];
                }
                output[index][p] = '\0';
                repeat[index] = 1;
            } else {
                repeat[index] = repeat[index] + 1;
            }
        }
    }

    if (count_lines) {
        for (q = 0; q < index + 1; q++) {
            printf(1, "%d %s \n", repeat[q], output[q]);
        }
    } else if (print_duplicates) {
        for (q = 0; q < index + 1; q++) {
            if (repeat[q] > 1) {
                printf(1, "%s \n", output[q]);
            }
        }
    } else {
        for (q = 0; q < index + 1; q++) {
            printf(1, "%s \n", output[q]);
        }
    }

    for (int j = 0; j <= index; j++) {
        free(output[j]);
    }

    return 0;
}

int main(int argc, char **argv) {
    int fd, r;
    if (argc <= 1) {
        uniq(0, "", 0, 0, 0); // Default behavior: no options
        exit();
    } else if (argc == 2) {
        for (r = 1; r < argc; r++) {
            if ((fd = open(argv[r], 0)) < 0) {
                printf(1, "uniq: cannot open %s\n", argv[r]);
                exit();
            }
            uniq(fd, argv[r], 0, 0, 0); // Default behavior: no options
            close(fd);
        }
        exit();
    } else {
        // Parse options
        int count_lines = 0;
        int print_duplicates = 0;
        int ignore_case = 0;

        for (r = 1; r < argc; r++) {
            if (strcmp(argv[r], "-c") == 0 || strcmp(argv[r], "-C") == 0) {
                count_lines = 1;
            } else if (strcmp(argv[r], "-d") == 0 || strcmp(argv[r], "-D") == 0) {
                print_duplicates = 1;
            } else if (strcmp(argv[r], "-i") == 0 || strcmp(argv[r], "-I") == 0) {
                ignore_case = 1;
            } else {
                break;
            }
        }

        for (; r < argc; r++) {
            if ((fd = open(argv[r], 0)) < 0) {
                printf(1, "uniq: cannot open %s\n", argv[r]);
                exit();
            }
            args = argv[r];
            uniq(fd, argv[r], count_lines, print_duplicates, ignore_case);
            close(fd);
        }
        exit();
    }
}
