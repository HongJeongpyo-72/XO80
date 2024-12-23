#include <string.h>
#include "wifi_host_os_api.h"

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_queue_t wifi_os_queue_create( uint32_t queue_len, uint32_t item_size)
{
    return (os_queue_t) xQueueCreate(queue_len, item_size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_queue_delete(os_queue_t queue)
{
    vQueueDelete(queue);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_receive(os_queue_t queue, void *buf, struct timeval *wait_time)
{
    if (queue == NULL)
        return -2;

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueReceive(queue, buf, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send(os_queue_t queue, void *item, struct timeval *wait_time)
{
    TickType_t ticks;

    if (queue == NULL)
        return -1;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSend(queue, item, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send_front(os_queue_t queue, void *item, struct timeval *wait_time)
{
    if (queue == NULL)
        return -1;

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSendToFront(queue, item, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send_from_isr(os_queue_t queue, void *item)
{
    if (queue == NULL)
        return -1;

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueSendToBackFromISR(queue, item, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_semaphore_t wifi_os_semphr_create_binary(void)
{
    return (os_semaphore_t) xSemaphoreCreateBinary();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_take(os_semaphore_t semphr, struct timeval *wait_time)
{
    TickType_t ticks;

    if (semphr == NULL)
        return -1;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xSemaphoreTake((SemaphoreHandle_t)semphr, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_give(os_semaphore_t semphr)
{
    if (semphr == NULL)
        return -1;

    if(pdTRUE == xSemaphoreGive((SemaphoreHandle_t)semphr))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_semphr_delete(os_semaphore_t semphr)
{
    if (semphr != NULL)
        vSemaphoreDelete((SemaphoreHandle_t)semphr);
}


/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_switch_context(void)
{
    taskYIELD();
}


/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_switch_context_from_isr(void)
{
    portYIELD_FROM_ISR(pdTRUE);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_task_create(task_func_t handler,
                        char *name,
                        uint16_t stack_size,
                        void *params,
                        UBaseType_t prio,
                        os_task_t *task_handle
                        )
{
    if(pdPASS == xTaskCreate(handler,name,stack_size/4,params,prio,task_handle))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void* wifi_os_malloc(size_t size)
{
    return pvPortMalloc(size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void* wifi_os_zalloc(size_t size)
{
    //return pvPortCalloc(1, size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_free(void *mem)
{
    vPortFree(mem);
}

