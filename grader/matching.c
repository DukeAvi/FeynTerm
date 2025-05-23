#include "matching.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int compare_insensitive(const void *a, const void *b)
{
    return strcasecmp(*(const char **)a, *(const char **)b);
}

void to_lower(char *str)
{
    for (; *str; str++)
    {
        *str = tolower(*str);
    }
}

AnswerList read_answers(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) return (AnswerList){NULL, 0};

    char buffer[1024];
    int count = 0;
    // First, count lines
    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }
    rewind(file);

    AnswerList list = {malloc(count * sizeof(char *)), count};
    int idx = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove newline
        buffer[strcspn(buffer, "\r\n")] = '\0';
        // Trim leading/trailing spaces
        char *start = buffer;
        while (isspace((unsigned char)*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';
        list.answers[idx] = strdup(start);
        to_lower(list.answers[idx]);
        idx++;
    }
    fclose(file);
    return list;
}

int count_matches(AnswerList key, AnswerList user)
{
    if (key.count == 0 || user.count == 0) {
        return -1;
    }

    int matches = 0;
    for (int i = 0; i < user.count; i++) {
        for (int j = 0; j < key.count; j++) {
            if (strcmp(user.answers[i], key.answers[j]) == 0) {
                matches++;
                break;
            }
        }
    }
    return matches;
}

void free_answers(AnswerList list)
{
    for (int i = 0; i < list.count; i++)
    {
        free(list.answers[i]);
    }
    free(list.answers);
}
