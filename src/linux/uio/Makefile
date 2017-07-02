OBJECTS=uio.o gpio_ll.o gpio.o bsp_led.o bsp_switch.o bsp_button.o
INCLUDE_PATH=../../inc/
SRC_PATH=../../
OPTIONS=-I$(INCLUDE_PATH) -c
GPIO_LL_DEP=$(INCLUDE_PATH)gpio_ll.h
GPIO_DEP=$(INCLUDE_PATH)gpio.h
BSP_LED_DEP=$(INCLUDE_PATH)bsp_led.h
BSP_SWT_DEP=$(INCLUDE_PATH)bsp_switch.h
BSP_BTN_DEP=$(INCLUDE_PATH)bsp_button.h

all: uio

uio: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

uio.o: uio.c $(GPIO_LL_DEP) $(INCLUDE_PATH)config.h $(INCLUDE_PATH)xparameters.h
	gcc $(OPTIONS) uio.c

bsp_button.o : $(BSP_BTN_DEP) $(GPIO_DEP) $(INCLUDE_PATH)config.h $(SRC_PATH)bsp/bsp_button.c
	gcc $(OPTIONS) $(SRC_PATH)bsp/bsp_button.c

bsp_switch.o : $(BSP_SWT_DEP) $(GPIO_DEP) $(INCLUDE_PATH)config.h $(SRC_PATH)bsp/bsp_switch.c
	gcc $(OPTIONS) $(SRC_PATH)bsp/bsp_switch.c

bsp_led.o : $(BSP_LED_DEP) $(GPIO_DEP) $(INCLUDE_PATH)config.h $(SRC_PATH)bsp/bsp_led.c
	gcc $(OPTIONS) $(SRC_PATH)bsp/bsp_led.c

gpio.o : $(GPIO_DEP) $(GPIO_LL_DEP) $(INCLUDE_PATH)gpio_defs.h $(SRC_PATH)gpio.c
	gcc $(OPTIONS) $(SRC_PATH)gpio.c

gpio_ll.o : $(GPIO_LL_DEP) $(SRC_PATH)gpio_ll.c
	gcc $(OPTIONS) $(SRC_PATH)gpio_ll.c

clean:
	rm *.o uio
