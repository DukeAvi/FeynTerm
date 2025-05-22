CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = feynterm
SRCS = main.c text_processor.c ignore_words.c
OBJS = $(SRCS:.c=.o)
HEADERS = text_processor.h ascii.h

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
