#include "img_processing.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define true 1
#define false 0

#ifdef _WIN32
#define MUTOOLEXE "Image_Processing_And_OCR\\mutool.exe"
#else
#define MUTOOLEXE "Image_Processing_And_OCR/mutool"
#endif

extern int file_count;
extern const char* pdf_file;

//Computes the number of pages in a given PDF.
int get_pdf_page_count() {
    char command[512];
    #ifdef _WIN32
    sprintf(command, "%s info \"%s\" > pages.txt", MUTOOLEXE, pdf_file);
    #else
    sprintf(command, "%s info '%s' > pages.txt", MUTOOLEXE, pdf_file);
    #endif
    int ret = system(command);
    if (ret != 0) {
        printf("mutool command failed!\n");
        return -1;
    }
    FILE* fp = fopen("pages.txt", "r");
    if (!fp) {
        printf("Could not open pages.txt\n");
        return -1;
    }
    char line[256];
    int pages = -1;
    while (fgets(line, sizeof(line), fp)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (strncasecmp(p, "pages:", 6) == 0) {
            sscanf(p + 6, "%d", &pages);
            break;
        }
    }
    fclose(fp);
    remove("pages.txt");
    if (pages == -1)
        printf("Failed to extract page count from output\n");
    return pages;
}


//Executes the command to render PDF to .png images.
bool render_pdf_pages_to_png() {
    char syscmd[512];
    #ifdef _WIN32
    sprintf(syscmd, "cd Image_Processing_And_OCR && mutool.exe draw -F txt -o tmp_resources.txt input.pdf > nul 2>&1");
    system(syscmd);
    #else
    sprintf(syscmd, "cd Image_Processing_And_OCR && ./mutool draw -F txt -o tmp_resources.txt input.pdf > /dev/null 2>&1");
    system(syscmd);
    #endif

    char command[300];
    for (int page = 1; page <= file_count; page++) {
        #ifdef _WIN32
        sprintf(command, "%s draw -o output_%d.png \"%s\" %d", MUTOOLEXE, page, pdf_file, page);
        #else
        sprintf(command, "%s draw -o output_%d.png '%s' %d", MUTOOLEXE, page, pdf_file, page);
        #endif
        if (system(command) != 0) {
            printf("Failed to render page %d\n", page);
            return false;
        }
    }
    return true;
}



//To render images from .png to .pgm format.
bool png_to_pgm_format() {
    char input_filename[100], output_filename[100];
    int count = 1;
    while (1) {
        sprintf(input_filename, "output_%d.png", count);
        sprintf(output_filename, "output_%d.pgm", count);
        int width, height, channels;
        unsigned char* img = stbi_load(input_filename, &width, &height, &channels, 0);
        if (img == NULL) break;
        FILE* ptr_to_outputfile = fopen(output_filename, "w");
        if (!ptr_to_outputfile) {
            printf("Failed to open %s for writing.\n", output_filename);
            stbi_image_free(img);
            return false;
        }
        fprintf(ptr_to_outputfile, "P2\n%d %d\n255\n", width, height);

        // Binarize directly while writing the PGM
        int index = -1, R = 0, G = 0, B = 0, gray_pixel = 0;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++) {
                index = (row * width + column) * channels;
                R = img[index], G = img[index + 1], B = img[index + 2];
                gray_pixel = round(0.299 * R + 0.587 * G + 0.114 * B);
                int bin_pixel = (gray_pixel >= THRESHOLD) ? 255 : 0;
                fprintf(ptr_to_outputfile, "%d ", bin_pixel);
            }
            fprintf(ptr_to_outputfile, "\n");
        }
        stbi_image_free(img);
        fclose(ptr_to_outputfile);
        count++;
    }
    return true;
}