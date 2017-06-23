/**
* @file MyGpio.cpp
* @brief Implementazione delle funzionalità del device driver C++ per la periferica GPIO.
* @author: Antonio Riccio, Andrea Scognamiglio, Stefano Sorrentino
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
* @addtogroup API_CPP
* @{
*/
/***************************** Include Files ********************************/
#include "MyGpio.h"

/**
* @brief Costruttore. Inizializza l'ogetto MyGpio.
*
* @param base_address è il puntatore all'indirizzo base della periferica.
* @param interrupt_support è una costante che specifica se la periferica supporta le interruzioni.
*   Se il valore è INT_ENABLED le interruzioni sono supportate dalla periferica, se il valore
*   è INT_DISABLED le interruzioni non sono supportate dalla periferica.
*
*/
MyGpio::MyGpio(uint32_t* base_address, interrupt interrupt_support) {
  this->isReady = COMPONENT_NOT_READY;

  // Verifica l'integrità del puntatore fornito in ingresso
  assert(base_address != NULL);

  // Popola l'oggetto con i dati forniti
  this->base_address = base_address;
  this->interrupt_support = interrupt_support;
  // Indica che l'istanza è pronta per l'uso, inizializzata senza errori
  this->isReady = COMPONENT_READY;
}

/**
* @brief Imposta la la direzione di input/output per i pin specificati.
*
* @param gpio_pin_mask è una maschera di bit che specifica sui quali pin operare.
* @param direction è una costante che specifica come devono essere impostati i pin.
*   Se il valore è GPIO_WRITE allora i pin sono configurati in scrittura, se il valore
*   è GPIO_READ i pin sono configurati in lettura.
*
* @return	None.
*
*/
void MyGpio::setDataDirection(uint32_t gpio_pin_mask, gpio_mode direction)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);

  uint32_t bit_status = gpio_read_mask(this->base_address, GPIO_TRI_OFFSET);
  uint32_t bits_to_write = (direction == GPIO_WRITE ? bit_status|gpio_pin_mask : bit_status &~(gpio_pin_mask));

  gpio_write_mask(this->base_address, GPIO_TRI_OFFSET, bits_to_write);
}

/**
* @brief Ritorna la direzione input/output per i pin specificati
*
* @param gpio_pin_mask è una maschera di bit che specifica sui quali pin operare.
*
* @return	Maschera di bit che specifica quali pin sono di input e
*   quali sono di output. I bit settati a 0 sono di output mentre bit settati
*   ad 1 sono di input.
*
*/
uint32_t MyGpio::getDataDirection(uint32_t gpio_pin_mask)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);

  return gpio_read_mask(this->base_address, GPIO_TRI_OFFSET);
}

/**
* @brief Legge lo stato dei pin per la periferica specificata.
*
* @return	Contenuto del registro di dato della periferica.
*
*/
uint32_t MyGpio::read_value()
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);

  return gpio_read_mask(this->base_address, GPIO_DIN_OFFSET);
}

/**
* @brief Scrive nel registro di uscita per la periferica specificata.
*
* @param data è il valore da scrivere sul registro di uscita.
*
* @return	None.
*
*/
void MyGpio::write_value(uint32_t data)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);

  gpio_write_mask(this->base_address, GPIO_DOUT_OFFSET, data);
}

/**
* @brief Commuta lo stato di uno o più bit nel registro specificato.
*
* @param register_offset è lo spiazzamento necessario a puntare al registro richiesto.
* @param mask è la maschera di bit che indica i bit da commutare. I bit settati a 1
*   sono commutati mentre bit settati a 0 mantengono lo stato precedente.
*
* @return	None.
*
*/
void MyGpio::toggle(uint32_t register_offset, uint32_t mask)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);

  // Verifica che l'offset sia compreso nello spazio di indirizzamento della periferica
  assert(register_offset >= 0);
  assert(register_offset <= 20);

  gpio_toggle_bit(this->base_address, register_offset, mask);
}

/**
* @brief Abilita le interruzioni per i pin specificati.
*
* @param mask è una maschera di bit che specifica i pin per i quali si vogliono abilitare
*   le interruzioni. Se il bit i-esimo è 1 allora saranno abilitate le interruzioni
*   per il pin i-esimo, se il bit è 0 viene preservata la configurazione precedente.
*
* @return	None.
*
*/
void MyGpio::interruptEnable(uint32_t mask)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(this->interrupt_support == INT_ENABLED);

  gpio_write_mask(this->base_address, GPIO_IER_OFFSET, gpio_read_mask(this->base_address, GPIO_IER_OFFSET) | mask);
}

/**
* @brief Disabilita le interruzioni per i pin specificati.
*
* @param mask è una maschera di bit che specifica i pin per i quali si vogliono disabilitare
*   le interruzioni. Se il bit i-esimo è 1 allora saranno disabilitate le interruzioni
*   per il pin i-esimo, se il bit è 0 viene preservata la configurazione precedente.
*
* @return	None.
*
*/
void MyGpio::interruptDisable(uint32_t mask)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(this->interrupt_support == INT_ENABLED);

  gpio_write_mask(this->base_address, GPIO_IER_OFFSET, gpio_read_mask(this->base_address, GPIO_IER_OFFSET) & ~mask);
}

/**
* Libera un interruzione pendente attraverso la maschera fornita.
*
* @param mask è una maschera di bit relativa all'interruzione da liberare.
*   Se il bit i-esimo è 1 l'interruzione è liberata per il pin i-esimo.
*
* @return	None.
*
* @note Questa funzione deve essere chiamata nella routine di servizio dell'interruzione
*   prima di effettuare qualsiasi operazione.
*/
void MyGpio::interruptClear(uint32_t mask)
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(this->interrupt_support == INT_ENABLED);

  gpio_write_mask(this->base_address, GPIO_ICL_OFFSET, mask);
  gpio_write_mask(this->base_address, GPIO_ICL_OFFSET, 0x00000000);
}

/**
* @brief Restituisce la maschera di abilitazione alle interruzioni.
*
* @return	Contenuto del registro di interrupt enable.
*
*/
uint32_t MyGpio::interruptGetEnabled()
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(this->interrupt_support == INT_ENABLED);

  return gpio_read_mask(this->base_address, GPIO_IER_OFFSET);
}

/**
* @brief Restituisce lo stato dei segnali di interruzione. Qualsiasi bit nella maschera
* settato a 1 indica che il pin associato a quel bit ha asserito una condizione
* di interruzione.
*
* @return	Contenuto del registro di pending interrupt.
*
* @note Lo stato dell'interruzione indica lo stato della linea associata
* al pin indipendentemente dal fatto che l'interruzione per quel pin sia stata abilitata o meno.
*
*/
uint32_t MyGpio::interruptGetStatus()
{
  // Verifica che il dispositivo è pronto e funzionante
  assert(this->isReady == COMPONENT_READY);
  // Verifica che il dispositivo supporta le interruzioni
  assert(this->interrupt_support == INT_ENABLED);

  return gpio_read_mask(this->base_address, GPIO_ISR_OFFSET);
}
