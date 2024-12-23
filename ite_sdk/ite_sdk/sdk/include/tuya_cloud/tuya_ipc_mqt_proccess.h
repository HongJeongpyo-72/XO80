/*********************************************************************************
  *Copyright(C),2017, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_ipc_mqt_proccess.h
**********************************************************************************/

#ifndef __TUYA_IPC_MQT_PROCCESS_H__
#define __TUYA_IPC_MQT_PROCCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"


/**
 * \typedef  typedef VOID (*GW_CLOUD_STORAGE_CB)(IN CONST CHAR* action, IN CONST CHAR* store_mode);
  * \brief 云存储功能开启服务回调回调函数定义
 * \param[in] action: store/abort
 * \param[in] store_mode: event/continue
 */
typedef VOID (*IPC_CLOUD_STORAGE_ORDER_CB)(IN CONST CHAR_T* action, IN CONST CHAR_T* store_mode);
typedef VOID (*IPC_CLOUD_STORAGE_EVENT_CB)(IN CONST CHAR_T* action, IN CONST INT_T duration, IN CONST CHAR_T* data);

typedef VOID (*IPC_DOORBELL_STATUS_CB)(IN CHAR_T* action,IN CHAR_T* extra_data);

typedef VOID (*IPC_CLOUD_AI_CB)(IN CONST CHAR_T* action, IN CONST CHAR_T* mode);

typedef VOID (*IPC_LOCAL_AI_CB)(IN CONST CHAR_T* action, IN CONST CHAR_T* face_id_list);

typedef int (*IPC_RTC_MQTT_DATA_CB)(IN CONST CHAR_T *dev_id, IN CONST CHAR_T * buffer, IN CONST INT_T len);


VOID tuya_ipc_mqt_cloud_storage_order_cb(IN CONST IPC_CLOUD_STORAGE_ORDER_CB cloud_storage_cb);

VOID tuya_ipc_mqt_cloud_storage_event_cb(IN CONST IPC_CLOUD_STORAGE_EVENT_CB cb);

VOID tuya_ipc_mqt_rtc_callback(IN CONST IPC_RTC_MQTT_DATA_CB pcbk);

VOID iot_register_extra_mqt_cb(VOID);

VOID tuya_ipc_mqt_local_ai_cb(IN CONST IPC_LOCAL_AI_CB cb);


typedef struct {
    IPC_CLOUD_STORAGE_ORDER_CB  cloud_storage_order_cb;
    IPC_CLOUD_STORAGE_EVENT_CB  cloud_storage_event_cb;
    IPC_DOORBELL_STATUS_CB      doorbell_status_cb;
    IPC_RTC_MQTT_DATA_CB        rtc_msg_cb;
    IPC_CLOUD_AI_CB             cloud_ai_cb;
    IPC_LOCAL_AI_CB             local_ai_cb;
}IPC_MQT_CTRL_S;


#ifdef __cplusplus
}
#endif

#endif

