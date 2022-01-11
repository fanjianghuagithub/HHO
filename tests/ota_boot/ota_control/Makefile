cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

OTA_BOOT_CTL_LIB_NAME := ota_boot_ctl

ifeq ($(OTA_DUAL_BOOT),1)
OTA_BOOT_CTL_LIB_NAME := $(OTA_BOOT_CTL_LIB_NAME)_dual
endif


$(OTA_BOOT_CTL_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(OTA_BOOT_CTL_LIB_NAME).a	 

ifeq ($(RTOS),1)
ccflags-y += -DRTOS
endif

ifneq ($(FLASH_FILL),)
ccflags-y += -DFLASH_FILL=$(FLASH_FILL)
endif

ccflags-y +=  \
    -Iapps/audioplayers \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/main \
    -Iapps/factory \
    -Iapps/pwl \
    -Iapps/key \
    -Iutils/boot_struct \
    -Iutils/heap    \
    -Iservices/bt_app \
    -Iservices/nvrecord \
	-Iservices/nv_section/factory_section \
    -Iservices/bt_profiles/stack \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/ble_stack/ble_ip \
    -Iservices/resources \
    -Iservices/overlay \
    $(BT_IF_INCLUDES) \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iplatform/hal \
	-Itests/ota_boot/inc \
    -Iutils/hwtimer_list \
	-Iutils/cqueue \
	-Iutils/intersyshci \
	-Iutils/retention_ram \
	-Iplatform/hal/best1000 \
	-Iplatform/drivers/norflash \
	-Iutils/boot_struct/ \
    -Iutils/crc32 \
    -Iutils/crc16 \
	-Imbed/api/
	
	
ifneq ($(FLASH_SIZE),)
CFLAGS_main.o += -DFLASH_SIZE=$(FLASH_SIZE)
CFLAGS_ota_control.o += -DFLASH_SIZE=$(FLASH_SIZE)
endif


