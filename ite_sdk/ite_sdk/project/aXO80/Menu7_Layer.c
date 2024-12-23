#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

static bool bLayer_Changing = false;//chpark.24.10.15
static bool bTest_Lock;
//chpark.24.09.12
static int v_small_icon_pos[10][2] = {    //x,y 좌표.
    { 44, 219 },//1
	{ 44, 378 },//2
	 { 44, 537 },//3
	 { 44, 696 },//4
	 { 44, 855 },//5
	 { 44, 1014 },//6

	 { 460, 219 },//7
	 { 460, 378 },//8
	 { 460, 537 },//9
	 { 460, 696 }//10
};

static int v_big_icon_pos[10][2] = {    //x,y 좌표.
	{ 44, 195 },//1
	{ 44, 339 },//2
	 { 44, 513 },//3
	 { 44, 672 },//4
	 { 44, 831 },//5
	 { 44, 990 },//6

	 { 460, 195 },//7
	 { 460, 339 },//8
	 { 460, 513 },//9
	 { 460, 672 }//10
};

static int h_small_icon_pos[10][2] = {    //x,y 좌표.
    { 44, 219 },//1
	{ 44, 378 },//2
	{ 44, 537 },//3
	 { 580, 219 },//4
	 { 580, 378 },//5
	 { 580, 537 },//6
	 	{ 1000, 219 },//7
	 	{ 1000, 378 },//8
	 	{ 1000, 537 },//9
	 		{ 1200, 537 }//10 xx
};

static int h_big_icon_pos[10][2] = {    //x,y 좌표.
	{ 44, 195 },//1
	{ 44, 339 },//2
	{ 44, 513 },//3
	 { 580, 195 },//4
	 { 580, 339 },//5
	 { 580, 513 },//6
	   { 1000, 195 },//7
	   { 1000, 339 },//8
	   { 1000, 513 },//9
	     { 1200, 513 }//10 xx
};

#define ICON_1st 0
#define ICON_2nd 1
#define ICON_3rd 2


#define Lock_Icon3_Left  0
#define Lock_Icon3_right 1

static int v_lock_icon_pos[2][2] = {    //x,y 좌표.
	{ 189, 511 },//left
	    { 605, 511 }//right
};

static int h_lock_icon_pos[2][2] = {    //x,y 좌표.
	{ 189, 511 },//left
	    { 1145, 511 }//right
};


bool M7_JogWheel_KeyMode(char* param); //chpark.24.09.26
bool M7_JogWheel_LockMode(char* param);
bool M7_JogButton_KeyMode(char* param);
bool M7_JogButton_LockMode(char* param);

 static ITUWidget* M7_TEST_BTN;
 
static ITUWidget* M7_CONT_ICON_LOCK;
static ITUWidget* M7_ICON_LOCK[3][2];  //0:M7_LSTATE_UNLOCK   1:M7_LSTATE_LOCK

//chpark.24.09.12
static ITUButton* M7_13_ZoomBTN;
static ITUAnimation* M7_13_ANI;
static ITUIcon* M7_13_Up1;
static ITUIcon* M7_13_Up0;
static ITUButton* M7_13_BTN;

static ITUButton* M7_12_ZoomBTN;
static ITUAnimation* M7_12_ANI;
static ITUIcon* M7_12_Up1;
static ITUIcon* M7_12_Up0;
static ITUButton* M7_12_BTN;
static ITUWidget* M7_12_G;
//

static ITUIcon* M7_11_G;
static ITUButton* M7_11_ZoomBTN;
static ITUAnimation* M7_11_ANI;
static ITUIcon* M7_11_Up1;
static ITUIcon* M7_11_Up0;
static ITUButton* M7_11_BTN;
static ITUIcon* M7_4_G;
static ITUButton* M7_4_ZoomBTN;
static ITUAnimation* M7_4_ANI;
static ITUIcon* M7_4_Up1;
static ITUIcon* M7_4_Up0;
static ITUButton* M7_4_BTN;
static ITUButton* M7_9_ZoomBTN;
static ITUAnimation* M7_9_ANI;
static ITUIcon* M7_9_Up1;
static ITUIcon* M7_9_Up0;
static ITUButton* M7_9_BTN;
static ITUButton* M7_7_ZoomBTN;
static ITUAnimation* M7_7_ANI;
static ITUIcon* M7_7_Up1;
static ITUIcon* M7_7_Up0;
static ITUButton* M7_7_BTN;
static ITUButton* M7_6_ZoomBTN;
static ITUAnimation* M7_6_ANI;
static ITUIcon* M7_6_Up1;
static ITUIcon* M7_6_Up0;
static ITUButton* M7_6_BTN;
static ITUButton* M7_5_ZoomBTN;
static ITUAnimation* M7_5_ANI;
static ITUIcon* M7_5_Up1;
static ITUIcon* M7_5_Up0;
static ITUButton* M7_5_BTN;
static ITUButton* M7_2b_ZoomBTN;
static ITUAnimation* M7_2b_ANI;
static ITUIcon* M7_2b_Up1;
static ITUIcon* M7_2b_Up0;
static ITUButton* M7_2b_BTN;
static ITUButton* M7_2a_ZoomBTN;
static ITUAnimation* M7_2a_ANI;
static ITUIcon* M7_2a_Up1;
static ITUIcon* M7_2a_Up0;
static ITUButton* M7_2a_BTN;
static ITUButton* M7_1_ZoomBTN;
static ITUAnimation* M7_1_ANI;
static ITUIcon* M7_1_Up1;
static ITUIcon* M7_1_Up0;
static ITUButton* M7_1_BTN;

static uint8_t M7_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_2a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_2b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M7_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down //chpark.24.09.12
static uint8_t M7_13_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down //chpark.24.09.12

static uint32_t M7_SecTimer;
static uint8_t M7_ExitCNT = 0;
static char M7_PARA[2];
bool Menu7LayerFlag = 0;
static uint8_t M7_Lang = 0;

uint8_t M7_AG_OptFlag = 0;		// CAN[OPI] 7byte,2~3bit 0:None Opt , 1:On Opt , 2:On Opt(Hold) , 3:None Function
uint8_t M7_AP_OptFlag = 0;		// CAN[N/A   ] 6byte,4~5bit 0:None Opt , 1:On Opt , 2:Off Opt(Hold) , 3:No Visual

//chpark.24.09.12
void M7_InitPosition(int nType);
void M7_IconMovePosition(int nNameNum, int nPos, bool bShow);
void M7_Show_LockData_AP();
void M7_Show_LockData();
int  M7_PW_GetCntNullChar();


void GotoMenu7(void);
void M7_SubMenuClear(uint8_t MenuIndex);
void M7_TitleUpdate(uint8_t tLang);
void Branch_SubMenu(uint8_t tSubMenu);

bool M7_AniMenuStatus(void);

void M7_SU_Display(uint8_t tDis_Flag);
void M7_AG_Display(uint8_t tDis_Flag);
void M7_AP_Display(uint8_t tDis_Flag);

bool M7_BTN_MouseDn(ITUWidget* widget, char* param);
bool M7_BTN_MouseUp(ITUWidget* widget, char* param);

bool M7_UserDisplay(void);

extern void GotoMenuTop(void);
bool M7_BTN_MouseUpFlag = 0;

bool CheckM6_9(void)
{
	//chpark.24.10.23
	if(g_nM7_StartType != M7_MODE_PUSH)
	{
		return 0;
	}

	//chpark.24.11.01
	if ((User_RegInfo[M7_UserIndex].RegFlag == 5)&&((gMenu_UI_Time != 0)&&((UserPW_Time_AUI1+TIME_CNT_5SEC)>gMainTimerCnt_10m)))
	{
		return 1;
	}
	else if(User_RegInfo[M7_UserIndex].RegFlag != 5)
	{
		return 0;
	}
	else
	{
		return 0;
	}
	/*
	if ((User_RegInfo[M7_UserIndex].RegFlag == 5)||((gMenu_UI_Time != 0)&&((UserPW_Time_AUI1+TIME_CNT_5SEC)>gMainTimerCnt_10m)))
		return 1;
	else
		return 0;
	*/
}

bool M7_Enter(ITUWidget* widget, char* param)
{
	M7_Lang = Lang_Flag[M7_UserIndex];		// S5 set

	M7_BTN_MouseUpFlag = 0;
	if (Menu7LayerFlag==0)
	{
		printf("[M7_Enter]SetInitAgain\n");
		//chpark.24.09.12
		SetWidget(M7_CONT_ICON_LOCK,"M7_Cont_Lock");
		for(int i=0;i<3;i++)
		{
			char tmp_WidgetName[50];
 			memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
            sprintf(tmp_WidgetName, "M7_Lock_%d",i+1);
            SetWidget(M7_ICON_LOCK[i][M7_LSTATE_LOCK],tmp_WidgetName);

			memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
            sprintf(tmp_WidgetName, "M7_UnLock_%d",i+1);
            SetWidget(M7_ICON_LOCK[i][M7_LSTATE_UNLOCK],tmp_WidgetName);
		}

		SetWidget(M7_13_ZoomBTN,"M7_13_ZoomBTN");
		SetWidget(M7_13_ANI,"M7_13_ANI");
		SetWidget(M7_13_Up1,"M7_13_Up1");
		SetWidget(M7_13_Up0,"M7_13_Up0");
		SetWidget(M7_13_BTN,"M7_13_BTN");

		SetWidget(M7_12_ZoomBTN,"M7_12_ZoomBTN");
		SetWidget(M7_12_ANI,"M7_12_ANI");
		SetWidget(M7_12_Up1,"M7_12_Up1");
		SetWidget(M7_12_Up0,"M7_12_Up0");
		SetWidget(M7_12_BTN,"M7_12_BTN");
		SetWidget(M7_12_G,	"M7_12_G");
		//

		SetWidget(M7_11_G,"M7_11_G");
		SetWidget(M7_11_ZoomBTN,"M7_11_ZoomBTN");
		SetWidget(M7_11_ANI,"M7_11_ANI");
		SetWidget(M7_11_Up1,"M7_11_Up1");
		SetWidget(M7_11_Up0,"M7_11_Up0");
		SetWidget(M7_11_BTN,"M7_11_BTN");
		SetWidget(M7_4_G,"M7_4_G");
		SetWidget(M7_4_ZoomBTN,"M7_4_ZoomBTN");
		SetWidget(M7_4_ANI,"M7_4_ANI");
		SetWidget(M7_4_Up1,"M7_4_Up1");
		SetWidget(M7_4_Up0,"M7_4_Up0");
		SetWidget(M7_4_BTN,"M7_4_BTN");
		SetWidget(M7_9_ZoomBTN,"M7_9_ZoomBTN");
		SetWidget(M7_9_ANI,"M7_9_ANI");
		SetWidget(M7_9_Up1,"M7_9_Up1");
		SetWidget(M7_9_Up0,"M7_9_Up0");
		SetWidget(M7_9_BTN,"M7_9_BTN");
		SetWidget(M7_7_ZoomBTN,"M7_7_ZoomBTN");
		SetWidget(M7_7_ANI,"M7_7_ANI");
		SetWidget(M7_7_Up1,"M7_7_Up1");
		SetWidget(M7_7_Up0,"M7_7_Up0");
		SetWidget(M7_7_BTN,"M7_7_BTN");
		SetWidget(M7_6_ZoomBTN,"M7_6_ZoomBTN");
		SetWidget(M7_6_ANI,"M7_6_ANI");
		SetWidget(M7_6_Up1,"M7_6_Up1");
		SetWidget(M7_6_Up0,"M7_6_Up0");
		SetWidget(M7_6_BTN,"M7_6_BTN");
		SetWidget(M7_5_ZoomBTN,"M7_5_ZoomBTN");
		SetWidget(M7_5_ANI,"M7_5_ANI");
		SetWidget(M7_5_Up1,"M7_5_Up1");
		SetWidget(M7_5_Up0,"M7_5_Up0");
		SetWidget(M7_5_BTN,"M7_5_BTN");
		SetWidget(M7_2b_ZoomBTN,"M7_2b_ZoomBTN");
		SetWidget(M7_2b_ANI,"M7_2b_ANI");
		SetWidget(M7_2b_Up1,"M7_2b_Up1");
		SetWidget(M7_2b_Up0,"M7_2b_Up0");
		SetWidget(M7_2b_BTN,"M7_2b_BTN");
		SetWidget(M7_2a_ZoomBTN,"M7_2a_ZoomBTN");
		SetWidget(M7_2a_ANI,"M7_2a_ANI");
		SetWidget(M7_2a_Up1,"M7_2a_Up1");
		SetWidget(M7_2a_Up0,"M7_2a_Up0");
		SetWidget(M7_2a_BTN,"M7_2a_BTN");
		SetWidget(M7_1_ZoomBTN,"M7_1_ZoomBTN");
		SetWidget(M7_1_ANI,"M7_1_ANI");
		SetWidget(M7_1_Up1,"M7_1_Up1");
		SetWidget(M7_1_Up0,"M7_1_Up0");
		SetWidget(M7_1_BTN,"M7_1_BTN");

		SetWidget(M7_TEST_BTN,"M7_Test_BTN_1");
		
		Menu7LayerFlag = 1;
	}

	M7_TitleUpdate(M7_Lang);
	//chpark.24.10.31
	#ifdef RELEASE_VER
		ituWidgetSetVisible(M7_TEST_BTN, false);
	#else
		ituWidgetSetVisible(M7_TEST_BTN, true);
	#endif
//	M7_SubMenuClear();
	
	//User_RegInfo[M7_UserIndex].RegFlag = 9;			// dev.opt
	M7_InitPosition(g_nM7_StartType);//chpark.24.09.12

	#ifdef TEST_USER_SET_ICON		// 	JPHong2024-02-07	Menu7_Layer	M7_9_ZoomBTN
		if(g_nM7_StartType == M7_MODE_PUSH)
		{
			User_RegInfo[M7_UserIndex].RegFlag = 5;
		}
	#endif

	
	
	//chpark.24.10.15
	if(g_nM7_StartType==M7_MODE_PUSH)
	{
		printf("g_nM7_StartType=%d User_RegInfo[%d].RegFlag = %d\n",g_nM7_StartType,M7_UserIndex,User_RegInfo[M7_UserIndex].RegFlag);
		if((UserPW_Time_AUI1+TIME_CNT_5SEC)>gMainTimerCnt_10m)
		{
			//printf("[M7_Enter][Error] UserPW_Time_AUI1=%d gMainTimerCnt_10m=%d\n",UserPW_Time_AUI1+TIME_CNT_5SEC,gMainTimerCnt_10m);
		}

		if(CheckM6_9()==1)
		{
			M7_SU_Display(1);		// display on
		}
		else
		{
			M7_SU_Display(0);		// display off
		}
	}
	else
	{
		M7_SU_Display(0);		// display off
	}

	//M7_AG_OptFlag = 1;			// dev.opt
	if(M7_AG_OptFlag == 0)
	{
		M7_AG_Display(0);			// gray out
	}
	else if(M7_AG_OptFlag == 3)
	{
		M7_AG_Display(3);			// display off
	}

	M7_AP_OptFlag = 1; //for test
	M7_AP_Display(M7_AP_OptFlag);	// JPHong 2024-03-13

	M7_UserDisplay();
	
	M7_SecTimer = gMainTimerCnt_10m;
	M7_ExitCNT = 0;

    return true;
}


//chpark.24.09.12
void M7_InitPosition(int nType)
{
/*
	if(MenuPreset_Choice_Index == mPreset_1st)
	{
		ituWidgetSetVisible(M7_1_BTN, false);
	}
	
	if(MenuPreset_Choice_Index == mPreset_2nd)
	{
		ituWidgetSetVisible(M7_2a_BTN, false);
		ituWidgetSetVisible(M7_2b_BTN, false);
	}
	printf("[M7_InitPosition]g_nM7_StartType:%d choiceIndex=%d\n", g_nM7_StartType,MenuPreset_Choice_Index);
*/
	bLayer_Changing = true;

	if (nType == M7_MODE_PUSH)
	{
		//push start
		//left
		M7_IconMovePosition(5, 2, true); //M1_5_BTN M1_5_ANI M7_5_ZoomBTN ->2

		M7_IconMovePosition(6, 3, true); //M1_6_ANI->3
		M7_IconMovePosition(7, 4, true); //M1_7_ANI->4
		M7_IconMovePosition(9, 5, true); //M1_9_ANI->5  //user

		//right
		M7_IconMovePosition(4, 6, true); //M7_4_ANI->6
		M7_IconMovePosition(11, 7, true); //M7_11_ANI->7

		M7_IconMovePosition(12, 8, false); //M7_12_ANI->x
		M7_IconMovePosition(13, 9, false); //M7_13_ANI->x
		
	}
	else if (nType == M7_MODE_KEY)
	{
		int nNumOfErrorChar = M7_PW_GetCntNullChar();
		if(nNumOfErrorChar>0)
		{
			//PW Format Error. force reset.
			g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
		}
		//lock state error check.
		for(int i=0;i<3;i++)
		{
			if(g_nM7_Lock_Data[i].Lock<M7_LSTATE_UNLOCK || g_nM7_Lock_Data[i].Lock>M7_LSTATE_LOCK)
			{
				printf("[M7_InitPosition] g_nM7_Lock_Data[%d].Lock error=>%d.\n",i,g_nM7_Lock_Data[i].Lock);
				g_nM7_Lock_Data[i].Lock = M7_LSTATE_UNLOCK; //default.
				printf("[M7_InitPosition] g_nM7_Lock_Data[%d].Lock error=>default setting=>%d\n",i,g_nM7_Lock_Data[i].Lock);
			}
			
			if(g_nM7_Lock_Data[i].Show<M7_LSTATE_HIDE || g_nM7_Lock_Data[i].Show>M7_LSTATE_SHOW)
			{
				printf("[M7_InitPosition] g_nM7_Lock_Data[%d].Show error=>%d.\n",i,g_nM7_Lock_Data[i].Show);
				g_nM7_Lock_Data[i].Show = M7_LSTATE_HIDE; //default.
				printf("[M7_InitPosition] g_nM7_Lock_Data[%d].Show error=>default setting=>%d\n",i,g_nM7_Lock_Data[i].Show);
			}
		}

		//key start
		//left
		M7_IconMovePosition(12, 2, true); //M7_12_ANI->2
		M7_IconMovePosition(13, 3, true); //M7_13_ANI->3
		M7_IconMovePosition(5, 4, true); //M1_5_BTN->4
		M7_IconMovePosition(6, 5, true); //M1_6_ANI->5

		M7_IconMovePosition(9, 9, false); //M7_9_ANI->x //user

		//right
		M7_IconMovePosition(7, 6, true); //M1_7_ANI->6
		M7_IconMovePosition(4, 7, true); //M7_4_ANI->7
		M7_IconMovePosition(11, 8, true); //M7_11_ANI->8

		int nX  = ituWidgetGetX(M7_12_ZoomBTN);
        int nY  = ituWidgetGetY(M7_12_ZoomBTN);
		printf("[M7_InitPosition][2] [Zoom]nX=%d nY=%d ituWidgetIsVisible=%d\n",nX,nY,ituWidgetIsVisible(M7_12_ZoomBTN));

		if(MenuPreset_Choice_Index == mPreset_lock)
		{
			
			MenuPreset_Choice_Index = mPreset_lock;//still
			printf("[M7_InitPosition][Menu lock Set] MenuPreset_Choice_Index=%d\r\n",MenuPreset_Choice_Index);
			
			//1,2,3 aux init
			M7_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
			ituWidgetSetVisible(M7_1_BTN, true);
			//ituAnimationGoto(M7_1_ANI, 0);
			ituWidgetSetVisible(M7_1_ANI, false);
			ituWidgetSetVisible(M7_1_ZoomBTN, false);

			ituWidgetSetVisible(M7_2a_BTN, true);
			//ituAnimationGoto(M7_2a_ANI, 0);
			ituWidgetSetVisible(M7_2a_ANI, false);
			ituWidgetSetVisible(M7_2a_ZoomBTN, false);

			ituWidgetSetVisible(M7_2b_BTN, true);
			//ituAnimationGoto(M7_2b_ANI, 0);
			ituWidgetSetVisible(M7_2b_ANI, false);
			ituWidgetSetVisible(M7_2b_ZoomBTN, false);

			ituWidgetSetVisible(M7_11_BTN, true);
			//ituAnimationGoto(M7_11_ANI, 0);
			ituWidgetSetVisible(M7_11_ANI, false);
			ituWidgetSetVisible(M7_11_ZoomBTN, false);
		}
		else
		{
			printf("[M7_InitPosition][NOT][Menu lock Set] MenuPreset_Choice_Index=%d\r\n",MenuPreset_Choice_Index);
		}
		
		if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		{
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_LOCK],v_lock_icon_pos[Lock_Icon3_right][0] ,v_lock_icon_pos[Lock_Icon3_right][1]);
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_UNLOCK],v_lock_icon_pos[Lock_Icon3_right][0] ,v_lock_icon_pos[Lock_Icon3_right][1]);
		}
		else
		{
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_LOCK],h_lock_icon_pos[Lock_Icon3_right][0] ,h_lock_icon_pos[Lock_Icon3_right][1]);
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_UNLOCK],h_lock_icon_pos[Lock_Icon3_right][0] ,h_lock_icon_pos[Lock_Icon3_right][1]);
		}

		printf("[M7_InitPosition][Keymode] x=%d y=%d\n",h_lock_icon_pos[Lock_Icon3_right][0],h_lock_icon_pos[Lock_Icon3_right][1]);
	}
	else if (nType == M7_MODE_LOCK)
	{
		M7_KeyMode_Choice_Index = MenuPreset_Choice_Index; //save index in M7 mode

		if(M7_LockMode_Choice_Index != mPreset_1st && M7_LockMode_Choice_Index != mPreset_2nd && M7_LockMode_Choice_Index != mPreset_AutoPressure)
		{
			M7_LockMode_Choice_Index = mPreset_1st;
		}
		MenuPreset_Choice_Index = M7_LockMode_Choice_Index; //load index in lock mode

		//setup icon's lock

		//left
		M7_IconMovePosition(11, 2, true); //M7_11_ANI->3

		M7_IconMovePosition(4, 3, false);
		M7_IconMovePosition(5, 3, false);
		M7_IconMovePosition(6, 3, false);
		M7_IconMovePosition(7, 3, false);
		M7_IconMovePosition(9, 3, false);

		M7_IconMovePosition(12, 3, false);
		M7_IconMovePosition(13, 3, false);

		
		if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		{
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_LOCK],v_lock_icon_pos[Lock_Icon3_Left][0] ,v_lock_icon_pos[Lock_Icon3_Left][1]);
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_UNLOCK],v_lock_icon_pos[Lock_Icon3_Left][0] ,v_lock_icon_pos[Lock_Icon3_Left][1]);
		}
		else
		{
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_LOCK],h_lock_icon_pos[Lock_Icon3_Left][0] ,h_lock_icon_pos[Lock_Icon3_Left][1]);
			ituWidgetSetPosition(M7_ICON_LOCK[ICON_3rd][M7_LSTATE_UNLOCK],h_lock_icon_pos[Lock_Icon3_Left][0] ,h_lock_icon_pos[Lock_Icon3_Left][1]);
		}

		printf("[M7_InitPosition][Lock mode] x=%d y=%d\n",h_lock_icon_pos[Lock_Icon3_Left][0],h_lock_icon_pos[Lock_Icon3_Left][1]);
	}
	else
	{
		printf("[M7_InitPosition]Error g_nM7_StartType:%d\n", g_nM7_StartType);
	}

	//=============================
		//chpark.24.11.07
		if(MenuPreset_Choice_Index<=0)
		{
			MenuPreset_Choice_Index = mPreset_1st;
		}

		
		switch(MenuPreset_Choice_Index)			// default 1st AUX
		{
			case mPreset_1st:
			{
				printf("User's AUX 6 Preset\r\n");
				/*
				ituWidgetSetVisible(M7_1_BTN, false);
				ituWidgetSetVisible(M7_1_ANI, true);
	
				M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_1_ANI,0);
				*/
				M7_1_Scaleflag = 0;
				ituWidgetSetVisible(M7_1_BTN, false);
				ituWidgetSetVisible(M7_1_ANI, false);
				ituWidgetSetVisible(M7_1_ZoomBTN, true);
			}
			break;
	
			case mPreset_2nd:
			{
				printf("User's AUX 2,4 Preset\r\n");
				/*
				ituWidgetSetVisible(M7_2a_BTN, false);
				ituWidgetSetVisible(M7_2b_BTN, false);
				ituWidgetSetVisible(M7_2a_ANI, true);
				ituWidgetSetVisible(M7_2b_ANI, true);
	
				M7_2a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_2b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_2a_ANI,0);
				ituAnimationPlay(M7_2b_ANI,0);
				*/
				M7_2a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_2b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_2a_BTN, false);
				ituWidgetSetVisible(M7_2b_BTN, false);
				ituWidgetSetVisible(M7_2a_ANI, false);
				ituWidgetSetVisible(M7_2b_ANI, false);
				ituWidgetSetVisible(M7_2a_ZoomBTN, true);
				ituWidgetSetVisible(M7_2b_ZoomBTN, true);
			}
	
			break;
	
			case mPreset_Lang:
			{
				printf("SysTem Language Set\r\n");
				/*
				ituWidgetSetVisible(M7_5_BTN, false);
				ituWidgetSetVisible(M7_5_ANI, true);
	
				M7_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_5_ANI,0);
				*/
				M7_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_5_BTN, false);
				ituWidgetSetVisible(M7_5_ANI, false);
				ituWidgetSetVisible(M7_5_ZoomBTN, true);
			}
			break;
	
			case mPreset_Opt:
			{
				printf("SysTem Option Set\r\n");
				/*
				ituWidgetSetVisible(M7_6_BTN, false);
				ituWidgetSetVisible(M7_6_ANI, true);
	
				M7_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_6_ANI,0);
				*/
				M7_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_6_BTN, false);
				ituWidgetSetVisible(M7_6_ANI, false);
				ituWidgetSetVisible(M7_6_ZoomBTN, true);
			}
			break;
	
			case mPreset_Radio:
			{
				printf("SysTem Radio Region Set\r\n");
				/*
				ituWidgetSetVisible(M7_7_BTN, false);
				ituWidgetSetVisible(M7_7_ANI, true);
	
				M7_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_7_ANI,0);
				*/
				M7_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_7_BTN, false);
				ituWidgetSetVisible(M7_7_ANI, false);
				ituWidgetSetVisible(M7_7_ZoomBTN, true);
			}
			break;
	
			case mPreset_User:
			{
				printf("SysTem User Infomaion Set\r\n");
				if(CheckM6_9()==1)
				{
					/*
					ituWidgetSetVisible(M7_9_BTN, false);
					ituWidgetSetVisible(M7_9_ANI, true);
	
					M7_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M7_9_ANI,0);
					*/
					M7_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M7_9_BTN, false);
					ituWidgetSetVisible(M7_9_ANI, false);
					ituWidgetSetVisible(M7_9_ZoomBTN, true);
				}
				else
				{
					M7_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M7_9_BTN, false);
					ituWidgetSetVisible(M7_9_ANI, false);
					ituWidgetSetVisible(M7_9_ZoomBTN, false);
				}
			}
			break;
	
			case mPreset_Greese:
			{
				printf("SysTem Auto Greese Set\r\n");
				
				if((M7_AG_OptFlag == 1) || (M7_AG_OptFlag == 2))
				{
					/*
					ituWidgetSetVisible(M7_4_BTN, false);
					ituWidgetSetVisible(M7_4_ANI, true);
	
					M7_4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M7_4_ANI,0);
					*/
					M7_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M7_4_BTN, false);
					ituWidgetSetVisible(M7_4_ANI, false);
					ituWidgetSetVisible(M7_4_ZoomBTN, true);
				}
				else if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
				{
					printf("Change User's AUX 5 Preset\r\n");
					MenuPreset_Choice_Index = mPreset_1st;
					/*
					ituWidgetSetVisible(M7_1_BTN, false);
					ituWidgetSetVisible(M7_1_ANI, true);
	
					M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M7_1_ANI,0);
					*/
					M7_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M7_1_BTN, false);
					ituWidgetSetVisible(M7_1_ANI, false);
					ituWidgetSetVisible(M7_1_ZoomBTN, true);
				}
			}
			break;
			
			case mPreset_AutoPressure:
			{
				if(M7_AP_OptFlag == 1)		// on
				{
					ituWidgetSetVisible(M7_11_BTN, false);
					ituWidgetSetVisible(M7_11_ANI, true);
	
					M7_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M7_11_ANI,0);
				}
				else if((M7_AP_OptFlag == 0)||(M7_AP_OptFlag == 2)||(M7_AP_OptFlag == 3))		// off , off(hold) , No display
				{
					printf("Change User's AUX 4 Preset\r\n");
					MenuPreset_Choice_Index = mPreset_1st;
					/*
					ituWidgetSetVisible(M7_1_BTN, false);
					ituWidgetSetVisible(M7_1_ANI, true);
	
					M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M7_1_ANI,0);
					*/
					M7_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M7_1_BTN, false);
					ituWidgetSetVisible(M7_1_ANI, false);
					ituWidgetSetVisible(M7_1_ZoomBTN, true);
				}
			}
			break;
			
			//chpark.24.09.12
			case mPreset_lock:
			{
				printf("[Init]Menu lock Set\r\n");
				/*
				ituWidgetSetVisible(M7_12_BTN, false);
				ituWidgetSetVisible(M7_12_ANI, true);
				ituWidgetSetVisible(M7_12_ZoomBTN, false);
	
				M7_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_12_ANI,0);
				*/
				M7_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_12_BTN, false);
				ituWidgetSetVisible(M7_12_ANI, false);
				ituWidgetSetVisible(M7_12_ZoomBTN, true);
			}
			break;

			case mPreset_password:
			{
				printf("Password Set\r\n");
				/*
				ituWidgetSetVisible(M7_13_BTN, false);
				ituWidgetSetVisible(M7_13_ANI, true);
	
				M7_13_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_13_ANI,0);
				*/
				M7_13_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M7_13_BTN, false);
				ituWidgetSetVisible(M7_13_ANI, false);
				ituWidgetSetVisible(M7_13_ZoomBTN, true);
			}
			break;
		}		

		//=============================
		M7_Show_LockData();
		//=============================

		bLayer_Changing = false;//chpark.24.10.15
}

void M7_IconMovePosition(int nNameNum, int nPos, bool bShow)
{	
	char tmp_WidgetName[100];

	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
	{
		if(nNameNum==4)
		{
			ituWidgetSetPosition(M7_4_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_BTN,bShow);

			ituWidgetSetPosition(M7_4_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_ANI,false);
			
			ituWidgetSetPosition(M7_4_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_ZoomBTN,false);

			ituWidgetSetPosition(M7_4_G,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_G,false);

			printf("M7 Autogreese [hide]\n");
		}
		else if(nNameNum==5)
		{
			ituWidgetSetPosition(M7_5_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_BTN,bShow);

			ituWidgetSetPosition(M7_5_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_ANI,false);
			
			ituWidgetSetPosition(M7_5_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_ZoomBTN,false);
		}
		else if(nNameNum==6)
		{
			ituWidgetSetPosition(M7_6_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_BTN,bShow);

			ituWidgetSetPosition(M7_6_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_ANI,false);
			
			ituWidgetSetPosition(M7_6_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_ZoomBTN,false);
		}
		else if(nNameNum==7)
		{
			ituWidgetSetPosition(M7_7_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_BTN,bShow);

			ituWidgetSetPosition(M7_7_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_ANI,false);
			
			ituWidgetSetPosition(M7_7_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_ZoomBTN,false);
		}
		else if(nNameNum==9)
		{
			ituWidgetSetPosition(M7_9_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_BTN,bShow);

			ituWidgetSetPosition(M7_9_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_ANI,false);
			
			ituWidgetSetPosition(M7_9_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_ZoomBTN,false);
		}
		else if(nNameNum==11)
		{
			ituWidgetSetPosition(M7_11_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_BTN,bShow);

			ituWidgetSetPosition(M7_11_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_ANI,false);
			
			ituWidgetSetPosition(M7_11_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_ZoomBTN,false);

			ituWidgetSetPosition(M7_11_G,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_G,false);
		}
		else if(nNameNum==12)
		{
			ituWidgetSetPosition(M7_12_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_BTN,bShow);

			ituWidgetSetPosition(M7_12_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_ANI,false);
			
			ituWidgetSetPosition(M7_12_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_ZoomBTN,false);

			ituWidgetSetPosition(M7_12_G,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_G,false);
			if(g_nM7_StartType == M7_MODE_KEY && g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
			{
				printf("[M7_BTN_MouseUp] g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF return.\n");
				ituWidgetSetVisible(M7_12_G,true);
			}
			else
			{
				ituWidgetSetVisible(M7_12_G,false);
			}
		}
		else if(nNameNum==13)
		{
			ituWidgetSetPosition(M7_13_BTN,v_small_icon_pos[nPos][0] ,v_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_BTN,bShow);

			ituWidgetSetPosition(M7_13_ANI,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_ANI,false);
			
			ituWidgetSetPosition(M7_13_ZoomBTN,v_big_icon_pos[nPos][0] ,v_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_ZoomBTN,false);
		}

		//printf("[M7_IconMovePosition] nNameNum=%d=>nPos=%d  x=%d ,y=%d\n",nNameNum,nPos, v_small_icon_pos[nPos][0],v_small_icon_pos[nPos][1] );
	}
	else
	{
		if(nNameNum==4)
		{
			ituWidgetSetPosition(M7_4_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_BTN,bShow);

			ituWidgetSetPosition(M7_4_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_ANI,false);
			
			ituWidgetSetPosition(M7_4_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_ZoomBTN,false);

			ituWidgetSetPosition(M7_4_G,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_4_G,false);

			printf("M7 Autogreese [hide]\n");
			
		}
		else if(nNameNum==5)
		{
			ituWidgetSetPosition(M7_5_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_BTN,bShow);

			ituWidgetSetPosition(M7_5_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_ANI,false);
			
			ituWidgetSetPosition(M7_5_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_5_ZoomBTN,false);
		}
		else if(nNameNum==6)
		{
			ituWidgetSetPosition(M7_6_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_BTN,bShow);

			ituWidgetSetPosition(M7_6_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_ANI,false);
			
			ituWidgetSetPosition(M7_6_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_6_ZoomBTN,false);
		}
		else if(nNameNum==7)
		{
			ituWidgetSetPosition(M7_7_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_BTN,bShow);

			ituWidgetSetPosition(M7_7_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_ANI,false);
			
			ituWidgetSetPosition(M7_7_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_7_ZoomBTN,false);
		}
		else if(nNameNum==9)
		{
			ituWidgetSetPosition(M7_9_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_BTN,bShow);

			ituWidgetSetPosition(M7_9_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_ANI,false);
			
			ituWidgetSetPosition(M7_9_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_9_ZoomBTN,false);
		}
		else if(nNameNum==11)
		{
			ituWidgetSetPosition(M7_11_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_BTN,bShow);

			ituWidgetSetPosition(M7_11_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_ANI,false);
			
			ituWidgetSetPosition(M7_11_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_ZoomBTN,false);

			ituWidgetSetPosition(M7_11_G,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_11_G,false);
			
		}
		else if(nNameNum==12)
		{
			ituWidgetSetPosition(M7_12_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_BTN,bShow);

			ituWidgetSetPosition(M7_12_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_ANI,false);
			
			ituWidgetSetPosition(M7_12_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_ZoomBTN,false);

			ituWidgetSetPosition(M7_12_G,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_12_G,false);

			//chpark.24.11.15
			if(g_nM7_StartType == M7_MODE_KEY && g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
			{
				printf("[M7_BTN_MouseUp] g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF return.\n");
				ituWidgetSetVisible(M7_12_G,true);
			}
			else
			{
				ituWidgetSetVisible(M7_12_G,false);
			}
		}
		else if(nNameNum==13)
		{
			ituWidgetSetPosition(M7_13_BTN,h_small_icon_pos[nPos][0] ,h_small_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_BTN,bShow);

			ituWidgetSetPosition(M7_13_ANI,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_ANI,false);
			
			ituWidgetSetPosition(M7_13_ZoomBTN,h_big_icon_pos[nPos][0] ,h_big_icon_pos[nPos][1]);
			ituWidgetSetVisible(M7_13_ZoomBTN,false);
		}
	}
}

void M7_Show_LockData()
{
	if(g_nM7_StartType==M7_MODE_PUSH || g_bPW100_PW_OnOff==PW100_PASS_MODE_OFF)
	{
		ituWidgetSetVisible(M7_CONT_ICON_LOCK, false);
		printf("[M7_Show_LockData]=%d\n",g_bPW100_PW_OnOff);
		return; 
	}
	else
	{
		ituWidgetSetVisible(M7_CONT_ICON_LOCK, true);
	}

	for(int i=0;i<3;i++)
	{
		ituWidgetSetVisible(M7_ICON_LOCK[i][M7_LSTATE_LOCK],  false);
		ituWidgetSetVisible(M7_ICON_LOCK[i][M7_LSTATE_UNLOCK], false);
	}

	if(g_nM7_StartType == M7_MODE_KEY)
	{
		for(int i=0;i<3;i++)
		{
			if(g_nM7_Lock_Data[i].Show==M7_LSTATE_SHOW)
			{
				if(g_nM7_Lock_Data[i].Lock==M7_LSTATE_UNLOCK)
				{
					printf("M7_Show_LockData][%d] M7_LSTATE_UNLOCK\n",i);
					ituWidgetSetVisible(M7_ICON_LOCK[i][M7_LSTATE_UNLOCK], true);
				}
				else if(g_nM7_Lock_Data[i].Lock==M7_LSTATE_LOCK)
				{
					printf("M7_Show_LockData][%d] M7_LSTATE_LOCK\n",i);
					ituWidgetSetVisible(M7_ICON_LOCK[i][M7_LSTATE_LOCK], true);
				}
			}
			else
			{
				//don't show
				printf("g_nM7_Lock_Data[%d]].Show: M7_LSTATE_HIDE  don't show\n",i);
			}
			
		}
	}
	else if(g_nM7_StartType == M7_MODE_LOCK)
	{
		for(int i=0;i<3;i++)
		{
			if(g_nM7_Lock_Data[i].Show == M7_LSTATE_SHOW)
			{
				printf("M7_Show_LockData][M7_MODE_LOCK][%d] M7_LSTATE_LOCK\n",i);
				ituWidgetSetVisible(M7_ICON_LOCK[i][M7_LSTATE_LOCK], true);
			}
			else
			{
				//don't show
				printf("g_nM7_Lock_Data[M7_MODE_LOCK][%d].Show: M7_LSTATE_HIDE  don't show\n",i);
			}
		}

		if(M7_AP_OptFlag !=1 )
		{
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);
		}
	}
	else
	{
		//error
	}
}


void M7_Show_LockData_AP()
{
	if(g_nM7_StartType==M7_MODE_PUSH || g_bPW100_PW_OnOff==PW100_PASS_MODE_OFF)
	{
		ituWidgetSetVisible(M7_CONT_ICON_LOCK, false);
		//printf("[M7_Show_LockData]=%d\n",g_bPW100_PW_OnOff);
		return; 
	}
	else
	{
		ituWidgetSetVisible(M7_CONT_ICON_LOCK, true);
	}

	if(g_nM7_StartType == M7_MODE_KEY)
	{
		if(g_nM7_Lock_Data[2].Show==M7_LSTATE_SHOW)
		{
			if(g_nM7_Lock_Data[2].Lock==M7_LSTATE_UNLOCK)
			{
				//printf("M7_Show_LockData][%d] M7_LSTATE_UNLOCK\n",2);
				ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);
				ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], true);
			}
			else if(g_nM7_Lock_Data[2].Lock==M7_LSTATE_LOCK)
			{
				//printf("M7_Show_LockData][%d] M7_LSTATE_LOCK\n",2);
				ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], true);
				ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);
			}
		}
		else
		{
			//don't show
			//printf("g_nM7_Lock_Data[%d]].Show: M7_LSTATE_HIDE  don't show\n",i);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);
		}
	}
	else if(g_nM7_StartType == M7_MODE_LOCK)
	{
		if(g_nM7_Lock_Data[2].Show == M7_LSTATE_SHOW)
		{
			//printf("M7_Show_LockData][M7_MODE_LOCK][%d] M7_LSTATE_LOCK\n",i);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], true);
		}
		else
		{
			//don't show
			//printf("g_nM7_Lock_Data[M7_MODE_LOCK][%d].Show: M7_LSTATE_HIDE  don't show\n",i);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);
		}
		

		if(M7_AP_OptFlag !=1 )
		{
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);
			ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);
		}
	}
	else
	{
		//error
	}
}

int M7_PW_GetCntNullChar()
{
	char Tmp_CurPassWord[MAX_PW100_PWD+2];

	memset(Tmp_CurPassWord, '\0', sizeof(Tmp_CurPassWord));
    memcpy(Tmp_CurPassWord,PW100_PWD,MAX_PW100_PWD);
    int len_cur;
    len_cur = strlen(Tmp_CurPassWord);

    //=====================================================================
    int nErrNum=0;
    for(int i=0;i<MAX_PW100_PWD;i++)
    {
        if(Tmp_CurPassWord[i]>=0x30 && Tmp_CurPassWord[i]<=0x39)
        {
               //0~9 num 
        }
        else
        {
            nErrNum++;
            //printf("Error PW[%d]:%s\n",i,PW100_Txt_CurPassWord[i]);
        }
    }
    
    if(len_cur<4 || nErrNum>0)
    {
		printf("[M7_PW_GetCntNullChar] Cur Password:%s  Reset Old Pass.: nErrNum:%d\n",Tmp_CurPassWord,nErrNum);
        
		memset(Tmp_CurPassWord, '\0', sizeof(Tmp_CurPassWord));
        memset(PW100_PWD, '\0', sizeof(PW100_PWD));
        len_cur = strlen(Tmp_CurPassWord);
    
		return len_cur;        
    }
	else
	{
		printf("\n[M7_PW_GetCntNullChar] Cur Password:%s  \n\n",Tmp_CurPassWord,len_cur);
	}
    //=====================================================================

	return 0;
}


void M7_Toggle_LockData(int nIndex)
{
	printf("[M7_Toggle_LockData] nIndex=%d  g_nM7_Lock_Data[%d].Show=%d\n",nIndex,nIndex,g_nM7_Lock_Data[nIndex].Show);
	//only in Lock mode, it works.
	g_nM7_Lock_Data[nIndex].Show = !g_nM7_Lock_Data[nIndex].Show;

	if(g_nM7_Lock_Data[nIndex].Show == M7_LSTATE_HIDE)
	{
		ituWidgetSetVisible(M7_ICON_LOCK[nIndex][M7_LSTATE_LOCK],   false);
		ituWidgetSetVisible(M7_ICON_LOCK[nIndex][M7_LSTATE_UNLOCK], false);

		printf("[M7_Toggle_LockData] =>[Draw HIDE] nIndex=%d  g_nM7_Lock_Data[%d].Show=%d\n",nIndex,nIndex,g_nM7_Lock_Data[nIndex].Show);
	} 
	else if(g_nM7_Lock_Data[nIndex].Show == M7_LSTATE_SHOW)
	{
		//If you touch the lock button even once, the lock button is locked again. 
		//below code.
		//g_nM7_Lock_Data[nIndex].Lock = M7_LSTATE_LOCK;//chpark.24.11.07
		ituWidgetSetVisible(M7_ICON_LOCK[nIndex][M7_LSTATE_LOCK],   true);
		ituWidgetSetVisible(M7_ICON_LOCK[nIndex][M7_LSTATE_UNLOCK], false);

		printf("[M7_Toggle_LockData] =>[Draw SHOW] nIndex=%d  g_nM7_Lock_Data[%d].Show=%d\n",nIndex,nIndex,g_nM7_Lock_Data[nIndex].Show);
	}
}

//====================================================================

bool M7_Timer(ITUWidget* widget, char* param)
{
	static uint32_t OnTimerCnt_33ms = 0;	// JPHong 2024-03-06
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	
	OnTimerCnt_33ms++;
#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		OnTimerCnt_100ms++;

		//chpark.24.10.15
		if(g_nM7_StartType==M7_MODE_PUSH)
		{
			if(CheckM6_9()==1)
			{
				M7_SU_Display(1);		// display on
			}
			else
			{
				M7_SU_Display(0);		// display off
			}
		}
		else
		{
			M7_SU_Display(0);		// display off
		}

		#ifdef TEST_USER_SET_ICON		// 	JPHong2024-02-07	Menu7_Layer	M7_9_ZoomBTN
			M7_SU_Display(1);		// display on
		#endif
	}	
	if((gMainTimerCnt_10m - M7_SecTimer) >= 100)
	{
		if(M7_ExitCNT<30)
			M7_ExitCNT++;

		if(M7_ExitCNT == 30)
		{
			M7_ExitCNT = 100;
			printf("\r\nM7 Menu 30Sec Exit > goto Main\r\n");
			
			#ifdef RELEASE_VER
				//chpark.24.09.12
				if(g_nM7_StartType == M7_MODE_LOCK)
				{
					g_nM7_StartType = M7_MODE_KEY;
				}
				
				GotoMain(MAIN_MODE_NORMAL); 
			#endif
		}

		M7_SecTimer = gMainTimerCnt_10m;
	}	

	//chpark.24.10.17
	if(g_nM7_StartType==M7_MODE_LOCK || bLayer_Changing )
	{
		// display off
		ituWidgetSetVisible(M7_4_ZoomBTN, false);
		ituWidgetSetVisible(M7_4_ANI, false);
		ituWidgetSetVisible(M7_4_G, false);

		ituWidgetSetVisible(M7_4_BTN, false);
	}
	else
	{
		//printf("[g_nM7_StartType]=%d\n",g_nM7_StartType);
		// CAN[OPI] polling 
		if((M7_AG_OptFlag == 1)||(M7_AG_OptFlag == 2))
		{
			if(MenuPreset_Choice_Index == mPreset_Greese)
			{
				ituWidgetSetVisible(M7_4_BTN, false);
				ituWidgetSetVisible(M7_4_ANI, false);
				ituWidgetSetVisible(M7_4_G, false);

				ituWidgetSetVisible(M7_4_ZoomBTN, true);
			}
			else
			{
				M7_AG_Display(1);			// display on
			}
		}
		else if(M7_AG_OptFlag == 0)
		{
			M7_AG_Display(0);			// gray out
		}
		else if(M7_AG_OptFlag == 3)
		{
			M7_AG_Display(3);			// display off
		}
	}

	// CAN[FNI1] polling
	
	if(M7_AP_OptFlag == 1)
	{
		if(MenuPreset_Choice_Index == mPreset_AutoPressure)
		{
			ituWidgetSetVisible(M7_11_BTN, false);
			ituWidgetSetVisible(M7_11_G, false);
			ituWidgetSetVisible(M7_11_ANI, false);
			
			ituWidgetSetVisible(M7_11_ZoomBTN, true);
		}
		else
		{
			M7_AP_Display(M7_AP_OptFlag);		// display on
		}	

		M7_Show_LockData_AP();//chpark.24.10.17			
	}
	else
	{
		if(g_nM7_StartType == M7_MODE_LOCK)
		{
			ituWidgetSetVisible(M7_11_BTN, false);
			ituWidgetSetVisible(M7_11_G, false);
			ituWidgetSetVisible(M7_11_ANI, false);
			
			ituWidgetSetVisible(M7_11_ZoomBTN, false);
		}
		else
		{
			M7_AP_Display(M7_AP_OptFlag);
		}
		
		ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_LOCK], false);//chpark.24.10.17	
		ituWidgetSetVisible(M7_ICON_LOCK[2][M7_LSTATE_UNLOCK], false);//chpark.24.10.17	
	}

	M7_AniMenuStatus();

    return true;
}

bool M7_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tMenuIndex = 0;
	
	M7_ExitCNT = 0;

	tJogWheel = (uint8_t)param[0];
	
	printf("\n[M7 JogWheel] %d\r\n",tJogWheel);

	if(g_nM7_StartType == M7_MODE_KEY)
	{ 
		M7_JogWheel_KeyMode(param);
		return true;
	}
	else if(g_nM7_StartType == M7_MODE_LOCK)
	{
		M7_JogWheel_LockMode(param);
		return true;
	}
	else
	{
		//pushmode continue.
	}

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		tMenuIndex = MenuPreset_Choice_Index;
			
		tMenuIndex--;
		if(tMenuIndex == 0)
			tMenuIndex = 8;

		if(tMenuIndex == 6)
		{
			if(CheckM6_9()==1)
				tMenuIndex = 6;
			else
				tMenuIndex = 5;
		}
		else if(tMenuIndex == 7)
		{
			if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
			{
				if(CheckM6_9()==1)
					tMenuIndex = 6;
				else
					tMenuIndex = 5;
			}
		}
		else if(tMenuIndex == 8)
		{
			if(M7_AP_OptFlag != 1)
			{
				if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
				{
					if(CheckM6_9()==1)
						tMenuIndex = 6;
					else
						tMenuIndex = 5;
				}
				else
					tMenuIndex = 7;
			}
			else
				tMenuIndex = 8;
		}

//		M7_SubMenuClear();

		memset(M7_PARA,2,0);
		sprintf(M7_PARA,"%1d",tMenuIndex);
		printf("JogWheel CCW Branch Submenu : %s\r\n",M7_PARA);

		M7_BTN_MouseDn(NULL, M7_PARA);
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		tMenuIndex = MenuPreset_Choice_Index;

		tMenuIndex++;
		if(tMenuIndex > 8)
			tMenuIndex = 1;

		if(tMenuIndex == 6)
		{
			if(CheckM6_9()==1)
				tMenuIndex = 6;
			else
			{
				if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
				{
					if(M7_AP_OptFlag != 1)
						tMenuIndex = 1;
					else
						tMenuIndex = 8;
				}
				else
					tMenuIndex = 7;
			}
		}
		else if(tMenuIndex == 7)
		{
			if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
			{
				if(M7_AP_OptFlag != 1)
					tMenuIndex = 1;
				else
					tMenuIndex = 8;
			}
			else
				tMenuIndex = 7;
		}
		else if(tMenuIndex == 8)
		{
			//if((M7_AP_OptFlag == 3)||(M7_AP_OptFlag == 2)||(M7_AP_OptFlag == 0))
			if(M7_AP_OptFlag != 1)
				tMenuIndex = 1;
			else
				tMenuIndex = 8;
		}

//		M7_SubMenuClear();

		memset(M7_PARA,2,0);
		sprintf(M7_PARA,"%1d",tMenuIndex);
		printf("JogWheel CW Branch Submenu : %s\r\n",M7_PARA);
			
		M7_BTN_MouseDn(NULL, M7_PARA);
	}
	
    return true;
}

bool M7_JogWheel_KeyMode(char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tMenuIndex = 0; //current menu index
	
	M7_ExitCNT = 0;

	tJogWheel = (uint8_t)param[0];
	
	printf("[M7_JogWheel_KeyMode] %d\r\n",tJogWheel);

	bool bCheck = false;
	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		bCheck = true;
		tMenuIndex = MenuPreset_Choice_Index;
		
		switch(tMenuIndex)
		{
			case mPreset_1st:
				tMenuIndex = mPreset_AutoPressure;
				if(M7_AP_OptFlag != 1)
				{
					tMenuIndex = mPreset_Greese;
					if(M7_AG_OptFlag != 1)
					{
						tMenuIndex = mPreset_Radio;
					}
				}
				break;

			case mPreset_2nd:
				tMenuIndex = mPreset_1st;
				break;	

			case mPreset_lock:
				tMenuIndex = mPreset_2nd;
				break;

			case mPreset_password:
				tMenuIndex = mPreset_lock;
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
				{
					tMenuIndex = mPreset_2nd;
				}
				break;

			case mPreset_Lang:
				tMenuIndex = mPreset_password;
				break;

			case mPreset_Opt:
				tMenuIndex = mPreset_Lang;
				break;

			case mPreset_Radio:
				tMenuIndex = mPreset_Opt;
				break;

			case mPreset_Greese:
				tMenuIndex = mPreset_Radio;
				break;

			case mPreset_AutoPressure:
				if(M7_AP_OptFlag != 1)
				{
					printf("[M7_JogWheel_KeyMode][Left][AP] M7_AP_OptFlag:%d\n",M7_AP_OptFlag);
					return false;
				}

				tMenuIndex = mPreset_Greese;
				if(M7_AG_OptFlag != 1)
				{
					tMenuIndex = mPreset_Radio;
				}
				break;

			default:
				printf("[M7_JogWheel_KeyMode][Left]error index:%d\n",tMenuIndex);
				break;
		}
		
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		bCheck = true;
		tMenuIndex = MenuPreset_Choice_Index;

		switch(tMenuIndex)
		{
			case mPreset_1st:
				tMenuIndex = mPreset_2nd;
				break;

			case mPreset_2nd:
				tMenuIndex = mPreset_lock;
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
				{
					tMenuIndex = mPreset_password;
				}
				break;	

			case mPreset_lock:
				tMenuIndex = mPreset_password;
				break;

			case mPreset_password:
				tMenuIndex = mPreset_Lang;
				break;

			case mPreset_Lang:
				tMenuIndex = mPreset_Opt;
				break;

			case mPreset_Opt:
				tMenuIndex = mPreset_Radio;
				break;

			case mPreset_Radio:
				tMenuIndex = mPreset_Greese;
				if(M7_AG_OptFlag != 1)
				{
					tMenuIndex = mPreset_AutoPressure;

					if(M7_AP_OptFlag != 1)
					{
						tMenuIndex = mPreset_1st;
					}
				}
				break;

			case mPreset_Greese:
				tMenuIndex = mPreset_AutoPressure;

				if(M7_AP_OptFlag != 1)
				{
					tMenuIndex = mPreset_1st;
				}

				break;

			case mPreset_AutoPressure:
				tMenuIndex = mPreset_1st;
				break;

			default:
				printf("[M7_JogWheel_KeyMode][Left]error index:%d\n",tMenuIndex);
				break;
		}
	}

	
	if(bCheck)
	{
		memset(M7_PARA,2,0);
		sprintf(M7_PARA,"%1d",tMenuIndex);
		printf("[M7_JogWheel_KeyMode]Branch Submenu : %s\r\n",M7_PARA);
			
		M7_BTN_MouseDn(NULL, M7_PARA);
	}

	return true;
}

bool M7_JogWheel_LockMode(char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tMenuIndex = 0;
	
	M7_ExitCNT = 0;

	tJogWheel = (uint8_t)param[0];
	
	printf("[M7_JogWheel_LockMode] %d\r\n",tJogWheel);
	bool bCheck = false;

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		bCheck = true;
		tMenuIndex = MenuPreset_Choice_Index;

		switch(tMenuIndex)
		{
			case mPreset_1st:
				tMenuIndex = mPreset_AutoPressure;
				if(M7_AP_OptFlag != 1)
				{
					tMenuIndex = mPreset_2nd;
				}
				break;

			case mPreset_2nd:
				tMenuIndex = mPreset_1st;
				break;

			case mPreset_AutoPressure:
				tMenuIndex = mPreset_2nd;
				//error check.
				if(M7_AP_OptFlag != 1)
				{
					printf("[M7_JogWheel_LockMode][Error][Left] M7_AP_OptFlag==%d \n",M7_AP_OptFlag);
					tMenuIndex = mPreset_2nd;
				}
				break;	

			default:

				break;		
		}
	
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		bCheck = true;
		tMenuIndex = MenuPreset_Choice_Index;
		switch(tMenuIndex)
		{
			case mPreset_1st:
				tMenuIndex = mPreset_2nd;
				break;

			case mPreset_2nd:
				tMenuIndex = mPreset_AutoPressure;
				if(M7_AP_OptFlag != 1)
				{
					tMenuIndex = mPreset_1st;
				}
				break;

			case mPreset_AutoPressure:
				tMenuIndex = mPreset_1st;
				//error check.
				if(M7_AP_OptFlag != 1)
				{
					printf("[M7_JogWheel_LockMode][Error][Right] M7_AP_OptFlag==%d \n",M7_AP_OptFlag);
					tMenuIndex = mPreset_1st;
				}
				break;	

			default:

				break;		
		}
		
	}

	if(bCheck)
	{
		memset(M7_PARA,2,0);
		sprintf(M7_PARA,"%1d",tMenuIndex);
		printf("[M7_JogWheel_LockMode]Branch Submenu : %s\r\n",M7_PARA);
			
		M7_BTN_MouseDn(NULL, M7_PARA);
	}


	return true;
}

//============================================================
bool M7_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;
	uint8_t tMenuIndex = 0;

	M7_ExitCNT = 0;
	
	tJogButton = (uint8_t)param[0];
	
	printf("[M7_JogButton] %d  g_nM7_StartType=%d\r\n",tJogButton,g_nM7_StartType);

	if(g_nM7_StartType == M7_MODE_KEY)
	{ 
		M7_JogButton_KeyMode(param);
		return true;
	}
	else if(g_nM7_StartType == M7_MODE_LOCK)
	{
		M7_JogButton_LockMode(param);
		return true;
	}
	else
	{
		//pushmode continue.
	}

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// 1>6or5,2>1,3>2,4>3,5>4,6>5,7>8or7,8>7or8
				uint8_t tDestMenu1[9] = {0,6,1,2,3,4,5,8,7};
				
				tMenuIndex = tDestMenu1[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_AutoPressure;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Greese;
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// 1>3,2>1,3>2,4>6or5,5>4,6>5,7>8or7,8>7or8
				uint8_t tDestMenu2[9] = {0,3,1,2,6,4,5,8,7};
				
				tMenuIndex = tDestMenu2[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_AutoPressure;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Greese;
				}
			}
			if(tMenuIndex > 0)	// jhlee 231011
			{
//				M7_SubMenuClear();

				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton UP Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;

		
		case JOG_DIAL_KEY_DOWN:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// 1>2,2>3,3>4,4>5,5>6or1,6>1,7>8or7,8>7or8
				uint8_t tDestMenu3[9] = {0,2,3,4,5,6,1,8,7};
				
				tMenuIndex = tDestMenu3[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
						tMenuIndex = mPreset_1st;
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_AutoPressure;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Greese;
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// 1>2,2>3,3>1,4>5,5>6or4,6>4,7>8or7,8>7or8
				uint8_t tDestMenu4[9] = {0,2,3,1,5,6,4,8,7};
				
				tMenuIndex = tDestMenu4[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
						tMenuIndex = mPreset_Opt;
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_AutoPressure;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Greese;
				}
			}
			
			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton DOWN Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;

		case JOG_DIAL_KEY_LEFT:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// 1>7or1,2>8or2,3>8or2,4>8or2,5>8or2,6>8or2,7>1,8>2
				uint8_t tDestMenu5[9] = {0,7,8,8,8,8,8,1,2};
				
				tMenuIndex = tDestMenu5[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_1st;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					//chpark.24.11.07
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = MenuPreset_Choice_Index;
				}

			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// 1>7or4,2>8or5,3>6or3,4>1,5>2,6>3,7>4,8>5
				uint8_t tDestMenu6[9] = {0,7,8,6,1,2,3,4,5};
				
				tMenuIndex = tDestMenu6[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
						tMenuIndex = mPreset_Lang;
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_Opt;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Radio;
				}
			}

			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton LEFT Branch Submenu : %s,%d\r\n",M7_PARA,MenuPreset_Choice_Index);
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;

		case JOG_DIAL_KEY_RIGHT:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// 1>7or1,2>8or2,3>8or2,4>8or2,5>8or2,6>8or2,7>1,8>2
				uint8_t tDestMenu7[9] = {0,7,8,8,8,8,8,1,2};
				
				tMenuIndex = tDestMenu7[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_1st;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = MenuPreset_Choice_Index;
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// 1>4,2>5,3>6or8or2,4>7or1,5>8or2,6>8or2,7>1,8>2
				//uint8_t tDestMenu8[9] = {0,4,5,6,7,8,8,1,2};
				uint8_t tDestMenu8[9] = {0,4,5,6,7,8,3,1,2}; //chpark.24.11.15
				
				tMenuIndex = tDestMenu8[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_User)
				{
					if(CheckM6_9()==1)
						tMenuIndex = mPreset_User;
					else
					{
						tMenuIndex = MenuPreset_Choice_Index;//chpark.24.11.15
					}
				}
				else if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_1st;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_2nd;
				}
			}

			if(tMenuIndex > 0)		// jhlee 231011
			{
//				M7_SubMenuClear();
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton RIGHT Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;

		case JOG_DIAL_KEY_ENTER_ON:
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:	// JPHong 2024-03-15
			if(MenuPreset_Choice_Index > 0)		
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",MenuPreset_Choice_Index);
				printf("[JogButton Enter] Branch Submenu : %s\r\n",M7_PARA);
		
				M7_BTN_MouseUp(NULL,M7_PARA);		
			}
		break;
	}

    return true;
}

bool M7_JogButton_KeyMode(char* param)
{
	uint8_t tJogButton = 0;
	uint8_t tMenuIndex = 0;

	M7_ExitCNT = 0;
	
	tJogButton = (uint8_t)param[0];
	
	printf("[M7_JogButton_KeyMode] %d\r\n",tJogButton);

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				//					   //[0,1,2, 3,4  5,6,7,8,9  10,11,12,13];
				uint8_t tDestMenu1[14] = {0,4,1,13,3, 8,0,5,7,0,  0, 0, 2,12};
				
				tMenuIndex = tDestMenu1[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						tMenuIndex= mPreset_lock;
					}
					else
					{
						tMenuIndex= mPreset_2nd;
					}
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				//					   //[0, 1,2, 3,4  5,6,7,8,9  10,11,12,13]
				uint8_t tDestMenu2[14] = {0,12,1,13,3, 8,0,5,7,0,  0, 0, 2, 4};
				
				tMenuIndex = tDestMenu2[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
					{
						tMenuIndex = mPreset_AutoPressure;
					}
					else if(M7_AG_OptFlag == 1)
					{
						tMenuIndex = mPreset_Greese;
					}
					else
					{
						tMenuIndex = mPreset_Radio;  
					}
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						tMenuIndex= mPreset_lock;
					}
					else
					{
						tMenuIndex= mPreset_2nd;
					}
				}
			}
			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton UP Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_DOWN:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				//					   //[0,1, 2,3,4  5,6,7,8,9  10,11,12,13]
				uint8_t tDestMenu3[14] = {0,2,12,4,1, 7,0,8,5,0,  0, 0,13, 3};
				
				tMenuIndex = tDestMenu3[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						tMenuIndex= mPreset_lock;
					}
					else
					{
						tMenuIndex= mPreset_password;
					}
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				//						//[0,1, 2,3, 4  5,6,7,8,9  10,11,12,13];
				uint8_t tDestMenu4[14]  = {0,2,12,4,13, 7,0,8,5,0,  0, 0,1, 3};
				tMenuIndex = tDestMenu4[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
					{
						tMenuIndex = mPreset_Greese;
					}
					else if(M7_AP_OptFlag == 1)
					{
						tMenuIndex = mPreset_AutoPressure;
					}
					else
					{
						tMenuIndex = mPreset_Radio;  
					}
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Radio;
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						tMenuIndex= mPreset_lock;
					}
					else
					{
						tMenuIndex= mPreset_1st;
					}
				}
			}
			
			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton DOWN Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_LEFT:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				//					   //[0,1,2,3,4  5,6,7, 8,9  10,11,12,13]
				uint8_t tDestMenu5[14] = {0,5,7,8,8, 1,0,2,12,0,  0, 0, 8, 8};
				
				tMenuIndex = tDestMenu5[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_2nd;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = MenuPreset_Choice_Index;//don't move
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				//						//[0,1,2,3, 4   5,6,7,8,9  10,11,12,13];
				uint8_t tDestMenu6[14]  = {0,5,7,2,12, 13,0,3,4,0,  0, 0, 8, 1};

				tMenuIndex = tDestMenu6[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese;
					else
						tMenuIndex = mPreset_Lang;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure;
					else
						tMenuIndex = mPreset_Opt;
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						tMenuIndex = mPreset_lock;
					}
					else if(M7_AP_OptFlag == 1)
					{
						tMenuIndex = mPreset_AutoPressure;
					}
					else
					{
						tMenuIndex = mPreset_Opt;
					}
				}
			}

			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton LEFT Branch Submenu : %s,%d\r\n",M7_PARA,MenuPreset_Choice_Index);
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				//					   //[0,1,2,3,4  5,6,7, 8,9  10,11,12,13]
				uint8_t tDestMenu7[14] = {0,5,7,8,8, 1,0,2,12,0,  0, 0, 8, 8};
				
				tMenuIndex = tDestMenu7[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)//gray, off
						tMenuIndex = mPreset_Greese;
					else //on
						tMenuIndex = mPreset_2nd;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1 ) //gray
						tMenuIndex = mPreset_AutoPressure; 
					else //on
						tMenuIndex = MenuPreset_Choice_Index;//dont move
				}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				//						//[0, 1,2,3,4  5,6,7,8,9   10,11,12,13];
				uint8_t tDestMenu8[14]  = {0,13,3,7,8, 1,0,2,12,0,  0, 0, 4, 5};

				tMenuIndex = tDestMenu8[MenuPreset_Choice_Index];

				if(tMenuIndex == mPreset_Greese)
				{
					if(M7_AG_OptFlag == 1)
						tMenuIndex = mPreset_Greese; 
					else
						tMenuIndex = mPreset_2nd;
				}
				else if(tMenuIndex == mPreset_AutoPressure)
				{
					if(M7_AP_OptFlag == 1)
						tMenuIndex = mPreset_AutoPressure; 
					else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
						tMenuIndex = mPreset_lock; 
					else
						tMenuIndex = mPreset_Opt;
				}
				else if(tMenuIndex == mPreset_lock)
				{
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
						tMenuIndex = mPreset_lock; 
					else
						tMenuIndex = mPreset_Opt;
				}
			}

			if(tMenuIndex > 0)		// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton RIGHT Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_ON:
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:	// JPHong 2024-03-15
			if(MenuPreset_Choice_Index > 0)
			{	
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",MenuPreset_Choice_Index);
				printf("JogButton Enter Branch Submenu : %s\r\n",M7_PARA);
		
				M7_BTN_MouseUp(NULL,M7_PARA);		
			}
		break;
	}

	return true;
}

bool M7_JogButton_LockMode(char* param)
{
	uint8_t tJogButton = 0;
	uint8_t tMenuIndex = 0;

	M7_ExitCNT = 0;
	
	tJogButton = (uint8_t)param[0];
	
	printf("[M7_JogButton_LockMode] %d\r\n",tJogButton);
	
	tMenuIndex = MenuPreset_Choice_Index;

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			switch(tMenuIndex)
			{
				case mPreset_1st:
					if(M7_AP_OptFlag == 1)
					{
						tMenuIndex = mPreset_AutoPressure;
					}
					else
					{
						tMenuIndex = mPreset_2nd;
					}
					break;

				case mPreset_2nd:
					tMenuIndex = mPreset_1st;
					break;

				case mPreset_AutoPressure:
					tMenuIndex = mPreset_2nd;
					//error check.
					if(M7_AP_OptFlag != 1)
					{
						printf("[M7_JogWheel_LockMode][Error][Left] M7_AP_OptFlag==%d \n",M7_AP_OptFlag);
						tMenuIndex = mPreset_2nd;
					}
					break;	

				default:

					break;		
			}
	
				
			
			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton UP Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_DOWN:
		{
				switch(tMenuIndex)
				{
					case mPreset_1st:
						tMenuIndex = mPreset_2nd;
						break;

					case mPreset_2nd:
						if(M7_AP_OptFlag == 1)
						{
							tMenuIndex = mPreset_AutoPressure;
						}
						else
						{
							tMenuIndex = mPreset_1st;
						}
						break;

					case mPreset_AutoPressure:
						tMenuIndex = mPreset_1st;
						
						//error check.
						if(M7_AP_OptFlag != 1)
						{
							printf("[M7_JogWheel_LockMode][Error][Right] M7_AP_OptFlag==%d \n",M7_AP_OptFlag);
							tMenuIndex = mPreset_1st;
						}

						break;	

					default:

						break;		
				}
			
			if(tMenuIndex > 0)	// jhlee 231011
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",tMenuIndex);
				printf("JogButton DOWN Branch Submenu : %s\r\n",M7_PARA);
			
				if (MenuPreset_Choice_Index!=tMenuIndex)	//JPHong 2024-03-21
					M7_BTN_MouseDn(NULL, M7_PARA);
			}
		}
		break;
		
		case JOG_DIAL_KEY_LEFT:
		{
			
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
			
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_ON:
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:	// JPHong 2024-03-15
			if(MenuPreset_Choice_Index > 0)	
			{
				memset(M7_PARA,2,0);
				sprintf(M7_PARA,"%1d",MenuPreset_Choice_Index);
				printf("JogButton Enter Branch Submenu : %s\r\n",M7_PARA);
		
				M7_BTN_MouseUp(NULL,M7_PARA);		
			}
		break;
	}

	return true;
}
//======================================================

bool M7_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	M7_ExitCNT = 0;
	
	tJogSwitch = param[0];
	printf("M7_JogSwitch JogSwitch %d\r\n",tJogSwitch);
	
	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
	//	GotoMenuTop();
	}
	//else if(tJogSwitch == JOG_DIAL_SW_HOME)
	else if(tJogSwitch == JOG_DIAL_SW_HOME_OFF)//chpark.24.11.07
	{
		//chpark.24.09.12
		if(g_nM7_StartType == M7_MODE_LOCK)
		{
			//chpark.24.11.04
			bLayer_Changing = true;

			M7_LockMode_Choice_Index = MenuPreset_Choice_Index; //save index of lock mode
			MenuPreset_Choice_Index  = mPreset_lock;

			g_nM7_StartType = M7_MODE_KEY;
		}

		GotoMain(MAIN_MODE_NORMAL); 
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)	//JPHong 2024-03-22
	{
		//chpark.24.09.23
		if(g_nM7_StartType == M7_MODE_LOCK)
		{
			M7_LockMode_Choice_Index = MenuPreset_Choice_Index; //save index of lock mode
			MenuPreset_Choice_Index  = mPreset_lock;

			printf("[M7_JogSwitch][JOG_DIAL_SW_ESC_ON_OFF][M7_MODE_LOCK]M7_KeyMode_Choice_Index:%d MenuPreset_Choice_Index:%d M7_LockMode_Choice_Index=%d\n",M7_KeyMode_Choice_Index,MenuPreset_Choice_Index,M7_LockMode_Choice_Index);

			g_nM7_StartType = M7_MODE_KEY;
			M7_InitPosition(g_nM7_StartType);
		}
		else
		{
			GotoMenuTop();
		}
	}
    return true;
}

bool M7D7_MouseUp(ITUWidget* widget, char* param)
{
	M7_ExitCNT = 0;
	
	// goto select submenu layer
	memset(M7_PARA,2,0);
	sprintf(M7_PARA,"%1d",MenuPreset_Choice_Index);
	printf("D7 MouseUp Branch Submenu : %s\r\n",M7_PARA);
	
	M7_BTN_MouseUp(NULL,M7_PARA);

    return true;
}

bool M7D6_MouseUp(ITUWidget* widget, char* param)
{
	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		//chpark.24.11.04
		bLayer_Changing = true;

		M7_LockMode_Choice_Index = MenuPreset_Choice_Index; //save index of lock mode
		MenuPreset_Choice_Index  = mPreset_lock;

		g_nM7_StartType = M7_MODE_KEY;

		printf("[M7D6_MouseUp] reset\n");
	}

	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M7D5_MouseUp(ITUWidget* widget, char* param)
{
	//chpark.24.09.23
	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		M7_LockMode_Choice_Index = MenuPreset_Choice_Index; //save index of lock mode
		MenuPreset_Choice_Index  = mPreset_lock;

		printf("[M7D5_MouseUp]M7_KeyMode_Choice_Index:%d MenuPreset_Choice_Index:%d M7_LockMode_Choice_Index=%d\n",M7_KeyMode_Choice_Index,MenuPreset_Choice_Index,M7_LockMode_Choice_Index);

		g_nM7_StartType = M7_MODE_KEY;
		M7_InitPosition(g_nM7_StartType);
	}
	else
	{
		GotoMenuTop();
	}
	
    return true;
}

bool M7_ZoomMouseUp(ITUWidget* widget, char* param)
{
	uint8_t tM7_BTN;

	M7_ExitCNT = 0;
	tM7_BTN = (uint8_t)atoi(param);

	// select goto sub menu layer
	printf(">> JPHong M7_ZoomMouseUp [%d]\r\n",tM7_BTN);
	Branch_SubMenu(tM7_BTN);
	
    return true;
}



bool M7_BTN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tM7_BTN;

	M7_ExitCNT = 0;
	
	tM7_BTN = (uint8_t)atoi(param);

	printf("[M7_BTN_MouseDn] tM7_BTN:%d\n",tM7_BTN);

	//chpark.24.09.23
	if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
	{
		if(tM7_BTN == mPreset_lock)
		{
			printf("[M7_BTN_MouseDn] g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF return.\n");
			return false;
		}
	}
	
	// select submenu scale up
	if (MenuPreset_Choice_Index!=tM7_BTN)
		M7_SubMenuClear(MenuPreset_Choice_Index);
	
	switch(tM7_BTN)
	{
		case mPreset_1st:			// aux 1 preset
		{
			printf("User's AUX 3 Preset\r\n");
			MenuPreset_Choice_Index = mPreset_1st;

			ituWidgetSetVisible(M7_1_BTN, false);
			ituWidgetSetVisible(M7_1_ANI, true);

			M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_1_ANI,0);
		}
		break;
		
		case mPreset_2nd:			// aux 2,4 preset
		{
			printf("User's AUX 2,4 Preset\r\n");
			MenuPreset_Choice_Index = mPreset_2nd;

			ituWidgetSetVisible(M7_2a_BTN, false);
			ituWidgetSetVisible(M7_2b_BTN, false);
			ituWidgetSetVisible(M7_2a_ANI, true);
			ituWidgetSetVisible(M7_2b_ANI, true);

			M7_2a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_2a_ANI,0);

			M7_2b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_2b_ANI,0);
		}
		break;
		
		case mPreset_Lang:			// language
		{
			printf("SysTem Language Set\r\n");
			MenuPreset_Choice_Index = mPreset_Lang;

			ituWidgetSetVisible(M7_5_BTN, false);
			ituWidgetSetVisible(M7_5_ANI, true);

			M7_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_5_ANI,0);
		}
		break;
		
		case mPreset_Opt:			// system option
		{
			printf("SysTem Option Set : test %d\r\n",MenuPreset_Choice_Index);
			MenuPreset_Choice_Index = mPreset_Opt;

			ituWidgetSetVisible(M7_6_BTN, false);
			ituWidgetSetVisible(M7_6_ANI, true);

			M7_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_6_ANI,0);
		}
		break;
		
		case mPreset_Radio:			// radio region
		{
			printf("SysTem Radio Region Set\r\n");

			MenuPreset_Choice_Index = mPreset_Radio;

			ituWidgetSetVisible(M7_7_BTN, false);
			ituWidgetSetVisible(M7_7_ANI, true);

			M7_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_7_ANI,0);
		}
		break;

		case mPreset_User:			// user config
		{
			printf("SysTem User Info Set\r\n");
			MenuPreset_Choice_Index = mPreset_User;

			ituWidgetSetVisible(M7_9_BTN, false);
			ituWidgetSetVisible(M7_9_ANI, true);

			M7_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_9_ANI,0);
		}
		break;		
		
		case mPreset_Greese:			// auto greese config
		{
			if((M7_AG_OptFlag == 1) || (M7_AG_OptFlag == 2))
			{
				printf("SysTem Auto Greese Set\r\n");
				MenuPreset_Choice_Index = mPreset_Greese;

				ituWidgetSetVisible(M7_4_BTN, false);
				ituWidgetSetVisible(M7_4_ANI, true);

				M7_4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_4_ANI,0);
			}
			else if((M7_AG_OptFlag == 0)||(M7_AG_OptFlag == 3))
			{
				printf("Change User's AUX 2 Preset\r\n");
				MenuPreset_Choice_Index = mPreset_1st;

				ituWidgetSetVisible(M7_1_BTN, false);
				ituWidgetSetVisible(M7_1_ANI, true);

				M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_1_ANI,0);
			}
		}
		break;		
		
		case mPreset_AutoPressure:			// auto pressure config
		{		
			if(M7_AP_OptFlag == 1)
			{
				printf("SysTem Auto Pressure Set\r\n");
				MenuPreset_Choice_Index = mPreset_AutoPressure;

				ituWidgetSetVisible(M7_11_BTN, false);
				ituWidgetSetVisible(M7_11_ANI, true);

				M7_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_11_ANI,0);
			}
			else if((M7_AP_OptFlag == 0)||(M7_AP_OptFlag == 2)||(M7_AP_OptFlag == 3))		// off, off(hold), No Display
			{
				printf("Change User's AUX 1 Preset\r\n");
				MenuPreset_Choice_Index = mPreset_1st;

				ituWidgetSetVisible(M7_1_BTN, false);
				ituWidgetSetVisible(M7_1_ANI, true);

				M7_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M7_1_ANI,0);
			}
		}
		break;

		case mPreset_lock:			//setup menu's lock
		{
			printf("Lock Status Setting 1\r\n");
			MenuPreset_Choice_Index = mPreset_lock;
			
			ituWidgetSetVisible(M7_12_BTN, false);
			ituWidgetSetVisible(M7_12_ANI, true);

			M7_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_12_ANI,0);
		}
		break;		

		case mPreset_password:			//setting password
		{
			printf("PasswordON Setting\r\n");
			MenuPreset_Choice_Index = mPreset_password;

			ituWidgetSetVisible(M7_13_BTN, false);
			ituWidgetSetVisible(M7_13_ANI, true);

			M7_13_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M7_13_ANI,0);
		}
		break;		

		case 30:			// test
		{
			//for test
			if(g_nM7_StartType == M7_MODE_PUSH)//chpark.24.09.12 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
			{ 
				if(MenuPreset_Choice_Index<=0 )
				{
					printf("[TEST][StartType][M7_MODE_KEY][1]g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
					MenuPreset_Choice_Index = mPreset_1st;
				}

				g_nM7_StartType = M7_MODE_KEY;

				printf("[TEST][StartType][M7_MODE_KEY][1]g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
				
				/*
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
				{
					char tmp_Txt_CurPassWord[MAX_PW100_PWD+2];
					//continue
					 memcpy(tmp_Txt_CurPassWord,PW100_PWD,MAX_PW100_PWD);
					int len_cur;
					len_cur = strlen(tmp_Txt_CurPassWord);

					//=====================================================================
					int nErrNum=0;
					for(int i=0;i<MAX_PW100_PWD;i++)
					{
						if(tmp_Txt_CurPassWord[i]>=0x30 && tmp_Txt_CurPassWord[i]<=0x39)
						{
							//0~9 num 
						}
						else
						{
							nErrNum++;
							//printf("Error PW[%d]:%s\n",i,PW100_Txt_CurPassWord[i]);
						}
					}
					
					if(len_cur<4 || nErrNum>0)
					{
						memset(tmp_Txt_CurPassWord, '\0', sizeof(tmp_Txt_CurPassWord));
						memset(PW100_PWD, '\0', sizeof(PW100_PWD));
						len_cur = strlen(tmp_Txt_CurPassWord);
						printf("[PW100_Enter] Reset Old Pass.: nErrNum:%d\n",nErrNum);

						g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
					}
				}
				else
				{
					g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;

					memset(PW100_PWD, '\0', sizeof(PW100_PWD));
					memset(PW100_Txt_CurPassWord, '\0', sizeof(PW100_Txt_CurPassWord));

					for(int i=0<i<3;i++)
					{

					}
				}
				*/

				g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;

				memset(PW100_PWD, '\0', sizeof(PW100_PWD));

				for(int i=0;i<3;i++)
				{
					g_nM7_Lock_Data[i].Lock = 0;
					g_nM7_Lock_Data[i].Show = 0;
				}

				printf("[TEST][StartType][M7_MODE_KEY][2]g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
				
			}
			else if(g_nM7_StartType == M7_MODE_KEY)
			{
				if(MenuPreset_Choice_Index<=0 || MenuPreset_Choice_Index==mPreset_lock || MenuPreset_Choice_Index==mPreset_password)
				{
					printf("[TEST][StartType][M7_MODE_KEY][1]g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
					MenuPreset_Choice_Index = mPreset_1st;
				}

				g_nM7_StartType = M7_MODE_PUSH;
				printf("[TEST][StartType][M7_MODE_PUSH]g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
			}
			else
			{

			}
			//
			M7_InitPosition(g_nM7_StartType);//for test
			M7_UserDisplay();//for test
		}
		break;

		case 31:
		{

		}

		break;
	}
	
    return true;
}

bool M7_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tM7_BTN;
	
	M7_ExitCNT = 0;
	
	tM7_BTN = (uint8_t)atoi(param);

	//chpark.24.09.23
	if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
	{
		if(tM7_BTN == mPreset_lock)
		{
			printf("[M7_BTN_MouseUp] g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF return.\n");
			return false;
		}
	}

	// select goto sub menu layer
	printf(">> JPHong M7_BTN_MouseUp [%d]\r\n",tM7_BTN);
	if ((tM7_BTN==8)&&(M7_11_Scaleflag == 1))
	{
		M7_BTN_MouseUpFlag = 1;
	}
	else
	{
		M7_BTN_MouseUpFlag = 0;
		Branch_SubMenu(tM7_BTN);
	}
	
    return true;
}


bool M7_13_ANIStopped(ITUWidget* widget, char* param)
{
	printf("\n>> JPHong M7_13_ANIStopped [%d][%d] MenuPreset_Choice_Index=%d\r\n",M7_13_Scaleflag,M7_BTN_MouseUpFlag,MenuPreset_Choice_Index);
	if(M7_13_Scaleflag == 1)
	{
		M7_13_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_13_ANI, false);
		ituWidgetSetVisible(M7_13_ZoomBTN, true);
	}
	else if(M7_13_Scaleflag == 2)
	{
		M7_13_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_13_ANI, false);
		ituWidgetSetVisible(M7_13_BTN, true);
	}
	//printf("[M7_13_ANIStopped] g_nM7_StartType=%d ituWidgetIsVisible=%d\n",g_nM7_StartType,ituWidgetIsVisible(M7_12_ZoomBTN));

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_13_ZoomBTN, false);
		ituWidgetSetVisible(M7_13_BTN, false);
	}

    return true;
}

bool M7_12_ANIStopped(ITUWidget* widget, char* param)
{
	printf("\n>> JPHong M7_12_ANIStopped [%d][%d][MenuPreset_Choice_Index:%d][g_bPW100_PW_OnOff:%d]\r\n",M7_12_Scaleflag,M7_BTN_MouseUpFlag,MenuPreset_Choice_Index,g_bPW100_PW_OnOff);
	if(M7_12_Scaleflag == 1)
	{
		M7_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_12_ANI, false);
		ituWidgetSetVisible(M7_12_ZoomBTN, true);
		
		int nX  = ituWidgetGetX(M7_12_ZoomBTN);
        int nY  = ituWidgetGetY(M7_12_ZoomBTN);
		printf("[M7_12_ANIStopped][up] M7_12_ZoomBTN Show: %d\n",ituWidgetIsVisible(M7_12_ZoomBTN));
		printf("[M7_12_ANIStopped][up] [Zoom]nX=%d nY=%d ituWidgetIsVisible=%d\n",nX,nY,ituWidgetIsVisible(M7_12_ZoomBTN));
	}
	else if(M7_12_Scaleflag == 2)
	{
		M7_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_12_ANI, false);
		ituWidgetSetVisible(M7_12_BTN, true);
		
		int nX  = ituWidgetGetX(M7_12_BTN);
        int nY  = ituWidgetGetY(M7_12_BTN);
		printf("[M7_12_ANIStopped][down] M7_12_BTN Show: %d\n",ituWidgetIsVisible(M7_12_BTN));
		printf("[M7_12_ANIStopped][down] [Normal]nX=%d nY=%d ituWidgetIsVisible=%d\n",nX,nY,ituWidgetIsVisible(M7_12_BTN));
	}

	//chpark.24.09.23
	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		//ituWidgetSetVisible(M7_12_BTN, false); //no need
		ituWidgetSetVisible(M7_12_ZoomBTN, false);

		printf("[M7_12_ANIStopped][M7_MODE_LOCK] disable. g_nM7_StartType=%d\n",g_nM7_StartType);
	}
	else
	{
		//ituWidgetSetVisible(M7_12_ZoomBTN, true);
		printf("[M7_12_ANIStopped] g_nM7_StartType=%d 12BTN=%d 12ZoomBTN=%d\n",g_nM7_StartType,ituWidgetIsVisible(M7_12_BTN),ituWidgetIsVisible(M7_12_ZoomBTN));
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_12_ZoomBTN, false);
		ituWidgetSetVisible(M7_12_BTN, false);
	}
	
    return true;
}

bool M7_11_ANIStopped(ITUWidget* widget, char* param)
{
	printf(">> JPHong M7_11_ANIStopped [%d][%d]\r\n",M7_11_Scaleflag,M7_BTN_MouseUpFlag);
	if(M7_11_Scaleflag == 1)
	{
		M7_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_11_ANI, false);
		ituWidgetSetVisible(M7_11_ZoomBTN, true);
	}
	else if(M7_11_Scaleflag == 2)
	{
		M7_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_11_ANI, false);
		ituWidgetSetVisible(M7_11_BTN, true);
	}
	if (M7_BTN_MouseUpFlag)
	{
		M7_BTN_MouseUpFlag = 0;
		Branch_SubMenu(8);
	}


    return true;
}

bool M7_4_ANIStopped(ITUWidget* widget, char* param)
{
	if(M7_4_Scaleflag == 1)
	{
		M7_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_4_ANI, false);
		ituWidgetSetVisible(M7_4_ZoomBTN, true);
	}
	else if(M7_4_Scaleflag == 2)
	{
		M7_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_4_ANI, false);
		ituWidgetSetVisible(M7_4_BTN, true);
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_4_ZoomBTN, false);
		ituWidgetSetVisible(M7_4_BTN, false);
	}

    return true;
}

bool M7_9_ANIStopped(ITUWidget* widget, char* param)
{
	if(M7_9_Scaleflag == 1)
	{
		M7_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_9_ANI, false);
		ituWidgetSetVisible(M7_9_ZoomBTN, true);
	}
	else if(M7_9_Scaleflag == 2)
	{
		M7_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_9_ANI, false);
		ituWidgetSetVisible(M7_9_BTN, true);
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_9_ZoomBTN, false);
		ituWidgetSetVisible(M7_9_BTN, false);
	}
	
    return true;
}

bool M7_7_ANIStopped(ITUWidget* widget, char* param)
{
	if(M7_7_Scaleflag == 1)
	{
		M7_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_7_ANI, false);
		ituWidgetSetVisible(M7_7_ZoomBTN, true);
	}
	else if(M7_7_Scaleflag == 2)
	{
		M7_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_7_ANI, false);
		ituWidgetSetVisible(M7_7_BTN, true);
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_7_ZoomBTN, false);
		ituWidgetSetVisible(M7_7_BTN, false);
	}

    return true;
}

bool M7_6_ANIStopped(ITUWidget* widget, char* param)
{
	if(M7_6_Scaleflag == 1)
	{
		M7_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_6_ANI, false);
		ituWidgetSetVisible(M7_6_ZoomBTN, true);
	}
	else if(M7_6_Scaleflag == 2)
	{
		M7_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_6_ANI, false);
		ituWidgetSetVisible(M7_6_BTN, true);
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_6_ZoomBTN, false);
		ituWidgetSetVisible(M7_6_BTN, false);
	}

    return true;
}

bool M7_5_ANIStopped(ITUWidget* widget, char* param)
{
	printf(">> JPHong M7_5_ANIStopped [%d][%d]\r\n",M7_5_Scaleflag,M7_BTN_MouseUpFlag);
	if(M7_5_Scaleflag == 1)
	{
		M7_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_5_ANI, false);
		ituWidgetSetVisible(M7_5_ZoomBTN, true);
	}
	else if(M7_5_Scaleflag == 2)
	{
		M7_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_5_ANI, false);
		ituWidgetSetVisible(M7_5_BTN, true);
	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_5_ZoomBTN, false);
		ituWidgetSetVisible(M7_5_BTN, false);
	}

    return true;
}

bool M7_2b_ANIStopped(ITUWidget* widget, char* param)
{
	printf(">> JPHong M7_2b_ANIStopped [%d][%d]\r\n",M7_11_Scaleflag,M7_BTN_MouseUpFlag);
	if(M7_2b_Scaleflag == 1)
	{
		M7_2b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_2b_ANI, false);
		ituWidgetSetVisible(M7_2b_ZoomBTN, true);
	}
	else if(M7_2b_Scaleflag == 2)
	{
		M7_2b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_2b_ANI, false);
		ituWidgetSetVisible(M7_2b_BTN, true);
	}

    return true;
}

bool M7_2a_ANIStopped(ITUWidget* widget, char* param)
{
	printf(">> JPHong M7_2a_ANIStopped [%d][%d]\r\n",M7_11_Scaleflag,M7_BTN_MouseUpFlag);
	if(M7_2a_Scaleflag == 1)
	{
		M7_2a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_2a_ANI, false);
		ituWidgetSetVisible(M7_2a_ZoomBTN, true);
	}
	else if(M7_2a_Scaleflag == 2)
	{
		M7_2a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_2a_ANI, false);
		ituWidgetSetVisible(M7_2a_BTN, true);
	}

    return true;
}

bool M7_1_ANIStopped(ITUWidget* widget, char* param)
{
	printf(">> JPHong M7_1_ANIStopped [%d][%d]\r\n",M7_1_Scaleflag,M7_BTN_MouseUpFlag);
	if(M7_1_Scaleflag == 1)
	{
		M7_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_1_ANI, false);
		ituWidgetSetVisible(M7_1_ZoomBTN, true);
	}
	else if(M7_1_Scaleflag == 2)
	{
		M7_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M7_1_ANI, false);
		ituWidgetSetVisible(M7_1_BTN, true);
	}

    return true;
}

bool M7_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M7_TopAdmin = ituSceneFindWidget(&theScene, "M7_TopAdmin");
	ITUWidget *M7_TopStandard = ituSceneFindWidget(&theScene, "M7_TopStandard");
	
	ITUText *M7_TopUserName = ituSceneFindWidget(&theScene, "M7_TopUserName");
	
	if(g_nM7_StartType==M7_MODE_PUSH)
	{
		if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
		{
			ituWidgetSetVisible(M7_TopStandard,false);
			ituWidgetSetVisible(M7_TopAdmin,true);
		}
		else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
		{
			ituWidgetSetVisible(M7_TopAdmin,false);
			ituWidgetSetVisible(M7_TopStandard,true);
		}
		else
		{
			ituWidgetSetVisible(M7_TopAdmin,false);
			ituWidgetSetVisible(M7_TopStandard,false);
		}

		if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
		{
			memset(tDestName,0,100);
		
			SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
			ituTextSetString(M7_TopUserName, tDestName);

			ituWidgetSetVisible(M7_TopUserName,true);
		}
		else
		{
			ituWidgetSetVisible(M7_TopUserName,false);
		}
	}
	else
	{
		ituWidgetSetVisible(M7_TopAdmin,	false);
		ituWidgetSetVisible(M7_TopStandard,	false);
		ituWidgetSetVisible(M7_TopUserName,	false);
	}
}

void M7_SubMenuClear(uint8_t MenuIndex)
{
	printf("M7_SubMenuClear [%d][%d]\n",MenuIndex,MenuPreset_Choice_Index);
	switch(MenuPreset_Choice_Index)			// default 1st AUX
	{
		case mPreset_1st:
		{
//			if(ituWidgetIsVisible(M7_1_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_1_ZoomBTN, false);
				ituWidgetSetVisible(M7_1_ANI, true);
				M7_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_1_ANI, 1);
			}		
		}
		break;
		case mPreset_2nd:
		{
//			if(ituWidgetIsVisible(M7_2a_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_2a_ZoomBTN, false);
				ituWidgetSetVisible(M7_2b_ZoomBTN, false);
				ituWidgetSetVisible(M7_2a_ANI, true);
				ituWidgetSetVisible(M7_2b_ANI, true);
				M7_2a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_2a_ANI, 1);
				M7_2b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_2b_ANI, 1);
			}
		}
		break;
		case mPreset_Lang:
		{
//			if(ituWidgetIsVisible(M7_5_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_5_ZoomBTN, false);
				ituWidgetSetVisible(M7_5_ANI, true);
				M7_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_5_ANI, 1);
			}
		}
		break;
		case mPreset_Opt:
		{
//			if(ituWidgetIsVisible(M7_6_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_6_ZoomBTN, false);
				ituWidgetSetVisible(M7_6_ANI, true);
				M7_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_6_ANI, 1);
			}
		}
		break;

		case mPreset_Radio:
		{
//			if(ituWidgetIsVisible(M7_7_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_7_ZoomBTN, false);
				ituWidgetSetVisible(M7_7_ANI, true);
				M7_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_7_ANI, 1);
			}
		}
		break;
		case mPreset_User:
		{
//			if(ituWidgetIsVisible(M7_9_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_9_ZoomBTN, false);
				ituWidgetSetVisible(M7_9_ANI, true);
				M7_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_9_ANI, 1);
			}
		}
		break;
		case mPreset_Greese:
		{
//			if(ituWidgetIsVisible(M7_4_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_4_ZoomBTN, false);
				ituWidgetSetVisible(M7_4_ANI, true);
				M7_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_4_ANI, 1);
			}
		}
		break;
		case mPreset_AutoPressure:
		{
//			if(ituWidgetIsVisible(M7_11_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M7_11_ZoomBTN, false);
				ituWidgetSetVisible(M7_11_ANI, true);
				M7_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_11_ANI, 1);
			}
		}
		break;

		case mPreset_lock:
		{
//			if(ituWidgetIsVisible(M7_12_ZoomBTN) == true)
			{
				printf("[M7_SubMenuClear]Hide 12 Zoom Btn==\n");
				
				ituWidgetSetVisible(M7_12_ZoomBTN, false);
				ituWidgetSetVisible(M7_12_ANI, true);
				M7_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_12_ANI, 1);
				
			}
		}
		break;

		case mPreset_password:
		{
//			if(ituWidgetIsVisible(M7_13_ZoomBTN) == true)
			{
				printf("[M7_SubMenuClear]Hide 13 Zoom Btn==\n");
				ituWidgetSetVisible(M7_13_ZoomBTN, false);
				ituWidgetSetVisible(M7_13_ANI, true);
				M7_13_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M7_13_ANI, 1);
			}
		}
		break;
	}
}

//chpark.24.11.19
void M7_TitleUpdate(uint8_t tLang)
{
	ITUWidget *M7_Title_ENG = ituSceneFindWidget(&theScene, "M7_Title_ENG");
	assert(M7_Title_ENG);
	ITUWidget *M7_Title_DEU = ituSceneFindWidget(&theScene, "M7_Title_DEU");
	assert(M7_Title_DEU);
	ITUWidget *M7_Title_FRA = ituSceneFindWidget(&theScene, "M7_Title_FRA");
	assert(M7_Title_FRA);
	ITUWidget *M7_Title_ITA = ituSceneFindWidget(&theScene, "M7_Title_ITA");
	assert(M7_Title_ITA);
	ITUWidget *M7_Title_ESP = ituSceneFindWidget(&theScene, "M7_Title_ESP");
	assert(M7_Title_ESP);

	//chpark.24.11.18
	if(g_nM7_StartType==M7_MODE_PUSH|| g_nM7_StartType==M7_MODE_LOCK)
	{
		ituWidgetSetVisible(M7_Title_ENG, false);
		ituWidgetSetVisible(M7_Title_DEU, false);
		ituWidgetSetVisible(M7_Title_FRA, false);
		ituWidgetSetVisible(M7_Title_ITA, false);
		ituWidgetSetVisible(M7_Title_ESP, false);
	}
	else
	{
		switch(tLang)
		{
			case Lang_ENG:
			{
				ituWidgetSetVisible(M7_Title_DEU, false);
				ituWidgetSetVisible(M7_Title_FRA, false);
				ituWidgetSetVisible(M7_Title_ITA, false);
				ituWidgetSetVisible(M7_Title_ESP, false);

				ituWidgetSetVisible(M7_Title_ENG, true);
			}
			break;
			
			case Lang_DEU:
			{
				ituWidgetSetVisible(M7_Title_ENG, false);
				ituWidgetSetVisible(M7_Title_FRA, false);
				ituWidgetSetVisible(M7_Title_ITA, false);
				ituWidgetSetVisible(M7_Title_ESP, false);

				ituWidgetSetVisible(M7_Title_DEU, true);
			}
			break;
			
			case Lang_FRA:
			{
				ituWidgetSetVisible(M7_Title_ENG, false);
				ituWidgetSetVisible(M7_Title_DEU, false);
				ituWidgetSetVisible(M7_Title_ITA, false);
				ituWidgetSetVisible(M7_Title_ESP, false);

				ituWidgetSetVisible(M7_Title_FRA, true);
			}
			break;

			case Lang_ITA:
			{
				ituWidgetSetVisible(M7_Title_ENG, false);
				ituWidgetSetVisible(M7_Title_DEU, false);
				ituWidgetSetVisible(M7_Title_FRA, false);
				ituWidgetSetVisible(M7_Title_ESP, false);

				ituWidgetSetVisible(M7_Title_ITA, true);
			}
			break;

			case Lang_ESP:
			{
				ituWidgetSetVisible(M7_Title_ENG, false);
				ituWidgetSetVisible(M7_Title_DEU, false);
				ituWidgetSetVisible(M7_Title_FRA, false);
				ituWidgetSetVisible(M7_Title_ITA, false);

				ituWidgetSetVisible(M7_Title_ESP, true);
			}
			break;
		}
	}
}

void Branch_SubMenu(uint8_t tSubMenu)
{
	printf("[Branch_SubMenu] g_nM7_StartType=%d g_bPW100_PW_OnOff=%d tSubMenu=%d M7_KeyMode_Choice_Index=%d\n",g_nM7_StartType, g_bPW100_PW_OnOff,tSubMenu,M7_KeyMode_Choice_Index);

	ITULayer *tBranchLayer;
	
	switch(tSubMenu)
	{
		case mPreset_1st:
			if(g_nM7_StartType==M7_MODE_LOCK)
			{
				M7_Toggle_LockData(0);//chpark.24.09.12
			}
			else
			{
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF || g_nM7_Lock_Data[0].Show == M7_LSTATE_HIDE || g_nM7_Lock_Data[0].Lock == M7_LSTATE_UNLOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_M7_LAYER;
					/*
					#ifdef RELEASE_VER
						tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
					#else
						tBranchLayer = ituSceneFindWidget(&theScene, "M7_S5");
					#endif
					*/
					tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
					ituLayerGoto(tBranchLayer);
				}
				else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON && g_nM7_Lock_Data[0].Lock == M7_LSTATE_LOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_1st_LAYER;

					g_nPW100_LayerType= PW100_MODE_INPUT;
					tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
					ituLayerGoto(tBranchLayer);
				}
			}

		break;
		
		case mPreset_2nd:
			if(g_nM7_StartType==M7_MODE_LOCK)
			{
				M7_Toggle_LockData(1);//chpark.24.09.12
			}
			else
			{
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF || g_nM7_Lock_Data[1].Show == M7_LSTATE_HIDE ||g_nM7_Lock_Data[1].Lock == M7_LSTATE_UNLOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_M7_LAYER;
					/*
					#ifdef RELEASE_VER
					tBranchLayer = ituSceneFindWidget(&theScene, "M7_S2");
					#else
					tBranchLayer = ituSceneFindWidget(&theScene, "M7_S6");
					#endif
					*/
					tBranchLayer = ituSceneFindWidget(&theScene, "M7_S2");
					ituLayerGoto(tBranchLayer);
				}
				else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON && g_nM7_Lock_Data[1].Lock == M7_LSTATE_LOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_2nd_LAYER;

					g_nPW100_LayerType= PW100_MODE_INPUT;
					tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
					ituLayerGoto(tBranchLayer);
				}
			}

		break;		

		case mPreset_Lang:
			tBranchLayer = ituSceneFindWidget(&theScene, "M7_S5");
			assert(tBranchLayer);
			ituLayerGoto(tBranchLayer);

		break;		

		case mPreset_Opt:
			tBranchLayer = ituSceneFindWidget(&theScene, "M7_S6");
			assert(tBranchLayer);
			ituLayerGoto(tBranchLayer);
			
		break;		
		
		case mPreset_Radio:
			tBranchLayer = ituSceneFindWidget(&theScene, "M7_S7");
			assert(tBranchLayer);
			ituLayerGoto(tBranchLayer);
			
		break;		

		case mPreset_User:
			//tBranchLayer = ituSceneFindWidget(&theScene, "M7_S9");
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				tBranchLayer = ituSceneFindWidget(&theScene, "M7_SU100");
			}
			else
			{
				tBranchLayer = ituSceneFindWidget(&theScene, "M7_SU100");
			}

			assert(tBranchLayer);
			ituLayerGoto(tBranchLayer);

		break;		

		case mPreset_Greese:
			tBranchLayer = ituSceneFindWidget(&theScene, "M7_S4");
			assert(tBranchLayer);
			ituLayerGoto(tBranchLayer);

		break;		

		case mPreset_AutoPressure:
			if(g_nM7_StartType==M7_MODE_LOCK)
			{
				M7_Toggle_LockData(2);//chpark.24.09.12
			}
			else
			{
				if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF || g_nM7_Lock_Data[2].Show == M7_LSTATE_HIDE || g_nM7_Lock_Data[2].Lock == M7_LSTATE_UNLOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_M7_LAYER;

					tBranchLayer = ituSceneFindWidget(&theScene, "M7_S11");
					ituLayerGoto(tBranchLayer);
				}
				else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON && g_nM7_Lock_Data[2].Lock == M7_LSTATE_LOCK)
				{
					g_nPW100_PreLayer = PW100_M7_LAYER;
					g_nPW100_NextLayer= PW100_AutoPressure_LAYER;

					g_nPW100_LayerType= PW100_MODE_INPUT;
					tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
					ituLayerGoto(tBranchLayer);
				}
			}

		break;		

		//========================================================
		//chpark.24.09.12
		case mPreset_lock:
			if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
			{
				//g_nPW100_PreLayer = PW100_M7_LAYER;
				//g_nPW100_NextLayer= PW100_M7_LAYER;
				//M7_InitPosition(M7_MODE_LOCK);
			}
			else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
			{
				printf("[SubMenu]M7_KeyMode_Choice_Index:%d MenuPreset_Choice_Index:%d M7_LockMode_Choice_Index=%d\n",M7_KeyMode_Choice_Index,MenuPreset_Choice_Index,M7_LockMode_Choice_Index);
				g_nPW100_PreLayer = PW100_M7_LAYER;
				g_nPW100_NextLayer= PW100_Lock_LAYER;

				g_nPW100_LayerType= PW100_MODE_INPUT;
				tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);
			}
		break;		

		case mPreset_password:
			if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
			{
				g_nPW100_PreLayer = PW100_M7_LAYER;
				g_nPW100_NextLayer= PW100_M7_LAYER;

				g_nPW100_LayerType= PW100_MODE_SET;
				tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);
			}
			else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
			{
				printf("[SubMenu]M7_KeyMode_Choice_Index:%d MenuPreset_Choice_Index:%d M7_LockMode_Choice_Index=%d\n",M7_KeyMode_Choice_Index,MenuPreset_Choice_Index,M7_LockMode_Choice_Index);

				g_nPW100_PreLayer = PW100_M7_LAYER;
				g_nPW100_NextLayer= PW100_PassSet_LAYER;

				g_nPW100_LayerType= PW100_MODE_INPUT;
				tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);
			}

		break;		
		//========================================================
		default:

		break;
	}
}

void M7_SU_Display(uint8_t tDis_Flag)
{
	if(g_nM7_StartType ==M7_MODE_KEY || g_nM7_StartType == M7_MODE_LOCK)
	{
		//printf("[M7_AG_Display] g_nM7_StartType == %d return.\n",g_nM7_StartType);
		return;
	}

	if(tDis_Flag == 1)		// M7_S9 User Edit Display on
	{
//		printf("M7_SU_Display [%d]\r\n",M7_9_Scaleflag);
#if 1		
		if(ituWidgetIsVisible(M7_9_ZoomBTN) == true)	// JPHong 2024-03-20 14587
		{
			ituWidgetSetVisible(M7_9_ZoomBTN, true);
			ituWidgetSetVisible(M7_9_BTN, false);			
		}
		else
		{
			ituWidgetSetVisible(M7_9_ZoomBTN, false);
			ituWidgetSetVisible(M7_9_BTN, true);
		}
#else
			ituWidgetSetVisible(M7_9_ZoomBTN, false);
			ituWidgetSetVisible(M7_9_BTN, true);
#endif
	}
	else if(tDis_Flag == 0)		// M7_S9 User Edit Display Off
	{
		ituWidgetSetVisible(M7_9_ZoomBTN, false);

		ituWidgetSetVisible(M7_9_BTN, false);
	}
}

void M7_AG_Display(uint8_t tDis_Flag)
{
	if(bLayer_Changing)
	{
		printf("[M7_AG_Display]bLayer_Changing=%d\n",bLayer_Changing);
		return;//chpark.24.10.15
	}
	else
	{

	}

	if(g_nM7_StartType == M7_MODE_LOCK)
	{
		//printf("[M7_AG_Display] g_nM7_StartType == %dreturn.\n",g_nM7_StartType);
		return;
	}

	if((tDis_Flag == 1)||(tDis_Flag == 2))		// M7_S4 Auto Grease Display on
	{
		ituWidgetSetVisible(M7_4_ZoomBTN, false);
		ituWidgetSetVisible(M7_4_ANI, false);
		ituWidgetSetVisible(M7_4_G, false);

		ituWidgetSetVisible(M7_4_BTN, true);
	}
	else if(tDis_Flag == 3)		// M7_S4 Auto Grease Display off
	{
		ituWidgetSetVisible(M7_4_ZoomBTN, false);
		ituWidgetSetVisible(M7_4_ANI, false);
		ituWidgetSetVisible(M7_4_G, false);

		ituWidgetSetVisible(M7_4_BTN, false);
	}
	else if(tDis_Flag == 0)		// M7_S4 Auto Grease Gray Out
	{
		ituWidgetSetVisible(M7_4_BTN, false);
		ituWidgetSetVisible(M7_4_ZoomBTN, false);
		ituWidgetSetVisible(M7_4_ANI, false);

		ituWidgetSetVisible(M7_4_G, true);
	}
}

void M7_AP_Display(uint8_t tDis_Flag)
{
	if(bLayer_Changing)return;//chpark.24.10.15

	if(tDis_Flag == 1)		// M7_S11 Auto Pressure Display on
	{
		ituWidgetSetVisible(M7_11_ZoomBTN, false);
		ituWidgetSetVisible(M7_11_ANI, false);
		ituWidgetSetVisible(M7_11_G, false);
		ituWidgetSetVisible(M7_11_BTN, true);
	}
	else if((tDis_Flag == 0)||(tDis_Flag == 2))		// M7_S11 Auto Pressure Gray Out
	{
		ituWidgetSetVisible(M7_11_ZoomBTN, false);
		ituWidgetSetVisible(M7_11_ANI, false);
		ituWidgetSetVisible(M7_11_G, true);
		ituWidgetSetVisible(M7_11_BTN, false);
	}
	else if(tDis_Flag == 3)		// M7_S11 Auto Pressure No Display
	{
		ituWidgetSetVisible(M7_11_ZoomBTN, false);
		ituWidgetSetVisible(M7_11_ANI, false);
		ituWidgetSetVisible(M7_11_G, false);
		ituWidgetSetVisible(M7_11_BTN, false);
	}
}

void GotoMenu7(void)
{
    ITULayer *Menu7_Layer = ituSceneFindWidget(&theScene, "Menu7_Layer");
    assert(Menu7_Layer);
    ituLayerGoto(Menu7_Layer);
}

bool M7_AniMenuStatus(void)
{
	return 0;

	if(ituWidgetIsVisible(M7_1_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 1)
		{
			M7_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			printf("[M7_1_ANIStopped] 111\n");
			M7_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_1_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 1)
		{
			M7_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			printf("[M7_1_ANIStopped] 222\n");
			M7_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_1_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 1)
		{
			M7_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			printf("[M7_1_ANIStopped] 333\n");
			M7_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_1_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 1)
		{
			M7_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			printf("[M7_1_ANIStopped] 444\n");
			M7_1_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M7_2a_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2a_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2a_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2a_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2a_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M7_2b_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2b_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2b_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_2b_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 2)
		{
			M7_2b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_2b_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M7_5_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 3)
		{
			M7_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_5_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 3)
		{
			M7_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_5_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 3)
		{
			M7_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_5_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 3)
		{
			M7_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_5_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M7_6_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 4)
		{
			M7_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_6_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 4)
		{
			M7_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_6_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 4)
		{
			M7_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_6_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 4)
		{
			M7_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_6_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M7_7_ZoomBTN) == true)
	{
		if(MenuPreset_Choice_Index != 5)
		{
			M7_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_7_ANI) == true)
	{
		if(MenuPreset_Choice_Index != 5)
		{
			M7_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_7_Up1) == true)
	{
		if(MenuPreset_Choice_Index != 5)
		{
			M7_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M7_7_Up0) == true)
	{
		if(MenuPreset_Choice_Index != 5)
		{
			M7_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M7_7_ANIStopped(NULL,NULL);
		}
	}

	//==========================================
	//chpark.24.09.26
	if(g_nM7_StartType == M7_MODE_KEY && g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
	{
		if(ituWidgetIsVisible(M7_12_ZoomBTN) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_lock)
			{
				printf("[M7_AniMenuStatus]Error Occur.%d\n",MenuPreset_Choice_Index);
				M7_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_12_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_12_ANI) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_lock)
			{
				printf("[M7_AniMenuStatus]Error Occur.%d\n",MenuPreset_Choice_Index);
				M7_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_12_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_12_Up1) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_lock)
			{
				printf("[M7_AniMenuStatus]Error Occur.%d\n",MenuPreset_Choice_Index);
				M7_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_12_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_12_Up0) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_lock)
			{
				printf("[M7_AniMenuStatus]Error Occur.%d\n",MenuPreset_Choice_Index);
				M7_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_12_ANIStopped(NULL,NULL);
			}
		}
	}
	//==========================================
	if(g_nM7_StartType == M7_MODE_KEY)
	{
		if(ituWidgetIsVisible(M7_13_ZoomBTN) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_password)
			{
				M7_13_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_13_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_13_ANI) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_password)
			{
				M7_13_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_13_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_13_Up1) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_password)
			{
				M7_13_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_13_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_13_Up0) == true)
		{
			if(MenuPreset_Choice_Index != mPreset_password)
			{
				M7_13_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_13_ANIStopped(NULL,NULL);
			}
		}
	}
	//===========================================
	if(g_nM7_StartType == M7_MODE_PUSH) //chpark.24.10.15
	{
		if(CheckM6_9()==1)
		{
			if(ituWidgetIsVisible(M7_9_ZoomBTN) == true)
			{
				if(MenuPreset_Choice_Index != 6)		// mPreset_User
				{
					M7_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_9_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_9_ANI) == true)
			{
				if(MenuPreset_Choice_Index != 6)
				{
					M7_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_9_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_9_Up1) == true)
			{
				if(MenuPreset_Choice_Index != 6)
				{
					M7_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_9_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_9_Up0) == true)
			{
				if(MenuPreset_Choice_Index != 6)
				{
					M7_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_9_ANIStopped(NULL,NULL);
				}
			}
		}
	}

	if(g_nM7_StartType == M7_MODE_PUSH || g_nM7_StartType == M7_MODE_KEY) //chpark.24.10.15
	{
		if((M7_AG_OptFlag == 1) || (M7_AG_OptFlag == 2))
		{
			if(ituWidgetIsVisible(M7_4_ZoomBTN) == true)
			{
				if(MenuPreset_Choice_Index != 7)		// mPreset_Greese
				{
					M7_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_4_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_4_ANI) == true)
			{
				if(MenuPreset_Choice_Index != 7)
				{
					M7_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_4_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_4_Up1) == true)
			{
				if(MenuPreset_Choice_Index != 7)
				{
					M7_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_4_ANIStopped(NULL,NULL);
				}
			}
			if(ituWidgetIsVisible(M7_4_Up0) == true)
			{
				if(MenuPreset_Choice_Index != 7)
				{
					M7_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
					M7_4_ANIStopped(NULL,NULL);
				}
			}
		}
	}

	if(M7_AP_OptFlag == 1)
	{
		if(ituWidgetIsVisible(M7_11_ZoomBTN) == true)
		{
			if(MenuPreset_Choice_Index != 8)		// mPreset_AutoPressure
			{
				M7_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_11_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_11_ANI) == true)
		{
			if(MenuPreset_Choice_Index != 8)
			{
				M7_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_11_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_11_Up1) == true)
		{
			if(MenuPreset_Choice_Index != 8)
			{
				M7_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_11_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(M7_11_Up0) == true)
		{
			if(MenuPreset_Choice_Index != 8)
			{
				M7_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				M7_11_ANIStopped(NULL,NULL);
			}
		}
	}
	
}

