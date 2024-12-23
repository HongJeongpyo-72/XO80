#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ite/itp.h"
#include "i2s/i2s.h"
#include "test.h"

#define I2S_REG_OUT_BIST            (0xD0100000 | 0x98)
static unsigned char ring[] = {
//#include "testwav/opendoor_8000_16_1.hex"
//#include "testwav/tone/R1000HZ_L500HZ.hex"
#include "testwav/test_48K_16bit.hex"
};

void i2s_out_bist_pause(int bist)
{
    if(bist){
        ithWriteRegMaskA(I2S_REG_OUT_BIST, 0, 1<<0);
    }else{
        ithWriteRegMaskA(I2S_REG_OUT_BIST, 1, 1<<0);
    } 
}

/**********************************************************/

void initDA()
{
    /* init DAC */
    dac_buf = (uint8_t*)malloc(DAC_BUFFER_SIZE);
    memset((uint8_t *) dac_buf, 0, DAC_BUFFER_SIZE);
    ithFlushDCacheRange(dac_buf, DAC_BUFFER_SIZE);
    memset((void *)&spec_daI, 0, sizeof(STRC_I2S_SPEC));
    spec_daI.channels                 = 2;
    spec_daI.sample_rate              = 48000;
    spec_daI.buffer_size              = DAC_BUFFER_SIZE;
    spec_daI.is_big_endian            = 0;
    spec_daI.base_i2s                 = (uint8_t *) dac_buf;
    spec_daI.sample_size              = 16;
    spec_daI.num_hdmi_audio_buffer    = 1;
    spec_daI.is_dac_spdif_same_buffer = 1;
    spec_daI.enable_Speaker           = 1;
    spec_daI.enable_HeadPhone         = 1;
    spec_daI.postpone_audio_output    = 1;
    spec_daI.base_spdif               = (uint8_t *) dac_buf;
    i2s_init_DAC(&spec_daI);
}

void playring()
{
    uint8_t  *psrc     = NULL;
    uint16_t bsize     = 10*(2*spec_daI.sample_rate*spec_daI.channels)/1000;//10ms data byte;
    uint32_t dataSize  = sizeof(ring);
    uint32_t sizecount = 0;
    psrc = (u8 *)malloc(bsize);
    printf("play\n");
    i2s_pause_DAC(0);
    
    /*play 1K tone test*/
    i2s_out_bist_pause(0);
    sleep(2);
    i2s_out_bist_pause(1);
    /*end*/

    while (1)
    {
        /*avoid put too many data into i2sbuf*/
        if(GET_DA_RW_GAP>GET_DA_BASE_LEN*3/4) {
            usleep(1000*GET_DA_RW_GAP/bsize);
            continue; 
        }
        /*get(read) data */
        if (sizecount + bsize < dataSize)
        {
            memcpy(psrc, ring + sizecount, bsize);
            sizecount += bsize;
        }
        else
        {
            if (sizecount < dataSize)
            {
                uint32_t szsec0 = dataSize - sizecount;
                uint32_t szsec1 = bsize - szsec0;
                memcpy(psrc, ring + sizecount, szsec0);
                sizecount += szsec0; // now :sizecount = dataSize;
                memset(psrc + szsec0, 0, szsec1);
                sizecount += szsec1;
            }
            sizecount = 0;
        }

        /*put data to i2s for playing*/
        i2s_da_data_put(psrc,bsize);

    }
    i2s_pause_DAC(1);
}

void *TestFunc(void *arg)
{
    itpInit();
    initDA();
    printf("*******init DA********\n");
/*****************play sound**********************************************/
    while (1)
    {
        i2s_set_direct_volperc(50);//0~100;
        playring();
    }
/*************************************************************************/
    return NULL;
}