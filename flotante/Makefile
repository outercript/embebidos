# Project: flotante
# Makefile created by Dev-C++ 5.3.0.3

CPP      = g++
CC       = gcc

BIN      = flotante.exe
CFLAGS   = $(INCS) -fprofile-arcs -ftest-coverage -Wall
RM       = rm -f

.PHONY: all clean

all: $(BIN)

clean:
	${RM} $(OBJ) $(BIN)

$(BIN): flotante.c
	$(CC) $(CFLAGS) $< -o $(BIN)
