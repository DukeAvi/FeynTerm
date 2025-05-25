#include <stdio.h>
#include "img_processing.h"
#include "character_segmentation.h"
#include "template_matching.h"

int file_count = 0; 
int line_count = 0;
int word_count = 0;
int letter_count = 0;
int letters_per_word[5000];

int main() {
    const char* input_file = "input.pdf";

    // PDF Processing
    file_count = get_pdf_page_count(input_file);
    if (file_count <= 0) {
        fprintf(stderr, "Error: Invalid PDF page count\n");
        return 1;
    }

    if (!render_pdf_pages_to_png(input_file)) {
        fprintf(stderr, "Error: PDF rendering failed\n");
        return 1;
    }

    // Image Conversion
    if (!png_to_pgm_format()) {
        fprintf(stderr, "Error: PNG to PGM conversion failed\n");
        return 1;
    }

    // Segmentation Pipeline
    line_segmentation();
    word_segmentation();
    letter_segmentation();
    rescale_pgm();

    // OCR Processing
    load_templates();
    template_matching();

    // cleanup unwanted files and folders.
    cleanup();

    printf("OCR completed successfully. Results saved to output.txt\n");
    return 0;
}