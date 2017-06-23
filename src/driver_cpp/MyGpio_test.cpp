/**
* @file MyGpio_test.cpp
* @brief Funzione per il testing delle funzionalità della periferica.
* @author: Antonio Riccio, Andrea Scognamiglio, Stefano Sorrentino
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
#include "MyGpio.h"
#include "config.h"

/**
* @brief Esegue un test di funzionamento accendendo e spegnendo i led in base
* allo stato degli switch e dei pulsanti.
*/
void MyGpio_test()
{
	MyGpio gpio_led((uint32_t*)GPIO_LED_BASEADDR, INT_DISABLED);
	MyGpio gpio_switch((uint32_t*)GPIO_SWITCH_BASEADDR, INT_DISABLED);
	MyGpio gpio_button((uint32_t*)GPIO_BUTTON_BASEADDR, INT_DISABLED);

	gpio_led.setDataDirection(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_WRITE);
	gpio_switch.setDataDirection(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_READ);
	gpio_button.setDataDirection(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_READ);

	for(;;){
		gpio_led.write_value(gpio_switch.read_value());
		gpio_led.write_value(gpio_button.read_value());
	}
}
