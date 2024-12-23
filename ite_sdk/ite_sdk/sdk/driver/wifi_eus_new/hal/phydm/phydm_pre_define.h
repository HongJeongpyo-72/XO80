/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/


#ifndef	__PHYDMPREDEFINE_H__
#define    __PHYDMPREDEFINE_H__

/* 1 ============================================================
 * 1  Definition
 * 1 ============================================================ */

#define PHYDM_CODE_BASE		"PHYDM_V024"
#define PHYDM_RELEASE_DATE		"20171213"

/*PHYDM API status*/
#define	PHYDM_SET_FAIL		0
#define	PHYDM_SET_SUCCESS	1
#define	PHYDM_SET_NO_NEED	3

/*PHYDM Set/Revert*/
#define	PHYDM_SET				1
#define	PHYDM_REVERT			2

/* Max path of IC */
/*N-IC*/
#define MAX_PATH_NUM_8188E		1
#define MAX_PATH_NUM_8188F		1
#define MAX_PATH_NUM_8710B		1
#define MAX_PATH_NUM_8723B		1
#define MAX_PATH_NUM_8723D		1
#define MAX_PATH_NUM_8703B		1
#define MAX_PATH_NUM_8192E		2
#define MAX_PATH_NUM_8192F		2
#define MAX_PATH_NUM_8197F		2
#define MAX_PATH_NUM_8198F		4
/*AC-IC*/
#define MAX_PATH_NUM_8821A		1
#define MAX_PATH_NUM_8881A		1
#define MAX_PATH_NUM_8821C		1
#define MAX_PATH_NUM_8195B		1
#define MAX_PATH_NUM_8812A		2
#define MAX_PATH_NUM_8822B		2
#define MAX_PATH_NUM_8822C		2
#define MAX_PATH_NUM_8814A		4
#define MAX_PATH_NUM_8814B		4
#define MAX_PATH_NUM_8814C		4

/* Max RF path */
#define PHYDM_MAX_RF_PATH_N		2	/*For old N-series IC*/
#define PHYDM_MAX_RF_PATH			4

/* number of entry */
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE))
	#ifdef DM_ODM_CE_MAC80211
		/* defined in wifi.h (32+1) */
	#else
		#define	ASSOCIATE_ENTRY_NUM					MACID_NUM_SW_LIMIT  /* Max size of asoc_entry[].*/
	#endif
	#define	ODM_ASSOCIATE_ENTRY_NUM				ASSOCIATE_ENTRY_NUM
#elif(DM_ODM_SUPPORT_TYPE & (ODM_AP))
	#define ASSOCIATE_ENTRY_NUM					NUM_STAT
	#define	ODM_ASSOCIATE_ENTRY_NUM				(ASSOCIATE_ENTRY_NUM+1)
#else
	#define ODM_ASSOCIATE_ENTRY_NUM				((ASSOCIATE_ENTRY_NUM*3)+1)
#endif

/* -----MGN rate--------------------------------- */

enum ODM_MGN_RATE {
	ODM_MGN_1M		= 0x02,
	ODM_MGN_2M		= 0x04,
	ODM_MGN_5_5M		= 0x0B,
	ODM_MGN_6M		= 0x0C,
	ODM_MGN_9M		= 0x12,
	ODM_MGN_11M		= 0x16,
	ODM_MGN_12M		= 0x18,
	ODM_MGN_18M		= 0x24,
	ODM_MGN_24M		= 0x30,
	ODM_MGN_36M		= 0x48,
	ODM_MGN_48M		= 0x60,
	ODM_MGN_54M		= 0x6C,
	ODM_MGN_MCS32	= 0x7F,
	ODM_MGN_MCS0		= 0x80,
	ODM_MGN_MCS1,
	ODM_MGN_MCS2,
	ODM_MGN_MCS3,
	ODM_MGN_MCS4,
	ODM_MGN_MCS5,
	ODM_MGN_MCS6,
	ODM_MGN_MCS7		= 0x87,
	ODM_MGN_MCS8,
	ODM_MGN_MCS9,
	ODM_MGN_MCS10,
	ODM_MGN_MCS11,
	ODM_MGN_MCS12,
	ODM_MGN_MCS13,
	ODM_MGN_MCS14,
	ODM_MGN_MCS15,
	ODM_MGN_MCS16	= 0x90,
	ODM_MGN_MCS17,
	ODM_MGN_MCS18,
	ODM_MGN_MCS19,
	ODM_MGN_MCS20,
	ODM_MGN_MCS21,
	ODM_MGN_MCS22,
	ODM_MGN_MCS23,
	ODM_MGN_MCS24	= 0x98,
	ODM_MGN_MCS25,
	ODM_MGN_MCS26,
	ODM_MGN_MCS27,
	ODM_MGN_MCS28,
	ODM_MGN_MCS29,
	ODM_MGN_MCS30,
	ODM_MGN_MCS31,
	ODM_MGN_VHT1SS_MCS0	= 0xa0,
	ODM_MGN_VHT1SS_MCS1,
	ODM_MGN_VHT1SS_MCS2,
	ODM_MGN_VHT1SS_MCS3,
	ODM_MGN_VHT1SS_MCS4,
	ODM_MGN_VHT1SS_MCS5,
	ODM_MGN_VHT1SS_MCS6,
	ODM_MGN_VHT1SS_MCS7,
	ODM_MGN_VHT1SS_MCS8,
	ODM_MGN_VHT1SS_MCS9,
	ODM_MGN_VHT2SS_MCS0	= 0xaa,
	ODM_MGN_VHT2SS_MCS1	= 0xab,
	ODM_MGN_VHT2SS_MCS2,
	ODM_MGN_VHT2SS_MCS3,
	ODM_MGN_VHT2SS_MCS4,
	ODM_MGN_VHT2SS_MCS5	= 0xaf,
	ODM_MGN_VHT2SS_MCS6	= 0xb0,
	ODM_MGN_VHT2SS_MCS7,
	ODM_MGN_VHT2SS_MCS8,
	ODM_MGN_VHT2SS_MCS9	= 0xb3,
	ODM_MGN_VHT3SS_MCS0	= 0xb4,
	ODM_MGN_VHT3SS_MCS1,
	ODM_MGN_VHT3SS_MCS2,
	ODM_MGN_VHT3SS_MCS3,
	ODM_MGN_VHT3SS_MCS4,
	ODM_MGN_VHT3SS_MCS5,
	ODM_MGN_VHT3SS_MCS6,
	ODM_MGN_VHT3SS_MCS7	= 0xbb,
	ODM_MGN_VHT3SS_MCS8	= 0xbc,
	ODM_MGN_VHT3SS_MCS9	= 0xbd,
	ODM_MGN_VHT4SS_MCS0	= 0xbe,
	ODM_MGN_VHT4SS_MCS1,
	ODM_MGN_VHT4SS_MCS2,
	ODM_MGN_VHT4SS_MCS3,
	ODM_MGN_VHT4SS_MCS4,
	ODM_MGN_VHT4SS_MCS5,
	ODM_MGN_VHT4SS_MCS6,
	ODM_MGN_VHT4SS_MCS7,
	ODM_MGN_VHT4SS_MCS8,
	ODM_MGN_VHT4SS_MCS9	= 0xc7,
	ODM_MGN_UNKNOWN
};

#define	ODM_MGN_MCS0_SG		0xc0
#define	ODM_MGN_MCS1_SG		0xc1
#define	ODM_MGN_MCS2_SG		0xc2
#define	ODM_MGN_MCS3_SG		0xc3
#define	ODM_MGN_MCS4_SG		0xc4
#define	ODM_MGN_MCS5_SG		0xc5
#define	ODM_MGN_MCS6_SG		0xc6
#define	ODM_MGN_MCS7_SG		0xc7
#define	ODM_MGN_MCS8_SG		0xc8
#define	ODM_MGN_MCS9_SG		0xc9
#define	ODM_MGN_MCS10_SG		0xca
#define	ODM_MGN_MCS11_SG		0xcb
#define	ODM_MGN_MCS12_SG		0xcc
#define	ODM_MGN_MCS13_SG		0xcd
#define	ODM_MGN_MCS14_SG		0xce
#define	ODM_MGN_MCS15_SG		0xcf

/* -----DESC rate--------------------------------- */

#define ODM_RATEMCS15_SG		0x1c
#define ODM_RATEMCS32			0x20


enum phydm_ctrl_info_rate {
	ODM_RATE1M			= 0x00,
	ODM_RATE2M			= 0x01,
	ODM_RATE5_5M			= 0x02,
	ODM_RATE11M			= 0x03,
/* OFDM Rates, TxHT = 0 */
	ODM_RATE6M			= 0x04,
	ODM_RATE9M			= 0x05,
	ODM_RATE12M			= 0x06,
	ODM_RATE18M			= 0x07,
	ODM_RATE24M			= 0x08,
	ODM_RATE36M			= 0x09,
	ODM_RATE48M			= 0x0A,
	ODM_RATE54M			= 0x0B,
/* MCS Rates, TxHT = 1 */
	ODM_RATEMCS0			= 0x0C,
	ODM_RATEMCS1			= 0x0D,
	ODM_RATEMCS2			= 0x0E,
	ODM_RATEMCS3			= 0x0F,
	ODM_RATEMCS4			= 0x10,
	ODM_RATEMCS5			= 0x11,
	ODM_RATEMCS6			= 0x12,
	ODM_RATEMCS7			= 0x13,
	ODM_RATEMCS8			= 0x14,
	ODM_RATEMCS9			= 0x15,
	ODM_RATEMCS10			= 0x16,
	ODM_RATEMCS11			= 0x17,
	ODM_RATEMCS12			= 0x18,
	ODM_RATEMCS13			= 0x19,
	ODM_RATEMCS14			= 0x1A,
	ODM_RATEMCS15			= 0x1B,
	ODM_RATEMCS16			= 0x1C,
	ODM_RATEMCS17			= 0x1D,
	ODM_RATEMCS18			= 0x1E,
	ODM_RATEMCS19			= 0x1F,
	ODM_RATEMCS20			= 0x20,
	ODM_RATEMCS21			= 0x21,
	ODM_RATEMCS22			= 0x22,
	ODM_RATEMCS23			= 0x23,
	ODM_RATEMCS24			= 0x24,
	ODM_RATEMCS25			= 0x25,
	ODM_RATEMCS26			= 0x26,
	ODM_RATEMCS27			= 0x27,
	ODM_RATEMCS28			= 0x28,
	ODM_RATEMCS29			= 0x29,
	ODM_RATEMCS30			= 0x2A,
	ODM_RATEMCS31			= 0x2B,
	ODM_RATEVHTSS1MCS0	= 0x2C,
	ODM_RATEVHTSS1MCS1	= 0x2D,
	ODM_RATEVHTSS1MCS2	= 0x2E,
	ODM_RATEVHTSS1MCS3	= 0x2F,
	ODM_RATEVHTSS1MCS4	= 0x30,
	ODM_RATEVHTSS1MCS5	= 0x31,
	ODM_RATEVHTSS1MCS6	= 0x32,
	ODM_RATEVHTSS1MCS7	= 0x33,
	ODM_RATEVHTSS1MCS8	= 0x34,
	ODM_RATEVHTSS1MCS9	= 0x35,
	ODM_RATEVHTSS2MCS0	= 0x36,
	ODM_RATEVHTSS2MCS1	= 0x37,
	ODM_RATEVHTSS2MCS2	= 0x38,
	ODM_RATEVHTSS2MCS3	= 0x39,
	ODM_RATEVHTSS2MCS4	= 0x3A,
	ODM_RATEVHTSS2MCS5	= 0x3B,
	ODM_RATEVHTSS2MCS6	= 0x3C,
	ODM_RATEVHTSS2MCS7	= 0x3D,
	ODM_RATEVHTSS2MCS8	= 0x3E,
	ODM_RATEVHTSS2MCS9	= 0x3F,
	ODM_RATEVHTSS3MCS0	= 0x40,
	ODM_RATEVHTSS3MCS1	= 0x41,
	ODM_RATEVHTSS3MCS2	= 0x42,
	ODM_RATEVHTSS3MCS3	= 0x43,
	ODM_RATEVHTSS3MCS4	= 0x44,
	ODM_RATEVHTSS3MCS5	= 0x45,
	ODM_RATEVHTSS3MCS6	= 0x46,
	ODM_RATEVHTSS3MCS7	= 0x47,
	ODM_RATEVHTSS3MCS8	= 0x48,
	ODM_RATEVHTSS3MCS9	= 0x49,
	ODM_RATEVHTSS4MCS0	= 0x4A,
	ODM_RATEVHTSS4MCS1	= 0x4B,
	ODM_RATEVHTSS4MCS2	= 0x4C,
	ODM_RATEVHTSS4MCS3	= 0x4D,
	ODM_RATEVHTSS4MCS4	= 0x4E,
	ODM_RATEVHTSS4MCS5	= 0x4F,
	ODM_RATEVHTSS4MCS6	= 0x50,
	ODM_RATEVHTSS4MCS7	= 0x51,
	ODM_RATEVHTSS4MCS8	= 0x52,
	ODM_RATEVHTSS4MCS9	= 0x53,
};

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#define ODM_NUM_RATE_IDX (ODM_RATEVHTSS4MCS9+1)
#else
	#if (RTL8192E_SUPPORT == 1) || (RTL8197F_SUPPORT == 1)
		#define ODM_NUM_RATE_IDX (ODM_RATEMCS15+1)
	#elif (RTL8723B_SUPPORT == 1) || (RTL8188E_SUPPORT == 1) || (RTL8188F_SUPPORT == 1)
		#define ODM_NUM_RATE_IDX (ODM_RATEMCS7+1)
	#elif (RTL8821A_SUPPORT == 1) || (RTL8881A_SUPPORT == 1)
		#define ODM_NUM_RATE_IDX (ODM_RATEVHTSS1MCS9+1)
	#elif (RTL8812A_SUPPORT == 1)
		#define ODM_NUM_RATE_IDX (ODM_RATEVHTSS2MCS9+1)
	#elif (RTL8814A_SUPPORT == 1)
		#define ODM_NUM_RATE_IDX (ODM_RATEVHTSS3MCS9+1)
	#else
		#define ODM_NUM_RATE_IDX (ODM_RATEVHTSS4MCS9+1)
	#endif
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#define CONFIG_SFW_SUPPORTED
#endif

/* 1 ============================================================
 * 1  enumeration
 * 1 ============================================================ */


/*	ODM_CMNINFO_INTERFACE */
enum odm_interface {
	ODM_ITRF_PCIE	=	0x1,
	ODM_ITRF_USB	=	0x2,
	ODM_ITRF_SDIO	=	0x4,
	ODM_ITRF_ALL	=	0x7,
};


/*========[Run time IC flag] ===============================================================================]*/

enum phydm_ic {
	ODM_RTL8188E	=	BIT(0),
	ODM_RTL8812		=	BIT(1),
	ODM_RTL8821		=	BIT(2),
	ODM_RTL8192E	=	BIT(3),
	ODM_RTL8723B	=	BIT(4),
	ODM_RTL8814A	=	BIT(5),
	ODM_RTL8881A	=	BIT(6),
	ODM_RTL8822B	=	BIT(7),
	ODM_RTL8703B	=	BIT(8),
	ODM_RTL8195A	=	BIT(9),
	ODM_RTL8188F	=	BIT(10),
	ODM_RTL8723D	=	BIT(11),
	ODM_RTL8197F	=	BIT(12),
	ODM_RTL8821C	=	BIT(13),
	ODM_RTL8814B	=	BIT(14),
	ODM_RTL8198F	=	BIT(15),
	ODM_RTL8710B	=	BIT(16),
	ODM_RTL8192F	=	BIT(17),
	ODM_RTL8822C	=	BIT(18),
	ODM_RTL8195B	=	BIT(19)
};

#define ODM_IC_N_1SS	(ODM_RTL8188E | ODM_RTL8188F | ODM_RTL8723B | ODM_RTL8703B | ODM_RTL8723D | ODM_RTL8195A | ODM_RTL8710B)
#define ODM_IC_N_2SS	(ODM_RTL8192E | ODM_RTL8197F | ODM_RTL8192F)
#define ODM_IC_N_3SS	0
#define ODM_IC_N_4SS	(ODM_RTL8198F)

#define ODM_IC_AC_1SS	(ODM_RTL8881A | ODM_RTL8821 | ODM_RTL8821C | ODM_RTL8195B)
#define ODM_IC_AC_2SS	(ODM_RTL8812 | ODM_RTL8822B | ODM_RTL8822C)
#define ODM_IC_AC_3SS	0
#define ODM_IC_AC_4SS	(ODM_RTL8814A | ODM_RTL8814B)

/*====the following macro DO NOT need to update when adding a new IC======= */
#define ODM_IC_1SS	(ODM_IC_N_1SS | ODM_IC_AC_1SS)
#define ODM_IC_2SS	(ODM_IC_N_2SS | ODM_IC_AC_2SS)
#define ODM_IC_3SS	(ODM_IC_N_3SS | ODM_IC_AC_3SS)
#define ODM_IC_4SS	(ODM_IC_N_4SS | ODM_IC_AC_4SS)

#define PHYDM_IC_ABOVE_1SS	(ODM_IC_1SS | ODM_IC_2SS | ODM_IC_3SS | ODM_IC_4SS)
#define PHYDM_IC_ABOVE_2SS	(ODM_IC_2SS | ODM_IC_3SS | ODM_IC_4SS)
#define PHYDM_IC_ABOVE_3SS	(ODM_IC_3SS | ODM_IC_4SS)
#define PHYDM_IC_ABOVE_4SS	ODM_IC_4SS

#define ODM_IC_11N_SERIES		(ODM_IC_N_1SS | ODM_IC_N_2SS | ODM_IC_N_3SS | ODM_IC_N_4SS)
#define ODM_IC_11AC_SERIES		(ODM_IC_AC_1SS | ODM_IC_AC_2SS | ODM_IC_AC_3SS | ODM_IC_AC_4SS)
/*====================================================*/

#define ODM_IC_11AC_1_SERIES			(ODM_RTL8812 | ODM_RTL8821 | ODM_RTL8881A)
#define ODM_IC_11AC_2_SERIES			(ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8821C | ODM_RTL8195B)
/*[EDCCA]*/
#define ODM_IC_11N_GAIN_IDX_EDCCA	(ODM_RTL8195A | ODM_RTL8703B | ODM_RTL8188F | ODM_RTL8723D | ODM_RTL8197F | ODM_RTL8710B)
#define ODM_IC_11AC_GAIN_IDX_EDCCA	(ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8821C)
#define ODM_IC_GAIN_IDX_EDCCA			(ODM_IC_11N_GAIN_IDX_EDCCA | ODM_IC_11AC_GAIN_IDX_EDCCA)
/*[Phy status type]*/
#define PHYSTS_2ND_TYPE_IC				(ODM_RTL8197F | ODM_RTL8822B | ODM_RTL8723D | ODM_RTL8821C | ODM_RTL8710B | ODM_RTL8195B)
#define PHYSTS_3RD_TYPE_IC				(ODM_RTL8198F | ODM_RTL8814B)
/*[FW Type]*/
#define PHYDM_IC_8051_SERIES			(ODM_RTL8881A | ODM_RTL8812 | ODM_RTL8821 | ODM_RTL8192E | ODM_RTL8723B | ODM_RTL8703B | ODM_RTL8188F)
#define PHYDM_IC_3081_SERIES			(ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8197F | ODM_RTL8821C | ODM_RTL8195B | ODM_RTL8198F)
/*[LA mode]*/
#define PHYDM_IC_SUPPORT_LA_MODE		(ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8197F | ODM_RTL8821C | ODM_RTL8195B | ODM_RTL8198F)
/*[BF]*/
#define ODM_IC_TXBF_SUPPORT			(ODM_RTL8192E | ODM_RTL8812 | ODM_RTL8821 | ODM_RTL8814A | ODM_RTL8881A | ODM_RTL8822B | ODM_RTL8197F | ODM_RTL8821C | ODM_RTL8195B | ODM_RTL8198F)
#define PHYDM_IC_SUPPORT_MU_BFEE		(ODM_RTL8822B | ODM_RTL8821C | ODM_RTL8814B | ODM_RTL8195B | ODM_RTL8198F)
#define PHYDM_IC_SUPPORT_MU_BFER		(ODM_RTL8822B | ODM_RTL8814B | ODM_RTL8198F)


/*========[Compile time IC flag] ===============================================================================]*/
/*========[AC/N Support] ===========================*/
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)

	#ifdef RTK_AC_SUPPORT
	#define ODM_IC_11AC_SERIES_SUPPORT		1
	#else
	#define ODM_IC_11AC_SERIES_SUPPORT		0
	#endif

	#define ODM_IC_11N_SERIES_SUPPORT			1

#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)

	#define ODM_IC_11AC_SERIES_SUPPORT		1
	#define ODM_IC_11N_SERIES_SUPPORT			1

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE) && defined(DM_ODM_CE_MAC80211)

	#define ODM_IC_11AC_SERIES_SUPPORT		1
	#define ODM_IC_11N_SERIES_SUPPORT			1

#else /*ODM_CE*/

	#if ((RTL8188E_SUPPORT == 1) || \
	(RTL8723B_SUPPORT == 1) || (RTL8192E_SUPPORT == 1) || (RTL8195A_SUPPORT == 1) || (RTL8703B_SUPPORT == 1) || \
	(RTL8188F_SUPPORT == 1) || (RTL8723D_SUPPORT == 1) || (RTL8197F_SUPPORT == 1) || (RTL8710B_SUPPORT == 1))
		#define ODM_IC_11N_SERIES_SUPPORT			1
		#define ODM_IC_11AC_SERIES_SUPPORT		0
	#else
		#define ODM_IC_11N_SERIES_SUPPORT			0
		#define ODM_IC_11AC_SERIES_SUPPORT		1
	#endif
#endif

/*===IC SS Compile Flag, prepare for code size reduction==============*/
#if ((RTL8188E_SUPPORT == 1) || (RTL8188F_SUPPORT == 1) || (RTL8723B_SUPPORT == 1) || (RTL8703B_SUPPORT == 1) ||\
	(RTL8723D_SUPPORT == 1) || (RTL8881A_SUPPORT == 1) || (RTL8821A_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) ||\
	(RTL8195A_SUPPORT == 1) || (RTL8710B_SUPPORT == 1) || (RTL8195B_SUPPORT == 1))

	#define PHYDM_COMPILE_IC_1SS
#endif

#if ((RTL8192E_SUPPORT == 1) || (RTL8197F_SUPPORT == 1) || (RTL8812A_SUPPORT == 1) || (RTL8822B_SUPPORT == 1))
	#define PHYDM_COMPILE_IC_2SS
#endif

/*#define PHYDM_COMPILE_IC_3SS*/

#if ((RTL8814B_SUPPORT == 1) || (RTL8814A_SUPPORT == 1) || (RTL8198F_SUPPORT == 1))
	#define PHYDM_COMPILE_IC_4SS
#endif

/*==[ABOVE N-SS COMPILE FLAG]=============================*/
#if (defined(PHYDM_COMPILE_IC_1SS) || defined(PHYDM_COMPILE_IC_2SS) || defined(PHYDM_COMPILE_IC_3SS) || defined(PHYDM_COMPILE_IC_4SS))
	#define PHYDM_COMPILE_ABOVE_1SS
#endif

#if (defined(PHYDM_COMPILE_IC_2SS) || defined(PHYDM_COMPILE_IC_3SS) || defined(PHYDM_COMPILE_IC_4SS))
	#define PHYDM_COMPILE_ABOVE_2SS
#endif

#if (defined(PHYDM_COMPILE_IC_3SS) || defined(PHYDM_COMPILE_IC_4SS))
	#define PHYDM_COMPILE_ABOVE_3SS
#endif

#if (defined(PHYDM_COMPILE_IC_4SS))
	#define PHYDM_COMPILE_ABOVE_4SS
#endif

/*========[New Phy-Status Support] =========================================================================]*/
#if (RTL8824B_SUPPORT == 1)
	#define CONFIG_PHYSTS_3RD_TYPE		1
#else
	#define CONFIG_PHYSTS_3RD_TYPE		0
#endif

#if ((RTL8197F_SUPPORT == 1) || (RTL8723D_SUPPORT == 1) || (RTL8822B_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) || (RTL8710B_SUPPORT == 1) )
	#define ODM_PHY_STATUS_NEW_TYPE_SUPPORT			1
#else
	#define ODM_PHY_STATUS_NEW_TYPE_SUPPORT			0
#endif

/*==================================================================================================]*/

#if ((RTL8822B_SUPPORT == 1) || (RTL8197F_SUPPORT == 1) || (RTL8821C_SUPPORT == 1))
#define PHYDM_COMMON_API_SUPPORT
#endif


#define	CCK_RATE_NUM		4
#define	OFDM_RATE_NUM	8

#define	LEGACY_RATE_NUM	12

#define	HT_RATE_NUM_4SS		32
#define	VHT_RATE_NUM_4SS		40

#define	HT_RATE_NUM_3SS		24
#define	VHT_RATE_NUM_3SS		30

#define	HT_RATE_NUM_2SS		16
#define	VHT_RATE_NUM_2SS		20

#define	HT_RATE_NUM_1SS		8
#define	VHT_RATE_NUM_1SS		10

#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	#define	HT_RATE_NUM		HT_RATE_NUM_4SS
	#define	VHT_RATE_NUM		VHT_RATE_NUM_4SS
#elif (defined(PHYDM_COMPILE_ABOVE_3SS))
	#define	HT_RATE_NUM		HT_RATE_NUM_3SS
	#define	VHT_RATE_NUM		VHT_RATE_NUM_3SS
#elif (defined(PHYDM_COMPILE_ABOVE_2SS))
	#define	HT_RATE_NUM		HT_RATE_NUM_2SS
	#define	VHT_RATE_NUM		VHT_RATE_NUM_2SS
#else
	#define	HT_RATE_NUM		HT_RATE_NUM_1SS
	#define	VHT_RATE_NUM		VHT_RATE_NUM_1SS
#endif

#define	LOW_BW_RATE_NUM	VHT_RATE_NUM


/* ODM_CMNINFO_CUT_VER */
enum odm_cut_version {
	ODM_CUT_A		=	0,
	ODM_CUT_B		=	1,
	ODM_CUT_C		=	2,
	ODM_CUT_D		=	3,
	ODM_CUT_E		=	4,
	ODM_CUT_F		=	5,
	ODM_CUT_G		=	6,
	ODM_CUT_H		=	7,
	ODM_CUT_I		=	8,
	ODM_CUT_J		=	9,
	ODM_CUT_K		=	10,
	ODM_CUT_TEST	=	15,
};

/* ODM_CMNINFO_FAB_VER */
enum odm_fab {
	ODM_TSMC	=	0,
	ODM_UMC	=	1,
};

/* ODM_CMNINFO_OP_MODE */
enum odm_operation_mode {
	ODM_NO_LINK		= BIT(0),
	ODM_LINK			= BIT(1),
	ODM_SCAN			= BIT(2),
	ODM_POWERSAVE	= BIT(3),
	ODM_AP_MODE		= BIT(4),
	ODM_CLIENT_MODE	= BIT(5),
	ODM_AD_HOC		= BIT(6),
	ODM_WIFI_DIRECT	= BIT(7),
	ODM_WIFI_DISPLAY	= BIT(8),
};

/* ODM_CMNINFO_WM_MODE */
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE))
enum odm_wireless_mode {
	ODM_WM_UNKNOW	= 0x0,
	ODM_WM_B			= BIT(0),
	ODM_WM_G			= BIT(1),
	ODM_WM_A			= BIT(2),
	ODM_WM_N24G		= BIT(3),
	ODM_WM_N5G		= BIT(4),
	ODM_WM_AUTO		= BIT(5),
	ODM_WM_AC		= BIT(6),
};
#else
enum odm_wireless_mode {
	ODM_WM_UNKNOWN	= 0x00,/*0x0*/
	ODM_WM_A			= BIT(0), /* 0x1*/
	ODM_WM_B			= BIT(1), /* 0x2*/
	ODM_WM_G			= BIT(2),/* 0x4*/
	ODM_WM_AUTO		= BIT(3),/* 0x8*/
	ODM_WM_N24G		= BIT(4),/* 0x10*/
	ODM_WM_N5G		= BIT(5),/* 0x20*/
	ODM_WM_AC_5G		= BIT(6),/* 0x40*/
	ODM_WM_AC_24G	= BIT(7),/* 0x80*/
	ODM_WM_AC_ONLY	= BIT(8),/* 0x100*/
	ODM_WM_MAX		= BIT(11)/* 0x800*/

};
#endif

/* ODM_CMNINFO_BAND */
enum odm_band_type {
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	ODM_BAND_2_4G	= BIT(0),
	ODM_BAND_5G		= BIT(1),
#else
	ODM_BAND_2_4G = 0,
	ODM_BAND_5G,
	ODM_BAND_ON_BOTH,
	ODM_BANDMAX
#endif
};


/* ODM_CMNINFO_SEC_CHNL_OFFSET */
enum phydm_sec_chnl_offset {
	PHYDM_DONT_CARE	= 0,
	PHYDM_BELOW		= 1,
	PHYDM_ABOVE		= 2
};

/* ODM_CMNINFO_SEC_MODE */
enum odm_security {
	ODM_SEC_OPEN			= 0,
	ODM_SEC_WEP40		= 1,
	ODM_SEC_TKIP			= 2,
	ODM_SEC_RESERVE		= 3,
	ODM_SEC_AESCCMP		= 4,
	ODM_SEC_WEP104		= 5,
	ODM_WEP_WPA_MIXED    = 6, /* WEP + WPA */
	ODM_SEC_SMS4			= 7,
};

/* ODM_CMNINFO_CHNL */

/* ODM_CMNINFO_BOARD_TYPE */
enum odm_board_type {
	ODM_BOARD_DEFAULT 	= 0,	  /* The DEFAULT case. */
	ODM_BOARD_MINICARD  = BIT(0), /* 0 = non-mini card, 1= mini card. */
	ODM_BOARD_SLIM      = BIT(1), /* 0 = non-slim card, 1 = slim card */
	ODM_BOARD_BT        = BIT(2), /* 0 = without BT card, 1 = with BT */
	ODM_BOARD_EXT_PA    = BIT(3), /* 0 = no 2G ext-PA, 1 = existing 2G ext-PA */
	ODM_BOARD_EXT_LNA   = BIT(4), /* 0 = no 2G ext-LNA, 1 = existing 2G ext-LNA */
	ODM_BOARD_EXT_TRSW  = BIT(5), /* 0 = no ext-TRSW, 1 = existing ext-TRSW */
	ODM_BOARD_EXT_PA_5G	= BIT(6), /* 0 = no 5G ext-PA, 1 = existing 5G ext-PA */
	ODM_BOARD_EXT_LNA_5G = BIT(7), /* 0 = no 5G ext-LNA, 1 = existing 5G ext-LNA */
};

enum odm_package_type {
	ODM_PACKAGE_DEFAULT	 = 0,
	ODM_PACKAGE_QFN68        = BIT(0),
	ODM_PACKAGE_TFBGA90      = BIT(1),
	ODM_PACKAGE_TFBGA79      = BIT(2),
};

enum odm_type_gpa {
	TYPE_GPA0 = 0x0000,
	TYPE_GPA1 = 0x0055,
	TYPE_GPA2 = 0x00AA,
	TYPE_GPA3 = 0x00FF,
	TYPE_GPA4 = 0x5500,
	TYPE_GPA5 = 0x5555,
	TYPE_GPA6 = 0x55AA,
	TYPE_GPA7 = 0x55FF,
	TYPE_GPA8 = 0xAA00,
	TYPE_GPA9 = 0xAA55,
	TYPE_GPA10 = 0xAAAA,
	TYPE_GPA11 = 0xAAFF,
	TYPE_GPA12 = 0xFF00,
	TYPE_GPA13 = 0xFF55,
	TYPE_GPA14 = 0xFFAA,
	TYPE_GPA15 = 0xFFFF,
};

enum odm_type_apa {
	TYPE_APA0 = 0x0000,
	TYPE_APA1 = 0x0055,
	TYPE_APA2 = 0x00AA,
	TYPE_APA3 = 0x00FF,
	TYPE_APA4 = 0x5500,
	TYPE_APA5 = 0x5555,
	TYPE_APA6 = 0x55AA,
	TYPE_APA7 = 0x55FF,
	TYPE_APA8 = 0xAA00,
	TYPE_APA9 = 0xAA55,
	TYPE_APA10 = 0xAAAA,
	TYPE_APA11 = 0xAAFF,
	TYPE_APA12 = 0xFF00,
	TYPE_APA13 = 0xFF55,
	TYPE_APA14 = 0xFFAA,
	TYPE_APA15 = 0xFFFF,
};

enum odm_type_glna {
	TYPE_GLNA0 = 0x0000,
	TYPE_GLNA1 = 0x0055,
	TYPE_GLNA2 = 0x00AA,
	TYPE_GLNA3 = 0x00FF,
	TYPE_GLNA4 = 0x5500,
	TYPE_GLNA5 = 0x5555,
	TYPE_GLNA6 = 0x55AA,
	TYPE_GLNA7 = 0x55FF,
	TYPE_GLNA8 = 0xAA00,
	TYPE_GLNA9 = 0xAA55,
	TYPE_GLNA10 = 0xAAAA,
	TYPE_GLNA11 = 0xAAFF,
	TYPE_GLNA12 = 0xFF00,
	TYPE_GLNA13 = 0xFF55,
	TYPE_GLNA14 = 0xFFAA,
	TYPE_GLNA15 = 0xFFFF,
};

enum odm_type_alna {
	TYPE_ALNA0 = 0x0000,
	TYPE_ALNA1 = 0x0055,
	TYPE_ALNA2 = 0x00AA,
	TYPE_ALNA3 = 0x00FF,
	TYPE_ALNA4 = 0x5500,
	TYPE_ALNA5 = 0x5555,
	TYPE_ALNA6 = 0x55AA,
	TYPE_ALNA7 = 0x55FF,
	TYPE_ALNA8 = 0xAA00,
	TYPE_ALNA9 = 0xAA55,
	TYPE_ALNA10 = 0xAAAA,
	TYPE_ALNA11 = 0xAAFF,
	TYPE_ALNA12 = 0xFF00,
	TYPE_ALNA13 = 0xFF55,
	TYPE_ALNA14 = 0xFFAA,
	TYPE_ALNA15 = 0xFFFF,
};

#define	PAUSE_FAIL		0
#define	PAUSE_SUCCESS	1

enum odm_parameter_init {
	ODM_PRE_SETTING = 0,
	ODM_POST_SETTING = 1,
	ODM_INIT_FW_SETTING
};


enum phydm_pause_type {
	PHYDM_PAUSE = 1,			/*Pause & Set new value*/
	PHYDM_PAUSE_NO_SET = 2,	/*Pause & Stay in current value*/
	PHYDM_RESUME = 3
};

enum phydm_pause_level {
	PHYDM_PAUSE_RELEASE = -1,
	PHYDM_PAUSE_LEVEL_0 = 0,	/* Low Priority function */
	PHYDM_PAUSE_LEVEL_1 = 1,	/* Middle Priority function */
	PHYDM_PAUSE_LEVEL_2 = 2,	/* High priority function (ex: Check hang function) */
	PHYDM_PAUSE_LEVEL_3 = 3,	/* Debug function (the highest priority) */
	PHYDM_PAUSE_MAX_NUM = 4
};

enum phydm_dis_hw_fun {
	HW_FUN_DIS = 0,		/*Disable a cetain HW function & backup the original value*/
	HW_FUN_RESUME = 1		/*Revert */
};

#endif
