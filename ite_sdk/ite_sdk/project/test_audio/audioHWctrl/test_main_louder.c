#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ite/itp.h"
#include "i2s/i2s.h" 
#include "test.h"


/**********************************************************/


void initAD(){
    /* init ADC */ 
    adc_buf = (uint8_t*)malloc(ADC_BUFFER_SIZE);
	memset((uint8_t*) adc_buf, 0, ADC_BUFFER_SIZE);
	memset((void*)&spec_adI, 0, sizeof(STRC_I2S_SPEC));
    spec_adI.channels					= 2;
    spec_adI.sample_rate				= 8000;
    spec_adI.buffer_size				= ADC_BUFFER_SIZE;
    spec_adI.is_big_endian				= 0;
	spec_adI.base_i2s				    = (uint8_t*) adc_buf;	
    spec_adI.sample_size				= 16;
    spec_adI.record_mode				= 1;
    spec_adI.from_LineIN				= 0;
    spec_adI.from_MIC_IN				= 1;
    //i2s_init_ADC(&spec_adI);
    i2s_init_ADC(&spec_adI);
    i2s_pause_ADC(1);
}

void initDA(){
	/* init DAC */
    dac_buf = (uint8_t*)malloc(DAC_BUFFER_SIZE);
	memset((uint8_t*) dac_buf, 0, DAC_BUFFER_SIZE);
	memset((void*)&spec_daI, 0, sizeof(STRC_I2S_SPEC));
    spec_daI.channels                 = 2;
    spec_daI.sample_rate              = 8000;
    spec_daI.buffer_size              = DAC_BUFFER_SIZE;
    spec_daI.is_big_endian            = 0;
    spec_daI.base_i2s                 = (uint8_t*) dac_buf;
    spec_daI.sample_size              = 16;
    spec_daI.num_hdmi_audio_buffer    = 0;
    spec_daI.is_dac_spdif_same_buffer = 1;
    spec_daI.enable_Speaker           = 1;
    spec_daI.enable_HeadPhone         = 1;
    spec_daI.postpone_audio_output    = 1;
    spec_daI.base_spdif                = (uint8_t*) dac_buf;
    //i2s_init_DAC(&spec_daI);
    i2s_init_DAC(&spec_daI);
    i2s_pause_DAC(1);
}


void louder(){
    //uint16_t DAbsize=640;
    uint16_t bsize=10*(2*spec_daI.sample_rate*spec_daI.channels)/1000;//10ms data byte;
    uint8_t *psrc=NULL;           
    psrc = (u8*)malloc(bsize);        
    i2s_pause_DAC(0); 
    i2s_pause_ADC(0);      
    while(1){
        
        if(GET_AD_RW_GAP<bsize){//wait i2s data bufer > bsize
            usleep(20000);
            continue ;  
        }else{
            i2s_ad_data_get(psrc,bsize);//get data from i2s
            i2s_da_data_put(psrc,bsize);
        }
    }
    if(psrc) free(psrc);
}  

void* TestFunc(void* arg)
{
    itpInit();
    initDA();
    initAD();
    //GPIO_switch_set(TELEPHONETUBE);
    //GPIO_switch_set(HEADSET);    
    
/******************rec sound and play shond*******************************/ 

    i2s_ADC_set_rec_volperc(50);//set rec 0~100 (faraday : level 60= 0db )
    i2s_set_direct_volperc(50);//set volume 0~100 (faraday : level 100= 0db )
    louder();
    
/*******************end***************************************************/    
    return NULL;
}