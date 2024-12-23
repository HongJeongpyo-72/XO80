#include <pthread.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "ite/ite_sd.h"
#include "../driver/net/ameba_sdio/include/freertos/ite_ndis_ameba.h"


static void rx_cb(void *arg, void *packet, int len)
{
    int i;
    uint8_t *data = (uint8_t*)packet;

    printf("\r\n[%04d]", len);
    for (i = 0; i < 14; i++)
    {
        printf("%02x ", data[i]);
    }
    printf("\r\n");

}

extern caddr_t __heap_start__;
extern caddr_t __heap_end__;

void heapStatHandler(void)
{
#if 1
    struct mallinfo mi = mallinfo();
    unsigned long total = (unsigned int)&__heap_end__ - (unsigned int)&__heap_start__;

    // heap usage
    printf("HEAP newlib: usage=%d/%d(%d%%),addr=0x%X\n",
        mi.uordblks,
        total,
        (int)(100.0f * mi.uordblks / total),
        &__heap_start__);
#endif

#ifdef CFG_DBG_RMALLOC
    Rmalloc_stat(__FILE__);
#endif

}

void* TestFunc(void* arg)
{
    int rc;
    SD_CARD_INFO card_info = { 0 };
    struct net_device *netdev;

    iteAmebaSdioWifiRegister();
again:
    heapStatHandler();
    iteSdcInitialize(SD_1, &card_info);

    netdev = iteAmebaGetNetdev();
    rc = iteAmebaOpen(netdev, rx_cb, NULL);
    if (rc)
        printf("iteAmebaOpen() rc = %x \n", rc);


    iteSdcTerminate(SD_1);
    sleep(2);
    goto again;

    // do the test
    for (;;)
    {
        sleep(1);
    }
    
    return NULL;
}

