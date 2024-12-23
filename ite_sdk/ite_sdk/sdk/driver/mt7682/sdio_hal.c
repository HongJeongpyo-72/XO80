#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sdio_io.h"



/* ========================================================== */
int sdio_func1_rd(struct sdio_func *func, uint32_t u4Register, void *pBuffer, uint32_t Length);
int sdio_func1_wr(struct sdio_func *func, uint32_t u4Register, void *pBuffer, uint32_t Length);



/* ========================================================== */


/* ========================================================== */

s32 init_xmit_priv(struct sdio_func *func)
{

}

s32 init_recv_priv(struct sdio_func *func)
{

}


/* ========================================================== */

void set_hal_ops(struct sdio_func *func)
{
    struct hal_ops *pHalFunc;

    pHalFunc->init_xmit_priv = &init_xmit_priv;
    pHalFunc->init_recv_priv = &init_recv_priv;
}


int sdio_func1_rd(struct sdio_func *func, uint32_t u4Register, void *pBuffer, uint32_t Length)
{
    int ret ;

    if (pBuffer == NULL)
        return -ENOBUFS;

    sdio_claim_host(func);
    ret = iteSdioMemcpyFromFifo(func, pBuffer, u4Register, Length);
    if (ret != 0) {
        printf("[ERR] function 1 read fail : addr : 0x%X , size : %d, err_ret: 0x%X\r\n\n", u4Register, Length, ret) ;
    }
    sdio_release_host(func);

    return ret;
}

int sdio_func1_wr(struct sdio_func *func, uint32_t u4Register, void *pBuffer,  uint32_t Length)
{
    int ret, i;

    if (pBuffer == NULL)
        return -ENOBUFS;

    sdio_claim_host(func);
    ret = iteSdioMemcpyToFifo(func, u4Register, pBuffer, Length);
    if (ret != 0) {
        printf("[ERR] function 1 write fail : addr : 0x%x , size : %d, err_ret: 0x%x\r\n", u4Register, Length, ret);
        printf("Error packet content = ") ;

        for (i = 0; i < Length; i++) {
            printf("%x ", *(unsigned char *)(pBuffer + i));
        }

        printf(" \r\n ");
    }
    sdio_release_host(func);

    return ret;
}

