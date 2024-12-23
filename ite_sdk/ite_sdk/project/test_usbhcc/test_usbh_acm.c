#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "ite/itp.h"

#define BUF_LEN     (16*1024)
static char rbuf[BUF_LEN];




static void* DoTest(void* arg)
{
    char buf[256], len;
    FILE* f = NULL;
    int filesize, rlen;
    int read_size = 0, ret;
    char filepath[PATH_MAX];

    sleep(2);
    
    while (ioctl(ITP_DEVICE_USBHACM, ITP_IOCTL_IS_CONNECTED, NULL) == 0) {
        printf(".\n");
        sleep(1);
    }

    printf("\n\nUSB Acm is ready...... ! \n\n");

    /* wait internal storage is ready */
    {
        ITPDriveStatus* driveStatusTable;
        ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

        printf("wait internal storage drive A: available....\n");
        while (1)
        {
            ITPDriveStatus* driveStatus = &driveStatusTable[0];
            printf("drive A: avail=%d\n", driveStatus->avail);
            if (driveStatus->avail)
                break;

            sleep(1);
        }
    }

    sleep(3);

    // device -> host: "wait command > send filename filesize"
    rlen = read(ITP_DEVICE_USBHACM, rbuf, BUF_LEN);
    rbuf[rlen] = '\0';
    printf("Device: %s \n", rbuf); 

    if (ioctl(ITP_DEVICE_USBHACM, ITP_IOCTL_IS_CONNECTED, NULL))
    {
        sprintf(filepath, "A:/%s", CFG_USBH_ACM_FILENAME);
        f = fopen(filepath, "rb");
        fseek(f, 0, SEEK_END);
        filesize = ftell(f);

        // host -> device: send cmd to device
        sprintf(buf, "send %s %d\n", CFG_USBH_ACM_FILENAME, filesize);
        printf("%s\n", buf);
        len = strlen(buf);
        write(ITP_DEVICE_USBHACM, buf, len); // sen

        // device -> host: just receive wait string....
        rlen = read(ITP_DEVICE_USBHACM, rbuf, BUF_LEN);
        rbuf[rlen] = '\0';
        printf("Device: %s \n", rbuf);
            
        // host -> device: start to send file data
        rlen = BUF_LEN;
        read_size = 0;
        fseek(f, 0, SEEK_SET);
        while (read_size < filesize) {
            if (rlen >(filesize - read_size))
                rlen = (filesize - read_size);
            ret = fread(rbuf, 1, rlen, f);
            if (ret != rlen) {
                printf("fread() %d != %d \n", ret, rlen);
                goto end;
            }
            rlen = write(ITP_DEVICE_USBHACM, rbuf, rlen);
            read_size += rlen;
            printf(" W: %d \n", read_size);
        }
        ret = fclose(f);

        printf("Host: send file done!! \n\n");

        rlen = read(ITP_DEVICE_USBHACM, rbuf, BUF_LEN);
        rbuf[rlen] = '\0';
        printf("Device: %s \n", rbuf);
    }

end:
    while (1);

    return NULL;
}

void* TestFunc(void* arg)
{
    pthread_t thread;
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    param.sched_priority = 4;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&thread, &attr, DoTest, NULL);

    while (1)
        sleep(1);

    return NULL;
}


