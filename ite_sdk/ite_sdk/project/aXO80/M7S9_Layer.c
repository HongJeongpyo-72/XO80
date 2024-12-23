#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S9
S9_Select
M15_Delete_ESP
M15_Delete_ITA
M15_Delete_FRA
M15_Delete_DEU
M15_Delete_ENG
M15_Delete_BTN
M15_Delete_SEL
M15_DelMSG_ESP
M15_DelMSG_ITA
M15_DelMSG_FRA
M15_DelMSG_DEU
M15_DelMSG_ENG
M15_G10
M15_G9
M15_G8
M15_G7
M15_G6
M15_G5
M15_G4
M15_G3
M15_G2
M15_G1
S9S_D7_BTN
S9S_D6_BTN
S9S_D5_BTN
M15_T10_Name
M15_T9_Name
M15_T8_Name
M15_T7_Name
M15_T6_Name
M15_T5_Name
M15_T4_Name
M15_T3_Name
M15_T2_Name
M15_T1_Name
M15_T10
M15_T9
M15_T8
M15_T7
M15_T6
M15_T5
M15_T4
M15_T3
M15_T2
M15_T1
M15_U10_Name
M15_U9_Name
M15_U8_Name
M15_U7_Name
M15_U6_Name
M15_U5_Name
M15_U4_Name
M15_U3_Name
M15_U2_Name
M15_U1_Name
M15_Del_Name
M15_Add_Name
M15_U10
M15_U9
M15_U8
M15_U7
M15_U6
M15_U5
M15_U4
M15_U3
M15_U2
M15_U1
M15_Del
M15_Add
M15_U10_BTN
M15_U9_BTN
M15_U8_BTN
M15_U7_BTN
M15_U6_BTN
M15_U5_BTN
M15_U4_BTN
M15_U3_BTN
M15_U2_BTN
M15_U1_BTN
M15_Del_BTN
M15_Add_BTN
M15_U10_SEL
M15_U9_SEL
M15_U8_SEL
M15_U7_SEL
M15_U6_SEL
M15_U5_SEL
M15_U4_SEL
M15_U3_SEL
M15_U2_SEL
M15_U1_SEL
M15_Del_SEL
M15_Add_SEL
S9_MSG
*/

static ITUText *S9_MSG;

// CAN_[UI2,0x1850FE8C]1~8 No UserInfo , CAN_[UI3,0x1850FF8C]9~10 No UserInfo
// Byte=b[0::3]PassCode_Digit + b[4]RFID Reg + b[5]BT Reg + b[6::7]Access Level
//UserInfo User_RegInfo[MAX_SU100_USER_NUM];			// register user infomation, system date resume(saved data)//chpark.24.01.31
//UserInfoDM14 User_RegInfoDM14;						// JPHong 2023-12-12//chpark.24.01.31


static uint8_t cUserPage = 0;		// 0:User SelectPage(add/delete), 1:User_EditPage, 2:User_NamePage, 3:User_PassCodePage, 4:User_RFKeyPage, 5:User_MacPage

static uint8_t Total_RegUser = 0;		// 
static uint8_t cUserIndex = 0;			// system data resume(M7_UserIndex),immobilizer checked user index
static UserInfo cUserInfo;

uint8_t S9_UserMode = 0;

uint8_t cAddUserIdx = 0;
uint8_t cEditUserIdx = 0;

static uint32_t S9_SecTimer;
static uint8_t S9_ExitCNT = 0;

void Select_UserDisplay(uint8_t tUserIdx);
void Select_UserClear(uint8_t tUserIdx);
void Sel_BorderDisplay(uint8_t tSelIdx,bool tOps);

void User_GrayWhite_Toggle(uint8_t tUserIdx);
void User_Display(uint8_t tUserIdx,bool tGrayOpt);
void User_AllClear(void);
void User_Clear(uint8_t tUserIdx);

void User_SetPassCode(uint8_t tUserIdx,uint64_t tPcode);
uint8_t PCode_Digit(uint64_t tPcode);

void User_M1(uint8_t tUserIdx);		// dev.opt
void User_M2(uint8_t tUserIdx);		// dev.opt
void User_O1(uint8_t tUserIdx);		// dev.opt
void User_O2(uint8_t tUserIdx);		// dev.opt
void User_O3(uint8_t tUserIdx);		// dev.opt
void User_O4(uint8_t tUserIdx);		// dev.opt
void User_O5(uint8_t tUserIdx);		// dev.opt
void User_O6(uint8_t tUserIdx);
void User_O7(uint8_t tUserIdx);
void User_O8(uint8_t tUserIdx);

void DBG_UserNameCode(uint8_t tUserIdx);
void ISO8859_to_UTF8(uint8_t *tDest,uint8_t *tSrc);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);

bool M7S9_Enter(ITUWidget* widget, char* param)
{
	ITUWidget *M15_Add_BTN = ituSceneFindWidget(&theScene, "M15_Add_BTN");
	assert(M15_Add_BTN);
	ITUWidget *M15_Add = ituSceneFindWidget(&theScene, "M15_Add");
	assert(M15_Add);
	ITUText *M15_Add_Name = ituSceneFindWidget(&theScene, "M15_Add_Name");
	assert(M15_Add_Name);

	ITUWidget *M15_Del_BTN = ituSceneFindWidget(&theScene, "M15_Del_BTN");
	assert(M15_Add_BTN);
	ITUWidget *M15_Del = ituSceneFindWidget(&theScene, "M15_Del");
	assert(M15_Del);
	ITUText *M15_Del_Name = ituSceneFindWidget(&theScene, "M15_Del_Name");
	assert(M15_Del_Name);

	ITUWidget *M15_DelMSG_ENG = ituSceneFindWidget(&theScene, "M15_DelMSG_ENG");
	assert(M15_DelMSG_ENG);
	ITUWidget *M15_DelMSG_DEU = ituSceneFindWidget(&theScene, "M15_DelMSG_DEU");
	assert(M15_DelMSG_DEU);
	ITUWidget *M15_DelMSG_FRA = ituSceneFindWidget(&theScene, "M15_DelMSG_FRA");
	assert(M15_DelMSG_FRA);
	ITUWidget *M15_DelMSG_ITA = ituSceneFindWidget(&theScene, "M15_DelMSG_ITA");
	assert(M15_DelMSG_ITA);
	ITUWidget *M15_DelMSG_ESP = ituSceneFindWidget(&theScene, "M15_DelMSG_ESP");
	assert(M15_DelMSG_ESP);
	
	uint8_t i,j,k;
	
	static uint8_t _1Time_Flag = 0;
	
	S9_MSG = ituSceneFindWidget(&theScene, "S9_MSG");		// dev.opt
	assert(S9_MSG);
	ituWidgetSetVisible(S9_MSG, false);
	
	// dev.opt
	if(_1Time_Flag == 0)
	{
		for(i=0;i<MAX_SU100_USER_NUM;i++)
		{
		
			User_RegInfo[i].RegFlag = 0;			// 0;Not Register ,1;Master ,2;Operater
			User_RegInfo[i].PassDigit = 0;		// PassCode Digit
			User_RegInfo[i].RFStatus = 0;			// RFID Register Status
			User_RegInfo[i].MacStatus = 0;		// BT MAC ID Register status
	
			for(j=0;j<MAX_SU100_USER_NAME;j++)
			{
				User_RegInfo[i].UserName[j] = 0;				// username Max. 10digit
			}
			for(j=0;j<MAX_SU100_USER_PW;j++)
			{
				User_RegInfo[i].UserPassCode[j] = 0;		// user passcode 10digit(Max 10digit,Min 4digit)
			}
			for(k=0;k<MAX_SU100_USER_MID;k++)
				User_RegInfo[i].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
		}
		
		_1Time_Flag = 1;
	}

	// dev.opt
	User_M1(10);
	User_M2(9);
	User_O1(1);
	User_O2(2);
	User_O3(3);
	User_O4(4);
	User_O5(5);
	User_O6(6);
	User_O7(7);
	User_O8(8);

	DBG_UserNameCode(10);
	DBG_UserNameCode(9);
	DBG_UserNameCode(1);
	DBG_UserNameCode(2);
	DBG_UserNameCode(3);
	DBG_UserNameCode(4);
	DBG_UserNameCode(5);
	DBG_UserNameCode(6);
	DBG_UserNameCode(7);
	DBG_UserNameCode(8);

	//M7_UserIndex = 8;		// dev.opt master user
	//M7_UserIndex = 4;		// dev.opt operater user

	// JPHong 2024-02-06  Delete init M7_UserIndex
	if (M7_UserIndex>=MAX_SU100_USER_NUM)
	{
		M7_UserIndex = 0;
	}
	cUserIndex = M7_UserIndex;

	Total_RegUser	= 0;				// Total Reg. User's count
	for(i=0;i<10;i++)
	{
		if(User_RegInfo[i].RegFlag != 0)
		{	
			Total_RegUser++;
		}
	}
	printf("\r\nTotal Register User = %d\r\n",Total_RegUser);

	// master / operater user check
	if(User_RegInfo[cUserIndex].RegFlag == 1)
	{
		// master user -> user select(add/delete) & edit mode
		printf("Master Register User = %d\r\n",cUserIndex+1);

		User_AllClear();

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
			ituWidgetSetVisible(M15_DelMSG_ENG,false);
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
			ituWidgetSetVisible(M15_DelMSG_DEU,false);
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
			ituWidgetSetVisible(M15_DelMSG_FRA,false);
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
			ituWidgetSetVisible(M15_DelMSG_ITA,false);
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
			ituWidgetSetVisible(M15_DelMSG_ESP,false);

		S9_UserMode = MaUserSelect_Mode;

		cAddUserIdx = 0;
		cEditUserIdx = 0;

		ituWidgetSetVisible(M15_Add_BTN,true);
		ituWidgetSetVisible(M15_Add,true);
		ituWidgetSetVisible(M15_Add_Name,true);

		ituWidgetSetVisible(M15_Del_BTN,true);
		ituWidgetSetVisible(M15_Del,true);
		ituWidgetSetVisible(M15_Del_Name,true);

		Select_UserDisplay(cUserIndex+1);

		for(i=1;i<11;i++)
		{
			if((User_RegInfo[i-1].RegFlag == 1)||(User_RegInfo[i-1].RegFlag == 2))
				User_Display(i,0);
		}
		
		Sel_BorderDisplay(cUserIndex+1,true);
	}
	else if(User_RegInfo[cUserIndex].RegFlag == 2)
	{
		// operater user - user edit mode	
		printf("Operater Register User = %d\r\n",cUserIndex+1);

		S9_UserMode = OpUserEdit_Mode;
		
		cAddUserIdx = 0;
		cEditUserIdx = 0;
		
		ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "S9_Edit");
		assert(tBranchLayer);
		ituLayerGoto(tBranchLayer);
	}

	S9_SecTimer = gMainTimerCnt_10m;
	S9_ExitCNT = 0;

    return true;
}

bool M7S9_Timer(ITUWidget* widget, char* param)
{

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

	if((gMainTimerCnt_10m - S9_SecTimer) >= 100)
	{
		if(S9_ExitCNT < 30)
			S9_ExitCNT++;
		else if(S9_ExitCNT == 30)
		{
			S9_ExitCNT = 100;
			printf("\r\nS9 Menu 30Sec Exit > goto Main\r\n");
			//GotoMain();
		}
			
		S9_SecTimer = gMainTimerCnt_10m;
	}


    return true;
}

bool M7S9_JogWheel(ITUWidget* widget, char* param)
{
	S9_ExitCNT = 0;

    return true;
}

bool M7S9_JogButton(ITUWidget* widget, char* param)
{
	S9_ExitCNT = 0;

    return true;
}

bool M7S9_JogSwitch(ITUWidget* widget, char* param)
{
	S9_ExitCNT = 0;

    return true;
}

bool S9S_D7_MouseUp(ITUWidget* widget, char* param)
{
	S9_ExitCNT = 0;

    return true;
}

bool S9S_D6_MouseUp(ITUWidget* widget, char* param)
{
	S9_ExitCNT = 0;

    return true;
}

bool S9S_D5_MouseUp(ITUWidget* widget, char* param)
{

	GotoMenu7();
	
    return true;
}

bool S9_SEL_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t i,tBTN_Index = 0;
	
	S9_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);

	printf("S9 BTN DOWN = %d\r\n",tBTN_Index);

	Sel_BorderDisplay(tBTN_Index,true);

	switch(tBTN_Index)
	{
		case 11:			// master user - user add
		{
			uint8_t i;
		
			// Master user check
			if(User_RegInfo[cUserIndex].RegFlag == 1)
			{
				// TXcan [UM1:18511ACE={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}] 0byte 0bit:1 send to ctrl
				// request addu ser
				
				// standby CAN RX[UM5:18511C8C={}]
				// recieve empty user number
				
				// dev.ops lowest empty user number
				for(i=0;i<10;i++)
				{
					if(User_RegInfo[i].RegFlag == 0)
					{
						cAddUserIdx = i+1;
						printf("Empty User Number = %d (dev.ops)\r\n",cAddUserIdx);

						break;
					}
				}

				// User add process start
				S9_UserMode = MaUserAdd_Mode;
			}
		}
		break;
		
		case 12:			// master user - user delete
		{
			ITUWidget *M15_Add_BTN = ituSceneFindWidget(&theScene, "M15_Add_BTN");
			assert(M15_Add_BTN);
			ITUWidget *M15_Add = ituSceneFindWidget(&theScene, "M15_Add");
			assert(M15_Add);
			ITUText *M15_Add_Name = ituSceneFindWidget(&theScene, "M15_Add_Name");
			assert(M15_Add_Name);

			ITUWidget *M15_Del_BTN = ituSceneFindWidget(&theScene, "M15_Del_BTN");
			assert(M15_Add_BTN);
			ITUWidget *M15_Del = ituSceneFindWidget(&theScene, "M15_Del");
			assert(M15_Del);
			ITUText *M15_Del_Name = ituSceneFindWidget(&theScene, "M15_Del_Name");
			assert(M15_Del_Name);

			ITUWidget *M15_DelMSG_ENG = ituSceneFindWidget(&theScene, "M15_DelMSG_ENG");
			assert(M15_DelMSG_ENG);
			ITUWidget *M15_DelMSG_DEU = ituSceneFindWidget(&theScene, "M15_DelMSG_DEU");
			assert(M15_DelMSG_DEU);
			ITUWidget *M15_DelMSG_FRA = ituSceneFindWidget(&theScene, "M15_DelMSG_FRA");
			assert(M15_DelMSG_FRA);
			ITUWidget *M15_DelMSG_ITA = ituSceneFindWidget(&theScene, "M15_DelMSG_ITA");
			assert(M15_DelMSG_ITA);
			ITUWidget *M15_DelMSG_ESP = ituSceneFindWidget(&theScene, "M15_DelMSG_ESP");
			assert(M15_DelMSG_ESP);

			// Master user check
			if(User_RegInfo[cUserIndex].RegFlag == 1)
			{
				// delete user mode
				// build user delete mode ui
				ituWidgetSetVisible(M15_Add_BTN,false);
				ituWidgetSetVisible(M15_Add,false);
				ituWidgetSetVisible(M15_Add_Name,false);

				ituWidgetSetVisible(M15_Del_BTN,false);
				ituWidgetSetVisible(M15_Del,false);
				ituWidgetSetVisible(M15_Del_Name,false);
				
				User_AllClear();
				
				Select_UserDisplay(cUserIndex+1);
				
				for(i=1;i<11;i++)
				{
					if((User_RegInfo[i-1].RegFlag == 1)||(User_RegInfo[i-1].RegFlag == 2))
						User_Display(i,1);
				}

				Sel_BorderDisplay(cUserIndex+1,true);
				
				if(Lang_Flag[M7_UserIndex] == Lang_ENG)
					ituWidgetSetVisible(M15_DelMSG_ENG,true);
				else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
					ituWidgetSetVisible(M15_DelMSG_DEU,true);
				else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
					ituWidgetSetVisible(M15_DelMSG_FRA,true);
				else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
					ituWidgetSetVisible(M15_DelMSG_ITA,true);
				else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
					ituWidgetSetVisible(M15_DelMSG_ESP,true);

				S9_UserMode = MaUserDelete_Mode;

			}
		}
		break;
	}

    return true;
}

bool S9_SEL_MouseUp(ITUWidget* widget, char* param)
{
	ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "S9_Edit");
	assert(tBranchLayer);

	uint8_t tBTN_Index = 0;
	
	S9_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);

	printf("S9 BTN UP = %d\r\n",tBTN_Index);

	Sel_BorderDisplay(tBTN_Index,false);

	switch(tBTN_Index)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		{
			if(S9_UserMode == MaUserDelete_Mode)
			{
				if(tBTN_Index != (cUserIndex+1))
				{
					// Gray/White toggle
					User_GrayWhite_Toggle(tBTN_Index);
				}
				else if(tBTN_Index == (cUserIndex+1))
				{
					printf("Warnning Self Delete !!\r\n");
				}
			}
			else if(S9_UserMode == MaUserSelect_Mode)
			{
				S9_UserMode = MaUserEdit_Mode;
		
				cAddUserIdx = 0;
				cEditUserIdx = tBTN_Index;
				ituLayerGoto(tBranchLayer);
			}
		}
		break;

		case 11:			// master user - user add
		{
			// User add process start
			S9_UserMode = MaUserAdd_Mode;
			
			cEditUserIdx = 0;
			printf("Add User Register= %d\r\n",cAddUserIdx);
			ituLayerGoto(tBranchLayer);
		}
		break;
		
		case 12:			// master user - user delete
		{
			// delete user process start
			// 
		}
		break;
	}

    return true;
}

void Sel_BorderDisplay(uint8_t tSelIdx,bool tOps)
{
	ITUWidget *M15_U1_SEL = ituSceneFindWidget(&theScene, "M15_U1_SEL");		// 1
	assert(M15_U1_SEL);
	ITUWidget *M15_U2_SEL = ituSceneFindWidget(&theScene, "M15_U2_SEL");		// 2
	assert(M15_U2_SEL);
	ITUWidget *M15_U3_SEL = ituSceneFindWidget(&theScene, "M15_U3_SEL");		// 3
	assert(M15_U3_SEL);
	ITUWidget *M15_U4_SEL = ituSceneFindWidget(&theScene, "M15_U4_SEL");		// 4
	assert(M15_U4_SEL);
	ITUWidget *M15_U5_SEL = ituSceneFindWidget(&theScene, "M15_U5_SEL");		// 5
	assert(M15_U5_SEL);
	ITUWidget *M15_U6_SEL = ituSceneFindWidget(&theScene, "M15_U6_SEL");		// 6
	assert(M15_U6_SEL);
	ITUWidget *M15_U7_SEL = ituSceneFindWidget(&theScene, "M15_U7_SEL");		// 7
	assert(M15_U7_SEL);
	ITUWidget *M15_U8_SEL = ituSceneFindWidget(&theScene, "M15_U8_SEL");		// 8
	assert(M15_U8_SEL);
	ITUWidget *M15_U9_SEL = ituSceneFindWidget(&theScene, "M15_U9_SEL");		// 9
	assert(M15_U9_SEL);
	ITUWidget *M15_U10_SEL = ituSceneFindWidget(&theScene, "M15_U10_SEL");		// 10
	assert(M15_U10_SEL);
	ITUWidget *M15_Add_SEL = ituSceneFindWidget(&theScene, "M15_Add_SEL");		// 11
	assert(M15_Add_SEL);
	ITUWidget *M15_Del_SEL = ituSceneFindWidget(&theScene, "M15_Del_SEL");		// 12
	assert(M15_Del_SEL);

	if(ituWidgetIsVisible(M15_U1_SEL) == true)
		ituWidgetSetVisible(M15_U1_SEL, false);
	if(ituWidgetIsVisible(M15_U2_SEL) == true)
		ituWidgetSetVisible(M15_U2_SEL, false);
	if(ituWidgetIsVisible(M15_U3_SEL) == true)
		ituWidgetSetVisible(M15_U3_SEL, false);
	if(ituWidgetIsVisible(M15_U4_SEL) == true)
		ituWidgetSetVisible(M15_U4_SEL, false);
	if(ituWidgetIsVisible(M15_U5_SEL) == true)
		ituWidgetSetVisible(M15_U5_SEL, false);
	if(ituWidgetIsVisible(M15_U6_SEL) == true)
		ituWidgetSetVisible(M15_U6_SEL, false);
	if(ituWidgetIsVisible(M15_U7_SEL) == true)
		ituWidgetSetVisible(M15_U7_SEL, false);
	if(ituWidgetIsVisible(M15_U8_SEL) == true)
		ituWidgetSetVisible(M15_U8_SEL, false);
	if(ituWidgetIsVisible(M15_U9_SEL) == true)
		ituWidgetSetVisible(M15_U9_SEL, false);
	if(ituWidgetIsVisible(M15_U10_SEL) == true)
		ituWidgetSetVisible(M15_U10_SEL, false);
/*
	if(ituWidgetIsVisible(M15_Add_SEL) == true)
		ituWidgetSetVisible(M15_Add_SEL, false);
	if(ituWidgetIsVisible(M15_Del_SEL) == true)
		ituWidgetSetVisible(M15_Del_SEL, false);
*/

	switch(tSelIdx)
	{
		case 1:
			ituWidgetSetVisible(M15_U1_SEL, tOps);
		break;

		case 2:
			ituWidgetSetVisible(M15_U2_SEL, tOps);
		break;

		case 3:
			ituWidgetSetVisible(M15_U3_SEL, tOps);
		break;

		case 4:
			ituWidgetSetVisible(M15_U4_SEL, tOps);
		break;

		case 5:
			ituWidgetSetVisible(M15_U5_SEL, tOps);
		break;

		case 6:
			ituWidgetSetVisible(M15_U6_SEL, tOps);
		break;

		case 7:
			ituWidgetSetVisible(M15_U7_SEL, tOps);
		break;

		case 8:
			ituWidgetSetVisible(M15_U8_SEL, tOps);
		break;

		case 9:
			ituWidgetSetVisible(M15_U9_SEL, tOps);
		break;

		case 10:
			ituWidgetSetVisible(M15_U10_SEL, tOps);
		break;

		case 11:
			ituWidgetSetVisible(M15_Add_SEL, tOps);
		break;

		case 12:
			ituWidgetSetVisible(M15_Del_SEL, tOps);
		break;
	}
}

void Select_UserDisplay(uint8_t tUserIdx)
{
	char ptUserIcon[15],ptUserName[15];
	uint8_t i;

	uint8_t tDestName[30];

	ITUWidget *M15_Icon[11];
	ITUText *M15_Name[11];

	memset(ptUserIcon,0,15);
	memset(ptUserName,0,15);
	
	for(i=1;i<11;i++)
	{
		sprintf(ptUserIcon,"M15_T%d",i);
		sprintf(ptUserName,"M15_T%d_Name",i);

		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_Name[i] = ituSceneFindWidget(&theScene, ptUserName);
		assert(M15_Name[i]);
		
		//printf("Set icon = %s\r\n",ptUserIcon);
		//printf("Set name = %s\r\n",ptUserName);
	}

	ituWidgetSetVisible(M15_Icon[tUserIdx],true);

	memset(tDestName,0,30);
	ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx-1].UserName);
	ituTextSetString(M15_Name[tUserIdx], tDestName);
	//ituTextSetString(M15_Name[tUserIdx], User_RegInfo[tUserIdx-1].UserName);

	ituWidgetSetVisible(M15_Name[tUserIdx],true);
}

void Select_UserClear(uint8_t tUserIdx)
{
	char ptUserIcon[15],ptUserName[15];
	uint8_t i;

	ITUWidget *M15_Icon[11];
	ITUText *M15_Name[11];

	memset(ptUserIcon,0,15);
	memset(ptUserName,0,15);
	
	for(i=1;i<11;i++)
	{
		sprintf(ptUserIcon,"M15_T%d",i);
		sprintf(ptUserName,"M15_T%d_Name",i);

		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_Name[i] = ituSceneFindWidget(&theScene, ptUserName);
		assert(M15_Name[i]);
	}

	ituWidgetSetVisible(M15_Icon[tUserIdx],false);
	ituWidgetSetVisible(M15_Name[tUserIdx],false);
}

void User_GrayWhite_Toggle(uint8_t tUserIdx)
{
	char ptUserIcon[15],ptGrayUserIcon[15];
	uint8_t i;

	ITUWidget *M15_Icon[11];
	ITUWidget *M15_GrayIcon[11];

	memset(ptUserIcon,0,15);
	memset(ptGrayUserIcon,0,15);

	for(i=1;i<11;i++)
	{
		sprintf(ptUserIcon,"M15_U%d",i);
		sprintf(ptGrayUserIcon,"M15_G%d",i);

		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_GrayIcon[i] = ituSceneFindWidget(&theScene, ptGrayUserIcon);
		assert(M15_GrayIcon[i]);
	}

	if(ituWidgetIsVisible(M15_Icon[tUserIdx]) == true)
	{
		ituWidgetSetVisible(M15_Icon[tUserIdx],false);
		ituWidgetSetVisible(M15_GrayIcon[tUserIdx],true);
	}
	else if(ituWidgetIsVisible(M15_GrayIcon[tUserIdx]) == true)
	{
		ituWidgetSetVisible(M15_GrayIcon[tUserIdx],false);
		ituWidgetSetVisible(M15_Icon[tUserIdx],true);
	}
}

void User_NameConvert(char *tDest,char *tSrc)
{
	
	
}

void User_Display(uint8_t tUserIdx,bool tGrayOpt)
{
	char ptUserIcon[15],ptGrayUserIcon[15],ptUserName[15],ptUserBTN[15];
	uint8_t i;

	uint8_t tDestName[30];

	ITUWidget *M15_BTN[11];
	ITUWidget *M15_Icon[11];
	ITUText *M15_Name[11];
	ITUWidget *M15_GrayIcon[11];

	memset(ptUserBTN,0,15);
	memset(ptUserIcon,0,15);
	memset(ptGrayUserIcon,0,15);
	memset(ptUserName,0,15);
	
	for(i=1;i<11;i++)
	{
		sprintf(ptUserBTN,"M15_U%d_BTN",i);
		sprintf(ptUserIcon,"M15_U%d",i);
		sprintf(ptGrayUserIcon,"M15_G%d",i);
		sprintf(ptUserName,"M15_U%d_Name",i);

		M15_BTN[i] = ituSceneFindWidget(&theScene, ptUserBTN);
		assert(M15_BTN[i]);
		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_GrayIcon[i] = ituSceneFindWidget(&theScene, ptGrayUserIcon);
		assert(M15_GrayIcon[i]);
		M15_Name[i] = ituSceneFindWidget(&theScene, ptUserName);
		assert(M15_Name[i]);
	}

	ituWidgetSetVisible(M15_BTN[tUserIdx],true);
	
	if(tGrayOpt == 0)
	{
		ituWidgetSetVisible(M15_GrayIcon[tUserIdx],false);
		ituWidgetSetVisible(M15_Icon[tUserIdx],true);
	}
	else if(tGrayOpt == 1)
	{
		ituWidgetSetVisible(M15_Icon[tUserIdx],false);
		ituWidgetSetVisible(M15_GrayIcon[tUserIdx],true);
	}

	memset(tDestName,0,30);
	ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx-1].UserName);
	ituTextSetString(M15_Name[tUserIdx], tDestName);

	ituWidgetSetVisible(M15_Name[tUserIdx],true);
}

void User_AllClear(void)
{
	char ptUserIcon[15],ptUserName[15],ptUserBTN[15];
	uint8_t i;

	ITUWidget *M15_BTN[11];
	ITUWidget *M15_Icon[11];
	ITUText *M15_Name[11];
	
	memset(ptUserBTN,0,15);
	memset(ptUserIcon,0,15);
	memset(ptUserName,0,15);
	
	for(i=1;i<11;i++)
	{
		sprintf(ptUserBTN,"M15_U%d_BTN",i);
		sprintf(ptUserIcon,"M15_U%d",i);
		sprintf(ptUserName,"M15_U%d_Name",i);

		M15_BTN[i] = ituSceneFindWidget(&theScene, ptUserBTN);
		assert(M15_BTN[i]);
		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_Name[i] = ituSceneFindWidget(&theScene, ptUserName);
		assert(M15_Name[i]);

		ituWidgetSetVisible(M15_BTN[i],false);
		ituWidgetSetVisible(M15_Icon[i],false);
		ituWidgetSetVisible(M15_Name[i],false);
	}
}

void User_Clear(uint8_t tUserIdx)
{
	char ptUserIcon[15],ptUserName[15],ptUserBTN[15];
	uint8_t i;

	ITUWidget *M15_BTN[11];
	ITUWidget *M15_Icon[11];
	ITUText *M15_Name[11];
	
	memset(ptUserBTN,0,15);
	memset(ptUserIcon,0,15);
	memset(ptUserName,0,15);
	
	for(i=1;i<11;i++)
	{
		sprintf(ptUserBTN,"M15_U%d_BTN",i);
		sprintf(ptUserIcon,"M15_U%d",i);
		sprintf(ptUserName,"M15_U%d_Name",i);

		M15_BTN[i] = ituSceneFindWidget(&theScene, ptUserBTN);
		assert(M15_BTN[i]);
		M15_Icon[i] = ituSceneFindWidget(&theScene, ptUserIcon);
		assert(M15_Icon[i]);
		M15_Name[i] = ituSceneFindWidget(&theScene, ptUserName);
		assert(M15_Name[i]);
	}

	ituWidgetSetVisible(M15_BTN[tUserIdx],false);
	ituWidgetSetVisible(M15_Icon[tUserIdx],false);
	ituWidgetSetVisible(M15_Name[tUserIdx],false);
}

void User_SetPassCode(uint8_t tUserIdx,uint64_t tPcode)
{
	uint8_t tPcodeDigit = 0;
	uint8_t tDigit = 0,i=0;

	uint64_t tPcode2 = 0;

	i=9;
	while(1)
	{
		tPcodeDigit = (uint8_t)(tPcode % 10);
		User_RegInfo[tUserIdx-1].UserPassCode[i] = tPcodeDigit;

		tPcode /= 10;
		tDigit++;
		i--;
		
		if(!tPcode)
			break;
	}
	
	printf("User_%d PassCode Digit %d\r\n",tUserIdx,tDigit);
	
	for(i=0;i<10;i++)
	{
		if(i==0)
			printf("Input Pass Code [%d ",User_RegInfo[tUserIdx-1].UserPassCode[i]);
		else if((i<9)&&(i>0))
			printf("%d ",User_RegInfo[tUserIdx-1].UserPassCode[i]);
		else if(i == 9)
			printf("%d]\r\n",User_RegInfo[tUserIdx-1].UserPassCode[i]);
	}
}

uint8_t PCode_Digit(uint64_t tPcode)
{
	uint8_t tDigit = 0;

	while(1)
	{
		tPcode /= 10;
		tDigit++;
		
		if(!tPcode)
		{
			break;
		}
	}
	
	return tDigit;
}

void DBG_UserNameCode(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[30];

	memset(tDestName,0,30);
	ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx-1].UserName);

	printf("User Name %d_[%s]\r\n",tUserIdx,tDestName);

	printf("SRC HexCode : ");
	for(i=0;i<30;i++)
	{
		if((uint8_t)User_RegInfo[tUserIdx-1].UserName[i] == 0)
			break;

		printf("%02X ",(uint8_t)User_RegInfo[tUserIdx-1].UserName[i]);
	}
	printf("\r\n");

	printf("DST HexCode : ");
	for(i=0;i<30;i++)
	{
		if((uint8_t)tDestName[i] == 0)
			break;

		printf("%02X ",(uint8_t)tDestName[i]);
	}
	printf("\r\n");
}

void ISO8859_to_UTF8(uint8_t *tDest,uint8_t *tSrc)
{
	uint8_t s_idx = 0,d_idx = 0;

	printf("\r\n");
	
	while(tSrc[s_idx] > 0)
	{
		if(tSrc[s_idx] < 0x80)
		{
			tDest[d_idx] = tSrc[s_idx];
			
			printf("Src[%02X],Dest[%02X]\r\n",(uint8_t)tSrc[s_idx],(uint8_t)tDest[d_idx]);

			d_idx++;
			s_idx++;
		}
		else if(tSrc[s_idx] >= 0x80)
		{
			tDest[d_idx] = (uint8_t)(0xC0|(tSrc[s_idx]>>6));
			tDest[d_idx+1] = (uint8_t)(0x80|(tSrc[s_idx]&0x3F));
			
			printf("Src[%02X],Dest[%02X %02X]\r\n",(uint8_t)tSrc[s_idx],(uint8_t)tDest[d_idx],(uint8_t)tDest[d_idx+1]);

			d_idx+=2;
			s_idx++;
		}
	}
	
//	printf("\r\n");
}

void User_M1(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 1;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"Big james");
		
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 1;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 2;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 3;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 4;
		
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0x00;
}

void User_M2(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 1;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"Jo?l");
		
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 9;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 8;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 7;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 6;
		
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0x00;
}

void User_O1(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 7;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"TAKEUCHI");
		
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 1;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 1;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 1;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 1;
		
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0x00;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0x00;
}

void User_O2(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 1;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"SPK-XO");
	//memset(User_RegInfo[tUserIdx-1].UserName,0,10);
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 2;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 2;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 2;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 2;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0x01;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0x0A;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0x02;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0x0C;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0x03;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0x0D;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0x04;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0x0E;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0x05;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0x0F;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0x06;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0x00;
}

void User_O3(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"G?nther");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 3;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 3;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 3;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 3;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}

void User_O4(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"J?r?me");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 4;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 4;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 4;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 4;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}

void User_O5(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"Fran?ois");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 5;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 5;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 5;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 5;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}

void User_O6(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"Lor?nza");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 6;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 6;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 6;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 6;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}

void User_O7(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"Beltr?n");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 7;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 7;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 7;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 7;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}

void User_O8(uint8_t tUserIdx)
{
	User_RegInfo[tUserIdx-1].RegFlag = 2;			// 0;Not Register ,1;Master ,2;Operater
	User_RegInfo[tUserIdx-1].PassDigit = 4;		// PassCode Digit
	User_RegInfo[tUserIdx-1].RFStatus = 0;			// RFID Register Status
	User_RegInfo[tUserIdx-1].MacStatus = 0;		// BT MAC ID Register status
	
	sprintf(User_RegInfo[tUserIdx-1].UserName,"�lvaro");		// iso8859-1 code
	
	User_RegInfo[tUserIdx-1].UserPassCode[0] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[1] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[2] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[3] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[4] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[5] = 0;
	User_RegInfo[tUserIdx-1].UserPassCode[6] = 8;
	User_RegInfo[tUserIdx-1].UserPassCode[7] = 8;
	User_RegInfo[tUserIdx-1].UserPassCode[8] = 8;
	User_RegInfo[tUserIdx-1].UserPassCode[9] = 8;

	//User_RegInfo[tUserIdx-1].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	User_RegInfo[tUserIdx-1].UserMacID[0] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[1] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[2] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[3] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[4] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[5] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[6] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[7] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[8] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[9] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[10] = 0;
	User_RegInfo[tUserIdx-1].UserMacID[11] = 0;
}
