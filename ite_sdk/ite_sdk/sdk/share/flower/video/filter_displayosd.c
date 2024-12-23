#include <stdio.h>
#include "flower/flower.h"
#include "flower/fliter_priv_def.h"
#include "ite/itv.h"
#include "ite/ith.h"
#include "libavcodec/avcodec.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
ISPOutPutStream*  gInputStream	= NULL;
int      gOSDMode               = OSD_ON;  /*0: update OSD only 1:preview only 2: update OSD and preview*/
//=============================================================================
//                              Private Function Declaration
//=============================================================================

static void displayosd_init(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	
}

static void displayosd_uninit(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
}

static void displayosd_process(IteFilter *f)
{
    IteQueueblk       rec_blk0 = {0};
    ITV_DBUF_PROPERTY dispProp = {0};
    ITV_DBUF_PROPERTY thumbnailProp = {0};
    IteQueueblk       blk_output0 = {0};
    AVFrame           orig = {0};
    char              *dbuf = NULL;
    unsigned char     index = 0;
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);

    while(f->run) {
        //printf("[%s] Filter(%d). thread run\n", __FUNCTION__, f->filterDes.id);
        if(ite_queue_get(f->input[0].Qhandle, &rec_blk0) == 0) {

			index = *(unsigned char*)rec_blk0.private1;

#ifdef CFG_ITV_VP_HIGH_QUALITY			
		    dbuf    = itv_get_dbuf_anchor();
			
            if (dbuf != NULL && gInputStream != NULL);
            {
                dispProp.src_w    = gInputStream->Width;
                dispProp.src_h    = gInputStream->Height;
                dispProp.ya       = gInputStream->DataAddrY[index];
                dispProp.ua       = gInputStream->DataAddrUV[index];
                dispProp.va       = gInputStream->DataAddrUV[index];
                dispProp.pitch_y  = gInputStream->PitchY;
                dispProp.pitch_uv = gInputStream->PitchUV;
				//only preview 
				if(gOSDMode == PREVIEW_ON)
					dispProp.bidx     = index + ITV_2D_INDEX_0;
				else
                	dispProp.bidx     = index + ITV_ENC_OSD_INDEX_0;
				
                dispProp.format   = MMP_ISP_IN_NV12;
				
                if (gOSDMode == OSD_ON)
                    itv_update_osd_anchor(&dispProp, &thumbnailProp);
                else if (gOSDMode == PREVIEW_OSD_ON)
                    itv_update_osddbuf_anchor(&dispProp, &thumbnailProp);
                else
                    itv_update_dbuf_anchor(&dispProp);//only preview on

                if (gOSDMode == OSD_ON || gOSDMode == PREVIEW_OSD_ON)
                {
                    orig.width = thumbnailProp.src_w;
                    orig.height = thumbnailProp.src_h;
                    orig.data[0] = thumbnailProp.ya;
                    orig.data[1] = thumbnailProp.ua;
                    orig.data[2] = thumbnailProp.va;
                    orig.linesize[0] = thumbnailProp.pitch_y;
                    orig.linesize[1] = thumbnailProp.pitch_uv;

                    mblk_ite *yuv_msg = NULL;
                    yuv_msg = allocb_ite(sizeof(AVFrame));
                    memcpy(yuv_msg->b_rptr, &orig, sizeof(AVFrame));
                    yuv_msg->b_wptr += sizeof(AVFrame);
                    blk_output0.datap = yuv_msg;
                    ite_queue_put(f->output[1].Qhandle, &blk_output0);
                }
            }
#endif
	            
        }				
        usleep(1000);
    }
	

    DEBUG_PRINT("[%s] Filter(%d) end\n", __FUNCTION__, f->filterDes.id);
    
    //return NULL;
}

static void f_displayosd_setinput(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gInputStream = (ISPOutPutStream*) arg;
}

static void f_displayosd_setmode(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gOSDMode = (int)arg;
}

static IteMethodDes FilterDisplayOsd_methods[] = {
    {ITE_FILTER_OSD_SETINPUT, f_displayosd_setinput},
	{ITE_FILTER_OSD_SETMODE , f_displayosd_setmode },
    {0, NULL}
};

IteFilterDes FilterDisplayOsd = {
    ITE_FILTER_DISPLAY_OSD_ID,
    displayosd_init,
    displayosd_uninit,
    displayosd_process,
    FilterDisplayOsd_methods
};



