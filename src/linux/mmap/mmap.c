/**
* @file mmap.c
* @brief Applicazione che fa uso della system call mmap per il controllo della periferica GPIO
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
* @addtogroup MMAP
* @{
*
* @details Questo modulo contiene codice che serve a controllare la periferica @ref GPIO
*		utilizzando la system call mmap offerta dal kernel Linux.
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

#include "config.h"
#include "bsp_led.h"
#include "bsp_switch.h"
#include "bsp_button.h"

#define DEBUG

enum input_source{
	SWITCH,
	BUTTON
};

enum input_source source;
int fd_mem;
unsigned page_size;
void *led_base_addr, *swt_base_addr;

/************************** Function Prototypes *****************************/
void setup(void);
void loop(void);

/**
*
* @addtogroup MMAP
* @{
*/
/**
* @details Esegue un test di funzionamento accendendo e spegnendo i LED in base
* 	allo stato degli switch o dei pulsanti.
*/
int main(int argc, char *argv[])
{
	//Scelta della fonte di ingresso tra switch o pulsanti
	source = SWITCH;

	printf("Controlla lo stato dei led muovendo gli switch o premendo i pulsanti!\n");
	printf("Per terminare l'applicazione premi CTRL+C\n");

	setup();
	for(;;) loop();

	// Unmapping degli indirizzi fisici della periferica con quelli
	// virtuali del processo
	munmap(led_base_addr, page_size);
	munmap(swt_base_addr, page_size);
	return 0;
}

/**
* @brief Configura l'hardware.
*
* @details Questa funzione apre il descrittore del device file relativo alla memoria
*		(/dev/mem), mappa gli indirizzi fisici della periferica con
*		gli indirizzi virtuali del processo che ne richiede i servizi e configura
*		opportunamente le periferiche hardware.
*/
void setup(void)
{
	unsigned long led_addr = GPIO_LED_BASEADDR;
	unsigned long swt_addr = (source == SWITCH ? GPIO_SWITCH_BASEADDR : GPIO_BUTTON_BASEADDR);
	unsigned page_addr_led, page_addr_swt;

	page_size = sysconf(_SC_PAGESIZE);

	#ifdef DEBUG
	printf("[DEBUG] Fonte di input scelta %i\n", source);
	printf("[DEBUG] Apertura del device file...\n");
	#endif

	// Apre il device file relativo alla memoria
	fd_mem = open("/dev/mem", O_RDWR);
	if (fd_mem < 1) {
		printf("Apertura device file (/dev/mem) non riuscita! Errore: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Calcolo dell'indirizzo della pagina fisica nella quale si trova la periferica
	page_addr_led = (led_addr & (~(page_size-1)));
	page_addr_swt = (swt_addr & (~(page_size-1)));

	#ifdef DEBUG
	printf("[DEBUG] Mapping degli indirizzi tra indirizzi fisici e virtuali...\n");
	printf("[DEBUG] Dimensione della pagina: %08x\n", page_size);
	printf("[DEBUG] Indirizzo della pagina per la periferica LED: %08x\n", page_addr_led);
	printf("[DEBUG] Indirizzo della pagina per la periferica LED: %08x\n", page_addr_swt);
	#endif

	// Mappa gli indirizzi fisici della periferica con quelli virtuali del processo
	// NOTA: questa funzione restituisce indirizzi virtuali DIVERSI a ciascun processo
	//			 che vuole far uso dei medesimi indirizzi fisici. Questo è possibile solo
	//			 se il flag MAP_SHARED è settato.
	led_base_addr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, page_addr_led);
	if(led_base_addr == MAP_FAILED){
		printf("Mapping degli indirizzi per i LED non riuscito. Errore: %s\n", strerror(errno));
		close(fd_mem);
		exit(EXIT_FAILURE);
	}

	swt_base_addr = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, page_addr_swt);
	if(led_base_addr == MAP_FAILED){
		printf("Mapping degli indirizzi per gli switch/pulsanti non riuscito. Errore: %s\n", strerror(errno));
		close(fd_mem);
		exit(EXIT_FAILURE);
	}

	#ifdef DEBUG
	printf("[DEBUG] Configurazione dei device hardware...\n");
	#endif

	// Configurazione dei LED
	led_init((uint32_t*)led_base_addr);
	led_enable(LED0|LED1|LED2|LED3);

	// Configurazione degli switch/pulsanti
	switch_init((uint32_t*)swt_base_addr, INT_DISABLED);
	switch_enable(SWT0|SWT1|SWT2|SWT3);

	#ifdef DEBUG
	printf("[DEBUG] Configurazione completata!\n");
	#endif
}

/**
* @brief Legge il valore degli switch/pulsanti e riporta il loro stato sui LED.
*/
void loop(void)
{
	unsigned swt_status = 0;

	// Lettura dello stato degli switch
	swt_status = switch_get_state(SWT0|SWT1|SWT2|SWT3);

	#ifdef DEBUG
	printf("[DEBUG] Stato degli switch %08x\n", swt_status);
	#endif

	// Propagazione dello stato degli switch/pulsanti sui LED
	led_off(~swt_status);
	led_on(swt_status);
}
/** @} */
