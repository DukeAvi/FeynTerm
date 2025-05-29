#ifndef PDF_PROCESSOR_H
#define PDF_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int file_count;
extern int line_count;
extern int word_count;
extern int letter_count;
extern int letters_per_word[5000];
extern const char* pdf_file;

void pdf_input_mode();

#endif