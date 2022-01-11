cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#   $(info $(cur_dir) )
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)main/*.c $(cur_dir)main/*.cpp $(cur_dir)main/*.S))
# obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ota_control/*.c $(cur_dir)ota_control/*.cpp $(cur_dir)ota_control/*.S))
obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ota_main/*.c $(cur_dir)ota_main/*.cpp $(cur_dir)ota_main/*.S))
# obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ota_me/*.c $(cur_dir)ota_me/*.cpp $(cur_dir)ota_me/*.S))
obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)utils/*.c $(cur_dir)utils/*.cpp $(cur_dir)utils/*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)
obj-y += ota_lib/ \
         ota_me/ \
         ota_control/ \
		 ../../apps/pwl/ \
         ../../apps/common/ \
         ../../utils/hwtimer_list/ \
		 ../../utils/retention_ram/ \


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
	-Iutils/crc16   \
    -Iutils/crc32   \
    -Iutils/heap    \
    -Iservices/bt_app \
    -Iservices/nvrecord \
	-Iservices/nv_section/factory_section \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/ble_stack/ble_ip \
    -Iservices/resources \
    -Iservices/overlay \
    -Iservices/osif \
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
	-Iutils/boot_struct \
	-Imbed/api \
	-Iinclude \
	-Iinclude/watchdog

ifneq ($(FLASH_SIZE),)
CFLAGS_main.o += -DFLASH_SIZE=$(FLASH_SIZE)
CFLAGS_ota_control.o += -DFLASH_SIZE=$(FLASH_SIZE)
endif

