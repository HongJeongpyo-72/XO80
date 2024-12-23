/*
 * Copyright (c) 2011 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * PAL Ethernet functions.
 *
 * @author Jim Tan
 * @version 1.0
 */
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "itp_cfg.h"
#include "ite/ite_mac.h"
#include "ite/itp.h"

typedef struct flash_param
{
	int len;
	uint8_t* data;
    uint32_t addr;
}flash_param;

static pthread_mutex_t paramWriteMutex;

int ParamRead(uint8_t *param)
{
    uint8_t *buf = NULL;
    uint32_t pos, blocksize = 0;
    int ret, i;
    uint32_t offset = 0;
    int fd = -1;
    flash_param *data = (flash_param *)param;

   // LOG_INFO "Read to %08x...\n", data->addr LOG_END

    fd = open(":nor", O_RDWR, 0);
    if (fd == -1)
    {
        LOG_ERR "open device error: %d\n", fd LOG_END
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        LOG_ERR "get block size error\n" LOG_END
            ret = __LINE__;
        goto end;
    }
    
    pos = data->addr / blocksize;
    if (lseek(fd, pos, SEEK_SET) != pos)
    {
        LOG_ERR "seek to sys param position %08x(%d) error\n", data->addr, pos LOG_END 
        goto end;
    }

    assert(blocksize >= 8);
    buf = (uint8_t *)malloc(blocksize);
    if (!buf)
    {
        ret = __LINE__;
        goto end;
    }

    ret = read(fd, buf, 1);
    if (ret != 1)
    {
        LOG_ERR "read sys param error: %d != 1\n", ret LOG_END
        goto end;
    }
    memcpy(data->data, buf, data->len);

end:
    if (fd != -1)
        close(fd);

    if (buf)
        free(buf);

    return ret;
}

void ParamWrite(uint8_t *param)
{
    uint8_t *buf = NULL;
    uint32_t pos, blocksize = 0;
    int ret, i;
    uint32_t offset = 0;
    int fd = -1;
    
    pthread_mutex_lock(&paramWriteMutex);

    flash_param *data = (flash_param *)param;

    LOG_INFO "Write to %08x...\n", data->addr LOG_END

    fd = open(":nor", O_RDWR, 0);
    LOG_INFO "nor fd: 0x%X\n", fd LOG_END

    if (fd == -1)
    {
        LOG_ERR "open device error: %d\n", fd LOG_END
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        LOG_ERR "get block size error\n" LOG_END
        goto end;
    }
    LOG_INFO "blocksize=%d\n", blocksize LOG_END

    pos = data->addr / blocksize;

    if (lseek(fd, pos, SEEK_SET) != pos)
    {
        LOG_ERR "seek to sys param position %08x(%d) error\n", data->addr, pos LOG_END
        goto end;
    }

    assert(blocksize >= 8);
    buf = (uint8_t *)malloc(blocksize);
    if (!buf)
    {
        ret = __LINE__;
        goto end;
    }

    memcpy(buf, data->data, data->len);

    if (write(fd, buf, 1) != 1)
    {
        LOG_ERR "write sys param fail\n" LOG_END 
        goto end;
    }

end:
    
    if (fd != -1)
        close(fd);

    if (buf)
        free(buf);

    pthread_mutex_unlock(&paramWriteMutex);
}

static int SysParamIoctl(int file, unsigned long request, void *ptr, void *info)
{
    switch (request)
    {
    case ITP_IOCTL_INIT:
		//printf(" SysParamIoctl mutex init \n");
        pthread_mutex_init(&paramWriteMutex, NULL);
        break;

    case ITP_IOCTL_READ_PARAM:
		//printf(" SysParamIoctl ITP_IOCTL_READ_PARAM \n");
        ParamRead((uint8_t *)ptr);
        break;

    case ITP_IOCTL_WRITE_PARAM:
		
		//printf(" SysParamIoctl ITP_IOCTL_WRITE_PARAM \n");
        ParamWrite((uint8_t *)ptr);
        break;

    default:
        errno = (ITP_DEVICE_SYS_PARAM << ITP_DEVICE_ERRNO_BIT) | 1;
        return -1;
    }
    return 0;
}

const ITPDevice itpDeviceSysParam =
{
    ":sysparam",
    itpOpenDefault,
    itpCloseDefault,
    itpReadDefault,
    itpWriteDefault,
    itpLseekDefault,
    SysParamIoctl,
    NULL
};
