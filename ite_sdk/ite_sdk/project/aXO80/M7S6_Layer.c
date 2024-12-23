#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

#define M7S6_AUTO_BTN 0
#define M7S6_DEC_BTN  1
#define M7S6_CAM_BTN  2
#define M7S6_ROT_BTN  3

static ITUWidget* M7S6_Line;
static ITUWidget* M7S6_Model;

static ITUWidget* M7S6_CONT_BTN[4]; //

static int v_M7S6_push_pos[4][2] = {    //x,y 좌표.
	{ 132, 250 },//M7S6_AUTO_BTN
	    { 132, 782 },//M7S6_DEC_BTN
		{0,0},//M7S6_CAM_BTN
		{466, 782}//M7S6_ROT_BTN
};

static int h_M7S6_push_pos[4][2] = {    //x,y 좌표.
	{ 104, 203 },//M7S6_AUTO_BTN
	    { 104, 484 },//M7S6_DEC_BTN
		{0,0},//M7S6_CAM_BTN
		{ 404, 484}//M7S6_ROT_BTN
};

static int v_M7S6_key_pos[4][2] = {    //x,y 좌표.
	{ 132, 250 },//M7S6_AUTO_BTN
	    { 466, 250 },//M7S6_DEC_BTN
		{0,0},//M7S6_CAM_BTN
		{132, 500}//M7S6_ROT_BTN
};

static int h_M7S6_key_pos[4][2] = {    //x,y 좌표.
	{ 104, 203 },//M7S6_AUTO_BTN
	    { 404, 203 },//M7S6_DEC_BTN
		{0,0},//M7S6_CAM_BTN
		{ 704, 203}//M7S6_ROT_BTN
};

void M7S6_InitPosition(int nType);//chpark.24.10.15
void M7S6_MenuTabEnter();//chpark.24.10.15
bool M7S6_JogWheel_KeyMode(char* param); //chpark.24.10.15
bool M7S6_JogWheel_PushMode(char* param);
bool M7S6_JogButton_KeyMode(char* param);
bool M7S6_JogButton_PushMode(char* param);


UserFunc Option_Function;		// [user_index] ; system data resume/save , on/off data ini-file save
//chpark.08.22

typedef struct _cUserFunc
{
	uint8_t AutoDecel;			// CAN[OPI2] 1byte 0-1 bit
	uint8_t AutoCamera;			// CAN[OPI2] 0byte 2-3 bit
	uint8_t AutoStop;				// CAN[OPI] 6byte 4-5 bit 
	uint8_t Rotate_Flag;		// CAN[OPI2] 1byte 2-3 bit
}cUserFunc;


static cUserFunc cUserOpt;
static uint8_t S6_SelectOpt = 0;

static uint32_t S6_SecTimer;
static uint8_t S6_ExitCNT = 0;

uint8_t cUserLang = NULL; // 20240416 add whlim

ITUWidget *S6_1_SEL = NULL; // 20240416 add whlim
ITUWidget *S6_2_SEL = NULL;
ITUWidget *S6_3_SEL = NULL;
ITUWidget *S6_4_SEL = NULL;

ITUWidget *S6_1_ON = NULL; // 20240416 add whlim
ITUWidget *S6_2_ON = NULL;
ITUWidget *S6_3_ON = NULL;
ITUWidget *S6_4_ON = NULL;

ITUWidget *S6_1_OFF = NULL; // 20240416 add whlim
ITUWidget *S6_2_OFF = NULL;
ITUWidget *S6_3_OFF = NULL;
ITUWidget *S6_4_OFF = NULL;

ITUWidget *S6_1_HOLD = NULL; // 20240416 add whlim
ITUWidget *S6_2_HOLD = NULL;
ITUWidget *S6_3_HOLD = NULL;
ITUWidget *S6_4_HOLD = NULL;

void S6_OPT_SelectClear(void);
void S6_OPT_Select(uint8_t tOptNum);
void S6_OPT_OnOff(uint8_t tOptNum,uint8_t tOptOnOff);
void S6_OPT_All_Off(uint8_t tUserLang);

bool M7S6_UserDisplay(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);


bool M7S6_Enter(ITUWidget* widget, char* param)
{
//	cUserOpt = Option_Function;		// JPHong 2024-06-16
	cUserOpt.AutoDecel	= Option_Function.AutoDecel;
	cUserOpt.AutoCamera = Option_Function.AutoCamera;
	
	if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		cUserOpt.AutoStop = Option_Function.AutoStop[M7_UserIndex];
	else
		cUserOpt.AutoStop = Option_Function.AutoStopKey;

	cUserOpt.Rotate_Flag = Option_Function.Rotate_Flag;
	
	cUserLang = Lang_Flag[M7_UserIndex];  // 20240416 add whlim

	SetWidget(S6_1_SEL,	"S6_1_SEL");
	SetWidget(S6_2_SEL,	"S6_2_SEL");
	SetWidget(S6_3_SEL,	"S6_3_SEL");
	SetWidget(S6_4_SEL,	"S6_4_SEL");

	switch(cUserLang)
	{
		case Lang_ENG:
			S6_1_ON = ituSceneFindWidget(&theScene, "S6_1_ON_ENG"); // 20240416 add whlim
			S6_2_ON = ituSceneFindWidget(&theScene, "S6_2_ON_ENG");
			S6_3_ON = ituSceneFindWidget(&theScene, "S6_3_ON_ENG");
			S6_4_ON = ituSceneFindWidget(&theScene, "S6_4_ON_ENG");

			S6_1_OFF = ituSceneFindWidget(&theScene, "S6_1_OFF_ENG"); // 20240416 add whlim
			S6_2_OFF = ituSceneFindWidget(&theScene, "S6_2_OFF_ENG");
			S6_3_OFF = ituSceneFindWidget(&theScene, "S6_3_OFF_ENG");
			S6_4_OFF = ituSceneFindWidget(&theScene, "S6_4_OFF_ENG");

			S6_1_HOLD = ituSceneFindWidget(&theScene, "S6_1_HOLD_ENG"); // 20240416 add whlim
			S6_2_HOLD = ituSceneFindWidget(&theScene, "S6_2_HOLD_ENG");
			S6_3_HOLD = ituSceneFindWidget(&theScene, "S6_3_HOLD_ENG");
			S6_4_HOLD = ituSceneFindWidget(&theScene, "S6_4_HOLD_ENG");
		break;
		case Lang_DEU:
			S6_1_ON = ituSceneFindWidget(&theScene, "S6_1_ON_DEU"); // 20240416 add whlim
			S6_2_ON = ituSceneFindWidget(&theScene, "S6_2_ON_DEU");
			S6_3_ON = ituSceneFindWidget(&theScene, "S6_3_ON_DEU");
			S6_4_ON = ituSceneFindWidget(&theScene, "S6_4_ON_DEU");

			S6_1_OFF = ituSceneFindWidget(&theScene, "S6_1_OFF_DEU"); // 20240416 add whlim
			S6_2_OFF = ituSceneFindWidget(&theScene, "S6_2_OFF_DEU");
			S6_3_OFF = ituSceneFindWidget(&theScene, "S6_3_OFF_DEU");
			S6_4_OFF = ituSceneFindWidget(&theScene, "S6_4_OFF_DEU");

			S6_1_HOLD = ituSceneFindWidget(&theScene, "S6_1_HOLD_DEU"); // 20240416 add whlim
			S6_2_HOLD = ituSceneFindWidget(&theScene, "S6_2_HOLD_DEU");
			S6_3_HOLD = ituSceneFindWidget(&theScene, "S6_3_HOLD_DEU");
			S6_4_HOLD = ituSceneFindWidget(&theScene, "S6_4_HOLD_DEU");		
		break;
		case Lang_FRA:
			S6_1_ON = ituSceneFindWidget(&theScene, "S6_1_ON_FRA"); // 20240416 add whlim
			S6_2_ON = ituSceneFindWidget(&theScene, "S6_2_ON_FRA");
			S6_3_ON = ituSceneFindWidget(&theScene, "S6_3_ON_FRA");
			S6_4_ON = ituSceneFindWidget(&theScene, "S6_4_ON_FRA");

			S6_1_OFF = ituSceneFindWidget(&theScene, "S6_1_OFF_FRA"); // 20240416 add whlim
			S6_2_OFF = ituSceneFindWidget(&theScene, "S6_2_OFF_FRA");
			S6_3_OFF = ituSceneFindWidget(&theScene, "S6_3_OFF_FRA");
			S6_4_OFF = ituSceneFindWidget(&theScene, "S6_4_OFF_FRA");

			S6_1_HOLD = ituSceneFindWidget(&theScene, "S6_1_HOLD_FRA"); // 20240416 add whlim
			S6_2_HOLD = ituSceneFindWidget(&theScene, "S6_2_HOLD_FRA");
			S6_3_HOLD = ituSceneFindWidget(&theScene, "S6_3_HOLD_FRA");
			S6_4_HOLD = ituSceneFindWidget(&theScene, "S6_4_HOLD_FRA");	
		break;
		case Lang_ITA:
			S6_1_ON = ituSceneFindWidget(&theScene, "S6_1_ON_ITA"); // 20240416 add whlim
			S6_2_ON = ituSceneFindWidget(&theScene, "S6_2_ON_ITA");
			S6_3_ON = ituSceneFindWidget(&theScene, "S6_3_ON_ITA");
			S6_4_ON = ituSceneFindWidget(&theScene, "S6_4_ON_ITA");

			S6_1_OFF = ituSceneFindWidget(&theScene, "S6_1_OFF_ITA"); // 20240416 add whlim
			S6_2_OFF = ituSceneFindWidget(&theScene, "S6_2_OFF_ITA");
			S6_3_OFF = ituSceneFindWidget(&theScene, "S6_3_OFF_ITA");
			S6_4_OFF = ituSceneFindWidget(&theScene, "S6_4_OFF_ITA");

			S6_1_HOLD = ituSceneFindWidget(&theScene, "S6_1_HOLD_ITA"); // 20240416 add whlim
			S6_2_HOLD = ituSceneFindWidget(&theScene, "S6_2_HOLD_ITA");
			S6_3_HOLD = ituSceneFindWidget(&theScene, "S6_3_HOLD_ITA");
			S6_4_HOLD = ituSceneFindWidget(&theScene, "S6_4_HOLD_ITA");	
		break;
		case Lang_ESP:
			S6_1_ON = ituSceneFindWidget(&theScene, "S6_1_ON_ESP"); // 20240416 add whlim
			S6_2_ON = ituSceneFindWidget(&theScene, "S6_2_ON_ESP");
			S6_3_ON = ituSceneFindWidget(&theScene, "S6_3_ON_ESP");
			S6_4_ON = ituSceneFindWidget(&theScene, "S6_4_ON_ESP");

			S6_1_OFF = ituSceneFindWidget(&theScene, "S6_1_OFF_ESP"); // 20240416 add whlim
			S6_2_OFF = ituSceneFindWidget(&theScene, "S6_2_OFF_ESP");
			S6_3_OFF = ituSceneFindWidget(&theScene, "S6_3_OFF_ESP");
			S6_4_OFF = ituSceneFindWidget(&theScene, "S6_4_OFF_ESP");

			S6_1_HOLD = ituSceneFindWidget(&theScene, "S6_1_HOLD_ESP"); // 20240416 add whlim
			S6_2_HOLD = ituSceneFindWidget(&theScene, "S6_2_HOLD_ESP");
			S6_3_HOLD = ituSceneFindWidget(&theScene, "S6_3_HOLD_ESP");
			S6_4_HOLD = ituSceneFindWidget(&theScene, "S6_4_HOLD_ESP");	
		break;
	}

	SetWidget(M7S6_CONT_BTN[M7S6_AUTO_BTN],	"S6_1_CONT");
	SetWidget(M7S6_CONT_BTN[M7S6_DEC_BTN],	"S6_2_CONT");
	SetWidget(M7S6_CONT_BTN[M7S6_CAM_BTN],	"S6_3_CONT");
	SetWidget(M7S6_CONT_BTN[M7S6_ROT_BTN],	"S6_4_CONT");
	
	SetWidget(M7S6_Line,	"S6_Line");
	SetWidget(M7S6_Model,	"S6_Model");


	static uint8_t S6_InitFlag=0;
	
	uint8_t i;

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

	//chpark.24.10.15
	//not use #3 
	ituWidgetSetVisible(M7S6_CONT_BTN[M7S6_CAM_BTN], 	false);
	//
	M7S6_InitPosition(g_nM7_StartType);
	//
	S6_OPT_SelectClear();

	S6_OPT_All_Off(cUserLang);	// 20240416 add whlim	// all off(gray out initial)

	if(cUserOpt.AutoStop < 2)
	{
		S6_OPT_Select(1);
		S6_SelectOpt = 1;
	}
	else if(cUserOpt.AutoDecel < 2)
	{
		S6_OPT_Select(2);
		S6_SelectOpt = 2;
	}
	else if(cUserOpt.Rotate_Flag < 2)
	{
		S6_OPT_Select(4);
		S6_SelectOpt = 4;
	}

	S6_OPT_OnOff(1,cUserOpt.AutoStop);	
	S6_OPT_OnOff(2,cUserOpt.AutoDecel);
	//S6_OPT_OnOff(3,cUserOpt.AutoCamera);//chpark.24.10.15
	S6_OPT_OnOff(4,cUserOpt.Rotate_Flag);

	M7S6_UserDisplay();

	S6_SecTimer = gMainTimerCnt_10m;
	S6_ExitCNT = 0;	

    return true;
}

//chpark.24.10.15
void M7S6_InitPosition(int nType)
{
	if(nType == M7_MODE_PUSH)
	{
		ituWidgetSetVisible(M7S6_Line, true);
		ituWidgetSetVisible(M7S6_Model,true);

		if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		{
			//ex)ituWidgetSetPosition(M7S6_CONT_BTN[M7S6_AUTO_BTN],v_M7S6_push_pos[M7S6_AUTO_BTN][0] ,v_M7S6_push_pos[M7S6_AUTO_BTN][1]);
			for(int i=0;i<4;i++)
			{
				ituWidgetSetPosition(M7S6_CONT_BTN[i],v_M7S6_push_pos[i][0] ,v_M7S6_push_pos[i][1]);
			}
		}
		else
		{
			for(int i=0;i<4;i++)
			{
				ituWidgetSetPosition(M7S6_CONT_BTN[i],h_M7S6_push_pos[i][0] ,h_M7S6_push_pos[i][1]);
			}
		}
	}
	else if(nType == M7_MODE_KEY)
	{
		ituWidgetSetVisible(M7S6_Line, false);
		ituWidgetSetVisible(M7S6_Model,false);

		if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		{
			//ex)ituWidgetSetPosition(M7S6_CONT_BTN[M7S6_AUTO_BTN],v_M7S6_push_pos[M7S6_AUTO_BTN][0] ,v_M7S6_push_pos[M7S6_AUTO_BTN][1]);
			for(int i=0;i<4;i++)
			{
				ituWidgetSetPosition(M7S6_CONT_BTN[i],v_M7S6_key_pos[i][0] ,v_M7S6_key_pos[i][1]);
			}
		}
		else
		{
			for(int i=0;i<4;i++)
			{
				ituWidgetSetPosition(M7S6_CONT_BTN[i],h_M7S6_key_pos[i][0] ,h_M7S6_key_pos[i][1]);
			}
		}
	}
	else
	{
		//error
	}
}


bool M7S6_Timer(ITUWidget* widget, char* param)
{
	if((gMainTimerCnt_10m - S6_SecTimer) >= 100)
	{
		if(S6_ExitCNT < 30)
			S6_ExitCNT++;
		if(S6_ExitCNT == 30)
		{
			S6_ExitCNT = 100;
			printf("\r\nM7S6 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}
			
		S6_SecTimer = gMainTimerCnt_10m;
	}

	if(cUserOpt.AutoDecel != Option_Function.AutoDecel)
	{
		cUserOpt.AutoDecel = Option_Function.AutoDecel;
		S6_OPT_OnOff(2,cUserOpt.AutoDecel);

#ifdef USED_SAVE_CONFIG		// JPHong 2024-04-17
				//Option_Function.AutoDecel
//				XO_Save_Config(M7_UserIndex,true,0);
#else
//				HT_Save_Config(1);
#endif
	}

	if(cUserOpt.AutoCamera != Option_Function.AutoCamera)
	{
		cUserOpt.AutoCamera = Option_Function.AutoCamera;
		S6_OPT_OnOff(3,cUserOpt.AutoCamera);

#ifdef USED_SAVE_CONFIG
				//Option_Function.AutoCamera
//				XO_Save_Config(M7_UserIndex,true,0);
#else
//				HT_Save_Config(1);
#endif
	}

	//S6_OPT_OnOff(3,cUserOpt.AutoStop);
	//S6_OPT_OnOff(4,cUserOpt.Rotate_Flag);

	M7S6_UserDisplay();
	
    return true;
}

//chpark.24.10.15
void M7S6_MenuTabEnter()
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	switch (S6_SelectOpt)
	{
		case 1:
		{
			if (cUserOpt.AutoStop < 2)
			{
				if (cUserOpt.AutoStop == 0)
					cUserOpt.AutoStop = 1;
				else if (cUserOpt.AutoStop == 1)
					cUserOpt.AutoStop = 0;

				if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
					Option_Function.AutoStop[M7_UserIndex] = cUserOpt.AutoStop;
				else
					Option_Function.AutoStopKey = cUserOpt.AutoStop;

				S6_OPT_OnOff(1, cUserOpt.AutoStop);

				#ifdef USED_SAVE_CONFIG
					//Option_Function.AutoStop
					XO_Save_Config(M7_UserIndex, true, 0);
				#else
					HT_Save_Config(1);
				#endif
			}
		}
		break;

		case 2:
		{
			uint8_t tAutoDecel1 = 0;

			if (cUserOpt.AutoDecel < 2)
			{
				if (cUserOpt.AutoDecel == 0)
					tAutoDecel1 = 1;
				else if (cUserOpt.AutoDecel == 1)
					tAutoDecel1 = 0;

				// CAN TX CAN[FNS1;0x1850E9CE]: 1F 00 00 00 0X 01 00 00
				txbuffer[0] = 0x1F;
				txbuffer[4] = tAutoDecel1;
				txbuffer[5] = 0x01;
				txbuffer[6] = 0x00;

				#if 0
					PushTxData(FNS1, txbuffer, 8);
				#else
					CanbusTxDataManagement(gMainTimerCnt_10m, FNS1, txbuffer);
				#endif	//JPHong 06-11
			}
		}
		break;

		case 4:
		{
			if (cUserOpt.Rotate_Flag < 2)
			{
				if (cUserOpt.Rotate_Flag == 0)
					cUserOpt.Rotate_Flag = 1;
				else if (cUserOpt.Rotate_Flag == 1)
					cUserOpt.Rotate_Flag = 0;

				Option_Function.Rotate_Flag = cUserOpt.Rotate_Flag;

				S6_OPT_OnOff(4, cUserOpt.Rotate_Flag);

				#ifdef USED_SAVE_CONFIG
					//Option_Function.Rotate_Flag
					XO_Save_Config(M7_UserIndex, true, 0);
				#else
					HT_Save_Config(1);
				#endif
			}
		}
		break;
	}
}


bool M7S6_JogWheel_PushMode(char* param)
{
	uint8_t tJogWheel = 0;

	S6_ExitCNT = 0;	
	tJogWheel = param[0];
	
	printf("S6 JogWheel = %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		S6_OPT_SelectClear();

		if(S6_SelectOpt == 1)
		{
			if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
		}
		else if(S6_SelectOpt == 2)
		{
			if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
		}
		else if(S6_SelectOpt == 4)
		{
			if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
		}

		S6_OPT_Select(S6_SelectOpt);
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		S6_OPT_SelectClear();

		if(S6_SelectOpt == 1)
		{
			if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
		}
		else if(S6_SelectOpt == 2)
		{
			if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;

		}
		else if(S6_SelectOpt == 4)
		{
			if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
		}
		S6_OPT_Select(S6_SelectOpt);
	}

	return true;
}

bool M7S6_JogWheel_KeyMode(char* param)
{
	uint8_t tJogWheel = 0;

	S6_ExitCNT = 0;	
	tJogWheel = param[0];
	
	printf("S6 JogWheel = %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		S6_OPT_SelectClear();

		if(S6_SelectOpt == 1)
		{
			if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			
		}
		else if(S6_SelectOpt == 2)
		{
			if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
		}
		else if(S6_SelectOpt == 4)
		{
			if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
		}

		S6_OPT_Select(S6_SelectOpt);
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		S6_OPT_SelectClear();

		if(S6_SelectOpt == 1)
		{
			if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
		}
		else if(S6_SelectOpt == 2)
		{
			if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
			else if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;

		}
		else if(S6_SelectOpt == 4)
		{
			if(cUserOpt.AutoStop < 2)
				S6_SelectOpt = 1;
			else if(cUserOpt.AutoDecel < 2)
				S6_SelectOpt = 2;
			else if(cUserOpt.Rotate_Flag < 2)
				S6_SelectOpt = 4;
		}
		S6_OPT_Select(S6_SelectOpt);
	}

	return true;
}


bool M7S6_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;

	S6_ExitCNT = 0;	
	tJogWheel = param[0];
	
	printf("S6 JogWheel = %d\r\n",tJogWheel);

	if(g_nM7_StartType == M7_MODE_PUSH)
	{ 
		M7S6_JogWheel_PushMode(param);
		return true;
	}
	else if(g_nM7_StartType == M7_MODE_KEY)
	{
		M7S6_JogWheel_KeyMode(param);
		return true;
	}

    return true;
}


bool M7S6_JogButton_PushMode(char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	uint8_t tJogButton = 0;
	
	S6_ExitCNT = 0;	
	tJogButton = param[0];
	switch (tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			if (S6_SelectOpt == 1)
			{
				if (cUserOpt.AutoDecel < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(2);
					S6_SelectOpt = 2;
				}
			}
			else if (S6_SelectOpt == 2)
			{
				if (cUserOpt.AutoStop < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(1);
					S6_SelectOpt = 1;
				}
			}
			else if (S6_SelectOpt == 4)
			{
				if (cUserOpt.AutoStop < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(1);
					S6_SelectOpt = 1;
				}
			}
		}
		break;

		case JOG_DIAL_KEY_DOWN:
		{
			if (S6_SelectOpt == 1)
			{
				if (cUserOpt.AutoDecel < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(2);
					S6_SelectOpt = 2;
				}
			}
			else if (S6_SelectOpt == 2)
			{
				if (cUserOpt.AutoStop < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(1);
					S6_SelectOpt = 1;
				}
			}
		}
		break;

		case JOG_DIAL_KEY_LEFT:
		{
			if (S6_SelectOpt == 1)
			{
				//not working
			}
			else if (S6_SelectOpt == 2)
			{
				if (cUserOpt.Rotate_Flag < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(4);
					S6_SelectOpt = 4;
				}
			}
			else if (S6_SelectOpt == 4)
			{
				if (cUserOpt.AutoDecel < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(2);
					S6_SelectOpt = 2;
				}
			}
		}
		break;

		case JOG_DIAL_KEY_RIGHT:
		{
			if (S6_SelectOpt == 1)
			{
				//not working
			}
			else if (S6_SelectOpt == 2)
			{
				if (cUserOpt.Rotate_Flag < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(4);
					S6_SelectOpt = 4;
				}
			}
			else if (S6_SelectOpt == 4)
			{
				if (cUserOpt.AutoDecel < 2)
				{
					S6_OPT_SelectClear();
					S6_OPT_Select(2);
					S6_SelectOpt = 2;
				}
			}
		}
		break;

		//case JOG_DIAL_KEY_ENTER_ON:
		case JOG_DIAL_KEY_ENTER_OFF://chpark.24.10.15
		{
			M7S6_MenuTabEnter();
		}
		break;

		//
		//break;

		default:
			break;
	}

	return true;
}


bool M7S6_JogButton_KeyMode(char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	uint8_t tJogButton = 0;
	
	S6_ExitCNT = 0;	
	tJogButton = param[0];

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		{
			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					if(S6_SelectOpt == 1)
					{
						if (cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}			
					}
					else if(S6_SelectOpt == 2)
					{
						//to do nothing 		
					}
					else if(S6_SelectOpt == 4)
					{
						if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}				
					}
				}
				break;
				
				case JOG_DIAL_KEY_DOWN:
				{
					if(S6_SelectOpt == 1)
					{
						if (cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}						
					}
					else if(S6_SelectOpt == 2)
					{
						if (cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}							
					}
					else if(S6_SelectOpt == 4)
					{
						if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}						
					}
				}
				break;

				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
				{
					if(S6_SelectOpt == 1)
					{
						if(cUserOpt.AutoDecel < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(2);
							S6_SelectOpt = 2;
						}
					}
					else if(S6_SelectOpt == 2)
					{
						if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}		
					}
					else if(S6_SelectOpt == 4)
					{
						
					}
				}
				break;
			
				//case JOG_DIAL_KEY_ENTER_ON:
				case JOG_DIAL_KEY_ENTER_OFF://chpark.24.10.15				
				{
					M7S6_MenuTabEnter();
				}
				break;
			}
		}
		break;
	
		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
				{
					//to do nothing
				}
				break;
				
				case JOG_DIAL_KEY_LEFT:
				{
					if(S6_SelectOpt == 1)
					{
						if(cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}
						else if(cUserOpt.AutoDecel < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(2);
							S6_SelectOpt = 2;
						}
					}
					else if(S6_SelectOpt == 2)
					{
						if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}	
						else if(cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}
						
					}
					else if(S6_SelectOpt == 4)
					{
						if(cUserOpt.AutoDecel < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(2);
							S6_SelectOpt = 2;
						}
						else if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}	
					}
				}
				break;
				
				case JOG_DIAL_KEY_RIGHT:
				{
					if(S6_SelectOpt == 1)
					{
						if(cUserOpt.AutoDecel < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(2);
							S6_SelectOpt = 2;
						}
						else if(cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}
					}
					else if(S6_SelectOpt == 2)
					{
						if(cUserOpt.Rotate_Flag < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(4);
							S6_SelectOpt = 4;
						}
						else if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}	
					}
					else if(S6_SelectOpt == 4)
					{
						if(cUserOpt.AutoStop < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(1);
							S6_SelectOpt = 1;
						}	
						else if(cUserOpt.AutoDecel < 2)
						{
							S6_OPT_SelectClear();
							S6_OPT_Select(2);
							S6_SelectOpt = 2;
						}
					}
				}
				break;
			
				//case JOG_DIAL_KEY_ENTER_ON:
				case JOG_DIAL_KEY_ENTER_OFF://chpark.24.10.15
				{
					M7S6_MenuTabEnter();
				}
				break;
			}
		}
		break;
	}

	return true;
}


bool M7S6_JogButton(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	uint8_t tJogButton = 0;
	
	S6_ExitCNT = 0;	
	tJogButton = param[0];
	
	printf("S6 JogButton = %d\r\n",tJogButton);

	if(g_nM7_StartType == M7_MODE_PUSH)
	{ 
		M7S6_JogButton_PushMode(param);
		return true;
	}
	else if(g_nM7_StartType == M7_MODE_KEY)
	{
		M7S6_JogButton_KeyMode(param);
		return true;
	}

    return true;
}

bool M7S6_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;

	S6_ExitCNT = 0;	
	tJogSwitch = param[0];
	
	printf("M7S6_JogSwitch = %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF) //JPHong 2024-03-22
		GotoMenu7();
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
		GotoMain(MAIN_MODE_NORMAL);
    return true;
}

bool S6D7_BTNMouseUp(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	S6_ExitCNT = 0;	

	switch(S6_SelectOpt)
	{
		case 1:
		{
			if(cUserOpt.AutoStop < 2)
			{
				if(cUserOpt.AutoStop == 0)
					cUserOpt.AutoStop = 1;
				else if(cUserOpt.AutoStop == 1)
					cUserOpt.AutoStop = 0;

				if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
					Option_Function.AutoStop[M7_UserIndex] = cUserOpt.AutoStop;
				else
					Option_Function.AutoStopKey = cUserOpt.AutoStop;

				S6_OPT_OnOff(1,cUserOpt.AutoStop);

#ifdef USED_SAVE_CONFIG
				//Option_Function.AutoStop
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
			}
		}
		break;
		
		case 2:
		{
			uint8_t tAutoDecel = 0;
			
			if(cUserOpt.AutoDecel < 2)
			{
				if(cUserOpt.AutoDecel == 0)
					tAutoDecel = 1;
				else if(cUserOpt.AutoDecel == 1)
					tAutoDecel = 0;

				// CAN TX CAN[FNS1;0x1850E9CE]: 1F 00 00 00 0X 01 00 00
				txbuffer[0] = 0x1F;
				txbuffer[4] = tAutoDecel;
				txbuffer[5] = 0x01;
				txbuffer[6] = 0x00;
				
				#if 0
					PushTxData (FNS1,txbuffer,8);
				#else
					CanbusTxDataManagement( gMainTimerCnt_10m,FNS1,txbuffer);
				#endif	//JPHong 06-11
			}
		}
		break;
		
		case 4:
		{
			if(cUserOpt.Rotate_Flag < 2)
			{
				if(cUserOpt.Rotate_Flag == 0)
					cUserOpt.Rotate_Flag = 1;
				else if(cUserOpt.Rotate_Flag == 1)
					cUserOpt.Rotate_Flag = 0;

				Option_Function.Rotate_Flag = cUserOpt.Rotate_Flag;

				S6_OPT_OnOff(4,cUserOpt.Rotate_Flag);

#ifdef USED_SAVE_CONFIG
				//Option_Function.Rotate_Flag
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
			}
		}
		break;
	}
	
	// system value resume/save
	
    return true;
}

bool S6D6_BTNMouseUp(ITUWidget* widget, char* param)
{
	
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool S6D5_BTNMouseUp(ITUWidget* widget, char* param)
{
	
	GotoMenu7();
	
    return true;
}

bool S6_BTN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tOptNum = 0;
	
	S6_ExitCNT = 0;	
	tOptNum = (uint8_t)atoi(param);

	printf("[S6_BTN_MouseDn]tOptNum=%d\n",tOptNum);
	
	switch(tOptNum)
	{
		case 1:
		{
			if(cUserOpt.AutoStop < 2)
			{
				S6_OPT_SelectClear();
				S6_OPT_Select(1);
				S6_SelectOpt = 1;
			}
		}	
		break;
		case 2:
		{
			if(cUserOpt.AutoDecel < 2)
			{
				S6_OPT_SelectClear();
				S6_OPT_Select(2);
				S6_SelectOpt = 2;
			}
		}	
		break;
		
		case 4:
		{
			printf("[S6_BTN_MouseDn]tOptNum=%d cUserOpt.Rotate_Flag:%d\n",tOptNum,cUserOpt.Rotate_Flag);
			if(cUserOpt.Rotate_Flag < 2)
			{
				S6_OPT_SelectClear();
				S6_OPT_Select(4);
				S6_SelectOpt = 4;
			}
		}	
		break;
	}
	
    return true;
}

bool S6_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	uint8_t tOptNum = 0;
	
	S6_ExitCNT = 0;	
	tOptNum = (uint8_t)atoi(param);

	switch(tOptNum)
	{
		case 1:
		{
			if(cUserOpt.AutoStop < 2)
			{
				if(cUserOpt.AutoStop == 0)
					cUserOpt.AutoStop = 1;
				else if(cUserOpt.AutoStop == 1)
					cUserOpt.AutoStop = 0;

				if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
					Option_Function.AutoStop[M7_UserIndex] = cUserOpt.AutoStop;
				else
					Option_Function.AutoStopKey = cUserOpt.AutoStop;

				S6_OPT_OnOff(1,cUserOpt.AutoStop);


#ifdef USED_SAVE_CONFIG
				//Option_Function.AutoStop
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
			}
		}
		break;

		case 2:
		{
			uint8_t tAutoDecel = 0;
			
			if(cUserOpt.AutoDecel < 2)
			{
				if(cUserOpt.AutoDecel == 0)
					tAutoDecel = 1;
				else if(cUserOpt.AutoDecel == 1)
					tAutoDecel = 0;

				// CAN TX CAN[FNS1;0x1850E9CE]: 1F 00 00 00 0X 01 00 00
				txbuffer[0] = 0x1F;
				txbuffer[4] = tAutoDecel;
				txbuffer[5] = 0x01;
				txbuffer[6] = 0x00;
				
				#if 0
					PushTxData (FNS1,txbuffer,8);
				#else
					CanbusTxDataManagement( gMainTimerCnt_10m,FNS1,txbuffer);
				#endif	//JPHong 06-11
			}
		}	
		break;
		case 4:
		{
			if(cUserOpt.Rotate_Flag < 2)
			{
				if(cUserOpt.Rotate_Flag == 0)
					cUserOpt.Rotate_Flag = 1;
				else if(cUserOpt.Rotate_Flag == 1)
					cUserOpt.Rotate_Flag = 0;

				Option_Function.Rotate_Flag = cUserOpt.Rotate_Flag;

				S6_OPT_OnOff(4,cUserOpt.Rotate_Flag);

#ifdef USED_SAVE_CONFIG
				//Option_Function.Rotate_Flag
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
			}
		}
		break;
	}

    return true;
}

bool M7S6_UserDisplay(void)
{
	//chpark.24.10.15
	if(g_nM7_StartType != M7_MODE_PUSH) 
	 return 0;

	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M7S6_TopAdmin = ituSceneFindWidget(&theScene, "M7S6_TopAdmin");
	assert(M7S6_TopAdmin);
	ITUWidget *M7S6_TopStandard = ituSceneFindWidget(&theScene, "M7S6_TopStandard");
	assert(M7S6_TopStandard);
	
	ITUText *M7S6_TopUserName = ituSceneFindWidget(&theScene, "M7S6_TopUserName");
	assert(M7S6_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M7S6_TopStandard,false);
		ituWidgetSetVisible(M7S6_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M7S6_TopAdmin,false);
		ituWidgetSetVisible(M7S6_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M7S6_TopAdmin,false);
		ituWidgetSetVisible(M7S6_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S6_TopUserName, tDestName);

		ituWidgetSetVisible(M7S6_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S6_TopUserName,false);
	}
	
	return true;
}

void S6_OPT_SelectClear(void)
{
	if(ituWidgetIsVisible(S6_1_SEL) == true)
		ituWidgetSetVisible(S6_1_SEL, false);
		
	if(ituWidgetIsVisible(S6_2_SEL) == true)
		ituWidgetSetVisible(S6_2_SEL, false);

	if(ituWidgetIsVisible(S6_3_SEL) == true)
		ituWidgetSetVisible(S6_3_SEL, false);

	if(ituWidgetIsVisible(S6_4_SEL) == true)
		ituWidgetSetVisible(S6_4_SEL, false);
}

void S6_OPT_Select(uint8_t tOptNum)
{
	switch(tOptNum)
	{
		case 1:			// auto eng stop
		{
			if((cUserOpt.AutoStop == 0)||(cUserOpt.AutoStop == 1))
				ituWidgetSetVisible(S6_1_SEL, true);
		}
		break;

		case 2:			// auto decel
		{
			if((cUserOpt.AutoDecel == 0)||(cUserOpt.AutoDecel == 1))
				ituWidgetSetVisible(S6_2_SEL, true);
		}
		break;
		case 3:			// auto camera
		{
			if((cUserOpt.AutoCamera == 0)||(cUserOpt.AutoCamera == 1))
				ituWidgetSetVisible(S6_3_SEL, true);
		}
		break;
		case 4:			// lcd ratate
		{
			if((cUserOpt.Rotate_Flag == 0)||(cUserOpt.Rotate_Flag == 1))
				ituWidgetSetVisible(S6_4_SEL, true);
		}
		break;
	}
}

void S6_OPT_OnOff(uint8_t tOptNum,uint8_t tOptOnOff)
{
	switch(tOptNum)
	{
		case 1:			// auto eng stop
		{
			if(cUserOpt.AutoStop == 3)
			{
				ituWidgetSetVisible(S6_1_ON, false);
				ituWidgetSetVisible(S6_1_OFF, false);
				ituWidgetSetVisible(S6_1_HOLD, false);
			}
			else if(cUserOpt.AutoStop == 2)
			{
				ituWidgetSetVisible(S6_1_ON, false);
				ituWidgetSetVisible(S6_1_OFF, false);
				ituWidgetSetVisible(S6_1_HOLD, true);
			}
			else if((cUserOpt.AutoStop == 0)||(cUserOpt.AutoStop == 1))
			{
				if(tOptOnOff == 0)		// off
				{
					//cUserOpt.AutoStop = 0;
					ituWidgetSetVisible(S6_1_ON, false);
					ituWidgetSetVisible(S6_1_OFF, true);
					ituWidgetSetVisible(S6_1_HOLD, false);
				}
				else if(tOptOnOff == 1)			// on
				{
					//cUserOpt.AutoStop = 1;
					ituWidgetSetVisible(S6_1_OFF, false);
					ituWidgetSetVisible(S6_1_ON, true);
					ituWidgetSetVisible(S6_1_HOLD, false);
				}
			}
		}
		break;

		case 2:			// auto decel
		{
			if(cUserOpt.AutoDecel == 3)
			{
				ituWidgetSetVisible(S6_2_ON, false);
				ituWidgetSetVisible(S6_2_OFF, false);
				ituWidgetSetVisible(S6_2_HOLD, false);

				if(ituWidgetIsVisible(S6_2_SEL) == true)
					ituWidgetSetVisible(S6_2_SEL, false);
			}
			else if(cUserOpt.AutoDecel == 2)
			{
				ituWidgetSetVisible(S6_2_ON, false);
				ituWidgetSetVisible(S6_2_OFF, false);
				ituWidgetSetVisible(S6_2_HOLD, true);

				if(ituWidgetIsVisible(S6_2_SEL) == true)
					ituWidgetSetVisible(S6_2_SEL, false);
			}
			else if((cUserOpt.AutoDecel == 0)||(cUserOpt.AutoDecel == 1))
			{
				if(tOptOnOff == 0)		// off
				{
					//cUserOpt.AutoDecel = 0;
					ituWidgetSetVisible(S6_2_ON, false);
					ituWidgetSetVisible(S6_2_OFF, true);
					ituWidgetSetVisible(S6_2_HOLD, false);
				}
				else if(tOptOnOff == 1)			// on
				{
					//cUserOpt.AutoDecel = 1;
					ituWidgetSetVisible(S6_2_OFF, false);
					ituWidgetSetVisible(S6_2_ON, true);
					ituWidgetSetVisible(S6_2_HOLD, false);
				}
			}
		}
		break;

		case 4:			// lcd ratate
		{
			if(cUserOpt.Rotate_Flag == 3)
			{
				ituWidgetSetVisible(S6_4_ON, false);
				ituWidgetSetVisible(S6_4_OFF, false);
				ituWidgetSetVisible(S6_4_HOLD, false);
			}
			else if(cUserOpt.Rotate_Flag == 2)
			{
				ituWidgetSetVisible(S6_4_ON, false);
				ituWidgetSetVisible(S6_4_OFF, false);
				ituWidgetSetVisible(S6_4_HOLD, true);
			}
			else if((cUserOpt.Rotate_Flag == 0)||(cUserOpt.Rotate_Flag == 1))
			{
				if(tOptOnOff == 0)		// off
				{
					//cUserOpt.Rotate_Flag = 0;
					ituWidgetSetVisible(S6_4_ON, false);
					ituWidgetSetVisible(S6_4_OFF, true);
					ituWidgetSetVisible(S6_4_HOLD, false);
				}
				else if(tOptOnOff == 1)			// on
				{
					//cUserOpt.Rotate_Flag = 1;
					ituWidgetSetVisible(S6_4_OFF, false);
					ituWidgetSetVisible(S6_4_ON, true);
					ituWidgetSetVisible(S6_4_HOLD, false);
				}
			}
		}
		break;
	}
}

void S6_OPT_All_Off(uint8_t tUserLang) // 20240416 add whlim
{
	ITUWidget *S6_1_ON_ENG = ituSceneFindWidget(&theScene, "S6_1_ON_ENG");
	ITUWidget *S6_2_ON_ENG = ituSceneFindWidget(&theScene, "S6_2_ON_ENG");
	ITUWidget *S6_3_ON_ENG = ituSceneFindWidget(&theScene, "S6_3_ON_ENG");
	ITUWidget *S6_4_ON_ENG = ituSceneFindWidget(&theScene, "S6_4_ON_ENG");

	ITUWidget *S6_1_OFF_ENG = ituSceneFindWidget(&theScene, "S6_1_OFF_ENG");
	ITUWidget *S6_2_OFF_ENG = ituSceneFindWidget(&theScene, "S6_2_OFF_ENG");
	ITUWidget *S6_3_OFF_ENG = ituSceneFindWidget(&theScene, "S6_3_OFF_ENG");
	ITUWidget *S6_4_OFF_ENG = ituSceneFindWidget(&theScene, "S6_4_OFF_ENG");

	ITUWidget *S6_1_HOLD_ENG = ituSceneFindWidget(&theScene, "S6_1_HOLD_ENG");
	ITUWidget *S6_2_HOLD_ENG = ituSceneFindWidget(&theScene, "S6_2_HOLD_ENG");
	ITUWidget *S6_3_HOLD_ENG = ituSceneFindWidget(&theScene, "S6_3_HOLD_ENG");
	ITUWidget *S6_4_HOLD_ENG = ituSceneFindWidget(&theScene, "S6_4_HOLD_ENG");

	ITUWidget *S6_1_ON_DEU = ituSceneFindWidget(&theScene, "S6_1_ON_DEU");
	ITUWidget *S6_2_ON_DEU = ituSceneFindWidget(&theScene, "S6_2_ON_DEU");
	ITUWidget *S6_3_ON_DEU = ituSceneFindWidget(&theScene, "S6_3_ON_DEU");
	ITUWidget *S6_4_ON_DEU = ituSceneFindWidget(&theScene, "S6_4_ON_DEU");

	ITUWidget *S6_1_OFF_DEU = ituSceneFindWidget(&theScene, "S6_1_OFF_DEU");
	ITUWidget *S6_2_OFF_DEU = ituSceneFindWidget(&theScene, "S6_2_OFF_DEU");
	ITUWidget *S6_3_OFF_DEU = ituSceneFindWidget(&theScene, "S6_3_OFF_DEU");
	ITUWidget *S6_4_OFF_DEU = ituSceneFindWidget(&theScene, "S6_4_OFF_DEU");

	ITUWidget *S6_1_HOLD_DEU = ituSceneFindWidget(&theScene, "S6_1_HOLD_DEU");
	ITUWidget *S6_2_HOLD_DEU = ituSceneFindWidget(&theScene, "S6_2_HOLD_DEU");
	ITUWidget *S6_3_HOLD_DEU = ituSceneFindWidget(&theScene, "S6_3_HOLD_DEU");
	ITUWidget *S6_4_HOLD_DEU = ituSceneFindWidget(&theScene, "S6_4_HOLD_DEU");

	ITUWidget *S6_1_ON_FRA = ituSceneFindWidget(&theScene, "S6_1_ON_FRA");
	ITUWidget *S6_2_ON_FRA = ituSceneFindWidget(&theScene, "S6_2_ON_FRA");
	ITUWidget *S6_3_ON_FRA = ituSceneFindWidget(&theScene, "S6_3_ON_FRA");
	ITUWidget *S6_4_ON_FRA = ituSceneFindWidget(&theScene, "S6_4_ON_FRA");

	ITUWidget *S6_1_OFF_FRA = ituSceneFindWidget(&theScene, "S6_1_OFF_FRA");
	ITUWidget *S6_2_OFF_FRA = ituSceneFindWidget(&theScene, "S6_2_OFF_FRA");
	ITUWidget *S6_3_OFF_FRA = ituSceneFindWidget(&theScene, "S6_3_OFF_FRA");
	ITUWidget *S6_4_OFF_FRA = ituSceneFindWidget(&theScene, "S6_4_OFF_FRA");

	ITUWidget *S6_1_HOLD_FRA = ituSceneFindWidget(&theScene, "S6_1_HOLD_FRA");
	ITUWidget *S6_2_HOLD_FRA = ituSceneFindWidget(&theScene, "S6_2_HOLD_FRA");
	ITUWidget *S6_3_HOLD_FRA = ituSceneFindWidget(&theScene, "S6_3_HOLD_FRA");
	ITUWidget *S6_4_HOLD_FRA = ituSceneFindWidget(&theScene, "S6_4_HOLD_FRA");

	ITUWidget *S6_1_ON_ITA = ituSceneFindWidget(&theScene, "S6_1_ON_ITA");
	ITUWidget *S6_2_ON_ITA = ituSceneFindWidget(&theScene, "S6_2_ON_ITA");
	ITUWidget *S6_3_ON_ITA = ituSceneFindWidget(&theScene, "S6_3_ON_ITA");
	ITUWidget *S6_4_ON_ITA = ituSceneFindWidget(&theScene, "S6_4_ON_ITA");

	ITUWidget *S6_1_OFF_ITA = ituSceneFindWidget(&theScene, "S6_1_OFF_ITA");
	ITUWidget *S6_2_OFF_ITA = ituSceneFindWidget(&theScene, "S6_2_OFF_ITA");
	ITUWidget *S6_3_OFF_ITA = ituSceneFindWidget(&theScene, "S6_3_OFF_ITA");
	ITUWidget *S6_4_OFF_ITA = ituSceneFindWidget(&theScene, "S6_4_OFF_ITA");

	ITUWidget *S6_1_HOLD_ITA = ituSceneFindWidget(&theScene, "S6_1_HOLD_ITA");
	ITUWidget *S6_2_HOLD_ITA = ituSceneFindWidget(&theScene, "S6_2_HOLD_ITA");
	ITUWidget *S6_3_HOLD_ITA = ituSceneFindWidget(&theScene, "S6_3_HOLD_ITA");
	ITUWidget *S6_4_HOLD_ITA = ituSceneFindWidget(&theScene, "S6_4_HOLD_ITA");

	ITUWidget *S6_1_ON_ESP = ituSceneFindWidget(&theScene, "S6_1_ON_ESP");
	ITUWidget *S6_2_ON_ESP = ituSceneFindWidget(&theScene, "S6_2_ON_ESP");
	ITUWidget *S6_3_ON_ESP = ituSceneFindWidget(&theScene, "S6_3_ON_ESP");
	ITUWidget *S6_4_ON_ESP = ituSceneFindWidget(&theScene, "S6_4_ON_ESP");

	ITUWidget *S6_1_OFF_ESP = ituSceneFindWidget(&theScene, "S6_1_OFF_ESP");
	ITUWidget *S6_2_OFF_ESP = ituSceneFindWidget(&theScene, "S6_2_OFF_ESP");
	ITUWidget *S6_3_OFF_ESP = ituSceneFindWidget(&theScene, "S6_3_OFF_ESP");
	ITUWidget *S6_4_OFF_ESP = ituSceneFindWidget(&theScene, "S6_4_OFF_ESP");

	ITUWidget *S6_1_HOLD_ESP = ituSceneFindWidget(&theScene, "S6_1_HOLD_ESP");
	ITUWidget *S6_2_HOLD_ESP = ituSceneFindWidget(&theScene, "S6_2_HOLD_ESP");
	ITUWidget *S6_3_HOLD_ESP = ituSceneFindWidget(&theScene, "S6_3_HOLD_ESP");
	ITUWidget *S6_4_HOLD_ESP = ituSceneFindWidget(&theScene, "S6_4_HOLD_ESP");

	ituWidgetSetVisible(S6_1_ON_ENG, false);
	ituWidgetSetVisible(S6_2_ON_ENG, false);
	ituWidgetSetVisible(S6_3_ON_ENG, false);
	ituWidgetSetVisible(S6_4_ON_ENG, false);

	ituWidgetSetVisible(S6_1_OFF_ENG, false);
	ituWidgetSetVisible(S6_2_OFF_ENG, false);
	ituWidgetSetVisible(S6_3_OFF_ENG, false);
	ituWidgetSetVisible(S6_4_OFF_ENG, false);

	ituWidgetSetVisible(S6_1_HOLD_ENG, false);
	ituWidgetSetVisible(S6_2_HOLD_ENG, false);
	ituWidgetSetVisible(S6_3_HOLD_ENG, false);
	ituWidgetSetVisible(S6_4_HOLD_ENG, false);

	ituWidgetSetVisible(S6_1_ON_DEU, false);
	ituWidgetSetVisible(S6_2_ON_DEU, false);
	ituWidgetSetVisible(S6_3_ON_DEU, false);
	ituWidgetSetVisible(S6_4_ON_DEU, false);

	ituWidgetSetVisible(S6_1_OFF_DEU, false);
	ituWidgetSetVisible(S6_2_OFF_DEU, false);
	ituWidgetSetVisible(S6_3_OFF_DEU, false);
	ituWidgetSetVisible(S6_4_OFF_DEU, false);

	ituWidgetSetVisible(S6_1_HOLD_DEU, false);
	ituWidgetSetVisible(S6_2_HOLD_DEU, false);
	ituWidgetSetVisible(S6_3_HOLD_DEU, false);
	ituWidgetSetVisible(S6_4_HOLD_DEU, false);

	ituWidgetSetVisible(S6_1_ON_FRA, false);
	ituWidgetSetVisible(S6_2_ON_FRA, false);
	ituWidgetSetVisible(S6_3_ON_FRA, false);
	ituWidgetSetVisible(S6_4_ON_FRA, false);

	ituWidgetSetVisible(S6_1_OFF_FRA, false);
	ituWidgetSetVisible(S6_2_OFF_FRA, false);
	ituWidgetSetVisible(S6_3_OFF_FRA, false);
	ituWidgetSetVisible(S6_4_OFF_FRA, false);

	ituWidgetSetVisible(S6_1_HOLD_FRA, false);
	ituWidgetSetVisible(S6_2_HOLD_FRA, false);
	ituWidgetSetVisible(S6_3_HOLD_FRA, false);
	ituWidgetSetVisible(S6_4_HOLD_FRA, false);

	ituWidgetSetVisible(S6_1_ON_ITA, false);
	ituWidgetSetVisible(S6_2_ON_ITA, false);
	ituWidgetSetVisible(S6_3_ON_ITA, false);
	ituWidgetSetVisible(S6_4_ON_ITA, false);

	ituWidgetSetVisible(S6_1_OFF_ITA, false);
	ituWidgetSetVisible(S6_2_OFF_ITA, false);
	ituWidgetSetVisible(S6_3_OFF_ITA, false);
	ituWidgetSetVisible(S6_4_OFF_ITA, false);

	ituWidgetSetVisible(S6_1_HOLD_ITA, false);
	ituWidgetSetVisible(S6_2_HOLD_ITA, false);
	ituWidgetSetVisible(S6_3_HOLD_ITA, false);
	ituWidgetSetVisible(S6_4_HOLD_ITA, false);

	ituWidgetSetVisible(S6_1_ON_ESP, false);
	ituWidgetSetVisible(S6_2_ON_ESP, false);
	ituWidgetSetVisible(S6_3_ON_ESP, false);
	ituWidgetSetVisible(S6_4_ON_ESP, false);

	ituWidgetSetVisible(S6_1_OFF_ESP, false);
	ituWidgetSetVisible(S6_2_OFF_ESP, false);
	ituWidgetSetVisible(S6_3_OFF_ESP, false);
	ituWidgetSetVisible(S6_4_OFF_ESP, false);

	ituWidgetSetVisible(S6_1_HOLD_ESP, false);
	ituWidgetSetVisible(S6_2_HOLD_ESP, false);
	ituWidgetSetVisible(S6_3_HOLD_ESP, false);
	ituWidgetSetVisible(S6_4_HOLD_ESP, false);
	
	

	if(cUserOpt.AutoDecel == 3)
	{
		if(ituWidgetIsVisible(S6_2_SEL) == true)
			ituWidgetSetVisible(S6_2_SEL, false);
	}
	else if(cUserOpt.AutoDecel == 2)
	{
		ituWidgetSetVisible(S6_2_HOLD, true);

		if(ituWidgetIsVisible(S6_2_SEL) == true)
			ituWidgetSetVisible(S6_2_SEL, false);
	}
	else if((cUserOpt.AutoDecel == 0)||(cUserOpt.AutoDecel == 1))
	{
		ituWidgetSetVisible(S6_2_OFF, true);
	}

	if(cUserOpt.AutoCamera == 3)
	{
		if(ituWidgetIsVisible(S6_3_SEL) == true)
			ituWidgetSetVisible(S6_3_SEL, false);
	}
	else if(cUserOpt.AutoCamera == 2)
	{
		ituWidgetSetVisible(S6_3_HOLD, true);

		if(ituWidgetIsVisible(S6_3_SEL) == true)
			ituWidgetSetVisible(S6_3_SEL, false);
	}
	else if((cUserOpt.AutoCamera == 0)||(cUserOpt.AutoCamera == 1))
	{
		ituWidgetSetVisible(S6_3_OFF, true);
	}

	if(cUserOpt.AutoStop == 2)
	{
		ituWidgetSetVisible(S6_1_HOLD, true);
	}
	else if((cUserOpt.AutoStop == 0)||(cUserOpt.AutoStop == 1))
	{
		ituWidgetSetVisible(S6_1_OFF, true);
	}

	if(cUserOpt.Rotate_Flag == 2)
	{
		ituWidgetSetVisible(S6_4_HOLD, true);
	}
	else if((cUserOpt.Rotate_Flag == 0)||(cUserOpt.Rotate_Flag == 1))
	{
		ituWidgetSetVisible(S6_4_OFF, true);
	}
}