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
 	const char *fileinsert="a:/0.wav a:/1.wav a:/2.wav a:/3.wav";
    
    flow_start_soundrw(flower,8000);//start record 

	while(1){
            IteAudioFlower *s=flower->audiostream;
            flow_mix_sound_play(flower,fileinsert);
            sleep(10);
            if(flower->audiostream==NULL) break;
    }
    flow_stop_soundrw(flower);//stop record


    printf("Test Task done\n");
}
