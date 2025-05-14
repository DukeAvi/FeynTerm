#ifndef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h> // for strncasecmp on some platforms


//Function declarations
int get_pdf_page_count(const char*);
bool render_pdf_pages_to_png(const char*, int);
bool png_to_pgm_format();


#endif