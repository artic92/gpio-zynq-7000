/**
* @file uio.c
* @brief Applicazione che fa uso di UIO per il controllo della periferica GPIO
*		senza il meccanismo dell interruzioni.
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
* @details In questo modulo sono presenti diversi programmi utili ad illustrare
*		l'utilizzo del kernel Linux per il controllo della periferica @ref GPIO.
*
* @addtogroup UIO
* @{
*
* @details Questo modulo contiene codice che serve a controllare la periferica GPIO
*		utilizzando il servizio Universal Input/Output offerto dal kernel Linux.
*		I driver sono scritti in due varianti che si differenziano per l'uso del
*		meccanismo delle interruzioni.
*
*/
/** @} */
/** @} */
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

int fd_led, fd_swt;
char *uiod_l, *uiod_s;
void *led_base_addr, *swt_base_addr;

/************************** Function Prototypes *****************************/
void setup(void);
void loop(void);

/**
*
* @addtogroup LINUX
* @{
*
* @addtogroup UIO
* @{
*/
/**
* @details Esegue un test di funzionamento accendendo e spegnendo i LED in base
* allo stato degli switch o dei pulsanti.
*/
int main(int argc, char *argv[])
{
	uiod_l = argv[1];
	uiod_s = argv[2];

	printf("Controlla lo stato dei led muovendo gli switch o premendo i pulsanti!\n");
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

/**
* @brief Configura l'hardware.
*
* @details Questa funzione apre i descrittori dei device file relativi alle periferiche
*		controllate dal modulo UIO, mappa gli indirizzi fisici della periferica con
*		gli inidirizzi virtuali del processo che ne richiede i servizi e configura
*		opportunamente la periferica hardware.
*/
void setup(void)
{
	#ifdef DEBUG
	printf("[DEBUG] Apertura dei device files...\n");
	#endif

	// Apre il device file relativo ai LED
	fd_led = open(uiod_l, O_RDWR);
	if (fd_led < 1) {
		printf("Apertura device file (%s) non riuscita! Errore: %s\n", uiod_l, strerror(errno));
		printf("Utilizzo del driver: ./uio input_device_path output_device_path.\n Es: ./uio /dev/uio0 /dev/uio1\n");
		exit(-1);
	}

	// Apre il device file relativo agli switch/pulsanti
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

	#ifdef DEBUG
	printf("[DEBUG] Configurazione completata!\n");
	#endif
}

/**
* @brief Legge il valore degli switch con una chiamata non bloccante e riporta
* 	il loro stato sui LED.
*/
void loop(void)
{
	unsigned swt_status = 0;

	// Lettura dello stato degli switch
	swt_status = *((unsigned *)(swt_base_addr + GPIO_DIN_OFFSET));

	#ifdef DEBUG
	printf("[DEBUG] Stato degli switch %08x\n", swt_status);
	#endif

	// Propagazione dello stato degli switch/pulsanti sui LED
	*((unsigned *)(led_base_addr + GPIO_DOUT_OFFSET)) = swt_status;
}
/** @} */
/** @} */
