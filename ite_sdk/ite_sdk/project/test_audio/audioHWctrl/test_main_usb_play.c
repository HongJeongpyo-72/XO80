#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ite/itp.h"
#include "i2s/i2s.h" 
#include "test.h"
#include "wavheader.h"

void initDA(wave_header_t wheader){
	/* init DAC */
    dac_buf = (uint8_t*)malloc(DAC_BUFFER_SIZE);
	memset((uint8_t*) dac_buf, 0, DAC_BUFFER_SIZE);
	memset((void*)&spec_daI, 0, sizeof(STRC_I2S_SPEC));
    spec_daI.channels                 = wheader.format_chunk.channel;
    spec_daI.sample_rate              = wheader.format_chunk.rate;
    spec_daI.buffer_size              = DAC_BUFFER_SIZE;
    spec_daI.is_big_endian            = 0;
    spec_daI.base_i2s                 = (uint8_t*) dac_buf;
    spec_daI.sample_size              = wheader.format_chunk.bitpspl;
    spec_daI.num_hdmi_audio_buffer    = 1;
    spec_daI.is_dac_spdif_same_buffer = 1;
    spec_daI.enable_Speaker           = 1;
    spec_daI.enable_HeadPhone         = 1;
    spec_daI.postpone_audio_output    = 1;
    spec_daI.base_spdif                = (uint8_t*) dac_buf;
    i2s_init_DAC(&spec_daI);
}

int pcm_Data_get(playdata *d,unsigned char *ptr,int bytes){
    int err;
    err = fread(ptr,1,bytes,d->fd);
    if (err>=0){
        if (err!=0){
            if (err<bytes)
                memset(ptr+err,0,bytes-err);
        }
    }
    return err;
}

void playring(playdata *d,bool loop){

	u8 *psrc=NULL;
    int bsize = 10*(2*spec_daI.sample_rate*spec_daI.channels)/1000;//10ms data byte;
    int err;
    i2s_pause_DAC(0);
    I2S_DA32_SET_WP(I2S_DA32_GET_RP());
    psrc = (unsigned char *)malloc(bsize);
    while(1){       

        /*avoid put too many data into i2sbuf*/
        if(GET_DA_RW_GAP>GET_DA_BASE_LEN*3/4) {
            usleep(1000*GET_DA_RW_GAP/bsize);
            continue; 
        }
        
        /*read wav file*/
        if(err=pcm_Data_get(d,psrc,bsize)){
            i2s_da_data_put(psrc,bsize);
            
            if(err<bsize){
                if(loop){/*loop play*/
                    fseek(d->fd, d->hsize, SEEK_SET);
                }else{/*just play 1 time*/
                    while(GET_DA_RW_GAP>bsize){
                        usleep(1000*(GET_DA_RW_GAP/bsize));
                    }
                break;     
                }
            }
        }


    }
    free(psrc);
    i2s_pause_DAC(1);
}

#if 1
void* TestFunc(void* arg)
{
    char *filename = "a:/test.wav";
    bool loop = true; //LOOP play ,
    wave_header_t wavehead;
    playdata *d;
    itpInit();
    sleep(1);

    d = INITplaydata();
    openfile(d,filename);
    get_wav_header(d,&wavehead);
    
    initDA(wavehead);
    i2s_set_direct_volperc(10);//set volume 0~100 (level 100 = 0db gain)
    playring(d,loop);
    
    
    if(d->fd){
        fclose(d->fd);
    }
    printf("end of play\n");
    return NULL;
}
#endif

#if 0
void* TestFunc(void* arg)
{
    char *filename[10] = {"a:full/test.wav",
                          "a:full/ring48K.wav",
                          "a:full/ring44K.wav",
                          "a:full/ring32K.wav",
                          "a:full/ring24K.wav",
                          "a:full/ring22K.wav",
                          "a:full/ring16K.wav",
                          "a:full/ring12K.wav",
                          "a:full/ring11K.wav",
                          "a:full/ring8K.wav",
    };
    bool loop = false; //LOOP play ,
    int i;
    itpInit();
    sleep(1);

    for(i=1;i<10;i++){
        wave_header_t wavehead;
        
        playdata *d = (playdata*)INITplaydata();
        
        openfile(d,filename[i]);
        get_wav_header(d,&wavehead);   
        initDA(wavehead);
        i2s_set_direct_volperc(i*10);//set volume 0~100 (level 100 = 0db gain)
        printf("level = %d\n",i*10);
        
       /* if(i%5==0)      GPIO_switch_set(DOUBLE_TELETUBE);
        else if(i%5==1) GPIO_switch_set(HEADSET);
        else if(i%5==2) GPIO_switch_set(TELEPHONETUBE);
        else if(i%5==3) GPIO_switch_set(HANDFREE);
        else if(i%5==4) GPIO_switch_set(DOUBLE_HEADSET);
       */ 
        playring(d,loop);
        
        DEINITplaydata(d);
        i2s_deinit_DAC();
        sleep(1);
    }
    
    printf("end\n");
    return NULL;
}
#endif