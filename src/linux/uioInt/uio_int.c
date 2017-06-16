/**
* @file uio_int.c
* @brief Applicazione che fa uso di UIO per il controllo della periferica GPIO
*		attraverso il meccaniscmo delle interruzioni.
* @author: Antonio Riccio
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
* This program is free software; you can redistribute it and/or modify it under the terms of the
* GNU General Public License as published by the
* Free Software Foundation; either version 3 of the License, or any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with this program;
* if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* @addtogroup LINUX
* @{
*
* @addtogroup UIO
* @{
*
* @addtogroup UIO_INTERRUPT
* @{
*/
/***************************** Include Files ********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "gpio_ll.h"

#define DEBUG
#define GPIO_MAP_SIZE 0x10000

int fd_led, fd_swt, led_data;
char *uiod_l, *uiod_s;
void *led_base_addr, *swt_base_addr;

/************************** Function Prototypes *****************************/
void setup(void);
void loop(void);

/**
* @details Questa applicazione fa uso del meccanismo delle interruzioni per implementare
* un contatore. Ogni volta che viene alzato uno switch/premuto un pulsante
* il contatore viene incrementato di un valore pari al valore in binario della
* configurazione dei pulsanti o degli switch.
*/
int main(int argc, char *argv[])
{
	uiod_l = argv[1];
	uiod_s = argv[2];

	printf("Per terminare l'applicazione premi CTRL+C\n");

	setup();
	for(;;) loop();

	// Unmapping degli indirizzi fisici della periferiche con quelli
	// virtuali del processo in esecuzione
	munmap(led_base_addr, GPIO_MAP_SIZE);
	munmap(swt_base_addr, GPIO_MAP_SIZE);

	// Chiusura dei file
	close(fd_led);
	close(fd_swt);
	return 0;
}

void setup(void)
{
	#ifdef DEBUG
	printf("[DEBUG] Apertura dei device files...\n");
	#endif

	// Apre il device file relativo ai LED in modalità sola srittura
	fd_led = open(uiod_l, O_RDWR);
	if (fd_led < 1) {
		printf("Apertura device file (%s) non riuscita! Errore: %s\n", uiod_l, strerror(errno));
		printf("Utilizzo del driver: ./uio input_device_path output_device_path.\n Es: ./uio /dev/uio0 /dev/uio1\n");
		exit(-1);
	}

	// Apre il device file relativo agli switch/pulsanti in modalità sola lettura
	fd_swt = open(uiod_s, O_RDWR);
	if (fd_swt < 1) {
		printf("Apertura device file (%s) non riuscita! Errore: %s\n", uiod_s, strerror(errno));
		printf("Utilizzo del driver: ./uio input_device_path output_device_path.\nEs: ./uio /dev/uio0 /dev/uio1\n");
		close(fd_led);
		exit(-1);
	}

	#ifdef DEBUG
	printf("[DEBUG] Mapping degli indirizzi tra indirizzi fisici e virtuali...\n");
	#endif

	// Mappa gli indirizzi fisici della periferiche con quelli virtuali del processo in esecuzione
	// NOTA: questa funzione restituisce indirizzi virtuali DIVERSI a ciascun processo
	//			 che vuole far uso dei medesimi indirizzi fisici. Questo è possibile solo
	//			 se il flag MAP_SHARED è settato.
	led_base_addr = mmap(NULL, GPIO_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_led, 0);
	if(led_base_addr == MAP_FAILED){
		printf("Mapping degli indirizzi per il descrittore %s non riuscito. Errore: %s\n", uiod_l, strerror(errno));
		close(fd_led);
		close(fd_swt);
		exit(-1);
	}

	swt_base_addr = mmap(NULL, GPIO_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_swt, 0);
	if(swt_base_addr == MAP_FAILED){
		printf("Mapping degli indirizzi per il descrittore %s non riuscito. Errore: %s\n", uiod_s, strerror(errno));
		munmap(led_base_addr, GPIO_MAP_SIZE);
		close(fd_led);
		close(fd_swt);
		exit(-1);
	}

	#ifdef DEBUG
	printf("[DEBUG] Configurazione dei device hardware...\n");
	#endif

	// Le GPIO dei LED sono configurate in scrittura
	*((unsigned *)(led_base_addr + GPIO_TRI_OFFSET)) = 0x000F;

	// Le GPIO degli switch/pulsanti sono configurate in lettura
	*((unsigned *)(swt_base_addr + GPIO_TRI_OFFSET)) = 0x0000;
	// Abilitazione del meccanismo delle interruzioni per gli switch/pulsanti
	*((unsigned *)(swt_base_addr + GPIO_IER_OFFSET)) = 0xFFFF;
	*((unsigned *)(swt_base_addr + GPIO_ICL_OFFSET)) = 0xFFFF;

	#ifdef DEBUG
	printf("[DEBUG] Configurazione completata!\n");
	#endif
}

void loop(void)
{
	int swt_status = 0;

	printf("In attesa di leggere...\n");
	// Lettura dello stato degli switch
	if(read(fd_swt, &swt_status, sizeof(swt_status)) < 0){
		printf("Lettura non riuscita. Errore: %s\n", strerror(errno));
		munmap(led_base_addr, GPIO_MAP_SIZE);
		munmap(swt_base_addr, GPIO_MAP_SIZE);
		close(fd_led);
		close(fd_swt);
		exit(-1);
	}
	printf("Il dato è arrivato!\n");

	if (write(fd_swt, &swt_status, sizeof(swt_status)) < sizeof(swt_status)) {
		perror("write");
		close(fd_swt);
		exit(EXIT_FAILURE);
	}

	// Acknoledge delle interruzioni
	*((unsigned *)(swt_base_addr + GPIO_ICL_OFFSET)) = 0xFFFF;

	// Incrementa variabile di conteggio in base allo stato degli switch/pulsanti
	led_data = led_data + swt_status;

	#ifdef DEBUG
	printf("[DEBUG] Stato degli switch %u\n", swt_status);
	#endif

	// Propagazione dello stato degli switch/pulsanti sui LED
	*((unsigned *)(led_base_addr + GPIO_DOUT_OFFSET)) = led_data;
}
/** @} */
/** @} */
/** @} */
