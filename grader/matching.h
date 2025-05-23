#ifndef MATCHING_H
#define MATCHING_H

#include <stdio.h>

typedef struct
{
    char **answers;
    int count;
} AnswerList;

int compare_insensitive(const void *a, const void *b);

void to_lower(char *str);

AnswerList read_answers(const char *filename);

int count_matches(AnswerList key, AnswerList user);

void free_answers(AnswerList list);

#endif // MATCHING_H
