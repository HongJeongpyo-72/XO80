/*
read file(mp3 aac wma(0) ,wav(x)),and decode to pcm data
*/
#include <stdio.h>
#include "flower/flower.h"
#include "include/audioqueue.h"
#include "include/fileheader.h"
#include "audio_mgr/audio_mgr.h"
#include "ite/audio.h"

/*RISC_USER_DEFINED_REG_BASE : 0xB0200080*/
#define DrvDumpPCM_RdPtr        (0xB0200080|0x18)
#define BIG_ENDIAN 1

#define BUFSIZE  (1024)//512
typedef struct _PlayerMgrData{
    void *fd;
    PlayerState state;
    rbuf_ite *buff;
    int rate;
    int bitsize;
    int nchannels;
    int loop_after;
    int codecType;

    uint8_t *codecbuf;
    int codelen;
    unsigned int RP;
    unsigned int WP;
    bool bypass;
    bool fileRead;

}PlayerMgrData;
//=============================================================================
//                              Private Function Declaration
//=============================================================================

int audiomgrCodecType(char* filename)
{
    char*                    ext;

    ext = strrchr(filename, '.');
    if (!ext)
    {
        printf("Invalid file name: %s\n", filename);
        return -1;
    }
    ext++;

    if (stricmp(ext, "wav") == 0)
    {
        return SMTK_AUDIO_WAV;     
    }
    else if (stricmp(ext, "mp3") == 0)
    {
        return SMTK_AUDIO_MP3;
    }
    else if (stricmp(ext, "wma") == 0)
    {
        return SMTK_AUDIO_WMA;
    }
    else if (stricmp(ext, "aac") == 0)
    {
        return SMTK_AUDIO_AAC;
    }
    else
    {
        printf("Unsupport file format: %s\n", ext);
        return -1;
    }
    return -1;
}
#ifdef BIG_ENDIAN
void swap_bytes(unsigned char *bytes, int len){
    int i;
    unsigned char tmp;
    for(i=0;i<len;i+=2){
        tmp=bytes[i];
        bytes[i]=bytes[i+1];
        bytes[i+1]=tmp;
    }
}
#endif

static int playermgr_stop(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    
    return 0;
}

static int playermgr_close(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    d->state=Closed; 
    if (d->fd)
    {
        fclose(d->fd);
        d->fd=NULL;
        //castor3snd_reinit_for_video_memo_play();
    }
    return 0;
}

static mblk_ite *codec_data(PlayerMgrData *d){
    mblk_ite *rm = NULL;
    uint32_t bsize = 0;
    uint32_t rp,wp;
    uint8_t *codecbuf=d->codecbuf;
    uint32_t codelen=d->codelen;
    
    rp=d->RP;
    wp=d->WP;
    if (rp <= wp){
        bsize = wp - rp;
        if (bsize){
            rm = allocb_ite(bsize);
            ithInvalidateDCacheRange(codecbuf + rp, bsize);
            memcpy(rm->b_wptr, codecbuf + rp, bsize);
            rm->b_wptr += bsize;
            rp += bsize;
        }
    }else{ // rp > wp
        bsize = (codelen - rp) + wp;
        if (bsize){     
            uint32_t szsec0 = codelen - rp;
            uint32_t szsec1 = bsize - szsec0;
            rm =allocb_ite(bsize);
            if (szsec0){
                ithInvalidateDCacheRange(codecbuf + rp, szsec0);
                memcpy(rm->b_wptr, codecbuf + rp, szsec0);
            }
            ithInvalidateDCacheRange(codecbuf, szsec1);
            memcpy(rm->b_wptr + szsec0, codecbuf, szsec1);
            rm->b_wptr += bsize;
            rp = szsec1;
        }
    }
    d->RP=d->WP;
    ithWriteRegA(DrvDumpPCM_RdPtr,d->RP);/*record PCM RP */
    return rm;
}

static void fileplay_do(IteFilter *f){
    int availSize =0;
    char buf[BUFSIZE];
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    IteQueueblk blk ={0};
    int rbytes=fread(buf,1,BUFSIZE,d->fd);

    if(rbytes==0) {
        d->fileRead=false;
        printf("finish file read\n");
        return;
    }
    
    do{//wait codec available buff size
        iteAudioGetAvailableBufferLength(ITE_AUDIO_OUTPUT_BUFFER, &availSize);
        usleep(10000);
        d->WP=smtkAudioMgrGetPcmIdx();
        if(d->RP!=d->WP){
            if(d->state==Closed){//wait codec parsing i2s info 
                d->RP=d->WP;
                ithWriteRegA(DrvDumpPCM_RdPtr,d->RP);
            }else{//get pcm data from codec;
                mblk_ite *om=codec_data(d);
                #ifdef BIG_ENDIAN
                swap_bytes(om->b_rptr,om->size);
                #endif
                blk.datap = om;
                ite_queue_put(f->output[0].Qhandle, &blk);
            }
        }         
    }while(availSize<=rbytes);
    iteAudioWriteStream(buf, rbytes);//write data for decoder
}

static int playermgr_open(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    const char *file=(const char*)arg;
    int tmp=0, cnt=0;
    
    if(d->fd) {
        fclose(d->fd);
        d->fd=NULL;
    }
    d->fd = fopen(file, "rb");
    if(d->fd==NULL) {
        printf("openfile error\n");
        return 0;
    }
    d->codecType=audiomgrCodecType(file);
    iteAudioOpenEngine(d->codecType);//load codec engine
    smtkAudioMgrSetPcmIdx(d->WP);
    ithWriteRegA(DrvDumpPCM_RdPtr,d->RP);
    iteAudioSetMusicCodecDump(1);//set dump flag
    iteAudioSetAttrib(ITE_AUDIO_I2S_INIT,&tmp); 
    
    while(tmp==0){//wait codec parsing i2s info;
    
        fileplay_do(f);
        
        if(cnt++>200){
            printf("timeout error %s %d\n",__FUNCTION__,__LINE__);
            if(cnt>210) printf("error init i2s\n"); break;
        }
        usleep(10000);
        iteAudioGetAttrib(ITE_AUDIO_I2S_INIT, &tmp);
    }
    iteAudioGetAttrib(  ITE_AUDIO_CODEC_SET_CHANNEL,        &d->nchannels);
    iteAudioGetAttrib(  ITE_AUDIO_CODEC_SET_SAMPLE_RATE,    &d->rate);
    iteAudioGetAttrib(  ITE_AUDIO_I2S_PTR,                  &d->codecbuf);
    iteAudioGetAttrib(  ITE_AUDIO_CODEC_SET_BUFFER_LENGTH,  &d->codelen);
    printf("%d %d %d\n",d->rate,d->nchannels,tmp);
    
    if(!d->bypass)
        Castor3snd_reinit_for_diff_rate(d->rate,16,d->nchannels);

    d->state=Playing;
    
    return 0;
}

static int playermgr_start(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    if (d->state==Paused)
        d->state = Playing;
    else{
        d->state = Dummy;
        printf("MSdummyPlaying scilent\n");
    }
    return 0;
}

static int playermgr_pause(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    bool pause=*((bool*)arg);
    if(pause)
        d->state=Paused;
    else
        d->state=Playing;
    return 0;
}

static int playermgr_IISbypass(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    d->bypass=true;
    return 0;
}

static int playermgr_loop(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    d->loop_after=*((int*)arg);
    return 0;
}

static int playermgr_get_rate(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    *(int*)arg = d->rate;
    return 0;
}

static int playermgr_get_channels(IteFilter *f, void *arg){
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    *(int*)arg = d->nchannels;
    return 0;
}

static void playermgr_init(IteFilter *f)
{
    PlayerMgrData *d=(PlayerMgrData*)ite_new(PlayerMgrData,1);
    d->fd=NULL;
    d->state=Closed;
    d->rate=8000;
    d->bitsize = 16;
    d->nchannels=1;
    d->loop_after=-1; /*by default, don't loop*/
    d->RP=0;
    d->WP=0;
    d->bypass=false;
    f->data=d;
    d->codecType=-1;
    d->fileRead=true;
}

static void playermgr_uninit(IteFilter *f)
{
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    if (d->fd>=0) playermgr_close(f,NULL);
    iteAudioSetMusicCodecDump(0);
    free(d);
}

static void playermgr_process(IteFilter *f)
{
    PlayerMgrData *d=(PlayerMgrData*)f->data;
    IteQueueblk blk ={0};
    int bytes =20*(2*d->rate*d->nchannels)/1000;//20ms data 

    while(f->run) {
        
        IteAudioQueueController(f,0,30,5);
        
        if (d->state==Playing){
            if(d->fileRead){
                fileplay_do(f);
            }else{
                /*get data from codec*/
                d->WP=smtkAudioMgrGetPcmIdx();
                if(d->RP!=d->WP){
                    mblk_ite *om=codec_data(d);
                    #ifdef BIG_ENDIAN
                    swap_bytes(om->b_rptr,om->size);
                    #endif
                    blk.datap = om;
                    ite_queue_put(f->output[0].Qhandle, &blk);
                }
                
                if(GET_DA_RW_GAP<256){//end of play;
                    if(d->loop_after<0){
                        printf("play eof\n");
                        d->state=Eof;
                    }else{
                        printf("play repeat\n");
                        fseek(d->fd,0,SEEK_SET);
                        d->fileRead=true;
                        continue;
                    }                    
                }
            }

        }
        
        if(d->state == Dummy){
            mblk_ite *om=allocb_ite(bytes);
            memset(om->b_wptr,0,bytes);
            om->b_wptr+=bytes;
            blk.datap = om;
            blk.private1=Eofsound;
            ite_queue_put(f->output[0].Qhandle, &blk);
        }
        
        if(d->state==Eof){
            mblk_ite *om=allocb_ite(bytes);
            memset(om->b_wptr,0,bytes);
            om->b_wptr+=bytes;
            blk.datap = om;
            blk.private1=Eofsound;
            ite_queue_put(f->output[0].Qhandle, &blk);
            usleep(100000);
        }
        usleep(10000);
    }
    ite_mblk_queue_flush(f->output[0].Qhandle);
    
    return NULL;

}

static IteMethodDes playermgr_methods[] = {
    {ITE_FILTER_FILEOPEN   , playermgr_open },
    {ITE_FILTER_LOOPPLAY   , playermgr_loop },
    {ITE_FILTER_PAUSE      , playermgr_pause },
    {ITE_FILTER_SET_BYPASS , playermgr_IISbypass},
    {ITE_FILTER_GETRATE    , playermgr_get_rate},
    {ITE_FILTER_GET_NCHANNELS, playermgr_get_channels},
    {0, NULL}
};

IteFilterDes FilterFilePlayMgr = {
    ITE_FILTER_FILEPLAYMGR_ID,
    playermgr_init,
    playermgr_uninit,
    playermgr_process,
    playermgr_methods
};



