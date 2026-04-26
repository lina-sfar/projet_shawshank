CC = gcc
CFLAGS = -Wall -I.. -I../lot1 -I../lot2 -I../lot3 -I../lot4 -I../lot5 -I../lot6
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
SRCS = main.c \
       ../lot1/lot1.c \
       ../lot2/lot2.c \
       ../lot3/lot3.c \
       ../lot4/lot4.c \
       ../lot5/lot5.c \
       ../lot6/lot6.c
TARGET = shawshank

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
