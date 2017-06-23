/**
* @file MyGpio.h
* @brief Versione C++ del driver per la periferica GPIO.
* @anchor GPIO
* @author: Antonio Riccio, Andrea Scognamiglio, Stefano Sorrentino
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
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
* @addtogroup API_CPP
* @{
* @details
*
* Questo modulo contiene la versione C++ delle API software per il device driver
* della periferica General Purpose I/O (@ref GPIO).
*
*/
/*****************************************************************************/
#ifndef SRC_MYGPIO_H_
#define SRC_MYGPIO_H_

/***************************** Include Files ********************************/
#include <assert.h>
#include <stdint.h>
#include "xstatus.h"
#include "gpio_ll.h"
#include "gpio_defs.h"

class MyGpio {
public:
	MyGpio(uint32_t* base_address, interrupt interrupt_support);

  /**
   * @name Funzioni di configurazione
   * @{
   */
	void setDataDirection(uint32_t gpio_pin_mask, gpio_mode direction);
	uint32_t getDataDirection(uint32_t gpio_pin_mask);
  /* @} */

  /**
   * @name Funzioni per le operazioni di I/O
   * @{
   */
	uint32_t read_value();
	void write_value(uint32_t data);
	void toggle(uint32_t register_offset, uint32_t mask);
  /* @} */

  /**
   * @name Funzioni per la gestione delle interruzioni
   * @{
   */
	void interruptEnable(uint32_t mask);
	void interruptDisable(uint32_t mask);
	void interruptClear(uint32_t mask);
	uint32_t interruptGetEnabled();
	uint32_t interruptGetStatus();
  /* @} */

private:
	uint32_t* base_address;          ///< Indirizzo base della periferica
	enum_ready isReady;              ///< Periferica inizializzata e pronta
	interrupt interrupt_support;     ///< Se è presente il supporto alle interruzioni
};

/**
 * @example MyGpio_test.c
 * @name Funzioni di testing
 */
void MyGpio_test();

#endif /* SRC_MYGPIO_H_ */
