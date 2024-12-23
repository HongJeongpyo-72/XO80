/***********************************************************
*  File: uni_system.c
*  Author: nzy
*  Date: 120427
***********************************************************/
#define _UNI_SYSTEM_GLOBAL
#include <string.h>
#if defined(TOOLCHAIN_GCC_ARM)
#include <malloc.h>
#endif

#include "tuya_hal_system.h"
#include "tuya_hal_wifi.h"
#include "../errors_compat.h"
#include "ite/itp.h"

#ifdef __OPENRTOS__
    #include "openrtos/FreeRTOS.h"
    #include "openrtos/task.h"
#endif // __OPENRTOS__

/***********************************************************
*************************micro define***********************
***********************************************************/


#if 0
#define LOGD PR_DEBUG
#define LOGT PR_TRACE
#define LOGN PR_NOTICE
#define LOGE PR_ERR
#else
#define LOGD(...) printf("[SYS DEBUG]" __VA_ARGS__)
#define LOGT(...) printf("[SYS TRACE]" __VA_ARGS__)
#define LOGN(...) printf("[SYS NOTICE]" __VA_ARGS__)
#define LOGE(...) printf("[SYS ERROR]" __VA_ARGS__)
#endif

#define SERIAL_NUM_LEN 32

/* reset mode
** PMU reg address: 0x2d; BIT[15-14] default: 00; set 11 by pwoer up;
** flag bit[14]: set 0 by deep_sleep;
** flag bit[15-14]: 00 reset form power down; 10 reset from deep_sleep; 11 reset from watchdog.
*/
#define RESET_MODE_MASK  0XC000
#define RESET_POWER_DOWN 0X0000
#define RESET_DEEP_SLEEP 0X8000
#define RESET_WATCH_DOG  0X4000


/***********************************************************
*************************variable define********************
***********************************************************/
static char reason_buf[64];
static char serial_no[SERIAL_NUM_LEN+1] = {0};

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: GetSystemTickCount 
*  Input: none
*  Output: none
*  Return: system tick count
***********************************************************/


SYS_TICK_T tuya_hal_get_systemtickcount(void)
{
    return xTaskGetTickCount();
}

/***********************************************************
*  Function: GetTickRateMs 
*  Input: none
*  Output: none
*  Return: tick rate spend how many ms
***********************************************************/
uint32_t tuya_hal_get_tickratems(void)
{
    return (SYS_TICK_T)1;
}

/***********************************************************
*  Function: SystemSleep 
*  Input: msTime
*  Output: none 
*  Return: none
*  Date: 120427
***********************************************************/
void tuya_hal_system_sleep(const unsigned long msTime)
{
    usleep(msTime*1000);
}

/***********************************************************
*  Function: SystemIsrStatus->direct system interrupt status
*  Input: none
*  Output: none 
*  Return: bool
***********************************************************/
bool tuya_hal_system_isrstatus(void)
{
    //if(0 != __get_IPSR()) {
       // return true;
   // }

    return false;
}

/***********************************************************
*  Function: SystemReset 
*  Input: msTime
*  Output: none 
*  Return: none
*  Date: 120427
***********************************************************/
void tuya_hal_system_reset(void)
{
    //itp_codec_standby();
    exit(0);
}

/***********************************************************
*  Function: uni_watchdog_init_and_start 
*  Input: timeval
*  Output: none 
*  Return: void *
***********************************************************/
//static wdt_t wdt_obj;

void tuya_hal_watchdog_init_start(int timeval)
{
#if defined(TY_WATCHDOG) && (TY_WATCHDOG==1)
    //rda_wdt_init(&wdt_obj, timeval);
    //rda_wdt_start(&wdt_obj);
#endif
}

/***********************************************************
*  Function: uni_watchdog_refresh 
*  Input: none
*  Output: none 
*  Return: void *
***********************************************************/
void tuya_hal_watchdog_refresh(void)
{
#if defined(TY_WATCHDOG) && (TY_WATCHDOG==1)
    //rda_wdt_feed(&wdt_obj);
#endif
}

/***********************************************************
*  Function: uni_watchdog_stop 
*  Input: none
*  Output: none 
*  Return: void *
***********************************************************/
void tuya_hal_watchdog_stop(void)
{
#if defined(TY_WATCHDOG) && (TY_WATCHDOG==1)
    //rda_wdt_stop(&wdt_obj);
#endif
}


/***********************************************************
*  Function: SysGetHeapSize 
*  Input: none
*  Output: none 
*  Return: int-> <0 means don't support to get heapsize
***********************************************************/
int tuya_hal_system_getheapsize(void)
{
    return 1024;

}

/***********************************************************
*  Function: GetSerialNo 
*  Input: none
*  Output: none 
*  Return: char *->serial number
***********************************************************/
char *tuya_hal_get_serialno(void)
{
    // if the device have unique serial number
    // then add get serial number code to serial_no array

    // if don't have unique serial number,then use mac addr

    return "123412567456890";
}

/***********************************************************
*  Function: system_get_rst_info 
*  Input: none
*  Output: none 
*  Return: char *->reset reason
***********************************************************/
char *tuya_hal_system_get_rst_info(void)
{
	TUYA_RESET_REASON_E rtn_value = tuya_hal_system_get_rst_reason();
    sprintf(reason_buf,"reset reason is :%d",rtn_value);
    return reason_buf;
}

/* reset mode RDA平台需要手动将重启原因设置到寄存器中，上电默认将重启原因设置为看门狗重启3，deep_sleep设置为1,软件重启之前设置为2，重新上电时寄存器信息将丢失，读取值为0
** PMU reg address: 0x2d; BIT[15-14] default: 00;
** flag bit[15-14]: 00(0) power down; 01(1) deep_sleep; 10(2) software reset; 11(3) watchdog.
*/
void tuya_system_set_rst_info(uint8_t rtn_value)
{
	#if 0
	uint16_t reg_cd_val;
    rd_rf_usb_reg(0xcd, &reg_cd_val, 0);
	reg_cd_val &= ~RESET_MODE_MASK;
	reg_cd_val |= rtn_value << 14;
	printf("wr_rf_usb_reg::reg_cd_val=0x%x\r\n",reg_cd_val);
	wr_rf_usb_reg(0xcd, reg_cd_val, 0);
	#endif
}

TUYA_RESET_REASON_E tuya_hal_system_get_rst_reason(void)
{
	#if 0
    static bool have_read = false;
    static TUYA_RESET_REASON_E cache_reason = TUYA_RESET_REASON_POWERON;
    if (!have_read) {
        unsigned char rtn_value;
        uint16_t reg_cd_val;
        rd_rf_usb_reg(0xcd, &reg_cd_val, 0);
        rtn_value = ((reg_cd_val & RESET_MODE_MASK) >> 14) & 0x03;
        switch (rtn_value) {
        case 0:
            cache_reason = TUYA_RESET_REASON_POWERON;
            break;
        case 1:
            cache_reason = TUYA_RESET_REASON_DEEPSLEEP;
            break;
        case 2:
            cache_reason = TUYA_RESET_REASON_SOFTWARE;
            break;
        case 3:
            cache_reason = TUYA_RESET_REASON_SOFTWARE_WATCHDOG;
            break;
        default:
            break;
        }
        tuya_system_set_rst_info(3);
        have_read = true;
    }
    return cache_reason;
	#endif
	return 0;
}

bool tuya_hal_system_rst_reason_poweron(void)
{
    TUYA_RESET_REASON_E reason = tuya_hal_system_get_rst_reason();
    if (TUYA_RESET_REASON_POWERON == reason) {
        return true;
    }
    return false;
}
/***********************************************************
*  Function: tuya_random
*  Input: none
*  Output: none
*  Return: random data in INT
***********************************************************/
uint32_t tuya_hal_random(void)
{
    srand(ithTimerGetCounter(portTIMER));
	return rand();
}

/***********************************************************
*  Function: tuya_get_random_data
*  Input: dst size
*  Output: none
*  Return: void
***********************************************************/
int tuya_hal_get_random_data(uint8_t* dst, int size, uint8_t range)
{
    int index = 0;

    if(range == 0)
        range = 0xFF;
        
    srand(ithTimerGetCounter(portTIMER));
    for(index = 0; index < size; index++)
        dst[index] = rand() % range;

    return OPRT_OK;
}

/***********************************************************
*  Function: tuya_set_lp_mode
*  Input: lp_enable
*  Output: none
*  Return: void
***********************************************************/
static bool is_lp_enable = false;
void tuya_hal_set_lp_mode(bool lp_enable)
{
    is_lp_enable = lp_enable;
}

/***********************************************************
*  Function: tuya_get_lp_mode
*  Input: none
*  Output: none
*  Return: bool
***********************************************************/
bool tuya_hal_get_lp_mode(void)
{
    return is_lp_enable;
}
