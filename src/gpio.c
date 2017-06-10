/**
* @file gpio.c
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
* Implementazione delle funzionalità del device driver per la periferica GPIO.
*
*/
/***************************** Include Files ********************************/
#include "gpio.h"

/**
* Inizializza l'istanza di myGpio_t fornita dal chiamante
* in base alle informazioni presenti nella struttura myGpio_config.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
*   La struttura dati deve essere preventivamente allocata dal chiamante.
*   Ogni successiva chiamata ad una funzione del driver deve essere fatta
*   fornendo questo puntatore.
* @param	config_ptr è un puntatore ad un'istanza di myGpio_config.
*   Il passaggio di questa struttura consente di associare la generica istanza
*   GPIO con un dispositivo specifico. La popolazione della struttura è demandata
*   all'utilizzatore.
*
* @return
* 		- XST_SUCCESS se l'inizializazzione è andata a buon fine.
*     - XST_FAILURE se l'inizializazzione non è andata a buon fine.
*/
int myGpio_init(myGpio_t* instance_ptr, myGpio_config* config_ptr)
{
  int status = XST_FAILURE;
  instance_ptr->isReady = COMPONENT_NOT_READY;

  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  assert(config_ptr != NULL);

  // Popola la struttura dati del device con i dati forniti
  instance_ptr->base_address = (uint32_t*)config_ptr->base_address;
  instance_ptr->interrupt_enabled = config_ptr->interrupt_enabled;

  // Indica che l'istanza è pronta per l'uso, inizializzata senza errori
  instance_ptr->isReady = COMPONENT_READY;
  status = XST_SUCCESS;
  return status;
}

/**
* Imposta la la direzione di input/output per i pin specificati.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param	gpio_pin_mask è una maschera di bit che specifica sui quali pin operare.
* @param	direction è una costante che specifica come devono essere impostati i pin.
*   Se il valore è GPIO_WRITE allora i pin sono configurati in scrittura, se il valore
*   è GPIO_READ i pin sono configurati in lettura.
*
* @return	None.
*
*/
void myGpio_setDataDirection(myGpio_t* instance_ptr, uint32_t gpio_pin_mask, gpio_mode direction)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);

  uint32_t bit_status = gpio_read_mask(instance_ptr->base_address, GPIO_TRI_OFFSET);
  uint32_t bits_to_write = (direction == GPIO_WRITE ? bit_status|gpio_pin_mask : bit_status &~(gpio_pin_mask));

  gpio_write_mask(instance_ptr->base_address, GPIO_TRI_OFFSET, bits_to_write);
}

/**
* Ritorna la direzione input/output per i pin specificati
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param	gpio_pin_mask è una maschera di bit che specifica sui quali pin operare.
*
* @return	Maschera di bit che specifica quali pin sono di input e
*   quali sono di output. I bit settati a 0 sono output mentre bit settati
*   ad 1 sono input.
*
*/
uint32_t myGpio_getDataDirection(myGpio_t* instance_ptr, uint32_t gpio_pin_mask)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);

  return gpio_read_mask(instance_ptr->base_address, GPIO_TRI_OFFSET);
}

/**
* Legge lo stato dei pin per la periferica specificata.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
*
* @return	Contenuto del registro di dato della periferica.
*
*/
uint32_t myGpio_read_value(myGpio_t* instance_ptr)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);

  return gpio_read_mask(instance_ptr->base_address, GPIO_DIN_OFFSET);
}

/**
* Scrive nel registro di uscita per la periferica specificata.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param  data è il valore da scrivere sul registro di uscita.
*
* @return	None.
*
*/
void myGpio_write_value(myGpio_t* instance_ptr, uint32_t data)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);

  gpio_write_mask(instance_ptr->base_address, GPIO_DOUT_OFFSET, data);
}

/**
* Commuta lo stato di uno o più bit nel registro specificato.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param  register_offset è lo spiazzamento necessario a puntare al registro richiesto.
* @param  mask è la maschera di bit che indica i bit da commutare. I bit settati a 1
*   sono commutati mentre bit settati a 0 mantengono lo stato precedente.
*
* @return	None.
*
*/
void myGpio_toggle(myGpio_t* instance_ptr, uint32_t register_offset, uint32_t mask)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);

  gpio_toggle_bit(instance_ptr->base_address, register_offset, mask);
}

/**
* Abilita le interruzioni per i pin specificati.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param	mask è una maschera di bit che specifica i pin per i quali si vogliono abilitare
*   le interruzioni. Se il bit i-esimo è 1 allora saranno abilitate le interruzioni
*   per il pin i-esimo, se il bit è 0 viene preservata la configurazione precedente.
*
* @return	None.
*
*/
void myGpio_interruptEnable(myGpio_t* instance_ptr, uint32_t mask)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(instance_ptr->interrupt_enabled == INT_ENABLED);

  gpio_write_mask(instance_ptr->base_address, GPIO_IER_OFFSET, gpio_read_mask(instance_ptr->base_address, GPIO_IER_OFFSET) | mask);
}

/**
* Disabilita le interruzioni per i pin specificati.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param	mask è una maschera di bit che specifica i pin per i quali si vogliono disabilitare
*   le interruzioni. Se il bit i-esimo è 1 allora saranno disabilitate le interruzioni
*   per il pin i-esimo, se il bit è 0 viene preservata la configurazione precedente.
*
* @return	None.
*
*/
void myGpio_interruptDisable(myGpio_t* instance_ptr, uint32_t mask)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(instance_ptr->interrupt_enabled == INT_ENABLED);

  gpio_write_mask(instance_ptr->base_address, GPIO_IER_OFFSET, gpio_read_mask(instance_ptr->base_address, GPIO_IER_OFFSET) & ~mask);
}

/**
* Libera un interruzione pendente attraverso la maschera fornita.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
* @param	mask è una maschera di bit relativa all'interruzione da liberare.
*   Se il bit i-esimo è 1 l'interruzione è liberata per il pin i-esimo.
*
* @return	None.
*
* @details Questa funzione dovrebbe essere chiamata prima che il
*   software abbia servito l'interruzione.
*/
void myGpio_interruptClear(myGpio_t* instance_ptr, uint32_t mask)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(instance_ptr->interrupt_enabled == INT_ENABLED);

  gpio_write_mask(instance_ptr->base_address, GPIO_ICL_OFFSET, mask);
  gpio_write_mask(instance_ptr->base_address, GPIO_ICL_OFFSET, 0x00000000);
}

/**
* Restituisce la maschera di abilitazione alle interruzioni.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
*
* @return	Contenuto del registro di interrupt enable.
*
*/
uint32_t myGpio_interruptGetEnabled(myGpio_t* instance_ptr)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(instance_ptr->interrupt_enabled == INT_ENABLED);

  return gpio_read_mask(instance_ptr->base_address, GPIO_IER_OFFSET);
}

/**
* Restituisce lo stato dei segnali di interruzione. Qualsiasi bit nella maschera
* settato a 1 indica che il pin associato a quel bit ha asserito una condizione
* di interruzione.
*
* @param	instance_ptr è un puntatore ad un'istanza di myGpio_t.
*
* @return	Contenuto del registro di pending interrupt.
*
* @note
*
* Lo stato dell'interruzione indica lo stato della linea associata al pin indipendentemente dal
* fatto che l'interruzione per quel pin sia stata abilitata o meno.
*
*****************************************************************************/
uint32_t myGpio_interruptGetStatus(myGpio_t* instance_ptr)
{
  // Verifica che il puntatore alla struttura dati non sia nullo
  assert(instance_ptr != NULL);
  // Verifica che il dispositivo è pronto e funzionante
  assert(instance_ptr->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(instance_ptr->interrupt_enabled == INT_ENABLED);

  return gpio_read_mask(instance_ptr->base_address, GPIO_ISR_OFFSET);
}
/** @} */
