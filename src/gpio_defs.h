/**
* @file gpio_defs.h
* @brief Definizioni utili per la gestione della periferica.
* @author: Antonio Riccio
* @copyright
* Copyright 2017 Antonio Riccio <antonio.riccio.27@gmail.com>, <antonio.riccio9@studenti.unina.it>.
* This program is free software; you can redistribute it and/or modify it under the terms
* of the GNU General Public License as published by the
* Free Software Foundation; either version 3 of the License, or any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with this program;
* if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* @addtogroup API_C
* @{
*/
/*****************************************************************************/
#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

/**
 * @brief Enumerazione che indica la presenza o meno al supporto delle interruzioni.
 *
 * Questa enumerazione deve essere utilizzata nella fase di configurazione della periferica
 * scegliendo in base alle capacità dell'hardware l'opzione appropriata.
 * @see myGpio_init()
 */
typedef enum
{
  INT_ENABLED,          /**< Supporto alle interruzioni presente nell'hardware */
  INT_DISABLED          /**< Supporto alle interruzioni non presente nell'hardware */
} interrupt_support;

/**
 * @brief Enumerazione che indica se il dispositivo è configurato e pronto.
 *
 * Questa enumerazione viene utilizzata dalla funzione init per indicare l'esito
 * dell'operazione di configurazione. Viene utiilizata dagli assert per eseguire
 * controlli preliminari prima di effettuare operazioni sulla periferica.
 */
typedef enum
{
  COMPONENT_READY,      /**< Componente pronto e configurato correttamente */
  COMPONENT_NOT_READY   /**< Componente non pronto e non configurato correttamente */
} enum_ready;

/**
 * @brief Enumerazione utile alla configurazione dei pin.
 *
 * Questa enumerazione deve essere utilizzata
 * per indicare la direzione di input/output dei pin specificati.
 * @see myGpio_setDataDirection()
 */
typedef enum
{
  GPIO_READ,            /**< Configurazione del pin in lettura */
  GPIO_WRITE            /**< Configurazione del pin in scrittura */
} gpio_mode;

/************************** Constant Definitions *****************************/
/**
 * @name Definizioni
 * @brief Maschere relative ai pin della periferica.
 * @{
 */
#define GPIO_PIN_0  (uint32_t) 1 << 0
#define GPIO_PIN_1  (uint32_t) 1 << 1
#define GPIO_PIN_2  (uint32_t) 1 << 2
#define GPIO_PIN_3  (uint32_t) 1 << 3
#define GPIO_PIN_4  (uint32_t) 1 << 4
#define GPIO_PIN_5  (uint32_t) 1 << 5
#define GPIO_PIN_6  (uint32_t) 1 << 6
#define GPIO_PIN_7  (uint32_t) 1 << 7
#define GPIO_PIN_8  (uint32_t) 1 << 8
#define GPIO_PIN_9  (uint32_t) 1 << 9
#define GPIO_PIN_10 (uint32_t) 1 << 10
#define GPIO_PIN_11 (uint32_t) 1 << 11
#define GPIO_PIN_12 (uint32_t) 1 << 12
#define GPIO_PIN_13 (uint32_t) 1 << 13
#define GPIO_PIN_14 (uint32_t) 1 << 14
#define GPIO_PIN_15 (uint32_t) 1 << 15
#define GPIO_PIN_16 (uint32_t) 1 << 16
#define GPIO_PIN_17 (uint32_t) 1 << 17
#define GPIO_PIN_18 (uint32_t) 1 << 18
#define GPIO_PIN_19 (uint32_t) 1 << 19
#define GPIO_PIN_20 (uint32_t) 1 << 20
#define GPIO_PIN_21 (uint32_t) 1 << 21
#define GPIO_PIN_22 (uint32_t) 1 << 22
#define GPIO_PIN_23 (uint32_t) 1 << 23
#define GPIO_PIN_24 (uint32_t) 1 << 24
#define GPIO_PIN_25 (uint32_t) 1 << 25
#define GPIO_PIN_26 (uint32_t) 1 << 26
#define GPIO_PIN_27 (uint32_t) 1 << 27
#define GPIO_PIN_28 (uint32_t) 1 << 28
#define GPIO_PIN_29 (uint32_t) 1 << 29
#define GPIO_PIN_30 (uint32_t) 1 << 30
#define GPIO_PIN_31 (uint32_t) 1 << 31
/* @} */

#endif /* SRC_GPIO_DEFS_H */
/** @} */
