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

#include "gpio_kmodule_config.h"

/**************************** Type Definitions ******************************/
// Struttura dati necessaria al driver per la gestione della periferica
struct gpio_dev_t{
  dev_t device_numbers;
  struct cdev device_cdev;

  unsigned long *base_addr;
  unsigned int size;

  struct resource res;
  unsigned int remap_size;
};

struct gpio_dev_t* gpio_dev_t_ptr;
static dev_t gpio_dev_number;

static struct class *gpio_class;

/************************** Function Prototypes *****************************/
static int gpio_open(struct inode *, struct file *);
static int gpio_release(struct inode *, struct file *);
static ssize_t gpio_read(struct file *, char __user *, size_t , loff_t *);
static ssize_t gpio_write(struct file *, char __user *, size_t , loff_t *);

// Operazioni supportate dal driver
static struct file_operations gpio_fops = {
		.owner    =   THIS_MODULE,     	/* Owner */
    .read     =   gpio_read,        /* Read method */
    .write    =   gpio_write,       /* Write method */
		.open     =   gpio_open,        /* Open method */
		.release  =   gpio_release      /* Release method */
};

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
int gpio_open(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "[GPIO driver] Apertura device file\n");
  struct gpio_dev_t* gpio_dev_ptr;

  // Macro che restituisce un puntatore a struct gpio_dev_t partendo dalla struttura
  // cdev che le è associata il cui nome, nella struct gpio_dev_t, coincide con quello
  // specificato nel terzo campo
  gpio_dev_ptr = container_of(inode->i_cdev, struct gpio_dev_t, device_cdev);

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
int gpio_release(struct inode *inode, struct file *filp)
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
  printk(KERN_INFO "[GPIO driver] Richiesta di lettura\n");

  struct gpio_dev_t* gpio_dev_t_ptr = filp->private_data;
  unsigned char valore_letto;

  valore_letto = ioread8(gpio_dev_t_ptr->base_addr + (GPIO_DIN_OFFSET/4));

  copy_to_user(buf, &valore_letto, 1);
  printk(KERN_INFO "[GPIO driver] Carettere letto: %08x\n", valore_letto);
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
ssize_t gpio_write(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
  printk(KERN_INFO "[GPIO driver] Funzione write\n");

  // unsigned char dato_letto;
  // copy_from_user(&dato_letto, buf, count);
  // printk(KERN_INFO "[GPIO driver] valore scritto %i\n", dato_letto);
  return count;
}

static int gpio_probe(struct platform_device *op)
{
  printk(KERN_INFO "[GPIO driver] Inizializzazione driver GPIO\n");

  int ret_status;
  struct device *dev = &op->dev;

  // Alloca la struttura dati di gestione del driver
  gpio_dev_t_ptr = kmalloc(sizeof(struct gpio_dev_t), GFP_KERNEL);
  if(gpio_dev_t_ptr == NULL){
    printk(KERN_WARNING "Allocazione della memoria non riuscita!");
    return -1;
  }

  if (of_address_to_resource(dev->of_node, 0, &gpio_dev_t_ptr->res)){
    printk(KERN_INFO "Cannot get device resource\n");
    kfree(gpio_dev_t_ptr);
  }

  /******************** Registrazione di un device a caratteri ************************/

  // Inizializza una struct cdev_t necessaria al kernel per la gestione del device driver
  // e le associa una struttura file_operations indicante i servizi supportati dal driver
  cdev_init(&gpio_dev_t_ptr->device_cdev, &gpio_fops);
  gpio_dev_t_ptr->device_cdev.owner = THIS_MODULE;

  // Alloca dinamicamente il primo range di device driver numbers diponibile
  ret_status = alloc_chrdev_region(&gpio_dev_number, 0, GPIOS_TO_MANAGE, DRIVER_NAME);
  if(ret_status < 0){
    printk(KERN_WARNING "Allocazione device numbers non riuscita!");
    kfree(gpio_dev_t_ptr);
    return ret_status;
  }

  // Aggiorna il device driver model
  ret_status = cdev_add(&gpio_dev_t_ptr->device_cdev, gpio_dev_number, 1);
  if(ret_status < 0){
    printk(KERN_WARNING "Registrazione del driver non riuscita!");
    unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
    kfree(gpio_dev_t_ptr);
    return ret_status;
  }

  printk(KERN_INFO "[GPIO driver] Registrazione device a caratteri avvenuta correttamente\n");

  /********************* Creazione del device file nella cartella /dev ****************/

  gpio_class = class_create(THIS_MODULE, DRIVER_NAME);
  if(!gpio_class){
    printk(KERN_INFO "Cannot create device class\n");
    unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
    kfree(gpio_dev_t_ptr);
    return -EFAULT;
  }

  if (device_create(gpio_class, NULL, gpio_dev_number ,NULL, "gpioDriver") == NULL){
    printk(KERN_INFO "Cannot create device\n");
    class_destroy(gpio_class);
    unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
    kfree(gpio_dev_t_ptr);
    return -EFAULT;
  }

  printk(KERN_INFO "[GPIO driver] Creazione device file avvenuta correttamente\n");

  /********************* Allocazione e mapping di memoria per l'I/O *******************/

  // gpio_dev_t_ptr->res = platform_get_resource(op, IORESOURCE_MEM, 0);
  // if(!gpio_dev_t_ptr->res){
  //   dev_err(&op->dev, "No memory resource\n");
  //   class_destroy(gpio_class);
  //   device_destroy(gpio_class, gpio_dev_number);
  //   unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
  //   kfree(gpio_dev_t_ptr);
  //   return -ENODEV;
  // }

  gpio_dev_t_ptr->size = gpio_dev_t_ptr->res.end - gpio_dev_t_ptr->res.start + 1;

  // Richiede l'allocazione di una certa area di memoria di lunghezza size a partire da res->start
  if(!request_mem_region(gpio_dev_t_ptr->res.start, gpio_dev_t_ptr->size, DRIVER_NAME)){
    printk(KERN_INFO "Cannot gain memory in exclusive way\n");
    class_destroy(gpio_class);
    device_destroy(gpio_class, gpio_dev_number);
    unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
    kfree(gpio_dev_t_ptr);
    return -ENOMEM;
  }

  gpio_dev_t_ptr->remap_size = resource_size(&gpio_dev_t_ptr->res);

  // Effettua il mapping tra gli indirzzi fisici dell'area di memoria di I/O e gli
  // indirizzi virtuali del processo driver
  gpio_dev_t_ptr->base_addr = ioremap(gpio_dev_t_ptr->res.start, gpio_dev_t_ptr->size);
  if (gpio_dev_t_ptr->base_addr == NULL) {
    printk(KERN_INFO "Cannot map virtual address\n");
    release_mem_region(gpio_dev_t_ptr->res.start, gpio_dev_t_ptr->remap_size);
    class_destroy(gpio_class);
    device_destroy(gpio_class, gpio_dev_number);
    unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
    kfree(gpio_dev_t_ptr);
    return -ENOMEM;
  }

  printk(KERN_INFO "[GPIO driver] Allocazione e mapping di memoria I/O avvenuta correttamente\n");
  printk(KERN_INFO "Driver correctly set up\n");
  return 0;
}

static int gpio_remove(struct platform_device *op)
{
  printk(KERN_INFO "[GPIO driver] Rimozione driver GPIO\n");

  iounmap(gpio_dev_t_ptr->base_addr);
  release_mem_region(gpio_dev_t_ptr->res.start, gpio_dev_t_ptr->remap_size);

  class_destroy(gpio_class);
  device_destroy(gpio_class, gpio_dev_number);

  cdev_del(&gpio_dev_t_ptr->device_cdev);
  unregister_chrdev_region(gpio_dev_number, GPIOS_TO_MANAGE);
  kfree(gpio_dev_t_ptr);

  printk(KERN_INFO "[GPIO driver] Rimozione risorse avvenuta correttamente\n");
  return 0;
}

// Struttura dati che identifica il device all'interno del device tree
static struct of_device_id gpio_match[] = {
		{.compatible = "ZynqSwitch"},
		{},
};

MODULE_DEVICE_TABLE(of, gpio_match);

static struct platform_driver gpio_driver = {
		.probe = gpio_probe,
		.remove = gpio_remove,
		.driver = {
				.name = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = gpio_match,
		},
};

module_platform_driver(gpio_driver);

// Informazioni associate al modulo
MODULE_AUTHOR("Antonio Riccio");
MODULE_DESCRIPTION("Modulo kernel per l'accesso ad una periferica GPIO su Zynq 7000");
MODULE_LICENSE("GPL");
/** @} */
