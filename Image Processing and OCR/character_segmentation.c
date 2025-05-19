#include "character_segmentation.h"

extern int file_count;
extern int line_count;
extern int word_count;

//Perform line_segmentation on a page of text.
void line_segmentation() {
    char filename[32];
    for(int file_index = 1; file_index <= file_count; file_index++) {
        snprintf(filename, sizeof(filename), "output_%d.pgm", file_index);
        FILE* fp = fopen(filename, "r");
        if(!fp) {
            perror("File Not Found! Continuing with other files..");
            continue;
        }
        char magic_no[3]; //P2 or P5 .pgm 
        int width, height, threshold;

        fscanf(fp, "%2s", magic_no);
        fscanf(fp, "%d %d", &width, &height);
        fscanf(fp, "%d", &threshold);

        int** pixels = (int**) malloc(height * sizeof(int*));
        if (!pixels) {
            fprintf(stderr, "Memory allocation failed for pixels array. Skipping file %s.\n", filename);
            fclose(fp);
            continue;
        }

        int allocation_failed = 0;
        for(int row = 0; row < height; row++) {
            pixels[row] = (int*) malloc(width * sizeof(int));
            if (!pixels[row]) {
                fprintf(stderr, "Memory allocation failed for pixels[%d]. Skipping file %s.\n", row, filename);
                // Free any previously allocated rows
                for (int i = 0; i < row; i++) free(pixels[i]);
                free(pixels);
                fclose(fp);
                allocation_failed = 1;
                break;
            }
            for(int column = 0; column < width; column++) {
                fscanf(fp, "%d", &pixels[row][column]);
            }
        }
        fclose(fp);
        if (allocation_failed) continue;

        int in_line, start_line, end_line;
        in_line = start_line = end_line = 0;

        for(int row = 0; row < height; row++) {
            int has_black_flag = 0; 
            for(int column = 0; column < width; column++) {
                if(pixels[row][column] == 0) {
                    has_black_flag = 1;
                    break;
                }
            }
            if(has_black_flag && !in_line) {
                start_line = row;
                in_line = 1;
            }

            else if(!has_black_flag && in_line) {
                in_line = 0;
                end_line = row - 1;
                char newFileName[32];
                sprintf(newFileName, "line_%d.pgm", ++line_count);

                int new_height = (end_line - start_line + 1) + 4; // 2 pixels padding top and bottom
                int new_width = width + 4; // 2 pixels padding left and right

                FILE* new_fp = fopen(newFileName, "w");
                if (!new_fp) {
                    perror("Failed to create line output file");
                    continue;
                }

                fprintf(new_fp, "P2\n%d %d\n%d\n", new_width, new_height, threshold);
                for(int r = -2; r < (end_line - start_line + 1) + 2; r++) {
                    for(int c = -2; c < width + 2; c++) {
                        if (r < 0 || r >= (end_line - start_line + 1) || c < 0 || c >= width)
                            fprintf(new_fp, "%d ", threshold);
                        else
                            fprintf(new_fp, "%d ", pixels[start_line + r][c]);
                    }
                    fprintf(new_fp, "\n");
                }

                fclose(new_fp);
            }
        }
        for(int row = 0; row < height; row++)
            free(pixels[row]);
        free(pixels);
    }
    return;
}

// Performs word segmentation on each line extracted previously from all pages.
void word_segmentation() {
    char filename[32];

    for (int line_index = 1; line_index <= line_count; line_index++) {
        snprintf(filename, sizeof(filename), "line_%d.pgm", line_index);
        FILE* fp = fopen(filename, "r");
        if (!fp) {
            perror("File Not Found! Continuing with other files..");
            continue;
        }

        char magic_no[3];
        int width, height, threshold;
        fscanf(fp, "%2s", magic_no);
        fscanf(fp, "%d %d", &width, &height);
        fscanf(fp, "%d", &threshold);

        int** pixels = (int**)malloc(height * sizeof(int*));
        for (int r = 0; r < height; r++) {
            pixels[r] = (int*)malloc(width * sizeof(int));
            for (int c = 0; c < width; c++) {
                fscanf(fp, "%d", &pixels[r][c]);
            }
        }
        fclose(fp);

        int in_word, start_word, end_word, blank_count;
        in_word = start_word = end_word = blank_count = 0;
        int blank_threshold = 5; // adjust based on font size/spacing

        for (int col = 0; col < width; col++) {
            int has_black_pixel = 0;
            for (int row = 0; row < height; row++) {
                if (pixels[row][col] == 0) {
                    has_black_pixel = 1;
                    break;
                }
            }

            if (has_black_pixel) {
                if (!in_word) {
                    in_word = 1;
                    start_word = col;
                }
                blank_count = 0; // reset blanks inside a word
            } else if (in_word) {
                blank_count++;
                if (blank_count >= blank_threshold) {
                    end_word = col - blank_count;

                    char out_filename[32];
                    snprintf(out_filename, sizeof(out_filename), "word_%d.pgm", ++word_count);
                    FILE* out_fp = fopen(out_filename, "w");
                    if (out_fp) {
                        int pad = 2;
                        int out_width = end_word - start_word + 1 + 2 * pad;
                        fprintf(out_fp, "P2\n%d %d\n%d\n", out_width, height + 2 * pad, threshold);

                        for (int r = -pad; r < height + pad; r++) {
                            for (int c = -pad; c < (end_word - start_word + 1) + pad; c++) {
                                int rr = r, cc = start_word + c;
                                if (rr >= 0 && rr < height && cc >= 0 && cc < width)
                                    fprintf(out_fp, "%d ", pixels[rr][cc]);
                                else
                                    fprintf(out_fp, "%d ", threshold);  // fill padding with white
                            }
                            fprintf(out_fp, "\n");
                        }
                        fclose(out_fp);
                    }
                    in_word = 0;
                    blank_count = 0;
                    end_word = 0;
                }
            }
        }

        // If file ended while still inside a word, write that word too
        if (in_word) {
            int end_word = width - 1;
            char out_filename[32];
            snprintf(out_filename, sizeof(out_filename), "word_%d.pgm", ++word_count);
            FILE* out_fp = fopen(out_filename, "w");
            if (out_fp) {
                int pad = 2;
                int out_width = end_word - start_word + 1 + 2 * pad;
                fprintf(out_fp, "P2\n%d %d\n%d\n", out_width, height + 2 * pad, threshold);

                for (int r = -pad; r < height + pad; r++) {
                    for (int c = -pad; c < (end_word - start_word + 1) + pad; c++) {
                        int rr = r, cc = start_word + c;
                        if (rr >= 0 && rr < height && cc >= 0 && cc < width)
                            fprintf(out_fp, "%d ", pixels[rr][cc]);
                        else
                            fprintf(out_fp, "%d ", threshold);  // fill padding with white
                    }
                    fprintf(out_fp, "\n");
                }
                fclose(out_fp);
            }
        }

        for (int r = 0; r < height; r++)
            free(pixels[r]);
        free(pixels);
    }
    return;
}

