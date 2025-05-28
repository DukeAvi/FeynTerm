# OCR SYSTEM DOCUMENTATION

## FILE STRUCTURE
1. **img_processing.h / img_processing.c** – PDF handling & image conversion  
2. **character_segmentation.h / character_segmentation.c** – Line, word & letter segmentation; PGM rescaling  
3. **template_matching.h / template_matching.c** – Template loading & character matching (Hausdorff + spacing)  
4. **main.c** – Orchestrates the full pipeline  

---

## CORE COMPONENTS

### 1. IMAGE PROCESSING MODULE (`img_processing.h` / `img_processing.c`)
**Data & Globals**  
```c
extern const char* input_file;
int file_count;    // Number of PDF pages
```

**Key Functions**  
- `int get_pdf_page_count(const char *pdf_file)`  
  - Uses MuPDF toolchain to count pages  
  - Returns -1 on failure  
- `bool render_pdf_pages_to_png(const char *pdf_file)`  
  - Renders each page via `mutool draw` to `output_<n>.png`  
  - Cross-platform (Windows vs. Unix)  
- `bool png_to_pgm_format(void)`  
  - Converts all `output_*.png` into ASCII PGM (`P2`) files  

---

### 2. SEGMENTATION MODULE (`character_segmentation.h` / `character_segmentation.c`)
**Data & Globals**  
```c
int line_count;             // Extracted lines  
int word_count;             // Extracted words  
int letter_count;           // Extracted letters  
int letters_per_word[5000]; // Letter counts per word  
```

**Key Functions**  
- `void line_segmentation(void)`  
  - Finds horizontal runs of black pixels in each page-PGM  
  - Emits `Lines/line_<n>.pgm` with vertical padding  
- `void word_segmentation(void)`  
  - Splits line-PGMs on blank-column gaps ≥ threshold  
  - Emits `Words/word_<n>.pgm` with bounding-box padding  
- `void letter_segmentation(void)`  
  - Splits each word-PGM into letters on minimal blank-column width  
  - Fills `letters_per_word[]` with count per word  
  - Emits `Letters/letter_<n>.pgm` with side padding  
- `void rescale_pgm(void)`  
  - Bilinearly resizes each letter-PGM to 20×20 (overwrites in P2 format)  

---

### 3. TEMPLATE MATCHING MODULE (`template_matching.h` / `template_matching.c`)
**Data & Globals**  
```c
#define MAX_POINTS 400
typedef struct { int x, y; } Point;
extern int valid_ascii[62];
extern int templates[62][20][20];
extern Point template_pts[62][MAX_POINTS];
extern int template_pt_count[62];
```

**Key Functions**  
- `void load_templates(void)`  
  - Loads 20×20 PGM templates for ASCII 0–9, A–Z, a–z  
  - Builds `template_pts[]` lists of black-pixel coords  
- `double modified_hausdorff(Point *A, int na, Point *B, int nb)`  
  - Computes bidirectional average minimal point distance  
  - Robust to small deformations & noise  
- `void template_matching(void)`  
  1. Reads each `Letters/letter_<n>.pgm` into a 20×20 matrix  
  2. Extracts letter point list  
  3. Finds template with minimal Hausdorff distance  
  4. Inserts spaces using `letters_per_word[]`  
  5. Writes recognized char or `?` to `output.txt`  

---

## MAIN PROGRAM (`main.c`)

**Workflow**  
1. **PDF Processing**  
   ```c
   file_count = get_pdf_page_count(input_file);
   render_pdf_pages_to_png(input_file);
   ```  
2. **Image Conversion**  
   ```c
   png_to_pgm_format();
   ```  
3. **Segmentation**  
   ```c
   line_segmentation();
   word_segmentation();
   letter_segmentation();
   rescale_pgm();
   ```  
4. **OCR Matching**  
   ```c
   load_templates();
   template_matching();
   ```  

---

## MEMORY MANAGEMENT
- Dynamic allocation for PGM pixel arrays in segmentation  
- `free()` loops after each stage to avoid leaks  

---

## ERROR HANDLING
- Checks return values of file I/O & `system()` calls  
- Early returns on failure in each stage  
- Clear error messages for missing files or failed commands  

---

## COMPILATION INSTRUCTIONS
```bash
gcc -std=c11 -O2 -o ocr \
    main.c \
    img_processing.c character_segmentation.c \
    template_matching.c -lm
```
_Windows (MinGW32):_
```bat
gcc -std=c11 -O2 -o ocr.exe ^
    main.c ^
    img_processing.c character_segmentation.c ^
    template_matching.c -lm
```

---

## SAMPLE USAGE
1. Place `input.pdf` in working directory.  
2. Build with `mingw32-make` and run the executable with `.\myprogram.exe`.  
3. Outputs:  
   - `temporary file : output_<n>.png` (rendered pages)  
   - `temporary files and folders` : `page_<n>.pgm`, `Lines/line_<n>.pgm`, `Words/word_<n>.pgm`, `Letters/letter_<n>.pgm`  
   - `output.txt` with recognized text  

---

## CUSTOMIZATION OPTIONS
- Adjust segmentation thresholds in `character_segmentation.c` (not recommended)
- Change padding sizes for lines/words/letters   (not recommended)
- Tune Hausdorff cutoff in `template_matching.c`  
- Add or replace templates in `CharacterTemplates/`  

---

## PERFORMANCE CHARACTERISTICS
- PDF rendering: depends on `mutool` speed  
- PNG→PGM conversion: O(pixels) per image  
- Segmentation: O(width×height) per stage  
- Matching: O(62 × N_points²) per letter (N_points ≤ 400)  

---

## LIMITATIONS
- Assumes uniformly scaled, non-rotated fonts free of any warping in image.
- Cannot operate on handwritten characters. 
- Sensitive to severe noise or skew  
- No support for multi-column layouts