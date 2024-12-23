#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/ith.h"
#include "ite/itp.h"


#if defined(CFG_TEST_SD_AMEBA)
void* Ameba_Test(void* arg)
#else
void* TestFunc(void* arg)
#endif
{
    pthread_t task;
    pthread_attr_t attr;
    ITPEthernetSetting setting = {0};

#if defined(CFG_TEST_AMEBA_LWIP)
    // init pal
    itpInit();
#endif

    setting.dhcp = 0;
    // ipaddr
    setting.ipaddr[0] = 192;
    setting.ipaddr[1] = 168;
    setting.ipaddr[2] = 1;
    setting.ipaddr[3] = 249;
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

    // do the test
    for (;;)
    {
#if defined(CFG_POWER_SLEEP)
        sleep(60);
        ioctl(ITP_DEVICE_POWER, ITP_IOCTL_SLEEP, NULL);
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);
#else
        sleep(1);
#endif
    }
    
    return NULL;
}

