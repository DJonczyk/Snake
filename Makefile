# Makefile for Snake Game

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = snake

# Platform detection
ifeq ($(OS),Windows_NT)
	LIBS =
	RM = del /Q
	TARGET_EXT = .exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LIBS = -lncurses
	endif
	ifeq ($(UNAME_S),Darwin)
		LIBS = -lncurses
	endif
	RM = rm -f
	TARGET_EXT =
endif

SRC = snake.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET)$(TARGET_EXT) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)$(TARGET_EXT)

run: $(TARGET)
	./$(TARGET)$(TARGET_EXT)

.PHONY: all clean run
