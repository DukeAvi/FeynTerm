# Windows Makefile for FeynTerm OCR Project
# Usage: make (or nmake, or mingw32-make, depending on your setup)

CC = gcc
CFLAGS = -Wall -O2
TARGET = myprogram.exe

SRCS = main.c stb_image.h img_processing.c character_segmentation.c template_matching.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	del /Q *.exe *.out 2>NUL || true

.PHONY: all clean
