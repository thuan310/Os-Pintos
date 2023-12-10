#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // Check if at least three arguments (name, student ID, age) are provided
    if (argc < 4) {
        printf("Insufficient arguments provided.\n");
        printf("Usage: %s <name> <studentID> <age>\n", argv[0]);
        return 1; // Return with an error code
    }

    // Extract arguments and print them
    char *name = argv[1];
    int studentID = atoi(argv[2]); // Convert string to integer
    int age = atoi(argv[3]); // Convert string to integer

    // Print the entered information
    printf("\nStudent's Information\n");
    printf("Name: %s\n", name);
    printf("Student ID: %d\n", studentID);
    printf("Age: %d\n", age);

    
	

    printf("\nPress any key to exit...\n");

    return 0;
}
