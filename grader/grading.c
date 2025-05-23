#include "grading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

GradeBoundary default_grading_scale[] = 
{
    {'S', 90},
    {'A', 80},
    {'B', 70},
    {'C', 60},
    {'D', 50},
    {'F', 0}
};

int num_grades = sizeof(default_grading_scale) / sizeof(default_grading_scale[0]);

void configure_grading_scale(GradeBoundary *scale) 
{
    printf("\nConfigure Grading Scale (enter grades in descending order):\n");

    char input[10];
    for (int i = 0; i < num_grades; i++) 
    {
        printf("Enter grade letter for %d%%+: ", scale[i].min_percentage);
        fgets(input, sizeof(input), stdin);
        scale[i].grade = toupper(input[0]);

        if (i < num_grades - 1) 
        {
            printf("Enter minimum percentage for %c grade: ", scale[i].grade);
            fgets(input, sizeof(input), stdin);
            scale[i + 1].min_percentage = atoi(input);
        }
    }
}

char calculate_grade(int correct, int total, GradeBoundary *scale) 
{
    if (total == 0) 
        return '?';

    int percentage = (correct * 100) / total;

    for (int i = 0; i < num_grades; i++) 
    {
        if (percentage >= scale[i].min_percentage) 
        {
            return scale[i].grade;
        }
    }

    return scale[num_grades - 1].grade;
}