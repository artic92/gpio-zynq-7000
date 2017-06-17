/**
* @file gpio_kmodule.c
* @brief Definizioni hardware-dependent necessarie al kernel driver.
* @author: Antonio Riccio
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 3 of the License, or any later version.
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details. You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#ifndef GPIO_KMODULE_CONFIG_H_
#define GPIO_KMODULE_CONFIG_H_

// Indica quante periferiche deve gestire il driver
// (o equivalentemente quanti minor number instanziare per un dato major number)
#define GPIOS_TO_MANAGE 1

#define DRIVER_NAME     "gpio_zynq_driver"

#define GPIO_DOUT_OFFSET  0
#define GPIO_DIN_OFFSET   8
#define GPIO_TRI_OFFSET   4
#define GPIO_IER_OFFSET  12
#define GPIO_ICL_OFFSET  16
#define GPIO_ISR_OFFSET  20

#endif /* GPIO_KMODULE_CONFIG_H_ */
