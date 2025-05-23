#include <stdio.h>
#include <string.h>
#include "matching.h"
#include "grading.h"

int main()
{
    char key_file[256], user_file[256];
    GradeBoundary current_scale[10];

    memcpy(current_scale, default_grading_scale, sizeof(default_grading_scale));

    printf("Grading System\n");
    printf("1. Use default grading scale\n");
    printf("2. Configure custom grading scale\n");
    printf("Choose option: ");

    char option;
    scanf("%c", &option);
    getchar();

    if (option == '2')
    {
        configure_grading_scale(current_scale);
    }

    printf("\nEnter answer key CSV filename: ");
    fgets(key_file, sizeof(key_file), stdin);
    key_file[strcspn(key_file, "\n")] = '\0';

    printf("Enter user answer CSV filename: ");
    fgets(user_file, sizeof(user_file), stdin);
    user_file[strcspn(user_file, "\n")] = '\0';

    AnswerList key = read_answers(key_file);
    AnswerList user = read_answers(user_file);

    if (key.answers && user.answers)
    {
        int matches = count_matches(key, user);
        if (matches >= 0)
        {
            printf("\nResults:\n");
            printf("Correct answers: %d/%d\n", matches, key.count);

            int percentage = (matches * 100) / key.count;
            printf("Percentage: %d%%\n", percentage);

            char grade = calculate_grade(matches, key.count, current_scale);
            printf("Grade: %c\n", grade);
        }
        else
        {
            printf("Error: Answer counts don't match!\n");
        }
    }
    else
    {
        printf("Error reading files!\n");
    }

    free_answers(key);
    free_answers(user);

    return 0;
}   
