/**
* @file config.h
* @author: Antonio Riccio
* @email antonio.riccio.27@gmail.com
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>
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
* @addtogroup gpio
* @{
* @details
*
* Questo file header isola le uniche definizioni dipendenti dal design hardware
* sottostante.
*
*/
#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/***************************** Include Files ********************************/
#include "xparameters.h"

#define GPIO_LED_BASEADDR 		XPAR_GPIO_0_S00_AXI_BASEADDR
#define GPIO_SWITCH_BASEADDR 	XPAR_GPIO_1_S00_AXI_BASEADDR
#define GPIO_BUTTON_BASEADDR  XPAR_GPIO_2_S00_AXI_BASEADDR

#define GIC_ID 					      XPAR_PS7_SCUGIC_0_DEVICE_ID

#define SWT_IRQn 				      XPAR_FABRIC_GPIO_1_IRQ_INTR
#define BTN_IRQn				      XPAR_FABRIC_GPIO_2_IRQ_INTR

#endif /* SRC_CONFIG_H_ */
/** @} */
