#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "fat/api/api_fat.h"
#include "ite/itp.h"


extern void* Card_Test(void* arg);
extern void* Ameba_Test(void* arg);

void* TestFunc(void* arg)
{
    pthread_t task;
    pthread_attr_t attr;

    // init pal
    itpInit();

    pthread_attr_init(&attr);
    pthread_create(&task, &attr, Card_Test, NULL);
    pthread_create(&task, &attr, Ameba_Test, NULL);

    while(1) 
        sleep(1);
}


