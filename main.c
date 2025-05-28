#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "ascii.h"
#include "text_processor.h"
#include "pdf_processor.h"
#include "grader/grading.h"

int main(void) {
    logo();
    
    int choice;
    printf("Choose input method:\n");
    printf("1. Manual input (type text)\n");
    printf("2. File input (provide text files)\n");
    printf("3. PDF input mode (provide a pdf)\n\n");
    printf("Enter your choice (1 or 2 or 3): ");
    scanf("%d", &choice);
    getchar(); 

    switch (choice) {
        case 1:
            manual_input_mode();
            break;
        case 2:
            file_input_mode();
            break;
        case 3:
            pdf_input_mode();
            file_input_mode();
            break;
        default:
            printf("Invalid choice. Exiting.\n");
            return 1;
    }

    // After CSVs are created, run grading
    char option;
    GradeBoundary current_scale[10];
    memcpy(current_scale, default_grading_scale, num_grades * sizeof(GradeBoundary));

    printf("\nGrading System\n");
    printf("1. Use default grading scale\n");
    printf("2. Configure custom grading scale\n");
    printf("Choose option: ");
    scanf(" %c", &option);
    getchar();
    if (option == '2') {
        configure_grading_scale(current_scale);
    }

    AnswerList key = read_answers("reference.csv");
    AnswerList user = read_answers("user_input.csv");

    if (key.answers && user.answers) {
        int matches = count_matches(key, user);
        if (matches >= 0) {
            printf("\nResults:\n");
            printf("Correct answers: %d/%d\n", matches, key.count);
            int percentage = (matches * 100) / key.count;
            printf("Percentage: %d%%\n", percentage);
            char grade = calculate_grade(matches, key.count, current_scale);
            printf("Grade: %c\n", grade);
        } else {
            printf("Error: Answer counts don't match!\n");
        }
    } else {
        printf("Error reading files!\n");
    }
    free_answers(key);
    free_answers(user);

    return 0;
}