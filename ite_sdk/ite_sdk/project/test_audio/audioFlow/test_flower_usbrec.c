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
 
    const char *filename="a:/rec.wav";
    
    flow_start_sound_record(flower,filename,8000);//start record
      
    sleep(10);//wait 10sec

    flow_stop_sound_record(flower);//stop record
    

    printf("Test Task done\n");
}
