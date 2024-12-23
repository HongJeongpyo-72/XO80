/***********************************************************
*  File: uni_mutex.c
*  Author: nzy
*  Date: 120427
***********************************************************/
#define _UNI_MUTEX_GLOBAL
#include <pthread.h>
#include "tuya_hal_mutex.h"
#include "tuya_hal_system_internal.h"
#include "../errors_compat.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
typedef void * THRD_MUTEX;

typedef struct
{
    pthread_mutex_t mutex;
}MUTEX_MANAGE,*P_MUTEX_MANAGE;

#ifndef os_printf
#define os_printf printf
#endif

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: CreateMutexAndInit 创建一个互斥量并初始化
*  Input: none
*  Output: pMutexHandle->新建的互斥量句柄
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_mutex_create_init(MUTEX_HANDLE *pMutexHandle)
{
    int ret = 0;
    if(!pMutexHandle)
        return OPRT_INVALID_PARM;
    
    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)tuya_hal_internal_malloc(sizeof(MUTEX_MANAGE));
    if(!(pMutexManage)){
		
        printf("##########tuya_hal_mutex_create_init OPRT_MALLOC_FAILED!!!\n");
        return OPRT_MALLOC_FAILED;
    }
    ret = pthread_mutex_init(&pMutexManage->mutex, NULL);
    if(ret)
    {
        tuya_hal_internal_free(pMutexManage);
        printf("##########tuya_hal_mutex_create_init ERROR!!!\n");
        return OPRT_INIT_MUTEX_FAILED;
    }
   // printf("##########tuya_hal_mutex_create_init(0x%X)\n", &pMutexManage->mutex);
    *pMutexHandle = (MUTEX_HANDLE)pMutexManage;

    return OPRT_OK;
}

/***********************************************************
*  Function: MutexLock 加锁
*  Input: mutexHandle->互斥量句柄
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_mutex_lock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
    {
        printf("##########tuya_hal_mutex_lock(0x%X) invalid\n", mutexHandle);
        return OPRT_INVALID_PARM;
    }    

    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;
    if (&pMutexManage->mutex == NULL || pMutexManage->mutex == NULL)
        printf("##########tuya_hal_mutex_lock(0x%X)\n", &pMutexManage->mutex);    
    pthread_mutex_lock(&pMutexManage->mutex);
    return OPRT_OK;
}

/***********************************************************
*  Function: MutexUnLock 解锁
*  Input: mutexHandle->互斥量句柄
*  Output: none4
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_mutex_unlock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
    {
        printf("##########tuya_hal_mutex_unlock(0x%X) invalid\n", mutexHandle);
        return OPRT_INVALID_PARM;
    }    

    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;  
    //printf("##########tuya_hal_mutex_unlock(0x%X)\n", &pMutexManage->mutex);  
    pthread_mutex_unlock(&pMutexManage->mutex);
    return OPRT_OK;
}

/***********************************************************
*  Function: ReleaseMutexManage 释放互斥锁管理结构资源
*  Input: mutexHandle->互斥量句柄
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_mutex_release(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
    {
        printf("##########tuya_hal_mutex_release(0x%X) invalid\n", mutexHandle);
        return OPRT_INVALID_PARM;
    }  

    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;

    pthread_mutex_destroy(&pMutexManage->mutex);
    tuya_hal_internal_free(mutexHandle);

    return OPRT_OK;
}


