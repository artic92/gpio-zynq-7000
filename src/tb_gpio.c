/**
* @file tb_gpio.c
* @brief Funzione per il testing delle funzionalità della periferica.
* @author: Antonio Riccio
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
* This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 3 of the License, or any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/
/***************************** Include Files ********************************/
#include "gpio.h"
#include "config.h"

/**
* @brief Esegue un test di funzionamento accendendo e spegnendo i led in base
* allo stato degli switch e dei pulsanti.
*/
void tb_gpio(void)
{
  myGpio_t gpio_led;
  myGpio_t gpio_switch;
  myGpio_t gpio_btn;

  myGpio_config gpio_config;
  gpio_config.base_address = (uint32_t*)GPIO_LED_BASEADDR;
  gpio_config.interrupt_config = INT_DISABLED;
  myGpio_init(&gpio_led, &gpio_config);

  gpio_config.base_address = (uint32_t*)GPIO_SWITCH_BASEADDR;
  myGpio_init(&gpio_switch, &gpio_config);

  gpio_config.base_address = (uint32_t*)GPIO_BUTTON_BASEADDR;
  myGpio_init(&gpio_btn, &gpio_config);

  myGpio_setDataDirection(&gpio_led, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_WRITE);
  myGpio_setDataDirection(&gpio_switch, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_READ);
  myGpio_setDataDirection(&gpio_btn, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_READ);

  for(;;){
    myGpio_write_value(&gpio_led, myGpio_read_value(&gpio_switch));
    myGpio_write_value(&gpio_led, myGpio_read_value(&gpio_btn));
  }
}
