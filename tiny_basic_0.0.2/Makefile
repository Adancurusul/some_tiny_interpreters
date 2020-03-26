tiny_basic_interpreter.o: tiny_basic_interpreter.c
	gcc -c $^
use.o:use.c tiny_basic_interpreter.h
	gcc -c use.c
run:use.o tiny_basic_interpreter.o
	gcc -o $@ $^

all: use.o tiny_basic_interpreter.o run