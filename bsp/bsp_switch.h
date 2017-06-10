/**
* @file bsp_switch.h
* @author: Antonio Riccio
* @email antonio.riccio.27@gmail.com
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>
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
* @addtogroup gpio
* @{
* @details
*
* Questo file header definisce un board support package per la gestione semplificata
* degli switch attraverso la periferica @ref gpio.
*
*/
#ifndef SRC_BSP_SWT_H_
#define SRC_BSP_SWT_H_

/***************************** Include Files *********************************/
#include <inttypes.h>
#include "config.h"

/**************************** Type Definitions ******************************/
typedef enum
{
  SWITCH_OFF,
  SWITCH_ON
} switch_status;

/************************** Constant Definitions *****************************/
#define SWT0 GPIO_PIN_0
#define SWT1 GPIO_PIN_1
#define SWT2 GPIO_PIN_2
#define SWT3 GPIO_PIN_3

/**
 * Funzione di inizializazzione
 *
 * @note
 *    L'implementazione della funzione di gestione dell'interruzione è demandata
 *    all'utilizzatore. Il nome dell'ISR è SWT_IRQHandler(void).
 */
void switch_init(interrupt_support int_config);

/**
 * Funzioni di configurazione
 */
void switch_enable(uint32_t swts_to_enable);

/**
 * Funzioni per le operazioni di I/O
 */
uint32_t switch_get_state(uint32_t mask);

#endif /* SRC_BSP_SWT_H_ */
/** @} */
