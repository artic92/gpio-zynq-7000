all: driver

driver: driver.o
	gcc -o driver driver.o

driver.o: driver.c
	gcc -c driver.c

clean:
	rm *.o
	rm driver
