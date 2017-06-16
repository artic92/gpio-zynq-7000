all: uio

uio: uio.o
	gcc -o uio uio.o

uio.o: uio.c ../../gpio_ll.h
	gcc -iquote ../../ -c uio.c

clean:
	rm *.o
	rm uio
