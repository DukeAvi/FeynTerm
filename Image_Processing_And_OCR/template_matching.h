#ifndef TEMPLATE_MATCHING_H
#define TEMPLATE_MATCHING_H

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_WORDCOUNT 5000
#define MAX_POINTS 400   // at most every pixel could be a point



extern int word_count;
extern int letter_count;
extern int letters_per_word[MAX_WORDCOUNT];

typedef struct { 
    int x, y; 
}Point;

void load_templates();
void template_matching();
void extract_template_points(int idx);
void cleanup();

#endif