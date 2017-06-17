all: mmap

mmap: mmap.o
	gcc -o mmap mmap.o

mmap.o: mmap.c ../../gpio_ll.h ../../config.h ../../xparameters.h
	gcc -iquote ../../ -c mmap.c

clean:
	rm *.o
	rm mmap
