/*
 * gpio_se.c
 *
 *  Created on: 03 mag 2017
 *      Author: antonio
 */

#include <assert.h>
#include "gpio_se.h"

void gpio_write_mask(uint32_t* gpio_base_ptr, int offset, uint32_t mask){
	*(gpio_base_ptr + offset/4) = mask;
}

uint32_t gpio_read_mask(uint32_t* gpio_base_ptr, int offset){
	return *(gpio_base_ptr + offset/4);
	//NOTA: offset viene moltiplicato per 4, per questo lo dividiamo per 4 per ottenere il valore originario
}

void gpio_toggle_bit(uint32_t* gpio_base_ptr, int offset, uint32_t mask){
	gpio_write_mask(gpio_base_ptr, offset, mask^gpio_read_mask(gpio_base_ptr, offset));
}
