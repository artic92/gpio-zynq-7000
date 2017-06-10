/**
* @file bsp_switch.c
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
* Implementazione delle funzioni di gestione degli switch.
*
*/
/***************************** Include Files *********************************/
#include "bsp_switch.h"

myGpio_t gpio_switch;

/**
 * Inizializza l'hardware di supporto.
 *
 * @param int_config è una costante che configura il supporto alle interruzioni.
 *    Se impostata come INT_ENABLED abilita le interruzioni, se impostata a
 *    INT_DISABLED il supporto alle interruzioni è disabilitato.
 *
 * @return none.
 */
void switch_init(interrupt_support int_config)
{
  myGpio_config gpio_config;
  gpio_config.base_address = GPIO_SWITCH_BASEADDR;
  gpio_config.interrupt_support = int_config;

  myGpio_init(&gpio_switch, &gpio_config);
}

/**
 * Abilita gli switch selezionati.
 *
 * @param swts_to_enable è la maschera di bit indicante gli switch da abilitare.
 *
 * @return none.
 */
void switch_enable(uint32_t swts_to_enable)
{
  myGpio_setDataDirection(&gpio_switch, swts_to_enable, GPIO_READ);
}

/**
 * Lettura dello stato degli switch selezionati.
 *
 * @param mask è la maschera di bit che specifica a quali switch leggere lo stato.
 *
 * @return Maschera di bit indicante la posizione dello switch.
 */
uint32_t switch_get_state(uint32_t mask)
{
  return myGpio_read_value(&gpio_switch) & mask;
}
/** @} */
