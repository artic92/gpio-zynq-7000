/*
 * gpio_se_ext.h
 *
 *  Created on: 01 giu 2017
 *      Author: antonio
 */

#ifndef SRC_GPIO_SE_EXT_H_
#define SRC_GPIO_SE_EXT_H_

#include "gpio_se.h"

/**
 * Questa funzione abilita le interruzioni per i pin specificati
 * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param mask è la maschera di bit dei i pin per i quali si vogliono abilitare
 * le interruzioni. Se il bit i-esimo è 1 allora saranno abilitate le interruzioni
 * se il bit è 0 viene preservata la configurazione precedente.
 */
void gpio_int_enable(uint32_t* gpio_base_ptr, uint32_t mask);

/**
 * Questa funzione disabilita le interruzioni per i pin specificati
 * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param mask è la maschera di bit dei i pin per i quali si vuole disabilitare
 * le interruzioni. Se il bit i-esimo è 1 allora saranno disabilitate le interruzioni
 * se il bit è 0 viene preservata la configurazione precedente.
 */
void gpio_int_disable(uint32_t* gpio_base_ptr, uint32_t mask);

/**
 * Questa funzione libera la/le interrupt pendenti attraverso la maschera fornita
 * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param mask è la maschera di bit relativa all'interruzione da liberare. Se il bit i-esimo è 1 l'interruzione
 * è liberata se il bit è 0 non verrà modificato lo stato precedente dell'interruzione.
 * @details Questa funzione dovrebbe essere chiamata dopo che il software abbia servito le interruzioni in sospeso.
 */
void gpio_clear_int(uint32_t* gpio_base_ptr, uint32_t mask);

///**
// * Questa funzione consente di generare via software un'interrupt per i pin specificati
// * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
// * @param mask mask è la maschera di bit dei i pin per i quali si vogliono abilitare
// * le interruzioni da software. Se il bit i-esimo è 1 queste saranno abilitate
// * se il bit è 0 viene preservata la configurazione precedente.
// * @details L'interrupt per i pin specificati deve essere prima abilitata affichè si possa usare questa funzione.
// */
//void gpio_set_pending(uint32_t* gpio_base_ptr, uint32_t mask);

/** Questa funzione consente di verificare lo stato di servizio di un'interruzione
 * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria assegnata alla periferica
 * @param mask mask è la maschera di bit dei i pin per i quali si vuole sapere lo stato dell'interruzione.
 * Se il bit i-esimo è 1 sarà restiuito lo stato del pin, se il bit è 0 viene restituito sempre 0.
 */
uint32_t gpio_get_pending(uint32_t* gpio_base_ptr, uint32_t mask);


#endif /* SRC_GPIO_SE_EXT_H_ */
