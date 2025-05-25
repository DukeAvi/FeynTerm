#include "character_segmentation.h"

//Perform line_segmentation on a page of text.
void line_segmentation() {
    struct stat st = {0};
    if (stat("Lines", &st) == -1) {
        #ifdef _WIN32
        mkdir("Lines");
        #else
        mkdir("Lines", 0700);
        #endif
    }

    char filename[64];
    for(int letter_index = 1; letter_index <= file_count; letter_index++) {
        snprintf(filename, sizeof(filename), "output_%d.pgm", letter_index);
        FILE* fp = fopen(filename, "r");
        if(!fp) {
            perror("File Not Found! Continuing with other files..");
            continue;
        }
        char magic_no[3]; //P2 .pgm 
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
                char newFileName[64];
                sprintf(newFileName, "Lines/line_%d.pgm", ++line_count);

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
                            fprintf(new_fp, "%d ", threshold); // fill padding with white
                        else if (start_line + r >= 0 && start_line + r < height)
                            fprintf(new_fp, "%d ", pixels[start_line + r][c]);
                        else
                            fprintf(new_fp, "%d ", threshold); // fill padding with white if out of bounds
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
    struct stat st = {0};
    if (stat("Words", &st) == -1) {
        #ifdef _WIN32
        mkdir("Words");
        #else
        mkdir("Words", 0700);
        #endif
    }

    word_count = 0; // Reset word count at the start
    char filename[64];
    for (int line_index = 1; line_index <= line_count; line_index++) {
        snprintf(filename, sizeof(filename), "Lines/line_%d.pgm", line_index);
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
        if (!pixels) {
            fprintf(stderr, "Memory allocation failed for pixels array. Skipping file %s.\n", filename);
            fclose(fp);
            continue;
        }
        int allocation_failed = 0;
        for (int r = 0; r < height; r++) {
            pixels[r] = (int*)malloc(width * sizeof(int));
            if (!pixels[r]) {
                fprintf(stderr, "Memory allocation failed for pixels[%d]. Skipping file %s.\n", r, filename);
                for (int i = 0; i < r; i++) free(pixels[i]);
                free(pixels);
                fclose(fp);
                allocation_failed = 1;
                break;
            }
            for (int c = 0; c < width; c++) {
                fscanf(fp, "%d", &pixels[r][c]);
            }
        }
        fclose(fp);
        if (allocation_failed) continue;
        int in_word = 0, start_word = 0, end_word = 0, blank_count = 0;
        int blank_threshold = 5; // Minimum blank columns to consider a word break
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
                blank_count = 0;
            } else if (in_word) {
                blank_count++;
                if (blank_count >= blank_threshold) {
                    end_word = col - blank_count;
                    if (end_word >= start_word) {
                        char out_filename[64];
                        snprintf(out_filename, sizeof(out_filename), "Words/word_%d.pgm", ++word_count);
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
                                        fprintf(out_fp, "%d ", threshold);
                                }
                                fprintf(out_fp, "\n");
                            }
                            fclose(out_fp);
                        }
                    }
                    in_word = 0;
                    blank_count = 0;
                    end_word = 0;
                }
            }
        }
        // If file ended while still inside a word, write that word too
        if (in_word) {
            end_word = width - 1;
            if (end_word >= start_word) {
                char out_filename[64];
                snprintf(out_filename, sizeof(out_filename), "Words/word_%d.pgm", ++word_count);
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
                                fprintf(out_fp, "%d ", threshold);
                        }
                        fprintf(out_fp, "\n");
                    }
                    fclose(out_fp);
                }
            }
        }
        for (int r = 0; r < height; r++)
            free(pixels[r]);
        free(pixels);
    }
    return;
}

void letter_segmentation() {
    struct stat st = {0};
    if (stat("Letters", &st) == -1) {
        #ifdef _WIN32
        mkdir("Letters");
        #else
        mkdir("Letters", 0700);
        #endif
    }

    letter_count = 0; // Reset letter count at the start
    int current_letter_count = 0;
    char filename[64];
    for(int word_index = 1; word_index <= word_count; word_index++) {
        snprintf(filename, sizeof(filename), "Words/word_%d.pgm", word_index);
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

        int in_letter = 0, start_letter = 0, end_letter = 0, blank_count = 0;
        int blank_threshold = 1; // 2 pixel blank for letter segmentation 

        for (int col = 0; col < width; col++) {
            int has_black_pixel = 0;
            for (int row = 0; row < height; row++) {
                if (pixels[row][col] == 0) {
                    has_black_pixel = 1;
                    break;
                }
            }
            if (has_black_pixel) {
                if (!in_letter) {
                    in_letter = 1;
                    start_letter = col;
                }
                blank_count = 0;
            } else if (in_letter) {
                blank_count++;
                if (blank_count >= blank_threshold) {
                    end_letter = col - blank_count;
                    char out_filename[64];
                    current_letter_count++;
                    letter_count++;
                    snprintf(out_filename, sizeof(out_filename), "Letters/letter_%d.pgm", letter_count);
                    FILE* out_fp = fopen(out_filename, "w");
                    if (out_fp) {
                        int pad = 2;
                        int out_width = end_letter - start_letter + 1 + 2 * pad;
                        fprintf(out_fp, "P2\n%d %d\n%d\n", out_width, height + 2 * pad, threshold);
                        for (int r = -pad; r < height + pad; r++) {
                            for (int c = -pad; c < (end_letter - start_letter + 1) + pad; c++) {
                                int rr = r, cc = start_letter + c;
                                if (rr >= 0 && rr < height && cc >= 0 && cc < width)
                                    fprintf(out_fp, "%d ", pixels[rr][cc]);
                                else
                                    fprintf(out_fp, "%d ", threshold); // fill padding with white
                            }
                            fprintf(out_fp, "\n");
                        }
                        fclose(out_fp);
                    }
                    in_letter = 0;
                    blank_count = 0;
                    end_letter = 0;
                }
            }
        }
        // If file ended while still inside a letter, write that letter too
        // Add this to complete the letter_segmentation function properly
        if (in_letter) {
            end_letter = width - 1;
            if (end_letter >= start_letter) {
                char out_filename[64];
                current_letter_count++;
                letter_count++;
                snprintf(out_filename, sizeof(out_filename), "Letters/letter_%d.pgm", letter_count);
                FILE* out_fp = fopen(out_filename, "w");
                if (out_fp) {
                    int pad = 2;
                    int out_width = end_letter - start_letter + 1 + 2 * pad;
                    fprintf(out_fp, "P2\n%d %d\n%d\n", out_width, height + 2 * pad, threshold);
                    for (int r = -pad; r < height + pad; r++) {
                        for (int c = -pad; c < (end_letter - start_letter + 1) + pad; c++) {
                            int rr = r, cc = start_letter + c;
                            if (rr >= 0 && rr < height && cc >= 0 && cc < width)
                                fprintf(out_fp, "%d ", pixels[rr][cc]);
                            else
                                fprintf(out_fp, "%d ", threshold);
                        }
                        fprintf(out_fp, "\n");
                    }
                    fclose(out_fp);
                }
            }
        }
        letters_per_word[word_index - 1] = current_letter_count;
        current_letter_count = 0;

        for(int row = 0; row < height; row++)
            free(pixels[row]);
        free(pixels);
    }
    return;
}

void rescale_pgm() {
    for (int letter_index = 1; letter_index <= letter_count; letter_index++) {
        char input_file[50];
        char output_file[50];
        sprintf(input_file, "Letters/letter_%d.pgm", letter_index);
        sprintf(output_file, "Letters/letter_%d.pgm", letter_index);

        FILE* fp = fopen(input_file, "r");
        if (!fp) {
            perror("Failed to open input file");
            continue;
        }

        char magic[3];
        int width, height, maxval;

        fscanf(fp, "%2s", magic);
        fscanf(fp, "%d %d", &width, &height);
        fscanf(fp, "%d", &maxval);

        int** input_pixels = malloc(height * sizeof(int*));
        for (int i = 0; i < height; i++) {
            input_pixels[i] = malloc(width * sizeof(int));
            for (int j = 0; j < width; j++) {
                fscanf(fp, "%d", &input_pixels[i][j]);
            }
        }
        fclose(fp);

        int resized[20][20];
        float x_ratio = (float)(width - 1) / 20.0f;
        float y_ratio = (float)(height - 1) / 20.0f;

        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
            float gx = j * x_ratio;
            float gy = i * y_ratio;
            int x = (int)gx;
            int y = (int)gy;
            float dx = gx - x;
            float dy = gy - y;

            int a = input_pixels[y][x];
            int b = (x + 1 < width) ? input_pixels[y][x + 1] : a;
            int c = (y + 1 < height) ? input_pixels[y + 1][x] : a;
            int d = (x + 1 < width && y + 1 < height) ? input_pixels[y + 1][x + 1] : a;

            float pixel = a * (1 - dx) * (1 - dy) +
                      b * dx * (1 - dy) +
                      c * (1 - dx) * dy +
                      d * dx * dy;
            resized[i][j] = (int)(pixel + 0.5f); // Assign the computed pixel value to the resized array
            }
        }

        FILE* fout = fopen(output_file, "w");
        if (!fout) {
            perror("Failed to open output file");
            continue;
        }

        fprintf(fout, "P2\n20 20\n%d\n", maxval);
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                fprintf(fout, "%d ", resized[i][j]);
            }
            fprintf(fout, "\n");
        }
        fclose(fout);

        for (int i = 0; i < height; i++) {
            free(input_pixels[i]);
        }
        free(input_pixels);
    }
}