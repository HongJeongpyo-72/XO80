#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ite/itp.h"
#include "i2s/i2s.h" 
#include "test.h"
#include "wavheader.h"

/**********************************************************/
void initAD(){
    /* init ADC */ 
    adc_buf = (uint8_t*)malloc(ADC_BUFFER_SIZE);
	memset((uint8_t*) adc_buf, 0, ADC_BUFFER_SIZE);
	memset((void*)&spec_adI, 0, sizeof(STRC_I2S_SPEC));
    spec_adI.channels					= 1;
    spec_adI.sample_rate				= 8000;
    spec_adI.buffer_size				= ADC_BUFFER_SIZE;
    spec_adI.is_big_endian				= 0;
	spec_adI.base_i2s				    = (uint8_t*) adc_buf;	
    spec_adI.sample_size				= 16;
    spec_adI.record_mode				= 1;
    spec_adI.from_LineIN				= 0;
    spec_adI.from_MIC_IN				= 1;
    i2s_init_ADC(&spec_adI);
    i2s_pause_ADC(1);
}


void recvoice(playdata *d,int sec){
    
    uint8_t *psrc = NULL;
    int rectime_ms=sec*1000;//record time (ms)
    int samplesize= 2*spec_adI.sample_rate*spec_adI.channels;//1 sec data byte;
    int bsize = 10*(samplesize)/1000;//10ms data byte;
    int btime = bsize*1000/(samplesize);//10 ms
    
    write_wav_header(d,&spec_adI,sec*samplesize);//write wav header
    psrc = (u8*)malloc(bsize);
    
    i2s_pause_ADC(0);
    printf("start recording . . .\n");
    
    while(rectime_ms > 0){
               
        if(GET_AD_RW_GAP<bsize){//wait i2s data bufer > bsize
            usleep(20000);
            continue ;  
        }else{
            i2s_ad_data_get(psrc,bsize);//get data from i2s
            fwrite(psrc,bsize,1,d->fd);
            rectime_ms-=btime;
            if(rectime_ms%1000==0)
                printf("count down = %d(ms)\n",rectime_ms);
        }
        
    }
    printf("stop recording . . .\n");
    i2s_pause_ADC(1);
    if(d->fd){
        fclose(d->fd);
    }
}

void* TestFunc(void* arg)
{
    char *filename = "a:/record.wav";
    playdata *d;
    itpInit();
    sleep(1);
    
    initAD();

    d = INITplaydata();
    openfilewb(d,filename);
    
    if(d->fd){
        i2s_ADC_set_rec_volperc(70);/*set AD levle*/
        recvoice(d,8);/*record 8 sec*/
        printf("save file in a:/record.wav\n");
        printf("now check USB disk file\n");
    }else{
        printf("Did USB be inserted in USB port? \n");
    }
    return NULL;
}