#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <direct.h>

#define THRESHOLD 120

//Function declarations
int get_pdf_page_count();
bool render_pdf_pages_to_png();
bool png_to_pgm_format();

#endif