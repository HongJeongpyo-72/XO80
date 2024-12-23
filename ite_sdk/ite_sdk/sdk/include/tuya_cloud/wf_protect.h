 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   wangym Linch                                                  *
 * @date    :   2020-03-03                                                    *
 * @brief   :   tuya pegasus help device config without password phase        *
 * @log     :   first implement by wangym                                     *
 =============================================================================*/

#ifndef __WF_PROTECT_H__
#define __WF_PROTECT_H__

#ifdef __cplusplus
    extern "C" {
#endif
/*============================ INCLUDES ======================================*/
#include "stdint.h"
#include "stdbool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef OPERATE_RET (*mqc_proto_801_4_user_cb)(IN ty_cJSON *root_json);

/*============================ PROTOTYPES ====================================*/
int tuya_wifi_protect_is_switching(void);
int tuya_wifi_protect_init(void);

int tuya_wifi_protect_register_cb(mqc_proto_801_4_user_cb cb);

#ifdef __cplusplus
} 
#endif

#endif
