obj-y := audioplayers/ common/ main/ key/ pwl/ battery/ factory/ 

ifeq ($(ICP1205),1)
obj-y += box/
endif

ifeq ($(ICP1205),1)
obj-y += box_2/
endif

ifeq ($(BLE_BOZUN),1)
obj-y += ble_bozun/
endif

ifeq ($(AW2217),1)
obj-y += aw22127/
endif

ifeq ($(AW_UPDATA_TEST),1)
obj-y += aw_updata/
endif

ifeq ($(SPT513),1)
obj-y += spt513/
endif

ifeq ($(APP_TEST_AUDIO),1)
obj-y += apptester/
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
obj-y += usbaudio/
endif
ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
obj-y += btusbaudio/
obj-y += usbaudio/
endif

ifeq ($(APP_TEST_SDMMC),1)
obj-y += sdmmc/
endif

ifeq ($(ANC_APP),1)
obj-y += anc/
endif

ifeq ($(ANC_ASSIST_ENABLED),1)
obj-y += voice_assist/
endif

ifeq ($(VOICE_DETECTOR_EN),1)
obj-y += voice_detector/
endif

ifeq ($(SENSOR_HUB),1)
obj-y += sensorhub/
endif
ifeq ($(PC_CMD_UART),1)
obj-y += cmd/
endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
					-Iapps/anc/inc \
					-Iapps/voice_assist/inc \
					-Iapps/common \
					-Iapps/sdmmc \
					-Iapps/main \
					-Iapps/cmd \
					-Iapps/key \
					-Iapps/pwl \
					-Iapps/battery \
					-Iservices/voicepath \
					-Iservices/audio_dump/include \
					-Iservices/ble_app/app_datapath \
					-Iservices/bt_app/a2dp_codecs/include \
					-Iutils/list \
					-Iutils/heap \
					-Iservices/multimedia/audio/process/filters/include
					
ifeq ($(ICP1205),1)
subdir-ccflags-y += -Iapps/box
endif

ifeq ($(ICP1205),1)
subdir-ccflags-y += -Iapps/box_2
endif
		
ifeq ($(BLE_BOZUN),1)
subdir-ccflags-y += -Iapps/ble_bozun
endif

ifeq ($(AW2217),1)
subdir-ccflags-y += -Iapps/aw22127
endif

ifeq ($(AW_UPDATA_TEST),1)
subdir-ccflags-y += -Iapps/aw_updata
endif

ifeq ($(SPT513),1)
subdir-ccflags-y += -Iapps/spt513
endif		
		
ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
subdir-ccflags-y += -Iapps/btusbaudio
endif

ifeq ($(A2DP_LDAC_ON),1)
subdir-ccflags-y += -Ithirdparty/audio_codec_lib/ldac/inc
endif

