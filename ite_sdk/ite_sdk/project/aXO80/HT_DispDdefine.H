#include "HT_SC_globale.H"
#include "scene.H"
#include "HT_SaveFlash.H"
#include "HT_Define.H"
#include "HT_globale.h"


typedef enum {
	cW_01_B=0,
	cW_02_B,	//1
	cW_03_B,
	cW_04_B,
	cW_05_B,
	cW_06_B,
	cW_07_B,
	cW_08_B,
	cW_09_B,
	cW_10_B,
	cW_11_B,
	cW_12_B,
	cW_13_B,
	cW_14_B,
	cW_15_B,
	cW_16_B,
	cW_17_B,
	cW_18_B,
	cW_19_B,
	cW_20_B,
	WARNING_ICON_CNT
} DISP_WARNING;
#define cW_EngStopSw	(WARNING_ICON_CNT+1)
#define cW_SafetyStart	(WARNING_ICON_CNT+2)





typedef enum {
	cF_01_B = 41,	//0
	cF_02_B,
	cF_03_B,
	cF_04_B,
	cF_05_B,
	cF_06_B,		//5
	cF_07_B,
	cF_08_B,
	cF_09_B,
	cF_10_B,
	cF_11_B,		//10
	cF_12_B,
	cF_13_B,
	cF_14_B,
	cF_15_B,
	cF_16_B,		//15
	cF_17_B,
	cF_18_B,
	cF_19_B,		//18
	cF_20_B,
	cF_21_B,
	cF_22_B,		//
	
	FUNCTION_ICON_CNT_MAX
} DISP_FUNCTION;

#define	FUNCTION_ICON_CNT	(FUNCTION_ICON_CNT_MAX-cF_01_B)

typedef enum {
	m14_MeterSymbol_EngLd 	= 11,
	m13_MeterSymbol_EngRPM 	= 12,
	m12_MeterSymbol_LiftLd 	= 13,
	m11_MeterSymbol_PM 	= 14,
	m11_MeterSymbol_AHS	= 15
} DISP_METER;




typedef enum {
    LANG_EN 	= 0,
    LANG_DE 	= 1,
    LANG_FR 	= 2,
    LANG_IT 	= 3
} SET_LANG;




extern uint8_t MainWarnigData[WARNING_ICON_CNT];
extern uint8_t MainFunctionData[FUNCTION_ICON_CNT];		//// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요 

bool MainLayer_BGDisp(uint8_t BG_DN,uint8_t BGColor,uint8_t MainMode);

void MeterERpmDisp(DISP_MODE DispMode,int32_t rpm);
bool MainLayer_MeterERpmDisp(DISP_MODE DispMode,int8_t Symbol,int32_t rpm);
void HourMeterDisp(DISP_MODE DispMode,uint8_t Trip,uint32_t HourMeterS);
bool MainLayer_HourMeterDisp(DISP_MODE DispMode,uint8_t Trip,uint32_t HourMeterS,uint32_t TimeCnt,uint8_t OnOff);
bool MainLayer_YearDayDisp(DISP_MODE DispMode,uint16_t Year,uint8_t Mon,uint8_t Day,uint8_t Hour,uint8_t Min,bool ReDisp);
bool MainLayer_MeterSymbolDisp(int DispData,DISP_MODE DispMode);
uint8_t MainLayer_FunctionDisp(int IconIndex,DISP_MODE DispMode);
uint8_t MainLayer_WarningDisp(int IconIndex,DISP_MODE DispMode);
bool MainLayer_ZoomDispW(int IconIndex,DISP_MODE DispMode);
bool MainLayer_ZoomDispF(int IconIndex,DISP_MODE DispMode);
uint8_t MainLayer_WarningDisp2(uint8_t IconIndex,DISP_MODE DispMode);
bool MainOnEnter_1(ITUWidget* widget, char* param);
bool MainOnJD(ITUWidget* widget, char* param);
bool Main_WarningDisp(uint32_t TimeCnt,bool RT_Flag);
bool Main_FunctionDisp(uint32_t TimeCnt,bool RT_Flag);
bool Main_FnModeDisp(uint32_t TimeCnt,bool OnOff);
bool MainLayer_SelBar(uint8_t IncDec,DISP_MODE DispMode);
