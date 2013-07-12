obj-m += h3.o

CCPREFIX=${PWD}/../linux-dev/dl/gcc-linaro-arm-linux-gnueabihf-4.7-2013.04-20130415_linux/bin/arm-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${PWD}/../linux-dev/KERNEL M=$(PWD) modules 
