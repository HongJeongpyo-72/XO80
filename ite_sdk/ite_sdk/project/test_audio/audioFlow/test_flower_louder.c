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
    //char *filepath="A:/rec1.wav";
 
    
    flow_start_soundrw(flower,8000);//start record
      
    /*sleep(1);//wait 10sec
    start_rec_wav(flower,filepath);
    sleep(5);
    stop_rec_wav(flower);
    sleep(1);
    */
    while(1){
        sleep(10);
    }
    flow_stop_soundrw(flower);//stop record


    printf("Test Task done\n");
}
