

//#define FUNC_ZOOM_ALL
//#define CAN_PARSING_TIMER		//150000?��,6ms ok
// 2?�� : 11ms
// 5?�� : 9ms

// FF FF FF FF FF FF FF FF
// 00 00 00 00 00 00 00 00
// AA AA AA AA AA AA AA AA
// 55 55 55 55 55 55 55 55
// FF FF AA AA 55 55 00 00

//#define TCMD    	0x1850FBCE 		//chpark pedal calibration.//chpark.09.13
#define XO_TMODE_REQ   0x121200AC  	//chpark.24.05.27
#define XO_TMODE_ACK   0x131300AC  	//chpark.24.05.27

#define MAIN_CON    0x18FEDAE4  	//chpark
#define AUX_PIO     0x1C5026E4		//chpark.

#define SUB_CON    	0x18FEDAE3  	//JPHong

#define PHONE_TIME 		0x78FFFFFF  	//JPHong 20204-10-09
#define LONG_KEY_TIME 	0x78FFFFFF  	//JPHong 20204-10-09

/*
Brightness 	: 0 	: -64 ~63 	(Reset : 0x00)
Contrast 	: 100 	: 0-127 	(Reset : 0x40)
Saturation 	: 64 	: 0-127 	(Reset : 0x40)
Hue 		: -10 	: -90~90 	(Reset : 0x00)
Sharpness 	: 0 	: 0~31 		(Reset : 0x00)
*/


#if 0	// Chip Default
	//Day
	#define DEFAULT_CAM_BR		0		// 2024-05-16		0	//2024-04-25 JPHong
	#define DEFAULT_CAM_CR		0x40	// 2024-05-16		100	//2024-04-25 JPHong
	#define DEFAULT_CAM_SR		0x40	// 2024-05-16		64	//2024-04-25 JPHong
	#define DEFAULT_CAM_HUE		0		// 2024-05-16		-10	//2024-04-25 JPHong
	#define DEFAULT_CAM_SP		0		// 2024-05-16		0	//2024-04-25 JPHong
	//Night
	#define DEFAULT_CAM_BR_N	0x15
	#define DEFAULT_CAM_CR_N	0x40
	#define DEFAULT_CAM_SR_N	0x40
	#define DEFAULT_CAM_HUE_N	0
	#define DEFAULT_CAM_SP_N	0
#elif 1		// 2024-10-10
	//Day
	#define DEFAULT_CAM_BR		5
	#define DEFAULT_CAM_CR		65
	#define DEFAULT_CAM_SR		124
	#define DEFAULT_CAM_HUE		17
	#define DEFAULT_CAM_SP		26
	//Night
	#define DEFAULT_CAM_BR_N	17
	#define DEFAULT_CAM_CR_N	72
	#define DEFAULT_CAM_SR_N	127
	#define DEFAULT_CAM_HUE_N	17
	#define DEFAULT_CAM_SP_N	20
#endif

extern uint8_t OPI2_1Byte_23bit;	//Rotation  JPHong 2024-10-14
extern uint8_t OPI2_0Byte_01bit;	//270 camera //chpark
extern uint8_t tOPI2_0Byte_01bit;	//Pre 270 camera //JPHong
extern uint8_t OPI2_0Byte_45bit;    //LED PACKAGE //chpark 
extern uint8_t OPI0Byte23bit;//chpark
extern uint8_t OPI6Byte01bit;//chpark
extern uint8_t OPI6Byte23bit;//chpark
extern uint8_t OPI7Byte23bit;//chpark
extern uint8_t OPI7Byte67bit;//chpark

//===============================
//chpark.
extern uint8_t gMB_Unit; //0:ST 1: US
extern uint8_t gMB_Language; //0:US  1:German 2:French 3:Italian 4:Spain


typedef enum {
	PEDAL_INIT	            = 0x10,
    PEDAL_STEP2_OPEN		= 0x11,
    PEDAL_STEP3_OPEN        = 0x12,

    BLADE_INIT              = 0x15,
    BLADE_STEP2_OPEN		= 0x16,
    BLADE_STEP3_OPEN		= 0x17
    
} MenuB_MSG_DETAIL;


typedef enum {
	EVENT_CUSTOM20_MB400 = ITU_EVENT_CUSTOM+20, 
    EVENT_CUSTOM21_MB400
} MenuB_MSG;
//===============================

#define XO_MICOM_SOFT_REQ	0x78EA01E4	// JPHong 2024-04-19
#define XO_MICOM_FIRM_UP	0x78EA02E4	// JPHong 2024-04-19
#define XO_MICOM_FIRM_DATA	0x80000000	// JPHong 2024-04-19

#define HEX_START_DATA	0x3A

#define MICOM_FIRM_MAIN_READY 	0x01
#define MICOM_FIRM_UPFIRM_READY 0x02
#define MICOM_FIRM_WRITE 		0x03
#define MICOM_FIRM_WRITE_END	0x04

#define MICOM_FIRM_MODE_ERROR 		0x11
#define MICOM_READY_ERROR 			0x12
#define MICOM_FIRM_SIZE_ERROR 		0x13
#define MICOM_FIRM_HEAD_ERROR 		0x14
#define MICOM_FIRM_CHECKSUM_ERROR 	0x15
#define MICOM_FIRM_WRITE_ERROR 		0x16




#define TCI 	0x1850F1E4
#define TS1 	0x185303CE
#define TS2 	0x185304CE
#define SAI 	0x185305E4
#define OPI3 	0x185039E4


#define AUI1 	0x18510E8C


#define CLM1 	0x14FF00E4
#define CLM2 	0x14500EE4
#define CLM3 	0x14500CE4
#define CLM4 	0x14500AE4
#define AG_I 	0x1C504AE4
#define CO 		0x185050E4			  // 100ms

#define JOG2ECU 0x18FF603B

#define ACDM	0x18FFA119

#define EI   	0x185009E4
#define DD   	0x18FEFCE4
#define HM   	0x185001E4
#define TM1  	0x185002E4
#define TM2  	0x185003E4
#define TM3  	0x185008E4
#define EEC1 	0x0CF00400
#define EEC2 	0x0CF00300
#define EEC3 	0x0CFEDF00
#define SI	 	0x18500DE4
#define MI	 	0x185000E4
#define AUX_IO2	0x18FF32CF
#define ET1		0x18FEEE00
#define ET2		0x0CFEA400
#define AT1T1I_	0x18FE5600
#define VEP1	0x18FEF7E4
#define AT1S	0x18FD7B00
#define OPI		0x185012E4
#define OPI2	0x185038E4
#define AUX_UL	0x1C501FE4
#define AUX_PUL	0x1C5025E4

#define AUX_UL5	0x1C5019E4
#define AUX_UL4	0x1C5018E4

#define SOFT_ID		0x18FEDACE		// JPHong 2024-02-07 v12
#define SOFT_REQ	0x18EACE00		// 0x18EACEE4
/*
TTMS 16450
CAN[SOFT]의 요청과 응답은 다음과 같이 하십시오.
SOFT_rquest (요청) : 0x18EACE##, ##는 모든 값에 해당합니다.
SOFT_response (응답) : 0x18FEDACE
*/


#define AUX_PC1	0x185016CE
#define AUX_PC2	0x185017CE

#define FNI1		0x1850E8E4		// add jhlee 240122
#define FNS1		0x1850E9CE

#define UWH			0x1850EDE4


#define LOCKSTAT 0x1C5085E4 


#define DMM 	0x18FF01E4

#define DM1aY 	0x18FECA00
#define DM1bY 	0x18FECA3D
#define DM1a 	0x1CFECA00
#define DM1b 	0x1CFECA3D

#define TP_CMaY 	0x18ECFF00
#define TP_CMbY 	0x18ECFF3D
#define TP_DTaY 	0x18EBFF00
#define TP_DTbY 	0x18EBFF3D
#define TP_CMa 		0x1CECFF00
#define TP_CMb 		0x1CECFF3D
#define TP_DTa 		0x1CEBFF00
#define TP_DTb 		0x1CEBFF3D

#define DM2aY 	0x18FECB00
#define DM2bY 	0x18FECB3D
#define DM2a 	0x1CFECB00
#define DM2b 	0x1CFECB3D


#if 0
	#define DM14aY 	0x18D90000
	#define DM14bY 	0x18D9003D
	#define DM14a 	0x1CD90000
	#define DM14b 	0x1CD9003D
	
	#define DM15aY 	0x18D80000
	#define DM15bY 	0x18D8003D
	#define DM15a 	0x1CD80000
	#define DM15b 	0x1CD8003D
	
	#define DM16aY 	0x18D70000
	#define DM16bY 	0x18D7003D
	#define DM16a 	0x1CD70000
	#define DM16b 	0x1CD7003D
#endif

#define PEEI	0x1850EAE4		// add 231214 jhlee
#define DM14T	0x18D9CE25		// 18D00000 18DFFFFF
#define DM16T	0x18D7CE25

                          
#define DM15D 	0x18D825CE
#define DM16D 	0x18D725CE

#define DM16_PGN 0xD700
                           
#define TP_CM_T	0x1CECCE25	// 1C000000  0x1CFFFFFF
#define TP_DT_T	0x1CEBCE25
#define TP_CM_D	0x1CEC25CE 
#define TP_DT_D	0x1CEB25CE 
                  
#define P_DTC	0x18FF04E4		// add 230517 jhlee
#define P_DTCU	0x18503FE4		// add 231214 jhlee

#define TP_CM_PGN	0x18ECFFE4	// add 230531 jhlee
#define TP_DT_PGN	0x18EBFFE4	// add 230531 jhlee

#define IEI1	0x18FFF800
#define EFL_P1 	0x18FEEF00
#define EFL_P2 	0x18FEDB00
#define LFE 	0x18FEF200
#define AMB 	0x18FEF500
#define TFAC 	0x10F01A00


#define IC1 	0x18FEF600
#define AT1IMG 	0x18FDB200
#define AT1IG2 	0x18FDB400 
#define A1DOC 	0x18FD2000


#define AT1OG1 		0x18F00F52
//#define AT1T1I 		0x18FE56A3		// modify 230602 jhlee
#define AT1T1I 		0x18FE563D			// modify 230602 jhlee
#define A1SCRDSI1	0x0CF0233D
#define A1SCREGT1 	0x14FD3E3D

#define HF 			0x185011E4 

#define LOCKSTAT 	0x1C5085E4

#define IMM_A 		0x18502E17
#define IMM_R 		0x18EACEE4
#define IMM_I		0x18502BCE
#define BS_EM_R		0x18FED000

#define ACAS		0x18FFA019
#define RI1			0x18FF704C
#define RI2			0x18FF714C
#define RI3			0x18FF724C
#define RI4			0x18FF734C
#define RI5			0x18FF744C


#define UA1			0x1850E6CE
#define UA2			0x1850E78C


#define UI1			0x18511D8C		// modify 230720 jhlee
#define UI2			0x18511E8C		// modify 230720 jhlee
#define UI3			0x18511F8C		// modify 230720 jhlee
#define UI4			0x1851208C		// add 230720 jhlee
#define UI5			0x1851218C		// add 230720 jhlee

#define UM1	        0x18511ACE      //chpark.08.22 //add user req,RFID register,access level register, delete user req
#define UM2	        0x18511BCE      //chpark.08.22 //passcode register
#define UM3	        0x1850DDCE      //chpark.08.22 //MAC ID send (not used)
#define UM4	        0x1850DECE      //chpark.08.22 //change user info (not used)
#define UM5	        0x18511C8C      //chpark.08.22 //complete in changing user's info (reply)

#define GFI			0x1850EEE4
#define PGN50EE 	0x50EE

#define CPR			0x18FF80E4
#define CPI			0x18FF811C
#define XO_SPEC		0x12345678

/*
#define HMH1		0x1850EDE4		// add 230620 jhlee
#define HMH2		0x1850EEE4
#define HMH3		0x1850EFE4
#define HMH4		0x1850F1E4
#define HMH5		0x1850F2E4		
#define HMH6		0x1850F3E4		// add end
*/

#define TCMD1       0x1850FBCE          //chpark pedal calibration.//chpark.07.18
#define TCMD       0x1850FBE4          //chpark pedal calibration.//chpark.07.18
/////////////////////////////////   보내�?   ////////////////////////////////
	#define CLI	 		0x18500BCE		//500ms	x| 00 00 00 3F C0 00 00 01 		x| 00 00 00 3F C0 00 00 01 x| 00 00 00 00 00 00 00 00 
	#define CLI2	 	0x185051CE
	#define CLI3	 	0x185053CE
	#define CLI4		0x185057CE		//200ms	x| FF FF FF FF FF FF FF FF 		x| 00 00 00 00 00 00 00 00
	#define CLI5	 	0x185058CE
	#define CLI6	 	0x185058CE
	
	#define CREEP		0x185302CE		//500ms	x| 03 FF FF FF FF FF FF FF 		x| 05 00 00 00 00 00 00 00 
	#define AUX_FC1		0x18500ECE		//1sec	x| 64 32 4B 1E 32 14 00 03 		x| 64 64 4B 4B 32 32 00 00 
	#define AUX_FC2		0x18500FCE		//1sec	x| 64 64 64 64 00 00 00 00 		x| 64 0A 64 64 00 00 00 00 
	#define ECO1		0x185010CE		//1sec	x| 00 00 00 00 00 00 00 00 		x| 00 00 00 00 00 00 00 00 
#define LOCKPASS 	0x1C5086CE
#define AG_S		0x1C504BCE
#define AG_ST	 	0x1C504CCE
#define PGN_REQ1	0x18EAE4CE
#define PGN_REQ2	0x18EAE3CE
#define OPS1		0x185013CE

#define BS_EM_W		0x18E300FA

// 보내�? End



#define IN1 	0x1C501AE4
#define IN2 	0x1C5020E4
#define IN3 	0x1C5021E4
#define IN4 	0x1C5022E4
#define IN5 	0x1C5041E4
#define IN6 	0x1C5042E4
#define IN7 	0x1C5044E4
#define IN8 	0x1C5047E4
#define IN9 	0x1C5048E4


#define OUT1 	0x1C501CE4
#define OUT2 	0x1C501DE4
#define OUT3 	0x1C501EE4
#define OUT4 	0x1C5023E4
#define OUT5	0x1C5043E4
#define OUT6	0x1C5054E4
#define OUT7	0x1C505AE4
#define OUT8	0x1C5027E4
#define OUT9	0x1C5088E4
#define OUT10	0x1C5090E4
#define OUT11	0x1C5091E4
#define OUT12	0x1C5092E4
#define OUT13	0x1C5093E4
#define OUT14	0x1C5094E4
#define OUT15	0x1C5095E4
#define OUT16	0x1C5096E4
#define OUT17	0x1C5097E4
#define OUT18	0x1C5098E4
#define OUT19	0x1C509AE4 //chpark.24.10.18
#define OUT20	0x1C509BE4 //chpark.24.10.18


#define AUX_IO 		0x1C5026E4
#define AUX1_IO 	0x1C5024E4
#define AUX2_IO 	0x1C5040E4
#define AUX4_IO 	0x1C5055E4
#define MTSPD 	0x1C5049E4
#define IN10 	0x1C5080E4
#define IN11 	0x1C5081E4
#define IN12 	0x1C5082E4
#define IN13 	0x1C5083E4
#define IN14 	0x1C5084E4
#define IN15 	0x1C5087E4
#define IN16 	0x1C5089E4
#define IN17 	0x1C508AE4
#define IN18 	0x1C508BE4
#define IN19 	0x1C508CE4
#define IN20 	0x1C508DE4
#define IN21 	0x1C50E0E4
#define IN22	0x1C50E1E4
#define IN23 	0x1C50E2E4
#define IN24 	0x1C50E3E4
#define IN25 	0x1C5098E4
#define IN26 	0x1C50E4E4

#define IN28 	0x1C50DBE4//chpark.09.13
#define IN29 	0x1C50DCE4//chpark.09.13
#define IN30 	0x1C50DFE4//chpark.09.13


#define HT_MICOM_RST		24
#define HT_GPIO_BUZZER		62
#define HT_GPIO_FLASH_WP	9


#if (MS_PER_FRAME == 33)	//33ms
	#define TIME_CNT_33ms_100MS		3
	#define TIME_CNT_33ms_300MS		9
	#define TIME_CNT_33ms_500MS		15
	#define TIME_CNT_33ms_600MS		18
	#define TIME_CNT_33ms_1000MS	30
	#define TIME_CNT_33ms_2000MS	60
	#define TIME_CNT_33ms_3000MS	91
	#define TIME_CNT_33ms_5000MS	152
	#define TIME_CNT_33ms_30S		909
#else //(MS_PER_FRAME == 17)	//17ms
	#define TIME_CNT_33ms_100MS		6
	#define TIME_CNT_33ms_300MS		18
	#define TIME_CNT_33ms_500MS		29
	#define TIME_CNT_33ms_600MS		35
	#define TIME_CNT_33ms_1000MS	59
	#define TIME_CNT_33ms_2000MS	119
	#define TIME_CNT_33ms_3000MS	176
	#define TIME_CNT_33ms_5000MS	294
	#define TIME_CNT_33ms_30S		1765
#endif


#define TIME_CNT_50MS	5
#define TIME_CNT_100MS	10
#define TIME_CNT_200MS	20
#define TIME_CNT_300MS	30
#define TIME_CNT_400MS	40
#define TIME_CNT_500MS	50
#define TIME_CNT_600MS	60
#define TIME_CNT_1SEC	100
#define TIME_CNT_1_1SEC	110
#define TIME_CNT_1_2SEC	120
#define TIME_CNT_1_5SEC	150
#define TIME_CNT_2SEC	200
#define TIME_CNT_2_5SEC	250
#define TIME_CNT_3SEC	300
#define TIME_CNT_4SEC	400
#define TIME_CNT_4_5SEC	450
#define TIME_CNT_5SEC	500
#define TIME_CNT_7SEC	700
#define TIME_CNT_10SEC	1000
#define TIME_CNT_15SEC	1500
#define TIME_CNT_20SEC	2000
#define TIME_CNT_30SEC	3000
#define TIME_CNT_1MIN	6000
#define TIME_CNT_2MIN	12000

#ifdef RELEASE_VER
    #define TIME_CNT_5SEC_CHPark	500//chpark.07.05 for test.
#else
    #define TIME_CNT_5SEC_CHPark	50000000//chpark.07.05 for test.
#endif

#define LONG_KEY_CMD		0x31	



#define NO_CHECK				0
#define PORTRAIT_1			1
#define PORTRAIT_2			2
#define LANDSCAPE_1			3
#define LANDSCAPE_2			4


#define BG_BLUE		1
#define BG_BLACK	2

#define Type1_YYYYMMDD	1
#define Type2_MMDDYYYY	2
#define Type3_DDMMYYYY	3


typedef enum {
    DISP_OFF 		= 0,
    DISP_ON 		= 1,
    DISP_NORMAL_ON 	= 2,
    DISP_ZOOM_ON 	= 3,
    DISP_ANIMATION 	= 4,
    DISP_ANI_TOGGLE	= 5,
    DISP_ANI_NORMAL	= 6
} DISP_MODE;

typedef enum {
    CAM_SEL_LEFT		= 21,	//LEFT	
    CAM_SEL_RIGHT		= 22,	//RIGHT
    CAM_SEL_AC			= 23,	//AC
    CAM_SEL_RADIO		= 24,	//RADIO
    CAM_SEL_BAR			= 25, 	//BAR
	MAIN_SEL_OFF 		= 70,
    MAIN_SEL_WARRING	= 71,
    MAIN_SEL_ATT 		= 72,
    MAIN_SEL_AIRCON		= 73,
    MAIN_SEL_RADIO 		= 74,
    MAIN_SEL_SPK 		= 75,
    MAIN_SEL_HOURMETER	= 76,
    MAIN_SEL_BAR 		= 77,
    MAIN_SEL_TIMEDATE	= 78,
    MAIN_SEL_POWER		= 79,
    MAIN_SEL_CAMERA		= 80,
    MAIN_SEL_MENU		= 91
} MAIN_SEL_IDX;


typedef enum {
	AC_SEL_OFF 		= 0,
    AC_SEL_AIRMODE	= 1,
    AC_SEL_ONOFF 	= 2,
    AC_SEL_DIR_L	= 3,
    AC_SEL_DIR_R	= 4,
    AC_SEL_TEMP_L 	= 5,
    AC_SEL_TEMP_R 	= 6,
    AC_SEL_LEVEL_L	= 7,
    AC_SEL_LEVEL_R	= 8,
    AC_SEL_AUTO 	= 9,
    AC_SEL_PWR		= 10,
   
    AC_SEL_ATT 		= 72,
    AC_SEL_AIRCON	= 73,
    AC_SEL_RADIO 	= 74,
    AC_SEL_SPK 		= 75,
    AC_SEL_HOURMETER= 76,
    AC_SEL_BAR 		= 77,
    AC_SEL_TIMEDATE	= 78,
    AC_SEL_POWER	= 79,
    AC_SEL_CAMERA	= 80,
    AC_SEL_MENU		= 91
} AC_SEL_IDX;

typedef enum {
	RD_SEL_OFF 		= 0,
    RD_SEL_1C		= 1,
    RD_SEL_2C		= 2,
    RD_SEL_3C		= 3,
    RD_SEL_4C		= 4,
    RD_SEL_5C		= 5,
    RD_SEL_6C		= 6,
    RD_SEL_7C		= 7,
    RD_SEL_8C		= 8,
    RD_SEL_TUNE_L 	= 9,
    RD_SEL_TUNE_R	= 10,
    RD_SEL_AUTO		= 11,
	RD_SEL_MODE 	= 12,
	RD_SEL_TONE_D 	= 13,
	RD_SEL_TONE_U	= 14,
	RD_SEL_VOL_D 	= 15,
	RD_SEL_VOL_U	= 16,
	RD_SEL_MUTE 	= 17,
	RD_SEL_PWR		= 18,
	RD_SEL_LOUD 	= 19,
	RD_SEL_SCAN 	= 20,
	DAB2_SEL_1C		= 21,
	DAB2_SEL_2C		= 22,
	DAB2_SEL_3C		= 23,
	DAB2_SEL_4C		= 24,
	DAB2_SEL_5C		= 25,
	DAB2_SEL_6C		= 26,
	DAB2_SEL_7C		= 27,
	DAB2_SEL_8C		= 28,
	BT_SEL_R27		= 29,// R27 BT_SEL_1C
	BT_SEL_R28		= 30,// R28 BT_SEL_2C
	BT_SEL_R29		= 31,// R29 BT_SEL_3C
	BT_SEL_R30		= 32,// R30 BT_SEL_4C
	PH_SEL_R34		= 33,// R34 PH_SEL_1C
	PH_SEL_R31		= 34,// R31 PH_SEL_2C
	PH_SEL_R32		= 35,// R32 PH_SEL_3C
	
	mRD_BT_CNT_ALL	= PH_SEL_R32,
	
	RD_SEL_SCROLL  = 40,//chpark.06.21
	
	RD_SEL_ATT 		= 72,
	RD_SEL_AIRCON	= 73,
	RD_SEL_RADIO 	= 74,
	RD_SEL_SPK 		= 75,
	RD_SEL_HOURMETER= 76,
	RD_SEL_BAR 		= 77,
	RD_SEL_TIMEDATE	= 78,
	RD_SEL_POWER	= 79,
	RD_SEL_CAMERA	= 80,
	RD_SEL_MENU		= 91
} RD_SEL_IDX;
#define DAB_PRG_CNT_V	7
#define DAB_PRG_CNT_H	8

typedef enum {			// CLI2[0] ���� ���� �����ȴ�
	MAIN_MODE_NORMAL		= 1,
	MAIN_MODE_AIRCON		= 2,
	MAIN_MODE_RADIO 		= 3,
	MAIN_MODE_CAMERA		= 4,
	MAIN_MODE_MENU			= 5,
	MAIN_MODE_BACK			= 6,
	MAIN_MODE_BLUETOOTH		= 7,
	MAIN_MODE_PHONE 		= 8,
	MAIN_MODE_DAB1			= 9,
	MAIN_MODE_PW			= 11,
	MAIN_MODE_USERINFO		= 12,
	MAIN_MODE_ENGN_COOL		= 13,
	MAIN_MODE_DAB2 			= 15,
	MAIN_MODE_DAB3 			= 16,
	MAIN_FIRMWARE_UP 		= 20,
	MAIN_MODE_MENU_ATT		= 21,
	MAIN_MODE_MENU_TIME 	= 22,
	MAIN_MODE_MENU_WARRING 	= 23,
    MAIN_MODE_IMMOBI		= 24  //chpark.24.10.30
} MAIN_MODE_IDX;

typedef enum {
	JOG_DIAL_ROT_LEFT		= 0x40,
    JOG_DIAL_ROT_RIGHT		= 0x41,
    JOG_DIAL_KEY_LEFT 		= 0x42,
    JOG_DIAL_KEY_RIGHT		= 0x43,
    JOG_DIAL_KEY_UP 		= 0x44,
    JOG_DIAL_KEY_DOWN		= 0x45,
    JOG_DIAL_KEY_ENTER_ON 	= 0x46,
    JOG_DIAL_ROT_LEFT_OFF	= 0x50,
    JOG_DIAL_ROT_RIGHT_OFF	= 0x51,
    JOG_DIAL_KEY_LEFT_OFF 	= 0x52,
    JOG_DIAL_KEY_RIGHT_OFF	= 0x53,
    JOG_DIAL_KEY_UP_OFF		= 0x54,
    JOG_DIAL_KEY_DOWN_OFF	= 0x55,
    JOG_DIAL_KEY_ENTER_OFF 	= 0x56,
    JOG_DIAL_KEY_OFF_MASK	= 0x50,
    
    
    JOG_DIAL_SW_ARICON		= 0x67,
    JOG_DIAL_SW_AC_AUTO		= 0x68,
    JOG_DIAL_SW_AC_OFF		= 0x69,
    JOG_DIAL_SW_ESC_ON		= 0x6A,
    JOG_DIAL_SW_HOME		= 0x6C,
    JOG_DIAL_SW_AUDIO		= 0x6D,
    JOG_DIAL_SW_ARICON_OFF	= 0x77,
    JOG_DIAL_SW_AC_AUTO_OFF	= 0x78,
    JOG_DIAL_SW_AC_OFF_OFF	= 0x79,
    JOG_DIAL_SW_ESC_ON_OFF	= 0x7A,
    JOG_DIAL_SW_HOME_OFF	= 0x7C,
    JOG_DIAL_SW_AUDIO_OFF	= 0x7D,
    JOG_DIAL_SW_OFF_MASK	= 0x70,
    
     
    JOG_DIAL_SW_NONE		= 0x8E,
    JOG_DIAL_SW_DEEP		= 0x8F,
    JOG_DIAL_KEY_NONE		= 0x80,
    JOG_DIAL_KEY_DEEP		= 0x81
} MAIN_JOG_DIAL_IDX;

#define MAX_SU100_USER_NUM		15 //chpark.08.22
#define MAX_SU100_USER_NAME		50 // JPHong 2023-11-06
#define MAX_SU100_NAME_CNT	    20 // chpark.12.12   //max real character count in list.
#define MAX_SU100_USER_PW		15 // JPHong 2023-11-06
#define MAX_SU100_USER_MID		15 // JPHong 2023-11-06
#define MAX_DEVICE_UID		    20 // chpark 2024.05.31
#define MAX_PW100_PWD		    4  // chpark 2024.09.09

static const char* DAB2_PTY_No[] =
{
	"ALL",
	"NEWS",
	"AFFAIRS",
	"INFO",
	"SPORT",
	"EDUCATE",
	"DRAMA",
	"CULTURE",
	"SCIENCE",
	"VARIED",
	"POP M",
	"ROCK M",
	"EASY M",
	"LIGHT M",
	"CLASSICS",
	"OTHER M",
	"WEATHER",
	"FINANCE",
	"CHILDREN",
	"SOCIAL",
	"RELIGION",
	"PHONE IN",
	"TRAVEL",
	"LEISURE",
	"JAZZ",
	"COUNTRY",
	"NATION M",
	"OLDIES",
	"FOLK M",
	"DOCUMENT"
};

#define MAX_DAB2LIST_COUNT (sizeof(DAB2_PTY_No) / sizeof(DAB2_PTY_No[0]))


typedef struct _UserRegisterDM14
{
	uint8_t RegKeyNum;
	uint8_t LeverPattern;	// 0-2bit: Lever Pattern
	uint8_t AutoCamera;		// 3bit: AutoCamera
	uint8_t AutoSpeedShift;	// 4bit: Auto Speed Shift
	uint8_t Language;		// 5-7bit: Language
    uint8_t Level;									// 0;Not Register ,1;Admin ,2;Standard ,3;N/A
	uint8_t UserName[MAX_SU100_USER_NAME];			// username 10digit
	uint8_t UserPassCode[MAX_SU100_USER_PW];		// user passcode 10digit(Max 10digit,Min 4digit)
}UserInfoDM14;

extern UserInfoDM14 User_RegInfoDM14;

typedef struct
{
    uint16_t 	Length;		// 0Byte,1Byte 5-7Bit
    uint8_t 	Command;	// 1Byte 1-3Bit
    uint32_t 	Pointer;	// 2-4Byte    
    uint8_t 	PointerEx;	// 5Byte   
    uint16_t 	User_Level;	// 6-7Byte
    uint16_t 	Key;		// 6-7Byte
    
    uint16_t 	Count;
    uint32_t 	Time;
    uint32_t DM14_ErrorFlag;
} DM14_DATA;

typedef struct
{
    uint16_t 	Length;	// 0Byte,1Byte 5-7Bit
    uint8_t 	Status;	// 1Byte 1-3Bit
    uint32_t 	EDCP;	// 2-4Byte
    uint8_t 	EDCPEx;	// 5Byte
    uint16_t 	Seed;	// 6-7Byte
    
    uint16_t 	Count;
} DM15_DATA;				

typedef struct
{
    uint8_t 	Control;	// 0Byte
    uint16_t 	Length;		// 1Byte,2Byte
    uint8_t 	TNoP;		// 3Byte	Total number of packets
    uint8_t 	MNoP;		// 4Byte	Maximum number of packets
    uint32_t 	PGN;		// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
} TPCM_RTS;

typedef struct
{
    uint8_t 	Control;	// 0Byte
    uint8_t 	NPS;		// 1Byte	Number of packets that can be sent
    uint8_t 	NPN;		// 2Byte	Next packet number to be sent
    uint16_t 	SAE;		// 3,4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
    uint32_t 	PGN;		// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
} TPCM_CTS;

typedef struct
{
    uint8_t 	Control;	// 0Byte
    uint16_t 	Size;		// 1,2Byte	Total message size, number of bytes
    uint8_t 	Packet;		// 3Byte	Total number of packets
    uint8_t 	SAE;		// 4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
    uint32_t 	PGN;		// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
    
    uint16_t 	Count;
} TPCM_ACK;

typedef struct
{
    uint8_t 	PSN;		// 0Byte	Sequence Number(packeted)
    uint8_t 	Data[525];	// 35x15(MAX_SU100_USER_NUM)
    uint16_t 	Count;
} DM14_TPDT;


#define DM14_W_CMD	0x02
#define DM14_P_CMD	0x00

#define DM14_RX_ERASE		0x00
#define DM14_RX_READ		0x01
#define DM14_RX_WRITE		0x02
#define DM14_RX_REQUEST		0x03
#define DM14_RX_COMPLETED	0x04
#define DM14_RX_FAILED		0x05
#define DM14_RX_LOAD		0x06
#define DM14_RX_GENERATION	0x07

#define DM14_TX_PROCEED		0x00
#define DM14_TX_BUSY		0x01
#define DM14_TX_COMPLETED	0x04
#define DM14_TX_FAILED		0x05


#define DM14_ERROR_LENGTH	0x00102
#define DM14_ERROR_POINTER	0x00100
#define DM14_ERROR_SEED		0x01003
#define DM14_ERROR_TIME		0x10002


/*
Control byte = 16, Destination Specific Request_To_Send (RTS)
Control byte = 17, Destination Specific Clear_To_Send (CTS)
Control byte = 19, End_of_Message Acknowledge
Control byte = 32, Broadcast Announce Message
*/

#define TPCM_RX_RTS		16
#define TPCM_RX_CTS		17
#define TPCM_RX_EOMA	19
#define TPCM_RX_BAM		32

#define DM16_PGN	0xD700


#define SU600_AUX_TYPE_NONE  0		// JPHong 2024-03-20
#define SU600_AUX_TYPE_YES   1
#define SU600_AUX_TYPE_NO    2


typedef struct
{
    int x;  ///< X coordinate, in pixels
    int y;  ///< Y coordinate, in pixels
    uint16_t Index;
} IconInfo;

typedef struct{
    uint32_t 	IDentifier;//Id
    uint8_t		RxData[8];
    volatile bool	Flag;
}CAN_RxTx_DATA;


#define SetWidget(DesIcon,IconName)	{														\
										DesIcon = ituSceneFindWidget(&theScene, IconName);	\
										assert(DesIcon);									\
									}

#define IncrementData(DataV,MaxV,defaultV)	{							\
												DataV++;				\
												if (DataV>=MaxV)		\
													DataV = defaultV;	\
											}

#define DecrementData(DataV,MinV,defaultV)	{							\
												if (DataV<=MinV)		\
													DataV = defaultV;	\
												DataV--;				\
											}

#define IncrementDataLong(DataV,MaxV,defaultV)	{										\
													IncrementData(DataV,MaxV,defaultV)	\
													IncrementData(DataV,MaxV,defaultV)	\
													IncrementData(DataV,MaxV,defaultV)	\
													IncrementData(DataV,MaxV,defaultV)	\
													IncrementData(DataV,MaxV,defaultV)	\
													IncrementData(DataV,MaxV,defaultV)	\
												}
#define DecrementDataLong(DataV,MinV,defaultV)	{										\
													DecrementData(DataV,MinV,defaultV)	\
													DecrementData(DataV,MinV,defaultV)	\
													DecrementData(DataV,MinV,defaultV)	\
													DecrementData(DataV,MinV,defaultV)	\
													DecrementData(DataV,MinV,defaultV)	\
													DecrementData(DataV,MinV,defaultV)	\
												}
											
////////////////////////  
	
	
	
typedef struct _HourMeterValue 
{
	uint8_t HourMeter_Toggle;
	uint8_t HourMeter_Trip;
	uint8_t HourMeter_Trip_Clear1; 	// Trip1 : 0-1bit  , Trip2 : 2-3bit  , Trip3 : 4-5bit  , 
	uint8_t HourMeter_Trip_Clear2; 	// Trip1 : 0-1bit  , Trip2 : 2-3bit  , Trip3 : 4-5bit  , 
    uint32_t HourMete_rHour[7];
    uint8_t Min_V;
    uint8_t Hour_V;
    uint8_t Day_V;
    uint8_t Mon_V;
    uint16_t Year_V;
} HourMeterValue;  

typedef struct _MeterRateValue 
{
	uint8_t Symbol; // m11_MeterSymbol_PM=11,m12_MeterSymbol_LiftLd=12,m13_MeterSymbol_EngRPM=13,m14_MeterSymbol_EngLd=14
	
    uint8_t PM;	// 0 ~ 100  0%~100%
    uint32_t PMTime;
    uint8_t LiftLd;	// 0 ~ 250  0%~100%
    uint32_t LiftLdTime;
    uint8_t EngRPM;	// 0 ~ 100  0%~100%
    uint32_t EngRPMTime;
    uint8_t EngLd;	// 0 ~ 250  0%~100%
    uint32_t EngLdTime;
    uint8_t ASH;	// 0 ~ 250  0%~100%
    uint32_t ASHTime;
//    bool MeterRevoFlag;
//    bool MeterLiftAlarmFlag;
} MeterRateValue;

typedef struct _AirConValue
{
	uint8_t AirMode; 	// In Out
    uint8_t ACTemp;		//
    bool ACOnOff;
    uint8_t ACLevel;	// 0-5
    bool ACAuto;
    uint8_t ACDir;		// 0-4
} AirConValue;


#define DAB_SVC_CNT	50
typedef struct _RadioValue
{
	bool RDOnOff; 		// oNoFF
	uint8_t RDMute;
	uint8_t RDLOUD;
    bool RDStereo;		//
    bool RDScan;
    uint8_t RDMode;		// AM,FM,BT,DAB1,DAB2
    uint16_t RDFreq;	//
    bool RDStep;
    uint8_t RDTone;
    uint8_t RDVol;
    uint8_t RDSelCh;
    uint8_t DABSelCh;
    uint8_t DABFreq;
    uint16_t DABServiceCnt[DAB_SVC_CNT];
    
    uint8_t BTPair;		// 
    uint8_t RDPhone;		// 0-4
    uint8_t RDPhoneNum[10];
    uint8_t RDPrg[120];	// 7*16 = 112
    uint8_t RDBCS[120];
    int PTYNo;

    int DAB2_PTYNo;//chpark.05.30 //?��?��롤에?�� ?��?��?�� ?��?��?�� ?��?��
    int DAB3_PTYNo;//chpark.05.30 //?��?��롤에?�� ?��?��?�� ?��?��?�� ?��?��
} RadioValue;

typedef struct _AUXValue 
{
    uint8_t FirstAUX_L;		// 0 ~ 250
    uint8_t FirstAUX_R;		// 0 ~ 250
    uint8_t SecondAUX_L;	// 0 ~ 250
    uint8_t SecondAUX_R;	// 0 ~ 250
    uint8_t FourthAUX_L;	// 0 ~ 250
    uint8_t FourthAUX_R;	// 0 ~ 250
} AUXValue;  
typedef struct _DATAValue 
{
	uint32_t EEC1_34Bytes;	// [?��진수 ?��?�� 범위 0 ~ 65,278] [분해?�� 0.125/bit)] [?���? 출력 범위 0 ~ 8,159]
	uint32_t EEC1_34Time;	// [?��진수 ?��?�� 범위 0 ~ 65,278] [분해?�� 0.125/bit)] [?���? 출력 범위 0 ~ 8,159]
	uint16_t EEC1_34Cnt;	// JPHong0830
	uint8_t ET1_1Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint32_t ET1_1Time;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint8_t SI_0Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint32_t SI_0Time;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint8_t AT1T1I_1Byte;	// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint32_t AT1T1I_1Time;	// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint8_t DD_1Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 100]
	uint32_t DD_1Time;		// [?��진수 ?��?�� 범위 0 ~ 250] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 100]
	uint8_t AT1T1I_0Byte;	// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 100]
	uint32_t AT1T1I_0Time;	// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 100]
	uint16_t VEP_45Bytes;	// [?��진수 ?��?�� 범위 0 ~ 1,790] [분해?�� 0.05/bit)] [?���? 출력 범위 0 ~ 89.5]
	uint32_t VEP_45Time;	// [?��진수 ?��?�� 범위 0 ~ 1,790] [분해?�� 0.05/bit)] [?���? 출력 범위 0 ~ 89.5]
	uint8_t EI_2Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
	uint32_t EI_2Time;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
	uint8_t AT1S_1Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
	uint32_t AT1S_1Time;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
	uint8_t ET1_0Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint32_t ET1_0Time;		// [?��진수 ?��?�� 범위 0 ~ 250] [OFFSET -40] [?���? 출력 범위 -40 ~ 210]
	uint8_t AT1S_2Byte;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
	uint32_t AT1S_2Time;		// [?��진수 ?��?�� 범위 0 ~ 250] ] [분해?�� 0.4/bit)] [?���? 출력 범위 0 ~ 250]
} DATAValue;  

//chpark.

//chpark.24.10.28
typedef struct _MB_IO_Value 
{
//Engine(1)---------------
    uint8_t  SI_3Byte;	    //(1)Accel position	
    uint32_t SI_3Time;      
    uint16_t EEC1_34Bytes;  //(2)Operating speed
    uint32_t EEC1_34Time; 
    uint8_t  EEC2_2Byte;    //(3)Torque
    uint32_t EEC2_2Time;  	
    uint8_t  EFL_P1_3Byte;	//(4)Oil pressure
    uint32_t EFL_P1_3Time;  
    uint8_t  SI_0Byte;		//(5)Hydraulic oil temp  
    uint32_t SI_0Time;      	

    uint8_t  ET1_0Byte;     //(6)Coolant temp.
    uint32_t ET1_0Time;     
    uint16_t LFE_01Bytes;   //(7)Fuel rate
    uint32_t LFE_01Time;    
    uint8_t  ET1_1Byte;     //(8)Fuel temp.
    uint32_t ET1_1Time;     
    uint8_t  AMB_0Byte;     //(9)Barometric pressure	
    uint32_t AMB_0Time;     
    uint16_t AMB_34Bytes;   //(10)Ambient air temp.
    uint32_t AMB_34Time;    	
    
    uint8_t  IC1_3Byte;     //(11)Engine air inlet pressure	
    uint32_t IC1_3Time;     
    uint8_t  IC1_2Byte;	    //(12)Boost temp.
    uint32_t IC1_2Time;
    uint16_t EFL_P2_23Bytes;//(13)Common-rail pressure
    uint32_t EFL_P2_23Time; 
    uint16_t TFAC_01Bytes;  //(14)Engine throttle actuator
    uint32_t TFAC_01Time;   
    uint16_t ET2_45Bytes;   //(15)EGR differential pressure
    uint32_t ET2_45Time;    
    
    uint16_t ET2_67Bytes;   //(16)EGR Temp.
    uint32_t ET2_67Time;    

    //uint8_t  IC1_1Byte;	
    //uint32_t IC1_1Time;  
    
    //uint16_t ET1_23Bytes;   
    //uint32_t ET1_23Time;    
//Engine(2)---------------
    uint16_t  AT1IMG_45Bytes;   //(1) DPF exhaust diff. pressure
    uint32_t  AT1IMG_45Time;    
    uint16_t  AT1IG2_23Bytes;   //(2) DPF inlet gas temp.
    uint32_t  AT1IG2_23Time;
    uint16_t  AT1IMG_23Bytes;   //(3) DPF intermediate temp.
    uint32_t  AT1IMG_23Time;    
    uint16_t  AT1IG2_01Bytes;   //(4) DPF outlet gas temp.
    uint32_t  AT1IG2_01Time;
    uint32_t  AT1S_2345Bytes;   //(5) DPF time since last	
    uint32_t  AT1S_2345Time;

    uint16_t  A1SCREGT1_01Bytes;//(6) SCR inlet gas temp.
    uint32_t  A1SCREGT1_01Time;
    uint8_t  AT1S_0Byte;	    //(7)DPF soot load percent
    uint32_t AT1S_0Time;    
    uint8_t  AT1S_1Byte;        //(8)DPF ash load percent
    uint32_t AT1S_1Time;    
    uint8_t  AT1T1I_0Byte;      //(9)DPF tank level
    uint32_t AT1T1I_0Time;  
    uint8_t  AT1T1I_1Byte;      //(10)DPF tank temp.
    uint32_t AT1T1I_1Time;  

    uint16_t  A1SCRDSI1_01Bytes;//(11)DEF actual quantity
    uint32_t  A1SCRDSI1_01Time;
    uint8_t   A1SCRDSI1_5Byte;	//(12)DEF absolute pressure
    uint32_t  A1SCRDSI1_5Time;  
//Vehicle Information(3) ---------------    
    uint8_t   AUX1_IO_7Byte;    //(1)AUX1 prop.
    uint32_t  AUX1_IO_7Time;    
    uint8_t   AUX2_IO_7Byte;    //(2)AUX2 prop.
    uint32_t  AUX2_IO_7Time;    
    uint8_t   AUX4_IO_7Byte;    //(3)AUX4 prop.
    uint32_t  AUX4_IO_7Time;    
    uint8_t   IN10_7Byte;       //(4)Blade lever
    uint32_t  IN10_7Time;
    uint8_t   IN10_6Byte;       //(5)Swing
    uint32_t  IN10_6Time;

    uint8_t   IN17_5Byte;       //(6)Adjust
    uint32_t  IN17_5Time;
    uint16_t  SI_45Bytes;	    //(7)Lift alarm sensor
    uint32_t  SI_45Time;     

    //uint16_t  IN23_45Bytes;   //(8)Adjust crowd
    //uint32_t  IN23_45Time;  
    //uint16_t  IN23_67Bytes;   //(9)Adjust dump
    //uint32_t  IN23_67Time;  
// ETC(4)--------------- 
    //program version ---------------   
    char MAIN_CON_1234567Bytes[10];//(1)Main controller
    //- (2)Display soft
    //Air Conditioner Information ---------------
    uint8_t   ACAS_0Byte;   //(3)Set point temp.
    uint32_t  ACAS_0Time;
    uint8_t   ACAS_2Byte;   //(4)Cabin point temp.
    uint32_t  ACAS_2Time;
    uint8_t   ACAS_4Byte;   //(5)Evaporator point temp.
    uint32_t  ACAS_4Time;


   
    //============================
    
    //---------------------
    //IO
    //Input1
    //(1)C1P06
    uint8_t   IN1_4Byte_4bit;	//chpark
    uint32_t  IN1_4Byte_4Time;  //chpark.07.05
    //uint8_t   IN2_5Byte_4bit;	//chpark //chpark.09.13
    //uint32_t  IN2_5Byte_4Time;  //chpark.07.05//chpark.09.13
    //(2)C1P07
   
    //(3)-
    //uint8_t   IN1_3Byte_0bit;	//chpark//chpark.09.13
    //uint32_t  IN1_3Byte_0Time;  //chpark.07.05//chpark.09.13
    //(4)C1P11    
    uint16_t  IN3_45Bytes;	    //chpark
    uint32_t  IN3_45Time;       //chpark.07.05
    //(5)C1P12
    uint16_t  OUT2_23Bytes;	    //chpark
    uint32_t  OUT2_23Time;      //chpark.07.05
    //uint8_t   IN2_5Byte_5bit;	//chpark
    //uint32_t  IN2_5Byte_5Time;  //chpark.07.05
    //(6)C1P15
    uint8_t   IN29_0Byte_3bit;	 //chpark.09.13
    uint32_t  IN29_0Byte_3Time;  //chpark.09.13
    //(7)C1P16
    uint16_t  IN3_67Bytes;	    //chpark
    uint32_t  IN3_67Time;       //chpark.07.05
    //(8)C1P17
    uint8_t   IN2_4Byte_7bit;	//chpark
    uint32_t  IN2_4Byte_7Time;  //chpark.07.05
    //(9)C1P18
    uint8_t   IN1_2Byte_4bit;	//chpark
    uint32_t  IN1_2Byte_4Time;  //chpark.07.05
    //(10)C1P19 - 

    //uint8_t   IN2_3Byte_0bit;	//chpark//chpark.09.13
    //uint32_t  IN2_3Byte_0Time;  //chpark.07.05//chpark.09.13
    //(11)C1P20
    uint16_t  OUT2_01Bytes;	    //chpark
    uint32_t  OUT2_01Time;      //chpark.07.05
    //(12)C1P21
    uint16_t  IN3_23Bytes;	    //chpark
    uint32_t  IN3_23Time;       //chpark.07.05

    //uint8_t   IN2_5Byte_2bit;	//chpark//chpark.09.13
    //uint32_t  IN2_5Byte_2Time;  //chpark.07.05//chpark.09.13

    //uint16_t  AUX2_IO_01Bytes;  //chpark//chpark.09.13
    //uint32_t  AUX2_IO_01Time;   //chpark.07.05//chpark.09.13
    //(13)C1P22-
    //(14)C1P26
    uint16_t  IN28_45Bytes;     //chpark.09.13
    uint32_t  IN28_45Time;      //chpark.09.13
    uint16_t  IN30_12Bytes;     //chpark.09.13
    uint32_t  IN30_12Time;      //chpark.09.13
    
    //uint16_t  AUX1_IO_01Bytes;  //chpark
    //uint32_t  AUX1_IO_01Time;   //chpark.07.05
    //(15)C1P27
    uint16_t  IN28_23Bytes;  //chpark.09.13
    uint32_t  IN28_23Time;   //chpark.09.13
    uint16_t  IN29_12Bytes;  //chpark.09.13
    uint32_t  IN29_12Time;   //chpark.09.13
    
    //(16)C1P28
    uint8_t   IN26_0Byte_4bit;   //chpark//chpark.09.13
    uint32_t  IN26_0Byte_4Time;  //chpark.07.05//chpark.09.13
    //uint8_t   IN1_4Byte_6bit;   //chpark
    //uint32_t  IN1_4Byte_6Time;  //chpark.07.05
    //(17)C1P29
    uint16_t  IN29_56Bytes;  //chpark.09.13
    uint32_t  IN29_56Time;   //chpark.09.13
    //(18)C1P30-
    //------------------------------------
    //Input2
    //(1)c2P01
    //uint16_t  IN25_45Bytes; //chpark//chpark.09.13
    //uint32_t  IN25_45Time;  //chpark.07.05//chpark.09.13
    uint16_t  IN30_56Bytes; //chpark.09.13
    uint32_t  IN30_56Time;  //chpark.09.13

    //(2)C2P02
    //uint16_t  IN16_67Bytes; //chpark
    //uint32_t  IN16_67Time;  //chpark.07.05
    uint16_t  IN30_34Bytes; //chpark.09.13
    uint32_t  IN30_34Time;  //chpark.09.13
    
    //(3)C2P03
    //uint16_t  IN16_23Bytes; //chpark
    //uint32_t  IN16_23Time;  //chpark.07.05
    uint16_t  IN29_34Bytes; //chpark.09.13
    uint32_t  IN29_34Time;  //chpark.09.13
    
    //(4)C2P04
    //uint8_t   IN2_5Byte_6bit;   //chpark
    //uint32_t  IN2_5Byte_6Time;  //chpark.07.05
    uint16_t  IN19_45Bytes;     //chpark.09.13
    uint32_t  IN19_45Time;      //chpark.09.13
    
    //(5)C2P05
    //uint8_t   IN1_2Byte_2bit;   //chpark
    //uint32_t  IN1_2Byte_2Time;  //chpark.07.05
    uint16_t  IN19_67Bytes;     //chpark.09.13
    uint32_t  IN19_67Time;      //chpark.09.13

    //(6)C2P06
    uint16_t  IN19_23Bytes; //chpark
    uint32_t  IN19_23Time;  //chpark.07.05
    //(7)C2P07
    uint16_t  IN21_01Bytes; //chpark
    uint32_t  IN21_01Time;  //chpark.07.05
    //(8)C2P08
    //uint8_t   IN2_5Byte_7bit;   //chpark
    //uint32_t  IN2_5Byte_7Time;  //chpark.07.05
    uint8_t   IN30_0Byte_3bit;   //chpark.09.13
    uint32_t  IN30_0Byte_3Time;  //chpark.09.13
    //(9)C2P11
    uint8_t   IN1_4Byte_0bit;   //chpark
    uint32_t  IN1_4Byte_0Time;  //chpark.07.05
    //(10)C2P12
    uint8_t   IN1_2Byte_3bit;   //chpark
    uint32_t  IN1_2Byte_3Time;  //chpark.07.05
    //uint8_t   IN1_1Byte_6bit;   //chpark
    //uint32_t  IN1_1Byte_6Time;  //chpark.07.05
    //(11)C2P13
    uint8_t   IN28_0Byte_1bit;   //chpark.09.13
    uint32_t  IN28_0Byte_1Time;  //chpark.09.13
    uint8_t   IN29_0Byte_2bit;	 //chpark.09.13
    uint32_t  IN29_0Byte_2Time;  //chpark.09.13
    uint8_t   IN29_0Byte_5bit;   //chpark.09.13
    uint32_t  IN29_0Byte_5Time;  //chpark.09.13
    //(12)C2P14 - 
    //(13)~(16) - 
    //uint8_t   IN1_2Byte_7bit;   //chpark
    //uint32_t  IN1_2Byte_7Time;  //chpark.07.05
    uint8_t   IN1_4Byte_5bit;   //chpark
    uint32_t  IN1_4Byte_5Time;  //chpark.24.10.18
    //(17)C2P19 -
    //(18)C2P20  
    uint8_t   IN2_1Byte_2bit;   //chpark
    uint32_t  IN2_1Byte_2Time;  //chpark.24.04.08
    uint8_t   IN2_2Byte_6bit;   //chpark
    uint32_t  IN2_2Byte_6Time;  //chpark.07.05
    //----------------------------------------
    //Input3
    //(1)C2P21
    //uint8_t   IN1_0Byte_3bit;       //chpark
    //uint32_t  IN1_0Byte_3Time;      //chpark.07.05
    uint8_t   IN28_1Byte_3bit;       //chpark
    uint32_t  IN28_1Byte_3Time;      //chpark.09.13
    uint8_t   IN30_0Byte_6bit;       //chpark
    uint32_t  IN30_0Byte_6Time;      //chpark.09.13
    //(2)C2P22
    //uint8_t   IN1_1Byte_0bit;       //chpark
    //uint32_t  IN1_1Byte_0Time;      //chpark.07.05
    uint8_t   IN28_1Byte_1bit;       //chpark.09.13
    uint32_t  IN28_1Byte_1Time;      //chpark.09.13
    uint8_t   IN30_0Byte_5bit;       //chpark.09.13
    uint32_t  IN30_0Byte_5Time;      //chpark.09.13
    
    //(3)C2P23
    //uint8_t   AUX1_IO_6Byte_01bit;  //chpark
    //uint32_t  AUX1_IO_6Byte_01Time; //chpark.07.05
    uint8_t   IN28_0Byte_2bit;  //chpark.09.13
    uint32_t  IN28_0Byte_2Time; //chpark.09.13
    uint8_t   IN29_0Byte_1bit;  //chpark.09.13
    uint32_t  IN29_0Byte_1Time; //chpark.09.13
    
    //(4)C2P24
    //uint8_t   AUX1_IO_6Byte_23bit;  //chpark
    //uint32_t  AUX1_IO_6Byte_23Time; //chpark.07.05
    uint8_t   IN28_0Byte_0bit;  //chpark.09.13
    uint32_t  IN28_0Byte_0Time; //chpark.09.13
    uint8_t   IN29_0Byte_0bit;  //chpark.09.13
    uint32_t  IN29_0Byte_0Time; //chpark.09.13
    //(5)C2P25
    //uint8_t   IN1_0Byte_2bit;   //chpark
    //uint32_t  IN1_0Byte_2Time;  //chpark.07.05
    uint8_t   IN28_1Byte_0bit;   //chpark.09.13
    uint32_t  IN28_1Byte_0Time;  //chpark.09.13
    uint8_t   IN30_0Byte_1bit;   //chpark.09.13
    uint32_t  IN30_0Byte_1Time;  //chpark.09.13

    //(6)C2P26
    uint8_t   IN26_1Byte_0bit;  //chpark
    uint32_t  IN26_1Byte_0Time; //chpark.07.05
    //(7)C2P27 -
    //(8)C2P28
    uint8_t   IN2_4Byte_6bit;   //chpark
    uint32_t  IN2_4Byte_6Time;  //chpark.07.05
    //(9)C2P29
    //uint16_t  IN19_45Bytes; //chpark
    //uint32_t  IN19_45Time;  //chpark.07.05
    uint8_t  IN30_0Byte_2bit;   //chpark.09.13
    uint32_t  IN30_0Byte_2Time;  //chpark.09.13
    
    //(10)C2P30
    //uint16_t  IN19_67Bytes; //chpark
    //uint32_t  IN19_67Time;  //chpark.07.05
    uint8_t  IN1_2Byte_2bit;   //chpark.09.13
    uint32_t  IN1_2Byte_2Time;  //chpark.09.13
    
    //-------------------
    //Output1
    //(1)C1P31
    uint8_t   OUT6_7Byte_3bit;   //chpark
    uint32_t  OUT6_7Byte_3Time;  

 

    //(2)C1P32
    uint16_t  OUT3_01Bytes; //chpark
    uint32_t  OUT3_01Time;  //chpark.07.05
    //(3)C1P33
    uint8_t   OUT1_3Byte_3bit;   //chpark
    uint32_t  OUT1_3Byte_3Time;  //chpark.07.05
    //(4)C1P34
    uint8_t   OUT1_0Byte_0bit;   //chpark
    uint32_t  OUT1_0Byte_0Time;  //chpark.07.05
    //(5)C1P35
    uint8_t   OUT1_4Byte_7bit;   //chpark
    uint32_t  OUT1_4Byte_7Time;  //chpark.07.05
    
    //(6)C1P36
    //uint8_t   OUT1_2Byte_5bit;   //chpark
    //uint32_t  OUT1_2Byte_5Time;  //chpark.07.05
    uint8_t   OUT1_3Byte_2bit;   //chpark.09.13
    uint32_t  OUT1_3Byte_2Time;  //chpark.09.13
    
    //(7)C1P37
    uint16_t  AUX_PIO_01Bytes; //chpark
    uint32_t  AUX_PIO_01Time;  //chpark.07.05
    //(8)C1P38
    uint16_t  AUX_PIO_23Bytes; //chpark
    uint32_t  AUX_PIO_23Time;  //chpark.07.05

    //(9)C1P39
    //uint8_t   OUT1_3Byte_2bit; //chpark
    //uint32_t  OUT1_3Byte_2Time;//chpark.07.05
    uint8_t   OUT1_2Byte_5bit; //chpark.09.13
    uint32_t  OUT1_2Byte_5Time;//chpark.09.13

    //(10)C1P40
    //uint8_t   OUT1_2Byte_4bit; //chpark
    //uint32_t  OUT1_2Byte_4Time;//chpark.07.05
    uint8_t   OUT1_2Byte_0bit;   //chpark
    uint32_t  OUT1_2Byte_0Time;  //chpark.07.05

    //(11)C1P41
    uint16_t  AUX4_IO_23Bytes; //chpark
    uint32_t  AUX4_IO_23Time;  //chpark.07.05
    //(12)C1P42
    uint16_t  AUX4_IO_45Bytes; //chpark
    uint32_t  AUX4_IO_45Time;  //chpark.07.05

    //(13)C1P43
    uint8_t   OUT1_1Byte_4bit;   //chpark
    uint32_t  OUT1_1Byte_4Time;  //chpark.07.05
    
    //(14)C1P44
    uint8_t   OUT1_2Byte_6bit;   //chpark
    uint32_t  OUT1_2Byte_6Time;  //chpark.07.05

    //(15)C1P45
    uint8_t   OUT1_0Byte_5bit;   //chpark
    uint32_t  OUT1_0Byte_5Time;  //chpark.07.05
    
    //(16)C1P46
    //uint8_t   OUT1_2Byte_0bit;   //chpark
    //uint32_t  OUT1_2Byte_0Time;  //chpark.07.05
    uint8_t   OUT1_2Byte_4bit;   //chpark.09.13
    uint32_t  OUT1_2Byte_4Time;  //chpark.09.13

    //(17)C1P47
    uint16_t  AUX1_IO_23Bytes; //chpark
    uint32_t  AUX1_IO_23Time;  //chpark.07.05
    //(18)C1P48
    uint16_t  AUX1_IO_45Bytes; //chpark
    uint32_t  AUX1_IO_45Time;  //chpark.07.05
    //---------------------------------------
    //Output2
    //(1)C1P49
    uint16_t  AUX2_IO_23Bytes; //chpark
    uint32_t  AUX2_IO_23Time;  //chpark.07.05
    //(2)C1P50
    uint16_t  AUX2_IO_45Bytes; //chpark
    uint32_t  AUX2_IO_45Time;  //chpark.07.05
    //(3)C2P31
    //uint16_t  OUT14_01Bytes; //chpark
    //uint32_t  OUT14_01Time;  //chpark.07.05
    uint16_t  OUT14_01Bytes; //chpark.09.13
    uint32_t  OUT14_01Time;  //chpark.09.13
    uint16_t  OUT10_67Bytes; //chpark.09.13
    uint32_t  OUT10_67Time;  //chpark.09.13
    //(4)C2P32
    //uint16_t  OUT14_23Bytes; //
    //uint32_t  OUT14_23Time;  //chpark.07.05
    uint16_t  OUT14_23Bytes; //chpark.09.13
    uint32_t  OUT14_23Time;  //chpark.09.13
    uint16_t  OUT10_45Bytes; //chpark.09.13
    uint32_t  OUT10_45Time;  //chpark.09.13
    //(5)C2P33
    uint8_t   OUT1_3Byte_4bit;   //chpark
    uint32_t  OUT1_3Byte_4Time;  //chpark.07.05
    uint8_t   OUT1_5Byte_7bit;   //chpark
    uint32_t  OUT1_5Byte_7Time;  //chpark.07.05

    uint8_t   OUT6_5Byte_3bit;   //chpark.24.01.25
    uint32_t  OUT6_5Byte_3Time;  //chpark.24.01.25

    uint8_t   OUT6_0Byte_0bit;   //chpark
    uint32_t  OUT6_0Byte_0Time;  //chpark.07.05
    uint8_t   OUT6_0Byte_3bit;   //chpark
    uint32_t  OUT6_0Byte_3Time;  //chpark.07.05
    uint8_t   OUT1_0Byte_3bit;   //chpark
    uint32_t  OUT1_0Byte_3Time;  //chpark.07.05
    
    uint16_t  OUT13_45Bytes; //chpark
    uint32_t  OUT13_45Time;  //chpark.07.05
    uint16_t  OUT13_67Bytes; //chpark
    uint32_t  OUT13_67Time;  //chpark.07.05
    uint16_t  OUT13_01Bytes; //chpark
    uint32_t  OUT13_01Time;  //chpark.07.05
    uint16_t  OUT13_23Bytes; //chpark
    uint32_t  OUT13_23Time;  //chpark.07.05

    uint8_t   OUT6_0Byte_4bit;  //chpark
    uint32_t  OUT6_0Byte_4Time; //chpark.07.05
    uint8_t   OUT1_0Byte_4bit;  //chpark
    uint32_t  OUT1_0Byte_4Time; //chpark.07.05
    uint8_t   OUT1_3Byte_7bit;  //chpark
    uint32_t  OUT1_3Byte_7Time; //chpark.07.05

    //--------------------------------------
    //Output3
    uint8_t   OUT6_0Byte_6bit;  //chpark
    uint32_t  OUT6_0Byte_6Time; //chpark.07.05
    uint8_t   OUT6_7Byte_4bit;   //chpark
    uint32_t  OUT6_7Byte_4Time;  //chpark.24.10.18

    uint8_t   OUT1_1Byte_7bit;  //chpark
    uint32_t  OUT1_1Byte_7Time; //chpark.07.05

    uint16_t  OUT10_01Bytes;//chpark
    uint32_t  OUT10_01Time; //chpark.07.05
    uint16_t  OUT10_23Bytes;//chpark
    uint32_t  OUT10_23Time; //chpark.07.05

       //OX module 
    uint16_t  OUT20_01Bytes;//chpark
    uint32_t  OUT20_01Time; //chpark.24.10.18
    uint16_t  OUT20_23Bytes;//chpark
    uint32_t  OUT20_23Time; //chpark.24.10.18

    uint8_t   OUT6_5Byte_6bit;   //chpark
    uint32_t  OUT6_5Byte_6Time;  //chpark.24.10.18
    uint8_t   OUT6_5Byte_7bit;   //chpark
    uint32_t  OUT6_5Byte_7Time;  //chpark.24.10.18
//--------------------------------------
    //Receiving just one opi is the same as receiving all of them.
    uint32_t  OPI_AllByte_Time; //chpark.07.05
    uint32_t  OPI2_AllByte_Time; //chpark.07.05

    uint16_t XO_Test_01Bytes;//for test chpark

//Calibration
    uint16_t TCMD_01Bytes;//chpark. calibaration.
    uint8_t  TCMD_2Byte;
    uint8_t  TCMD_3Byte_47bit;//chpark	
    uint8_t  TCMD_4Byte;	

}MB_IO_Value; 

typedef struct _AUX1Preset 
{
	uint8_t  	Percent1st[6];
	uint8_t  	Percent2nd[4];
	uint8_t  	OMD[3];			// Out Mode
	uint8_t  	ATTIcon[3]; 	// 0~8
	uint8_t 	No;				// ?��?��?��?�� ?���? 0-2
	uint16_t  	MPA[3];
} AUX1Pre; 

typedef struct _AUX1Value 
{
	AUX1Pre  Pre[4];	// ?��?��?���? ?��?�� �?
	uint8_t  AB_Min;
	uint8_t  AB_Max;
	uint16_t  MPA_Min;
	uint16_t  MPA_Max;
	uint16_t  AUXUL5_1st[4][6];
	uint16_t  AUXUL5_2nd[4][6];
	uint16_t  AUXUL5_3rd[4][6];
	uint32_t  Time1;
	uint32_t  Time2;
} AUX1Value; 

typedef struct _ErrorValue 
{
	uint16_t  	EMR[4];
	uint32_t  	ECM_S[3];
	uint8_t  	ECM_F[3];
	uint8_t  	ECM_CNT;
} ErrorValue; 

typedef struct _CamColor 
{
	int8_t 	Brightness;
	uint8_t Contrast;
	uint8_t Saturation;
	int8_t 	Hue;
	uint8_t Sharpness;
} CamColor; 



extern bool gOnFlag_1S;
extern bool gOnFlag_500ms;
extern bool gOnFlag_300ms;


//extern uint8_t gMachine_PresetNo;	// 0~3
extern uint8_t gMachine_PresetNo2;	// 0~3
extern uint8_t ModelType;
extern bool Main_CoolantOrange;


extern uint8_t RotationMode;
extern uint32_t RotationModeTime;

extern uint8_t MainBtFlag;

extern uint8_t SubModelType;

extern uint8_t Main_WEngStopSw;
extern uint8_t Main_WSafetyStart;

extern uint8_t Phone0Byte;
extern uint8_t PowerBtFlag;

extern uint8_t Main_wZoomIndex;
extern uint8_t Main_fZoomIndex;

extern uint32_t Main_wZoomIndex_Time;		// JPHong 2023-10-12
extern uint32_t Main_wZoomIndex_Time_Disp;		// JPHong 2023-10-24
extern uint32_t Main_fZoomIndex_Time;		// JPHong 2023-10-12
extern uint32_t Main_fZoomIndex_Time_Disp;		// JPHong 2023-10-24


extern uint16_t Main_EI_5BValue;
extern uint32_t Main_EI_5BTime;
extern uint16_t Main_DD_1BValue;
extern uint16_t Main_SI_67BValue;
extern uint32_t Main_SI_67BTime;
extern uint16_t Main_EI_34BValue;
extern uint8_t CLM2_7Byte_5bit;	//0:No change , 1: 1->0(off) ,  2: 0->1(on) 


extern uint8_t Main_CLI2[8];
extern uint8_t Main_CLI3[8];

extern HourMeterValue HMV;
extern MeterRateValue MRV;
extern AirConValue ACV;
extern RadioValue RDV;

extern uint8_t AUI1_0B_3210b;
extern uint8_t UserPW_PassFlag;
extern uint32_t UserPW_Time_UA2;
extern uint32_t UserPW_Time_AUI1;
extern uint32_t Phone_Time;
				
extern uint8_t LogoPW_Index;
extern uint32_t LogoPW_Data;
				
extern uint8_t  gBuzzerMode;	//0:OFF , 1:ON(100)OFF(1900) , 2:ON(200)OFF(800) , 3:ON(500)OFF(500) , 4:ON(300)OFF(300)  , 5:ON , 6:ON(100)OFF(9900)  , 7:ON(100)OFF(100) 3Time   , 10:ON(500)OFF
extern uint32_t gBuzzerTimerCnt_10m;
extern uint32_t gMainTimerCnt_10m;
extern uint32_t UpdateDateCnt;
extern uint32_t CLM1_Time;
extern uint32_t CLM2_Time;
extern uint32_t CLM3_Time;
extern uint32_t CLM4_Time;
extern uint32_t AG_I_Time;
extern uint32_t EI_Time;
extern uint32_t DD_Time;
extern uint32_t SI_Time;
extern uint32_t MI_Time;
extern uint32_t HM_Time;
extern uint32_t M9HM_Time;
extern uint32_t TM1_Time;
extern uint32_t TM2_Time;
extern uint32_t TM3_Time;

extern uint32_t AUX_IO2_Time;
extern uint32_t IN1_Time;
extern uint32_t IN2_Time;
extern uint32_t OUT1_Time;
extern uint32_t OUT6_Time;
extern uint32_t IN17_Time;
extern uint32_t IN3_Time;
extern uint32_t IN4_Time;
extern uint32_t IN5_Time;
extern uint32_t IN6_Time;
extern uint32_t IN7_Time;
extern uint32_t IN9_Time;
extern uint32_t IN13_Time;
extern uint32_t IN23_Time;
extern uint32_t OUT2_Time;
extern uint32_t OUT3_Time;
extern uint32_t OUT4_Time;
extern uint32_t OUT8_Time;
extern uint32_t AUX1_Time;
extern uint32_t AUX2_Time;
extern uint32_t MTSPD_Time;
extern uint32_t OUT5_Time;


extern uint8_t	KeyPressCnt; 


extern uint16_t gEMR[4];			// for Error Display , DMM

extern uint32_t gSPN_sECU;			// single ECU ErrorCode
extern uint8_t gFMI_sECU;
extern uint32_t gSPN_sDCU;			// single DCU ErrorCode
extern uint8_t gFMI_sDCU;
extern uint32_t gSPN_ECU[3];		// Engine ECU ErrorCode
extern uint8_t gFMI_ECU[3];
extern uint32_t gSPN_DCU[3];		// Engine DCU ErrorCode	
extern uint8_t gFMI_DCU[3];

extern uint32_t gSPN[3];			// for Error Display
extern uint8_t gFMI[3];				// for Error Display

extern uint8_t gACERR_Code[8];

extern uint32_t gEMR_Time[5];	// 0.0 ~ 99999.9hr (0.1hr/bit)CAN[P_DTCU] 0x18503FE4
extern uint16_t gEMR_Past[5];	//
extern uint8_t gPastCar_UserIdx[5];

extern uint32_t gSPN_sPast;					// single Past Eng ErrorCode CAN[PEEI] 0x1850EAE4
extern uint8_t gFMI_sPast;
extern uint32_t gSPN_Past[4];		// Multi Past Eng ErrorCode
extern uint8_t gFMI_Past[4];

extern uint32_t gPastEng_Time[4];			// for Past ENG Error Meter Display
extern uint8_t gPastEng_UserIdx[4];

extern DATAValue DTVal;
extern MB_IO_Value gMB_IO_Value;//chpark.

extern uint8_t gUserPW[4];
extern uint8_t gPW_NextMode;	// 	0:NG ,11:1st AUX  ,12:2nd /4th   ,13:LOCK ,14:PASSWORD
extern bool gAutoGreaseFlag;	// 0:off , 1:on
extern uint8_t MeterLiftLdFlag;	// 0:off , 1:on 

extern AUX1Value AUX1Val;
extern uint8_t SIDEAUX_Machine;		// 0~8
extern uint8_t AutoTankDispFlag;
extern uint8_t PressureDispFlag;


extern uint8_t AUX1CurrentOutMode;
extern uint8_t DTM_DateTime_Mode;
extern uint8_t BG_Color;
extern uint8_t LCD_BL_Day;
extern uint8_t LCD_BL_Night;
extern uint8_t SIUS_SEL_Flag;
extern uint8_t Main_DayNight;


extern CamColor CC1;
extern CamColor CC2;



extern uint8_t Fn_Cursor;
extern uint8_t Fn_ModeDepth;
extern uint8_t gCameraModeDisp;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
extern uint8_t gCameraStatus0;	// 
extern uint8_t gCameraLock0;	// 0:off , 1:lock
extern uint8_t gCameraStatus1;	// 
extern uint8_t gCameraLock1;	// 0:off , 1:lock
extern uint8_t MenuCameraMode;
extern uint8_t gCreepMode;	// 0:off , 1:on   7bit : OPI2  , 6bit : CLM2
extern uint8_t gCreepValue;
extern ErrorValue	ErVal; 




extern uint8_t OPI_5Byte_45bit;	//Active power control 0 off  
extern uint8_t OPI2_1Byte_45bit;	//Two-Speed Travel with Automatic Shift  0 off 
extern uint8_t CPI_0Byte;
extern uint8_t TempCPI_0Byte;
extern uint8_t OPI3Byte23bit;
extern uint8_t OPI1Byte67bit;
extern uint8_t OPI2Byte45bit;
extern uint8_t OPI0Byte67bit;
extern uint8_t OPI3Byte01bit;

extern uint8_t OPI3Byte45bit;
extern uint8_t OPI2Byte67bit;
extern uint8_t OPI1Byte45bit;
extern uint8_t OPI0Byte45bit;
extern uint8_t OPI1Byte01bit;

extern uint8_t OPI2Byte23bit;
extern uint8_t OPI0Byte01bit;
extern uint8_t OPI4Byte01bit;
extern uint8_t OPI4Byte45bit;
extern uint8_t OPI2Byte01bit;

extern uint8_t OPI4Byte67bit;
extern uint8_t OPI6Byte45bit;
extern uint8_t OPI5Byte23bit;
extern uint8_t OPI21Byte45bit;
extern uint8_t OPI24Byte01bit;

extern uint8_t Func_APC_Flag;	//Active power control 0off  
extern uint8_t Func_ACM_Flag;	//Auto camera mode  0off  
extern uint8_t Func_TSTAS_Flag;	//Two-Speed Travel with Automatic Shift  0 off 
extern uint8_t Func_Creep_Flag;	//CreepMode  0 off 

extern uint8_t Lock_1st_Flag;	// 0:off , 1:on
extern uint8_t Lock_2nd_Flag;	// 0:off , 1:on

extern bool gMainScreenMode;
extern bool Main_CanCamOn;
extern uint8_t gRead_Gpio_Value;
extern bool EEPROM_Lock;
extern bool EEPROM_LockMode;
extern uint8_t EEPROM_LockPass;

extern uint8_t AUX1st_LockMode;
extern uint8_t AUX2nd_LockMode;


extern uint8_t AG_ForecedSuppy;
extern uint8_t AG_Time;						// modify uint32_t -> uint8_t (min) for AG_S
extern uint8_t AG_Max;
extern uint8_t AG_Min;
extern bool AG_Start;
extern bool AG_Stop;
extern uint8_t S4_2SecFlag;

extern uint8_t Main_DriveMode;		// 0:굴착  , 1:Drive

extern uint8_t SDLK_LONG_FLAG;

extern uint8_t Main_Select_Index;	// 0:off , 1:Warring(x)  , 2:ATT  , 3:AirCon  , 4:Radio  , 5:HourMeter  , 6:Bar  , 7:TimeDate(x)
extern uint8_t Main_Select_Index_OnOff;
extern uint8_t MainMode_AC_Select_Index;	// 0-6
extern uint8_t MainMode_RD_Select_Index;
extern uint8_t MainMode_DAB_Select_Index;
extern uint8_t MainMode_DAB2_Select_Index;
extern uint8_t MainMode_BT_Select_Index;
extern uint8_t MainMode_Phone_Select_Index;

extern uint8_t Main_Disp_Mode;		// 0:Normal , 1:AirCon  , 2:Radio  , 3:DAB1  , 4:DAB2  , 5:Bluetooth  , 6:Phone  , 7:Camera
extern uint8_t Main_Disp_Mode_Flag;	// 0:Normal , 1:Changing
extern bool MainLayerFlag;
extern bool MainLayerFlag4ZommIcon;


extern uint8_t PL_Flag_New_Value;
extern uint8_t PL_Flag_Manual;


extern uint8_t AC_SEL_DIR_LR_FLAG;
extern uint8_t AC_SEL_TEMP_LR_FLAG;
extern uint8_t AC_SEL_LEVEL_LR_FLAG;

extern uint32_t AC_SEL_DIR_R_CNT;
extern uint32_t AC_SEL_TEMP_R_CNT;
extern uint32_t AC_SEL_LEVEL_R_CNT;
extern uint32_t AC_SEL_DIR_L_CNT;
extern uint32_t AC_SEL_TEMP_L_CNT;
extern uint32_t AC_SEL_LEVEL_L_CNT;

extern bool MainMode_Bluetooth_Pairing_Flag;


extern uint8_t RD_SEL_TUNE_LR_FLAG;
extern uint8_t RD_SEL_TONE_DU_FLAG;
extern uint8_t RD_SEL_VOL_DU_FLAG;
extern uint32_t RD_SEL_TUNE_L_CNT;
extern uint32_t RD_SEL_TONE_D_CNT;
extern uint32_t RD_SEL_VOL_D_CNT;
extern uint32_t RD_SEL_TUNE_R_CNT;
extern uint32_t RD_SEL_TONE_U_CNT;
extern uint32_t RD_SEL_VOL_U_CNT;

extern uint32_t Media_RunTime_Cnt;
extern uint32_t PH_RunTime_Cnt;


#define MAX_DAB3LIST_COUNT 	500
#define MAX_DAB3LIST_CHAR 	80
#define MAX_DAB3LIST_CHAR_LIMIT 50
extern char DAB3PTYNo[MAX_DAB3LIST_COUNT][MAX_DAB3LIST_CHAR];
//chpark.07.18
//for MAX 20 character(H-mode)
//for MAX 30 character(V-mode)
extern char DAB3PTYNo_LIMIT[MAX_DAB3LIST_COUNT][MAX_DAB3LIST_CHAR_LIMIT];



//extern void HT_Save_Config(bool SaveFlag);

extern uint8_t RotationDir(void);

// use menu 2
extern uint32_t Trip1_Value;		// CAN[TM1] 0-3byte
extern uint32_t Trip2_Value;		// CAN[TM1] 4-7byte
extern uint32_t Trip3_Value;		// CAN[TM2] 0-3byte
extern uint32_t Trip4_Value;		// CAN[TM2] 4-7byte
extern uint32_t Trip5_Value;		// CAN[TM3] 0-3byte
extern uint32_t Trip6_Value;		// CAN[TM3] 4-7byte

// use menu 3
extern uint8_t AUX_A_Value,AUX_B_Value;
extern uint8_t AUX_C_Value,AUX_D_Value;
extern uint8_t AUX_G_Value,AUX_H_Value;

extern uint32_t AUX_Value_CNT;

// use menu4
extern uint8_t M4_BGMode;					// BG_Color 				// BG_BLUE:1 , BG_BLACK:2
extern uint8_t M4_UnitFlag;				// SIUS_SEL_Flag		// 0:SI , 1:US

// use menu 5
extern uint16_t M5_RPM;
extern uint8_t M5_WTemp;
extern uint8_t M5_OTemp;
extern uint8_t M5_UTemp;
extern uint16_t M5_Fuel;
extern uint16_t M5_Urea;
extern uint32_t M5_Batt;
extern uint8_t M5_PM;
extern uint8_t M5_ASH;

extern uint32_t M5_RPM_Timer;
extern uint32_t M5_WTemp_Timer;
extern uint32_t M5_OTemp_Timer;
extern uint32_t M5_UTemp_Timer;
extern uint32_t M5_Fuel_Timer;
extern uint32_t M5_Urea_Timer;
extern uint32_t M5_Batt_Timer;
extern uint32_t M5_PM_Timer;
extern uint32_t M5_ASH_Timer;

// menu7
extern uint8_t M7_AG_OptFlag;		// CAN[OPI] 7byte,2~3bit
extern uint8_t M7_AP_OptFlag;		// CAN[FNI1] 6byte,4~5bit 0:None Opt , 1:On Opt , 2:Off Opt(Hold) , 3:No Visual

// menu7 sub1
typedef struct UserPresetData
{
	uint8_t A_Value;				// AUX1	1way flow rate
	uint8_t B_Value;				// AUX1	2way flow rate
	uint8_t P1_Value;			// AUX1	Pressure
	uint8_t ATT_State;
	uint8_t Way_Opt;
}UserPresetData;

extern uint8_t M7_UserIndex;			// 0 ~ 9 ; 10user
extern uint8_t M7_UserIndexEx;


extern uint8_t S1_AUX1_FlowRate_Max;			// CAN[AUX_U/L] 0byte
extern uint8_t S1_AUX1_FlowRate_Min;			// CAN[AUX_U/L] 1byte
extern uint8_t S1_AUX1_Pressure_Max;			// CAN[AUX_PU/L] 0byte
extern uint8_t S1_AUX1_Pressure_Min;			// CAN[AUX_PU/L] 1byte

extern uint8_t S1_1WAY_1st_X[6];			// CAN[AUX_UL5] 0byte 0 , 1 ~ 6byte
extern uint8_t S1_1WAY_1st_Y[6];			// CAN[AUX_UL4] 0byte 1 , 1 ~ 6byte
extern uint8_t S1_1WAY_2nd_X[6];			// CAN[AUX_UL5] 0byte 2 , 1 ~ 6byte
extern uint8_t S1_1WAY_2nd_Y[6];			// CAN[AUX_UL4] 0byte 3 , 1 ~ 6byte

extern uint8_t M7_P1_OPI_Flag;				// CAN[OPI] 6byte 0~1bit; 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
extern uint8_t M7_Way_OPI_Flag;				// CAN[OPI] 0byte 2~3bit; 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
extern uint32_t M7_Way_Timer;					// add 230615 jhlee
extern uint32_t M7_P1_Timer;					// add 230615 jhlee

// menu7 sub2
typedef struct UserAUX24Data
{
	uint8_t C_Value;				// AUX2	1way flow rate
	uint8_t D_Value;				// AUX2	2way flow rate
	uint8_t P2_Value;			// AUX2 Pressure
	uint8_t G_Value;				// AUX4 1way flow rate
	uint8_t H_Value;				// AUX4 2way flow rate
	uint8_t P4_Value;			// AUX4 Pressure
}UserAUX24Data;

extern uint8_t S2_AUX2_FlowRate_Max;		// CAN[AUX_U/L] 2byte
extern uint8_t S2_AUX2_FlowRate_Min;		// CAN[AUX_U/L] 3byte
extern uint8_t S2_AUX2_Pressure_Max;		// CAN[AUX_PU/L] 2byte
extern uint8_t S2_AUX2_Pressure_Min;		// CAN[AUX_PU/L] 3byte

extern uint8_t S2_AUX4_FlowRate_Max;		// CAN[AUX_U/L] 4byte
extern uint8_t S2_AUX4_FlowRate_Min;		// CAN[AUX_U/L] 5byte
extern uint8_t S2_AUX4_Pressure_Max;		// CAN[AUX_PU/L] 4byte
extern uint8_t S2_AUX4_Pressure_Min;		// CAN[AUX_PU/L] 5byte

extern uint8_t S1_2WAY_X[6];			// CAN[AUX_UL5] 1 ~ 6byte
extern uint8_t S1_2WAY_Y[6];			// CAN[AUX_UL4] 1 ~ 6byte
extern uint8_t S1_4WAY_X[6];			// CAN[AUX_UL5] 1 ~ 6byte
extern uint8_t S1_4WAY_Y[6];			// CAN[AUX_UL4] 1 ~ 6byte

extern uint8_t M7_P2_OPI_Flag;					// CAN[OPI] 6byte 2~3bit; 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
extern uint8_t M7_P4_OPI_Flag;					// CAN[OPI] 0byte 6~7bit (AUX4); 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
extern uint8_t M7_P24_OPI_Flag;					// CAN[OPI] 7byte 6~7bit (AUX2/4); 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL

// menu7 sub5
#define Lang_ENG		0
#define Lang_DEU		1
#define Lang_FRA		2
#define Lang_ITA		3
#define Lang_ESP		4

// menu7 sub6
typedef struct _UserFunc
{
	uint8_t AutoDecel;						// CAN[OPI2] 1byte 0-1 bit
	uint8_t AutoCamera;						// CAN[OPI2] 0byte 2-3 bit
	uint8_t AutoStop[MAX_SU100_USER_NUM];	// CAN[OPI] 6byte 4-5 bit 
	uint8_t AutoStopKey;					// CAN[OPI] 6byte 4-5 bit 
	uint8_t Rotate_Flag;					// CAN[OPI2] 1byte 2-3 bit
}UserFunc;

extern UserFunc Option_Function;		// [user_index] //chpark.08.22

// menu7 sub4 ; Auto greese
extern uint8_t AG_OnTime;					// CAN[AG_I] 4byte
extern uint8_t AG_ActiveStatus;		// CAN[AG_I] 0byte,0bit 0:None opt , 1:On Opt
extern uint8_t AG_SupplyFlag;			// CAN[AG_I] 1byte,0bit 0:No Supply(STOP) , 1:Force Supply(WORK)
extern uint8_t AG_ErrorStatus;		// CAN[AG_I] 1byte,6bit 0:Normal , 1:Error
extern uint8_t AG_MaxLimit;				// CAN[AG_I] 2byte
extern uint8_t AG_MinLimit;				// CAN[AG_I] 3byte

// menu7 sub9 ; User Config
//chpark.08.22 : New User Config
typedef struct _UserRegister
{
    bool    bRegister;                  // Registered user //chpark.11.02
	uint8_t RegKeyNum;                  // 0~14 Key value
	uint8_t ShowUserList;				// 0:No Show 1:Show
	uint8_t RegFlag;					// 0;Not Register ,1;Admin ,2;Standard ,3;N/A
	uint8_t PassNOD;				    // PassCode number of digit //chpark.08.22
	uint8_t PassDigit;					// PassCode Digit
	uint8_t RFStatus;					// RFID Register Status
	uint8_t MacStatus;				 	// BT MAC ID Register status
	
	uint8_t UserName[MAX_SU100_USER_NAME];			// username 10digit
	uint8_t UserPassCode[MAX_SU100_USER_PW];		// user passcode 10digit(Max 10digit,Min 4digit)
	uint8_t UserMacID[MAX_SU100_USER_MID];								// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	
	bool    bDelSelect;         //Icon Color. chpark.07.18
	uint8_t Language;           //user Language
	
	uint8_t AccessAUX;
	
	uint8_t nTmp_PassCode_Skip;
	uint8_t nTmp_RF_Skip;
	
	uint8_t nTmp_RFStatus;
	uint8_t nTmp_RegFlag;
	uint8_t nTmp_AccessAUX;
	
	bool    bAddProcessing;  

}UserInfo;

uint8_t Device_UID[20]; //chpark.24.05.31
extern UserInfo User_RegInfo[MAX_SU100_USER_NUM];			// register user infomation, system date resume(saved data)
extern bool CanbusTxDataManagement( uint32_t TimerCnt_10ms, uint32_t ID,uint8_t *TxData);
extern bool CalRadioFreq(char *tmp,uint8_t RDMode,uint16_t RDFreq);

// menu9
typedef struct _UserWorkTime
{
	uint8_t tUserNum;
	uint32_t tSrtTime;
	uint32_t tEndTime;
}UserWorkTime;

extern UserWorkTime UserMeter[10];
//extern uint32_t UserMeter_Timer;

extern uint8_t S11_Auto1_Flag;			// FNI1 6byte,6~7bit
extern uint8_t S11_Auto2_Flag;			// FNI1 7byte,0~1bit
extern uint8_t S11_Auto12_Flag;			// FNI1 7byte 2~3bit

extern void GotoMain(uint8_t SubMode);
extern void GotoMainReal(uint8_t SubMode);
extern void DAB2_Set_JD_Button(uint8_t btnIdx);//chpark.09.09

extern uint32_t DAB2_DataChkTime; //chpark.09.25
extern uint32_t DAB3_DataChkTime; //chpark.07.18
extern bool g_bDAB2_DataMode;       //chpark.09.09
extern int  g_DAB2_PreSelect_Index; //chpark.11.15  V<->H Mode change.
extern int  g_DAB3_PreSelect_Index; //chpark.12.05  V<->H Mode change.

extern int  g_DAB2_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.
extern int  g_DAB3_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.
extern int  g_DAB2_H_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.
extern int  g_DAB3_H_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.

extern uint32_t g_nDAB2_Data_LastRecvTime;   //chpark.12.05
extern uint32_t g_nDAB3_Data_LastRecvTime;   //chpark.12.05

//chpark.08.22

extern int g_nSU100_Mode;
extern uint8_t g_nSU100_AddUserIdx;
extern uint8_t g_nSU100_EditUserIdx;
extern uint8_t g_nSU100_CurUserIdx;


#define SU100_Select_Mode			1
#define SU100_Add_Mode				2
#define SU100_Delete_Mode1			3
#define SU100_Delete_Mode2			4
#define SU100_Delete_Mode3			5//chpark.11.20
#define SU100_Edit_Mode				6
#define SU100_Delete_Result_Success 7
#define SU100_Delete_Result_Fail 	8
#define SU100_Delete_Result_Fail_NoAdmin 	9 //chpark.24.03.19

#define SU100_ICON_AD   0
#define SU100_ICON_ST   1


extern int g_nSU100_PreLayer;//

#define M7_SU100_LAYER			    1
#define M7_SU200_EDIT_LAYER			2
#define M7_SU300_NAME_LAYER			3
#define M7_SU400_PASS_LAYER			4
#define M7_SU500_RFID_LAYER			5
#define M7_SU600_LEVEL_LAYER        6


extern void SU100_ISO8859_to_UTF8(uint8_t *tDest,uint8_t *tSrc);
extern bool SU100_Is_Empty_InFlash(uint8_t *tSrc);//chpark.12.13


extern int g_bSU100_TestUserSetting;
extern int g_nSU200_InitButtonIndex;

extern uint8_t SU100_UI1_3Byte;			// CAN[UI1] 3byte //chpark.08.22
extern uint8_t SU100_UI1_4Byte;			// CAN[UI1] 4byte //chpark.08.22

extern uint8_t SU100_UI2_All_Byte[8];	//chpark.08.22
extern uint8_t SU100_UI3_All_Byte[8];	//chpark.08.22
extern uint8_t SU100_UI4_All_Byte[8];	//chpark.08.22
extern uint8_t SU100_UI5_All_Byte[8];	//chpark.08.22

extern uint8_t SU100_UM5_All_Byte[8];	//chpark.08.22
//edit
extern uint8_t  SU200_UM5_1Byte_67bit;		//chpark.08.22
//Name
extern uint8_t  SU300_UM5_2Byte_07bit;		//chpark.08.22
extern uint8_t  SU300_UM5_1Byte_45bit;		//chpark.08.22
//Passcode
extern uint32_t SU400_Passcode_InNumber;
extern uint8_t  SU400_UM5_0Byte_01bit;		//chpark.10.30
//RFID
extern uint8_t  SU500_UM5_0Byte_23bit;		//chpark.08.22
//level
extern uint8_t  SU600_UM5_1Byte_03bit;		//chpark.08.22

//
extern uint32_t g_UM5_ChkTime;  //chpark.08.22
//

extern uint32_t gMenu_UI_Time;	// jhlee 240202

extern uint32_t g_UI1_ChkTime;	//chpark.10.12
extern uint32_t g_UI2_ChkTime;	//chpark.10.12
extern uint32_t g_UI3_ChkTime;	//chpark.10.12
extern uint32_t g_UI4_ChkTime;	//chpark.10.12
extern uint32_t g_UI5_ChkTime;	//chpark.10.12



#define SU100_USER_N_R   0 //not registered user//chpark.24.02.13
#define SU100_USER_DEFAULT 1 //Default User No.1 No.2 //chpark.11.02
#define SU100_USER_SPAD 2 //Super plus admin
#define SU100_USER_SAD  3 //Super admin
#define SU100_USER_AD   5 //Admin user
#define SU100_USER_ST   9 //Standard user

#define SU100_USER_DEL  10 //User to be deleted
#define SU100_USER_N_A  15  //N/A user //chpark.24.02.13
#define SU100_USER_INIT 20  //Init Setting Value //chpark.24.02.13

extern bool gUI_Server_Mode; //chpark.10.12
extern void SU100_Send_Add_Cancel();//chpark.10.30
extern void DM14_Timer(void);
extern bool MICOM_RxParsing(void);

typedef struct
{
	uint8_t PacketCnt;
	uint8_t DataLenght;
	uint8_t Status;			// 0:no event , 1:process , 2:complete
	uint8_t TPDT[50];
}MainUI;
extern MainUI MUI;

extern uint8_t MicomFirm_UP_Flag;
extern uint8_t MicomFirm_ReciveData;
extern uint32_t MicomFirmDataSize;
extern uint8_t g_nFac_TestModeFlag; //chpark.24.05.08
extern uint8_t g_nTest_Step;//chpark.24.06.03
extern uint8_t gMicomFirmVer[8];

extern uint8_t g_TestMode_Req_Time; //chpark.24.05.27

extern bool g_IM_b_ExistLayer;//chpark.24.08.29
extern uint8_t g_IM_Main_Step;
extern uint8_t g_IM_RD_Select; //1:register 2: delete
extern uint8_t g_IM_YN_Select; //1:yes 2: no

extern void GotoBSImmobilizer();//chpark.24.10.30
extern void MenuIM_RcvData(int nData); //chpark.24.08.29

extern uint32_t g_IM_RecvTime;	//chpark.24.08.29


//chpark.24.09.12
#define PW100_M7_LAYER			    0
#define PW100_1st_LAYER			    1
#define PW100_2nd_LAYER			    2
#define PW100_Lock_LAYER			3
#define PW100_PassSet_LAYER			4
#define PW100_AutoPressure_LAYER	5
#define PW100_BTOP_LAYER	        6 //MenuBTop
#define PW100_BTOP_OPT_LAYER	    7 //MenuBTop //chpark.24.10.22
#define PW100_MAIN_LAYER	        10 //Main //chpark.24.10.15

extern bool g_bPW100_PW_OnOff;
extern int g_nPW100_LayerType;//chpark.24.09.09
extern int g_nPW100_PreLayer;
extern int g_nPW100_NextLayer;


extern uint8_t PW100_PWD[5]; //chpark.24.09.09

#define M7_MODE_PUSH  0
#define M7_MODE_KEY   1
#define M7_MODE_LOCK  2

extern uint8_t g_nM7_StartType; //0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK

extern uint8_t g_TCI_0B;
extern uint16_t g_TCI_1B;
extern uint32_t g_TCI_Time;


//chpark.24.09.12
#define M7_LSTATE_UNLOCK  	0
#define M7_LSTATE_LOCK   	1

#define M7_LSTATE_HIDE 		0
#define M7_LSTATE_SHOW 		1

//layer type setup
#define PW100_MODE_SET     0
#define PW100_MODE_INPUT   1

//layer type setup->password mode setting
#define PW100_PASS_MODE_OFF 0
#define PW100_PASS_MODE_ON  1

typedef struct  _M7_Lock_Data
{
	bool Lock;				// 0:unlock 1:lock 2:none
	bool Show;				// 0:hide / 1:show
}M7_Lock_Data;

extern M7_Lock_Data g_nM7_Lock_Data[3];

extern uint8_t g_PW100_Password_5Byte; //chpark.24.11.11
extern uint8_t g_PW100_Password_6Byte; //chpark.24.11.11
extern void PW100_Save_CLI();//24.11.11
