obj-m:=test_tools.o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
 
default:
    $(MAKE) -C $(KDIR) M=$(PWD) modules
 
clean:
    rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions *.order *symvers *Module.markers
