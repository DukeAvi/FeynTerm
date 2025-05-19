#include <stdio.h>
#include "img_processing.h"
#include "character_segmentation.h"

int file_count = 0;
int line_count = 0;
int word_count = 0;

int main() {
    const char* pdf_input = "sample.pdf";
    
    file_count = get_pdf_page_count(pdf_input);
    if (file_count <= 0) {
        printf("Couldn't determine page count\n");
        return 1;
    }
    
    if (!render_pdf_pages_to_png(pdf_input)) {
        printf("Rendering failed\n");
        return 1;
    }
    
    if (!png_to_pgm_format()) {
        printf("Conversion failed\n");
        return 1;
    }

    line_segmentation();
    word_segmentation();
    
    printf("Program Successful\n");
    return 0;
}

