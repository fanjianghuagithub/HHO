#include "plat_addr_map.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "hal_bootmode.h"
#include "hal_trace.h"
#include "cmsis.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"
#include "apps.h"
#include "hal_gpio.h"
#include "tgt_hardware.h"
#include "hal_norflash.h"
#include "hal_chipid.h"
#include "bt_drv_interface.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "hal_cache.h"
#include "factory_section.h"
//#include "watchdog/watchdog.h"
// for init
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_pwl.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_utils.h"
#include "bt_if.h"
#include "app_utils.h"

// for progress
#include "hal_uart.h"
#include "tool_msg.h"
#include "cmsis_os.h"
#include "cmsis_nvic.h"
#include "ota_main.h"
#include "ota_control.h"
#include "retention_ram.h"
#include "app_key.h"
#include "app_utils.h"

extern "C" {
#include "utils.h"
#include "osapi.h"
#include "besble.h"
#include "hal_intersys.h"
#include "app_bt_stream.h"
#include "watchdog.h"
void BESHCI_Poll(void);
void BESHCI_SCO_Data_Start(void);
void BESHCI_SCO_Data_Stop(void);
void BESHCI_LockBuffer(void);
void BESHCI_UNLockBuffer(void);
}

#ifdef DEBUG
#if __APP_IMAGE_FLASH_OFFSET__<(128*1024)
//#error "app image flash offset is too small, the ota bin may overwrite the start of the app image!"
#endif
#endif

#define REG_SET_FIELD(reg, mask, shift, v)\
                        do{ \
                            *(volatile unsigned int *)(reg) &= ~(mask<<shift); \
                            *(volatile unsigned int *)(reg) |= (v<<shift); \
                        }while(0)

typedef void (*OTA_10_SECOND_TIMER_CB_T)(void);

/********************** Init Thread *********************/
static osThreadId ota_init_tid;
static void OtaInitThread(void const *argument);

#define OTA_INIT_STK_SZ (0x1000)
osThreadDef(OtaInitThread, osPriorityHigh, 1, OTA_INIT_STK_SZ, "OtaInitThread");

static void OtaInitThread(void const *argument)
{
    btdrv_start_bt();
    osDelay(2000);
    otaMainInit();

#define WAITING_BT_READY_TIME_SLICE_MS  100
#define WAITING_BT_READY_TIMEOUT_MS     5000
#define WAITING_BT_READY_MAX_TIME_SLICE \
        (WAITING_BT_READY_TIMEOUT_MS/WAITING_BT_READY_TIME_SLICE_MS)
    uint32_t waitingTimeSliceCnt = 0;
    while (!is_bt_init_done() &&
        (waitingTimeSliceCnt++ < WAITING_BT_READY_MAX_TIME_SLICE))
    {
        osDelay(WAITING_BT_READY_TIME_SLICE_MS);
    }
    TRACE(1,"BT init costs %d ms", waitingTimeSliceCnt*WAITING_BT_READY_TIME_SLICE_MS);

#if defined(CHIP_BEST1400)||defined(CHIP_BEST2300P) || defined(CHIP_BEST2300A)
    bt_drv_extra_config_after_init();
#endif

    osThreadTerminate(ota_init_tid);
}

/************************************************************/

static void ota_timehandler(void const *param);
osTimerDef (OTA, ota_timehandler);
static osTimerId ota_10_second_timer = NULL;

#define WATCH_DOG_TIMEOUT_IN_SECONDS     15

#if (OTA_SPEC_VERSION > 1)
extern void ota_execute_the_final_operation(void);

#endif

typedef struct
{
    uint8_t timer_id;
    uint8_t timer_en;
    uint8_t timer_count;
    OTA_10_SECOND_TIMER_CB_T cb;
} OTA_10_SECOND_TIMER_STRUCT;

void ota_shutdwon(void);
void _ota_watchdog_ping(void);

static OTA_10_SECOND_TIMER_STRUCT ota_wd_ping_timer =
{
    .timer_id = OTA_WATCH_DOG_PING_TIMER_ID,
    .timer_en = 0,
    .timer_count = 0,
    .cb = _ota_watchdog_ping
};

static void ota_timehandler(void const *param)
{
#if (OTA_SPEC_VERSION > 1)
    ota_execute_the_final_operation();
#endif

    if (ota_wd_ping_timer.timer_en)
    {
        ota_wd_ping_timer.cb();
    }
}

static void ota_timer_init(void)
{
    if (ota_10_second_timer == NULL)
        ota_10_second_timer = osTimerCreate (osTimer(OTA), osTimerPeriodic, NULL);

    if (ota_10_second_timer) {
        osTimerStop(ota_10_second_timer);
        TRACE(0,"Start ota timer.");
        osTimerStart(ota_10_second_timer, 1000 * OTA_WD_PING_PERIOD_IN_SECONDS);
    }
}

void ota_reset_timer(uint32_t intervalInMs)
{
    if (ota_10_second_timer) {
        ota_start_10_second_timer(OTA_WATCH_DOG_PING_TIMER_ID);

        osTimerStop(ota_10_second_timer);
        TRACE(0,"Restart ota timer.");
        osTimerStart(ota_10_second_timer, intervalInMs);
    }
}

void ota_start_10_second_timer(uint8_t timer_id)
{
    if (timer_id == OTA_WATCH_DOG_PING_TIMER_ID) {
        ota_wd_ping_timer.timer_en = 1;
        ota_wd_ping_timer.timer_count = 0;
    }
}

void ota_stop_10_second_timer(uint8_t timer_id)
{
    if (timer_id == OTA_WATCH_DOG_PING_TIMER_ID) {
        ota_wd_ping_timer.timer_en = 0;
        ota_wd_ping_timer.timer_count = 0;
    }
}

void ota_reset_10_second_counter(uint8_t timer_id)
{
    if (timer_id == OTA_WATCH_DOG_PING_TIMER_ID) {
        ota_wd_ping_timer.timer_count = 0;
    }
}

static osThreadId main_thread_tid = NULL;

int system_shutdown(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x4);
    return 0;
}

int system_reset(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x8);
    return 0;
}

int tgt_hardware_setup(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl, sizeof(cfg_hw_pinmux_pwl)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    return 0;
}

#define WAIT_TRACE_TIMEOUT                      MS_TO_TICKS(200)

typedef void (*FLASH_ENTRY)(void);

#define EXEC_CODE_BASE (FLASH_BASE+__APP_IMAGE_FLASH_OFFSET__)

extern FLASH_OTA_BOOT_INFO_T otaBootInfoInFlash;

static const struct boot_struct_t * const boot_struct =
    (struct boot_struct_t *)EXEC_CODE_BASE;

static int app_is_otaMode(void)
{
    uint32_t bootmode = hal_sw_bootmode_get();

    return (bootmode & HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
}

#define DLD_UPGRADE_FAIL_DIVE_INTO_DLD_MODE 0
#if DLD_UPGRADE_FAIL_DIVE_INTO_DLD_MODE
static bool is_force_boot_enter_usb_dld_mode(void)
{
    uint32_t magic_number = app_get_magic_number();
    uint32_t magic = boot_struct->hdr.magic;
    TRACE(4,"%s magic_number = 0x%x magic = 0x%x exeu_addr = 0x%x",__func__,magic_number,magic,(uint32_t)&boot_struct->hdr);
    osDelay(10);
    if((app_get_magic_number() == NORMAL_BOOT) && (NORMAL_BOOT == boot_struct->hdr.magic) )
        return false;
    if(app_get_magic_number() == COPY_NEW_IMAGE)
        return false;

    return true;
}

static bool isCharging()
{
    enum PMU_CHARGER_STATUS_T charger = PMU_CHARGER_UNKNOWN;
    hal_analogif_open();

    charger = pmu_charger_get_status();
    if (charger == PMU_CHARGER_PLUGIN)
        return true;
    return false;
}

static void ota_enter_usb_dld_mode(void)
{
    TRACE(1,"%s",__func__);
    hal_sw_bootmode_clear(0xffffffff);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_FORCE_USB_DLD | HAL_SW_BOOTMODE_SKIP_FLASH_BOOT);
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
    while(1);
}

#endif

extern "C" void BootSelect(void)
{
	FLASH_ENTRY entry;

    if (app_is_otaMode())
        return;

    // Workaround for reboot: controller in standard SPI mode while FLASH in QUAD mode
    // First read will fail when FLASH in QUAD mode, but it will make FLASH roll back to standard SPI mode
    // Second read will succeed

    volatile uint32_t* ota_magic;

    ota_magic = (volatile uint32_t *)&otaBootInfoInFlash;

    // First read (and also flush the controller prefetch buffer)
    *(ota_magic + 0x400);
#if 0//OTA_NON_CACHE_READ_ISSUE_WORKAROUND
    if (NORMAL_BOOT == *ota_magic)
#else
    if ((NORMAL_BOOT == *ota_magic) && (NORMAL_BOOT == boot_struct->hdr.magic))
#endif
    {
#if defined(SLAVE_BIN_FLASH_OFFSET) && (SLAVE_BIN_FLASH_OFFSET != 0)
        if (isCharging())
        {
            // Disable all IRQs
            NVIC_DisableAllIRQs();
            // Ensure in thumb state        0xc120010;//
            entry = (FLASH_ENTRY)FLASH_TO_FLASHX((uint8_t *)(&boot_struct->hdr + 1)+SLAVE_BIN_FLASH_OFFSET);
            entry = (FLASH_ENTRY)((uint32_t)entry | 1);
            entry();
        }
        else
#endif
        {
            // Disable all IRQs
            NVIC_DisableAllIRQs();
            // Ensure in thumb state
            entry = (FLASH_ENTRY)FLASH_TO_FLASHX(&boot_struct->hdr + 1);
            entry = (FLASH_ENTRY)((uint32_t)entry | 1);
            entry();
        }
    }
}

void ota_shutdwon(void)
{
#if (OTA_SPEC_VERSION > 1)
    if (ota_is_in_progress())
    {
        return;
    }
#endif
    TRACE(0,"ota_shutdwon...");
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x4);
}

void _ota_watchdog_ping(void)
{
#ifdef __WATCHER_DOG_RESET__
    hal_wdt_ping(HAL_WDT_ID_0);
#endif

#if (OTA_SPEC_VERSION > 1)
    ota_wd_ping_timer.timer_count++;
    if (ota_wd_ping_timer.timer_count > (OTA_POWER_OFF_TIMEOUT_IN_SECONDS/OTA_WD_PING_PERIOD_IN_SECONDS) &&
        !ota_is_in_progress())
    {
        ota_shutdwon();
    }
#endif
}

int ota_key_handler(uint32_t key_code, uint8_t key_event)
{
    if ((key_code == APP_KEY_CODE_PWR) && (key_event == APP_KEY_EVENT_LONGLONGPRESS))
    {
        ota_shutdwon();
    }
    return 0;
}

uint32_t* __userdata_start;
uint32_t* __factory_start;

OTA_SOFTWARE_REV_INFO_T otaSoftwareRevInfoInFlash __attribute((section(".ota_boot_rev"))) =
{0, 0, 1, 0};

OTA_SOFTWARE_REV_INFO_T otaSoftwareRevInfoInRam;

extern "C" BtStatus RXBUFF_Init(void);
extern "C" int nv_record_env_init(void);
extern "C" bool nvrec_dev_data_open(void);

uint32_t user_data_nv_flash_offset;

#if DUAL_IMAGE_COPY_ENABLED
uint32_t new_image_copy_flash_offset;
#endif

int ota_init(void)
{
    int nRet = 0;

    TRACE(0,"ota_init");

    app_wdt_open(WATCH_DOG_TIMEOUT_IN_SECONDS);
    app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);

    //hal_key_open(false, ota_key_handler);
#ifdef __APP_USER_DATA_NV_FLASH_OFFSET__
    user_data_nv_flash_offset = __APP_USER_DATA_NV_FLASH_OFFSET__;
#else
    user_data_nv_flash_offset = hal_norflash_get_flash_total_size(HAL_FLASH_ID_0) - 2*4096;
#endif

    __userdata_start = (uint32_t *)(user_data_nv_flash_offset + OTA_FLASH_LOGIC_ADDR);
    __factory_start = (uint32_t *)(user_data_nv_flash_offset + OTA_FLASH_LOGIC_ADDR + 4096);

    TRACE(2,"The user data flash addr 0x%x factory data flash addr 0x%x", (uint32_t)__userdata_start, (uint32_t)__factory_start);
    TRACE(1,"The flash size is 0x%x", hal_norflash_get_flash_total_size(HAL_FLASH_ID_0));

    hal_cache_invalidate(HAL_CACHE_ID_D_CACHE,
        (uint32_t)__userdata_start,
        4096);

    hal_cache_invalidate(HAL_CACHE_ID_D_CACHE,
        (uint32_t)__factory_start,
        4096);

    TRACE(4,"The OTA software rev is %d.%d.%d.%d",
        otaSoftwareRevInfoInFlash.softwareRevByte0,
        otaSoftwareRevInfoInFlash.softwareRevByte1,
        otaSoftwareRevInfoInFlash.softwareRevByte2,
        otaSoftwareRevInfoInFlash.softwareRevByte3);

    otaSoftwareRevInfoInRam = otaSoftwareRevInfoInFlash;

#if DUAL_IMAGE_COPY_ENABLED
    uint32_t magicNumber = app_get_magic_number();

    TRACE(1,"magic num 0x%x", magicNumber);

    new_image_copy_flash_offset = NEW_IMAGE_FLASH_OFFSET;

    if (COPY_NEW_IMAGE == magicNumber)
    {
        hal_norflash_disable_protection(HAL_FLASH_ID_0);
        TRACE(2,"Start copying new image from 0x%x to 0x%x...",
            new_image_copy_flash_offset, OTA_FLASH_OFFSET_OF_IMAGE);
        app_copy_new_image(new_image_copy_flash_offset, OTA_FLASH_OFFSET_OF_IMAGE);
        TRACE(0,"Copying new image failed, enter OTA mode.");
    }

    hal_wdt_ping(HAL_WDT_ID_0);
#endif

	factory_section_open();

    ota_timer_init();
#ifdef __WATCHER_DOG_RESET__
    ota_start_10_second_timer(OTA_WATCH_DOG_PING_TIMER_ID);
#endif

    hci_buff_list_init();
    //RXBUFF_Init();

    ota_init_tid = osThreadCreate(osThread(OtaInitThread), NULL);

    return nRet;
}

int ota_deinit(void)
{
    osDelay(1000);
	return 0;
}

extern void ota_burn_data_init(void);

int main(void)
{
    uint8_t sys_case = 0;
    int ret = 0;

    tgt_hardware_setup();

    main_thread_tid = osThreadGetId();

    hal_sys_timer_open();
    hwtimer_init();

    OS_Init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);

    hal_gpdma_open();

#ifdef DEBUG
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif

#if (DEBUG_PORT == 2)
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif
    hal_sleep_start_stats(10000, 10000);
#endif

    hal_iomux_ispi_access_init();

#if DUAL_IMAGE_COPY_ENABLED
    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_ENTER_HIDE_BOOT)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
    }
#endif
#if DLD_UPGRADE_FAIL_DIVE_INTO_DLD_MODE
    else{
        if((is_force_boot_enter_usb_dld_mode() == true) && (isCharging() == true)){
            TRACE(1,"%s",__func__);
            ota_enter_usb_dld_mode();
        }
    }
#endif

    TRACE(1,"\nchip metal id = %x\n", hal_get_chip_metal_id());

    TRACE(1,"boot_struct->hdr.magic 0x%x", boot_struct->hdr.magic);

    pmu_open();

    hal_cmu_simu_init();

    hal_sys_timer_open();

    ret = ota_init();

    ota_burn_data_init();

#if (OTA_SPEC_VERSION > 1)
    ota_control_reset_env();
#endif

    if (!ret){
        while(1)
        {
            osEvent evt;
            osSignalClear (main_thread_tid, 0x0f);
            //wait any signal
            evt = osSignalWait(0x0, osWaitForever);

            //get role from signal value
            if(evt.status == osEventSignal)
            {
                if(evt.value.signals & 0x04)
                {
                    sys_case = 1;
                    break;
                }
                else if(evt.value.signals & 0x08)
                {
                    sys_case = 2;
                    break;
                }
            }else{
                sys_case = 1;
                break;
            }
         }
    }
    //system_shutdown_wdt_config(10);
    ota_deinit();
    TRACE(1,"byebye~~~ %d\n", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TRACE(0,"shutdown\n");
        pmu_shutdown();
    }else if (sys_case == 2){
        TRACE(0,"reset\n");
        hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
    }
    return 0;
}

