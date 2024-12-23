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

/*********************/
/*   FC 1: A->B->C   */
/*********************/
void* TestFunc(void* arg)
{
    pthread_t       task;    
    itpInit();
    pthread_create(&task, NULL, event_loop, NULL);
    AudioInit();
    sleep(2);
    flower = IteStreamInit();

    //IteFChain fc1=s->fc;
    const char *filename="a:/test.wav";// or test.mp3 ,wma aac

    flow_start_sound_play(flower,filename,Repeat);
    flow_set_dac_level(30);
    while(1) {
        sleep(1);
        if(flower->audiostream==NULL) break;
    }

    flow_stop_sound_play(flower);

    printf("Test Task done\n");
}
