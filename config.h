/*
 * config.h
 *
 *  Created on: 08 giu 2017
 *      Author: antonio
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include "xparameters.h"

#define GPIO_LED_BASEADDR 		XPAR_GPIO_0_S00_AXI_BASEADDR
#define GPIO_SWITCH_BASEADDR 	XPAR_GPIO_1_S00_AXI_BASEADDR
#define GPIO_BTN_BASEADDR		  XPAR_GPIO_2_S00_AXI_BASEADDR

#define GIC_ID 					      XPAR_PS7_SCUGIC_0_DEVICE_ID

#define SWT_IRQn 				      XPAR_FABRIC_GPIO_1_IRQ_INTR
#define BTN_IRQn				      XPAR_FABRIC_GPIO_2_IRQ_INTR


#endif /* SRC_CONFIG_H_ */
