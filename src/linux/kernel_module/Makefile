obj-m := kmodule.o

KERNEL_SOURCE := /opt/linux-Digilent-Dev/
PWD := $(shell pwd)
ARCH=arm
CROSS_COMPILE=arm-xilinx-linux-gnueabi-

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
