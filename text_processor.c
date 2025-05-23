#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "text_processor.h"

// These are defined in text_processor.h, no need to redefine them here
// #define Max_Words 100
// #define Max_Words_Length 50
// #define Max_Filename_Length 100

// Using ignore_words from ignore_words.c
extern const char *ignore_words[];
extern int ignore_words_count;

void convert_lowercase(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower((unsigned char)word[i]);
    }
}

int if_ignored(const char *word) {
    for (int i = 0; i < ignore_words_count; i++) {
        if (strcmp(word, ignore_words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void write_toCSV(const char *filename, char *text) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s for writing!\n", filename);
        return;
    }

    char word[Max_Words_Length] = "";
    int j = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        if (isalnum(text[i])) {
            if (j < Max_Words_Length - 1) {
                word[j++] = tolower(text[i]);
            }
        } else if (j > 0) {
            word[j] = '\0';
            if (!if_ignored(word)) {
                fprintf(file, "%s\n", word);
            }
            j = 0;
        }
    }

    if (j > 0) {
        word[j] = '\0';
        if (!if_ignored(word)) {
            fprintf(file, "%s\n", word);
        }
    }

    fclose(file);
}

bool read_from_file(const char *filename, char *buffer, int buffer_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s for reading!\n", filename);
        return false;
    }

    size_t len = fread(buffer, 1, buffer_size - 1, file);
    buffer[len] = '\0';
    fclose(file);
    return true;
}

void manual_input_mode() {
    char reference_input[Max_Words * Max_Words_Length];
    char user_input[Max_Words * Max_Words_Length];

    printf("Enter the Reference input:\n");
    fgets(reference_input, sizeof(reference_input), stdin);
    reference_input[strcspn(reference_input, "\n")] = '\0';

    printf("Enter the user's answer:\n");
    fgets(user_input, sizeof(user_input), stdin);
    user_input[strcspn(user_input, "\n")] = '\0';

    write_toCSV("reference.csv", reference_input);
    write_toCSV("user_input.csv", user_input);

    printf("\nCSV files 'reference.csv' and 'user_input.csv' created successfully.\n");
}

void file_input_mode() {
    char reference_file[Max_Filename_Length];
    char user_file[Max_Filename_Length];
    char reference_input[Max_Words * Max_Words_Length];
    char user_input[Max_Words * Max_Words_Length];

    printf("Enter the filename for the reference text: ");
    fgets(reference_file, sizeof(reference_file), stdin);
    reference_file[strcspn(reference_file, "\n")] = '\0';

    printf("Enter the filename for the user's answer: ");
    fgets(user_file, sizeof(user_file), stdin);
    user_file[strcspn(user_file, "\n")] = '\0';

    if (!read_from_file(reference_file, reference_input, sizeof(reference_input))) {
        return;
    }
    if (!read_from_file(user_file, user_input, sizeof(user_input))) {
        return;
    }

    write_toCSV("reference.csv", reference_input);
    write_toCSV("user_input.csv", user_input);    printf("\nCSV files 'reference.csv' and 'user_input.csv' created successfully.\n");
}