OBJS= \
tiny_basic_interpreter.o \
malloc.o \


tiny_basic_interpreter.o: tiny_basic_interpreter.c
	gcc -c $^
use.o:use.c tiny_basic_interpreter.h
	gcc -c use.c
malloc.o:malloc.c malloc.h
	gcc -c malloc.c
run:use.o $(OBJS)
	gcc -o $@ $^

all: use.o tiny_basic_interpreter.o run