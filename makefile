GCC = gcc -g

all: chell.o
	$(GCC) -o chell.out chell.o

chell.o: chell.c
	$(GCC) -c chell.c

run: chell.out
	./chell.out

clean:
	rm -f *~ *.out *.o *#
