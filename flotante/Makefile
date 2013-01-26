# Project: flotante
# Makefile created by Dev-C++ 5.3.0.3

CPP      = g++
CC       = gcc

OBJ      = main.o $(RES)
LINKOBJ  = main.o $(RES)

LIBS     = -fprofile-arcs
BIN      = flotante.exe
CXXFLAGS = $(CXXINCS) -fprofile-arcs -ftest-coverage -Wall
CFLAGS   = $(INCS) -fprofile-arcs -ftest-coverage -Wall
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)
