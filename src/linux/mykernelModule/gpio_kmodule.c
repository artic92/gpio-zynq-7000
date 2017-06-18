/**
* @file gpio_kmodule.c
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
*/
/***************************** Include Files ********************************/
#include <linux/init.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
//#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/device.h> /* class_creatre */
#include <linux/cdev.h> /* cdev_init */
#include <linux/platform_device.h> /*platform_device*/
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

// Indica quante periferiche deve gestire il driver
// (o equivalentemente quanti minor number instanziare per un dato major number)
#define GPIOS_TO_MANAGE   3
#define DRIVER_NAME       "gpiodrv"

#define GPIO_DOUT_OFFSET  0
#define GPIO_DIN_OFFSET   8
#define GPIO_TRI_OFFSET   4
#define GPIO_IER_OFFSET  12
#define GPIO_ICL_OFFSET  16
#define GPIO_ISR_OFFSET  20

// Instanzia una struttura idr
// TODO SPIEGARE COS'è IDR
static DEFINE_IDR(gpio_idr);
static DEFINE_MUTEX(minor_lock);  /*< Mutex per l'accesso all'idr

/**************************** Type Definitions ******************************/
// Struttura dati contenente tutte le informazioni necessarie al driver per la
// gestione delle periferiche
struct gpio_dev_t{
  struct cdev device_cdev;
  struct resource res;
  unsigned long *base_addr;
  struct class *gpio_class;
  dev_t gpio_dev_number;
  unsigned int irq;
  int major;
};

struct gpio_device{
  struct cdev device_cdev;
  struct resource res;
  unsigned long *base_addr;
  dev_t gpio_dev_number;
  unsigned int irq;
};

struct gpio_dev_t* gpio_dev_t_ptr;

/************************** Function Prototypes *****************************/
static int gpio_open(struct inode *, struct file *);
static int gpio_release(struct inode *, struct file *);
ssize_t gpio_read(struct file *, char __user *, size_t, loff_t *);
ssize_t gpio_write(struct file *, const char __user *, size_t, loff_t *);
static irqreturn_t gpio_isr(int irq, struct pt_regs * regs);

// Operazioni supportate dal driver
static struct file_operations gpio_fops = {
		.owner    =   THIS_MODULE,     	/* Owner */
    .read     =   gpio_read,        /* Read method */
    .write    =   gpio_write,       /* Write method */
		.open     =   gpio_open,        /* Open method */
		.release  =   gpio_release      /* Release method */
};

/*
 * @brief Chiamata dal kernel quando esiste un device la cui descrizione nel device-tree
 *    coincide con quella esportata dal modulo attraverso la macro module_platform_driver.
 *
 * @param op
 *
 * @return
 */
static int gpio_probe(struct platform_device *op)
{
  int ret_status;
  unsigned int size;
  struct gpio_device *gpio_device_ptr;

  printk(KERN_INFO "[GPIO driver] Probing device...\n");

  // const struct of_device_id *match;
  // // Controlla che la funzione probe sia stata chimata effettivemente perchè vi
  // // è una compatibilità con la struttura of_device_id dichiarata dal modulo
  // match = of_match_device(gpio_match, &op->dev);
  // if (!match){
  //   printk(KERN_WARNING "Probe chiamata su hardware non compatibile!");
  //   return -EINVAL;
  // }

  gpio_device_ptr = kmalloc(sizeof(struct gpio_device), GFP_KERNEL);
  if(gpio_device_ptr == NULL){
    printk(KERN_WARNING "Allocazione della memoria non riuscita!");
    return -1;
  }

  mutex_lock(&minor_lock);
	ret_status = idr_alloc(&gpio_idr, gpio_device_ptr, 0, GPIOS_TO_MANAGE, GFP_KERNEL);
	if (ret_status == -ENOSPC) {
		printk(KERN_WARNING "Troppi dispositivi GPIO!");
    kfree(gpio_device_ptr);
		return -EINVAL;
	}
	mutex_unlock(&minor_lock);

  gpio_device_ptr->gpio_dev_number = MKDEV(gpio_dev_t_ptr->major, ret_status);

  /********************* Creazione del device file nella cartella /dev ****************/
  if (device_create(gpio_dev_t_ptr->gpio_class, NULL, gpio_device_ptr->gpio_dev_number, NULL, "gpio%d", ret_status) == NULL){
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
    device_destroy(gpio_dev_t_ptr->gpio_class, gpio_device_ptr->gpio_dev_number);

    mutex_lock(&minor_lock);
    idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);

    kfree(gpio_device_ptr);
    return -1;
  }

  // Restituisce informazioni riguardante la parte relativa alle interruzioni.
  // In particolare ricerca il primo tag interrupts e ne restituisce l'irq.
  // Il terzo parametro indica quale tag interrupts considerare (se ce ne sono
  // più di uno). In questo caso viene considerato il primo.
  // Esempio: interrupts = <0 29 4> -> irq_of_parse_and_map restituirà 29
  gpio_device_ptr->irq = irq_of_parse_and_map(op->dev.of_node, 0);

  /*************** Richiesta e registrazione di un interrupt handler *****************/
  // Se il dispositivo supporta le interruzioni
  if(gpio_device_ptr->irq != 0){
    printk(KERN_INFO "[GPIO driver] Gestione dell'interrupt line: %d\n", gpio_device_ptr->irq);

    ret_status = request_irq(gpio_device_ptr->irq, (irq_handler_t) gpio_isr, 0, DRIVER_NAME, NULL);
    if(ret_status){
      printk(KERN_WARNING "Cannot get interrupt line %d\n", gpio_device_ptr->irq);
      device_destroy(gpio_dev_t_ptr->gpio_class, gpio_device_ptr->gpio_dev_number);

      mutex_lock(&minor_lock);
      idr_remove(&gpio_idr, ret_status);
      mutex_unlock(&minor_lock);

      kfree(gpio_device_ptr);
      return ret_status;
    }
  }

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
    device_destroy(gpio_dev_t_ptr->gpio_class, gpio_device_ptr->gpio_dev_number);

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
    if(gpio_device_ptr->irq != 0){
      free_irq(gpio_device_ptr->irq, NULL);
    }
    device_destroy(gpio_dev_t_ptr->gpio_class, gpio_device_ptr->gpio_dev_number);

    mutex_lock(&minor_lock);
    idr_remove(&gpio_idr, ret_status);
    mutex_unlock(&minor_lock);

    kfree(gpio_device_ptr);
    return -ENOMEM;
  }

  printk(KERN_INFO "[GPIO driver] Allocazione e mapping di memoria I/O avvenuta correttamente\n");
  printk(KERN_INFO "[GPIO driver] Probing completato correttamente\n");
  return 0;
}

static int gpio_remove(struct platform_device *op)
{//TODO
  // printk(KERN_INFO "[GPIO driver] Rimozione driver GPIO\n");
  //
  // iounmap(gpio_dev_t_ptr->base_addr);
  // release_mem_region(gpio_dev_t_ptr->res.start, resource_size(&gpio_dev_t_ptr->res));
  //
  // free_irq(gpio_dev_t_ptr->irq, NULL);
  //
  // device_destroy(gpio_dev_t_ptr->gpio_class, gpio_dev_t_ptr->gpio_dev_number);
  //
  // printk(KERN_INFO "[GPIO driver] Rimozione risorse avvenuta correttamente\n");
  return 0;
}

/*
 * @brief Chiamata dal kernel quando un processo apre il device file relativo alla periferica.
 *
 * @param inode è l'inode relativo al device file. Contiene il major ed il minor number associato
 *    associato al driver della periferica.
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 *
 * @return
 */
static int gpio_open(struct inode *inode, struct file *filp)
{
  struct gpio_device* gpio_dev_ptr;

  printk(KERN_INFO "[GPIO driver] Apertura device file\n");
  printk(KERN_INFO "[GPIO driver] Minor number associato al file %i\n", iminor(inode));

  mutex_lock(&minor_lock);
  gpio_dev_ptr = idr_find(&gpio_idr, iminor(inode));
  mutex_unlock(&minor_lock);

  if (gpio_dev_ptr == NULL) {
    printk(KERN_WARNING "[GPIO driver] Non ho trovato il puntatore alla struttura gpio_device!!\n");
    return -ENODEV;
  }

  // Macro che restituisce un puntatore a struct gpio_dev_t partendo dalla struttura
  // cdev che le è associata. Il nome della struct cdev_t, nella struct gpio_dev_t,
  // deve coincidere con quello specificato nel terzo campo
  // gpio_dev_ptr = container_of(inode->i_cdev, struct gpio_dev_t, device_cdev);

  // Associa la scruttura appena ottenuta con il file appena aperto
  filp->private_data = gpio_dev_ptr;
  return 0;
}

/*
 * @brief Chiamata dal kernel quando il device file viene chiuso da tutti i processi che
 *    lo avevano in precedenza aperto.
 *
 * @param inode è l'inode relativo al device file. Contiene il major ed il minor number associato
 *    associato al driver della periferica.
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 *
 * @return
 */
static int gpio_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "[GPIO driver] Rilascio device file\n");
  return 0;
}

/*
 * @brief Chiamata dal kernel ogni volta che si legge dal device file.
 *
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 * @param buf è un puntatore ad un buffer nel quale il processo user-space chiamante andrà a
 *    leggere una volta terminata l'operazione.
 * @param count è la dimensione del buffer buf.
 * @param offp
 *
 * @return
 *
 * @warning L'accesso al buffer non può essere diretto ma mediato attraverso la Funzione
 *    copy_to_user per motivi di portabilità.
 */
ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
  unsigned char valore;
  struct gpio_device* gpio_dev_t_ptr;

  printk(KERN_INFO "[GPIO driver] Richiesta di lettura\n");

  gpio_dev_t_ptr = filp->private_data;

  valore = ioread8(gpio_dev_t_ptr->base_addr + (GPIO_DIN_OFFSET/4));

  if(copy_to_user(buf, &valore, 1) != 0){
    printk(KERN_WARNING "[GPIO driver] Problema nella copia dei dati al processo user-space!\n");
    return -EFAULT;
  }

  printk(KERN_INFO "[GPIO driver] Carettere letto: %08x\n", valore);
  return 1;
}

/*
 * @brief Chiamata dal kernel ogni volta che si scrive sul device file.
 *
 * @param filp è il puntatore ad una struttura struct file che viene creata per ogni processo
 *    che apre il device file.
 * @param buf è un puntatore ad un buffer nel quale il processo user-space ha inserito
 *    i dati da scrivere.
 * @param count è la dimensione del buffer buf.
 * @param offp
 *
 * @return
 */
ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
  unsigned char valore;
  struct gpio_device* gpio_dev_t_ptr;

  printk(KERN_INFO "[GPIO driver] Richiesta di scrittura\n");

  gpio_dev_t_ptr = filp->private_data;

  if(copy_from_user(&valore, buf, 1) != 0){
    printk(KERN_WARNING "[GPIO driver] Problema nella copia dei dati dal processo user-space!\n");
    return -EFAULT;
  }

  iowrite8(0x0F, gpio_dev_t_ptr->base_addr + (GPIO_TRI_OFFSET/4));
  iowrite8(valore, gpio_dev_t_ptr->base_addr + (GPIO_DOUT_OFFSET/4));

  printk(KERN_INFO "[GPIO driver] Valore scritto %08x\n", valore);
  return 1;
}

static irqreturn_t gpio_isr(int irq, struct pt_regs * regs)
{
  printk(KERN_INFO "[GPIO driver] Inizio IRQ handling\n");

  // Acknoledgement delle interruzioni pendenti

  // Sblocca eventuali processi in attesa di leggere
  printk(KERN_INFO "Process %i (%s) awakening the readers...\n", current->pid, current->comm);

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
		.probe = gpio_probe,
		.remove = gpio_remove,
		.driver = {
				.name = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = gpio_match,
		},
};

// module_platform_driver(gpio_driver);

/************************** Funzioni di inizializzazione *************************/
// Queste funzioni effettuano operazioni che devono essere svolte solo all'atto
// dell'inserimento del modulo
static int __init gpio_init(void)
{
  int ret_status;

  printk(KERN_INFO "[GPIO driver] Inzio fase di inizializzazione...");

  // Alloca la struttura dati di gestione del driver
  gpio_dev_t_ptr = kmalloc(sizeof(struct gpio_dev_t), GFP_KERNEL);
  if(gpio_dev_t_ptr == NULL){
    printk(KERN_WARNING "Allocazione della memoria non riuscita!");
    return -1;
  }

  /******************** Registrazione di un device a caratteri ************************/
  // Inizializza una struct cdev_t necessaria al kernel per la gestione del device driver
  // e le associa una struttura file_operations indicante i servizi supportati dal driver
  cdev_init(&gpio_dev_t_ptr->device_cdev, &gpio_fops);
  gpio_dev_t_ptr->device_cdev.owner = THIS_MODULE;

  // Alloca dinamicamente il primo range di device driver numbers diponibile
  // I minor number vanno da 0 a GPIOS_TO_MANAGE-1
  ret_status = alloc_chrdev_region(&gpio_dev_t_ptr->gpio_dev_number, 0, GPIOS_TO_MANAGE-1, DRIVER_NAME);
  if(ret_status < 0){
    printk(KERN_WARNING "Allocazione device numbers non riuscita!");
    kfree(gpio_dev_t_ptr);
    return ret_status;
  }

  gpio_dev_t_ptr->major = MAJOR(gpio_dev_t_ptr->gpio_dev_number);

  // Aggiorna il device driver model
  ret_status = cdev_add(&gpio_dev_t_ptr->device_cdev, gpio_dev_t_ptr->gpio_dev_number, GPIOS_TO_MANAGE-1);
  if(ret_status < 0){
    printk(KERN_WARNING "Registrazione del driver non riuscita!");
    unregister_chrdev_region(gpio_dev_t_ptr->gpio_dev_number, GPIOS_TO_MANAGE-1);
    kfree(gpio_dev_t_ptr);
    return ret_status;
  }


  // Crea una struct class associata al driver
  gpio_dev_t_ptr->gpio_class = class_create(THIS_MODULE, DRIVER_NAME);
  if(!gpio_dev_t_ptr->gpio_class){
    printk(KERN_INFO "Cannot create device class\n");
    cdev_del(&gpio_dev_t_ptr->device_cdev);
    unregister_chrdev_region(gpio_dev_t_ptr->gpio_dev_number, GPIOS_TO_MANAGE-1);
    kfree(gpio_dev_t_ptr);
    return -EFAULT;
  }

  printk(KERN_INFO "[GPIO driver] Registrazione device a caratteri avvenuta correttamente\n");
  printk(KERN_INFO "[GPIO driver] Fine fase di inizializzazione...");
  return platform_driver_register(&gpio_driver);;
}

// static void __exit gpio_exit(void)
// {
//   class_destroy(gpio_dev_t_ptr->gpio_class);
//   cdev_del(&gpio_dev_t_ptr->device_cdev);
//   unregister_chrdev_region(gpio_dev_t_ptr->gpio_dev_number, GPIOS_TO_MANAGE-1);
//   kfree(gpio_dev_t_ptr);
// }

subsys_initcall(gpio_init); // grazie driver gpio xilinx!!!
// subsys_initcall(gpio_exit);

/********************* Informazioni associate al modulo ************************/
MODULE_AUTHOR("Antonio Riccio");
MODULE_DESCRIPTION("Modulo kernel per l'accesso ad una periferica GPIO su Zynq 7000");
MODULE_LICENSE("GPL");
/** @} */
