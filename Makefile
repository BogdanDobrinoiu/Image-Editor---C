CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = image_editor

SRCS = main.c image_editor.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

pack:
	zip -FSr 315CA_DobrinoiuBogdanMihai_Tema3.zip README Makefile *.c *.h

.PHONY: all clean pack