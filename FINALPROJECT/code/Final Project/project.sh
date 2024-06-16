#!/bin/bash
touch sample.txt
# Function to compile a C program
compile_program() {
    program_name=$1

    # Print a message before compiling
    echo "--------Compiling $program_name.c with gcc...------------------------"

    # Compile the C program
    gcc -o "$program_name" "$program_name.c"
}

# Function to execute a C program with specified arguments
execute_program() {
    program_name=$1
    shift

    # Print a message before executing
    echo "Executing $program_name with arguments: $@"

    # Run the executable with specified arguments
    ./"$program_name" "$@"
}

# Compile and execute each C program
compile_program run1
execute_program run1 sample.txt -w 4024 102400
execute_program run1 sample.txt -r 1024 1024

compile_program run2
execute_program run2 sample.txt 128
execute_program run2 sample.txt 256
execute_program run2 sample.txt 512
execute_program run2 sample.txt 1024

compile_program run3
execute_program run3 sample.txt 64
execute_program run3 sample.txt 128
execute_program run3 sample.txt 256
execute_program run3 sample.txt 512
execute_program run3 sample.txt 1024
execute_program run3 sample.txt 2048
execute_program run3 sample.txt 4096


#compile_program run4
#execute_program run4  # Uncomment this line if you want to run run4

compile_program run5
execute_program run5 sample.txt 


# Print a message before compiling fast.c
printf "\nCompiling fast.c \n"

# Compile fast.c
gcc -o fast fast.c -lm -O3

# Run the fast executable
./fast sample.txt
