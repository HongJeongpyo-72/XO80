#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/ith.h"
#include "ite/itp.h"
#include "ite/ite_mac.h"

#define CFG_PING_IP     "192.168.1.100"

static void DoPing(void)
{
    //printf("\n\nping: 172.217.160,78 \n\n");
    //ping_set_target("172.217.160.78"); // google(172.217.160.78)
    printf("\n\nping: %s \n\n", CFG_PING_IP);
    ping_set_target(CFG_PING_IP);
    ping_init();
}

void* TestEthernet(void* arg)
{
    pthread_t task;
    pthread_attr_t attr;
    ITPEthernetSetting setting = {0};

    setting.dhcp = 0;
    // ipaddr
    setting.ipaddr[0] = 192;
    setting.ipaddr[1] = 168;
    setting.ipaddr[2] = 1;
    setting.ipaddr[3] = 1;
    // netmask
    setting.netmask[0] = 255;
    setting.netmask[1] = 255;
    setting.netmask[2] = 255;
    setting.netmask[3] = 0;
    // gateway
    setting.gw[0] = 192;
    setting.gw[1] = 168;;
    setting.gw[2] = 1;
    setting.gw[3] = 254;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);
    
    /* wait link up */
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL)) {
        sleep(1);
        putchar('.');
        fflush(stdout);
    }
    printf("Network connected.. \n");

    DoPing();

    // do the test
    for (;;)
    {
        sleep(1);
    }
    
    return NULL;
}

