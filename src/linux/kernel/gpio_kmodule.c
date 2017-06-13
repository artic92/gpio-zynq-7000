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
* @addtogroup KERNEL_MODULE
* @{
*/
/***************************** Include Files ********************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include "gpio_kmodule_config.h"

struct gpio_dev_t* gpio_dev_t_ptr;

/**************************** Type Definitions ******************************/
// Struttura dati necessaria al driver per la gestione della periferica
struct gpio_dev_t{
  dev_t device_numbers;
  struct cdev device_cdev;
};

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

static int __init gpio_driver_init(void)
{
   printk(KERN_INFO "Inizializzazione driver GPIO\n");

   int ret_status;

   // Alloca la struttura dati di gestione del driver
   gpio_dev_t_ptr = kmalloc(sizeof(struct gpio_dev_t), GFP_KERNEL);
   if(gpio_dev_t_ptr == NULL){
     printk(KERN_WARNING "Allocazione della memoria non riuscita!");
     return -1;
   }

   // Alloca dinamicamente il primo range di device driver numbers diponibile
   ret_status = alloc_chrdev_region(&gpio_dev_t_ptr->device_numbers, 0, GPIOS_TO_MANAGE, DRIVER_NAME);
   if(ret_status < 0){
     printk(KERN_WARNING "Allocazione device numbers non riuscita!");
     kfree(gpio_dev_t_ptr);
     return ret_status;
   }

   // ************************** Registrazione del device ****************************

   // Ottiene una struttura cdev_t necessaria al kernel per la gestione del device driver
   cdev_init(&gpio_dev_t_ptr->device_cdev, &gpio_fops);
   gpio_dev_t_ptr->device_cdev.owner = THIS_MODULE;

   // Aggiorna il device driver model
   ret_status = cdev_add(&gpio_dev_t_ptr->device_cdev, gpio_dev_t_ptr->device_numbers, 1);
   if(ret_status < 0){
     printk(KERN_WARNING "Registrazione del driver non riuscita!");
     unregister_chrdev_region(gpio_dev_t_ptr->device_numbers, GPIOS_TO_MANAGE);
     kfree(gpio_dev_t_ptr);
     return ret_status;
   }

   return 0;
}

static void __exit gpio_driver_exit(void)
{
   printk(KERN_INFO "Deallocazione driver GPIO\n");

   cdev_del(&gpio_dev_t_ptr->device_cdev);
   unregister_chrdev_region(gpio_dev_t_ptr->device_numbers, GPIOS_TO_MANAGE);
   kfree(gpio_dev_t_ptr);
}

// Chiamata dal kernel quando un processo apre il file relativo alla periferica.
int gpio_open(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "[driver GPIO] Funzione open\n");
  struct gpio_dev_t* gpio_dev_ptr;

  // Restituisce un puntatore a struct gpio_dev_t partendo dalla struttura cdev che le è associata
  gpio_dev_ptr = container_of(inode->i_cdev, struct gpio_dev_t, device_cdev);
  filp->private_data = gpio_dev_ptr;

  return 0;
}

// Chiamata dal kernel quando nessun processo ha aperto il file relativo alla periferica.
int gpio_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "[driver GPIO] Funzione release driver GPIO\n");
  return 0;
}

// Chiamata quando si legge dal device file
ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
  printk(KERN_INFO "[driver GPIO] Funzione read\n");

  copy_to_user(buf, (const void*)2, count);
  printk(KERN_INFO "[driver GPIO] Lettura di un carattere\n");
  return count;
}

// Chiamata quando si scrive sul device file
ssize_t gpio_write(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
  printk(KERN_INFO "[driver GPIO] Funzione write\n");

  int dato_letto;
  copy_from_user(&dato_letto, buf, count);
  printk(KERN_INFO "[driver GPIO] valore scritto %i\n", dato_letto);
  return count;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_AUTHOR("Antonio Riccio");
MODULE_DESCRIPTION("Modulo kernel per l'accesso ad una periferica GPIO su Zynq 7000");
MODULE_LICENSE("GPL");
/** @} */
