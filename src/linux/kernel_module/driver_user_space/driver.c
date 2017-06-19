/**
* @file driver.c
* @brief Applicazione che usa il modulo kernel per il controllo della periferica GPIO.
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
* @addtogroup KERNEL_MODULE
* @{
*
* @addtogroup DRIVER
* @{
*
* @details Questo modulo contiene codice che serve a controllare la periferica GPIO
*		mediante il servizio offerto dal modulo kernel sviluppato.
*/
/** @} */
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

#define DEBUG

int fd_led, fd_swt;
unsigned char led_data = 0;
char *dev_l, *dev_s;
void *led_base_addr, *swt_base_addr;

/************************** Function Prototypes *****************************/
void setup(void);
void loop(void);

/**
*
* @addtogroup LINUX
* @{
*
* @addtogroup KERNEL_MODULE
* @{
*
* @addtogroup DRIVER
* @{
*/
/**
* @details Questa applicazione fa uso del meccanismo delle interruzioni per implementare
* un contatore. Ogni volta che viene alzato uno switch/premuto un pulsante
* il contatore viene incrementato di un valore pari al valore in binario della
* configurazione dei pulsanti o degli switch. Il valore del contatore è rappresentato
* sui LED.
*/
int main(int argc, char *argv[])
{
	dev_l = argv[1];
	dev_s = argv[2];

	printf("Per terminare l'applicazione premi CTRL+C\n");

	setup();
	for(;;) loop();

	// Chiusura dei file
	close(fd_led);
	close(fd_swt);
	return 0;
}

/**
* @brief Configura l'hardware.
*
* @details Apre i descrittori dei device file relativi alle periferiche,
*		mappa gli indirizzi fisici della periferica con gli indirizzi virtuali
*		del processo e configura opportunamente i registri della periferica.
*/
void setup(void)
{
	#ifdef DEBUG
	printf("[DEBUG] Apertura dei device files...\n");
	#endif

	// Apre il device file relativo ai LED
	fd_led = open(dev_l, O_RDWR);
	if (fd_led < 1) {
		printf("Apertura device file (%s) non riuscita! Errore: %s\n", dev_l, strerror(errno));
		printf("Utilizzo del driver: ./driver input_device_path output_device_path.\n Es: ./driver /dev/gpio0 /dev/gpio1\n");
		exit(-1);
	}

	// Apre il device file relativo agli switch/pulsanti
	fd_swt = open(dev_s, O_RDWR);
	if (fd_swt < 1) {
		printf("Apertura device file (%s) non riuscita! Errore: %s\n", dev_s, strerror(errno));
		printf("Utilizzo del driver: ./driver input_device_path output_device_path.\n Es: ./driver /dev/gpio0 /dev/gpio1\n");
		close(fd_led);
		exit(-1);
	}

	#ifdef DEBUG
	printf("[DEBUG] Configurazione completata!\n");
	#endif
}

/**
* @brief Legge il valore degli switch/pulsanti con una chiamata bloccante e riporta
* 	il loro stato sui LED.
*/
void loop(void)
{
	unsigned char swt_status;

	printf("In attesa che il dato sia pronto...\n");

	// Chiamata bloccante
	if(read(fd_swt, &swt_status, sizeof(swt_status)) != sizeof(swt_status)){
		printf("Lettura non riuscita. Errore: %s\n", strerror(errno));
		close(fd_led);
		close(fd_swt);
		exit(EXIT_FAILURE);
	}

	// Incrementa la variabile di conteggio in base allo stato degli switch/pulsanti
	led_data = led_data + swt_status;

	#ifdef DEBUG
	printf("[DEBUG] Stato degli switch %08x\n", swt_status);
	printf("[DEBUG] Stato del conteggio %08x\n", led_data);
	#endif

	if (write(fd_led, &led_data, sizeof(led_data)) < sizeof(led_data)) {
		printf("Scrittura non riuscita. Errore: %s\n", strerror(errno));
		close(fd_led);
		close(fd_swt);
		exit(EXIT_FAILURE);
	}
}
/** @} */
/** @} */
/** @} */
