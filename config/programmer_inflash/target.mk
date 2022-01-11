CHIP		?= best1305

DEBUG		?= 1

NOSTD		?= 1

LIBC_ROM	?= 1

PROGRAMMER	:= 1

FAULT_DUMP	?= 0

WATCHER_DOG ?= 1

init-y		:=
core-y		:= tests/programmer/ platform/cmsis/ platform/hal/
core-y      += tests/programmer_inflash/

LDS_FILE	:= programmer_inflash.lds

export CRC32_ROM ?= 1

KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal

KBUILD_CPPFLAGS += -DPROGRAMMER_INFLASH \

KBUILD_CFLAGS +=

LIB_LDFLAGS +=

CFLAGS_IMAGE +=

LDFLAGS_IMAGE +=

ifeq ($(CHIP),best1501)
# enabled when 1501 flash remap is ready
export OTA_REBOOT_FLASH_REMAP ?= 1
else
export OTA_REBOOT_FLASH_REMAP ?= 0
endif

# trace baudrate 
KBUILD_CPPFLAGS += -DDOWNLOAD_UART_BANDRATE=921600

# Offset address of applacation in flash
APP_IMAGE_FLASH_OFFSET ?= 0x18000
KBUILD_CPPFLAGS += -DAPP_IMAGE_FLASH_OFFSET=$(APP_IMAGE_FLASH_OFFSET)
                   
# ifneq ($(filter best1501 best2001,$(CHIP)),)
# KBUILD_CPPFLAGS += -DAPP_ENTRY_ADDRESS=0x2c018000 \
#                    -DDOWNLOAD_UART_BANDRATE=921600
# else
# KBUILD_CPPFLAGS += -DAPP_ENTRY_ADDRESS=0x3c018000 \
#                    -DDOWNLOAD_UART_BANDRATE=921600
# endif

export FLASH_UNIQUE_ID ?= 1
export TRACE_BAUD_RATE := 10*115200

ifeq ($(OTA_REBOOT_FLASH_REMAP),1)
KBUILD_CPPFLAGS += -DOTA_REBOOT_FLASH_REMAP
KBUILD_CPPFLAGS += -DOTA_REMAP_OFFSET=0x200000
endif # REMAP

# copy new image from flash offset address.
NEW_IMAGE_FLASH_OFFSET ?= 0x200000
KBUILD_CPPFLAGS += -DNEW_IMAGE_FLASH_OFFSET=$(NEW_IMAGE_FLASH_OFFSET)

# open watch dog function
ifeq ($(WATCHER_DOG),1)
KBUILD_CPPFLAGS += -D__WATCHER_DOG_RESET__
endif
