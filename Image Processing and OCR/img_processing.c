#include "img_processing.h"

//Computes the number of pages in a given PDF.
int get_pdf_page_count(const char* pdf_file) {
    char command[512];
    sprintf(command, ".\\mutool.exe info \"%s\" > pages.txt", pdf_file);
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
        if (strncasecmp(line, "pages:", 6) == 0) {
            sscanf(line + 6, "%d", &pages);
            break;
        }
    }
    fclose(fp);
    remove("pages.txt");
    if (pages == -1)
        printf("Failed to extract page count from output\n");
    return pages;
}


//Executes the powershell command to render PDF to .png images.
bool render_pdf_pages_to_png(const char* pdf_file, int max_pages) {
    char command[300];
    for (int page = 1; page <= max_pages; page++) {
            sprintf(command, ".\\mutool.exe draw -o output_%d.png \"%s\" %d", 
                    page, pdf_file, page);
        if (system(command) != 0) {
            printf("Failed to render page %d\n", page);
            return 0; //returning false
        }
    }
    return 1; //returning true
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
            return 0; //returns false
        }
        fprintf(ptr_to_outputfile, "P2\n%d %d\n255\n", width, height);
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int index = (j * width + i) * channels;
                int R = img[index], G = img[index + 1], B = img[index + 2];
                int gray = round(0.299 * R + 0.587 * G + 0.114 * B);
                fprintf(ptr_to_outputfile, "%d ", gray);
            }
            fprintf(ptr_to_outputfile, "\n");
        }
        stbi_image_free(img);
        fclose(ptr_to_outputfile);
        count++;
    }
    return 1; //returns true
}