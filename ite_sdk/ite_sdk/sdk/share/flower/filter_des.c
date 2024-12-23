#include <malloc.h>
#include <string.h>
#include "flower/flower.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
extern IteFilterDes FilterA;
extern IteFilterDes FilterB;
extern IteFilterDes FilterC;
extern IteFilterDes FilterD;
extern IteFilterDes FilterE;
extern IteFilterDes FilterF;
#if defined(CFG_NET_ENABLE)
extern IteFilterDes FilterUDPsend;
extern IteFilterDes FilterUDPrecv;
extern IteFilterDes FilterTCPsend;
extern IteFilterDes FilterTCPrecv;
#endif
#if defined(CFG_VIDEO_ENABLE)
extern IteFilterDes FilterCaptureOnfly;
extern IteFilterDes FilterISPOnfly;
extern IteFilterDes FilterH264ENC;
extern IteFilterDes FilterH264DEC;
extern IteFilterDes FilterDisplay;
extern IteFilterDes FilterDisplayOsd;
extern IteFilterDes FilterJpegWriter;
extern IteFilterDes FilterFileWriter;
extern IteFilterDes FilterRecAVI;
extern IteFilterDes FilterIPCam;
#endif
#if defined(CFG_QR_DECODE_ENABLE)
extern IteFilterDes FilterQRScanner;
#endif

#ifdef CFG_AUDIO_ENABLE
extern IteFilterDes FilterSndWtire;
extern IteFilterDes FilterSndRead;
extern IteFilterDes FilterFilePlay;
    #if CFG_BUILD_AUDIO_MGR
extern IteFilterDes FilterFilePlayMgr;
extern IteFilterDes FilterStreamMgr;
    #endif
extern IteFilterDes FilterFileRec;
extern IteFilterDes FilterFileMix;
extern IteFilterDes FilterHDT;
extern IteFilterDes FilterCSeparate;
extern IteFilterDes FilterCMerge;
extern IteFilterDes FilterUlawEnc;
extern IteFilterDes FilterUlawDec;
extern IteFilterDes FilterAlawEnc;
extern IteFilterDes FilterAlawDec;
extern IteFilterDes FilterVoid;
extern IteFilterDes FilterTee;
extern IteFilterDes	FilterVolume;
extern IteFilterDes FilterResample;
extern IteFilterDes FilterChadapt;
extern IteFilterDes FilterMix;
extern IteFilterDes FilterLoopBack;
    #if CFG_BUILD_AUDIO_PREPROCESS
extern IteFilterDes	FilterDenoise;
extern IteFilterDes FilterAec;
    #endif
    #if CFG_BUILD_ASR
extern IteFilterDes FilterAsr;
    #endif 
#endif

// Filter Set
IteFilterDes *gFilterDesSet[] = {
    &FilterA,
    &FilterB,
    &FilterC,
    &FilterD,
    &FilterE,
    &FilterF,
#if defined(CFG_NET_ENABLE)    
    &FilterUDPsend,
    &FilterUDPrecv,
    &FilterTCPsend,
    &FilterTCPrecv,
#endif
#if defined(CFG_VIDEO_ENABLE)
	&FilterCaptureOnfly,
	&FilterISPOnfly,
	&FilterH264ENC,
	&FilterH264DEC,
	&FilterDisplay,
	&FilterDisplayOsd,
	&FilterJpegWriter,
	&FilterFileWriter,
	&FilterRecAVI,
	&FilterIPCam,
#endif
#if defined(CFG_QR_DECODE_ENABLE)
	&FilterQRScanner,
#endif
#ifdef CFG_AUDIO_ENABLE
    &FilterSndWtire,
    &FilterSndRead,
    &FilterFilePlay,
    #if CFG_BUILD_AUDIO_MGR
    &FilterFilePlayMgr,
    &FilterStreamMgr,
    #endif
    &FilterFileRec,
    &FilterFileMix,
    &FilterHDT,
    &FilterCSeparate,
    &FilterCMerge,
    &FilterUlawEnc,
    &FilterUlawDec,
    &FilterAlawEnc,
    &FilterAlawDec,
    &FilterVoid,
    &FilterTee,
    &FilterVolume,
    &FilterResample,
    &FilterChadapt,
	&FilterMix,
    &FilterLoopBack,
    #if CFG_BUILD_AUDIO_PREPROCESS
    &FilterAec,
    &FilterDenoise,
    #endif
    #if CFG_BUILD_ASR
    &FilterAsr,
    #endif
#endif
    NULL
};



