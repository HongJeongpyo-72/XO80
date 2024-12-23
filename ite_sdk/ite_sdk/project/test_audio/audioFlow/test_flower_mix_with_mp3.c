#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "flower.h"
#include "sdl/main_processor_message_queue.h"//mp3


//======================================================//
IteFlower *flower = NULL;

static void *event_loop(void *arg)//mp3
{
    while(1)
    {
        uint16_t nAudioPluginRegister = 0;
        nAudioPluginRegister = getAudioPluginMessageStatus();
        if (((nAudioPluginRegister & 0xc000)>>14) == SMTK_AUDIO_PROCESSOR_ID)
        {
            // do audio api
            //printf("nAudioPluginRegister 0x%x \n",nAudioPluginRegister);
            smtkMainProcessorExecuteAudioPluginCmd(nAudioPluginRegister);
        }
        
        usleep(5000);
    } 
}

/*********************/
/*   FC 1: A->B->C   */
/*********************/
void* TestFunc(void* arg)
{
	//for mp3
	pthread_t       task;    
    itpInit();
    pthread_create(&task, NULL, event_loop, NULL);
    ///
    
	AudioInit();
    sleep(2);
    flower = IteStreamInit();

    //IteFChain fc1=s->fc;
    const char *filename="a:/test_1ch.mp3";
	char fileinsert[64];
	
    flow_start_sound_play_mp3(flower,filename,Repeat);
	flow_set_dac_level(50);
	
	int i = 0;
	int j = 0;
	sleep(10);
	for(j = 0; j < 3; j++){
		for(i = 0; i < 2; i++){
			sprintf(fileinsert,"a:/%d_2chn.wav",i);
	    	flow_mix_sound_play_mp3(flower,fileinsert);
			if(i == 1){
				flow_mix_sound_play_mp3(flower,NULL);	
			}	
		}
		sleep(10);
	}
    while(1){
            sleep(10);
            if(flower->audiostream==NULL) break;
    }

    flow_stop_sound_play_mp3(flower);

    printf("Test Task done\n");
}
