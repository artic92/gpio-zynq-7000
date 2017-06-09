/*
 * switch_gpio.c
 *
 *  Created on: 04 mag 2017
 *      Author: antonio
 */

#include "switch_gpio.h"
#include "gpio_ll.h"

void switch_enable(uint32_t swts_to_enable){
	gpio_write_mask(GPIO_SWITCH_BASEADDR, GPIO_TRI_OFFSET, gpio_read_mask(GPIO_SWITCH_BASEADDR, GPIO_TRI_OFFSET) & ~swts_to_enable);
}

void switch_disable(uint32_t swts_to_disable){
	gpio_write_mask(GPIO_SWITCH_BASEADDR, GPIO_TRI_OFFSET, gpio_read_mask(GPIO_SWITCH_BASEADDR, GPIO_TRI_OFFSET) | swts_to_disable);
}

uint32_t switch_get_state(uint32_t mask){
	return gpio_read_mask(GPIO_SWITCH_BASEADDR, GPIO_DIN_OFFSET);
}
