/**
* @file bsp_button.c
* @brief Implementazione delle funzioni di gestione dei bottoni.
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
#include "bsp_button.h"

myGpio_t gpio_button;

/**
 * @brief Inizializza l'hardware di supporto.
 *
 * @param int_config è una costante che configura il supporto alle interruzioni.
 *    Se impostata come INT_ENABLED abilita le interruzioni, se impostata a
 *    INT_DISABLED il supporto alle interruzioni è disabilitato.
 *
 * @note
 *    L'implementazione della funzione di gestione dell'interruzione è demandata
 *    all'utilizzatore. Il nome dell'ISR è BTN_IRQHandler(void).
 *
 * @return none.
 */
void button_init(interrupt int_config)
{
  myGpio_config gpio_config;
  gpio_config.base_address = (uint32_t*)GPIO_BUTTON_BASEADDR;
  gpio_config.interrupt_config = int_config;

  myGpio_init(&gpio_button, &gpio_config);
}

/**
 * @brief Abilita i button selezionati.
 *
 * @param btns_to_enable è la maschera di bit indicante gli button da abilitare.
 *
 * @return none.
 */
void button_enable(uint32_t btns_to_enable)
{
  myGpio_setDataDirection(&gpio_button, btns_to_enable, GPIO_READ);
}

/**
 * @brief Lettura dello stato dei button selezionati.
 *
 * @param mask è la maschera di bit che specifica a quali button leggere lo stato.
 *
 * @return Maschera di bit indicante la posizione dello button.
 */
uint32_t button_get_state(uint32_t mask)
{
  return myGpio_read_value(&gpio_button) & mask;
}
/** @} */
