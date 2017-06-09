/*
 * gpio_se.h
 *
 *  Created on: 03 mag 2017
 *      Author: antonio
 */

/**
 * @brief Driver per il controllo di una generica periferica GPIO
 */

#ifndef SRC_GPIO_SE_H_
#define SRC_GPIO_SE_H_

#include <inttypes.h>
#include "gpio_se_ext.h"

/**
 * Spiazzamenti al registro base da utilizzare per l'accesso ai registri della periferica
 */
#define GPIO_OUT 0
#define GPIO_ENABLE 4
#define GPIO_IN 8

/**
 * Scrittura di una maschera di bit in uno dei registri della periferica
 * @param gpio_base_ptr puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param offset spiazzamento necessario a puntare al registro corretto
 * @param mask maschera di bit da scrivere
 */
void gpio_write_mask(uint32_t* gpio_base_ptr, int offset, uint32_t mask);

/**
 * Lettura del contenuto di uno dei registri della periferica
 * @param gpio_base_ptr puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param offset spiazzamento necessario a puntare al registro corretto
 * @return contenuto del registro richiesto
 */
uint32_t gpio_read_mask(uint32_t* gpio_base_ptr, int offset);

/**
 * Funzione per la commutazione di uno o più bit di un registro della periferica
 * @param gpio_base_ptr puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param offset spiazzamento necessario a puntare al registro corretto
 * @param mask maschera di bit da commutare
 */
void gpio_toggle_bit(uint32_t* gpio_base_ptr, int offset, uint32_t mask);


#endif /* SRC_GPIO_SE_H_ */
