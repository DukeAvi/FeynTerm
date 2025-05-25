#ifndef CHARACTER_SEGMENTATION_H
#define CHARACTER_SEGMENTATION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_WORDCOUNT 5000

extern int file_count;
extern int line_count;
extern int word_count;
extern int letter_count;
extern int letters_per_word[MAX_WORDCOUNT];

void line_segmentation();
void word_segmentation();
void letter_segmentation();
void rescale_pgm();

#endif
