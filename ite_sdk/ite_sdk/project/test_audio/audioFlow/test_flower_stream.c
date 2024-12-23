#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "flower.h"
#include "sdl/main_processor_message_queue.h"
#include "ite/audio.h"
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

int Data_get(FILE *d,unsigned char *ptr,int bytes){
    int err;
    err = fread(ptr,1,bytes,d);
    if (err>=0){
        if (err!=0){
            if (err<bytes)
                memset(ptr+err,0,bytes-err);
        }
    }
    return err;
}

static void *file_read(void *arg)
{
    FILE* fd=NULL;
    const char *filename="a:/test.mp3";// or test.mp3 ,wma aac
    unsigned char *psrc=NULL;
    int bsize=512;
    int err;
    int bufSize;
    psrc = (unsigned char *)malloc(bsize);
    
    if (filename != NULL)
    {
        if(fd) {
            fclose(fd);
            fd=NULL;
        }
        fd = fopen(filename, "rb");
        if(fd){
            printf("file %s exit\n",filename);
        }else{
            printf("file %s not exit\n",filename);
        }
    }else{
        printf("file %s not exit\n",filename);
    }
    
    while(1)
    {
        err=0;
        if(err=Data_get(fd,psrc,bsize)){
            streamQPut(psrc,bsize);
            if(err<bsize){
                if(1){
                    streamQPut(psrc,0);//eof set;
                    printf("file read eof\n");
                    break;
                }else{
                    fseek(fd, 0, SEEK_SET);/*loop play*/
                }                
            }
        }

        usleep(10000);
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
    flower = IteStreamInit();//init flower
    streamQInit(ITE_MP3_DECODE);//init stream Q 
    pthread_create(&task, NULL, file_read, NULL);//stream put
    StreamFlowStart(flower,sound_playback_event_handler);//mp3 stream play
    
    flow_set_dac_level(10);
    while(1) {
        sleep(2);
        //StreamFlowPause(flower);
        //sleep(2);
        //StreamFlowResume(flower);
        if(flower->audiostream==NULL) break;
    }

    StreamFlowStop(flower);//stop stream play
    streamQUninit();//
    printf("Test Task done\n");
}
