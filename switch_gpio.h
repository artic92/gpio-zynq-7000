/*
 * switch_gpio.h
 *
 *  Created on: 04 mag 2017
 *      Author: antonio
 */

/**
 * @brief Driver per il controllo degli switch sulla Zybo mediante GPIO
 */

#ifndef SRC_SWITCH_GPIO_H_
#define SRC_SWITCH_GPIO_H_

#include <inttypes.h>
#include "config.h"

#define SWT0 1 << 0
#define SWT1 1 << 1
#define SWT2 1 << 2
#define SWT3 1 << 3

/**
 * Funzione per l'abilitazione
 * @param swts_to_enable maschera di bit indicante il/gli switch da abilitare
 */
void switch_enable(uint32_t swts_to_enable);

/**
 * Funzione per la disabilitazione
 * @param swts_to_disable maschera di bit indicante il/gli switch da disabilitare
 */
void switch_disable(uint32_t swts_to_disable);

/**
 * Funzione per la raccolta dello stato di funzionamento
 * @param mask maschera di bit indicante il/gli switch da controllare
 * @return stato degli switch richiesti restituito come maschera di bit
 */
uint32_t switch_get_state(uint32_t mask);

#endif /* SRC_SWITCH_GPIO_H_ */
