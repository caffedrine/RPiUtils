KDIR  := /lib/modules/$(shell uname -r)/build

obj-m+=testmod.o
 
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean