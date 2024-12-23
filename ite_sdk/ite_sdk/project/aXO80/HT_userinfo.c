#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#include "HT_DispDdefine.h"
#include "ite/itp.h"
#include "Menu_Layer.h"

static bool LayerFlag = 0;
static char widgetName[30];

#define UI_GRIP_CON_CNT		20
#define UI_GRIP_ICON_CNT	23


static ITUContainer* UserInformation=NULL;
static ITUIcon* UI_OK_Sel;
static ITUIcon* UI_AUTO_Sel;
static ITUButton* UI_OK_Bt;
static ITUButton* UI_AUTO_Bt;
static ITUText* UI_UserNameTx;

static ITUIcon* UI_CON[UI_GRIP_CON_CNT];
static ITUIcon* UI_ICON[UI_GRIP_CON_CNT][UI_GRIP_ICON_CNT];


static ITUIcon* UI_TEXT[5];
static ITUIcon* UI_AUTO_ON[5];
static ITUIcon* UI_AUTO_OFF[5];
static ITUIcon* UI_TITLE[5];
static ITUIcon* UI_TITLE_KEY[5];
static ITUIcon* UI_USER_STANDARD;
static ITUIcon* UI_USER_ADMIN;
static ITUBackground* UserIF_S_BG;
static ITUBackground* UserIF_M_BG;


static ITUIcon* UI_SBG[12];
static ITUIcon* UI_MBG[14];




uint8_t UI_Timer = 0;
MainUI MUI;
uint8_t UI_SelIndex=1;


bool UI_Sel(uint8_t SelIndex);


void ResetUI(void)
{
	UserInformation=NULL;
	LayerFlag=0;
}


#define UI_ICON_CNT_S	12
#define UI_ICON_CNT_M	20
							// 			 1 ,2 , 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20
uint8_t UI_IconPos_M[UI_ICON_CNT_M] =	{1 ,1 , 2, 2, 3, 3, 4, 5, 6, 7, 8, 8, 9, 9,10,10,11,12,13,14};

							// 			 1 ,2 , 3, 4, 5, 6, 7, 8, 9,10,11,12
uint8_t UI_IconPos_S[UI_ICON_CNT_S] =	{1 ,1 , 2, 3, 4, 5, 6, 6, 7, 8, 9,10};
uint8_t UI_IconPos_S_H[UI_ICON_CNT_S] =	{1 ,2 , 3, 4, 5, 6, 7, 8, 9,10,11,12};

bool  UI_GripIconPos(uint8_t UserSA,bool ReDisp);	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준


bool UI_GripIconDisp(uint8_t UserSA,bool ReDisp)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
{
	int i,j;
	static uint8_t tUI_ICON[50]={0xFE,};
	static uint8_t tUserSA=0xFE;
	uint8_t tReDisp=0;
	
	
	if ((ReDisp==1)||(tUserSA!=UserSA))
	{
		memset (tUI_ICON,0xFE,sizeof(tUI_ICON));
		UI_GripIconPos(UserSA,ReDisp);
		tUserSA=UserSA;
	}
	
	i = 0;
	
	{
		if (UserSA==1)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			for (i=0;i<UI_ICON_CNT_M;i++)
			{	
				if ((ReDisp)||(tUI_ICON[i+1]!=MUI.TPDT[i+1]))
				{
					ituWidgetSetVisible(UI_MBG[UI_IconPos_M[i]-1], DISP_OFF);
					tReDisp = 1;
				}
			}
			for (i=0;i<UI_ICON_CNT_M;i++)
			{	
				if (tReDisp)
				{
					printf("UI_GripIconDisp : %02d==>MUI.TPDT:[%d]\n",i,MUI.TPDT[i+1]);
					for (j=0;j<UI_GRIP_ICON_CNT;j++)
					{
						ituWidgetSetVisible(UI_ICON[i][j], DISP_OFF);
					}
					if (MUI.TPDT[i+1]!=0xFF)
					{
						ituWidgetSetVisible(UI_MBG[UI_IconPos_M[i]-1], DISP_ON);
					}
					if ((MUI.TPDT[i+1]!=0)&&(MUI.TPDT[i+1]<=35))
					{
						if ((MUI.TPDT[i+1])>=32)
							ituWidgetSetVisible(UI_ICON[i][MUI.TPDT[i+1]-1-12], DISP_ON);
						else
							ituWidgetSetVisible(UI_ICON[i][MUI.TPDT[i+1]-1], DISP_ON);
					}
					tUI_ICON[i+1]=MUI.TPDT[i+1];
				}
			}
		}
		else if (UserSA==2)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			for (i=0;i<UI_ICON_CNT_S;i++)
			{
				if ((ReDisp)||(tUI_ICON[i+1]!=MUI.TPDT[i+1]))
				{
					if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
					{
						ituWidgetSetVisible(UI_SBG[UI_IconPos_S_H[i]-1], DISP_OFF);
					}
					else
					{
						ituWidgetSetVisible(UI_SBG[UI_IconPos_S[i]-1], DISP_OFF);
					}
					tReDisp = 1;
				}
			}		
			for (i=0;i<UI_ICON_CNT_S;i++)
			{
				if (tReDisp)
				{
					printf("UI_GripIconDisp : %02d==>MUI.TPDT:[%d]\n",i,MUI.TPDT[i+1]);
					for (j=0;j<UI_GRIP_ICON_CNT;j++)
						ituWidgetSetVisible(UI_ICON[i][j], DISP_OFF);
					if (MUI.TPDT[i+1]!=0xFF)
					{
						if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
						{
							ituWidgetSetVisible(UI_SBG[UI_IconPos_S_H[i]-1], DISP_ON);
						}
						else
						{
							ituWidgetSetVisible(UI_SBG[UI_IconPos_S[i]-1], DISP_ON);
						}
					}
					if ((MUI.TPDT[i+1]!=0)&&(MUI.TPDT[i+1]<=35))
					{
						if ((MUI.TPDT[i+1])>=32)
							ituWidgetSetVisible(UI_ICON[i][MUI.TPDT[i+1]-1-12], DISP_ON);
						else
							ituWidgetSetVisible(UI_ICON[i][MUI.TPDT[i+1]-1], DISP_ON);
					}
					tUI_ICON[i+1]=MUI.TPDT[i+1];
				}
			}
		}
	}
}



bool UI_GripIconPos(uint8_t UserSA,bool ReDisp)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
{
	int i,j;
	static uint8_t tUserSA=0;
	

	for (i=0;i<UI_GRIP_CON_CNT;i++)
	{
		for (j=0;j<UI_GRIP_ICON_CNT;j++)
		{
			ituWidgetSetVisible(UI_ICON[i][j], DISP_OFF);
		}
	}
	i = 0;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
	{
		if (UserSA==1)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			ituWidgetSetPosition(UI_CON[i], 78,450);		// 01
			i++;
			ituWidgetSetPosition(UI_CON[i], 78,531);		// 02
			i++;
			ituWidgetSetPosition(UI_CON[i], 261,456);		// 03
			i++;
			ituWidgetSetPosition(UI_CON[i], 338,456);		// 04
			i++;
			ituWidgetSetPosition(UI_CON[i], 490,446);		// 05
			i++;
			ituWidgetSetPosition(UI_CON[i], 568,446);		// 06
			i++;
			ituWidgetSetPosition(UI_CON[i], 78,366);		// 07  78,366	// 2024-03-09
			i++;
			ituWidgetSetPosition(UI_CON[i], 300,366);		// 08  300,366
			i++;
			ituWidgetSetPosition(UI_CON[i], 535,366);		// 09
			i++;
			ituWidgetSetPosition(UI_CON[i], 535,525);		// 10
			i++;
			ituWidgetSetPosition(UI_CON[i], 880,456);		// 11
			i++;
			ituWidgetSetPosition(UI_CON[i], 956,456);		// 12
			i++;
			ituWidgetSetPosition(UI_CON[i], 1140,450);		// 13
			i++;
			ituWidgetSetPosition(UI_CON[i], 1140,531);		// 14
			i++;
			ituWidgetSetPosition(UI_CON[i], 646,446);		// 15
			i++;
			ituWidgetSetPosition(UI_CON[i], 723,446);		// 16
			i++;
			ituWidgetSetPosition(UI_CON[i], 901,366);		// 17
			i++;
			ituWidgetSetPosition(UI_CON[i], 1140,366);		// 18
			i++;
			ituWidgetSetPosition(UI_CON[i], 684,366);		// 19
			i++;
			ituWidgetSetPosition(UI_CON[i], 684,525);		// 20
		}
		else if (UserSA==2)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			ituWidgetSetPosition(UI_CON[i], 78,382);		// 01
			i++;
			ituWidgetSetPosition(UI_CON[i], 314,382);		// 02
			i++;
			ituWidgetSetPosition(UI_CON[i], 314,462);		// 03
			i++;
			ituWidgetSetPosition(UI_CON[i], 548,440);		// 04
			i++;
			ituWidgetSetPosition(UI_CON[i], 78,462);		// 05
			i++;
			ituWidgetSetPosition(UI_CON[i], 548,518);		// 06
			i++;
			ituWidgetSetPosition(UI_CON[i], 904,382);		// 07
			i++;
			ituWidgetSetPosition(UI_CON[i], 1140,382);		// 08
			i++;
			ituWidgetSetPosition(UI_CON[i], 1140,462);		// 09
			i++;
			ituWidgetSetPosition(UI_CON[i], 677,440);		// 10
			i++;
			ituWidgetSetPosition(UI_CON[i], 904,462);		// 11
			i++;
			ituWidgetSetPosition(UI_CON[i], 677,518);		// 12
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 13
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 14
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 15
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 16
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 17
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 18
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 19
			i++;
			ituWidgetSetPosition(UI_CON[i], 1218,738);		// 20
		}	//if (UserSA==1)
	}
	else
	{
		if (UserSA==1)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			ituWidgetSetPosition(UI_CON[i], 68,537);		// 01
			i++;
			ituWidgetSetPosition(UI_CON[i], 68,618);		// 02
			i++;
			ituWidgetSetPosition(UI_CON[i], 250,553);		// 03
			i++;
			ituWidgetSetPosition(UI_CON[i], 328,553);		// 04
			i++;
			ituWidgetSetPosition(UI_CON[i], 250,791);		// 05
			i++;
			ituWidgetSetPosition(UI_CON[i], 328,791);		// 06
			i++;
			ituWidgetSetPosition(UI_CON[i], 68,463);		// 07  	325,463// 2024-03-09
			i++;
			ituWidgetSetPosition(UI_CON[i], 325,463);		// 08	68,463
			i++;
			ituWidgetSetPosition(UI_CON[i], 68,724);		// 09
			i++;
			ituWidgetSetPosition(UI_CON[i], 68,826);		// 10
			i++;
			ituWidgetSetPosition(UI_CON[i], 408,553);		// 11
			i++;
			ituWidgetSetPosition(UI_CON[i], 486,553);		// 12
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,537);		// 13
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,618);		// 14
			i++;
			ituWidgetSetPosition(UI_CON[i], 408,791);		// 15
			i++;
			ituWidgetSetPosition(UI_CON[i], 486,791);		// 16
			i++;
			ituWidgetSetPosition(UI_CON[i], 414,463);		// 17
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,463);		// 18
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,724);		// 19
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,826);		// 20
		}
		else if (UserSA==2)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
		{
			ituWidgetSetPosition(UI_CON[i], 147,437);		// 01
			i++;
			ituWidgetSetPosition(UI_CON[i], 224,437);		// 02
			i++;
			ituWidgetSetPosition(UI_CON[i], 305,590);		// 03
			i++;
			ituWidgetSetPosition(UI_CON[i], 305,748);		// 04
			i++;
			ituWidgetSetPosition(UI_CON[i], 68,590);		// 05
			i++;
			ituWidgetSetPosition(UI_CON[i], 305,826);		// 06
			i++;
			ituWidgetSetPosition(UI_CON[i], 513,437);		// 07
			i++;
			ituWidgetSetPosition(UI_CON[i], 590,437);		// 08
			i++;
			ituWidgetSetPosition(UI_CON[i], 670,590);		// 09
			i++;
			ituWidgetSetPosition(UI_CON[i], 434,748);		// 10
			i++;
			ituWidgetSetPosition(UI_CON[i], 434,590);		// 11
			i++;
			ituWidgetSetPosition(UI_CON[i], 434,826);		// 12
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 13
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 14
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 15
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 16
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 17
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 18
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 19
			i++;
			ituWidgetSetPosition(UI_CON[i], 738,1218);		// 20
		}	//if (UserSA==1)
	}
}




bool UI_UserEngineAuto(int IconIndex,DISP_MODE DispMode,bool ReDisp)
{
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
	static uint8_t tAutoStop=0xFE;
	static uint8_t tAutoStopKey=0xFE;
	static uint8_t tLang_Flag=0xFE;
	int i;

	if (LayerFlag)
	{
		if ((IconIndex!=tIconIndex)||(DispMode!=tDispMode)||(Option_Function.AutoStop[IconIndex]!=tAutoStop)||(Option_Function.AutoStopKey!=tAutoStopKey)||(Lang_Flag[IconIndex]!=tLang_Flag)||(ReDisp))
		{
			printf("UI_UserEngineAuto==>IconIndex:[%d], Lang_Flag:[%d], AutoStop:[%d]\n",IconIndex,Lang_Flag[IconIndex],Option_Function.AutoStop[IconIndex]);
			for (i=Lang_ENG;i<=Lang_ESP;i++)
			{
				ituWidgetSetVisible(UI_AUTO_ON[i], DISP_OFF);
				ituWidgetSetVisible(UI_AUTO_OFF[i], DISP_OFF);

//				printf("UI_AUTO_ON:[%d] DISP_OFF\n",i);
//				printf("UI_AUTO_OFF:[%d] DISP_OFF\n",i);
			}
#if 1


			if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
			{
				if (Option_Function.AutoStop[IconIndex]==1)
				{
					ituWidgetSetVisible(UI_AUTO_ON[Lang_Flag[IconIndex]], DISP_ON);
	//				printf("UI_AUTO_ON:[%d] DISP_ON\n",Lang_Flag[IconIndex]);
				}
				else
				{
					ituWidgetSetVisible(UI_AUTO_OFF[Lang_Flag[IconIndex]], DISP_ON);
	//				printf("UI_AUTO_OFF:[%d] DISP_ON\n",Lang_Flag[IconIndex]);
				}
			}
			else
			{
				if (Option_Function.AutoStopKey==1)
				{
					ituWidgetSetVisible(UI_AUTO_ON[Lang_Flag[IconIndex]], DISP_ON);
	//				printf("UI_AUTO_ON:[%d] DISP_ON\n",Lang_Flag[IconIndex]);
				}
				else
				{
					ituWidgetSetVisible(UI_AUTO_OFF[Lang_Flag[IconIndex]], DISP_ON);
	//				printf("UI_AUTO_OFF:[%d] DISP_ON\n",Lang_Flag[IconIndex]);
				}
			}
#endif
			tIconIndex=IconIndex;
			tDispMode=DispMode;
			tAutoStop=Option_Function.AutoStop[IconIndex];
			tAutoStopKey=Option_Function.AutoStopKey;

			tLang_Flag=Lang_Flag[IconIndex];
		}
	}
	return true;
}


bool UI_UserIcon (int IconIndex,DISP_MODE DispMode,bool ReDisp)
{
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
	
	if (LayerFlag)
	{
		if ((IconIndex!=tIconIndex)||(DispMode!=tDispMode)||(ReDisp))
		{
			printf("UI_UserIcon==>IconIndex:[%d],RegFlag:[%d],ReDisp:[%d]\n",IconIndex,User_RegInfo[IconIndex].RegFlag,ReDisp);
			ituWidgetSetVisible(UI_USER_STANDARD, DISP_OFF);
			ituWidgetSetVisible(UI_USER_ADMIN, DISP_OFF);
			if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY(Default)  2:M7_MODE_LOCK
				ituWidgetSetVisible(UI_USER_STANDARD, DISP_ON);
			else if ((User_RegInfo[IconIndex].RegFlag == 5))
			{
				ituWidgetSetVisible(UI_USER_ADMIN, DISP_ON);
			}
			else if ((User_RegInfo[IconIndex].RegFlag == 2)||(User_RegInfo[IconIndex].RegFlag == 3)||(User_RegInfo[IconIndex].RegFlag == 9))
			{
				ituWidgetSetVisible(UI_USER_STANDARD, DISP_ON);
			}
			tIconIndex=IconIndex;
			tDispMode=DispMode;			
		}
	}
	return true;
}

bool UI_UserName(int IconIndex,DISP_MODE DispMode,bool ReDisp)
{
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
	char tmp[50];
	
	if (LayerFlag)
	{
		if ((IconIndex!=tIconIndex)||(DispMode!=tDispMode)||(ReDisp))
		{
			printf("UI_UserName==>IconIndex:[%d],gUI_Server_Mode:[%d],ReDisp:[%d]\n",IconIndex,gUI_Server_Mode,ReDisp);
			ituWidgetSetVisible(UI_UserNameTx, DISP_OFF);
			if ((DispMode==DISP_ON)&&(gUI_Server_Mode==1)&&(M7_UserIndex<MAX_SU100_USER_NUM))
			{
				sprintf(tmp, "%s", User_RegInfo[IconIndex].UserName);
				ituTextSetString(UI_UserNameTx, tmp);
				if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY(Default)  2:M7_MODE_LOCK
					ituWidgetSetVisible(UI_UserNameTx, DISP_ON);
			}
			else
				ituWidgetSetVisible(UI_UserNameTx, DISP_OFF);
			tIconIndex=IconIndex;
			tDispMode=DispMode;
		}
	}
	return true;
}


bool UI_BG(int GripMode , int IconIndex,DISP_MODE DispMode,bool ReDisp)
{
	static uint8_t tGripMode=0;
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
	int i;
		
	if (LayerFlag)
	{
		if ((GripMode!=tGripMode)||(IconIndex!=tIconIndex)||(DispMode!=tDispMode)||(ReDisp))
		{
			printf("UI_BG==>GripMode:[%d],IconIndex:[%d],tIconIndex:[%d],DispMode:[%d],tDispMode:[%d],ReDisp:[%d],MUI.Status:[%d]\n",GripMode,IconIndex,tIconIndex,DispMode,tDispMode,ReDisp,MUI.Status);
			ituWidgetSetVisible(UserIF_S_BG, DISP_OFF);
			ituWidgetSetVisible(UserIF_M_BG, DISP_OFF);
			
			if (GripMode == 2)	// XO ==> 0:OFF , 1:다기능그립(KPM제) , 2:표준
			{
				ituWidgetSetVisible(UserIF_S_BG, DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(UserIF_M_BG, DISP_ON);
			}
			for (i=Lang_ENG;i<=Lang_ESP;i++)
			{
				ituWidgetSetVisible(UI_TITLE[i], DISP_OFF);
				ituWidgetSetVisible(UI_TITLE_KEY[i], DISP_OFF);
				ituWidgetSetVisible(UI_TEXT[i], DISP_OFF);
			}
			
			for (i=0;i<UI_ICON_CNT_M;i++)
			{	
				ituWidgetSetVisible(UI_MBG[UI_IconPos_M[i]-1], DISP_OFF);
			}
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				for (i=0;i<UI_ICON_CNT_S;i++)
				{
					ituWidgetSetVisible(UI_SBG[UI_IconPos_S_H[i]-1], DISP_OFF);
				}
			}
			else
			{
				for (i=0;i<UI_ICON_CNT_S;i++)
				{
					ituWidgetSetVisible(UI_SBG[UI_IconPos_S[i]-1], DISP_OFF);
				}
			}

			ituWidgetSetVisible(UI_TITLE[Lang_Flag[M7_UserIndex]], DISP_OFF);
			ituWidgetSetVisible(UI_TITLE_KEY[Lang_Flag[M7_UserIndex]], DISP_OFF);	
			if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY(Default)  2:M7_MODE_LOCK
			{
				ituWidgetSetVisible(UI_TITLE[Lang_Flag[M7_UserIndex]], DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(UI_TITLE_KEY[Lang_Flag[M7_UserIndex]], DISP_ON);
			}
			ituWidgetSetVisible(UI_TEXT[Lang_Flag[M7_UserIndex]], DISP_ON);
			tGripMode=GripMode;
			tIconIndex=IconIndex;
			tDispMode=DispMode;			
		}
	}
	return true;
}
bool UserInformation_OnOff (uint8_t OnOff)
{
	SetWidget(UserInformation,"UserInformation");
	if (OnOff==DISP_ON)
	{
		ituWidgetSetVisible(UserInformation, DISP_ON);
		MainButton_OnOff (DISP_OFF);
	}
	else
	{
		ituWidgetSetVisible(UserInformation, DISP_OFF);
		MainButton_OnOff (DISP_ON);
	}
}


bool UserInfoOnEnter(void)
{
	int j,i=0;
	printf("UserInfoOnEnter=%d,M7_UserIndex:%d %d[%d]\n",LayerFlag,M7_UserIndex,User_RegInfo[M7_UserIndex].RegFlag,g_nM7_StartType);
	
	Main_Disp_Mode=MAIN_MODE_USERINFO;
	if ((LayerFlag==0)||(Main_Disp_Mode==MAIN_MODE_USERINFO))
	{

		SetWidget(UI_OK_Sel,"UI_OK_Sel");
		SetWidget(UI_AUTO_Sel,"UI_AUTO_Sel");
		SetWidget(UI_OK_Bt,"UI_OK_Bt");
		SetWidget(UI_AUTO_Bt,"UI_AUTO_Bt");
		for (i=0;i<UI_GRIP_CON_CNT;i++)
		{
			sprintf(widgetName, "UI_CON_%02d",i+1);
			SetWidget(UI_CON[i],widgetName);
			for (j=0;j<UI_GRIP_ICON_CNT;j++)
			{
				sprintf(widgetName, "UI_C%02d_%02d",i+1,j+1);
				SetWidget(UI_ICON[i][j],widgetName);
			}
		}
		
		for (i=0;i<UI_ICON_CNT_S;i++)
		{
			sprintf(widgetName, "UI_SBG_%02d",i+1);
			SetWidget(UI_SBG[i],widgetName);
		}
		for (i=0;i<14;i++)
		{
			sprintf(widgetName, "UI_MBG_%02d",i+1);
			SetWidget(UI_MBG[i],widgetName);
		}		
		
				
		SetWidget(UI_UserNameTx,"UI_UserNameTx");
		for (i=Lang_ENG;i<=Lang_ESP;i++)
		{
			sprintf(widgetName, "UI_AUTO_ON_%01d",i);
//			printf("UI_AUTO_ON:[%d][%s]\n",i,widgetName);
			SetWidget(UI_AUTO_ON[i],widgetName);
			
			sprintf(widgetName, "UI_AUTO_OFF_%01d",i);
//			printf("UI_AUTO_OFF:[%d][%s]\n",i,widgetName);
			SetWidget(UI_AUTO_OFF[i],widgetName);

			sprintf(widgetName, "UI_TITLE_%01d",i);
			SetWidget(UI_TITLE[i],widgetName);
			sprintf(widgetName, "UI_TITLE_K%01d",i);
			SetWidget(UI_TITLE_KEY[i],widgetName);

			sprintf(widgetName, "UI_TEXT_%01d",i);
			SetWidget(UI_TEXT[i],widgetName);
		}
		
		SetWidget(UI_USER_STANDARD,"UI_USER_STANDARD");
		SetWidget(UI_USER_ADMIN,"UI_USER_ADMIN");
		SetWidget(UserIF_S_BG,"UserIF_S_BG");
		SetWidget(UserIF_M_BG,"UserIF_M_BG");
		
		
		LayerFlag = 1;
	}

	if ((Lang_Flag[M7_UserIndex]<Lang_ENG)||(Lang_Flag[M7_UserIndex]>Lang_ESP))
	{
		Lang_Flag[M7_UserIndex] = Lang_ENG;
	}

		UI_BG(MUI.TPDT[0],M7_UserIndex,DISP_ON,1);
		UI_UserName(M7_UserIndex,DISP_ON,1);
		UI_UserIcon (M7_UserIndex,DISP_ON,1);
		UI_UserEngineAuto(M7_UserIndex,DISP_ON,1);
		UI_GripIconDisp(MUI.TPDT[0],1);
		
		
	UI_Sel(UI_SelIndex);

	ReadyRotation();	// JPHong 2023-10-24
	ScreenSetOnOff(1);	// JPHong 2023-12-04
	
	UI_Timer = gMainTimerCnt_10m;
	
	return true;
}


bool UI_OK_Bt_Func(ITUWidget* widget, char* param);
bool UI_AUTO_Bt_Func(ITUWidget* widget, char* param);



bool UserInfoJDR(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	if (LayerFlag==0)
		return 0;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	JogData = param[0];
//	printf("UserInfoJDR= '0x%X' %d\n",JogData,LogoPWSelIndex);

	{
		switch (JogData)
		{
			case JOG_DIAL_ROT_RIGHT:
			case JOG_DIAL_ROT_LEFT:
			{
				if (UI_SelIndex==0)
				{
					UI_SelIndex = 1;
				}
				else
				{
					UI_SelIndex = 0;
				}
				UI_Sel(UI_SelIndex);
			}
			break;	
		}
	}
    return true;
}

bool UserInfoJDK(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	char JD_Buf[10] = {0,};
	
	if (LayerFlag==0)
		return 0;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	JogData = param[0];

	{
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			switch (JogData)
			{
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
				{
					if (UI_SelIndex==0)
					{
						UI_SelIndex = 1;
					}
					else
					{
						UI_SelIndex = 0;
					}
					UI_Sel(UI_SelIndex);
				}
				break;
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					if (UI_SelIndex==0)
					{
						JD_Buf[0] = 0x30;
						UI_AUTO_Bt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0x30;
						UI_OK_Bt_Func(NULL, JD_Buf);
					}			
				}
				break;
				
			}
		}
		else	//if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			switch (JogData)
			{
				case JOG_DIAL_KEY_LEFT:
				{
				}
				break;
				case JOG_DIAL_KEY_RIGHT:
				{
				}
				break;
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
				{
					if (UI_SelIndex==0)
					{
						UI_SelIndex = 1;
					}
					else
					{
						UI_SelIndex = 0;
					}
					UI_Sel(UI_SelIndex);
				}
				break;
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					if (UI_SelIndex==0)
					{
						JD_Buf[0] = 0x30;
						UI_AUTO_Bt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0x30;
						UI_OK_Bt_Func(NULL, JD_Buf);
					}				
				}
				break;
				
			}
		}
	}
	
    return true;
}

bool UserInfoJDS(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	
	if (LayerFlag==0)
		return 0;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	JogData = param[0];

	if (JogData==JOG_DIAL_SW_ESC_ON_OFF)
	{
	}

    return true;
}



bool UI_Sel(uint8_t SelIndex)
{
	char i;
	#ifdef TEST_ROTATION
		return true;
	#endif
	
	printf("\n===>UI_Sel:%d\n",SelIndex);
	
	
	if (LayerFlag)
	{
		ituWidgetSetVisible(UI_OK_Sel, DISP_OFF);
		ituWidgetSetVisible(UI_AUTO_Sel, DISP_OFF);
		
		if (SelIndex==0)
		{
			ituWidgetSetVisible(UI_AUTO_Sel, DISP_ON);
			UI_SelIndex = SelIndex;
		}
		else if (SelIndex==1)
		{
			ituWidgetSetVisible(UI_OK_Sel, DISP_ON);
			UI_SelIndex = SelIndex;
		}
	}
}


bool UI_OK_Bt_Func(ITUWidget* widget, char* param)
{
	static uint8_t UI_BtData = 0xFF;
	UI_BtData=(param[0]&0x0F);
	
	printf("\n===>UI_OK_Bt_Func:%d\n",UI_BtData);
	if (UI_BtData==1)
	{
		UI_Sel(1);
	}
	else
	{
		Main_CLI3[7] = Main_CLI3[7]|0x01;
		// GotoMain(MAIN_MODE_NORMAL);
		UserInformation_OnOff (DISP_OFF);
		Main_Disp_Mode = MAIN_MODE_NORMAL;
	}
	
    return true;
}

bool UI_AUTO_Bt_Func(ITUWidget* widget, char* param)
{
	static uint8_t UI_BtData = 0xFF;
	UI_BtData=(param[0]&0x0F);
	
	printf("\n===>UI_AUTO_Bt_Func:%d\n",UI_BtData);
	
	if (UI_BtData==1)
	{
		UI_Sel(0);
	}
	else
	{		
		if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		{
			if (Option_Function.AutoStop[M7_UserIndex]==0)
				Option_Function.AutoStop[M7_UserIndex] = 1;
			else
				Option_Function.AutoStop[M7_UserIndex] = 0;
			UI_UserEngineAuto(M7_UserIndex,DISP_ON,1);
			#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				//Option_Function.AutoStop
				XO_Save_Config(M7_UserIndex,true,0);
			#else
				HT_Save_Config(1);
			#endif
		}
		else
		{
			if (Option_Function.AutoStopKey==0)
				Option_Function.AutoStopKey = 1;
			else
				Option_Function.AutoStopKey = 0;
			UI_UserEngineAuto(M7_UserIndex,DISP_ON,1);
			#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				//Option_Function.AutoStop
				XO_Save_Config(M7_UserIndex,true,true);  //JPHong 2024-12-18 
			#else
				HT_Save_Config(1);
			#endif				
		}
	}
	
    return true;
}


