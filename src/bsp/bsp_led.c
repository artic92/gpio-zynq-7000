/**
* @file bsp_led.c
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
* @addtogroup bsp
* @{
* @details
*
* Implementazione delle funzioni di gestione dei LED.
*
*/
/***************************** Include Files *********************************/
#include "bsp_led.h"

myGpio_t gpio_led;

/**
 * Inizializza l'hardware di supporto.
 *
 * @param none.
 *
 * @return none.
 */
void led_init(void)
{
  myGpio_config gpio_config;
  gpio_config->base_address = GPIO_LED_BASEADDR;
  gpio_config->interrupt_support = INT_DISABLED;

  myGpio_init(&gpio_led, &gpio_config);
}

/**
 * Abilita i LED selezionati.
 *
 * @param leds_to_enable è la maschera di bit indicante i LED da abilitare.
 *
 * @return none.
 */
void led_enable(uint32_t leds_to_enable)
{
  myGpio_setDataDirection(&gpio_led, leds_to_enable, GPIO_WRITE);
  myGpio_write_value(&gpio_led, leds_to_enable, 0x00000000);
}

/**
 * Disabilita i LED selezionati.
 *
 * @param leds_to_enable è la maschera di bit indicante i LED da disabilitare.
 *
 * @return none.
 */
void led_disable(uint32_t leds_to_disable)
{
  myGpio_setDataDirection(&gpio_led, leds_to_disable, GPIO_READ);
}

/**
 * Accende i LED selezionati.
 *
 * @param on_leds è la maschera di bit indicante i LED da accendere.
 *
 * @return none.
 *
 * @note
 *    Questa funzione deve essere chiamata dopo aver abilitato i LED.
 */
void led_on(uint32_t on_leds)
{
  myGpio_write_value(&gpio_led, myGpio_read_value(&gpio_led) | on_leds);
}

/**
 * Spegne i LED selezionati.
 *
 * @param on_leds è la maschera di bit indicante i LED da spegnere.
 *
 * @return none.
 *
 * @note
 *    Questa funzione deve essere chiamata dopo aver abilitato i LED.
 */
void led_off(uint32_t off_leds)
{
  myGpio_write_value(&gpio_led, myGpio_read_value(&gpio_led) & ~off_leds);
}

/**
 * Commuta lo stato dei LED selezionati.
 *
 * @param toggle_leds è la maschera di bit indicante i LED da commutare.
 *
 * @note
 *    Questa funzione deve essere chiamata dopo aver abilitato i LED.
 */
void led_toggle(uint32_t toggle_leds)
{
  myGpio_toggle(&gpio_led, toggle_leds);
}

/** @} */
