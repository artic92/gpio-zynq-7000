/*
 * gpio_se_ext.c
 *
 *  Created on: 01 giu 2017
 *      Author: antonio
 */

#include "gpio_se_ext.h"

/**
 * Spiazzamenti al registro base da utilizzare per l'accesso ai registri della periferica
 */
#define GPIO_INT_ENABLE 12
#define GPIO_INT_CLEAR 16
#define GPIO_INT_PENDING 20
#define GPIO_INT_CLR_DEF_VAL 0x00000000

void gpio_int_enable(uint32_t* gpio_base_ptr, uint32_t mask)
{
	gpio_write_mask(gpio_base_ptr, GPIO_INT_ENABLE, gpio_read_mask(gpio_base_ptr, GPIO_INT_ENABLE) | mask);
}

void gpio_int_disable(uint32_t* gpio_base_ptr, uint32_t mask)
{
	gpio_write_mask(gpio_base_ptr, GPIO_INT_ENABLE, gpio_read_mask(gpio_base_ptr, GPIO_INT_ENABLE) & ~mask);
}

void gpio_clear_int(uint32_t* gpio_base_ptr, uint32_t mask)
{
	gpio_write_mask(gpio_base_ptr, GPIO_INT_CLEAR, mask);
	gpio_write_mask(gpio_base_ptr, GPIO_INT_CLEAR, GPIO_INT_CLR_DEF_VAL);
}

//void gpio_set_pending(uint32_t* gpio_base_ptr, uint32_t mask)
//{
//	gpio_write_mask(gpio_base_ptr, GPIO_INT_ENABLE, gpio_read_mask(gpio_base_ptr, GPIO_INT_ENABLE) | mask);
//}

uint32_t gpio_get_pending(uint32_t* gpio_base_ptr, uint32_t mask)
{
	return gpio_read_mask(gpio_base_ptr, GPIO_INT_PENDING) & mask;
}
