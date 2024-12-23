
#include "tuya_hal_memory.h"
#include "tuya_hal_system_internal.h"
#include "tuya_hal_mutex.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/
static TUYA_MALLOC_FUNC_T s_internal_malloc_func = NULL;
static TUYA_FREE_FUNC_T   s_internal_free_func   = NULL;

/***********************************************************
*************************function define********************
***********************************************************/

void *tuya_hal_system_malloc(const size_t size)
{
    return malloc(size);
}

void tuya_hal_system_free(void* ptr)
{
	volatile void* tmp;
	tmp =ptr;
    if(ptr > 0x4000000){
        printf("(ERROR)tuya_hal_system_freeXX=0x%x\n", ptr);
	return;}
    //usleep(10000);
    free(ptr);
}

void *tuya_hal_system_calloc(size_t numitems, size_t size)
{
	return calloc(numitems, size);
}

void* tuya_hal_system_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}


int tuya_hal_set_mem_func(TUYA_MALLOC_FUNC_T malloc_func, TUYA_FREE_FUNC_T free_func)
{
	printf("tuya_hal_set_mem_func malloc_func=0x%x free_func=0x%x\n", malloc_func,free_func);

    s_internal_malloc_func = malloc_func;
    s_internal_free_func = free_func;
    return 0;
}

void* tuya_hal_internal_malloc(const size_t size)
{
    if (s_internal_malloc_func) {
        return s_internal_malloc_func(size);
    } else {
        return tuya_hal_system_malloc( size);
    }
}

void tuya_hal_internal_free(void* ptr)
{
    if (s_internal_free_func) {
        s_internal_free_func(ptr);
    } else {
        tuya_hal_system_free(ptr);
    }
}

