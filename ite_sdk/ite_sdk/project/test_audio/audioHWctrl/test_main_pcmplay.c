#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ite/itp.h"
#include "i2s/i2s.h" 
#include "test.h"

/**********************************************************/

void initDA(int rate,int bitsize,int channels){
	/* init DAC */
    dac_buf = (uint8_t*)malloc(DAC_BUFFER_SIZE);
	memset((uint8_t*) dac_buf, 0, DAC_BUFFER_SIZE);
	memset((void*)&spec_daI, 0, sizeof(STRC_I2S_SPEC));
    spec_daI.channels                 = channels;
    spec_daI.sample_rate              = rate;
    spec_daI.buffer_size              = DAC_BUFFER_SIZE;
    spec_daI.is_big_endian            = 0;
    spec_daI.base_i2s                 = (uint8_t*) dac_buf;
    spec_daI.sample_size              = bitsize;
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

/*pcm data from file*/
FILE* fd;
void pcm_file_open(char *filename){
    
    if (filename != NULL)
    {
        if(fd) {
            fclose(fd);
            fd=NULL;
        }
        fd = fopen(filename, "rb");
        printf("file %s exit\n",filename);
    }else{
        printf("file %s not exit\n",filename);
    }
    return ;
}

int pcm_Data_get(unsigned char *ptr,int bytes){
    int err;
    err = fread(ptr,1,bytes,fd);
    if (err>=0){
        if (err!=0){
            if (err<bytes)
                memset(ptr+err,0,bytes-err);
        }
    }
    return err;
}
/**********************************************/

void playpcm(void){
    //uint16_t DAbsize=640;
    uint16_t bsize=10*(2*spec_daI.sample_rate*spec_daI.channels)/1000;//10ms data byte;
    uint8_t *psrc=NULL;           
    psrc = (u8*)malloc(bsize);        
    i2s_pause_DAC(0); 
    while(1){
        
        if(GET_DA_RW_GAP>GET_DA_BASE_LEN*3/4) {
            usleep(1000*GET_DA_RW_GAP/bsize);//avoid put too many data into i2sbuf
            continue; 
        }
        
        if(pcm_Data_get(psrc,bsize)){
            i2s_da_data_put(psrc,bsize);//pcm data to i2s(play)
        }else{
            
            //if(1){ //loop play
            //   fseek(fd, 0, SEEK_SET);
            //   continue;
            //}
            printf("EoF read file\n");
            break;
            
        }

    }
    
    while(GET_DA_RW_GAP>bsize){
        printf("wait\n");
        usleep(10000*(GET_DA_RW_GAP/bsize));//play over residual pcm data in i2sbuf
    }    
    
    if(psrc) free(psrc);
}  

void* TestFunc(void* arg)
{
    char *filename = "a:/pcm8K16b2c.raw";
    itpInit();
    sleep(1);//wait 1sec for USB driver
    
    pcm_file_open(filename);//open pcm.raw
    initDA(8000,16,2);//init i2s 8K 16bit 1channel

    audio_DA_volume_set(50);//set volume 0~100 (faraday : level 100= 0db )
    playpcm();
    printf("end of test play pcm\n");
 
    return NULL;
}