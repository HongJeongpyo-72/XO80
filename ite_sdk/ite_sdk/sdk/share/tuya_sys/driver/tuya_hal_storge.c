/***********************************************************
*  File: uni_storge.c 
*  Author: nzy
*  Date: 20170920
***********************************************************/
#define __UNI_STORGE_GLOBALS
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include "tuya_hal_storge.h"
#include "tuya_hal_mutex.h"
#include "tuya_cloud_error_code.h"
#include "../errors_compat.h"
#include "tuya_cloud_types.h"
#include "ite/itp.h"

#define PRINTF      printf
#define LOG_ERR     PRINTF("ERR:"
#define LOG_DBG     PRINTF("DEBUG:"
#define LOG_INFO    PRINTF("INFO:"
#define LOG_END         );

#define PR_FLASH_DBG

#define BLOCK_SZ      0x10000
/***********************************************************
*************************micro define***********************
***********************************************************/
#if 1
#define PARTITION_SIZE (1 << 12) /* 4KB */
//#define PARTITION_SIZE FLASH_ERASE_4KB  /* 4KB */
#define FLH_BLOCK_SZ PARTITION_SIZE

// flash map
#define TUYA_FLASH_ADDR (CFG_NOR_RESERVED_SIZE - 0x20000)
#define TUYA_FLASH_SWAP_ADDR (TUYA_FLASH_ADDR - 0x10000)

#define SIMPLE_FLASH_START (TUYA_FLASH_ADDR)
#define SIMPLE_FLASH_SIZE 0x10000 // 64K

#define SIMPLE_FLASH_SWAP_START (TUYA_FLASH_ADDR - 0x4000)
#define SIMPLE_FLASH_SWAP_SIZE 0x4000 // 16K

#define SIMPLE_FLASH_KEY_ADDR  (SIMPLE_FLASH_SWAP_START - 0x1000)//4k
#else
#define PARTITION_SIZE (1 << 16) /* 4KB */
//#define PARTITION_SIZE FLASH_ERASE_4KB  /* 4KB */
#define FLH_BLOCK_SZ PARTITION_SIZE

// flash map
#define SIMPLE_FLASH_START (0x300000 + (0x200000 - 0x100000))
#define SIMPLE_FLASH_SIZE 0x100000 // 64K

#define SIMPLE_FLASH_SWAP_START (0x300000 + (0x200000 - 0x100000 - 0x40000))
#define SIMPLE_FLASH_SWAP_SIZE 0x40000 // 16K

#define SIMPLE_FLASH_KEY_ADDR  (0x300000 + (0x200000 - 0x100000 - 0x40000 - 0x10000))//4k

#endif
// 0x8000+0x1000+0x4000+0x10000=0x1d000=116K
/***********************************************************
*************************variable define********************
***********************************************************/

static UNI_STORAGE_DESC_S storage = {
    SIMPLE_FLASH_START,
    SIMPLE_FLASH_SIZE,
    FLH_BLOCK_SZ,
    SIMPLE_FLASH_SWAP_START,
    SIMPLE_FLASH_SWAP_SIZE,
    SIMPLE_FLASH_KEY_ADDR
};

static MUTEX_HANDLE FlashMutex = NULL;
//static flash_t Flash;
//static osMutexDef_t os_mutex_def_mylinks_flash;
static UF_PARTITION_TABLE_S uf_file = {
    .sector_size = PARTITION_SIZE,
    .uf_partition_num = 1,
    .uf_partition = {
        {SIMPLE_FLASH_KEY_ADDR - 0x8000, 0x8000}
    }
};

typedef struct flash_param
{
	int len;
	uint8_t* data;
    uint32_t addr;
}flash_param;

flash_param* gSimple_flash_data = NULL;
flash_param* gSwap_flash_data = NULL;

/***********************************************************
**********
***************function define********************
***********************************************************/

/***********************************************************
*  Function: tuya_hal_flash_init
*  Input: null
*  Output: null
*  Return: 0 success
***********************************************************/
int tuya_hal_flash_init(void)
{
    PR_FLASH_DBG("+++++++++++++++++++++++++++++\n");
    gSimple_flash_data = malloc(sizeof(flash_param));
    if(gSimple_flash_data!= NULL)
    {
        gSimple_flash_data->data = malloc(BLOCK_SZ);
        if(gSimple_flash_data->data != NULL)
        {
            gSimple_flash_data->len = BLOCK_SZ;
            gSimple_flash_data->addr = TUYA_FLASH_ADDR;
            ioctl(ITP_DEVICE_SYS_PARAM, ITP_IOCTL_READ_PARAM, (void*)gSimple_flash_data);
        }
    }

    gSwap_flash_data = malloc(sizeof(flash_param));
    if(gSwap_flash_data!= NULL)
    {
        gSwap_flash_data->data = malloc(BLOCK_SZ);
        if(gSwap_flash_data->data != NULL)
        {
            gSwap_flash_data->len = BLOCK_SZ;
            gSwap_flash_data->addr = TUYA_FLASH_SWAP_ADDR;
            ioctl(ITP_DEVICE_SYS_PARAM, ITP_IOCTL_READ_PARAM, (void*)gSwap_flash_data);
        }
    }

    return 0;
}

/***********************************************************
*  Function: uni_flash_read
*  Input: addr size
*  Output: dst
*  Return: none
***********************************************************/
int tuya_hal_flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    flash_param *data = NULL;

  //  LOG_INFO "[%s] addr:%08x size:%ld\n", __FUNCTION__, addr, size LOG_END

    if((addr - TUYA_FLASH_SWAP_ADDR) < BLOCK_SZ)
        data = gSwap_flash_data;
    else
        data = gSimple_flash_data;

    if(data == NULL || dst == NULL)
	{
		// LOG_INFO "[%s] fail. data:%08x dst:%08x\n", __FUNCTION__, data, dst LOG_END
		// sleep(1);			
        return -1;
	}

    memcpy(dst, data->data+addr%BLOCK_SZ, size); 
    
//	NorRead_raw_tuya(addr,dst,size);
  //  LOG_INFO "%02x %02x %02x %02x %02x %02x %02x %02x\n", *dst, *(dst+1), *(dst+2), *(dst+3), *(dst+4), *(dst+5), *(dst+6), *(dst+7) LOG_END
    return 0;
}

/***********************************************************
*  Function: uni_flash_write
*  Input: addr src size
*  Output: none
*  Return: none
***********************************************************/
int tuya_hal_flash_write(const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int m_swap_flash_index = 0;
    flash_param *data = NULL;

   // LOG_INFO "[%s] addr:%08x size:%ld\n", __FUNCTION__, addr, size LOG_END

    if((addr - TUYA_FLASH_SWAP_ADDR) < BLOCK_SZ)
    {
        data = gSwap_flash_data;
        m_swap_flash_index = 1;
    }
    else
    {
        data = gSimple_flash_data;
        m_swap_flash_index = 2;
    }

    if(src == NULL)
	{
        return -1;
	}

    memcpy(data->data+addr%BLOCK_SZ, src, size);
    ioctl(ITP_DEVICE_SYS_PARAM, ITP_IOCTL_WRITE_PARAM, (void*)data);//
	//NorWrite_raw_tuya(addr,src,size);	
    return 0;
}

/***********************************************************
*  Function: uni_flash_erase
*  Input: addr size
*  Output:
*  Return: none
***********************************************************/
int tuya_hal_flash_erase(const uint32_t addr, const uint32_t size)
{
    flash_param *data = NULL;
    
   // LOG_INFO "[%s] addr:%08x size:%ld\n", __FUNCTION__, addr, size LOG_END
    if((addr - TUYA_FLASH_SWAP_ADDR) < BLOCK_SZ)
        data = gSwap_flash_data;
    else
        data = gSimple_flash_data;

    if(data == NULL)
        return -1;

    memset(data->data+addr%BLOCK_SZ, 0xff, size);     
//	NorErase_raw_tuya(addr,size);

    return 0;
}

int tuya_hal_flash_reset_cfg()
{	
    tuya_hal_mutex_lock(FlashMutex);
    //printf("write to addr:%x, size:%d\n", addr, size);
  //  PR_FLASH_DBG("\n##tuya_hal_flash_reset_cfg # New APP Reset flash ##########\n");
    tuya_hal_flash_erase(SIMPLE_FLASH_START, SIMPLE_FLASH_SIZE*2);
    //rt_sf_erase(SIMPLE_FLASH_START, SIMPLE_FLASH_SIZE);

    tuya_hal_mutex_unlock(FlashMutex);

    return 0;
}

/***********************************************************
*  Function: uni_get_storge_desc
*  Input: none
*  Output: none
*  Return: UNI_STORGE_DESC_S
***********************************************************/
UNI_STORAGE_DESC_S* tuya_hal_storage_get_desc(void)
{
    return &storage;
}

UF_PARTITION_TABLE_S* tuya_hal_uf_get_desc(void)
{
    return &uf_file;
}


