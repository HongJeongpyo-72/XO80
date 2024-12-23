#ifndef __VIDEO_DEVICE_TABLE_H__
#define __VIDEO_DEVICE_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "ite/mmp_types.h"
#include "mmp_capture.h"
//=============================================================================
//                Constant Definition
//=============================================================================
//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
//=============================================================================
//                HDMIRX
//=============================================================================
typedef struct CAP_HDMI_TIMINFO_TABLE_TAG
{
    MMP_UINT16 Index;
    MMP_UINT16 HActive;
    MMP_UINT16 VActive;
    MMP_UINT16 HTotal;
    MMP_UINT16 VTotal;
    MMP_UINT16 FrameRate;
    MMP_UINT16 ROIPosX;
    MMP_UINT16 ROIPosY;
    MMP_UINT16 ROIWidth;
    MMP_UINT16 ROIHeight;
    MMP_UINT16 Skippattern;
    MMP_UINT16 SkipPeriod;
    MMP_UINT16 HPolarity  : 1;
    MMP_UINT16 VPolarity  : 1;
    MMP_UINT16 CLKInverse : 1;
    MMP_UINT16 ClkDelay;
} CAP_HDMI_TIMINFO_TABLE;

#ifdef CFG_IT66021
//IT66021 HDMI Table
static CAP_HDMI_TIMINFO_TABLE HDMI_TABLE [] = {
    //CEA timing
    //Index, HActive, VActive,    HTotal,     VTotal,       FrameRate,                           ROIX, ROIY,    ROIW, ROIH,   SkipPattern,  SkipPeriod,   Hpor,      Vpor,CLKInv, Clkdelay
    { 0,      720,    480,    858,    262, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   720,   480,      0xF,      0x3,       0,      0,  0x0, 0xb     }, //480i60
    { 1,      720,    480,    858,    525, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   720,   480,      0xF,      0x3,       0,      0,  0x0, 0xb     }, //480P60
    { 2,      720,    576,    864,    312, MMP_CAP_FRAMERATE_50HZ,           0, 0,   720,   576,      0xF,      0x3,       0,      0,  0x0, 0xb     }, //576i50
    { 3,      720,    576,    864,    625, MMP_CAP_FRAMERATE_50HZ,           0, 0,   720,   576,      0xF,      0x3,       0,      0,  0x0, 0xb     }, //576P50
    { 4,     1280,    720,   1650,    750, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1280,   720,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //720P60
    { 5,     1280,    720,   1980,    750, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1280,   720,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //720P50
    { 6,     1920,   1080,   2200,   1125, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0x4     }, //1080P60
    { 7,     1920,   1080,   2640,   1125, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0x4     }, //1080P50
    { 8,     1920,   1080,   2200,    562, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //1080i60
    { 9,     1920,   1080,   2640,    562, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //1080i50
    {10,     1920,   1080,   2750,   1125, MMP_CAP_FRAMERATE_24HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //1080P24
    {11,      640,    480,    800,    525, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   640,   480,      0xF,      0x3,       0,      0,  0x0, 0xb     }, //640p60
    {12,     1920,   1080,   2200,   1125, MMP_CAP_FRAMERATE_29_97HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //1080P30
    {13,     1920,   1080,   2640,   1125, MMP_CAP_FRAMERATE_25HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xb     }, //1080P25
    //VESA timing
    //Index, HActive, VActive,    HTotal,    VTotal,          FrameRate,                          ROIX, ROIY,  ROIW,   ROIH,    SkipPattern,  SkipPeriod, Hpor,      Vpor,CLKInv,Clkdelay
    {14,      800,    600,   1056,    628, MMP_CAP_FRAMERATE_60HZ,           0, 0,   800,   600,      0xF,      0x3,       1,      1,  0x1, 0x0     }, //800x600_60P
    {15,     1024,    768,   1344,    806, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1024,   768,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //1024x768_60P
    {16,     1280,    768,   1664,    798, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   768,      0xF,      0x3,       0,      1,  0x0, 0xf     }, //1280X768_60P
    {17,     1280,    800,   1440,    823, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   800,      0xF,      0x3,       1,      0,  0x0, 0xf     }, //1280X800_60P
    {18,     1280,    960,   1800,   1000, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   960,      0xF,      0x3,       1,      1,  0x0, 0x6     }, //1280X960_60P
    {19,     1280,   1024,   1688,   1066, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,  1024,      0xF,      0x3,       1,      1,  0x1, 0x0     }, //1280X1024_60P
    {20,     1360,    768,   1792,    795, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1360,   768,      0xF,      0x3,       1,      1,  0x0, 0xa     }, //1360X768_60P
    {21,     1366,    768,   1500,    800, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1366,   768,      0xF,      0x3,       1,      1,  0x0, 0xa     }, //1366X768_60P
    {22,     1400,   1050,   1864,   1089, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1400,  1050,      0xF,      0x3,       0,      1,  0x1, 0x0     }, //1400X1050_60P
    {23,     1440,    900,   1904,    934, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1440,   900,      0xF,      0x3,       0,      1,  0x1, 0x0     }, //1440X900_60P
    {24,     1600,    900,   1800,   1000, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1600,   900,      0xF,      0x3,       1,      1,  0x0, 0x5     }, //1600X900_60P
    {25,     1600,   1200,   2160,   1250, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1600,  1200,      0xF,      0x3,       1,      1,  0x0, 0x2     }, //1600X1200_60P
    {26,     1680,   1050,   2240,   1089, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1680,  1050,      0xF,      0x3,       0,      1,  0x0, 0x4     }, //1680X1050_60P
    {27,     1680,   1050,   1840,   1080, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1680,  1050,      0xF,      0x3,       0,      1,  0x0, 0x2     }, //1680X1050_60P(apple tv)  
    {28,     1440,    900,   1600,    926, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1440,   900,      0xF,      0x3,       0,      1,  0x0, 0x2     }, //1440X900_60P(apple tv)   
};
#else
//IT6801 HDMI Table
static CAP_HDMI_TIMINFO_TABLE HDMI_TABLE [] = {
    //CEA timing
    //Index, HActive, VActive,    HTotal,     VTotal,       FrameRate,                           ROIX, ROIY,    ROIW, ROIH,   SkipPattern,  SkipPeriod,   Hpor,      Vpor,CLKInv, Clkdelay
    { 0,      720,    480,    858,    262, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   720,   480,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //480i60
    { 1,      720,    480,    858,    525, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   720,   480,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //480P60
    { 2,      720,    576,    864,    312, MMP_CAP_FRAMERATE_50HZ,           0, 0,   720,   576,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //576i50
    { 3,      720,    576,    864,    625, MMP_CAP_FRAMERATE_50HZ,           0, 0,   720,   576,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //576P50
    { 4,     1280,    720,   1650,    750, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1280,   720,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //720P60
    { 5,     1280,    720,   1980,    750, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1280,   720,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //720P50
    { 6,     1920,   1080,   2200,   1125, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080P60
    { 7,     1920,   1080,   2640,   1125, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080P50
    { 8,     1920,   1080,   2200,    562, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080i60
    { 9,     1920,   1080,   2640,    562, MMP_CAP_FRAMERATE_50HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080i50
    {10,     1920,   1080,   2750,   1125, MMP_CAP_FRAMERATE_24HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080P24
    {11,      640,    480,    800,    525, MMP_CAP_FRAMERATE_59_94HZ,        0, 0,   640,   480,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //640p60
    {12,     1920,   1080,   2200,   1125, MMP_CAP_FRAMERATE_29_97HZ,        0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080P30
    {13,     1920,   1080,   2640,   1125, MMP_CAP_FRAMERATE_25HZ,           0, 0,  1920,  1080,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1080P25
    //VESA timing
    //Index, HActive, VActive,    HTotal,    VTotal,          FrameRate,                          ROIX, ROIY,  ROIW,   ROIH,    SkipPattern,  SkipPeriod, Hpor,      Vpor,CLKInv,Clkdelay
    {14,      800,    600,   1056,    628, MMP_CAP_FRAMERATE_60HZ,           0, 0,   800,   600,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //800x600_60P
    {15,     1024,    768,   1344,    806, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1024,   768,      0xF,      0x3,       0,      0,  0x0, 0xf     }, //1024x768_60P
    {16,     1280,    720,   1664,    746, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   720,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1280X720_60P
    {17,     1280,    768,   1664,    798, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   768,      0xF,      0x3,       0,      1,  0x0, 0xf     }, //1280X768_60P
    {18,     1280,    800,   1440,    823, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   800,      0xF,      0x3,       1,      0,  0x0, 0xf     }, //1280X800_60P
    {19,     1280,    960,   1800,   1000, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,   960,      0xF,      0x3,       1,      1,  0x0, 0x8     }, //1280X960_60P
    {20,     1280,   1024,   1688,   1066, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1280,  1024,      0xF,      0x3,       1,      1,  0x0, 0x8     }, //1280X1024_60P
    {21,     1360,    768,   1792,    795, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1360,   768,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1360X768_60P
    {22,     1366,    768,   1500,    800, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1366,   768,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1366X768_60P
    {23,     1400,   1050,   1864,   1089, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1400,  1050,      0xF,      0x3,       0,      1,  0x0, 0xf     }, //1400X1050_60P
    {24,     1440,    900,   1904,    934, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1440,   900,      0xF,      0x3,       0,      1,  0x0, 0x8     }, //1440X900_60P
    {25,     1600,   1200,   2160,   1250, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1600,  1200,      0xF,      0x3,       1,      1,  0x0, 0xf     }, //1600X1200_60P
    {26,     1680,   1050,   2240,   1089, MMP_CAP_FRAMERATE_60HZ,           0, 0,  1680,  1050,      0xF,      0x3,       0,      1,  0x0, 0xf     }, //1680X1050_60P
};
#endif
//=============================================================================
//                NORMAL VIDEO TABLE
//=============================================================================
typedef struct CAP_NORMAL_TIMINFO_TABLE_TAG
{
    MMP_UINT16 Index;
    MMP_UINT16 HActive;
    MMP_UINT16 VActive;
    MMP_UINT16 FrameRate;
    MMP_UINT16 ROIPosX;
    MMP_UINT16 ROIPosY;
    MMP_UINT16 ROIWidth;
    MMP_UINT16 ROIHeight;
    MMP_UINT16 HNum1;
    MMP_UINT16 LineNum1;
    MMP_UINT16 LineNum2;
    MMP_UINT16 LineNum3;
    MMP_UINT16 LineNum4;
    MMP_UINT16 ClkDelay;
} CAP_NORMAL_TIMINFO_TABLE;

//Normal Video Table , if add new sensor , you need add your timing in here.
static CAP_NORMAL_TIMINFO_TABLE VIDEO_TABLE [] = {
#if 1 /* added by sjjang for tp2825*/
   { 0,  	 960,     480,	MMP_CAP_FRAMERATE_29_97HZ,	  0,	0,	  960,   480,	0,	    0,	      0,	    0, 	     0,	      0x6},	 //FOR KOCOM CVBS NTSC CAM
   { 5, 	 960,	  576,	MMP_CAP_FRAMERATE_25HZ,		  0,	0,	  960,	 576,	0, 	    0,		  0,		0,		 0,       0x6},  //FOR KOCOM CVBS PAL CAM
   { 1,     1280, 	  720,	MMP_CAP_FRAMERATE_30HZ,		  0,	0,	 1280,   720, 	0,	    0,	      0,	    0,       0,		  0xc},  //FOR KOCOM AHD,TVI,CVI	720P30	CAM
   { 2,     1280, 	  720,	MMP_CAP_FRAMERATE_25HZ,       0,	0,	 1280,   720, 	0,	    0,	      0,	    0,       0,	      0xc},  //FOR KOCOM AHD,TVI,CVI	720P25	CAM
   { 3, 	 960,	 1080,	MMP_CAP_FRAMERATE_30HZ,       0,	0,	  960,  1080, 	0,	    0,	      0,	    0,       0,	      0xc},  //FOR KOCOM AHD,TVI,CVI 1080P30	CAM
   { 4,  	 960,	 1080,	MMP_CAP_FRAMERATE_25HZ,       0,	0,	  960,  1080, 	0,	    0,	      0,	    0,       0,	      0xc},  //FOR KOCOM AHD,TVI,CVI 1080P25	CAM
#else                                                                                                                                                   //Index, HActive, VActive,    FRate,   ROIX, ROIY, ROIW,   ROIH,    HNum1,   LineNum1,      LineNum2,         LineNum3,   LineNum4,     CLKDELAY
    {0,      720,    487, MMP_CAP_FRAMERATE_59_94HZ,        0,    0,  720,     480,       0,        0,          0,             0,       0,      0x0}, //480i60 //for ADV7180 ,RN6854M and RN6752V1
    {1,      720,    480, MMP_CAP_FRAMERATE_59_94HZ,        0,    0,  720,     480,       0,        0,          0,             0,       0,      0x0}, //480i60
    {2,      720,    576, MMP_CAP_FRAMERATE_50HZ,           0,    0,  720,     576,       0,        0,          0,             0,       0,      0x4}, //576i50
    {3,     1280,    720, MMP_CAP_FRAMERATE_25HZ,           0,    0, 1280,     720,       0,        0,          0,             0,       0,      0x0}, //720P25
    {4,     1280,    720, MMP_CAP_FRAMERATE_30HZ,           0,    0, 1280,     720,       0,        0,          0,             0,       0,      0x0}, //720P30
    {5,     1280,    960, MMP_CAP_FRAMERATE_30HZ,           0,    0, 1280,     960,       0,        0,          0,             0,       0,      0x0}, //960P30
    {6,     1920,   1080, MMP_CAP_FRAMERATE_25HZ,           0,    0, 1920,    1080,       0,        0,          0,             0,       0,      0x0}, //1080P25
    {7,     1920,   1080, MMP_CAP_FRAMERATE_30HZ,           0,    0, 1920,    1080,       0,        0,          0,             0,       0,      0x0}, //1080P30
  	{8,     1600,   1200, MMP_CAP_FRAMERATE_23_97HZ,        0,    0, 1600,    1200,       0,        0,          0,             0,       0,      0x0}, //1200P23    
#endif  	
};

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif