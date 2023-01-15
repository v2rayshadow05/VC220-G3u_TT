DRIVER_PATH = src

usbip_server: stub_driver

stub_driver:
	#make clean 
	make -C $(DRIVER_PATH) DEBUG=n
	cp $(DRIVER_PATH)/usbip.ko .
	cp $(DRIVER_PATH)/usbip_common_mod.ko .


clean:
	-rm -f usbip.ko usbip_common_mod.ko
	make -C $(DRIVER_PATH) clean
