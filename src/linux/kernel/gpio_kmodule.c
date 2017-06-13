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

/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/

static int __init hello_init(void)
{
   printk(KERN_ALERT "Hello, world\n");
   return 0;
}

static void __exit hello_exit(void)
{
   printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Antonio Riccio");
MODULE_DESCRIPTION("Modulo kernel per l'accesso ad una periferica GPIO su Zynq 7000");
MODULE_LICENSE("GPL");
/** @} */
