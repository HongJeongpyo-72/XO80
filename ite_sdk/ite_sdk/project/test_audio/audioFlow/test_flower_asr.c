#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "flower.h"

//======================================================//
IteFlower *flower = NULL;

/*********************/
/*   FC 1: A->B->C   */
/*********************/
void* TestFunc(void* arg)
{
    itpInit();
    sleep(2);
    flower = IteStreamInit();

    //IteFChain fc1=s->fc;

    flow_start_asr(flower,NULL);

    while(1){
        //printf("aaaa\n");
        sleep(5);
    }
    flow_stop_asr(flower);

    printf("Test Task done\n");
}
