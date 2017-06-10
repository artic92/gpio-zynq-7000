/*
 * led_gpio.h
 *
 *  Created on: 03 mag 2017
 *      Author: antonio
 */

/**
 * @brief Driver per il controllo dei LED sulla Zybo mediante GPIO
 */

#ifndef SRC_LED_GPIO_H_
#define SRC_LED_GPIO_H_

#include <inttypes.h>
#include "config.h"

#define LED0 1 << 0
#define LED1 1 << 1
#define LED2 1 << 2
#define LED3 1 << 3

/**
 * Funzione per l'abilitazione
 * @param leds_to_enable maschera di bit indicante il/i LED da abilitare
 */
void led_enable(uint32_t leds_to_enable);

/**
 * Funzione per la disabilitazione
 * @param leds_to_disable maschera di bit indicante il/i LED da disabilitare
 */
void led_disable(uint32_t leds_to_disable);

/**
 * Funzione per l'accensione
 * @param on_leds maschera di bit indicante il/i LED da accendere
 * @details I LED devono essere abilitati prima di utilizzare questa funzione
 */
void led_on(uint32_t on_leds);

/**
 * Funzione per lo spegnimento
 * @param off_leds maschera di bit indicante il/i LED da spegnere
 * @details I LED devono essere abilitati prima di utilizzare questa funzione
 */
void led_off(uint32_t off_leds);

/**
 * Funzione per l'inversione dello stato di funzionamento
 * @param toggle_leds maschera di bit indicante il/i LED da commutare
 * @details I LED devono essere abilitati prima di utilizzare questa funzione
 */
void led_toggle(uint32_t toggle_leds);

#endif /* SRC_LED_GPIO_H_ */
