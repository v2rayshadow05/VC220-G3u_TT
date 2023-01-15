deps_config := \
	/mydisk/VR600_ISP/linux_bcm_VR600TTv1/build/../build/sysdeps/linux/Config.in

.config include/config.h: $(deps_config)

$(deps_config):
