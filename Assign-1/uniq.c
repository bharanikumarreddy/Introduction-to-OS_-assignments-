#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buffer[1000000];
char *arguments;

int str_cmp(const char *str1, const char *str2) {
    for (int i = 0; str1[i] && str2[i]; ++i) {
        char char1 = str1[i] | 32; // Convert to lowercase
        char char2 = str2[i] | 32; 
        if (char1 != char2) {
            return char1 - char2;
        }
    }
    return 0;
}

int final_count = 0;
int k, x, y, z, b, bytes_read, i; 
int current_index = 0, m = 0, line_count = 0, char_count = 0, duplicate_count = 0;

int unique_lines(int file_descriptor, char *filename, int count_lines, int print_duplicates, int ignore_case) {
    char current_line[10000], *output_lines[10000];
    int line_repeats[1000];

    while ((bytes_read = read(file_descriptor, buffer, sizeof(buffer))) > 0) {
        for (b = 0; b < bytes_read; b++) {
            if (buffer[b] == '\n') line_count++;
        }
        for (i = 0; buffer[i] != '\n'; i++) {
            char_count++;
            current_line[i] = buffer[i];
        }

        current_line[i] = '\0';
        line_repeats[0] = 1;
        output_lines[0] = (char *)malloc((char_count + 1) * sizeof(char *));
        for (i = 0; i < char_count + 1; i++) output_lines[current_index][i] = current_line[i];
        output_lines[current_index][i] = '\0';
        k = i;

        while (final_count < line_count - 1) {
            final_count++;
            duplicate_count = 0;
            for (i = k; buffer[i] != '\n'; i++) duplicate_count++, current_line[m++] = buffer[i];
            current_line[m] = '\0';
            k = k + duplicate_count + 1, m = 0;

            int compare_result = ignore_case ? str_cmp(output_lines[current_index], current_line) : strcmp(output_lines[current_index], current_line);

            if (compare_result != 0) {
                current_index = current_index + 1;
                output_lines[current_index] = (char *)malloc((duplicate_count + 1) * sizeof(char *));
                for (x = 0; x < duplicate_count; x++) output_lines[current_index][x] = current_line[x];
                output_lines[current_index][x] = '\0';
                line_repeats[current_index] = 1;
            } else line_repeats[current_index]++;
        }
    }

    for (y = 0; y < current_index + 1; y++) {
        if (count_lines) printf(1, "%d %s \n", line_repeats[y], output_lines[y]);
        else if (print_duplicates && line_repeats[y] > 1) printf(1, "%s \n", output_lines[y]);
        else if (!print_duplicates) printf(1, "%s \n", output_lines[y]);
        free(output_lines[y]);
    }

    return 0;
}

int main(int argc, char **argv) {
    int file_descriptor, r;

    
    int count_lines = 0, print_duplicates = 0, ignore_case = 0;

    if (argc <= 1) {
        unique_lines(0, "", count_lines, print_duplicates, ignore_case);
        exit();
    } else if (argc == 2) {
        for (r = 1; r < argc; r++) {
            if ((file_descriptor = open(argv[r], 0)) < 0) {
                printf(1, "unique_lines: cannot open %s\n", argv[r]);
                exit();
            }
            unique_lines(file_descriptor, argv[r], count_lines, print_duplicates, ignore_case);
            close(file_descriptor);
        }
        exit();
    } else {
        
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
            if ((file_descriptor = open(argv[r], 0)) < 0) {
                printf(1, "unique_lines: cannot open %s\n", argv[r]);
                exit();
            }
            arguments = argv[r];
            unique_lines(file_descriptor, argv[r], count_lines, print_duplicates, ignore_case);
            close(file_descriptor);
        }
        exit();
    }
}
