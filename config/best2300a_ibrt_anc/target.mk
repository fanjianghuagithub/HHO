include config/best2300a_ibrt/target.mk

#### ANC DEFINE START ######
export ANC_APP ?= 1

#### ANC CONFIG ######
export ANC_FF_CHECK ?= 1
export ANC_FB_CHECK ?= 0
export ANC_FF_ENABLED	?= 1
export ANC_FB_ENABLED	?= 1
export ANC_ASSIST_ENABLED   ?= 0
export AUDIO_ANC_FB_MC ?= 0
export AUDIO_SECTION_SUPPT ?= 1
export AUD_SECTION_STRUCT_VERSION ?= 2
export AUDIO_ANC_FB_MC_HW ?=1
export AUDIO_ANC_FB_ADJ_MC ?=0
##### ANC DEFINE END ######

APP_ANC_TEST ?= 1

TEST_OVER_THE_AIR ?= 1

TOTA ?= 0
#OTA_CODE_OFFSET ?= 0x20000
ifeq ($(ANC_APP),1)
KBUILD_CPPFLAGS += \
    -DANC_APP \
    -D__BT_ANC_KEY__
endif

ifeq ($(USE_CYBERON),1)

export THIRDPARTY_LIB ?= cyberon
KBUILD_CPPFLAGS += -D__CYBERON

export KWS_IN_RAM := 1
ifeq ($(KWS_IN_RAM),1)
CPPFLAGS_${LDS_FILE} += -DKWS_IN_RAM
endif #KWS_IN_RAM

endif #USE_CYBERON
