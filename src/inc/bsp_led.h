/**
* @file bsp_led.h
* @brief Definisce un board support package per la gestione semplificata
* dei LED attraverso la periferica @ref GPIO.
* @author: Antonio Riccio
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
* @addtogroup BSP
* @{
*
* @details
*   API che definiscono un basic support package per l'utilizzo semplificato dei
*   LED, switch e bottoni per la board Zybo.
*
*/
#ifndef SRC_BSP_LED_H_
#define SRC_BSP_LED_H_

/***************************** Include Files *********************************/
#include <inttypes.h>
#include <stddef.h>
#include "config.h"
#include "gpio.h"

/************************** Constant Definitions *****************************/
/**
 * @name Definizioni dei pin
 * @{
 */
#define LED0 GPIO_PIN_0
#define LED1 GPIO_PIN_1
#define LED2 GPIO_PIN_2
#define LED3 GPIO_PIN_3
/** @} */

/**
 * @name Funzioni di inizializazzione
 * @{
 */
void led_init(uint32_t* base_address);
/** @} */

/**
 * @name Funzioni di configurazione
 * @{
 */
void led_enable(uint32_t leds_to_enable);
void led_disable(uint32_t leds_to_disable);
/** @} */

/**
 * @name Funzioni per le operazioni di I/O
 * @{
 */
void led_on(uint32_t on_leds);
void led_off(uint32_t off_leds);
void led_toggle(uint32_t toggle_leds);
/** @} */

#endif /* SRC_BSP_LED_H_ */
/** @} */
