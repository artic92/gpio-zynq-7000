/******************************************************************************
*
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
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 */

#include <stdio.h>
#include "gpio_se.h"
#include "led_gpio.h"
#include "switch_gpio.h"
#include "config.h"
#include "xscugic.h"

XScuGic gic_inst;

int setup(void);
void loop(void);
void gpio_IRQHandler(void*);

int main()
{

    setup();
    for(;;) loop();

    return 0;
}

int setup()
{
	XScuGic_Config *gic_conf;
	int status;

	led_enable(LED0 | LED1 | LED2 | LED3);
	switch_enable(SWT0 | SWT1 | SWT2 | SWT3);

//	Xil_ExceptionInit();

	gic_conf = XScuGic_LookupConfig(GIC_ID);
	status = XScuGic_CfgInitialize(&gic_inst, gic_conf, gic_conf->CpuBaseAddress);
	if(status != XST_SUCCESS)
		return status;

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, (void*)&gic_inst);
	Xil_ExceptionEnable();

	status = XScuGic_Connect(&gic_inst, GPIO_IRQn, (Xil_InterruptHandler)gpio_IRQHandler, (void*)&gic_inst);
	if(status != XST_SUCCESS)
		return status;

	gpio_int_enable(GPIO_SWITCH_BASEADDR, SWT0 | SWT1 | SWT2 | SWT3);
	XScuGic_Enable(&gic_inst, GPIO_IRQn);
	return XST_SUCCESS;
}

void loop()
{
//	led_on(switch_get_state(SWT0 | SWT1 | SWT2 | SWT3));
//	led_off(~switch_get_state(SWT0 | SWT1 | SWT2 | SWT3));
}


void gpio_IRQHandler(void* data)
{
	// Questa istruzione viene messa da parte per poter considerare lo stato delle interruzioni nell'istante di inizio della ISR
	// Se viene messa più volte, potrebbe accadere che l'interruzione relativa ad un altro pin viene servita qnd invece nn lo è
	uint32_t pending_pins = gpio_get_pending(GPIO_SWITCH_BASEADDR, SWT0 | SWT1 | SWT2 | SWT3);
	led_toggle(pending_pins);
	gpio_clear_int(GPIO_SWITCH_BASEADDR, pending_pins);
}

