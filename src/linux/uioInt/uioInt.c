//===- uioInt.c ----------------------------------------------------*- C -*-===//
//
//  Copyright (C) 2017  Mario Barbareschi (mario.barbareschi@unina.it)
//
//  This file is part of Linux Driver: Examples.
//
//  Linux Driver: Examples is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Linux Driver: Examples is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with Linux Driver: Examples. If not, see
//  <https://www.gnu.org/licenses/agpl-3.0.html>.
//
//===----------------------------------------------------------------------===//
/// \file uioInt.cpp
/// \author Mario Barbareschi
/// \brief This application reads/writes GPIO devices by means of a UIO device
/// and handles the interruption offering an ISR example.
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gpio.h"

#define IN 0
#define OUT 1

void usage(void);

int main(int argc, char *argv[]){
	int c;
	int fd;
	int direction=IN;
	char *uiod;
	int value = 0;

	void *ptr;

	uint32_t info = 1; /* unmask */
	ssize_t nb;

	printf("GPIO UIO test.\n");
	while((c = getopt(argc, argv, "d:io:h")) != -1) {
		switch(c) {
		case 'd':
			uiod=optarg;
			break;
		case 'i':
			direction=IN;
			break;
		case 'o':
			direction=OUT;
			value=atoi(optarg);
			break;
		case 'h':
			usage();
			return 0;
		default:
			printf("invalid option: %c\n", (char)c);
			usage();
			return -1;
		}

	}

	/* Open the UIO device file */
	fd = open(uiod, O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		printf("Invalid UIO device file:%s.\n", uiod);
		usage();
		return -1;
	}

	/* mmap the UIO device */
	ptr = mmap(NULL, GPIO_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if (direction == IN) {
	/* Read from GPIO */
		*((unsigned *)(ptr + GPIO_TRI_OFFSET)) = 0;
		for(;;){

			/* Enable Interrupts */
			Gpio_Global_Interrupt(ptr, GLOBAL_INTR_ENABLE);

			/* Wait for interrupt */
			nb = read(fd, &info, sizeof(info));
			if (nb == sizeof(info)) {

				/* Actually, the proper ISR starts here! */
				value = *((unsigned *) (ptr + GPIO_DATAIN_OFFSET));
				printf("%s: input: %08x\n",argv[0], value);
			}

			nb = write(fd, &info, sizeof(info)); //xkè fa write??
			if (nb < sizeof(info)) {
				perror("write");
				close(fd);
				exit(EXIT_FAILURE);
			}

			
			Gpio_Ack_Interrupt(ptr, CHANNEL1_ACK);
		}

	} else {
	/* Write to GPIO */
		*((unsigned *)(ptr + GPIO_TRI_OFFSET)) = 255;

		*((unsigned *)(ptr + GPIO_DATAOUT_OFFSET)) = value;
	}

	munmap(ptr, GPIO_MAP_SIZE);

	return 0;
}


void usage(){
	printf("*argv[0] -d <UIO_DEV_FILE> -i|-o <VALUE>\n");
	printf("    -d               UIO device file. e.g. /dev/uio0");
	printf("    -i               Input from GPIO\n");
	printf("    -o <VALUE>       Output to GPIO\n");
	return;
}
