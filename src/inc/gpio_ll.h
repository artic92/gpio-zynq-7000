 /**
 * @file gpio_ll.h
 * @brief Definizioni e prototipi di funzioni di basso livello per l'accesso
 * diretto alla periferica.
 * @author: Antonio Riccio
 * @copyright
 * Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
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
 * @addtogroup API_LL
 * @{
 *
 * @details Funzioni di basso livello per l'accesso diretto alla periferica. Questo livello
 *    è utilizzato dall'API di livello superiore per l'accesso alla periferica.
 */
#ifndef SRC_GPIO_LL_H_
#define SRC_GPIO_LL_H_

/***************************** Include Files *********************************/
#include <inttypes.h>

/************************** Constant Definitions *****************************/
/**
 * @name Registri
 * @brief Spiazzamenti da utilizzare per l'accesso ai registri della periferica
 * @{
 */
#define GPIO_DOUT_OFFSET  0       ///< Registro per i dati in scrittura
#define GPIO_DIN_OFFSET   8       ///< Registro per i dati in lettura
#define GPIO_TRI_OFFSET   4       ///< Registro per il settaggio della modalità in lettura/scrittura
#define GPIO_IER_OFFSET  12       ///< Registro per l'abilitazione alle interruzioni
#define GPIO_ICL_OFFSET  16       ///< Registro per l'acknoledge delle interruzioni
#define GPIO_ISR_OFFSET  20       ///< Registro per la lettura delle interruzioni pending
/* @} */

/**
 * @brief Scrive un valore in un registro della periferica. La scrittura è su 32 bit.
 *
 * @param gpio_base_ptr è il puntatore all'indirizzo base della periferica.
 * @param offset è lo spiazzamento necessario a puntare al registro corretto.
 * @param mask è la maschera di bit da scrivere nel registro.
 *
 * @return none.
 */
void gpio_write_mask(uint32_t* gpio_base_ptr, int offset, uint32_t mask);

/**
 * @brief Legge un valore da un registro della periferica. La lettura è su 32 bit.
 *
 * @param gpio_base_ptr è il puntatore all'indirizzo base della periferica.
 * @param offset è lo spiazzamento necessario a puntare al registro corretto.
 *
 * @return dato letto dal registro richiesto.
 */
uint32_t gpio_read_mask(uint32_t* gpio_base_ptr, int offset);

/**
 * @brief Commuta uno o più bit di un registro della periferica.
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
