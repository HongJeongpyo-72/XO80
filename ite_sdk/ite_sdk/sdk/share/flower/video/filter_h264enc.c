#include <stdio.h>
#include "flower/flower.h"
#include "flower/fliter_priv_def.h"
#include "video_encoder/video_encoder_flower.h"
#include "tuya_cloud/tuya_ipc_media.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
/*define video encode parameter*/
#if defined(CFG_DUAL_STREAM)
    #define VIDEO_STREAM_NUM    2
    #define VIDEO_SAMPLE_NUM    7
    #define VIDEO_SAMPLE_0_SIZE 500 * 1024
    #define VIDEO_SAMPLE_1_SIZE 200 * 1024
#else

#ifdef MULTI_SLICE
    #define VIDEO_STREAM_NUM    1
    #define VIDEO_SAMPLE_NUM    30
    #define VIDEO_SAMPLE_0_SIZE 100 * 1024
    #define VIDEO_SAMPLE_1_SIZE 10 * 1024
#else
    #define VIDEO_STREAM_NUM    1
    #define VIDEO_SAMPLE_NUM    7
    #define VIDEO_SAMPLE_0_SIZE 500 * 1024
    #define VIDEO_SAMPLE_1_SIZE 100 * 1024
#endif // end MULTI_SLICE

#endif// end CFG_DUAL_STREAM

typedef struct VIDEO_SAMPLE_TAG
{
    mblk_ite    *m_video;
    bool        bGetStream;
    uint32_t    dataSize;
    uint32_t    timeStamp;
    bool        bIFrame;
    bool        bLastSlice;
} VIDEO_SAMPLE;

ISP_BUFFER_INFO      gVideoEncBuffer = {0};
static uint32_t      gChkIndex = 0;
static VIDEO_SAMPLE  *gptVideoSample[VIDEO_STREAM_NUM][VIDEO_SAMPLE_NUM] = {0};
static uint32_t      gVideoSendIdx[VIDEO_STREAM_NUM] = {0};

static int           gIC = -1; /*Inited count*/


//=============================================================================
//                              Private Function Declaration
//=============================================================================
static void
getVideoStream(
    VIDEO_STREAM_INFO *streamInfo)
{
    gptVideoSample[streamInfo->streamIndex][streamInfo->bufIndex]->bIFrame      = streamInfo->bIFrame;
    gptVideoSample[streamInfo->streamIndex][streamInfo->bufIndex]->dataSize     = streamInfo->dataSize;
    gptVideoSample[streamInfo->streamIndex][streamInfo->bufIndex]->timeStamp    = streamInfo->timeStamp;
    gptVideoSample[streamInfo->streamIndex][streamInfo->bufIndex]->bLastSlice   = streamInfo->bLastSlice;
    gChkIndex = streamInfo->bufIndex;
    //printf("[H264enc] GetVideoStream %d \n", streamInfo->bufIndex);
    gptVideoSample[streamInfo->streamIndex][streamInfo->bufIndex]->bGetStream   = true;
}

static void h264enc_init(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gIC = (gIC + 1) % VIDEO_STREAM_NUM;

}

static void h264enc_uninit(IteFilter *f)
{
	int i =0, j = 0;
	
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	VideoEncoder_Pause(0);
	VideoEncoder_Close();
	VideoEncoder_Deinit();
	
   	for (j = 0; j < VIDEO_STREAM_NUM; j++)
		for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
		{
		    if (gptVideoSample[j][i] != NULL && gptVideoSample[j][i]->m_video != NULL)
		    {
		    	freemsg_ite(gptVideoSample[j][i]->m_video);
		         gptVideoSample[j][i]->m_video = NULL;
		    }
		}

    for (j = 0; j < VIDEO_STREAM_NUM; j++)
		for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
		{
		    if (gptVideoSample[j][i] != NULL)
		    {
		        free(gptVideoSample[j][i]);
		        gptVideoSample[j][i] = NULL;
		    }
		}

	gIC = -1;	

}

#ifdef CFG_TUYA_CLOUD_ENABLE
void tuya_video_pack(unsigned char* pVideoBuf,int pVideoSize ,uint8_t index, uint32_t ts)
{
    static char SPSBuf[256];
    static char PPSBuf[256];
    static int SPS_Size;
    static int PPS_Size;

    MEDIA_FRAME_S h264_frame = {0};
        int pVideoSize2 = 0;
        unsigned char *pVideoBuf2 = NULL;
		int pos = 0;
		unsigned char nalu_header[4] = { 0, 0, 0, 1 };

        //pVideoBuf = GetTuyaVideoStreamPacketQueue(&pVideoSize);
#define START_CODE_POS	4

		
	//	ithPrintf("tuya_video_pack 0x%x- size %d\n",pVideoBuf, pVideoSize);
	//	ithPrintf("0x%x 0x%x 0x%x 0x%x 0x%x  \n",pVideoBuf[0] ,pVideoBuf[1],pVideoBuf[2],pVideoBuf[3],pVideoBuf[4]);

        if( (pVideoBuf != NULL)&& (pVideoSize != 0) )
        {            
	
            //Note: For I frame of H264, SPS/PPS/SEI/IDR should be combined within one frame, and the NALU separator should NOT be deleted.
            // printf("pVideoBuf2[0]:%02x,pVideoBuf2[1]:%02x,pVideoBuf2[2]:%02x,pVideoBuf2[3]:%02x,pVideoBuf2[4]:%02x,pVideoBuf2[5]:%02x\n",pVideoBuf2[0],pVideoBuf2[1],pVideoBuf2[2],pVideoBuf2[3],pVideoBuf2[4],pVideoBuf2[5]);
            if( pVideoBuf[START_CODE_POS]==0x25)
            {
				//ithPrintf("-I-");

    	        VIDEO_HEADER_INFO headInfo;
				headInfo.streamIndex = index;
	            VideoEncoder_GetHdrInfo(&headInfo);
                pVideoSize2 = pVideoSize+headInfo.SPS_Size +headInfo.PPS_Size; 
				
				//ithPrintf("SPS=%d PPS=%d ,pVideoSize2 %d \n",headInfo.SPS_Size,headInfo.PPS_Size,pVideoSize2);
                pVideoBuf2 = malloc(pVideoSize2);
                if(pVideoBuf2 != NULL)
                {
                    memcpy(pVideoBuf2, headInfo.SPSBuf, headInfo.SPS_Size);
                //ithPrintf("kkk0x%x 0x%x 0x%x 0x%x 0x%x  \n",pVideoBuf2[0] ,pVideoBuf2[1],pVideoBuf2[2],pVideoBuf2[3],pVideoBuf2[4]);
                pos += headInfo.SPS_Size;
                memcpy(pVideoBuf2+pos, headInfo.PPSBuf, headInfo.PPS_Size);
                //ithPrintf("kkk0x%x 0x%x 0x%x 0x%x 0x%x  \n",pVideoBuf2[0] ,pVideoBuf2[1],pVideoBuf2[2],pVideoBuf2[3],pVideoBuf2[4]);
                pos += headInfo.PPS_Size;
				memcpy(pVideoBuf2+pos, pVideoBuf, pVideoSize);
                //ithPrintf("kkk0x%x 0x%x 0x%x 0x%x 0x%x  \n",pVideoBuf2[0] ,pVideoBuf2[1],pVideoBuf2[2],pVideoBuf2[3],pVideoBuf2[4]);
                }
				h264_frame.p_buf = pVideoBuf2;
				h264_frame.type = E_VIDEO_I_FRAME;
                h264_frame.size = pVideoSize2;
                //ithPrintf("kkk0x%x 0x%x 0x%x 0x%x 0x%x  \n",h264_frame.p_buf[0] ,h264_frame.p_buf[1],h264_frame.p_buf[2],h264_frame.p_buf[3],h264_frame.p_buf[4]);
            }
            else
            {
            
				//ithPrintf("-P-");
				//pVideoSize2 = pVideoSize;
				//pVideoBuf2 = malloc(pVideoSize2);
				//memcpy(pVideoBuf2+pos, pVideoBuf, pVideoSize);
				//printf("\n P =0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",pVideoBuf[0],pVideoBuf[1],pVideoBuf[2],pVideoBuf[3],pVideoBuf[4],pVideoBuf[5]);
 
				h264_frame.p_buf = pVideoBuf;
                h264_frame.type = E_VIDEO_PB_FRAME;
                h264_frame.size = pVideoSize;
				//ithPrintf("yyy0x%x 0x%x 0x%x 0x%x 0x%x  \n",h264_frame.p_buf[0] ,h264_frame.p_buf[1],h264_frame.p_buf[2],h264_frame.p_buf[3],h264_frame.p_buf[4]);
			//printf("\n P =0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",h264_frame.p_buf[0],h264_frame.p_buf[1],h264_frame.p_buf[2],h264_frame.p_buf[3],h264_frame.p_buf[4],h264_frame.p_buf[5]);
            }

			
            h264_frame.pts = (uint64_t)ts * 1000;
            h264_frame.timestamp = (uint64_t)ts * 1000;
            /* Send HD video data to the SDK */
            //if(theConfig.tuya_video_channel == 0)
            
            //ithPrintf("0x%x 0x%x 0x%x 0x%x 0x%x  \n",h264_frame.p_buf[0] ,h264_frame.p_buf[1],h264_frame.p_buf[2],h264_frame.p_buf[3],h264_frame.p_buf[4]);
                TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_SUB, &h264_frame);
            /* Send SD video data to the SDK */
            //if(theConfig.tuya_video_channel == 1)
            //    TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_SUB, &h264_frame);
            // TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_SUB, &h264_frame);



			
            if(pVideoBuf2){
          	  free(pVideoBuf2); pVideoBuf2=NULL;}
            return;
        }  


}
#endif

static void h264enc_process(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
    uint8_t k = 0; //dual stream index
    uint8_t tuya_calling = 0;
    mblk_ite *m = NULL;
	IteQueueblk blk_output0 = {0};
	IteQueueblk blk_output1 = {0};

    gVideoSendIdx[k] = 0;
	VideoEncoder_Open(k);
#if 1
    while(f->run) {
#ifdef CFG_TUYA_CLOUD_ENABLE     	
        tuya_calling = get_TUYA_APP_call_state();	
#endif    
		/*get new h264 stream*/
		if (gptVideoSample[k][gVideoSendIdx[k]]->bGetStream)
	    {            
            if (gptVideoSample[k][gVideoSendIdx[k]]->bIFrame)
            {
                VIDEO_HEADER_INFO headInfo;
                headInfo.streamIndex    = k;
                VideoEncoder_GetHdrInfo(&headInfo);

				//if(!tuya_calling)	
	                m = allocb_ite(headInfo.SPS_Size);

                if (m != NULL)
                {
                    memcpy(m->b_wptr, headInfo.SPSBuf, headInfo.SPS_Size);
                    m->b_wptr   += headInfo.SPS_Size;
                    m->b_rptr   += 4;
                    blk_output0.datap = m;
                    ite_queue_put(f->output[0].Qhandle, &blk_output0);
                }

				if(f->output[1].Qhandle)
				{
					mblk_ite *data = NULL;
					data = allocb_ite(headInfo.SPS_Size);
					memcpy(data->b_wptr, headInfo.SPSBuf, headInfo.SPS_Size);
					data->b_wptr += headInfo.SPS_Size;
					data->b_rptr += 4;
		            blk_output1.datap = data;
		            ite_queue_put(f->output[1].Qhandle, &blk_output1);
				}
                
				//if(!tuya_calling)
                	m = allocb_ite(headInfo.PPS_Size);

                if (m != NULL)
                {
                    memcpy(m->b_wptr, headInfo.PPSBuf, headInfo.PPS_Size);
                    m->b_wptr   += headInfo.PPS_Size;
                    m->b_rptr   += 4;
                    blk_output0.datap = m;
                    ite_queue_put(f->output[0].Qhandle, &blk_output0);
                }

				if(f->output[1].Qhandle)
				{
					mblk_ite *data = NULL;
					data = allocb_ite(headInfo.PPS_Size);
					memcpy(data->b_wptr, headInfo.PPSBuf, headInfo.PPS_Size);
					data->b_wptr += headInfo.PPS_Size;
					data->b_rptr += 4;
		            blk_output1.datap = data;
		            ite_queue_put(f->output[1].Qhandle, &blk_output1);
				}
            }
	    	if (gptVideoSample[k][gVideoSendIdx[k]]->m_video != NULL)
	    	{
				if(f->output[1].Qhandle)
				{
					mblk_ite *data = NULL;
					data = allocb_ite(gptVideoSample[k][gVideoSendIdx[k]]->dataSize);
					memcpy(data->b_wptr, gptVideoSample[k][gVideoSendIdx[k]]->m_video->b_rptr, gptVideoSample[k][gVideoSendIdx[k]]->dataSize);
					data->b_wptr += gptVideoSample[k][gVideoSendIdx[k]]->dataSize;
					data->b_rptr += 4;
		            blk_output1.datap = data;
		            ite_queue_put(f->output[1].Qhandle, &blk_output1);
				}
			
	    		gptVideoSample[k][gVideoSendIdx[k]]->m_video->b_wptr += gptVideoSample[k][gVideoSendIdx[k]]->dataSize;
#ifdef CFG_TUYA_CLOUD_ENABLE
                //send to TUYA app ring buffer
                tuya_video_pack(gptVideoSample[k][gVideoSendIdx[k]]->m_video->b_rptr,gptVideoSample[k][gVideoSendIdx[k]]->dataSize, k, gptVideoSample[k][gVideoSendIdx[k]]->timeStamp);
                //freemsg_ite(gptVideoSample[k][gVideoSendIdx[k]]->m_video);
#endif                               
                gptVideoSample[k][gVideoSendIdx[k]]->m_video->b_rptr += 4;

                //blk_output0.datap = gptVideoSample[k][gVideoSendIdx[k]]->m_video;
                //if(!tuya_calling)
                {
                	blk_output0.datap = gptVideoSample[k][gVideoSendIdx[k]]->m_video;
	                ite_queue_put(f->output[0].Qhandle, &blk_output0);
	            }
	            //else
	              	//freemsg_ite(gptVideoSample[k][gVideoSendIdx[k]]->m_video);

                if (k == 0)
                    gptVideoSample[k][gVideoSendIdx[k]]->m_video = allocb_ite(VIDEO_SAMPLE_0_SIZE);
                
				gptVideoSample[k][gVideoSendIdx[k]]->bGetStream = false;
				
				gptVideoSample[k][gVideoSendIdx[k]]->bLastSlice = false;
				
				VideoEncoder_SetStreamBuf(k, gVideoSendIdx[k], gptVideoSample[k][gVideoSendIdx[k]]->m_video->b_wptr);
				
				gVideoSendIdx[k] = (gVideoSendIdx[k] + 1) % VIDEO_SAMPLE_NUM;
	    	}
	    }
		/*get new source buufer index*/
        if(ite_queue_get(f->input[0].Qhandle, &blk_output0) == 0) {
            //printf("[%s] FilterH264 data=%x\n", __FUNCTION__, *(unsigned char*)blk.private1);
			VideoEncoder_SetEncodeIdx(*(unsigned char*)blk_output0.private1);
        }		
        usleep(1000);
    }
#else
    while(f->run) {
		if(ite_queue_get(f->input[0].Qhandle, &blk_output0) == 0) {
			printf("[%s] FilterH264 data=%x\n", __FUNCTION__, blk_output0.data);
		}
		usleep(1000);
    }
#endif

}

static void h264enc_method(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
}

static void f_h264enc_setinput(IteFilter *f, void *arg)
{
    int i = 0;
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	ISPOutPutStream* gInputStream = (ISPOutPutStream*) arg;

	if(gInputStream->BufferCount <= 0) {
		DEBUG_PRINT("[%s] Input Info Error \n", __FUNCTION__);
		return;
	}
	
	if(gIC == 0)/*only do one times*/
	{
		for(i = 0; i <  gInputStream->BufferCount; i++)
		{
			gVideoEncBuffer.pISPBufAdrY[i] = (unsigned char*)gInputStream->DataAddrY[i];
			gVideoEncBuffer.pISPBufAdrU[i] = (unsigned char*)gInputStream->DataAddrUV[i];
		}
		VideoEncoder_SetISPBUFFERINFO(gVideoEncBuffer);
		VideoEncoder_SetWidthHeight(gInputStream->Width, gInputStream->Height);
		VideoEncoder_SetFrameRate(gInputStream->Framerate);
		VideoEncoder_SetStreamBufCallBack(getVideoStream);
		VideoEncoder_Init(ENCODER_WITH_PREVIEW_MODE);
	}

	/* main stream allocate*/
	for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
	{
		if (NULL == gptVideoSample[0][i])
			gptVideoSample[0][i] = (VIDEO_SAMPLE *)malloc(sizeof(VIDEO_SAMPLE));
	
		if (NULL != gptVideoSample[0][i])
			memset(gptVideoSample[0][i], 0, sizeof(VIDEO_SAMPLE));
	}
	for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
	{
		gptVideoSample[0][i]->m_video		= allocb_ite(VIDEO_SAMPLE_0_SIZE);
		gptVideoSample[0][i]->bGetStream	= false;
	
		VideoEncoder_SetStreamBuf(0, i, gptVideoSample[0][i]->m_video->b_wptr);
		ithPrintf("[%s] Allocate (%x)\n", __FUNCTION__,gptVideoSample[0][i]->m_video->b_wptr);
	}
}

static void f_h264enc_setmobile(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
}

static IteMethodDes FilterH264enc_methods[] = {
    {ITE_FILTER_ENC_SETINPUT, f_h264enc_setinput},
    {ITE_FILTER_ENC_SETMOBILE, f_h264enc_setmobile},
    {0, NULL}
};

IteFilterDes FilterH264ENC = {
    ITE_FILTER_H264ENC_ID,
    h264enc_init,
    h264enc_uninit,
    h264enc_process,
    FilterH264enc_methods
};



