/**
* @file gpio.h
* @author: Antonio Riccio
* @email antonio.riccio.27@gmail.com
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>
* This program is free software; you can redistribute it and/or modify it under the terms of the
* GNU General Public License as published by the
* Free Software Foundation; either version 3 of the License, or any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
* even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with this program; if not,
* write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* @addtogroup gpio
* @{
* @details
*
* Questo file contiene le definizioni delle API software per il device driver
* della periferica General Purpose I/O (GPIO).
*
* Il controllore GPIO realizzato è un IP core che fornisce le seguenti caratteristiche:
*   - Supporto fino a 32 pin di I/O
*   - Ogni pin può essere configurato sia in input che in output
*   - Supporto configurabile alle interruzioni
*
* Il driver fornisce funzioni di gestione delle interruzioni. Tuttavia l'implementazione
* della routine di servizio dell'interruzione è lasciata all'utilizzatore.
*
* Questo driver è stato realizzato per essere agnostico rispetto al processore e all'RTOS.
* Eventuali necessità di gestione della memoria dinamica, threads o mutua esclusione
* tra thread e memoria virtuale devono essere soddisfatti dai livelli superiori a questo driver.
*
* @note
*
* Questa API effettua operazioni di I/O a 32 bit verso i registri della periferica.
* Nel caso si utilizzino meno di 32 bit, i bit non utlizzati verranno
* letti come zero e scritti come don't care.
*
*/
/*****************************************************************************/
#ifndef GPIO_H
#define GPIO_H

/***************************** Include Files ********************************/
#include <assert.h>
#include <inttypes.h>
#include "xstatus.h"
#include "gpio_ll.h"
#include "gpio_defs.h"

/**************************** Type Definitions ******************************/
/**
 * Questa struttura contiene informazioni di configurazione.
 */
typedef struct {
	uint32_t* base_address;	   							/* Indirizzo base della periferica */
	interrupt_support interrupt_enabled;	  /* Interruzioni presenti in hw */
} myGpio_config;

/**
 * Struttura dati del GPIO driver. L'utilizzatore deve allocare una struttura di
 * questo tipo per ogni periferica GPIO che intende gestire. Questo poichè le
 * funzioni nell'API richiedono un puntatore ad una variabile di questo tipo.
 */
typedef struct {
	uint32_t* base_address;	 								/* Indirizzo base della periferica */
	enum_ready isReady;		         								/* periferica inizializzata e pronta */
	interrupt_support interrupt_enabled;	 	/* Interruzioni presenti in hw */
} myGpio_t;

/************************** Function Prototypes *****************************/
/**
 * Funzione di inizializazzione
 */
int myGpio_init(myGpio_t *instance_ptr, myGpio_config *config_ptr);

/**
 * Funzioni di configurazione
 */
void myGpio_setDataDirection(myGpio_t *instance_ptr, uint32_t gpio_pin_mask, gpio_mode direction);
uint32_t myGpio_getDataDirection(myGpio_t *instance_ptr, uint32_t gpio_pin_mask);

/**
 * Funzioni per le operazioni di I/O
 */
uint32_t myGpio_read_value(myGpio_t *instance_ptr);
void myGpio_write_value(myGpio_t *instance_ptr, uint32_t data);
//TODO FUNZIONE DI toggle

/**
 * Funzioni per la gestione delle interruzioni
 */
void myGpio_interruptEnable(myGpio_t *instance_ptr, uint32_t mask);
void myGpio_interruptDisable(myGpio_t *instance_ptr, uint32_t mask);
void myGpio_interruptClear(myGpio_t *instance_ptr, uint32_t mask);
uint32_t myGpio_interruptGetEnabled(myGpio_t *instance_ptr);
uint32_t myGpio_interruptGetStatus(myGpio_t *instance_ptr);

/**
 * Funzione di testing
 */
void tb_gpio(void);

#endif /* SRC_GPIO_H_ */
/** @} */
