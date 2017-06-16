#include "gpio.h"

int Gpio_Global_Interrupt(long* baseAddress, unsigned long mask)
{
	baseAddress[GPIO_IER_OFFSET/4]=mask;
	return 1;
}

int Gpio_Ack_Interrupt(long* baseAddress, int channel)
{
	baseAddress[GPIO_ICL_OFFSET/4] = 0xFFFFFFFF;
	return 1;
}
