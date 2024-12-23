#include <stdio.h>
#include "flower/flower.h"
#include "include/audioqueue.h"
#include "include/volumeapi.h"


typedef struct HDTstate{
    //IteFilter *msF;
    //rbuf_ite *Rref;
    //rbuf_ite *Reco;
    Volume *vref;
    Volume *veco;
    //int framesize;
	//int samplerate;
    bool bypass_mode;
}HDTstate;

static inline int16_t saturate(int val) {
	return (val>32767) ? 32767 : ( (val<-32767) ? -32767 : val);
}

static void hdt_init(IteFilter *f){
    HDTstate *s=(HDTstate *)ite_new(HDTstate,1);
	f->data=s;
    //s->Rref =ite_rbuf_init(320*4);
    //s->Reco =ite_rbuf_init(320*4);
    s->vref = VOICE_INIT();
    s->veco = VOICE_INIT();
    VOICE_set_vad_threshold(s->vref,0.08);
    VOICE_set_vad_threshold(s->veco,0.08);
    s->bypass_mode=false;
    //s->msF = f;

}



static void hdt_uninit(IteFilter *f)
{     HDTstate *s=(HDTstate*)f->data;
    //ite_rbuf_free(s->Rref);
    //ite_rbuf_free(s->Reco);
    VOICE_UNINIT(s->vref);
    VOICE_UNINIT(s->veco);
    free(s);
}

/*  input[0]= reference signal from far end (sent to soundcard)
 *  input[1]= near speech & echo signal    (read from soundcard)
 *  output[0]=  is a copy of inputs[0] to be sent to soundcard
 *  output[1]=  near end speech, echo removed - towards far end
*/
static void hdt_process(IteFilter *f){
    HDTstate *s=(HDTstate*)f->data;
    IteQueueblk blkref={0};
    IteQueueblk blkeco={0};

	bool linein=true;
	while(f->run){
 
        if (s->bypass_mode) {
            if(ite_queue_get(f->input[0].Qhandle, &blkref) == 0){
                ite_queue_put(f->output[0].Qhandle, &blkref);
            }
            if(ite_queue_get(f->input[1].Qhandle, &blkeco) == 0){
            ite_queue_put(f->output[1].Qhandle, &blkeco);
            }
            usleep(20000);
            continue;
        }else{
            int input0=ite_queue_get_size(f->input[0].Qhandle);
            int input1=ite_queue_get_size(f->input[1].Qhandle);
            
            while(input0>0&&input1>0){
                mblk_ite *iref,*ieco;
                
                if(ite_queue_get(f->input[0].Qhandle, &blkref) == 0) iref=blkref.datap;
                if(ite_queue_get(f->input[1].Qhandle, &blkeco) == 0) ieco=blkeco.datap;
                
                if(VOICE_vaddetect(s->vref,iref)){
                    //switch to line out;
                    if(linein) {
                        VOICE_set_vad_threshold(s->vref,0.06);
                        VOICE_set_vad_threshold(s->veco,0.10);
                        linein=false;
                    }
                    memset(ieco->b_rptr,0,ieco->size);
                }else{
                    //switch to line in;
                    if(!linein) {
                        VOICE_set_vad_threshold(s->vref,0.10);
                        VOICE_set_vad_threshold(s->veco,0.08);;
                        linein=true;
                    }
                    if(VOICE_vaddetect(s->veco,ieco)){
                        s->vref->vad_dur=0;
                    }
                    memset(iref->b_rptr,0,iref->size);
                }
                ite_queue_put(f->output[0].Qhandle, &blkref);
                ite_queue_put(f->output[1].Qhandle, &blkeco);
                input0--;
                input1--;
                usleep(10000);
            }
            usleep(10000);
        }
	}
    
    ite_mblk_queue_flush(f->input[0].Qhandle);  
    ite_mblk_queue_flush(f->input[1].Qhandle);  
    ite_mblk_queue_flush(f->output[0].Qhandle);  
    ite_mblk_queue_flush(f->output[1].Qhandle);  

	return NULL;
}

static void hdt_set_bypass(IteFilter *f, void *arg)
{
    HDTstate *s=(HDTstate*)f->data;
    s->bypass_mode=*((int*)arg);
}

static IteMethodDes hdt_methods[] = {
    {ITE_FILTER_SET_VALUE, hdt_set_bypass},
    {0, NULL}
};


IteFilterDes FilterHDT = {
    ITE_FILTER_HDT_ID,
    hdt_init,
    hdt_uninit,
    hdt_process,
    hdt_methods
};

