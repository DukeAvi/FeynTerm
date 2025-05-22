#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define Max_Words 100
#define Max_Words_Length 50
#define Max_Filename_Length 100

// Function to convert a string to lowercase
void convert_lowercase(char *word);

// Function to check if a word is in the ignore list
int if_ignored(const char *word);

// Function to write filtered text to CSV file
void write_toCSV(const char *filename, char *text);

// Function to read text from a file
bool read_from_file(const char *filename, char *buffer, int buffer_size);

// Function for manual input mode
void manual_input_mode();

// Function for file input mode
void file_input_mode();

#endif // TEXT_PROCESSOR_H