///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <config.h>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2018/05/10
//   @fileversion: ITE_MHLRX_SAMPLE_V1.27
//******************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define _MCU_8051_

//#ifndef _MCU_8051_
//#include "platform.h"
//#endif

/*IC type define*/
#if (!defined(_IT6801_))&&(!defined(_IT6802_))&&(!defined(_IT6803_))
    //#define _IT6802_
    #define _IT6801_
#endif

#ifdef _IT6803_
    #pragma message("defined _IT6803_")
    //#define ENABLE_IT6803	// only for IT6803 Usage
#endif


#ifdef _IT6802_
#pragma message("defined _IT6802_")
#endif

#ifdef _IT6801_
//#pragma message("defined _IT6801_")
#endif

//#define SUPPORT_I2C_SLAVE
#ifdef SUPPORT_I2C_SLAVE
#pragma message ("SUPPORT_I2C_SLAVE defined")
#endif

#define _EN_DUAL_PIXEL_CTRL_
#define _EN_BLOCK_PWRDN_
#define SUPPORT_OUTPUTRGB
#define SUPPORT_INPUTYUV

#endif // _CONFIG_H_
