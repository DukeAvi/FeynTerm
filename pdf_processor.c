#include "pdf_processor.h"
#include "Image_Processing_and_OCR/img_processing.h"
#include "Image_Processing_and_OCR/character_segmentation.h"
#include "Image_Processing_and_OCR/template_matching.h"

// Global variable definitions (not declarations)
int file_count = 0;
int line_count = 0;
int word_count = 0;
int letter_count = 0;
int letters_per_word[5000];
const char* pdf_file = "input.pdf";

void pdf_input_mode() {
    // PDF Processing
    file_count = get_pdf_page_count();
    if (file_count <= 0) {
        fprintf(stderr, "Error: Invalid PDF page count\n");
        return;
    }

    if (!render_pdf_pages_to_png()) {
        fprintf(stderr, "Error: PDF rendering failed\n");
        return;
    }

    // Image Conversion
    if (!png_to_pgm_format()) {
        fprintf(stderr, "Error: PNG to PGM conversion failed\n");
        return;
    }

    // Segmentation Pipeline
    line_segmentation();
    word_segmentation();
    letter_segmentation();
    rescale_pgm();

    // OCR Processing
    load_templates();
    // Change working directory for template_matching if needed, or update template_matching to use out_txt
    template_matching();

    // cleanup unwanted files and folders.
    cleanup();

    printf("OCR completed successfully. Results saved to output.txt\n\n");
}