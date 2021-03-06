/**
* @file main.cc
* @brief Applicazione dimostrativa
* @author: Antonio Riccio, Andrea Scognamiglio, Stefano Sorrentino
* @copyright
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
*/

#include "xscugic.h"
#include "MyGpio.h"
#include "config.h"

XScuGic gic_inst;
MyGpio gpio_led((uint32_t*)GPIO_LED_BASEADDR, INT_DISABLED);
MyGpio gpio_switch((uint32_t*)GPIO_SWITCH_BASEADDR, INT_ENABLED);

static int led_data;

int setup(void);
void loop(void);
void gpio_IRQHandler(void*);

/**
*
* @brief Applicazione di testing per le funzionalità della periferica.
*
* Questa applicazione fa uso del meccanismo delle interruzioni per implementare
* un contatore. Ogni volta che viene alzato uno switch/premuto un pulsante
* il contatore viene incrementato di un valore pari al valore in binario della
* configurazione dei pulsanti o degli switch. L'incremento del conteggio è effettuato
* nella ISR opportunamente definita.
*/
int main()
{
    setup();
    for(;;) loop();

    return 0;
}

/**
* @brief Setup dell'hardware.
*/
int setup()
{
	XScuGic_Config* gic_conf;
	int status;

  // inizializzazione delle periferiche GPIO
  gpio_led.setDataDirection(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_WRITE);
  gpio_switch.setDataDirection(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_READ);

  // Configurazione del GIC
	gic_conf = XScuGic_LookupConfig(GIC_ID);
	status = XScuGic_CfgInitialize(&gic_inst, gic_conf, gic_conf->CpuBaseAddress);
	if(status != XST_SUCCESS)
		return status;

  // Registrazione presso la PS dell'interrupt handler del GIC
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, (void*)&gic_inst);
	Xil_ExceptionEnable();

  // Registrazione presso il GIC della routine di gestione dell'interruzione per la periferica GPIO
	status = XScuGic_Connect(&gic_inst, SWT_IRQn, (Xil_InterruptHandler)gpio_IRQHandler, (void*)&gic_inst);
	if(status != XST_SUCCESS)
		return status;

  // Abilitazione delle interruzioni presso la periferica e presso il GIC
	gpio_switch.interruptEnable(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	gpio_switch.interruptClear(GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	XScuGic_Enable(&gic_inst, SWT_IRQn);
	return XST_SUCCESS;
}

void loop(){}

/**
* @brief ISR per il servizio dell'interruzione.
*/
void gpio_IRQHandler(void* data)
{
	// Ottenimento dello stato dei pin all'inizio dell'IRQ
	uint32_t pending_int = gpio_switch.interruptGetStatus();

  uint32_t swt_data = gpio_switch.read_value();
  led_data = led_data + swt_data;
  gpio_led.write_value(led_data);

	gpio_switch.interruptClear(pending_int);
}
