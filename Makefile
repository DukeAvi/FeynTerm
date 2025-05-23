CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = memory_checker
SRCS = main.c text_processor.c ignore_words.c grader/grading.c grader/matching.c
OBJS = $(SRCS:.c=.o)
HEADERS = text_processor.h ascii.h grader/grading.h grader/matching.h

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
