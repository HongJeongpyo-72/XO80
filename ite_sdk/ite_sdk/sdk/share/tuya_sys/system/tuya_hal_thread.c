
#include "tuya_hal_thread.h"
#include "../errors_compat.h"
#include <pthread.h>
#include "lwip/sys.h"


int tuya_hal_thread_create(THREAD_HANDLE* thread,
                            const char* name,
                            uint32_t stack_size,
                            uint32_t priority,
                            THREAD_FUNC_T func,
                            void* const arg)
{
    pthread_t task;
    pthread_attr_t attr;
    struct sched_param param;
    uint32_t s_size = stack_size;
    int ret = 0;

    //if (NULL == thread) {
    //    return OPRT_INVALID_PARM;
    //}

    if(s_size < 25*1024)
        s_size = 25*1024;

    if(s_size > 65535*4)
        s_size = 65535*4;

ithPrintf("\n&tuya_hal_thread_create =%d \n",s_size);

    pthread_attr_init(&attr);
    param.sched_priority = priority - 2;
    pthread_attr_setschedparam(&attr, &param);
 //   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, s_size);
    ret = pthread_create(&task, &attr, func, arg);
    //ret = xTaskCreate(func, name, stack_size/sizeof(portSTACK_TYPE), arg, tskIDLE_PRIORITY, thread);
	if(ret) 
    {
        return OPRT_THRD_CR_FAILED;
    }

    return OPRT_OK;
}

int tuya_hal_thread_release(THREAD_HANDLE thread)
{
ithPrintf("\n&&&&&&&&&&&&&&&&7tuya_hal_thread_release \n");
    if (NULL == thread) {
        return OPRT_INVALID_PARM;
    }

   // pthread_join(thread, NULL);
    return OPRT_OK;
}

int tuya_hal_thread_is_self(THREAD_HANDLE thread, bool* is_self)
{
#if 0
    if (NULL == thread || NULL == is_self) {
        return OPRT_INVALID_PARM;
    }

    THREAD_HANDLE self_handle = rda_thread_get_id();
    if (NULL == self_handle) {
        return OPRT_COM_ERROR;
    }

    *is_self = (thread == self_handle);
#endif
    return OPRT_OK;
}

int tuya_hal_thread_set_self_name(const char* name)
{
    return OPRT_OK;
}

