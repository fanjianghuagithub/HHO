export SINGLE_WIRE_DOWNLOAD ?= 1
# KBUILD_CPPFLAGS += -DUNCONDITIONAL_ENTER_SINGLE_WIRE_DOWNLOAD

include config/programmer_inflash/target.mk

core-y += tests/programmer_ext/ota_copy/

$(info ----------programmer in flash---------------------)
$(info NEW_IMAGE_FLASH_OFFSET: $(NEW_IMAGE_FLASH_OFFSET))
$(info OTA_REBOOT_FLASH_REMAP: $(OTA_REBOOT_FLASH_REMAP))
$(info APP_ENTRY_ADDRESS: $(APP_ENTRY_ADDRESS))
$(info APP_IMAGE_FLASH_OFFSET: $(APP_IMAGE_FLASH_OFFSET))
$(info -------------------------------)