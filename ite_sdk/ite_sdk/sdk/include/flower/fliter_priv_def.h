/*
 * Copyright (c) 2004 ITE Technology Corp. All Rights Reserved.
 */
/** @file
 * each filter private defines.
 *
 * @author
 * @version 1.0
 */
#ifndef FILTERPRIVDEFINES_H
#define FILTERPRIVDEFINES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "lwip/sockets.h"

#define max_call_num         4
//=============================================================================
//                              Structure Definition
//=============================================================================
/**
 * stream flow definition
 */
typedef enum{
	AUDIO_INPUT = 0,
	AUDIO_OUTPUT,
	VIDEO_INPUT,
	VIDEO_OUTPUT,
}MEDIA_TYPE;

/**
 * OSD MODE definition 
 */

typedef enum{
	OSD_ON,
    PREVIEW_ON,
	PREVIEW_OSD_ON,
}OSDMODE_TYPE;


/**
 * UDP socket definition
 */
typedef struct udp_config{
	MEDIA_TYPE c_type;
	struct ip_mreq mreq;
	unsigned char group_ip[16];
	void* remote_ip;
	unsigned short remote_port;
	int enable_multicast;
	int cur_socket;
}udp_config_t;

/**
 * TCP socket definition
 */
typedef struct tcp_config{
	MEDIA_TYPE c_type;
	void* remote_ip;
	unsigned short remote_port;
	int cur_socket;
	int cur_cli_socket[max_call_num];
	bool connect_accepted[max_call_num];
}tcp_config_t;


/**
 * Sensor Stream Data definition
 */
struct _IteSensorStream{
    unsigned int  Width;
    unsigned int  Height;
    unsigned int  PitchY;
    unsigned int  PitchUV;
    unsigned int  DataAddrY;
    unsigned int  DataAddrU;
    unsigned int  DataAddrV;
	unsigned int  Framerate;
    unsigned char DataFormat;
    unsigned char Interlanced;
};
typedef struct _IteSensorStream IteSensorStream;

typedef struct _ISPQUEUEINFO{
    unsigned char FinishBufferIdx;             /*finish buffer index                   */
    unsigned char CurrentBufferIdx;            /*current  write buffer index           */
};
typedef struct _ISPQUEUEINFO ISPQUEUEINFO;

/**
 * ISP Out Put Data definition
 */
struct _ISPOutPutStream {
	unsigned int  Width;
	unsigned int  Height;
    unsigned int  PitchY;
    unsigned int  PitchUV;
	unsigned int  BufVramAddr;
	unsigned int  BufSysAddr;
	unsigned int  BufferCount;
    unsigned int  DataAddrY[5];
    unsigned int  DataAddrUV[5];
	unsigned int  Framerate;
};
typedef struct _ISPOutPutStream ISPOutPutStream ;

//=============================================================================
//                              Global Data Definition
//=============================================================================
#define DEF_BitStream_BUF_LENGTH 	(256 << 10)
#define DEF_FileStream_Name_LENGTH 	(256)

//=============================================================================
//                              Public Function Definition
//=============================================================================
extern void (*filewriter_callback)(void *arg);


#ifdef __cplusplus
}
#endif

#endif /* FILTERPRIVDEFINES_H */
