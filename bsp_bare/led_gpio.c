/*
 * led_gpio.c
 *
 *  Created on: 03 mag 2017
 *      Author: antonio
 */

#include "led_gpio.h"
#include "gpio_ll.h"

void led_enable(uint32_t leds_to_enable){
	gpio_write_mask(GPIO_LED_BASEADDR, GPIO_TRI_OFFSET, gpio_read_mask(GPIO_LED_BASEADDR, GPIO_TRI_OFFSET) | leds_to_enable);
}

void led_disable(uint32_t leds_to_disable){
	gpio_write_mask(GPIO_LED_BASEADDR, GPIO_TRI_OFFSET, gpio_read_mask(GPIO_LED_BASEADDR, GPIO_TRI_OFFSET) & ~leds_to_disable);
}

void led_on(uint32_t on_leds){
	gpio_write_mask(GPIO_LED_BASEADDR, GPIO_DOUT_OFFSET, gpio_read_mask(GPIO_LED_BASEADDR, GPIO_DOUT_OFFSET) | on_leds);
}

void led_off(uint32_t off_leds){
	gpio_write_mask(GPIO_LED_BASEADDR, GPIO_DOUT_OFFSET, gpio_read_mask(GPIO_LED_BASEADDR, GPIO_DOUT_OFFSET) & ~off_leds);
}

void led_toggle(uint32_t toggle_leds){
	gpio_toggle_bit(GPIO_LED_BASEADDR, GPIO_DOUT_OFFSET, toggle_leds);
}
