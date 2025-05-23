#ifndef GRADING_H
#define GRADING_H

typedef struct 
{
    char **answers;
    int count;
} AnswerList;

typedef struct 
{
    char grade;
    int min_percentage;
} GradeBoundary;

extern GradeBoundary default_grading_scale[];
extern int num_grades;

int compare_insensitive(const void *a, const void *b);

void to_lower(char *str);

AnswerList read_answers(const char *filename);

int count_matches(AnswerList key, AnswerList user);

void configure_grading_scale(GradeBoundary *scale);

char calculate_grade(int correct, int total, GradeBoundary *scale);

void free_answers(AnswerList list);

#endif // GRADING_H
