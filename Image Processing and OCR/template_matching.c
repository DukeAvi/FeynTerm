#include "template_matching.h"

int valid_ascii[94];
int templates_raw[94][20][20];        // original pixel arrays
Point template_pts[94][MAX_POINTS];   // extracted black-pixel coordinates
int template_pt_count[94];            // how many points per template

// Precompute point lists from each loaded template
void extract_template_points(int idx) {
    int cnt = 0;
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 20; x++) {
            if (templates_raw[idx][y][x] == 0) {
                template_pts[idx][cnt++] = (Point){x, y};
            }
        }
    }
    template_pt_count[idx] = cnt;
}

// Load templates and build point sets
void load_templates() {
    int idx = 0;
    for (int c = 33; c <= 126; c++) valid_ascii[idx++] = c;

    for (int t = 0; t < 94; t++) {
        char fname[64];
        sprintf(fname, "CharacterTemplates/template_ord%d.pgm", valid_ascii[t]);
        FILE *fp = fopen(fname, "r");
        if (!fp) continue;

        char magic[3];
        int w, h, maxv;
        fscanf(fp, "%2s %d %d %d", magic, &w, &h, &maxv);
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                int pix;
                fscanf(fp, "%d", &pix);
                templates_raw[t][y][x] = pix;
            }
        }
        fclose(fp);
        extract_template_points(t);
    }
}

// Compute Modified Hausdorff Distance between point sets A and B
static double modified_hausdorff(Point *A, int na, Point *B, int nb) {
    double sumAB = 0.0, sumBA = 0.0;

    // A → B
    for (int i = 0; i < na; i++) {
        double min_d = DBL_MAX;
        for (int j = 0; j < nb; j++) {
            double dx = A[i].x - B[j].x;
            double dy = A[i].y - B[j].y;
            double d = dx*dx + dy*dy;
            if (d < min_d) min_d = d;
        }
        sumAB += sqrt(min_d);
    }

    // B → A
    for (int i = 0; i < nb; i++) {
        double min_d = DBL_MAX;
        for (int j = 0; j < na; j++) {
            double dx = B[i].x - A[j].x;
            double dy = B[i].y - A[j].y;
            double d = dx*dx + dy*dy;
            if (d < min_d) min_d = d;
        }
        sumBA += sqrt(min_d);
    }

    double avgAB = sumAB / na;
    double avgBA = sumBA / nb;
    return (avgAB > avgBA ? avgAB : avgBA);
}

void template_matching() {
    FILE *out = fopen("output.txt", "r");
    if (!out){
        rename("tmp_resources.txt", "output.txt");
        return;
    }

    int word_idx = 0, char_idx = 0;

    for (int i = 1; i <= letter_count; i++) {
        char fname[64];
        sprintf(fname, "Letters/letters_%d.pgm", i);
        FILE *fp = fopen(fname, "r");
        if (!fp) {
            fprintf(out, "?");
            continue;
        }

        char magic[3];
        int w, h, maxv;
        fscanf(fp, "%2s %d %d %d", magic, &w, &h, &maxv);

        int letter_raw[20][20];
        Point letter_pts[MAX_POINTS];
        int letter_n = 0;

        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                fscanf(fp, "%d", &letter_raw[y][x]);
                if (letter_raw[y][x] == 0 && letter_n < MAX_POINTS) {
                    letter_pts[letter_n++] = (Point){x, y};
                }
            }
        }
        fclose(fp);

        if (letter_n == 0) {
            fprintf(out, "?");
            continue;
        }

        double best_dist = DBL_MAX;
        char best_char = '?';
        for (int t = 0; t < 94; t++) {
            int nt = template_pt_count[t];
            if (nt == 0) continue;
            double d = modified_hausdorff(letter_pts, letter_n,
                                          template_pts[t], nt);
            if (d < best_dist) {
                best_dist = d;
                best_char = (char)valid_ascii[t];
            }
        }

        fprintf(out, "%c", best_dist < 3.5 ? best_char : '?');
        char_idx++;

        if (word_idx < word_count && char_idx >= letters_per_word[word_idx]) {
            fprintf(out, " ");
            word_idx++;
            char_idx = 0;
        }
    }
    fclose(out);
    return;
}

void cleanup() {
    remove("output_1.png");
    remove("output_1.pgm");
    remove("tmp_resources.txt");

    #ifndef _WIN32
    if (system("rm -rf Letters") != 0) printf("Failed to delete Letters folder\n");
    if (system("rm -rf Lines") != 0) printf("Failed to delete Lines folder\n");
    if (system("rm -rf Words") != 0) printf("Failed to delete Words folder\n");
    
    #else
    if (system("rmdir /s /q Letters") != 0) printf("Failed to delete Letters folder\n");
    if (system("rmdir /s /q Lines") != 0) printf("Failed to delete Lines folder\n");
    if (system("rmdir /s /q Words") != 0) printf("Failed to delete Words folder\n");
    #endif
}
