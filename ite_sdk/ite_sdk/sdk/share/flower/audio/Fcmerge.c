#include <stdio.h>
#include "flower/flower.h"

#define DEF_SWITCH 1

#ifdef DEF_SWITCH
#include "include/volumeapi.h"
#define SWITCH_PIN 55
#endif 

typedef struct _MergeData{
    void *datap;
    bool cswap;
    #ifdef DEF_SWITCH
    Volume *vL;
    Volume *vR;
    #endif 
} MergeData;

static void merge_init(IteFilter *f){
    MergeData *s=(MergeData *)ite_new(MergeData,1);
    s->cswap=true;
    #ifdef DEF_SWITCH
    s->vL = VOICE_INIT();
    s->vR = VOICE_INIT();
    VOICE_set_vad_threshold(s->vL,0.08);
    VOICE_set_vad_threshold(s->vR,0.08);
    //ithGpioClear(SWITCH_PIN);
    //ithGpioSetOut(SWITCH_PIN);
    #endif
    f->data=s;
}
static void merge_uninit(IteFilter *f){
    MergeData *s=(MergeData*)f->data;  
    #ifdef DEF_SWITCH
    VOICE_UNINIT(s->vL);
    VOICE_UNINIT(s->vR);
    #endif
    free(s); 
}

static void merge_process(IteFilter *f)
{
    MergeData *s=(MergeData*)f->data;  
    IteQueueblk blkR = {0};
    IteQueueblk blkL = {0};
    IteQueueblk blk  = {0};
    int linein=0;
    while(f->run) {      

        if(ite_queue_get(f->input[0].Qhandle, &blkL) == 0 && 
           ite_queue_get(f->input[1].Qhandle, &blkR) == 0 ){
            mblk_ite *iL=blkL.datap;
            mblk_ite *iR=blkR.datap;
            mblk_ite *o;
            int nbyte=iL->size;
            o=allocb_ite(nbyte*2);
            #ifdef DEF_SWITCH
            if(VOICE_vaddetect(s->vL,iL)){
                if(linein) {
                    VOICE_set_vad_threshold(s->vL,0.06);
                    VOICE_set_vad_threshold(s->vR,0.10);
                    linein=0;
                    printf("vL=%f vR=%f [%d] line out\n",s->vL->energy,s->vR->energy,linein);
                }
                //switch to line out;
                //if(ithGpioGet(SWITCH_PIN)){//mic > lineIn => lineOut state lineOUT:High
                //    ithGpioClear(SWITCH_PIN);
                //    ithGpioSetOut(SWITCH_PIN);
                //}
            
                memset(iR->b_rptr,0,iR->size);
            }else{
                if(!linein) {
                    VOICE_set_vad_threshold(s->vL,0.10);
                    VOICE_set_vad_threshold(s->vR,0.08);
                    linein=1;
                    printf("vL=%f vR=%f [%d] line in\n",s->vL->energy,s->vR->energy,linein);
                }
                //switch to line in;
                //if(!ithGpioGet(SWITCH_PIN)) {//lineIn > mic => lineIn state lineIN:low
                //    ithGpioClear(SWITCH_PIN);
                //    ithGpioSet(SWITCH_PIN);
                //}

                if(VOICE_vaddetect(s->vR,iR)){
                    s->vL->vad_dur=0;
                }else{
                    memset(iR->b_rptr,0,iR->size);
                }
                memset(iL->b_rptr,0,iL->size);
            }
            //printf("vL=%f vR=%f [%d]\n",s->vL->energy,s->vR->energy,linein);
            #endif
            
            for(;iL->b_rptr<iL->b_wptr;o->b_wptr+=4,iL->b_rptr+=2,iR->b_rptr+=2){
                if(s->cswap){
                    *((int16_t*)(o->b_wptr+2))=(int)*(int16_t*)iL->b_rptr;
                    *((int16_t*)(o->b_wptr  ))=(int)*(int16_t*)iR->b_rptr;
                }else{
                    *((int16_t*)(o->b_wptr  ))=(int)*(int16_t*)iL->b_rptr;
                    *((int16_t*)(o->b_wptr+2))=(int)*(int16_t*)iR->b_rptr;      
                }
            }
            

            if(iL) freemsg_ite(iL);
            if(iR) freemsg_ite(iR);
            
            blk.datap = o;
            ite_queue_put(f->output[0].Qhandle, &blk);
        }
    }
    ite_mblk_queue_flush(f->input[0].Qhandle);
    ite_mblk_queue_flush(f->input[1].Qhandle);
    ite_mblk_queue_flush(f->output[0].Qhandle);

}

static void merge_set_method(IteFilter *f, void *arg)
{

}

static IteMethodDes merge_methods[] = {
    {ITE_FILTER_A_Method, merge_set_method},
    {0, NULL}
};

IteFilterDes FilterCMerge = {
    ITE_FILTER_MERGE_ID,
    merge_init,
    merge_uninit,
    merge_process,
    merge_methods
};

