/**
* @file kmodule.c
* @brief Implementazione del device driver per la periferica GPIO come modulo del Linux kernel.
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
*
* @addtogroup LINUX
* @{
*
* @addtogroup KERNEL_MODULE
* @{
*
* @addtogroup MODULE
* @{
*
* @details In questo modulo è presente la documentazione riguardante il modulo kernel
*   che gestisce la @ref GPIO. Per maggiori dettagli sul significato di ciascuna istruzione
*   consultare il codice sorgente opportunamente commentato.
*/
/***************************** Include Files ********************************/
#include <linux/init.h>
#include <linux/kernel.h>                   /* printk() */
#include <linux/slab.h>                     /* kmalloc() */
#include <linux/fs.h>                       /* everything... */
#include <linux/errno.h>                    /* error codes */
#include <linux/types.h>                    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>                    /* O_ACCMODE */
#include <asm/uaccess.h>                    /* copy_from/to_user */
#include <linux/device.h>                   /* class_create */
#include <linux/cdev.h>                     /* cdev_init */
#include <linux/platform_device.h>          /*platform_device*/
#include <asm/io.h>
#include <linux/unistd.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/poll.h>
#include <linux/of_address.h>
#include <linux/sched.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/idr.h>

/************************** Constant Definitions *****************************/
#define GPIOS_TO_MANAGE   3           ///< Indica quante periferiche deve gestire il driver
                                      ///<  (o equivalentemente quanti minor number
                                      ///<  istanziare per un dato major number)

#define DRIVER_NAME       "gpiodrv"   ///< Nome con il quale il driver si registra presso il kernel

#define NO 0
#define YES 1

#define GPIO_DOUT_OFFSET  0
#define GPIO_DIN_OFFSET   8
#define GPIO_TRI_OFFSET   4
#define GPIO_IER_OFFSET  12
#define GPIO_ICL_OFFSET  16
#define GPIO_ISR_OFFSET  20
#define INT_ENABLE 0x0000000F

/*
 *  La struttura dati idr è utilizzata nel kernel per gestire assegnazioni di identificativi
 *  ad ogetti e consentirne l'indirizzamento attraverso questi identificativi.
 *  Per approfondimenti si veda: https://lwn.net/Articles/103209/
 */
static DEFINE_IDR(gpio_idr);        ///< Istanzia una struttura idr.
static DEFINE_IDR(irq_idr);         ///< Istanzia una struttura idr.

/*
 *  E' necessario serializzare l'accesso alla struttura dati idr
 *  per evitare collisioni, dal momento che il driver può avere più istanze
 *  diverse perchè può gestire più dispositivi contemporaneamente.
 */
static DEFINE_MUTEX(minor_lock);    ///< Crea un semaforo binario per l'accesso alla struttura dati gpio_idr.

// Dati globali a supporto del driver
struct class *gpio_class;
struct cdev *device_cdev_p;
int major;

dev_t gpiodrv_dev_number;   ///< Struttura che conserva i device numbers del driver
spinlock_t read_lock;       ///< Spinlock utilizzato per gestire l'accesso alla variabile CAN_READ
wait_queue_head_t rdqueue;  ///< Wait queue sulla quale i processi si bloccano quando viene richiesta una lettura
int can_read = NO;          ///< Variabile di sincronizzazione tra ISR e processo di lettura

/**************************** Type Definitions ******************************/
/**
 * @brief Struttura dati per la gestione del singolo dispositivo GPIO.
 *
 * @details Il driver crea una struttura di questo tipo per ogni dispositivo
 *    presente nel sistema.
 *
 */
struct gpio_device{
  unsigned long *base_addr; ///< Indirizzo (virtuale) base della periferica
  unsigned int irq;         ///< Numero di interruzione (se la periferica genera interruzioni, altrimenti non è specificato)
  struct resource res;      ///< Struttura dati popolata da informazioni estratte dal device-tree
  dev_t gpiox_dev_number;   ///< Device numbers della periferica (ogni periferica ha un minor number diverso)
  spinlock_t write_lock;    ///< Spinlock per garantire l'accesso in mutua esclusione all'operazione di scrittura
};

/************************** Function Prototypes *****************************/
int gpio_open(struct inode *, struct file *);
int gpio_release(struct inode *, struct file *);
ssize_t gpio_read(struct file *, char __user *, size_t, loff_t *);
ssize_t gpio_write(struct file *, const char __user *, size_t, loff_t *);
irqreturn_t gpio_isr(int irq, struct pt_regs * regs);

/**
 * @brief Operazioni supportate dal driver.
 *
 * @details Ogni driver Linux deve implementare tutti o parte dei metodi dell'interfaccia
 *    del VFS per consentire ai processi user-space di accedere alla periferica.
 *
 */
static struct file_operations gpio_fops = {
		.owner    =   THIS_MODULE,     	///< Proprietario
    .read     =   gpio_read,        ///< Metodo per la lettura
    .write    =   gpio_write,       ///< Metodo per la lettura
		.open     =   gpio_open,        ///< Metodo per l'apertura del device file
		.release  =   gpio_release      ///< Metodo per il rilascio del file aperto legato al device file
};

/**
 * @brief Funzione di probing.
 *
 * @details Chiamata dal kernel quando esiste un device la cui descrizione nel device-tree
 *    coincide con quella esportata dal modulo attraverso la macro module_platform_driver.
 *
 * @param op struttura che contiene informazioni utili all'inizializzazione del device
 *    come ad esempio le informazioni provenineti dal device-tree.
 *
 * @return
 *    - 0 se il procedimento di probing è andato a buon fine.
 *    - errno se il procedimento di probing non è andato a buon fine.
 */
static int gpio_probe(struct platform_device *op)
{
  int ret_status;
  unsigned int size;
  struct gpio_device *gpio_device_ptr;

  printk(KERN_INFO "[GPIO driver] Probing device...\n");

  gpio_device_ptr = kmalloc(sizeof(struct gpio_device), GFP_KERNEL);
  if(!gpio_device_ptr){
    printk(KERN_WARNING "Allocazione della memoria non riuscita!");
    return -1;
  }

  platform_set_drvdata(op, (void*)gpio_device_ptr);
  spin_lock_init(&gpio_device_ptr->write_lock);

  mutex_lock(&minor_lock);
    // Richiede l'allocazione nell'idr del puntatore al dispositivo gpio_device_ptr
    // La funzione restituisce l'identificativo dell'oggetto nella struttura dati
    // L'identificativo è un valore compreso tra 0 e GPIOS_TO_MANAGE
    // Questo identificativo è utilizzato come minor number da assegnare alla periferica
  	ret_status = idr_alloc(&gpio_idr, gpio_device_ptr, 0, GPIOS_TO_MANAGE, GFP_KERNEL);
	mutex_unlock(&minor_lock);

  if (ret_status == -ENOSPC) {
    printk(KERN_WARNING "Richiesti troppi dispositivi di quanti se ne sono allocati!");
    kfree(gpio_device_ptr);
    return -EINVAL;
  }

  // Crea una struttura dev_t con il major number del driver ed il primo minor number disponibile
  gpio_device_ptr->gpiox_dev_number = MKDEV(major, ret_status);

  /********************* Creazione del device file ***********************************/
  if (!device_create(gpio_class, NULL, gpio_device_ptr->gpiox_dev_number, NULL, "gpio%d", ret_status)){
    printk(KERN_INFO "Cannot create device\n");
    mutex_lock(&minor_lock);
      idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);
    kfree(gpio_device_ptr);
    return -EFAULT;
  }

  printk(KERN_INFO "[GPIO driver] Creazione device file avvenuta correttamente\n");

  /******************** Estrazione informazioni dal device-tree ***********************/
  // Popola la struct res del contenuto del tag "reg" del device-tree
  // Esempio: reg = <0x43c00000 0x10000>
  // of_address_to_resource pertanto effettuerà le seguenti azioni:
  // res.start = 0x43c00000 e res.end = 0x43c01000
  if (of_address_to_resource(op->dev.of_node, 0, &gpio_device_ptr->res)){
    printk(KERN_INFO "Cannot get device resource\n");
    device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
    mutex_lock(&minor_lock);
      idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);
    kfree(gpio_device_ptr);
    return -1;
  }

  // Restituisce informazioni riguardante la parte relativa alle interruzioni
  // In particolare ricerca il primo tag interrupts e ne restituisce l'irq
  // Il secondo parametro indica quale tag interrupts considerare (se ce ne sono
  // più di uno). In questo caso viene considerato il primo
  // Esempio: interrupts = <0 29 4> -> irq_of_parse_and_map restituirà 29
  gpio_device_ptr->irq = irq_of_parse_and_map(op->dev.of_node, 0);

  /********************* Allocazione e mapping della memoria I/O *********************/
  // Richiede l'allocazione di una certa area di memoria di grandezza resource_size(&gpio_dev_t_ptr->res)
  // per il driver DRIVER_NAME
  // La macro resource_size restituisce la dimensione del segmento fisico associato alla periferica
  // Esempio: reg = <0x43c00000 0x10000> -> resource_size restituirà 0x10000
  if(!request_mem_region(gpio_device_ptr->res.start, resource_size(&gpio_device_ptr->res), DRIVER_NAME)){
    printk(KERN_INFO "Cannot gain memory in exclusive way\n");
    if(gpio_device_ptr->irq != 0){
      free_irq(gpio_device_ptr->irq, NULL);
    }
    device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
    mutex_lock(&minor_lock);
      idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);
    kfree(gpio_device_ptr);
    return -ENOMEM;
  }

  size = gpio_device_ptr->res.end - gpio_device_ptr->res.start + 1;

  // Effettua il mapping tra il segmento di memoria fisico associato alla periferica
  // e lo spazio virtuale del processo
  // NOTA: la combinazione ioremap + of_address_to_resource è equivalente alla chiamata
  //       di of_iomap(res.start, resource_size(&res))
  gpio_device_ptr->base_addr = ioremap(gpio_device_ptr->res.start, size);
  if (!gpio_device_ptr->base_addr) {
    printk(KERN_INFO "Cannot map virtual address\n");
    release_mem_region(gpio_device_ptr->res.start, resource_size(&gpio_device_ptr->res));
    device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
    mutex_lock(&minor_lock);
      idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);
    kfree(gpio_device_ptr);
    return -ENOMEM;
  }

  /*************** Richiesta e registrazione di un interrupt handler *****************/
  // La registrazione avviene solo se il dispositivo supporta le interruzioni
  if(gpio_device_ptr->irq != 0){
    printk(KERN_INFO "[GPIO driver] Gestione dell'interrupt line: %d\n", gpio_device_ptr->irq);

    // Associa nell'idr la coppia IRQ -> base_addr
    ret_status = idr_alloc(&irq_idr, gpio_device_ptr->base_addr, gpio_device_ptr->irq, gpio_device_ptr->irq+1, GFP_KERNEL);
    printk(KERN_INFO "[GPIO driver] Base address memorizzato con ID: %i\n", ret_status);

    if (ret_status == -ENOSPC) {
      printk(KERN_WARNING "Non è possibile allocare nell'idr il puntatore alla zona fisica di memoria!");
      release_mem_region(gpio_device_ptr->res.start, resource_size(&gpio_device_ptr->res));
      device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
      mutex_lock(&minor_lock);
        idr_remove(&gpio_idr, ret_status);
      mutex_unlock(&minor_lock);
      kfree(gpio_device_ptr);
      return -EINVAL;
    }

    // Registrazione di un ISR per l'irq numero gpio_device_ptr->irq
    ret_status = request_irq(gpio_device_ptr->irq, (irq_handler_t) gpio_isr, 0, DRIVER_NAME, NULL);
    if(ret_status){
      printk(KERN_WARNING "Cannot get interrupt line %d\n", gpio_device_ptr->irq);
      release_mem_region(gpio_device_ptr->res.start, resource_size(&gpio_device_ptr->res));
      device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
      mutex_lock(&minor_lock);
        idr_remove(&gpio_idr, ret_status);
      mutex_unlock(&minor_lock);
      kfree(gpio_device_ptr);
      return ret_status;
    }

    // Abilita le interruzioni nella periferica
    iowrite32(INT_ENABLE, gpio_device_ptr->base_addr + (GPIO_IER_OFFSET/4));
  }

  printk(KERN_INFO "[GPIO driver] Allocazione e mapping di memoria I/O avvenuta correttamente\n");
  printk(KERN_INFO "[GPIO driver] Probing completato!\n");
  return 0;
}

/**
 * @brief Funzione di rimozione.
 *
 * @details Chiamata dal kernel quando un dispositivo non è più presente nel sistema.
 *    Dealloca tutte le strutture dati allocate in precedenza per gestire il particolare device.
 *
 * @param op struttura che contiene informazioni utili all'deallocazione del device
 *
 * @return
 *    - 0 se il procedimento di rimozione è andato a buon fine.
 *    - errno se il procedimento di probing non è andato a buon fine.
 */
static int gpio_remove(struct platform_device *op)
{
  struct gpio_device *gpio_device_ptr;
  int minor_number;

  gpio_device_ptr = platform_get_drvdata(op);
  minor_number = MINOR(gpio_device_ptr->gpiox_dev_number);

  printk(KERN_INFO "[GPIO driver] Rimozione strutture dati per il device %i\n", minor_number);

  iounmap(gpio_device_ptr->base_addr);
  release_mem_region(gpio_device_ptr->res.start, resource_size(&gpio_device_ptr->res));
  if(gpio_device_ptr->irq != 0){
    free_irq(gpio_device_ptr->irq, NULL);
  }
  device_destroy(gpio_class, gpio_device_ptr->gpiox_dev_number);
  mutex_lock(&minor_lock);
    idr_remove(&gpio_idr, minor_number);
  mutex_unlock(&minor_lock);
  kfree(gpio_device_ptr);
  return 0;
}

/**
 * @brief Chiamata dal kernel quando un processo apre il device file relativo alla periferica.
 *
 * @param inode è l'inode relativo al device file. Contiene il major ed il minor number
 *    alla particolare periferica.
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 *
 * @return
 *    - 0 se il procedimento di apertura è andato a buon fine.
 *    - errno se il procedimento di apertura non è andato a buon fine.
 */
int gpio_open(struct inode *inode, struct file *filp)
{
  struct gpio_device* gpio_dev_ptr;

  printk(KERN_INFO "[GPIO driver] Apertura device file...\n");
  printk(KERN_INFO "[GPIO driver] Minor number associato alla periferica %i\n", iminor(inode));

  mutex_lock(&minor_lock);
    // Interroga la struttura dati idr per ottenere il puntatore al device identificato dal suo minor number
    // Il minor number è presente nell'inode del device file
    gpio_dev_ptr = idr_find(&gpio_idr, iminor(inode));
  mutex_unlock(&minor_lock);

  if (!gpio_dev_ptr) {
    printk(KERN_WARNING "[GPIO driver] Puntatore alla struttura gpio_device non trovato!\n");
    return -ENODEV;
  }

  // Associa il puntatore appena ottenuto alla struttura file creata dal kernel
  filp->private_data = gpio_dev_ptr;
  return 0;
}

/**
 * @brief Chiamata dal kernel quando il device file viene chiuso da tutti i processi che
 *    lo avevano in precedenza aperto.
 *
 * @param inode è l'inode relativo al device file. Contiene il major ed il minor number associato
 *    associato al driver della periferica.
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 *
 * @return
 *    - 0 se il procedimento di release è andato a buon fine.
 *    - errno se il procedimento di release non è andato a buon fine.
 */
int gpio_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "[GPIO driver] Rilascio device file\n");
  return 0;
}

/**
 * @brief Chiamata dal kernel ogni volta che un processo legge dal device file. La lettura è bloccante.
 *
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 * @param buf è un puntatore ad un buffer nel quale il processo user-space chiamante andrà a
 *    leggere una volta terminata l'operazione.
 * @param count è la dimensione del buffer buf.
 * @param offp è lo spiazzamento all'interno del file.
 *
 * @return
 *    - count se il procedimento di lettura è andato a buon fine.
 *    - errno se il procedimento di lettura non è andato a buon fine.
 *
 * @note
 *    L'accesso al buffer non può essere diretto ma mediato attraverso la funzione
 *    copy_to_user per motivi di portabilità.
 */
ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
  int ret_status;
  unsigned char valore;
  struct gpio_device* gpio_dev_t_ptr;

  printk(KERN_INFO "[GPIO driver] Richiesta di lettura\n");

  gpio_dev_t_ptr = filp->private_data;

  printk(KERN_DEBUG "Process %i (%s) going to sleep\n", current->pid, current->comm);
  // Attende sulla wait queue il verificarsi di un dato evento, indicato dalla
  // condizione nel secondo parametro
  ret_status = wait_event_interruptible(rdqueue, can_read == YES);
  if(ret_status != 0){
    printk(KERN_DEBUG "Qualche segnale ha interrotto il sonno. Uscita dalla funzione...");
    return -ERESTARTSYS;
  }
  printk(KERN_DEBUG "Awoken %i (%s)\n", current->pid, current->comm);

  // Lettura del dato alla periferica
  valore = ioread8(gpio_dev_t_ptr->base_addr + (GPIO_DIN_OFFSET/4));

  spin_lock_irq(&read_lock);
    can_read = NO;
  spin_unlock_irq(&read_lock);

  // Passaggio al processo user-space del dato appena letto
  if(copy_to_user(buf, &valore, 1) != 0){
    printk(KERN_WARNING "[GPIO driver] Problema nella copia dei dati al processo user-space!\n");
    return -EFAULT;
  }

  printk(KERN_INFO "[GPIO driver] Carettere letto: %08x\n", valore);
  return count;
}

/**
 * @brief Chiamata dal kernel ogni volta che un processo scrive sul device file.
 *
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 * @param buf è un puntatore ad un buffer nel quale il processo user-space ha inserito
 *    i dati da scrivere.
 * @param count è la dimensione del buffer buf.
 * @param offp è lo spiazzamento all'interno del file.
 *
 * @return
 *    - count se il procedimento di scrittura è andato a buon fine.
 *    - errno se il procedimento di scrittura non è andato a buon fine.
 *
 * @note
 *    L'accesso alla periferica è gestito attraverso uno spinlock per garantire
 *    mutua esclusione.
 */
ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
  unsigned char valore;
  struct gpio_device* gpio_dev_t_ptr;
  unsigned long flags;

  printk(KERN_INFO "[GPIO driver] Richiesta di scrittura\n");

  gpio_dev_t_ptr = filp->private_data;

  if(copy_from_user(&valore, buf, 1) != 0){
    printk(KERN_WARNING "[GPIO driver] Problema nella copia dei dati dal processo user-space!\n");
    return -EFAULT;
  }

  // L'accesso alla periferica è gestito in mutua esclusione poichè è
  // una risorsa condivisa tra più processi
  spin_lock_irqsave(&gpio_dev_t_ptr->write_lock, flags);
    iowrite8(0x0F, gpio_dev_t_ptr->base_addr + (GPIO_TRI_OFFSET/4));
    iowrite8(valore, gpio_dev_t_ptr->base_addr + (GPIO_DOUT_OFFSET/4));
  spin_unlock_irqrestore(&gpio_dev_t_ptr->write_lock, flags);

  printk(KERN_INFO "[GPIO driver] Valore scritto %08x\n", valore);
  return count;
}

/**
 * @brief ISR della periferica.
 *
 * @param irq è l'irq number della linea di interruzione.
 * @param regs è un puntatore ad una struttura pt_regs che contiene i valori dei
 *    registri del processore all'atto dell'interruzione.
 *
 * @return
 *    - IRQ_HANDLED se l'interruzione è stata servita correttamente.
 *    - errno se l'interruzione non è stata servita correttamente.
 *
 */
irqreturn_t gpio_isr(int irq, struct pt_regs * regs)
{
  uint32_t pending_interrupt;
  unsigned long* gpio_base_addr_ptr;
  unsigned long flags;

  printk(KERN_INFO "[GPIO driver] Inizio IRQ handling\n");

  gpio_base_addr_ptr = idr_find(&irq_idr, irq);
  if (!gpio_base_addr_ptr) {
    printk(KERN_WARNING "[GPIO driver] Puntatore al base address non trovato!\n");
    return -ENODEV;
  }

  // Acknoledgement delle interruzioni pendenti
  pending_interrupt = ioread32(gpio_base_addr_ptr + (GPIO_ISR_OFFSET/4));
  iowrite32(pending_interrupt, gpio_base_addr_ptr + (GPIO_ICL_OFFSET/4));

  // Sblocca eventuali processi in attesa di leggere
  spin_lock_irqsave(&read_lock, flags);
    can_read = YES;
  spin_unlock_irqrestore(&read_lock, flags);

  printk(KERN_INFO "Process %i (%s) awakening the readers...\n", current->pid, current->comm);
  wake_up_interruptible(&rdqueue);

  printk(KERN_INFO "[GPIO driver] Fine IRQ handling\n");
  return IRQ_HANDLED;
}

/************************** Mapping col device tree ****************************/
// Il driver verrà associato a ciascuna periferica che nel device tree esporrà
// le proprietà espresse nella struttura of_device_id
// NOTA: E' possibile aggiungere più compatibilità, purchè la lista sia terminata
//       da una struct NULL
static struct of_device_id gpio_match[] = {
		{.compatible = "embedded.unina.it,gpiov2.0"},
		{},
};

MODULE_DEVICE_TABLE(of, gpio_match);

// Questa struttura indica al kernel quali funzioni chiamare se esiste, nel device
// tree, dell'hardware compatibile con la struttura di tipo of_device_id
// Nel caso esista una compatibilità, la funzione .probe verrà chiamata per prima
static struct platform_driver gpio_driver = {
		.probe  = gpio_probe,
		.remove = gpio_remove,
		.driver = {
				.name  = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = gpio_match,
		},
};

/********************* Funzioni di inizializzazione del modulo *************************/
/**
 * @brief Operazioni svolte all'atto dell'inserimento del modulo.
 *
 * @return
 *    - 0 se il procedimento è andato a buon fine.
 *    - errno se il procedimento non è andato a buon fine.
 */
static int __init gpio_init(void)
{
  int ret_status;

  printk(KERN_INFO "[GPIO driver] Inzio fase di inizializzazione...");

  /******************** Registrazione di un device a caratteri ************************/
  // Inizializza una struct cdev_t necessaria al kernel per la gestione del device driver
  // e le associa una struttura file_operations indicante i servizi supportati dal driver
  device_cdev_p = cdev_alloc();
  device_cdev_p->ops = &gpio_fops;
  device_cdev_p->owner = THIS_MODULE;

  // Alloca dinamicamente il primo range di device driver numbers diponibile
  // I minor number vanno da 0 a GPIOS_TO_MANAGE
  ret_status = alloc_chrdev_region(&gpiodrv_dev_number, 0, GPIOS_TO_MANAGE, DRIVER_NAME);
  if(ret_status < 0){
    printk(KERN_WARNING "Allocazione device numbers non riuscita!");
    return ret_status;
  }

  // Richiede al kernel il primo major number disponibile
  major = MAJOR(gpiodrv_dev_number);

  // Aggiorna il device driver model con GPIOS_TO_MANAGE dispositivi
  ret_status = cdev_add(device_cdev_p, gpiodrv_dev_number, GPIOS_TO_MANAGE);
  if(ret_status < 0){
    printk(KERN_WARNING "Registrazione del driver non riuscita!");
    unregister_chrdev_region(gpiodrv_dev_number, GPIOS_TO_MANAGE);
    return ret_status;
  }

  /*************** Registrazione class structure (Linux Device Model) ***************/
  // Crea una struct class associata al driver
  gpio_class = class_create(THIS_MODULE, DRIVER_NAME);
  if(!gpio_class){
    printk(KERN_INFO "Cannot create device class\n");
    cdev_del(device_cdev_p);
    unregister_chrdev_region(gpiodrv_dev_number, GPIOS_TO_MANAGE);
    return -EFAULT;
  }

  spin_lock_init(&read_lock);
  init_waitqueue_head(&rdqueue);

  printk(KERN_INFO "[GPIO driver] Fine fase di inizializzazione...");
  // Da questo punto in avanti ogni periferica che risulta compatibile
  // con il driver verrà inizializzata attraverso la funzione probe
  return platform_driver_register(&gpio_driver);
}

/**
 * @brief Operazioni svolte all'atto della rimozione del modulo.
 *
 */
static void __exit gpio_exit(void)
{
  printk(KERN_INFO "[GPIO driver] Deinizializzazione...");

  platform_driver_unregister(&gpio_driver);
  class_destroy(gpio_class);
  cdev_del(device_cdev_p);
  unregister_chrdev_region(gpiodrv_dev_number, GPIOS_TO_MANAGE);
  mutex_destroy(&minor_lock);
  idr_destroy(&gpio_idr);
  idr_destroy(&irq_idr);
}

// Macro che consentono di definire funzioni che vengono chiamate
// all'atto dell'inserimento/rimozione del modulo
// Per approfondimenti si veda : https://stackoverflow.com/questions/15541290/what-is-the-difference-between-module-init-and-subsys-initcall-while-initializin
subsys_initcall(gpio_init);
module_exit(gpio_exit);

/********************* Informazioni associate al modulo ************************/
MODULE_AUTHOR("Antonio Riccio");
MODULE_DESCRIPTION("Modulo kernel per l'accesso ad una periferica GPIO su Zynq 7000");
MODULE_LICENSE("GPL");
/** @} */
/** @} */
/** @} */
