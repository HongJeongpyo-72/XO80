#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"
#include "i2s/i2s.h" 
#include "test.h"
#include "wavheader.h"
#include "audio_ctrl.h"

#define ADC_MEMORY_SIZE (1024*1024*2) 

unsigned char *recdata;
uint32_t sizecount = 0; 

/**********************************************************/
void initAD(){
    /* init ADC */ 
    adc_buf = (uint8_t*)malloc(ADC_BUFFER_SIZE);
	memset((uint8_t*) adc_buf, 0, ADC_BUFFER_SIZE);
	memset((void*)&spec_adI, 0, sizeof(STRC_I2S_SPEC));
    spec_adI.channels					= 2;
    spec_adI.sample_rate				= 16000;
    spec_adI.buffer_size				= ADC_BUFFER_SIZE;
    spec_adI.is_big_endian				= 0;
	spec_adI.base_i2s				    = (uint8_t*) adc_buf;	
    spec_adI.base_hdmi[0]               = (uint8_t*) adc_buf;
    spec_adI.base_hdmi[1]               = (uint8_t*) adc_buf;
    spec_adI.base_hdmi[2]               = (uint8_t*) adc_buf;
    spec_adI.base_hdmi[3]               = (uint8_t*) adc_buf;
    spec_adI.sample_size				= 16;
    spec_adI.record_mode				= 1;
    spec_adI.from_LineIN				= 0;
    spec_adI.from_MIC_IN				= 1;
    audio_init_AD(&spec_adI);
    audio_pause_AD(1);
}


void recvoice(){
    
    uint8_t *psrc = NULL;   
    int i=0;
    int bsize = audio_get_bsize_from_bps(&spec_adI)*4;      
    psrc = (u8*)malloc(bsize);
    recdata = (uint8_t*)malloc(ADC_MEMORY_SIZE);
    memset(recdata,0,ADC_MEMORY_SIZE);
    i2s_pause_ADC(0);
    printf("start recording . . .\n");
    while(sizecount < ADC_MEMORY_SIZE){
        
        if((sizecount+bsize) > ADC_MEMORY_SIZE ) 
            break;

            
        if(audio_HDMIRX_Data_Recv(&spec_adI,psrc,bsize,0)){
            memcpy(recdata+sizecount,psrc,bsize);
            sizecount += bsize;
            //audio_AD_wait_time(&spec_adI,bsize);
            printf("sizecount=%d (%d)\n",sizecount,ADC_MEMORY_SIZE);
        }
    }
    printf("stop recording . . .\n");
    
    dump_i2s_reg();    
    
    audio_pause_AD(1);
}


void* TestFunc(void* arg)
{
    char *filename = "a:/pdm.bin";
    wave_header_t wavehead;
    playdata *d;
    itpInit();
    sleep(1);
    
    initAD();
    //GPIO_switch_set(TELEPHONETUBE);
    //GPIO_switch_set(HEADSET);

    d = INITplaydata();
    openfilewb(d,filename);

    audio_AD_volume_set(70);
    recvoice();

    //write_wav_header(d,&spec_adI,ADC_MEMORY_SIZE);
    fwrite(recdata,ADC_MEMORY_SIZE,1,d->fd);
    
    if(d->fd){
        fclose(d->fd);
    }
    printf("save file in %s\n",filename);
    printf("now check USB disk file\n");
    return NULL;
}