/***********************************************************
*  File: uni_semaphore.c
*  Author: nzy
*  Date: 120427
***********************************************************/
#define _UNI_SEMAPHORE_GLOBAL
//#include "rda_sys_wrapper.h"

#include "ite/itp.h"
#include "tuya_hal_semaphore.h"
#include "tuya_hal_system_internal.h"
#include "../errors_compat.h"


/***********************************************************
*************************micro define***********************
***********************************************************/

#if 0
#define LOGD PR_DEBUG
#define LOGT PR_TRACE
#define LOGN PR_NOTICE
#define LOGE PR_ERR
#else
#define LOGD(...) printf("[SEM DEBUG]" __VA_ARGS__)
#define LOGT(...) printf("[SEM TRACE]" __VA_ARGS__)
#define LOGN(...) printf("[SEM NOTICE]" __VA_ARGS__)
#define LOGE(...) printf("[SEM ERROR]" __VA_ARGS__)

#define  printf_sema //printf

#endif



typedef struct
{
    sem_t sem;
}SEM_MANAGE,*P_SEM_MANAGE;

static sem_t *mite[512] = {0};
static int i = 0;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: CreateSemaphore 创建一个信号量
*  Input: reqSize->申请的内存大小
*  Output: none
*  Return: NULL失败，非NULL成功
*  Date: 120427
***********************************************************/
static SEM_HANDLE CreateSemaphore(void)
{
    P_SEM_MANAGE pSemManage;
    
    pSemManage = (P_SEM_MANAGE)tuya_hal_internal_malloc(sizeof(SEM_MANAGE));

    return (SEM_HANDLE)pSemManage;
}

/***********************************************************
*  Function: InitSemaphore
*  Input: semHandle->信号量操作句柄
*         semCnt
*         sem_max->no use for linux os
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
static int InitSemaphore(const SEM_HANDLE semHandle, const uint32_t semCnt,\
                                 const uint32_t sem_max)
{
    if(!semHandle)
        return OPRT_INVALID_PARM;
        
    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    int ret = sem_init(&pSemManage->sem, 0, semCnt);
    if(ret != OPRT_OK)
    {
        printf_sema("semaphore init fails %u %u\n", semCnt, sem_max);
        return OPRT_INIT_SEM_FAILED;
    }else
    {
        printf_sema("$$$$$$$$$$$$$$$$$$malloc = 0x%x\n", &pSemManage->sem);
        mite[i] = &pSemManage->sem;
        i++;
    }
    return OPRT_OK;
}

int tuya_hal_semaphore_create_init(SEM_HANDLE *pHandle, const uint32_t semCnt, \
                          const uint32_t sem_max)
{    
    if(NULL == pHandle)
    {
        printf_sema("invalid param\n");
        return OPRT_INVALID_PARM;
    }
    
    *pHandle = CreateSemaphore();
    if(*pHandle == NULL)
    {
        printf_sema("malloc fails\n");
        return OPRT_MALLOC_FAILED;
    }

    int ret = InitSemaphore(*pHandle, semCnt, sem_max);

    if(ret != OPRT_OK)
    {
        tuya_hal_internal_free(*pHandle);
        *pHandle = NULL;
        LOGE("semaphore init fails %u %u\n", semCnt, sem_max);
        return ret;
    }

    return ret;
}

/***********************************************************
*  Function: WaitSemaphore
*  Input: semHandle->信号量操作句柄
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_semaphore_wait(const SEM_HANDLE semHandle)
{
	if(!semHandle) 
        return OPRT_INVALID_PARM;

    P_SEM_MANAGE pSemManage;
    int ret = 0;
    pSemManage = (P_SEM_MANAGE)semHandle;
    printf_sema("\n&&tuya_hal_semaphore_wait=0x%x\n", &pSemManage->sem);
    ret = sem_wait(&pSemManage->sem);
    printf_sema("\n&&tuya_hal_semaphore_wait status=%d(0x%x)\n", ret, &pSemManage->sem);
    if(ret == 0)
        return OPRT_OK;
    else
        return OPRT_WAIT_SEM_FAILED;
}

/***********************************************************
*  Function: PostSemaphore
*  Input: semHandle->信号量操作句柄
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_semaphore_post(const SEM_HANDLE semHandle)
{
	if(!semHandle) 
        return OPRT_INVALID_PARM;
    
    P_SEM_MANAGE pSemManage;
    int ret = 0;
    pSemManage = (P_SEM_MANAGE)semHandle;
    
    
    for(int j = 0; j < 512; j++)
    {    
        if(&pSemManage->sem == mite[j])
        {
            printf_sema("\n&&tuya_hal_semaphore_post=0x%x\n", &pSemManage->sem);
            if(&pSemManage->sem > 0x20000000)
                while(1);
            ret = sem_post(&pSemManage->sem);
            if(ret == 0)
                return OPRT_OK;
            else
                return OPRT_POST_SEM_FAILED;
        }
    }
    //if(false == tuya_hal_system_isrstatus())
        //sem_post(&pSemManage->sem);
    //else
    //    itpSemPostFromISR(&pSemManage->sem);
    printf_sema("\n&&_hal_semaphore_post=0x%x\n", &pSemManage->sem);
            if(&pSemManage->sem > 0x20000000)
                while(1);
    return OPRT_INVALID_PARM;
}

/***********************************************************
*  Function: ReleaseSemaphore
*  Input: semHandle->信号量操作句柄
*  Output: none
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
int tuya_hal_semaphore_release(const SEM_HANDLE semHandle)
{
    if(!semHandle)
        return OPRT_INVALID_PARM;

    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;
    printf_sema("\n&&tuya_hal_semaphore_release=0x%x\n", &pSemManage->sem);
    sem_destroy(&pSemManage->sem);
    tuya_hal_internal_free(semHandle);
    
    return OPRT_OK;
}



