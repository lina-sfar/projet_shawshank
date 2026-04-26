# Makefile pour le LOT 1 (Joueur)

CC = gcc
CFLAGS = -Wall -I.
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
TARGET = test_lot1
SRCS = lot1.c lot1main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
