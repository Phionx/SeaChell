GCC = gcc -g
all: compile

compile: chell.o
	$(GCC) -o chell.out chell.o

chell.o: chell.c
	$(GCC) -c chell.c

run: compile
	./chell.out

clean:
	rm -f *~ *.out *.o *#
