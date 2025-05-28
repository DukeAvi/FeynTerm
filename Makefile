CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = memory_checker
SRCS = main.c text_processor.c pdf_processor.c ignore_words.c grader/grading.c grader/matching.c Image_Processing_And_OCR/img_processing.c Image_Processing_And_OCR/character_segmentation.c Image_Processing_And_OCR/template_matching.c 

OBJS = $(SRCS:.c=.o)

HEADERS = text_processor.h pdf_processor.h ascii.h grader/grading.h grader/matching.h stb_image.h Image_Processing_And_OCR/img_processing.h Image_Processing_And_OCR/character_segmentation.h Image_Processing_And_OCR/template_matching.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.csv
