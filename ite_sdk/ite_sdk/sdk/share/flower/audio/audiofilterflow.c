#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "openrtos/FreeRTOS.h"
#include "flower/flower.h"
#include "audio_mgr/audio_mgr.h"

/*play sound start*/
void audio_flower_free(IteAudioFlower *s){
    
    if(s->Fsndwrite) ite_filter_delete(s->Fsndwrite);
    if(s->Fsndread)  ite_filter_delete(s->Fsndread);
    if(s->Fsource)   ite_filter_delete(s->Fsource);
    if(s->Fdestinat) ite_filter_delete(s->Fdestinat);
    if(s->Fudprecv)   ite_filter_delete(s->Fudprecv);
    if(s->Fudpsend) ite_filter_delete(s->Fudpsend);
	if(s->Ftcprecv)   ite_filter_delete(s->Ftcprecv);
    if(s->Ftcpsend) ite_filter_delete(s->Ftcpsend);
    if(s->Fmix)      ite_filter_delete(s->Fmix);
    if(s->Faec)      ite_filter_delete(s->Faec);
    if(s->Fdecoder)  ite_filter_delete(s->Fdecoder);
    if(s->Fencoder)  ite_filter_delete(s->Fencoder);
    if(s->Fasr)      ite_filter_delete(s->Fasr);
    //if(s->Frec_avi)  ite_filter_delete(s->Frec_avi);
    if(s->Ftee)      ite_filter_delete(s->Ftee);
    //ite_flower_deinit();
    if(s) free(s);
}

void PlayFlowStart(IteFlower *f,const char* filepath,PlaySoundCase mode,cb_sound_t cb){

    //IteFChain fc1=s->fc;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    
    //ite_flower_init();
    if(audiomgrCodecType(filepath)==SMTK_AUDIO_WAV){
        s->Fsource   = ite_filter_new(ITE_FILTER_FILEPLAY_ID);//wav
    }else {
        s->Fsource   = ite_filter_new(ITE_FILTER_FILEPLAYMGR_ID);//mp3 aac ...
    }

    s->Fmix      = ite_filter_new(ITE_FILTER_FILEMIX_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);

    
    ite_filter_call_method(s->Fsource,ITE_FILTER_FILEOPEN ,(void*)filepath);
    ite_filter_call_method(s->Fsource,ITE_FILTER_LOOPPLAY,&mode); 
    //ite_filter_call_method(s->Fmix,ITE_FILTER_FILEOPEN,(void*)fileinsert); 
    if(cb) ite_filter_call_method(s->Fsndwrite,ITE_FILTER_SET_CB,cb);
    
    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_link(&s->fc, -1, s->Fsource, -1);
    if(s->Fmix) ite_filterChain_link(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_link(&s->fc, 0, s->Fsndwrite, 0);

    //ite_filterChain_print(&s->fc);

    ite_filterChain_run(&s->fc);  
    
    f->audiocase=AudioPlayFile;
    f->audiostream = s;
}

void PlayFlowStop(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AudioPlayFile) return;

    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsource, -1);
    if(s->Fmix) ite_filterChain_unlink(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_unlink(&s->fc, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fc);

    audio_flower_free(s);
}

void PlayFlowPause(IteFlower *f,bool pause){
    IteAudioFlower *s=f->audiostream;
    IteFilter *F1=s->Fsource;
    IteFilter *F2=s->Fsndwrite;
    if(F1->filterDes.id==ITE_FILTER_FILEPLAY_ID && 
       F2->filterDes.id==ITE_FILTER_SNDWRITE_ID ){
        ite_filter_call_method(F1,ITE_FILTER_PAUSE,&pause);
        ite_filter_call_method(F2,ITE_FILTER_PAUSE,&pause);
    }
}

void PlayFlowMix(IteFlower *f,const char *fileinsert){
    IteAudioFlower *s=f->audiostream;
    IteFilter *F1=s->Fmix;
    if(F1->filterDes.id==ITE_FILTER_FILEMIX_ID){
        ite_filter_call_method(F1,ITE_FILTER_FILEOPEN,(void*)fileinsert); 
    }
}

/*play sound end*/

/*record sound start*/
void RecFlowStart(IteFlower *f,const char* filename,int rate){
    
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    
    //ite_flower_init();

    s->Fsndread   = ite_filter_new(ITE_FILTER_SNDREAD_ID);
	//s->Fmix        = ite_filter_new(ITE_FILTER_DENOISE_ID);
    s->Fdestinat  = ite_filter_new(ITE_FILTER_FILEREC_ID);

    ite_filter_call_method(s->Fdestinat,ITE_FILTER_FILEOPEN ,(void*)filename);
    ite_filter_call_method(s->Fdestinat,ITE_FILTER_SETRATE ,&rate);
    
    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);

    ite_filterChain_link(&s->fc, -1, s->Fsndread, -1);
	//ite_filterChain_link(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_link(&s->fc, 0, s->Fdestinat, 0);

    //ite_filterChain_print(&s->fc);

    ite_filterChain_run(&s->fc);
    f->audiocase=AudioRecFile;
    f->audiostream = s;
}

void RecFlowStop(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AudioRecFile) return;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsndread, -1);
	//ite_filterChain_unlink(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_unlink(&s->fc, 0, s->Fdestinat, 0);
    ite_filterChain_delete(&s->fc);
    
    audio_flower_free(s);
}


/*record sound end*/

/*sound RW start*/
void SndrwFlowStart(IteFlower *f,int rate,AudioCodecType type){
    
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    
    //ite_flower_init();
    
    s->Fsndread  = ite_filter_new(ITE_FILTER_SNDREAD_ID);
	s->Fmix      = ite_filter_new(ITE_FILTER_FILEMIX_ID);
    //s->Fencoder  = ite_filter_new(ITE_FILTER_ALAW_ENC_ID);
    //s->Fdecoder  = ite_filter_new(ITE_FILTER_ALAW_DEC_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    Castor3snd_reinit_for_diff_rate(rate,16,1);

    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_link(&s->fc, -1, s->Fsndread, -1);
    if(s->Fencoder) ite_filterChain_link(&s->fc, 0, s->Fencoder, 0);
    if(s->Fdecoder) ite_filterChain_link(&s->fc, 0, s->Fdecoder, 0);
	if(s->Fmix) ite_filterChain_link(&s->fc, 0, s->Fmix, 0);
	ite_filterChain_link(&s->fc, 0, s->Fsndwrite, 0);

    //ite_filterChain_print(&s->fc);

    ite_filterChain_run(&s->fc);
    
    f->audiocase=AnalogLocalBroadcast;
    f->audiostream = s;
}

void SndrwFlowStop(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AnalogLocalBroadcast) return;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsndread, -1);
    if(s->Fencoder) ite_filterChain_unlink(&s->fc, 0, s->Fencoder, 0);
    if(s->Fdecoder) ite_filterChain_unlink(&s->fc, 0, s->Fdecoder, 0);
	if(s->Fmix) ite_filterChain_unlink(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_unlink(&s->fc, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fc);
    
    audio_flower_free(s);
}
/*sound RW end*/

/*audio call start*/
void AudioFlowStart(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){

    udp_config_t udp_conf;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
       
    s->Fudprecv  = ite_filter_new(ITE_FILTER_UDP_RECV_ID);
    s->Fdecoder  = ite_filter_new(ITE_FILTER_ULAW_DEC_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    s->Fsndread = ite_filter_new(ITE_FILTER_SNDREAD_ID);

	if(f->videostream->Frec_avi)
		s->Frec_avi = f->videostream->Frec_avi;
	
    s->Fencoder = ite_filter_new(ITE_FILTER_ULAW_ENC_ID);
    s->Fudpsend = ite_filter_new(ITE_FILTER_UDP_SEND_ID);
    
    if(CFG_AEC_ENABLE) {
        s->Faec     = ite_filter_new(ITE_FILTER_AEC_ID);
        s->Fsource  = ite_filter_new(ITE_FILTER_LOOPBACK_ID);
    }

    memset(&udp_conf,'\0',sizeof(udp_config_t));
    udp_conf.remote_port = rem_port;
    udp_conf.cur_socket = -1;
    udp_conf.c_type = AUDIO_INPUT;
    memset(udp_conf.group_ip,'\0',16);
    udp_conf.remote_ip = NULL;
    ite_filter_call_method(s->Fudprecv, ITE_FILTER_UDP_RECV_SET_PARA, (void*)&udp_conf);
        
    memset(&udp_conf,'\0',sizeof(udp_config_t));
	udp_conf.remote_port = rem_port;
	udp_conf.cur_socket = -1;
	udp_conf.c_type = AUDIO_OUTPUT;	
	memset(udp_conf.group_ip,'\0',16);
	udp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Fudpsend,ITE_FILTER_UDP_SEND_SET_PARA ,(void*)&udp_conf);
    
    Castor3snd_reinit_for_diff_rate(rate,16,1);

    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fcc, "FC 2");
    ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fccc, "FC 3");
    ite_filterChain_setConfig(&s->fccc, ARRAY_COUNT_OF(args), args);
    
/*   FC 1: A->B->C     */
/*                     */
/*   FC 2: D->[AEC]->F */
/*              |      */
/*   FC 3: E--->*      */       

    ite_filterChain_link(&s->fc ,-1,s->Fudprecv, -1);
    if(s->Fdecoder) ite_filterChain_link(&s->fc , 0,s->Fdecoder ,  0);
    ite_filterChain_link(&s->fc , 0,s->Fsndwrite,  0);
    
    ite_filterChain_link(&s->fcc,-1,s->Fsndread , -1);
    if(s->Faec) {
        ite_filterChain_link(&s->fcc, 0,s->Faec     ,  1);
        ite_filterChain_link(&s->fccc,-1,s->Fsource , -1);
        ite_filterChain_link(&s->fccc, 0,s->Faec    , 0);
    }
    if(s->Fencoder) ite_filterChain_link(&s->fcc, 0,s->Fencoder ,    0);
    ite_filterChain_link(&s->fcc, 0,s->Fudpsend,  0);

	if(s->Frec_avi) ite_filterChain_A_link_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);

    //ite_filterChain_print(&s->fc );
    //ite_filterChain_print(&s->fcc);

    ite_filterChain_run(&s->fc );
    ite_filterChain_run(&s->fcc);
    ite_filterChain_run(&s->fccc);

    f->audiocase=AudioNetCom;
    f->audiostream = s;
}

void AudioFlowStop(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AudioNetCom) return;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc ,-1,s->Fudprecv, -1);
    if(s->Fdecoder) ite_filterChain_unlink(&s->fc , 0,s->Fdecoder ,  0);
    ite_filterChain_unlink(&s->fc , 0,s->Fsndwrite,  0);
    ite_filterChain_delete(&s->fc); 
    
    ite_filterChain_stop(&s->fcc);
    ite_filterChain_stop(&s->fccc);
    if(s->Fsndread) ite_filterChain_unlink(&s->fcc,  -1,s->Fsndread , -1);
    if(s->Faec)     {
        ite_filterChain_unlink(&s->fcc, 0,s->Faec     ,  1);
        ite_filterChain_unlink(&s->fccc,-1,s->Fsource , -1);
        ite_filterChain_unlink(&s->fccc, 0,s->Faec    , 0);
    }
    if(s->Fencoder) ite_filterChain_unlink(&s->fcc, 0,s->Fencoder ,  0);
    if(s->Fudpsend) ite_filterChain_unlink(&s->fcc, 0,s->Fudpsend ,  0);
	if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);
    ite_filterChain_delete(&s->fcc);
    ite_filterChain_delete(&s->fccc);
    
    audio_flower_free(s);
}
/*sound AudioFlowStop end*/


/*audio tcp call start*/
void AudioTcpFlowStart(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){

    tcp_config_t tcp_conf;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
       
    s->Ftcprecv  = ite_filter_new(ITE_FILTER_TCP_RECV_ID);
    s->Fdecoder  = ite_filter_new(ITE_FILTER_ULAW_DEC_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    s->Fsndread = ite_filter_new(ITE_FILTER_SNDREAD_ID);

	if(f->videostream->Frec_avi)
		s->Frec_avi = f->videostream->Frec_avi;
	
    s->Fencoder = ite_filter_new(ITE_FILTER_ULAW_ENC_ID);
    s->Ftcpsend = ite_filter_new(ITE_FILTER_TCP_SEND_ID);
    
    if(CFG_AEC_ENABLE) {
        s->Faec     = ite_filter_new(ITE_FILTER_AEC_ID);
        s->Fsource  = ite_filter_new(ITE_FILTER_LOOPBACK_ID);
    }

    memset(&tcp_conf,'\0',sizeof(tcp_config_t));
    tcp_conf.remote_port = rem_port;
    tcp_conf.cur_socket = -1;
    tcp_conf.c_type = AUDIO_INPUT;
    tcp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Ftcprecv, ITE_FILTER_TCP_RECV_SET_PARA, (void*)&tcp_conf);
        
    memset(&tcp_conf,'\0',sizeof(tcp_config_t));
	tcp_conf.remote_port = rem_port;
	tcp_conf.cur_socket = -1;
	tcp_conf.c_type = AUDIO_OUTPUT;	
	tcp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Ftcpsend,ITE_FILTER_TCP_SEND_SET_PARA ,(void*)&tcp_conf);
    
    Castor3snd_reinit_for_diff_rate(rate,16,1);

    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fcc, "FC 2");
    ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fccc, "FC 3");
    ite_filterChain_setConfig(&s->fccc, ARRAY_COUNT_OF(args), args);
    
/*   FC 1: A->B->C     */
/*                     */
/*   FC 2: D->[AEC]->F */
/*              |      */
/*   FC 3: E--->*      */       

    ite_filterChain_link(&s->fc ,-1,s->Ftcprecv, -1);
    if(s->Fdecoder) ite_filterChain_link(&s->fc , 0,s->Fdecoder ,  0);
    ite_filterChain_link(&s->fc , 0,s->Fsndwrite,  0);
    
    ite_filterChain_link(&s->fcc,-1,s->Fsndread , -1);
    if(s->Faec) {
        ite_filterChain_link(&s->fcc, 0,s->Faec     ,  1);
        ite_filterChain_link(&s->fccc,-1,s->Fsource , -1);
        ite_filterChain_link(&s->fccc, 0,s->Faec    , 0);
    }
    if(s->Fencoder) ite_filterChain_link(&s->fcc, 0,s->Fencoder ,    0);
    ite_filterChain_link(&s->fcc, 0,s->Ftcpsend,  0);

	if(s->Frec_avi) ite_filterChain_A_link_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);

    //ite_filterChain_print(&s->fc );
    //ite_filterChain_print(&s->fcc);

    ite_filterChain_run(&s->fc );
    ite_filterChain_run(&s->fcc);
    ite_filterChain_run(&s->fccc);

    f->audiocase=AudioNetCom;
    f->audiostream = s;
}

void AudioTcpFlowStop(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AudioNetCom) return;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc ,-1,s->Ftcprecv, -1);
    if(s->Fdecoder) ite_filterChain_unlink(&s->fc , 0,s->Fdecoder ,  0);
    ite_filterChain_unlink(&s->fc , 0,s->Fsndwrite,  0);
    ite_filterChain_delete(&s->fc); 
    
    ite_filterChain_stop(&s->fcc);
    ite_filterChain_stop(&s->fccc);
    if(s->Fsndread) ite_filterChain_unlink(&s->fcc,  -1,s->Fsndread , -1);
    if(s->Faec)     {
        ite_filterChain_unlink(&s->fcc, 0,s->Faec     ,  1);
        ite_filterChain_unlink(&s->fccc,-1,s->Fsource , -1);
        ite_filterChain_unlink(&s->fccc, 0,s->Faec    , 0);
    }
    if(s->Fencoder) ite_filterChain_unlink(&s->fcc, 0,s->Fencoder ,  0);
    if(s->Ftcpsend) ite_filterChain_unlink(&s->fcc, 0,s->Ftcpsend ,  0);
	if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);
    ite_filterChain_delete(&s->fcc);
    ite_filterChain_delete(&s->fccc);
    
    audio_flower_free(s);
}
/*sound AudioTcpFlowStop end*/

/*analog audio call start*/
void AnalogFlowStart(IteFlower *f,int rate){
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    
    Castor3snd_reinit_for_diff_rate(rate,16,2);
      
    s->Fsndread  = ite_filter_new(ITE_FILTER_SNDREAD_ID);
    s->Fencoder  = ite_filter_new(ITE_FILTER_SEPARATE_ID);
    s->Fdecoder  = ite_filter_new(ITE_FILTER_MERGE_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    //if(CFG_AEC_ENABLE) s->Faec     = ite_filter_new(ITE_FILTER_AEC_ID);
    if(s->Faec) {//**note: if enable aec ,data size(128*2) will be reshape
        int in_channels=2;
        
        s->Fsource  = ite_filter_new(ITE_FILTER_LOOPBACK_ID);
        s->Ftee     = ite_filter_new(ITE_FILTER_CHADAPT_ID); //2 channel to 1 channel
        ite_filter_call_method(s->Ftee,ITE_FILTER_SET_VALUE,&in_channels);
    }
    
    if(s->Faec){
        ite_filterChain_build(&s->fcc, "FC 2");
        ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
        
        ite_filterChain_link(&s->fcc, -1, s->Fsource, -1);
        ite_filterChain_link(&s->fcc, 0, s->Ftee, 0);
        ite_filterChain_link(&s->fcc, 0, s->Faec, 0);
        ite_filterChain_run(&s->fcc);
        
    }
    
    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_link(&s->fc, -1, s->Fsndread, -1);
    ite_filterChain_link(&s->fc,  0, s->Fencoder, 0);
    if(s->Faec){
        ite_filterChain_A_link_B(true, &s->fc, s->Fencoder, 0, s->Faec, 1);
        ite_filterChain_A_link_B(true, &s->fc, s->Faec, 0, s->Fdecoder, 0);
    }else{
        ite_filterChain_A_link_B(true, &s->fc, s->Fencoder, 0, s->Fdecoder, 0);
    }
    ite_filterChain_A_link_B(true, &s->fc, s->Fencoder, 1, s->Fdecoder, 1);
    ite_filterChain_A_link_B(true, &s->fc, s->Fdecoder, 0, s->Fsndwrite, 0);

    ite_filterChain_run(&s->fc);
    
    f->audiocase=AnalogLocalCom;
    f->audiostream = s;
}

void AnalogFlowStop(IteFlower *f){

    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AnalogLocalCom) return;
    
    if(s->Faec){
        ite_filterChain_stop(&s->fcc);
        ite_filterChain_unlink(&s->fcc, -1, s->Fsource, -1);
        ite_filterChain_unlink(&s->fcc, 0, s->Ftee, 0);
        ite_filterChain_unlink(&s->fcc, 0, s->Faec, 0);
        ite_filterChain_delete(&s->fcc);
    }
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsndread, -1);
    ite_filterChain_unlink(&s->fc,  0, s->Fencoder, 0);
    if(s->Faec){
        ite_filterChain_A_unlink_B(true, &s->fc, s->Fencoder, 0, s->Faec, 1);
        ite_filterChain_A_unlink_B(true, &s->fc, s->Faec, 0, s->Fdecoder, 0);
    }else{
        ite_filterChain_A_unlink_B(true, &s->fc, s->Fencoder, 0, s->Fdecoder, 0);
    }
    ite_filterChain_A_unlink_B(true, &s->fc, s->Fencoder, 1, s->Fdecoder, 1);
    ite_filterChain_A_unlink_B(true, &s->fc, s->Fdecoder, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fc);
    
    audio_flower_free(s);
}
/*analog SndrwFlowStop start*/

/*audio call start*/
void AnalogNetFlowStart(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){
    udp_config_t udp_conf;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    int pin=0;
       
    s->Fudprecv  = ite_filter_new(ITE_FILTER_UDP_RECV_ID);
    s->Fdecoder  = ite_filter_new(ITE_FILTER_ULAW_DEC_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    s->Fsndread = ite_filter_new(ITE_FILTER_SNDREAD_ID);
    s->Fencoder = ite_filter_new(ITE_FILTER_ULAW_ENC_ID);
    s->Fudpsend = ite_filter_new(ITE_FILTER_UDP_SEND_ID);
    
    //s->Faec      = ite_filter_new(ITE_FILTER_HDT_ID);
    if(s->Faec) pin=1;

    memset(&udp_conf,'\0',sizeof(udp_config_t));
    udp_conf.remote_port = rem_port;
    udp_conf.cur_socket = -1;
    udp_conf.c_type = AUDIO_INPUT;
    memset(udp_conf.group_ip,'\0',16);
    udp_conf.remote_ip = NULL;
    ite_filter_call_method(s->Fudprecv, ITE_FILTER_UDP_RECV_SET_PARA, (void*)&udp_conf);
        
    memset(&udp_conf,'\0',sizeof(udp_config_t));
	udp_conf.remote_port = rem_port;
	udp_conf.cur_socket = -1;
	udp_conf.c_type = AUDIO_OUTPUT;	
	memset(udp_conf.group_ip,'\0',16);
	udp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Fudpsend,ITE_FILTER_UDP_SEND_SET_PARA ,(void*)&udp_conf);
    
    Castor3snd_reinit_for_diff_rate(rate,16,1);
    i2s_DAC_channel_switch(1,0);//change right as main channel
    i2s_ADC_channel_switch(1,0);//change right as main channel

    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fcc, "FC 2");
    ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
    
/*   FC 1: A->*->C   */
/*            B      */
/*   FC 2: D->*->F   */

    ite_filterChain_link(&s->fc ,-1,s->Fudprecv, -1);
    if(s->Fdecoder) ite_filterChain_link(&s->fc , 0,s->Fdecoder ,  0);
    if(s->Faec)     ite_filterChain_link(&s->fc , 0,s->Faec     ,  0);
    ite_filterChain_link(&s->fc , 0,s->Fsndwrite,  0);
    
    ite_filterChain_link(&s->fcc,-1,s->Fsndread , -1);
    if(s->Faec)     ite_filterChain_link(&s->fcc,   0,s->Faec     ,pin);
    if(s->Fencoder) ite_filterChain_link(&s->fcc, pin,s->Fencoder ,  0);
    ite_filterChain_link(&s->fcc, 0,s->Fudpsend,  0);

	//if(s->Frec_avi) ite_filterChain_A_link_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);

    ite_filterChain_run(&s->fc );
    ite_filterChain_run(&s->fcc);
    
    f->audiocase=AnalogNetCom;
    f->audiostream = s;
}

void AnalogNetFlowStop(IteFlower *f){
    int pin=0;
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AnalogNetCom) return;
    
    if(s->Faec) pin=1;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc ,-1,s->Fudprecv, -1);
    if(s->Fdecoder) ite_filterChain_unlink(&s->fc , 0,s->Fdecoder ,  0);
    if(s->Faec)     ite_filterChain_unlink(&s->fc , 0,s->Faec ,      0);
    ite_filterChain_unlink(&s->fc , 0,s->Fsndwrite,  0);
    ite_filterChain_delete(&s->fc); 
    
    ite_filterChain_stop(&s->fcc);
    ite_filterChain_unlink(&s->fcc,  -1,s->Fsndread , -1);
    if(s->Faec)     ite_filterChain_unlink(&s->fcc,   0,s->Faec ,    pin);
    if(s->Fencoder) ite_filterChain_unlink(&s->fcc, pin,s->Fencoder ,  0);
    ite_filterChain_unlink(&s->fcc, 0,s->Fudpsend ,  0);
	//if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);
    ite_filterChain_delete(&s->fcc);
    
    audio_flower_free(s);
    i2s_DAC_channel_switch(1,1);//change left as main channel
    i2s_ADC_channel_switch(1,1);//change left as main channel
}
/*sound AnalogFlowNetStop end*/

/*audio tcp call start*/
void AnalogNetTcpFlowStart(IteFlower *f, int rate, AudioCodecType type, const char *rem_ip, unsigned short rem_port){
    tcp_config_t tcp_conf;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    int pin=0;
       
    s->Ftcprecv  = ite_filter_new(ITE_FILTER_TCP_RECV_ID);
    s->Fdecoder  = ite_filter_new(ITE_FILTER_ULAW_DEC_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);
    
    s->Fsndread = ite_filter_new(ITE_FILTER_SNDREAD_ID);
    s->Fencoder = ite_filter_new(ITE_FILTER_ULAW_ENC_ID);
    s->Ftcpsend = ite_filter_new(ITE_FILTER_TCP_SEND_ID);
    
    //s->Faec      = ite_filter_new(ITE_FILTER_HDT_ID);
    if(s->Faec) pin=1;

    memset(&tcp_conf,'\0',sizeof(tcp_config_t));
    tcp_conf.remote_port = rem_port;
    tcp_conf.cur_socket = -1;
    tcp_conf.c_type = AUDIO_INPUT;
    tcp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Ftcprecv, ITE_FILTER_TCP_RECV_SET_PARA, (void*)&tcp_conf);
        
    memset(&tcp_conf,'\0',sizeof(tcp_config_t));
	tcp_conf.remote_port = rem_port;
	tcp_conf.cur_socket = -1;
	tcp_conf.c_type = AUDIO_OUTPUT;	
	tcp_conf.remote_ip = rem_ip;
    ite_filter_call_method(s->Ftcpsend,ITE_FILTER_TCP_SEND_SET_PARA ,(void*)&tcp_conf);
    
    Castor3snd_reinit_for_diff_rate(rate,16,1);
    i2s_DAC_channel_switch(1,0);//change right as main channel
    i2s_ADC_channel_switch(1,0);//change right as main channel

    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_build(&s->fcc, "FC 2");
    ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
    
/*   FC 1: A->*->C   */
/*            B      */
/*   FC 2: D->*->F   */

    ite_filterChain_link(&s->fc ,-1,s->Ftcprecv, -1);
    if(s->Fdecoder) ite_filterChain_link(&s->fc , 0,s->Fdecoder ,  0);
    if(s->Faec)     ite_filterChain_link(&s->fc , 0,s->Faec     ,  0);
    ite_filterChain_link(&s->fc , 0,s->Fsndwrite,  0);
    
    ite_filterChain_link(&s->fcc,-1,s->Fsndread , -1);
    if(s->Faec)     ite_filterChain_link(&s->fcc,   0,s->Faec     ,pin);
    if(s->Fencoder) ite_filterChain_link(&s->fcc, pin,s->Fencoder ,  0);
    ite_filterChain_link(&s->fcc, 0,s->Ftcpsend,  0);

	//if(s->Frec_avi) ite_filterChain_A_link_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);

    ite_filterChain_run(&s->fc );
    ite_filterChain_run(&s->fcc);
    
    f->audiocase=AnalogNetCom;
    f->audiostream = s;
}

void AnalogNetTcpFlowStop(IteFlower *f){
    int pin=0;
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AnalogNetCom) return;
    
    if(s->Faec) pin=1;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc ,-1,s->Ftcprecv, -1);
    if(s->Fdecoder) ite_filterChain_unlink(&s->fc , 0,s->Fdecoder ,  0);
    if(s->Faec)     ite_filterChain_unlink(&s->fc , 0,s->Faec ,      0);
    ite_filterChain_unlink(&s->fc , 0,s->Fsndwrite,  0);
    ite_filterChain_delete(&s->fc); 
    
    ite_filterChain_stop(&s->fcc);
    ite_filterChain_unlink(&s->fcc,  -1,s->Fsndread , -1);
    if(s->Faec)     ite_filterChain_unlink(&s->fcc,   0,s->Faec ,    pin);
    if(s->Fencoder) ite_filterChain_unlink(&s->fcc, pin,s->Fencoder ,  0);
    ite_filterChain_unlink(&s->fcc, 0,s->Ftcpsend ,  0);
	//if(s->Frec_avi) ite_filterChain_A_unlink_B(false, &s->fcc, s->Fsndread, 1, s->Frec_avi, 1);
    ite_filterChain_delete(&s->fcc);
    
    audio_flower_free(s);
    i2s_DAC_channel_switch(1,1);//change left as main channel
    i2s_ADC_channel_switch(1,1);//change left as main channel
}
/*sound AnalogFlowNetStop end*/

/**/
void StreamFlowStart(IteFlower *f,cb_sound_t cb){
    #if CFG_BUILD_AUDIO_MGR
    //IteFChain fc1=s->fc;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    
    s->Fsource   = ite_filter_new(ITE_FILTER_STREAMMGR_ID);//mp3 aac ...

    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);

    ite_filter_call_method(s->Fsource,ITE_FILTER_FILEOPEN ,NULL);
    if(cb) ite_filter_call_method(s->Fsndwrite,ITE_FILTER_SET_CB,cb);
    
    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_link(&s->fc, -1, s->Fsource, -1);
    ite_filterChain_link(&s->fc, 0, s->Fsndwrite, 0);

    ite_filterChain_run(&s->fc);  
    
    f->audiocase=StreamPlay;
    f->audiostream = s;
    #endif
}

void StreamFlowStop(IteFlower *f){
    #if CFG_BUILD_AUDIO_MGR
    IteAudioFlower *s=f->audiostream;

    if(f->audiocase!=StreamPlay) return;
    
    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsource, -1);
    ite_filterChain_unlink(&s->fc, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fc);
    
    audio_flower_free(s);
    #endif
}

void StreamFlowPause(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    int pause=1;
    if(f->audiocase!=StreamPlay) return;
    ite_filter_call_method(s->Fsource,ITE_FILTER_PAUSE ,&pause);
    ite_filter_call_method(s->Fsndwrite,ITE_FILTER_PAUSE ,&pause);
}

void StreamFlowResume(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    int pause=0;
    if(f->audiocase!=StreamPlay) return;
    ite_filter_call_method(s->Fsource,ITE_FILTER_PAUSE ,&pause);
    ite_filter_call_method(s->Fsndwrite,ITE_FILTER_PAUSE ,&pause);
   
}
/**/

/*mp3 mix wav start*/
void PlayFlowStartMix(IteFlower *f,const char* filepath,PlaySoundCase mode,cb_sound_t cb){

    //IteFChain fc1=s->fc;
    IteAudioFlower *s=(IteAudioFlower*)ite_new0(IteAudioFlower,1);
    char* args[] ={"-S","-Q=32",};
    int input_rate = 0;
	int output_rate = 0;
    int output_channel= 0;

    //fc: file->mix->spk
    if(audiomgrCodecType(filepath)==SMTK_AUDIO_WAV){
        s->Fsource   = ite_filter_new(ITE_FILTER_FILEPLAY_ID);//wav
    }else {
        s->Fsource   = ite_filter_new(ITE_FILTER_FILEPLAYMGR_ID);//mp3 aac ...
    }

    s->Fmix      = ite_filter_new(ITE_FILTER_MIX_ID);
    s->Fsndwrite = ite_filter_new(ITE_FILTER_SNDWRITE_ID);

	//fcc: insert file(wav)->resample->mix->spk
	s->Fsrc = ite_filter_new(ITE_FILTER_FILEPLAY_ID);//wav
	s->Fencoder = ite_filter_new(ITE_FILTER_RESAMPLE_ID);
	s->Ftee     = ite_filter_new(ITE_FILTER_CHADAPT_ID);
    
    ite_filter_call_method(s->Fsource,ITE_FILTER_FILEOPEN ,(void*)filepath);
    ite_filter_call_method(s->Fsource,ITE_FILTER_LOOPPLAY,&mode);
    if(cb) ite_filter_call_method(s->Fsndwrite,ITE_FILTER_SET_CB,cb);

	ite_filter_call_method(s->Fsource,ITE_FILTER_GETRATE,&output_rate); 
	ite_filter_call_method(s->Fencoder,ITE_FILTER_OUTPUT_RATE,&output_rate);
    ite_filter_call_method(s->Fsource,ITE_FILTER_GET_NCHANNELS,&output_channel);
    ite_filter_call_method(s->Ftee,ITE_FILTER_SET_NCHANNELS,&output_channel);
	ite_filter_call_method(s->Fsrc,ITE_FILTER_SET_BYPASS,NULL);
	
	
    
    ite_filterChain_build(&s->fc, "FC 1");
    ite_filterChain_setConfig(&s->fc, ARRAY_COUNT_OF(args), args);
    
    ite_filterChain_link(&s->fc, -1, s->Fsource, -1);
    if(s->Fmix) ite_filterChain_link(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_link(&s->fc, 0, s->Fsndwrite, 0);

	ite_filterChain_build(&s->fcc, "FC 2");
    ite_filterChain_setConfig(&s->fcc, ARRAY_COUNT_OF(args), args);
	ite_filterChain_link(&s->fcc, -1, s->Fsrc, -1);
	ite_filterChain_link(&s->fcc, 0, s->Fencoder, 0);
    ite_filterChain_link(&s->fcc, 0, s->Ftee, 0);
	ite_filterChain_link(&s->fcc, 0, s->Fmix, 1);
	ite_filterChain_link(&s->fcc, 0, s->Fsndwrite, 0);

    ite_filterChain_run(&s->fc);  
    
    f->audiocase=AudioPlayFile;
    f->audiostream = s;
}

void PlayFlowStopMix(IteFlower *f){
    IteAudioFlower *s=f->audiostream;
    if(f->audiocase!=AudioPlayFile) return;

    ite_filterChain_stop(&s->fc);
    ite_filterChain_unlink(&s->fc, -1, s->Fsource, -1);
    if(s->Fmix) ite_filterChain_unlink(&s->fc, 0, s->Fmix, 0);
    ite_filterChain_unlink(&s->fc, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fc);

	ite_filterChain_stop(&s->fcc);
    ite_filterChain_unlink(&s->fcc, -1, s->Fsrc, -1);
    if(s->Fencoder) ite_filterChain_unlink(&s->fcc, 0, s->Fencoder, 0);
    ite_filterChain_unlink(&s->fcc, 0, s->Ftee, 0);
    ite_filterChain_unlink(&s->fcc, 0, s->Fmix, 1);
	ite_filterChain_unlink(&s->fcc, 0, s->Fsndwrite, 0);
    ite_filterChain_delete(&s->fcc);
	

    audio_flower_free(s);
}

void PlayFlowMixSound(IteFlower *f,const char *fileinsert){
    IteAudioFlower *s=f->audiostream;
    IteFilter *F1=s->Fsrc;
	IteFilter *F2=s->Fencoder;
    IteFilter *F3=s->Ftee;
	IteFilter *F4=s->Fsource;
	IteFilter *Fmix=s->Fmix;
	int inpu_rate = 0;
	int channels = 0;
	int out_channels = 0;
	static bool start = false;
	int is_busy = 0;
	
	if(F1->filterDes.id==ITE_FILTER_FILEPLAY_ID){
		while(1){
			ite_filter_call_method(F1,ITE_FILTER_GET_STATUS,&is_busy);
			if(is_busy == 0){
				//printf("insert mix data!!\n");
				break;
			}
			sleep(1);
		}
	}
	if(fileinsert == NULL){
		if(F1->filterDes.id==ITE_FILTER_FILEPLAY_ID)
			ite_filter_call_method(F1,ITE_FILTER_SET_VALUE,NULL);
	}
	else{
	    if(F1->filterDes.id==ITE_FILTER_FILEPLAY_ID){
			ite_filter_call_method(F1,ITE_FILTER_FILEOPEN,(void*)fileinsert);
			ite_filter_call_method(F1,ITE_FILTER_GETRATE,&inpu_rate);
			ite_filter_call_method(F1,ITE_FILTER_GET_NCHANNELS,&channels);
			
			if(F2->filterDes.id==ITE_FILTER_RESAMPLE_ID){
				ite_filter_call_method(F2,ITE_FILTER_INPUT_RATE,&inpu_rate);
				ite_filter_call_method(F2,ITE_FILTER_SET_NCHANNELS,&channels);
			}
	        if(F3->filterDes.id==ITE_FILTER_CHADAPT_ID){
				if(F4->filterDes.id==ITE_FILTER_FILEPLAYMGR_ID){
					ite_filter_call_method(F4,ITE_FILTER_GET_NCHANNELS,&out_channels);
				}
	            ite_filter_call_method(F3,ITE_FILTER_SET_VALUE,&channels);//set input channel
	            ite_filter_call_method(F3,ITE_FILTER_SET_NCHANNELS,&out_channels);//set output channel
	        }
	        
	    }
	}
	if(start == false){
		ite_filterChain_run(&s->fcc);
		start = true;
	}
	
}
/*mp3 mix wav end*/

void AudioStreamCancel(IteFlower *f){
    //check audiostream be NULL;
    if(f->audiostream!=NULL){
        PlayFlowStop(f);
        RecFlowStop(f);
		if(f->audiostream->Ftcprecv || f->audiostream->Ftcpsend)
			AudioTcpFlowStop(f);
		else
        	AudioFlowStop(f);
        AnalogFlowStop(f);
		if(f->audiostream->Ftcprecv || f->audiostream->Ftcpsend)
			AnalogNetTcpFlowStop(f);
		else
        	AnalogNetFlowStop(f);
        SndrwFlowStop(f);
        #ifdef CFG_BUILD_ASR
        ResmplePlayFlowStop(f);
        #endif
        StreamFlowStop(f);
        f->audiocase=AudioIdel;
        f->audiostream=NULL;
    }
}