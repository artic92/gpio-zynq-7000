#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	int fd;
	int value = 8;
	char* dev;
	unsigned int info = 0;
	ssize_t nb;

	printf("Driver di prova del gpio kernel module.\n");

	printf("Apro il device file.\n");
	fd = open(argv[0], O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		printf("Invalid device file:%s.\n", dev);
		return -1;
	}

	printf("Effettuo una lettura.\n");
	nb = read(fd, &info, sizeof(info));
	printf("Valore letto: %i", info);

	printf("Writing %d on %s...\n", value, dev);
	write(fd, &value, 4);

	return 0;
}
