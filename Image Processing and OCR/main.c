#include <stdio.h>
#include "img_processing.h"
#include "img_processing.c"

int main() {
    const char* pdf_input = "input.pdf";
    
    int page_count = get_pdf_page_count(pdf_input);
    if (page_count <= 0) {
        printf("Couldn't determine page count\n");
        return 1;
    }
    
    if (!render_pdf_pages_to_png(pdf_input, page_count)) {
        printf("Rendering failed\n");
        return 1;
    }
    
    if (!png_to_pgm_format()) {
        printf("Conversion failed\n");
        return 1;
    }
    
    printf("Program Successful\n");
    return 0;
}

