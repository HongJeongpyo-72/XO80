#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "flower.h"
#include "sdl/main_processor_message_queue.h"


//======================================================//
IteFlower *flower = NULL;

/****audio mgr :mp3 play***/

static void *event_loop(void *arg)
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
/**/

/*********************/
/*   FC 1: A->B->C   */
/*********************/
void* TestFunc(void* arg)
{
    itpInit();
/*audio mgr :mp3 play needed(filename = test.mp3)*/
    pthread_t       task; 
    pthread_create(&task, NULL, event_loop, NULL);
    AudioInit();
/**/
    sleep(2);
    flower = IteStreamInit();

    const char *filename="a:/test.mp3";//a:/test.wav

	ResmplePlayASRAECFlowStart(flower,filename,Repeat,sound_playback_event_handler);
    /*
    PlayFlowStart(flower,filename,Repeat,sound_playback_event_handler);
    SynASRFlowStart(flower,sound_playback_event_handler);
    */
	flow_set_adc_level(100);
    flow_set_dac_level(30);
    while(1) {
        sleep(1);
    }
    
	ResmplePlayASRAECFlowStop(flower);
    /*
    SynASRFlowStop(flower);
    PlayFlowStop(flower);
    */
    printf("Test Task done\n");
}
