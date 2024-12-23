/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_ipc_sd_demo
**********************************************************************************/
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "tuya_ipc_api.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_common_demo.h"

/************************
Description: Using the TUYA SD card storage and playback function, 
the developers need to implement the relevant interface.
Noteï¼šInterface names cannot be changed, and these interfaces are declared and used in SDK.
This demo file gives the management implementation of SD card operation for typical embedded devices in Linux system.
Developers can modify it according to their practical application.

VOID tuya_ipc_sd_format(VOID);
VOID tuya_ipc_sd_remount(VOID);
E_SD_STATUS tuya_ipc_sd_get_status(VOID);
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free);
CHAR_T *tuya_ipc_get_sd_mount_path(VOID);

************************/

#define TYDEBUG printf
#define TYERROR printf

CHAR_T *sd_path = NULL;

//Implementation Interface of Formatting Operation
VOID tuya_ipc_sd_format(VOID)
{

}

//Implementation Interface for Remounting
VOID tuya_ipc_sd_remount(VOID)
{

}

STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID)
{
    BOOL_T sd_on = IPC_APP_get_sd_record_onoff();
    if(sd_on)
    {
        UINT_T sd_mode = IPC_APP_get_sd_record_mode();
        if(0 == sd_mode)
        {
            return SS_WRITE_MODE_EVENT;
        }
        else
        {
            return SS_WRITE_MODE_ALL;
        }
    }
    else
    {
        return SS_WRITE_MODE_NONE;
    }
}

//Implementation Interface for Obtaining SD Card Status
E_SD_STATUS tuya_ipc_sd_get_status(VOID)
{
    //char *path = UpgradeGetSDPath();
    if(sd_path)
        return SD_STATUS_NORMAL;
    else
        return SD_STATUS_NOT_EXIST;
}

//SD card capacity acquisition interface, unit: KB
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free)
{
    *p_total = 16*1024*1024;
    *p_used = 0;
    *p_free = 16*1024*1024;
}

//get the path of mounting sdcard
CHAR_T *tuya_ipc_get_sd_mount_path(VOID)
{
    return sd_path;
}

//The maximum number of events per day, exceeding this value, there will be an exception when playback and can not play.
//Too much setting of this value will affect the query efficiency
#define MAX_EVENT_NUM_PER_DAY   (500)
extern IPC_MEDIA_INFO_S s_media_info;

OPERATE_RET TUYA_APP_Init_Stream_Storage(IN CONST CHAR_T *p_sd_base_path)
{
    sd_path = p_sd_base_path;
    PR_DEBUG("Init Stream_Storage SD:%s", p_sd_base_path);
    OPERATE_RET ret = tuya_ipc_ss_init(p_sd_base_path, &s_media_info,MAX_EVENT_NUM_PER_DAY, NULL);
    if(ret != OPRT_OK)
    {
        PR_ERR("Init Main Video Stream_Storage Fail. %d", ret);
        return OPRT_COM_ERROR;
    }
    //Àq»{¶}±Ò³sÄò¦sÀx
    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_ALL);
    return OPRT_OK;
}


