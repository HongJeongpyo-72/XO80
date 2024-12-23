#include <pthread.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ite/itp.h"



void* TestFunc(void* arg)
{
    int connected = 0;

    for (;;)
    {
        if (connected) {
            if (!ioctl(ITP_DEVICE_USBDFSG, ITP_IOCTL_IS_CONNECTED, NULL)) {
                printf("usb device mst leave.... \n\n");
                connected = 0;
            }
        }
        else {
            if (ioctl(ITP_DEVICE_USBDFSG, ITP_IOCTL_IS_CONNECTED, NULL)) {
                printf("\n\nusb device mst enter.... \n");
                connected = 1;
            }
        }
        usleep(66000);
    }
}


