all: intuio

intuio: uio_int.o
	gcc -o intuio uio_int.o

uio_int.o: uio_int.c ../../gpio_ll.h
	gcc -iquote ../../ -c uio_int.c

clean:
	rm *.o
	rm intuio
