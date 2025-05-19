#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#define THRESHOLD 128 //For Binarization 

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h> // for strncasecmp on some platforms


//Function declarations
int get_pdf_page_count(const char*);
bool render_pdf_pages_to_png(const char*);
bool png_to_pgm_format();


#endif