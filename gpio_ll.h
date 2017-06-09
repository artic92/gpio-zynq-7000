 /**
 *
 * @file gpio_ll.h
 * @addtogroup gpio
 * @{
 *
 * Questo file header contiene definizioni e funzioni di basso livello che possono essere
 * usate per accedere alla periferica.
 *
 */

#ifndef SRC_GPIO_LL_H_
#define SRC_GPIO_LL_H_

/***************************** Include Files *********************************/
#include <inttypes.h>
#include "gpio_se_ext.h"

/************************** Constant Definitions *****************************/
/** @name Registri
 *
 * Spiazzamenti da utilizzare per l'accesso ai registri della periferica
 * @{
 */
#define GPIO_DOUT_OFFSET  0
#define GPIO_DIN_OFFSET   8
#define GPIO_TRI_OFFSET   4
#define GPIO_IER_OFFSET  12
#define GPIO_ICL_OFFSET  16
#define GPIO_ISR_OFFSET  20
/* @} */

/**
 * Scrive un valore in un registro della periferica. La scrittura è su 32 bit.
 *
 * @param gpio_base_ptr è il puntatore all'indirizzo base della periferica.
 * @param offset è lo spiazzamento necessario a puntare al registro corretto.
 * @param mask è la maschera di bit da scrivere nel registro.
 *
 * @return none.
 */
void gpio_write_mask(uint32_t* gpio_base_ptr, int offset, uint32_t mask);

/**
 * Legge un valore da un registro della periferica. La lettura è su 32 bit.
 *
 * @param gpio_base_ptr è il puntatore all'indirizzo base della periferica.
 * @param offset è lo spiazzamento necessario a puntare al registro corretto.
 *
 * @return dato letto dal registro richiesto.
 */
uint32_t gpio_read_mask(uint32_t* gpio_base_ptr, int offset);

/**
 * Commuta uno o più bit di un registro della periferica.
 *
 * @param gpio_base_ptr è il puntatore all'indirizzo base dell'area di memoria
 *    assegnata alla periferica.
 * @param offset è lo spiazzamento necessario a puntare al registro corretto.
 * @param mask è la maschera di bit da commutare. Se viene scritto 1 allora il
 *    il pin commuta se invece viene scritto 0 lo stato del pin rimane invariato.
 *
 * @return none.
 */
void gpio_toggle_bit(uint32_t* gpio_base_ptr, int offset, uint32_t mask);


#endif /* SRC_GPIO_LL_H_ */
/** @} */
