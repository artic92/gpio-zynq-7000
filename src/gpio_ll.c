/**
* @file gpio_ll.c
* @brief Implementazione delle funzionalità di accesso di basso livello.
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
* @addtogroup API_LL
* @{
*/
/***************************** Include Files ********************************/
#include <assert.h>
#include "gpio_ll.h"

void gpio_write_mask(uint32_t* gpio_base_ptr, int offset, uint32_t mask){
	*(gpio_base_ptr + offset/4) = mask;
}

uint32_t gpio_read_mask(uint32_t* gpio_base_ptr, int offset){
	return *(gpio_base_ptr + offset/4);
}

void gpio_toggle_bit(uint32_t* gpio_base_ptr, int offset, uint32_t mask){
	gpio_write_mask(gpio_base_ptr, offset, mask^gpio_read_mask(gpio_base_ptr, offset));
}
/** @} */
