#ifndef GPIO
#define GPIO

#define XGPIO_GIE_GINTR_ENABLE_MASK	0x80000000

#define GPIO_DATAOUT_OFFSET 	0x00
#define GPIO_TRI_OFFSET 	0x04
#define GPIO_DATAIN_OFFSET 	0x08

#define GPIO_IER_OFFSET  0x0C
#define GPIO_ICL_OFFSET  0x10
#define GPIO_ISR_OFFSET  0x14

#define CHANNEL1_ACK 0

#define GLOBAL_INTR_ENABLE 0x0000000F

#define GPIO_MAP_SIZE 0x10000	//spazio di indirizzamento del device

int Gpio_Global_Interrupt(long* baseAddress, unsigned long mask);
int Gpio_Ack_Interrupt(long* baseAddress, int channel);

#endif
