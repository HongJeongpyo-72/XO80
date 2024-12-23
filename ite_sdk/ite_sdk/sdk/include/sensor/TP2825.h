#ifndef __TP2825_H__
#define __TP2825_H__

#if defined(CFG_SENSOR_ENABLE)

#include "ite/itp.h"
#include "ith/ith_defs.h"
#include "ite/ith.h"
#include "mmp_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif






#if 1
	 #define USED_SVM_SIDE					// JPHong 2023-01-31
	
	#ifdef USED_SVM_SIDE					// Side : DEMO_978_DUALCAP_XO-24-04-16_SWAP.txt
		#define TP2825B_I2C_A 		0x8A
		#define TP2825B_I2C_B 		0x88
		#define SENSOR_IIC_0		3
		#define SENSOR_IIC_3		0
		#define XO_SN1_GPIO_RST1 	14
		#define XO_SN1_GPIO_INTR1	93
		#define XO_SN1_GPIO_RST2 	95
		#define XO_SN1_GPIO_INTR2	91
	#else									// Rear : DEMO_978_DUALCAP_XO-24-04-16.txt
		#define TP2825B_I2C_A 		0x88
		#define TP2825B_I2C_B 		0x8A
		#define SENSOR_IIC_0		0
		#define SENSOR_IIC_3		3
		#define XO_SN1_GPIO_RST1 	95
		#define XO_SN1_GPIO_INTR1	91
		#define XO_SN1_GPIO_RST2 	14
		#define XO_SN1_GPIO_INTR2	93
	#endif

#endif


//X10LightDriver_t1.h
void TP2825_init(void);
typedef struct TP2825SensorDriverStruct *TP2825SensorDriver;
SensorDriver TP2825SensorDriver_Create();
static void TP2825SensorDriver_Destory(SensorDriver base);
void TP2825Initialize(uint16_t Mode);
void TP2825Terminate(void);
void TP2825OutputPinTriState(uint8_t flag);
uint16_t TP2825GetProperty(MODULE_GETPROPERTY property);
uint8_t TP2825GetStatus(MODULE_GETSTATUS Status);
void TP2825PowerDown(uint8_t enable);
uint8_t TP2825IsSignalStable(uint16_t Mode);


int tp2825_cam_running();


void TP2825_2_init(void);
typedef struct TP2825_2SensorDriverStruct *TP2825_2SensorDriver;
SensorDriver TP2825_2SensorDriver_Create();
static void TP2825_2SensorDriver_Destory(SensorDriver base);
void TP2825_2Initialize(uint16_t Mode);
void TP2825_2Terminate(void);
void TP2825_2OutputPinTriState(uint8_t flag);
uint16_t TP2825_2GetProperty(MODULE_GETPROPERTY property);
uint8_t TP2825_2GetStatus(MODULE_GETSTATUS Status);
void TP2825_2PowerDown(uint8_t enable);
uint8_t TP2825_2IsSignalStable(uint16_t Mode);


int tp2825_2_cam_running();


//end of X10LightDriver_t1.h

enum{
	ITE_CVBS_NTSC	 = 0x01,
	ITE_CVBS_PAL  	= 0x02,

	ITE_AHD_720P25  =	  0x03,
	ITE_AHD_720P30  =	    0x04,	
	ITE_AHD_1080P25  =	    0x05,
	ITE_AHD_1080P30  =	    0x06,

	ITE_TVI_720P25  =	    0x07,
	ITE_TVI_720P30  =	    0x08,
	ITE_TVI_1080P25  =	    0x09,
	ITE_TVI_1080P30  =	    0x0a,	

	ITE_CVI_720P25  =	    0x0b,
	ITE_CVI_720P30  =	    0x0c,
	ITE_CVI_1080P25  =	    0x0d,
	ITE_CVI_1080P30  =	    0x0e	
};
#ifdef __cplusplus
}
#endif

#else /*CFG_SENSOR_ENABLE*/
#include "ite/itp.h"
#include "ite/ith_defs.h"
#include "ite/ith.h"
#include "mmp_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

int tp2825_cam_available(void);
void TP2825B_SetRX(uint8_t index);
void TP2825B_SetTX(uint8_t index);

typedef struct TP2825SensorDriverStruct *TP2825SensorDriver;
SensorDriver TP2825SensorDriver_Create();
//static void TP2825SensorDriver_Destory(SensorDriver base);
//void TP2825Initialize(uint16_t Mode);
//void TP2825Terminate(void);
//void TP2825OutputPinTriState(uint8_t flag);
//uint16_t TP2825GetProperty(MODULE_GETPROPERTY property);
//uint8_t TP2825GetStatus(MODULE_GETSTATUS Status);
//void TP2825PowerDown(uint8_t enable);
//uint8_t TP2825IsSignalStable(uint16_t Mode);

//enum {
//	ITE_CVBS_NTSC	= 0x01,
//	ITE_CVBS_PAL  	= 0x02,
//
//	ITE_AHD_720P25	= 0x03,
//	ITE_AHD_720P30	= 0x04,
//	ITE_AHD_1080P25	= 0x05,
//	ITE_AHD_1080P30	= 0x06,
//
//	ITE_TVI_720P25	= 0x07,
//	ITE_TVI_720P30	= 0x08,
//	ITE_TVI_1080P25	= 0x09,
//	ITE_TVI_1080P30	= 0x0a,
//
//	ITE_CVI_720P25	= 0x0b,
//	ITE_CVI_720P30	= 0x0c,
//	ITE_CVI_1080P25	= 0x0d,
//	ITE_CVI_1080P30	= 0x0e
//};

enum {
	CH1 = 0,
	CH2,
	CH3,
	CH4,
	AutoSel
};

enum {
	VIN1_Y = 0,
	VIN2_Y,
	VIN3_Y,
	VIN4_Y,
	VIN1_C,
	VIN2_C,
	VIN3_C,
	VIN4_C
};

enum {
	TP2802_720P60       = 	0x00,
	TP2802_720P50       =   0x01,
	TP2802_1080P30      =	0x02,
	TP2802_1080P25      =	0x03,
	TP2802_720P30       =   0x04,
	TP2802_720P25       =	0x05,
	TP2802_SD           =   0x06,
	INVALID_FORMAT      =	0x07,
	TP2802_PAL	        =   0x08,
	TP2802_NTSC	        =   0x09,
	TP2802_TVI_1080P30  =   0x0a,
	TP2802_720P30V2     =	0x0c,
	TP2802_720P25V2     =	0x0d,
	TP2802_3M18         =   0x20,   //2048x1536@18.75 for TVI
	TP2802_5M12         =   0x21,   //2592x1944@12.5 for TVI
	TP2802_4M15         =   0x22,   //2688x1520@15 for TVI
	TP2802_3M20         =   0x23,   //2048x1536@20 for TVI
	TP2802_4M12         =   0x24,   //2688x1520@12.5 for TVI
	TP2802_6M10         =   0x25,   //3200x1800@10 for TVI
	TP2802_QHD30        =   0x26,   //2560x1440@30 for TVI/HDA/HDC
	TP2802_QHD25        =   0x27,   //2560x1440@25 for TVI/HDA/HDC
	TP2802_QHD15        =   0x28,   //2560x1440@15 for HDA
	TP2802_QXGA18       =   0x29,   //2048x1536@18 for HDA/TVI
	TP2802_QXGA30       =   0x2a,   //2048x1536@30 for HDA
	TP2802_QXGA25       =   0x2b,   //2048x1536@25 for HDA
	TP2802_4M30         =   0x2c,   //2688x1520@30 for TVI(for future)
	TP2802_4M25         =   0x2d,   //2688x1520@25 for TVI(for future)
	TP2802_5M20         =   0x2e,   //2592x1944@20 for TVI/HDA
	TP2802_8M15         =   0x2f,   //3840x2160@15 for TVI
	TP2802_8M12         =   0x30,   //3840x2160@12.5 for TVI
	TP2802_1080P15      =   0x31,   //1920x1080@15 for TVI
	TP2802_1080P60      =   0x32,   //1920x1080@60 for TVI
	TP2802_960P30       =   0x33,   //1280x960@30 for TVI
	TP2802_1080P20      =   0x34    //1920x1080@20 for TVI
};

enum {
    VIDEO_UNPLUG = 0,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};

enum {
    STD_TVI = 0,
    STD_HDA,
    STD_HDC,
    STD_HDA_DEFAULT,
    STD_HDC_DEFAULT
};

enum {
    PTZ_TVI  = 0,
    PTZ_HDA_1= 1,
    PTZ_HDC_1= 4,
    PTZ_HDA_2= 6,
    PTZ_HDC_2= 5,
	PTZ_CVBS = 3
};

enum {
    RES_1080P = 0,
    RES_720P,
    RES_480I,
    RES_576I
};

#define FLAG_LOSS        0x80
#define FLAG_LOCKED      0x60

#define Bits16out	     0
#define Bits8out	     1

#define NoVid	         0
#define IsVid	         1

#define	USER_DATA_UPDATE 157
#define	FIRMWARE_UPDATE	 158
#define	FONT_UPDATE      159

#define	BLOCK_LENGTH	 128

////BIT0 - BIT31
//#define C_BIT0                                            0x01
//#define C_BIT1                                            0x02
//#define C_BIT3                                            0x08
//#define C_BIT2                                            0x04
//#define C_BIT4                                            0x10
//#define C_BIT5                                            0x20
//#define C_BIT6                                            0x40
//#define C_BIT7                                            0x80

#ifdef __cplusplus
}
#endif

#endif /*CFG_SENSOR_ENABLE*/
#endif	/* __TP2825_H__ */
