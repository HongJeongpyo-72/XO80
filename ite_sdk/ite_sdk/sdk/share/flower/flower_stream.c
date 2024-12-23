#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "ite/ith.h"
#include "ite/itp.h"
#include "flower/flower.h"

#ifdef CFG_ITV_VP_HIGH_QUALITY
#define CONFIG_PREVIEWOSD 1 
#else
#define CONFIG_PREVIEWOSD 0
#endif

#ifdef CFG_H264_ENCODER_ENABLE
#define ENC_WIDTH  SENSOR_WIDTH
#define ENC_HEIGHT SENSOR_HEIGHT
#define ENC_PITCH  SENSOR_WIDTH
#else
#define ENC_WIDTH  1280
#define ENC_HEIGHT 720
#define ENC_PITCH  1280
#endif
static bool audiostream_release = false;
pthread_mutex_t AudioFlow_mutex      = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t VideoFlow_mutex = PTHREAD_MUTEX_INITIALIZER;

static Callback_t sound_cb= NULL;
static void flow_filter_callbackfunc(int state,void *arg)
{
    if (sound_cb)
    {
        return sound_cb(state,arg);
    }
}

static void video_flower_free(IteVideoFlower *s){
    
    if(s->Fcap)       	ite_filter_delete(s->Fcap);
    if(s->Fisp)       	ite_filter_delete(s->Fisp);      
#if CONFIG_PREVIEWOSD
	if(s->Fosd)       	ite_filter_delete(s->Fosd);
#endif		
    if(s->Fh264enc)   	ite_filter_delete(s->Fh264enc);
    if(s->Fudpsend)   	ite_filter_delete(s->Fudpsend);
    if(s->Fudprecv)   	ite_filter_delete(s->Fudprecv);
	if(s->Ftcpsend)   	ite_filter_delete(s->Ftcpsend);
    if(s->Ftcprecv)   	ite_filter_delete(s->Ftcprecv);
    if(s->Fh264dec)   	ite_filter_delete(s->Fh264dec);
	if(s->Fdisplay)   	ite_filter_delete(s->Fdisplay);
	if(s->Fjpegwriter)	ite_filter_delete(s->Fjpegwriter);
	if(s->Ffilewriter)	ite_filter_delete(s->Ffilewriter);
	if(s->Frec_avi)		ite_filter_delete(s->Frec_avi);
	if(s->Fipcam)		ite_filter_delete(s->Fipcam);
	if(s->Fqrscanner)   ite_filter_delete(s->Fqrscanner);
    if(s->ISPout.BufVramAddr) 
	{
		itpVmemFree(s->ISPout.BufVramAddr);
	}
    if(s) free(s);
 
}

void sound_playback_event_handler(PlaySoundCase nEventID, void *arg)
{
    switch(nEventID)
    {
        case Eofsound:
            audiostream_release=true;
            break;
        case Eofmixsound:
            printf("Eof mix sound\n");
            break;
        case Asrevent:
            printf("asr event\n");
            flow_filter_callbackfunc(ASR_SUCCESS_ARG,arg);

            break;
    }
}

static void *_background_iterate(IteFlower *f){
    while(1){    
        pthread_mutex_lock(&AudioFlow_mutex);
        if (f->audiostream && audiostream_release){
            flow_stop_sound_play(f);
        }
        pthread_mutex_unlock(&AudioFlow_mutex);
        usleep(500000);
    }
}
/*Init Flow*/
IteFlower *IteStreamInit(void){
    audio_init_AD();
    audio_init_DA();
    IteFlower *f= (IteFlower*)ite_new0(IteFlower,1);
    ite_flower_init();
    {
        pthread_t Thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&Thread, &attr, _background_iterate, f);    
    }
    
    return f;
}
/*play api*/
void flow_start_sound_play(IteFlower *f,const char* filepath,PlaySoundCase mode){


    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
    
    pthread_mutex_lock(&AudioFlow_mutex);
    
    //if(mode==Normal)
        PlayFlowStart(f,filepath,mode,sound_playback_event_handler);
    //else
    //    PlayFlowStart(f,filepath,mode,NULL);
    
    audiostream_release= false;
    
    pthread_mutex_unlock(&AudioFlow_mutex);

}

void flow_stop_sound_play(IteFlower *f){
    AudioStreamCancel(f);
    audiostream_release= false;
}

void flow_pause_sound_play(IteFlower *f,bool pause){//pause play
    if(f->audiostream!=NULL){
        PlayFlowPause(f,pause);
    }
}

void flow_mix_sound_play(IteFlower *f,const char *filepath){//mix wav sound
    if(f->audiostream!=NULL){
        PlayFlowMix(f,filepath);
    }
}

/*player end*/
/*record api*/
void flow_start_sound_record(IteFlower *f,const char* filepath,int rate){
    
    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
    
    RecFlowStart(f,filepath,rate);
    
}

void flow_stop_sound_record(IteFlower *f){
    AudioStreamCancel(f);
}
/*record end*/
/*asr api*/
void flow_start_asr(IteFlower *f,Callback_t func){
    
    pthread_mutex_lock(&AudioFlow_mutex);
    if(f->asrstream!=NULL) flow_stop_asr(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
    sound_cb=func;
    AsrFlowStart(f,sound_playback_event_handler);
    
}

void flow_stop_asr(IteFlower *f){

    if(f->asrstream!=NULL){
        AsrFlowStop(f);
        sound_cb=NULL;
        f->asrstream=NULL;
    }
}
/*asr*/
/*audio call api*/
void flow_start_audioflow(IteFlower *f){
    
    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
    
    SndrwFlowStart(f,8000,ULAW);
    
}

void flow_stop_audioflow(IteFlower *f){
    AudioStreamCancel(f);
}
/*audio call end*/
/*mic & spk work*/
void flow_start_soundrw(IteFlower *f){
    
    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
    
    SndrwFlowStart(f,8000,ULAW);
    
}

void flow_stop_soundrw(IteFlower *f){
    AudioStreamCancel(f);
}
/*end*/

/*start camera show flow*/
uint8_t flow_start_camera_show(IteFlower *f, int ch){

    char* args[] ={"-Q=32"};
    unsigned short FR   = 0;
    unsigned short INTERLANCED   = 0;
    int  Mode = PREVIEW_ON;
    int size = 0;
    int i = 0;
    pthread_mutex_lock(&VideoFlow_mutex);
    
    IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
    //********************************************************
    //SENSOR---CAPTURE->ISP0->ISP1(PREVIEW)
    //********************************************************
    vstream->Fcap     = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
    vstream->Fisp     = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
#if CONFIG_PREVIEWOSD
    vstream->Fosd     = ite_filter_new(ITE_FILTER_DISPLAY_OSD_ID);
#endif
	//if sensor support multi-channel, this method can choose channel.
	ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_SETSENSORINIT, (void*)ch);//if sensor support multi-channel.
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETFRAMERATE, (void*)&FR);
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETINTERLANCED, (void*)&INTERLANCED);
    /*Video Process Output Setting*/
    vstream->ISPout.Width   = ENC_WIDTH;
    vstream->ISPout.Height  = ENC_HEIGHT;
    vstream->ISPout.PitchY  = ENC_PITCH;
    vstream->ISPout.PitchUV = ENC_PITCH;
    vstream->ISPout.BufferCount = 3;
    vstream->ISPout.Framerate   = FR;
    
    /*Buffer Allocate*/
    size = (vstream->ISPout.Width * vstream->ISPout.Height * 3 * vstream->ISPout.BufferCount)/2;
    vstream->ISPout.BufVramAddr =  itpVmemAlignedAlloc(8, size);
    vstream->ISPout.BufSysAddr = (unsigned int ) ithMapVram(vstream->ISPout.BufVramAddr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
    /*Buffer Assign*/
    for (i = 0; i < vstream->ISPout.BufferCount; i++)
    {
        vstream->ISPout.DataAddrY[i] = vstream->ISPout.BufSysAddr + ((vstream->ISPout.Width * vstream->ISPout.Height * 3 * i)/2);
        vstream->ISPout.DataAddrUV[i] = vstream->ISPout.DataAddrY[i] + (vstream->ISPout.Width * vstream->ISPout.Height);
        //printf("ISP output mem addr(%d) (0x%x)\n", i, vstream->ISPout.DataAddrY[i]);
    }
    ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETOUTPUT, (void*)&vstream->ISPout);
	//***************************************************************************
	// If source is interlanced , Capture and ISP use momory mode.
	//***************************************************************************
	ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETINPUTMODE, (void*)INTERLANCED);	
#if CONFIG_PREVIEWOSD
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETINPUT,  (void*)&vstream->ISPout);
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETMODE,  (void*)Mode);
#endif
    ite_filterChain_build(&vstream->fc, "FC 3");
    ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);
	ite_filterChain_link(&vstream->fc, -1, vstream->Fcap, -1);
	ite_filterChain_link(&vstream->fc, 0, vstream->Fisp, 0);
#if CONFIG_PREVIEWOSD
	ite_filterChain_link(&vstream->fc, 0, vstream->Fosd, 0);
#endif
    printf("===Filter Chain run===\n");
    ite_filterChain_run(&vstream->fc);

    f->videostream = vstream;

    pthread_mutex_unlock(&VideoFlow_mutex);


}

/*stop camera show flow*/
void flow_stop_camera_show(IteFlower *f){

	pthread_mutex_lock(&VideoFlow_mutex);
    if(f->videostream!=NULL){
		
    	IteVideoFlower *s=f->videostream;
    	ite_filterChain_stop(&s->fc);

		if(s->Fcap) 	ite_filterChain_unlink(&s->fc,-1,s->Fcap, -1);
		if(s->Fisp) 	ite_filterChain_unlink(&s->fc, 0,s->Fisp,  0);
		if(s->Fosd) 	ite_filterChain_unlink(&s->fc, 0,s->Fosd,  0);

	    ite_filterChain_delete(&s->fc);
	    
	    video_flower_free(s);
    }
	pthread_mutex_unlock(&VideoFlow_mutex);
}

/*create Camera stream record flow*/
void flow_create_camera_record(IteFlower *f, int ch){
	char* args[] ={"-Q=32"};
	unsigned short FR	= 0;
	unsigned short INTERLANCED	 = 0;
	int  Mode = PREVIEW_OSD_ON;
    int size = 0;
    int i = 0;
	pthread_mutex_lock(&VideoFlow_mutex);
	
	IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
	//********************************************************
	//SENSOR---CAPTURE->ISP0->ISP1(OSD)->H264 ENCODER->AVI
	//                      |                    
	//                      ->JPEG Writer(thumbnail)->JPEG FILE Writer
	//********************************************************
	vstream->Fcap	  = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
	vstream->Fisp	  = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
#if CONFIG_PREVIEWOSD
	vstream->Fosd	  = ite_filter_new(ITE_FILTER_DISPLAY_OSD_ID);
#endif
	vstream->Fh264enc = ite_filter_new(ITE_FILTER_H264ENC_ID);
	vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
	vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
	vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);

	//if sensor support multi-channel, this method can choose channel.
	ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_SETSENSORINIT, (void*)ch);//if sensor support multi-channel.
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETFRAMERATE, (void*)&FR);
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETINTERLANCED, (void*)&INTERLANCED);
    /*Video Process Output Setting*/
    vstream->ISPout.Width   = ENC_WIDTH;
    vstream->ISPout.Height  = ENC_HEIGHT;
    vstream->ISPout.PitchY  = ENC_PITCH;
    vstream->ISPout.PitchUV = ENC_PITCH;
    vstream->ISPout.BufferCount = 3;
    vstream->ISPout.Framerate   = FR;
    
    /*Buffer Allocate*/
    size = (vstream->ISPout.Width * vstream->ISPout.Height * 3 * vstream->ISPout.BufferCount)/2;
    vstream->ISPout.BufVramAddr =  itpVmemAlignedAlloc(8, size);
    vstream->ISPout.BufSysAddr = (unsigned int ) ithMapVram(vstream->ISPout.BufVramAddr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
    /*Buffer Assign*/
    for (i = 0; i < vstream->ISPout.BufferCount; i++)
    {
        vstream->ISPout.DataAddrY[i] = vstream->ISPout.BufSysAddr + ((vstream->ISPout.Width * vstream->ISPout.Height * 3 * i)/2);
        vstream->ISPout.DataAddrUV[i] = vstream->ISPout.DataAddrY[i] + (vstream->ISPout.Width * vstream->ISPout.Height);
        //printf("ISP output mem addr(%d) (0x%x)\n", i, vstream->ISPout.DataAddrY[i]);
    }
    ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETOUTPUT, (void*)&vstream->ISPout);
	//***************************************************************************
	// If source is interlanced , Capture and ISP use momory mode.
	//***************************************************************************
	ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETINPUTMODE, (void*)INTERLANCED);	
#if CONFIG_PREVIEWOSD
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETINPUT,  (void*)&vstream->ISPout);
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETMODE,  (void*)Mode);
#endif
    ite_filter_call_method(vstream->Fh264enc, ITE_FILTER_ENC_SETINPUT,  (void*)&vstream->ISPout);

    ite_filterChain_build(&vstream->fc, "FC 2");
    ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);
	ite_filterChain_link(&vstream->fc, -1, vstream->Fcap, -1);
	ite_filterChain_link(&vstream->fc, 0, vstream->Fisp, 0);
#if CONFIG_PREVIEWOSD
	ite_filterChain_link(&vstream->fc, 0, vstream->Fosd, 0);
#endif
	ite_filterChain_link(&vstream->fc, 0, vstream->Fh264enc, 0);
	ite_filterChain_link(&vstream->fc, 0, vstream->Frec_avi, 0);
#if CONFIG_PREVIEWOSD
	ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fosd, 1, vstream->Fjpegwriter, 0);
	ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);
#endif

    printf("===Filter Chain run===\n");
    ite_filterChain_run(&vstream->fc);

    f->videostream = vstream;

	pthread_mutex_unlock(&VideoFlow_mutex);
}

/*destroy camera stream record flow*/
void flow_destroy_camera_record(IteFlower *f){

	pthread_mutex_lock(&VideoFlow_mutex);
    if(f->videostream!=NULL){
		
    	IteVideoFlower *s=f->videostream;
    	ite_filterChain_stop(&s->fc);

		if(s->Fcap) 	ite_filterChain_unlink(&s->fc,-1,s->Fcap, -1);
		if(s->Fisp) 	ite_filterChain_unlink(&s->fc, 0,s->Fisp,  0);
		if(s->Fosd) 	ite_filterChain_unlink(&s->fc, 0,s->Fosd,  0);
		if(s->Fh264enc) ite_filterChain_unlink(&s->fc, 0,s->Fh264enc,  0);
		if(s->Frec_avi) ite_filterChain_unlink(&s->fc, 0,s->Frec_avi,  0);		
		if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fosd, 1, s->Fjpegwriter, 0);
		if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter,  0);

	    ite_filterChain_delete(&s->fc);
	    
	    video_flower_free(s);
    }
	pthread_mutex_unlock(&VideoFlow_mutex);
}

/**/
void flow_start_camera_record(IteFlower *f, char* file_path){

	int videofps = 25;
	pthread_mutex_lock(&VideoFlow_mutex);
	
	VideoMemoInfo *info = (VideoMemoInfo*)ite_new0(VideoMemoInfo,1);
    strcpy(info->videomemo_file, file_path);
	info->video_width = f->videostream->ISPout.Width;
	info->video_height = f->videostream->ISPout.Height;
	
	switch(f->videostream->ISPout.Framerate)
	{
		case VIDEO_FRAMERATE_25HZ:
			videofps = 25;
			break;
		case VIDEO_FRAMERATE_30HZ:
			videofps = 30;
			break;
		case VIDEO_FRAMERATE_50HZ:
			videofps = 50;
			break;
		case VIDEO_FRAMERATE_60HZ:
			videofps = 60;
			break;			
	}
	info->video_fps = videofps;
	ite_filter_call_method(f->videostream->Frec_avi, ITE_FILTER_REC_AVI_OPEN, (void*)info);
	if(info)
		free(info);	
	pthread_mutex_unlock(&VideoFlow_mutex);
}

void flow_start_camera_record_split(IteFlower *f, char* file_path){

	int videofps = 25;
	pthread_mutex_lock(&VideoFlow_mutex);
	
	VideoMemoInfo *info = (VideoMemoInfo*)ite_new0(VideoMemoInfo,1);
    strcpy(info->videomemo_file, file_path);
	info->video_width = f->videostream->ISPout.Width;
	info->video_height = f->videostream->ISPout.Height;
	
	switch(f->videostream->ISPout.Framerate)
	{
		case VIDEO_FRAMERATE_25HZ:
			videofps = 25;
			break;
		case VIDEO_FRAMERATE_30HZ:
			videofps = 30;
			break;
		case VIDEO_FRAMERATE_50HZ:
			videofps = 50;
			break;
		case VIDEO_FRAMERATE_60HZ:
			videofps = 60;
			break;			
	}
	info->video_fps = videofps;
	ite_filter_call_method(f->videostream->Frec_avi, ITE_FILTER_REC_AVI_OPEN_PARTIAL, (void*)info);
	if(info)
		free(info);	
	pthread_mutex_unlock(&VideoFlow_mutex);
}


void flow_stop_camera_record(IteFlower *f){

	pthread_mutex_lock(&VideoFlow_mutex);
	ite_filter_call_method(f->videostream->Frec_avi, ITE_FILTER_REC_AVI_CLOSE, NULL);
	pthread_mutex_unlock(&VideoFlow_mutex);
}

bool flow_polling_camera_status(IteFlower *f){
	bool StableStatus = false;
	ite_filter_call_method(f->videostream->Fcap, ITE_FILTER_CAP_GETSENSORSTABLE, (void*)&StableStatus);
	return StableStatus;
}

bool flow_polling_capture_status(IteFlower *f){
	bool ErrorStatus = false;
	ite_filter_call_method(f->videostream->Fcap, ITE_FILTER_CAP_GETERROR, (void*)&ErrorStatus);
	return ErrorStatus;
}



/*video udp stream*/
uint8_t flow_start_udp_videostream(IteFlower *f, Call_info *call_list, unsigned short rem_port, VideoStreamDir dir){

    IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
    char* args[] ={"-Q=32"};
    udp_config_t udp_conf;
    unsigned short FR   = 0;
	unsigned short INTERLANCED   = 0;
	int  Mode = PREVIEW_OSD_ON;
	int  ch   = 0;
    int size = 0;
    int i = 0;

    if(dir == VideoStreamSendOnly)
    {
#if CONFIG_PREVIEWOSD
		//********************************************************
		//SENSOR---CAPTURE->ISP0->ISP1(OSD)->H264 ENCODER->UDP SEND
		//********************************************************
        vstream->Fcap     = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
        vstream->Fisp     = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
		vstream->Fosd     = ite_filter_new(ITE_FILTER_DISPLAY_OSD_ID);
        vstream->Fh264enc = ite_filter_new(ITE_FILTER_H264ENC_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
        vstream->Fudpsend = ite_filter_new(ITE_FILTER_UDP_SEND_ID);
        vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
        vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);
#else
		//********************************************************
		//SENSOR---CAPTURE->ISP0->H264 ENCODER->UDP SEND
		//********************************************************
		vstream->Fcap	  = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
		vstream->Fisp	  = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
		vstream->Fh264enc = ite_filter_new(ITE_FILTER_H264ENC_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
		vstream->Fudpsend = ite_filter_new(ITE_FILTER_UDP_SEND_ID);
#endif
        memset(&udp_conf,'\0',sizeof(udp_config_t));
    	udp_conf.remote_port = rem_port;
    	udp_conf.cur_socket = -1;
    	udp_conf.c_type = VIDEO_OUTPUT;	
    	memset(udp_conf.group_ip,'\0',16);
    	udp_conf.remote_ip = call_list;
        ite_filter_call_method(vstream->Fudpsend,ITE_FILTER_UDP_SEND_SET_PARA ,(void*)&udp_conf);
		//if sensor support multi-channel, this method can choose channel.
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_SETSENSORINIT, (void*)ch);
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETFRAMERATE, (void*)&FR);
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETINTERLANCED, (void*)&INTERLANCED);
		
        /*Video Process Output Setting*/
        vstream->ISPout.Width   = ENC_WIDTH;
        vstream->ISPout.Height  = ENC_HEIGHT;
        vstream->ISPout.PitchY  = ENC_PITCH;
        vstream->ISPout.PitchUV = ENC_PITCH;
        vstream->ISPout.BufferCount = 3;
        vstream->ISPout.Framerate   = FR;
        
        /*Buffer Allocate*/
        size = (vstream->ISPout.Width * vstream->ISPout.Height * 3 * vstream->ISPout.BufferCount)/2;
        vstream->ISPout.BufVramAddr =  itpVmemAlignedAlloc(8, size);
        vstream->ISPout.BufSysAddr = (unsigned int ) ithMapVram(vstream->ISPout.BufVramAddr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
        /*Buffer Assign*/
        for (i = 0; i < vstream->ISPout.BufferCount; i++)
        {
            vstream->ISPout.DataAddrY[i] = vstream->ISPout.BufSysAddr + ((vstream->ISPout.Width * vstream->ISPout.Height * 3 * i)/2);
            vstream->ISPout.DataAddrUV[i] = vstream->ISPout.DataAddrY[i] + (vstream->ISPout.Width * vstream->ISPout.Height);
            printf("ISP output mem addr(%d) (0x%x)\n", i, vstream->ISPout.DataAddrY[i]);
        }
        ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETOUTPUT, (void*)&vstream->ISPout);
		//***************************************************************************
		// If source is interlanced , Capture and ISP use momory mode.
		//***************************************************************************
		ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETINPUTMODE, (void*)INTERLANCED);		
#if CONFIG_PREVIEWOSD
		ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETINPUT,  (void*)&vstream->ISPout);
		ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETMODE,  (void*)Mode);
#endif
        ite_filter_call_method(vstream->Fh264enc, ITE_FILTER_ENC_SETINPUT,  (void*)&vstream->ISPout);

        ite_filterChain_build(&vstream->fc, "FC 1");
        ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);

        
        ite_filterChain_link(&vstream->fc, -1, vstream->Fcap, -1);
        ite_filterChain_link(&vstream->fc, 0, vstream->Fisp, 0);
#if CONFIG_PREVIEWOSD
        ite_filterChain_link(&vstream->fc, 0, vstream->Fosd, 0);
#endif
        ite_filterChain_link(&vstream->fc, 0, vstream->Fh264enc, 0);
        ite_filterChain_link(&vstream->fc, 0, vstream->Fudpsend, 0);

		//avi recoder
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fh264enc, 1, vstream->Frec_avi, 0);
#if CONFIG_PREVIEWOSD
        ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fosd, 1, vstream->Fjpegwriter, 0);
        ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);
#endif
        //ite_filterChain_print(&fc);
        
        //printf("===Filter Chain run===\n");
        ite_filterChain_run(&vstream->fc);

        f->videostream = vstream;
    }
    else if(dir == VideoStreamRecvOnly)
    {
        vstream->Fudprecv = ite_filter_new(ITE_FILTER_UDP_RECV_ID);
		vstream->Fh264dec = ite_filter_new(ITE_FILTER_H264DEC_ID);
		vstream->Fdisplay = ite_filter_new(ITE_FILTER_DISPLAY_ID);
		vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
		vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
		
        memset(&udp_conf,'\0',sizeof(udp_config_t));
    	udp_conf.remote_port = rem_port;
    	udp_conf.cur_socket = -1;
    	udp_conf.c_type = VIDEO_INPUT;	

        ite_filter_call_method(vstream->Fudprecv, ITE_FILTER_UDP_RECV_SET_PARA, (void*)&udp_conf);

        ite_filterChain_build(&vstream->fc, "FC 1");
        ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);
        
        ite_filterChain_link(&vstream->fc, -1, vstream->Fudprecv, -1);
       	ite_filterChain_link(&vstream->fc, 0, vstream->Fh264dec, 0);
		ite_filterChain_link(&vstream->fc, 0, vstream->Fdisplay, 0);

		//avi recoder
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fudprecv, 1, vstream->Frec_avi, 0);
		
		//snapshot
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fh264dec, 1, vstream->Fjpegwriter, 0);
		ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);
        
        //printf("===Filter Chain run===\n");
        ite_filterChain_run(&vstream->fc);

        f->videostream = vstream;
    }
}

/*video tcp stream*/
uint8_t flow_start_tcp_videostream(IteFlower *f, Call_info *call_list, unsigned short rem_port, VideoStreamDir dir){

    IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
    char* args[] ={"-Q=32"};
    tcp_config_t tcp_conf;
    unsigned short FR   = 0;
	unsigned short INTERLANCED   = 0;
	int  Mode = PREVIEW_OSD_ON;
	int  ch   = 0;
    int size = 0;
    int i = 0;

    if(dir == VideoStreamSendOnly)
    {
#if CONFIG_PREVIEWOSD
		//********************************************************
		//SENSOR---CAPTURE->ISP0->ISP1(OSD)->H264 ENCODER->UDP SEND
		//********************************************************
        vstream->Fcap     = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
        vstream->Fisp     = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
		vstream->Fosd     = ite_filter_new(ITE_FILTER_DISPLAY_OSD_ID);
        vstream->Fh264enc = ite_filter_new(ITE_FILTER_H264ENC_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
        vstream->Ftcpsend = ite_filter_new(ITE_FILTER_TCP_SEND_ID);
        vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
        vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);
#else
		//********************************************************
		//SENSOR---CAPTURE->ISP0->H264 ENCODER->UDP SEND
		//********************************************************
		vstream->Fcap	  = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
		vstream->Fisp	  = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
		vstream->Fh264enc = ite_filter_new(ITE_FILTER_H264ENC_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
		vstream->Ftcpsend = ite_filter_new(ITE_FILTER_TCP_SEND_ID);
#endif
        memset(&tcp_conf,'\0',sizeof(tcp_config_t));
    	tcp_conf.remote_port = rem_port;
    	tcp_conf.cur_socket = -1;
    	tcp_conf.c_type = VIDEO_OUTPUT;	
    	tcp_conf.remote_ip = call_list;
        ite_filter_call_method(vstream->Ftcpsend,ITE_FILTER_TCP_SEND_SET_PARA ,(void*)&tcp_conf);
		//if sensor support multi-channel, this method can choose channel.
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_SETSENSORINIT, (void*)ch);
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETFRAMERATE, (void*)&FR);
        ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETINTERLANCED, (void*)&INTERLANCED);
		
        /*Video Process Output Setting*/
        vstream->ISPout.Width   = ENC_WIDTH;
        vstream->ISPout.Height  = ENC_HEIGHT;
        vstream->ISPout.PitchY  = ENC_PITCH;
        vstream->ISPout.PitchUV = ENC_PITCH;
        vstream->ISPout.BufferCount = 3;
        vstream->ISPout.Framerate   = FR;
        
        /*Buffer Allocate*/
        size = (vstream->ISPout.Width * vstream->ISPout.Height * 3 * vstream->ISPout.BufferCount)/2;
        vstream->ISPout.BufVramAddr =  itpVmemAlignedAlloc(8, size);
        vstream->ISPout.BufSysAddr = (unsigned int ) ithMapVram(vstream->ISPout.BufVramAddr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
        /*Buffer Assign*/
        for (i = 0; i < vstream->ISPout.BufferCount; i++)
        {
            vstream->ISPout.DataAddrY[i] = vstream->ISPout.BufSysAddr + ((vstream->ISPout.Width * vstream->ISPout.Height * 3 * i)/2);
            vstream->ISPout.DataAddrUV[i] = vstream->ISPout.DataAddrY[i] + (vstream->ISPout.Width * vstream->ISPout.Height);
            printf("ISP output mem addr(%d) (0x%x)\n", i, vstream->ISPout.DataAddrY[i]);
        }
        ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETOUTPUT, (void*)&vstream->ISPout);
		//***************************************************************************
		// If source is interlanced , Capture and ISP use momory mode.
		//***************************************************************************
		ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETINPUTMODE, (void*)INTERLANCED);		
#if CONFIG_PREVIEWOSD
		ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETINPUT,  (void*)&vstream->ISPout);
		ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETMODE,  (void*)Mode);
#endif
        ite_filter_call_method(vstream->Fh264enc, ITE_FILTER_ENC_SETINPUT,  (void*)&vstream->ISPout);

        ite_filterChain_build(&vstream->fc, "FC 1");
        ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);

        
        ite_filterChain_link(&vstream->fc, -1, vstream->Fcap, -1);
        ite_filterChain_link(&vstream->fc, 0, vstream->Fisp, 0);
#if CONFIG_PREVIEWOSD
        ite_filterChain_link(&vstream->fc, 0, vstream->Fosd, 0);
#endif
        ite_filterChain_link(&vstream->fc, 0, vstream->Fh264enc, 0);
        ite_filterChain_link(&vstream->fc, 0, vstream->Ftcpsend, 0);

		//avi recoder
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fh264enc, 1, vstream->Frec_avi, 0);
#if CONFIG_PREVIEWOSD
        ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fosd, 1, vstream->Fjpegwriter, 0);
        ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);
#endif
        //ite_filterChain_print(&fc);
        
        //printf("===Filter Chain run===\n");
        ite_filterChain_run(&vstream->fc);

        f->videostream = vstream;
    }
    else if(dir == VideoStreamRecvOnly)
    {
        vstream->Ftcprecv = ite_filter_new(ITE_FILTER_TCP_RECV_ID);
		vstream->Fh264dec = ite_filter_new(ITE_FILTER_H264DEC_ID);
		vstream->Fdisplay = ite_filter_new(ITE_FILTER_DISPLAY_ID);
		vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
		vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);
		vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);
		
		memset(&tcp_conf,'\0',sizeof(tcp_config_t));
    	tcp_conf.remote_port = rem_port;
		tcp_conf.remote_ip = call_list;
    	tcp_conf.cur_socket = -1;
    	tcp_conf.c_type = VIDEO_INPUT;

        ite_filter_call_method(vstream->Ftcprecv, ITE_FILTER_TCP_RECV_SET_PARA, (void*)&tcp_conf);

        ite_filterChain_build(&vstream->fc, "FC 1");
        ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);
        
        ite_filterChain_link(&vstream->fc, -1, vstream->Ftcprecv, -1);
       	ite_filterChain_link(&vstream->fc, 0, vstream->Fh264dec, 0);
		ite_filterChain_link(&vstream->fc, 0, vstream->Fdisplay, 0);

		//avi recoder
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Ftcprecv, 1, vstream->Frec_avi, 0);
		
		//snapshot
		ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fh264dec, 1, vstream->Fjpegwriter, 0);
		ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);
        
        //printf("===Filter Chain run===\n");
        ite_filterChain_run(&vstream->fc);

        f->videostream = vstream;
    }
}



/*start audio udp stream*/
uint8_t flow_start_udp_audiostream(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){

    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
   
    AudioFlowStart(f,rate,type,rem_ip,rem_port);
}

/*start audio tcp stream*/
uint8_t flow_start_tcp_audiostream(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){

    pthread_mutex_lock(&AudioFlow_mutex);
    AudioStreamCancel(f);
    pthread_mutex_unlock(&AudioFlow_mutex);
   
    AudioTcpFlowStart(f,rate,type,rem_ip,rem_port);
}

/*stop video udp stream*/
uint8_t flow_stop_udp_videostream(IteFlower *f, VideoStreamDir dir){
    IteVideoFlower *s=f->videostream;

    ite_filterChain_stop(&s->fc);
    if(dir == VideoStreamSendOnly)
    {
	    if(s->Fcap)     ite_filterChain_unlink(&s->fc,-1,s->Fcap, -1);
        if(s->Fisp)     ite_filterChain_unlink(&s->fc, 0,s->Fisp,  0);
#if CONFIG_PREVIEWOSD
		if(s->Fosd)     ite_filterChain_unlink(&s->fc, 0,s->Fosd,  0);
#endif
        if(s->Fh264enc) ite_filterChain_unlink(&s->fc, 0,s->Fh264enc,  0);
        if(s->Fudpsend) ite_filterChain_unlink(&s->fc, 0,s->Fudpsend,  0);
		if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fc, s->Fh264enc, 1, s->Frec_avi, 0);
#if CONFIG_PREVIEWOSD
        if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fosd, 1, s->Fjpegwriter, 0);
        if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter, 0);
#endif
    }
    else if(dir == VideoStreamRecvOnly)
    {
        if(s->Fudprecv) ite_filterChain_unlink(&s->fc,-1,s->Fudprecv, -1);
		if(s->Fh264dec) ite_filterChain_unlink(&s->fc, 0,s->Fh264dec,  0);
		if(s->Fdisplay) ite_filterChain_unlink(&s->fc, 0,s->Fdisplay,  0);
		if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fc, s->Fudprecv, 1, s->Frec_avi, 0);
		if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fh264dec, 1, s->Fjpegwriter, 0);
		if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter,  0);
    }
    ite_filterChain_delete(&s->fc);
    
    video_flower_free(s);
}

/*stop video udp stream*/
uint8_t flow_stop_tcp_videostream(IteFlower *f, VideoStreamDir dir){
    IteVideoFlower *s=f->videostream;

    ite_filterChain_stop(&s->fc);
    if(dir == VideoStreamSendOnly)
    {
	    if(s->Fcap)     ite_filterChain_unlink(&s->fc,-1,s->Fcap, -1);
        if(s->Fisp)     ite_filterChain_unlink(&s->fc, 0,s->Fisp,  0);
#if CONFIG_PREVIEWOSD
		if(s->Fosd)     ite_filterChain_unlink(&s->fc, 0,s->Fosd,  0);
#endif
        if(s->Fh264enc) ite_filterChain_unlink(&s->fc, 0,s->Fh264enc,  0);
        if(s->Ftcpsend) ite_filterChain_unlink(&s->fc, 0,s->Ftcpsend,  0);
		if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fc, s->Fh264enc, 1, s->Frec_avi, 0);
#if CONFIG_PREVIEWOSD
        if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fosd, 1, s->Fjpegwriter, 0);
        if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter, 0);
#endif
    }
    else if(dir == VideoStreamRecvOnly)
    {
        if(s->Ftcprecv) ite_filterChain_unlink(&s->fc,-1,s->Ftcprecv, -1);
		if(s->Fh264dec) ite_filterChain_unlink(&s->fc, 0,s->Fh264dec,  0);
		if(s->Fdisplay) ite_filterChain_unlink(&s->fc, 0,s->Fdisplay,  0);
		if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fc, s->Ftcprecv, 1, s->Frec_avi, 0);
		if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fh264dec, 1, s->Fjpegwriter, 0);
		if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter,  0);
    }
    ite_filterChain_delete(&s->fc);
    
    video_flower_free(s);
}

/*stop audio udp stream*/
uint8_t flow_stop_udp_audiostream(IteFlower *f){
    AudioStreamCancel(f);
}

/*stop audio tcp stream*/
uint8_t flow_stop_tcp_audiostream(IteFlower *f){
    AudioStreamCancel(f);
}

uint8_t flow_start_recv_ipcamstream(IteFlower *f)
{
	IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
	char* args[] ={"-Q=32"};
	
	vstream->Fipcam = ite_filter_new(ITE_FILTER_IPCAM_ID);
	vstream->Fh264dec = ite_filter_new(ITE_FILTER_H264DEC_ID);
	vstream->Fdisplay = ite_filter_new(ITE_FILTER_DISPLAY_ID);
	vstream->Fjpegwriter = ite_filter_new(ITE_FILTER_JPEG_WRITER_ID);
	vstream->Ffilewriter = ite_filter_new(ITE_FILTER_FILE_WRITER_ID);
	vstream->Frec_avi = ite_filter_new(ITE_FILTER_REC_AVI_ID);

	ite_filterChain_build(&vstream->fc, "FC 1");
	ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);

	ite_filterChain_link(&vstream->fc, -1, vstream->Fipcam, -1);
	ite_filterChain_link(&vstream->fc, 0, vstream->Fh264dec, 0);
	ite_filterChain_link(&vstream->fc, 0, vstream->Fdisplay, 0);

	//avi recoder
	ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fipcam, 1, vstream->Frec_avi, 0);
	
	//snapshot
	ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fh264dec, 1, vstream->Fjpegwriter, 0);
	ite_filterChain_link(&vstream->fc, 0, vstream->Ffilewriter, 0);

	ite_filterChain_run(&vstream->fc);
	f->videostream = vstream;
}

uint8_t flow_stop_recv_ipcamstream(IteFlower *f)
{
	IteVideoFlower *s=f->videostream;
	
	ite_filterChain_stop(&s->fc);
	
	if(s->Fipcam) ite_filterChain_unlink(&s->fc,-1,s->Fipcam, -1);
	if(s->Fh264dec) ite_filterChain_unlink(&s->fc, 0,s->Fh264dec,  0);
	if(s->Fdisplay) ite_filterChain_unlink(&s->fc, 0,s->Fdisplay,  0);
	if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fc, s->Fipcam, 1, s->Frec_avi, 0);
	if(s->Fjpegwriter) ite_filterChain_A_unlink_B(false, &s->fc, s->Fh264dec, 1, s->Fjpegwriter, 0);
	if(s->Ffilewriter) ite_filterChain_unlink(&s->fc, 0,s->Ffilewriter,  0);
	
	ite_filterChain_delete(&s->fc);

	video_flower_free(s);
}

#ifdef CFG_QR_DECODE_ENABLE
/*create Camera qrscaner flow*/
void flow_create_qrscanner(IteFlower *f, int ch){

	char* args[] ={"-Q=32"};
	unsigned short FR	= 0;
	unsigned short INTERLANCED	 = 0;
	int  Mode = PREVIEW_ON;
    int size = 0;
    int i = 0;
	pthread_mutex_lock(&VideoFlow_mutex);
	
	IteVideoFlower *vstream = (IteVideoFlower *)ite_new0 (IteVideoFlower, 1);
	//********************************************************
	//SENSOR---CAPTURE->ISP0->ISP1(PREVIEW)
	//                      ->QR Scanner
	//********************************************************
	vstream->Fcap	  = ite_filter_new(ITE_FILTER_CAPONFLY_ID);
	vstream->Fisp	  = ite_filter_new(ITE_FILTER_ISPONFLY_ID);
#if CONFIG_PREVIEWOSD
	vstream->Fosd	  = ite_filter_new(ITE_FILTER_DISPLAY_OSD_ID);
#endif
	vstream->Fqrscanner = ite_filter_new(ITE_FILTER_QRSCANNER_ID); 

	//if sensor support multi-channel, this method can choose channel.
	ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_SETSENSORINIT, (void*)ch);//if sensor support multi-channel.
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETFRAMERATE, (void*)&FR);
    ite_filter_call_method(vstream->Fcap, ITE_FILTER_CAP_GETINTERLANCED, (void*)&INTERLANCED);
    /*Video Process Output Setting*/
    vstream->ISPout.Width   = ENC_WIDTH;
    vstream->ISPout.Height  = ENC_HEIGHT;
    vstream->ISPout.PitchY  = ENC_PITCH;
    vstream->ISPout.PitchUV = ENC_PITCH;
    vstream->ISPout.BufferCount = 3;
    vstream->ISPout.Framerate   = FR;
    
    /*Buffer Allocate*/
    size = (vstream->ISPout.Width * vstream->ISPout.Height * 3 * vstream->ISPout.BufferCount)/2;
    vstream->ISPout.BufVramAddr =  itpVmemAlignedAlloc(8, size);
    vstream->ISPout.BufSysAddr = (unsigned int ) ithMapVram(vstream->ISPout.BufVramAddr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
    /*Buffer Assign*/
    for (i = 0; i < vstream->ISPout.BufferCount; i++)
    {
        vstream->ISPout.DataAddrY[i] = vstream->ISPout.BufSysAddr + ((vstream->ISPout.Width * vstream->ISPout.Height * 3 * i)/2);
        vstream->ISPout.DataAddrUV[i] = vstream->ISPout.DataAddrY[i] + (vstream->ISPout.Width * vstream->ISPout.Height);
        //printf("ISP output mem addr(%d) (0x%x)\n", i, vstream->ISPout.DataAddrY[i]);
    }
    ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETOUTPUT, (void*)&vstream->ISPout);
	//***************************************************************************
	// If source is interlanced , Capture and ISP use momory mode.
	//***************************************************************************
	ite_filter_call_method(vstream->Fisp, ITE_FILTER_ISP_SETINPUTMODE, (void*)INTERLANCED);	
#if CONFIG_PREVIEWOSD
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETINPUT,  (void*)&vstream->ISPout);
	ite_filter_call_method(vstream->Fosd, ITE_FILTER_OSD_SETMODE,  (void*)Mode);
#endif
	ite_filter_call_method(vstream->Fqrscanner,ITE_FILTER_QR_SETINPUT, (void*)&vstream->ISPout);

    ite_filterChain_build(&vstream->fc, "FC 3");
    ite_filterChain_setConfig(&vstream->fc, ARRAY_COUNT_OF(args), args);
	ite_filterChain_link(&vstream->fc, -1, vstream->Fcap, -1);
	ite_filterChain_link(&vstream->fc, 0, vstream->Fisp, 0);
#if CONFIG_PREVIEWOSD
	ite_filterChain_link(&vstream->fc, 0, vstream->Fosd, 0);
#endif

	ite_filterChain_A_link_B(false, &vstream->fc, vstream->Fisp, 1, vstream->Fqrscanner, 0);

    printf("===Filter Chain run===\n");
    ite_filterChain_run(&vstream->fc);

    f->videostream = vstream;

	pthread_mutex_unlock(&VideoFlow_mutex);

}

void flow_destroy_qrscanner(IteFlower *f){

	pthread_mutex_lock(&VideoFlow_mutex);
    if(f->videostream!=NULL){
		
    	IteVideoFlower *s=f->videostream;
    	ite_filterChain_stop(&s->fc);

		if(s->Fcap) 	ite_filterChain_unlink(&s->fc,-1,s->Fcap, -1);
		if(s->Fisp) 	ite_filterChain_unlink(&s->fc, 0,s->Fisp,  0);
		if(s->Fosd) 	ite_filterChain_unlink(&s->fc, 0,s->Fosd,  0);
		if(s->Fqrscanner) ite_filterChain_A_unlink_B(false, &s->fc, s->Fisp, 1, s->Fqrscanner, 0);
	    ite_filterChain_delete(&s->fc);
	    
	    video_flower_free(s);
    }
	pthread_mutex_unlock(&VideoFlow_mutex);

}

bool flow_polling_qrscanner_status(IteFlower *f, unsigned char* data){
	unsigned short localsize	 = 0;
	
	ite_filter_call_method(f->videostream->Fqrscanner, ITE_FILTER_QR_GETPAYLOADSIZE, (void*)&localsize);
	if(localsize > 0)
	{
		ite_filter_call_method(f->videostream->Fqrscanner, ITE_FILTER_QR_GETPAYLOAD, (void*)data);
		return true; //success 
	}
	else
		return false;//fail

}
#endif

