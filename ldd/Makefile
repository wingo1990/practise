ifneq ($(KERNELRELEASE),)
obj-m := hello_world.o
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	@rm -f *.ko *.o *.mod.c modules.order  Module.symvers
endif
