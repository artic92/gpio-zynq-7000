/**
* @file bsp_led.c
* @brief Implementazione delle funzioni di gestione dei LED.
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
* @addtogroup BSP
* @{
*/
/***************************** Include Files *********************************/
#include "bsp_led.h"

myGpio_t gpio_led;

/**
 * @brief Inizializza l'hardware di supporto.
 *
 * @param base_address è l'indirizzo base della periferica. Se specificato come
 *    NULL viene utilizzato l'indirizzo di default.
 *
 * @return none.
 */
void led_init(uint32_t* base_address)
{
  myGpio_config gpio_config;
  gpio_config.base_address = (base_address == NULL ? (uint32_t*)GPIO_LED_BASEADDR : base_address);
  gpio_config.interrupt_config = INT_DISABLED;

  myGpio_init(&gpio_led, &gpio_config);
}

/**
 * @brief Abilita i LED selezionati.
 *
 * @param leds_to_enable è la maschera di bit indicante i LED da abilitare.
 *
 * @return none.
 */
void led_enable(uint32_t leds_to_enable)
{
  myGpio_setDataDirection(&gpio_led, leds_to_enable, GPIO_WRITE);
  myGpio_write_value(&gpio_led, leds_to_enable);
}

/**
 * @brief Disabilita i LED selezionati.
 *
 * @param leds_to_disable è la maschera di bit indicante i LED da disabilitare.
 *
 * @return none.
 */
void led_disable(uint32_t leds_to_disable)
{
  myGpio_setDataDirection(&gpio_led, leds_to_disable, GPIO_READ);
}

/**
 * @brief Accende i LED selezionati.
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
 * @brief Spegne i LED selezionati.
 *
 * @param off_leds è la maschera di bit indicante i LED da spegnere.
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
 * @brief Commuta lo stato dei LED selezionati.
 *
 * @param toggle_leds è la maschera di bit indicante i LED da commutare.
 *
 * @note
 *    Questa funzione deve essere chiamata dopo aver abilitato i LED.
 */
void led_toggle(uint32_t toggle_leds)
{
  myGpio_toggle(&gpio_led, GPIO_DOUT_OFFSET, toggle_leds);
}

/** @} */
