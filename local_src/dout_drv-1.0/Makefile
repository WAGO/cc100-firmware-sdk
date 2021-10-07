ifeq ($(PTX_KERNEL_DIR),)

all:
	@echo "please export PTX_KERNEL_DIR"

clean:
	rm -rf *.o *.ko *.kmod *.mod.c .*.cmd .*.o.d Module.symvers .tmp_versions modules.order

else

obj-m	:= dout_drv.o

modules modules_install clean:
	$(MAKE) -C $(PTX_KERNEL_DIR) M=$(CURDIR) V=$(V) $@

endif
