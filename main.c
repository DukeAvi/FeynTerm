#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "ascii.h"
#include "text_processor.h"

int main(void) {
    logo();
    
    int choice;
    printf("Choose input method:\n");
    printf("1. Manual input (type text)\n");
    printf("2. File input (provide text files)\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    getchar(); 

    switch (choice) {
        case 1:
            manual_input_mode();
            break;
        case 2:
            file_input_mode();
            break;
        default:
            printf("Invalid choice. Exiting.\n");
            return 1;
    }

    return 0;
}