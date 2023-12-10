#include "userprog/process.h"

void test_process_execute_arguments(void) {
    // Test case: Running a simple program with arguments
    char command_line[] = "echo Hello World!";
    tid_t tid = process_execute(command_line);

    // Validate if process creation was successful
    ASSERT(tid != TID_ERROR);

    // Add additional checks or assertions as needed
}

void run_test(void) {
    test_process_execute_arguments();
    // Add more test cases if necessary
}

