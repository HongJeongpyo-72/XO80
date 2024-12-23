#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"


// CAN_[UI2,0x1850FE8C]1~8 No UserInfo , CAN_[UI3,0x1850FF8C]9~10 No UserInfo
// Byte=b[0::3]PassCode_Digit + b[4]RFID Reg + b[5]BT Reg + b[6::7]Access Level
//UserInfo User_RegInfo[15];			// register user infomation, system date resume(saved data)
static bool m_bSU100_Start_Check_SliderBar = false;
static bool m_bSU100_AUI1_Rcv_Error = false;//chpark.24.02.14

static bool m_bSU100_Button_Down = false;//chpark.24.03.06

//chpark.11.20
//char *g_SU100_MSG_TEXT_DEL[5];		//"Select users to delete."
//char *g_SU100_MSG_TEXT_CF_DEL[5];	//"Are you sure you want to delete the user?"
//char *g_SU100_MSG_TEXT_SELF_DEL[5];	//"Your account will be deleted. OK?"

#define SU100_LIST_START_USER_IDX 2

static ITUWidget* SU100_Slide_CONT;
static ITUWidget* SU100_SlideBar;

//===========================
//Global value 
int g_nSU100_Mode;
int g_nSU200_InitButtonIndex;
int g_nSU100_PreLayer;

uint8_t SU100_UI1_3Byte;			// CAN[UI1] 3byte //chpark.08.22
uint8_t SU100_UI1_4Byte;			// CAN[UI1] 4byte //chpark.08.22
uint8_t SU100_UI2_All_Byte[8];		//chpark.08.22
uint8_t SU100_UI3_All_Byte[8];		//chpark.08.22
uint8_t SU100_UI4_All_Byte[8];		//chpark.08.22
uint8_t SU100_UI5_All_Byte[8];		//chpark.08.22

uint8_t SU100_UM5_All_Byte[8];		//chpark.08.22

//===========================

static UserInfo SU100_List_RegInfo[MAX_SU100_USER_NUM];    // SU100 user info.
static uint8_t SU100_Delete_RES[MAX_SU100_USER_NUM];       // Response User info from Controller.

uint8_t SU100_List_UserCnt = 0;
static bool m_bSU100_List_Init = false;


static uint8_t cUserPage = 0;       // 0:User SelectPage(add/delete), 1:User_Delete Page	

uint8_t g_nSU100_CurUserIdx = 0;			// system data resume(M7_UserIndex),immobilizer checked user index
static UserInfo SU100_CurUserInfo;              // current user

int SU100_SelectUserIdx;
uint8_t g_nSU100_AddUserIdx;
uint8_t g_nSU100_EditUserIdx;

static uint32_t SU100_SecTimer;
static uint32_t SU100_ExitCNT = 0;
static uint32_t SU100_SendCNT = 0;

void SU100_DBG_UserNameCode(uint8_t tUserIdx);
void SU100_ISO8859_to_UTF8(uint8_t *tDest,uint8_t *tSrc);

int  SU100_GetUserCount();
int  SU100_GetAdminCount();

bool SU100_ListBox_Select(int nIndex);
bool SU100_ListBox_H_Select(int nIndex);//chpark.01.19
void SU100_SetUserTitle(uint8_t tUserIdx);

void SU100_InitControl();
void SU100_InitMode(int nMode);

//for jog focus
#define SU100_SEL_A_BTN 	20
#define SU100_SEL_D_BTN 	21
#define SU100_SEL_DEL_BTN 	30 
#define SU100_SEL_YES_BTN 	40 
#define SU100_SEL_NO_BTN 	41 


#define SU100_LANG_ENG 0
#define SU100_LANG_GER 1
#define SU100_LANG_FR  2
#define SU100_LANG_IT  3
#define SU100_LANG_SP  4


#define SU100_CONT		0
#define SU100_BTN		1
#define SU100_SEL	    2

#define SU100_CONT_DELMODE		0
#define SU100_CONT_DEL_MSG		1
#define SU100_CONT_DEL_BTN	    2
#define SU100_CONT_ASK_MSG	    3
#define SU100_CONT_SELF_ASK_MSG	4
#define SU100_CONT_YESNO_BTN	5

#define SU100_CONT_RESULT		5
#define SU100_CONT_ERROR_MSG	6

#define SU100_SUCCESS			0
#define SU100_FAIL				1

#define SU100_RCV_INIT           0
#define SU100_RCV_DEL_USER_CHK   1

//
static ITUWidget* SU100_TRANS_WINDOW; //chpark.24.06.17
//


static ITUWidget* SU100_TOP_ICON[2]; //
static ITUWidget* SU100_TOP_NAME; //

static ITUWidget* SU100_A_MODE[3]; //
static ITUWidget* SU100_A_MODE_ICON[5]; //

static ITUWidget* SU100_D_MODE[3]; //
static ITUWidget* SU100_D_MODE_ICON[5]; //

static ITUWidget* SU100_DELETE_MODE[10]; //Are you sure you want to delete the user?


static ITUWidget* SU100_DEL_SELECT_MSG[5]; //
static ITUWidget* SU100_DEL_BTN[5]; //

static ITUWidget* SU100_DEL_ASK_MSG[5]; //
static ITUWidget* SU100_DEL_SELF_ASK_MSG[5]; //chpark.01.31
static ITUWidget* SU100_YESNO_SEL[2]; //

static ITUWidget* SU100_YES_BTN[5]; //
static ITUWidget* SU100_NO_BTN[5]; //

static ITUWidget* SU100_DEL_ERROR_MSG[5]; //
static ITUWidget* SU100_DEL_RESULT_ICON[5]; //

static ITUWidget* SU100_DEL_SEL;

static ITUWidget* SU100_CONTAIN_DEL_BTN;//chpark.24.01.31
static ITUWidget* SU100_CONTAIN_YESNO_BTN;//chpark.24.01.31

void SU100_Chk_DelResult_FromLocal();
void SU100_Chk_DelResult_FromCT(int nSuc);
void SU100_DeleteUserInfo(int nIndex);

static bool m_bSU100_GoTo_SelectMode = false;
static int m_nGoTo_SelectModeCnt=0;

void SU100_SetListUserInfo();
void SU100_CleanUserInfo();

void SU100_Button_Select(int nIndex);
void SU100_MenuTabToggle();
void SU100_MenuTabEnter(int nIndex);
void SU100_MenuTabESC();

void SU100_MenuTabNext();
void SU100_MenuTabPrev();
void SU100_MenuTabUp();
void SU100_MenuTabDown();
void SU100_MenuTabLeftRight();

bool SU100_Chk_AddUser();//chpark.08.18
int  SU100_Chk_DelUser();


void SU100_Chk_DelUser_FromCT();//chpark.08.22 //check user status from Controller

static bool m_bSU100_MoveLayer = false;

bool m_bSU100_ListTouchDown = false;


static uint8_t SU100_UM5_Rcv_Type=SU100_RCV_INIT;
static bool    m_bSU100_Chk_Success = true;
bool SU100_Send_TX(int nType);

void SU100_Send_Add_Cancel();//chpark.10.30

//chpark.01.18
static ITUComboTable* SU100_cbt = NULL;
static ITUText* SU100_text = NULL;
static ITUContainer* SU100_container = NULL;

static ITUComboTable* SU100_h_cbt = NULL;
static ITUText* SU100_h_text = NULL;
static ITUContainer* SU100_h_container = NULL;

//vmode
 void SU100_Combo_SetSlideEnd_AutoFocus();//chpark.24.06.27
 void SU100_Combo_SetMaxMoveLimit();//chpark.24.05.22
 bool SU100_Combo_PageChanged(ITUWidget* widget, char* param);
 bool SU100_Combo_MouseUp(ITUWidget* widget, char* param);
 bool SU100_Combo_Select(ITUWidget* widget, char* param);
 bool SU100_Combo_Button_Up(ITUWidget* widget, char* param);
//hmode
 bool SU100_Combo_H_PageChanged(ITUWidget* widget, char* param);
 bool SU100_Combo_H_MouseUp(ITUWidget* widget, char* param);
 bool SU100_Combo_H_Select(ITUWidget* widget, char* param);
 bool SU100_Combo_H_Button_Up(ITUWidget* widget, char* param);
 bool SU100_Combo_H_Button_Down(ITUWidget* widget, char* param);

void SU100_Combo_H_SetSlideEnd_AutoFocus();//chpark.24.06.27
void SU100_Combo_H_SetMaxMoveLimit();//chpark.24.05.22
void SU100_Combo_DrawPage(int nIndex);
void SU100_Combo_H_DrawPage(int nIndex);
void SU100_Combo_SetFocus(int nIndex);
void SU100_Combo_H_SetFocus(int nIndex);
void SU100_Combo_DrawItem(int nIndex,bool bShow);//chpark.24.01.19
void SU100_Combo_H_DrawItem(int nIndex,bool bShow);//chpark.24.01.19

void SU100_Combo_GotoPage(int nIndex);    //start at 0 page
void SU100_Combo_H_GotoPage(int nIndex);    //start at 0 page

void SU100_Combo_InitPosition(); //start at 0 index

static bool SU100_b_VMode = false;

void SU100_Set_ScrollBar(int nIndex); //chpark.01.15
void SU100_Set_H_ScrollBar(int nIndex); //chpark.01.15

void SU100_Combo_Check_DragRegion(); //chpark.24.02.20
void SU100_Combo_H_Check_DragRegion(); //chpark.24.02.20

//===========================================================
void SU100_Combo_SetFocus(int nIndex)
{
	int y = ituWidgetGetHeight(SU100_container);
    int dy=0;

    ITUWidget* pPageFirstItem  = SU100_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = SU100_cbt->itemArr[SU100_cbt->selectIndex];
	
	//printf("[SU100_Combo_SetFocus]SU100_cbt->selectIndex:%d , SU100_cbt->lastselectIndex=%d\n",SU100_cbt->selectIndex,SU100_cbt->lastselectIndex); 
    //printf("[SU100_Combo_SetFocus]pPageFirstItem->rect.height:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.height,pPageSelectItem->rect.y); 
    dy = pPageFirstItem->rect.height;

	int nTmpIndex = nIndex;
    //Make sure it's the same page.
    if(SU100_cbt->selectIndex>SU100_cbt->lastselectIndex)
    {
        //when index up
        if(nTmpIndex<5)
        {
            dy = 0;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }

		if(SU100_cbt->selectIndex==0)
		{
			//printf("[SU100_Combo_SetFocus][-1->0]pPageFirstItem->rect.y:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.y,pPageSelectItem->rect.y); 

			//dy =  pPageFirstItem->rect.height;

			int nLast_Diff=0;
            nLast_Diff = pPageFirstItem->rect.y - 0;//pPageSelectItem->rect.y);
			
			ituComboTableMoveXY(SU100_cbt, 0, -nLast_Diff);
			printf("nLast_Diff:%d -nLast_Diff:%d\n",nLast_Diff,-nLast_Diff);
            
		}
        else if( pPageSelectItem->rect.y>((5-1)*160) )//chpark.01.19
        {
			//printf("[SU100_Combo_SetFocus][>4]pPageFirstItem->rect.y:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.y,pPageSelectItem->rect.y); 

            int nLast_Diff=0;
            //nLast_Diff = abs(5*160 - pPageSelectItem->rect.y);
            //if(nLast_Diff<160)
			nLast_Diff = 5*160 - pPageSelectItem->rect.y; //chpark.24.04.03
            if(abs(nLast_Diff)<160)
            {
                ituComboTableMoveXY(SU100_cbt, 0, (-dy)+nLast_Diff);
				//printf("[SU100_Combo_SetFocus][Up](1)nLast_Diff:%d (-dy)+nLast_Diff:%d\n",nLast_Diff,(-dy)+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_cbt, 0, (-dy)+nLast_Diff);
				//printf("[SU100_Combo_SetFocus][Up](2)-dy:%d   nLast_Diff:%d (-dy)+nLast_Diff:%d\n",-dy,nLast_Diff,(-dy)+nLast_Diff);
            }
        }

        ituWidgetUpdate(SU100_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
        //printf("[SU100_Combo_SetFocus][Up] SU100_SelectUserIdx=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",SU100_SelectUserIdx,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
    else
    {
        //when index down
        if(nTmpIndex<5 && pPageSelectItem->rect.y<4*160)
        {
            dy = pPageFirstItem->rect.height;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }
        
        if(pPageSelectItem->rect.y<0 )
        {
            //move to next line(+ 1 line height & diff)
            //If it does not fit the exact line, adjust within the diff( + 1 line & diff).
            int nLast_Diff=0;
            nLast_Diff = abs(160 + pPageSelectItem->rect.y);
            if(nLast_Diff<160)
	        {
                ituComboTableMoveXY(SU100_cbt, 0, dy-nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_cbt, 0, dy);
            }
        }
		else if( pPageSelectItem->rect.y>((5-1)*160) )//chpark.01.19
        {
			//printf("[SU100_Combo_SetFocus][Down][>4]pPageFirstItem->rect.y:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.y,pPageSelectItem->rect.y); 

            int nLast_Diff=0;
            //nLast_Diff = abs(5*160 - pPageSelectItem->rect.y);
            //if(nLast_Diff<160)
			nLast_Diff = 5*160 - pPageSelectItem->rect.y; //chpark.24.04.03
            if(abs(nLast_Diff)<160)
            {
                ituComboTableMoveXY(SU100_cbt, 0, (-dy)+nLast_Diff);
				//printf("[SU100_Combo_SetFocus][Down](3)nLast_Diff:%d (-dy)+nLast_Diff:%d\n",nLast_Diff,(-dy)+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_cbt, 0, (-dy)+nLast_Diff);
				//printf("[SU100_Combo_SetFocus][Down](4)-dy:%d  nLast_Diff:%d  (-dy)+nLast_Diff:%d\n",-dy,nLast_Diff,(-dy)+nLast_Diff);
            }
        }

        ituWidgetUpdate(SU100_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
        //printf("[SU100_Combo_SetFocus][Down] SU100_SelectUserIdx=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",SU100_SelectUserIdx,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
}

void SU100_Combo_H_SetFocus(int nIndex)
{
	int y = ituWidgetGetHeight(SU100_h_container);
    int dy=0;
   
    ITUWidget* pPageFirstItem  = SU100_h_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = SU100_h_cbt->itemArr[SU100_h_cbt->selectIndex];
	
	//printf("[SU100_Combo_H_SetFocus]SU100_h_cbt->selectIndex:%d , SU100_h_cbt->lastselectIndex=%d\n",SU100_h_cbt->selectIndex,SU100_h_cbt->lastselectIndex); 
    //printf("[SU100_Combo_H_SetFocus]pPageFirstItem->rect.height:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.height,pPageSelectItem->rect.y); 
    dy = pPageFirstItem->rect.height;

	int nTmpIndex = nIndex;
    //Make sure it's the same page.
    if(SU100_h_cbt->selectIndex>=SU100_h_cbt->lastselectIndex)//chpark.01.19
    {
        //when index up
        if(nTmpIndex<4)
        {
            dy = 0;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }

		if((SU100_h_cbt->selectIndex==0 && SU100_h_cbt->lastselectIndex==-1) )
		{
			//printf("[SU100_Combo_H_SetFocus][-1->0]pPageFirstItem->rect.height:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.height,pPageSelectItem->rect.y); 
			
			int nLast_Diff=0;
            nLast_Diff = pPageFirstItem->rect.y - 0;//pPageSelectItem->rect.y);
			
			ituComboTableMoveXY(SU100_h_cbt, 0, -nLast_Diff);
			//printf("nLast_Diff:%d -nLast_Diff:%d\n",nLast_Diff,-nLast_Diff);

		}
		else if( pPageSelectItem->rect.y>((3-1)*130) )//chpark.24.02.06
        {
			//printf("[SU100_Combo_H_SetFocus][>4]pPageFirstItem->rect.y:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.y,pPageSelectItem->rect.y); 

            int nLast_Diff=0;
            
			//nLast_Diff = abs(3*130 - pPageSelectItem->rect.y);
            //if(nLast_Diff<130)
			nLast_Diff = 3*130 - pPageSelectItem->rect.y; //chpark.24.04.03
            if(abs(nLast_Diff)<130)
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy+nLast_Diff);
				//printf("nLast_Diff:%d -dy+nLast_Diff:%d\n",nLast_Diff,-dy+nLast_Diff);

            }
            else
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy+nLast_Diff);
				//printf("-dy:%d   nLast_Diff:%d \n",-dy,nLast_Diff);
            }

        }

        ituWidgetUpdate(SU100_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
        //printf("[SU100_Combo_H_SetFocus][Up] SU100_SelectUserIdx=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",SU100_SelectUserIdx,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
    else
    {
        //when index down
		if(nTmpIndex<6 && (pPageSelectItem->rect.y<(3)*130) )//chpark.24.02.06
        {
            dy = pPageFirstItem->rect.height;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }

        if(pPageSelectItem->rect.y<0  )
        {
            //move to next line(+ 1 line height & diff)
            //If it does not fit the exact line, adjust within the diff( + 1 line & diff).
            int nLast_Diff=0;
            //nLast_Diff = abs(180 + pPageSelectItem->rect.y);
            //if(nLast_Diff<180)
			nLast_Diff = abs(130 + pPageSelectItem->rect.y);//chpark.24.02.06
            if(nLast_Diff<130)//chpark.24.02.06
	        {
                ituComboTableMoveXY(SU100_h_cbt, 0, dy-nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, dy);
            }
        }
		else if( pPageSelectItem->rect.y>((3-1)*130) )//chpark.24.02.06
        {
			//printf("[SU100_Combo_H_SetFocus][>4]pPageFirstItem->rect.y:%d , pPageSelectItem->rect.y=%d\n",pPageFirstItem->rect.y,pPageSelectItem->rect.y); 

            int nLast_Diff=0;
            
			//nLast_Diff = abs(3*130 - pPageSelectItem->rect.y);
            //if(nLast_Diff<130)
			nLast_Diff = 3*130 - pPageSelectItem->rect.y; //chpark.24.04.03
            if(abs(nLast_Diff)<130)
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy+nLast_Diff);
				//printf("nLast_Diff:%d -dy+nLast_Diff:%d\n",nLast_Diff,-dy+nLast_Diff);

            }
            else
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy+nLast_Diff);
				//printf("-dy:%d   nLast_Diff:%d \n",-dy,nLast_Diff);
            }

        }

        ituWidgetUpdate(SU100_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);

        //printf("[SU100_Combo_H_SetFocus][Down] SU100_SelectUserIdx=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",SU100_SelectUserIdx,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }

}

bool SU100_Combo_PageChanged(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	int nTouchIndex = atoi(param);
    if (SU100_cbt)
	{
		//int vPage = ituComboTableGetCurrentPageNumber(SU100_cbt);
		//char vStr[10] = "";
		//sprintf(vStr, "%d", vPage);
		//ituTextSetString(text, vStr);
        //printf("[SU100_Combo_PageChanged]PageNum:%d ===== %d\n",vPage,atoi(param),nTouchIndex);
	}
    else
    {
        printf("[SU100_Combo_PageChanged]SU100_cbt is NULL Error====\n");
    }


    if(nTouchIndex==1000 || nTouchIndex==2000) //chpark.24.06.27
    {
		//printf("[SU100_Combo_PageChanged][%d]==\n",nTouchIndex);
		//=================
		//adjust button
        ITUWidget* pPageFirstItem = SU100_cbt->itemArr[0];
        //chpark.24.04.03
        ITUWidget* pPageSelectItem = SU100_cbt->itemArr[SU100_cbt->selectIndex];

        int  first_item_pos	= pPageFirstItem->rect.y;//first item postion
        int  re_reitem_pos 	= round(first_item_pos/160)*160 - first_item_pos;
        
        //printf("[SU100_Combo_PageChanged]==== first_item_pos:%d re_reitem_pos:(1):%d\n",first_item_pos,re_reitem_pos);
       
        ituComboTableMoveXY(SU100_cbt, 0, re_reitem_pos);//move to zero position
        
        //
        int  dy =  pPageFirstItem->rect.height;

        if( pPageSelectItem->rect.y>((5-1)*160) )
        {
            int nLast_Diff=0;
            nLast_Diff = (5*160 - pPageSelectItem->rect.y);
            //printf("[SU100_Combo_PageChanged]==== pPageSelectItem->rect.y:%d nLast_Diff:%d -dy+nLast_Diff:(2):%d\n",pPageSelectItem->rect.y,nLast_Diff,-dy+nLast_Diff);
            if(abs(nLast_Diff)<160)
            {
                ituComboTableMoveXY(SU100_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_cbt, 0, -dy);
            }
        }
		//=================
		SU100_Combo_SetMaxMoveLimit();
		//=================
		
		//chpark.24.06.27
		if(nTouchIndex==2000)
		{
			SU100_Combo_SetSlideEnd_AutoFocus();
		}
		//=================

		//=================
		SU100_Set_ScrollBar(0); //make slide bitmap to moves
		//=================
		
        //printf("[SU100_Combo_PageChanged]\n");
    }
	//else if(nTouchIndex==2000)
	//{
	//	printf("[SU100_Combo_PageChanged][%d]==\n",nTouchIndex);
	//}

	m_bSU100_Button_Down = false;
	
	return true;
}

bool SU100_Combo_H_PageChanged(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;

	int nTouchIndex = atoi(param);
    if (SU100_h_cbt)
	{
		int vPage = ituComboTableGetCurrentPageNumber(SU100_h_cbt);
		char vStr[10] = "";
		
        //printf("[SU100_Combo_H_PageChanged]PageNum:%d ===== %d\n",vPage,atoi(param),nTouchIndex);
	}
    else
    {
        //printf("[SU100_Combo_H_PageChanged]SU100_cbt is NULL Error====\n");
    }
	//chpark.24.02.19
	
	//chpark.24.05.20
    if(nTouchIndex==1000 || nTouchIndex==2000)//chpark.24.06.27
    {
		//printf("[SU100_Combo_H_PageChanged]\n");

		//=================
		//adjust button
        ITUWidget* pPageFirstItem = SU100_h_cbt->itemArr[0];
        //chpark.24.04.03
        ITUWidget* pPageSelectItem = SU100_h_cbt->itemArr[SU100_h_cbt->selectIndex];

        int  first_item_pos	= pPageFirstItem->rect.y;//first item postion
        int  re_reitem_pos 	= round(first_item_pos/130)*130 - first_item_pos;
        
        //printf("[SU100_Combo_H_PageChanged]==== first_item_pos:%d re_reitem_pos:(1):%d\n",first_item_pos,re_reitem_pos);
       
        ituComboTableMoveXY(SU100_h_cbt, 0, re_reitem_pos);//move to zero position
        
        //
        int  dy =  pPageFirstItem->rect.height;

        if( pPageSelectItem->rect.y>((3-1)*130) )
        {
            int nLast_Diff=0;
            nLast_Diff = (3*130 - pPageSelectItem->rect.y);
            //printf("[SU100_Combo_H_PageChanged]==== pPageSelectItem->rect.y:%d nLast_Diff:%d -dy+nLast_Diff:(2):%d\n",pPageSelectItem->rect.y,nLast_Diff,-dy+nLast_Diff);
            if(abs(nLast_Diff)<130)
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(SU100_h_cbt, 0, -dy);
            }
        }
		//=================
		SU100_Combo_H_SetMaxMoveLimit();

		//chpark.24.06.27
		if(nTouchIndex==2000)
		{
			SU100_Combo_H_SetSlideEnd_AutoFocus();
		}

		SU100_Set_H_ScrollBar(0); //make slide bitmap to moves
		//=================
	}

	m_bSU100_Button_Down = false;

	return true;
}

void SU100_Combo_SetMaxMoveLimit()//chpark.24.05.22
{
	//Max Limit
    int	nITU_Combo_nTop =  (-1)*(SU100_cbt->itemArr[0]->rect.height)*(SU100_cbt->itemcount-5);
    if(SU100_cbt->itemcount>5 && SU100_cbt->itemArr[0]->rect.y<=nITU_Combo_nTop)
    {
        int nLast_Diff = nITU_Combo_nTop - (SU100_cbt->itemArr[0]->rect.y);
        ituComboTableMoveXY(SU100_cbt, 0, abs(nLast_Diff));//move to re position
        //rintf("[SU100_Combo_SetMaxMoveLimit]====%d SU100_cbt->itemArr[0]->rect.y=%d\n",abs(nLast_Diff),SU100_cbt->itemArr[0]->rect.y);
    }
}

void SU100_Combo_H_SetMaxMoveLimit()//chpark.24.05.22
{
	//Max Limit
    int	nITU_Combo_nTop =  (-1)*(SU100_h_cbt->itemArr[0]->rect.height)*(SU100_h_cbt->itemcount-3);
    if(SU100_h_cbt->itemcount>3 && SU100_h_cbt->itemArr[0]->rect.y<=nITU_Combo_nTop)
    {
        int nLast_Diff = nITU_Combo_nTop - (SU100_h_cbt->itemArr[0]->rect.y);
        ituComboTableMoveXY(SU100_h_cbt, 0, abs(nLast_Diff));//move to re position
        //printf("[SU100_Combo_H_SetMaxMoveLimit]====%d SU100_h_cbt->itemArr[0]->rect.y=%d\n",abs(nLast_Diff),SU100_h_cbt->itemArr[0]->rect.y);
    }
}

void SU100_Combo_SetSlideEnd_AutoFocus()//chpark.24.06.27
{
	ITUWidget* pPageFirstItem  = SU100_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = SU100_cbt->itemArr[SU100_cbt->selectIndex];

	if( pPageSelectItem->rect.y>=(5*160) )
	{
		//down sliding-> bottom auto focus
		int nIndex = abs(pPageFirstItem->rect.y)/160+5-1;
		if(nIndex>=SU100_cbt->itemcount)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = SU100_cbt->itemcount-1;
		}
		else
		{
			//printf("[Slide->SetFocus Again]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);				
		}
					
		SU100_ListBox_Select(nIndex);
	}
	else if(pPageSelectItem->rect.y<0)
	{
		//up sliding-> top auto focus
		int nIndex;
		
		nIndex= abs(pPageFirstItem->rect.y)/160;
		
		if(nIndex>=SU100_List_UserCnt)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = SU100_List_UserCnt-1;
		}
		else
		{
			//printf("[Slide->SetFocus Again][2]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);
						
		}

		SU100_ListBox_Select(nIndex);
	}
	else
	{
		//printf("[Slide->SetFocus Again]pPageSelectItem->rect.y=%d\n",pPageSelectItem->rect.y);
	}
}

void SU100_Combo_H_SetSlideEnd_AutoFocus()//chpark.24.06.27
{
	ITUWidget* pPageFirstItem  = SU100_h_cbt->itemArr[0];

    ITUWidget* pPageSelectItem = SU100_h_cbt->itemArr[SU100_h_cbt->selectIndex];

	if(pPageSelectItem->rect.y>=(3*130))
	{
		//down sliding-> bottom auto focus
		int nIndex;
		if(SU100_SelectUserIdx%2==0)
		{
			//even : left
			nIndex= abs(pPageFirstItem->rect.y)/130*2+6-2;
		}
		else
		{
			//odd : right
			nIndex= abs(pPageFirstItem->rect.y)/130*2+6-1;
		}

		if(nIndex>=SU100_List_UserCnt)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = SU100_List_UserCnt-1;
		}
		else
		{
			//printf("[Slide->H_SetFocus Again][1]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);
						
		}

		SU100_ListBox_H_Select(nIndex);
	}
	else if(pPageSelectItem->rect.y<0)
	{
		//up sliding-> top auto focus
		int nIndex;
		if(SU100_SelectUserIdx%2==0)
		{
			//even : left
			nIndex= abs(pPageFirstItem->rect.y)/130*2;
		}
		else
		{
			//odd : right
			nIndex= abs(pPageFirstItem->rect.y)/130*2+1;
		}

		if(nIndex>=SU100_List_UserCnt)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = SU100_List_UserCnt-1;
		}
		else
		{
			//printf("[Slide->H_SetFocus Again][2]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);
						
		}

		SU100_ListBox_H_Select(nIndex);
	}
	else
	{
		//printf("[Slide->H_SetFocus Again]pPageSelectItem->rect.y=%d\n",pPageSelectItem->rect.y);
	}
}

//chpark.01.03
//By modifying the sdk,
//Mouse up actually functions as a mouse down.
bool SU100_Combo_MouseUp(ITUWidget* widget, char* param)
{
	//printf("[SU100_Combo_MouseUp] called=%d====\n",SU100_cbt->selectIndex);	

	SU100_ExitCNT = 0;
	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		printf("Message Showing...\r\n");
		return false;
	}

	int nUserCnt = SU100_GetUserCount();

	if(SU100_cbt->selectIndex>=nUserCnt)
	{
		SU100_cbt->selectIndex = SU100_cbt->lastselectIndex;
		SU100_cbt->lastselectIndex = -1;
		printf("[SU100_Combo_MouseUp][ERROR] select error=%d====\n",SU100_cbt->selectIndex);	
		return false;
	}
	
	SU100_SelectUserIdx = SU100_cbt->selectIndex;

	if(g_nSU100_Mode==SU100_Select_Mode || g_nSU100_Mode==SU100_Delete_Mode1)
	{
		if(SU100_SelectUserIdx>=0 && SU100_SelectUserIdx<SU100_SEL_A_BTN)
		{
			ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
		}
		
		SU100_Combo_DrawPage(0);
	}
	else
	{
		printf("[SU100_Combo_MouseUp][ERROR] g_nSU100_Mode error=%d====\n",g_nSU100_Mode);	
	}

	m_bSU100_Button_Down = true;//chpark.24.03.06

	return true;
}

bool SU100_Combo_Select(ITUWidget* widget, char* param)
{
	int nTouchIndex,nSelectIndex;
    
    nTouchIndex = atoi(param);//
    nSelectIndex = nTouchIndex/10;

    if(nTouchIndex%10==0)
    {
        //printf("[SU100_Combo_Select] Left Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }
    else
    {
        //printf("[SU100_Combo_Select] Right Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }
    
	return true;
}

bool SU100_Combo_Button_Up(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	m_bSU100_Button_Down = false;//chpark.24.03.06

	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		printf("Message Showing...\r\n");
		return false;
	}

	int nButtonIndex;
    
    nButtonIndex = atoi(param);

    //printf("[SU100_Combo_Button_Up]nButtonIndex=%d selectIndex:%d SU100_SelectUserIdx:%d====\n",nButtonIndex,SU100_cbt->selectIndex,SU100_SelectUserIdx);

	int nUserCnt = SU100_GetUserCount();

	if(SU100_cbt->selectIndex>=nUserCnt)
	{
		SU100_cbt->selectIndex = SU100_cbt->lastselectIndex;
		SU100_cbt->lastselectIndex = -1;
		return false;
	}

	if(g_nSU100_Mode==SU100_Select_Mode || g_nSU100_Mode==SU100_Delete_Mode1)
	{
		SU100_MenuTabEnter(SU100_SelectUserIdx);  
	}
	else
	{
		printf("[SU100_Combo_Button_Up][ERROR] g_nSU100_Mode error=%d====\n",g_nSU100_Mode);	
	}

	return true;
}

//hmode

//chpark.01.03
//By modifying the sdk,
//Mouse up actually functions as a mouse down.
bool SU100_Combo_H_MouseUp(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	return true;
}

bool SU100_Combo_H_Select(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	int nTouchIndex,nSelectIndex;
    
    nTouchIndex = atoi(param);//
    nSelectIndex = nTouchIndex/10;

    if(nTouchIndex%10==0)
    {
        //printf("[SU100_Combo_H_Select] Left Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }
    else
    {
        //printf("[SU100_Combo_H_Select] Right Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }

	return true;
}


bool SU100_Combo_H_Button_Down(ITUWidget* widget, char* param)
{
	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		printf("Message Showing...\r\n");
		return false;
	}

	int nButtonIndex;
    
    nButtonIndex = atoi(param);
   
    //Selectindex is already processed within the combo table.
    int nTmpSelecIdx;
	SU100_List_UserCnt = SU100_GetUserCount();
        
    if(nButtonIndex==0)
    {
		//left  
		nTmpSelecIdx = SU100_h_cbt->selectIndex*2;
		if(nTmpSelecIdx>=SU100_List_UserCnt)
		{
			return false;
		}
    }
    else
    {
		//right
		nTmpSelecIdx = SU100_h_cbt->selectIndex*2+1;
		if(nTmpSelecIdx>=SU100_List_UserCnt)
		{
			return false;
		}
    }
	//printf("\n\n[SU100_Combo_H_Button_Down]nButtonIndex=%d selectIndex:%d SU100_h_cbt->selectIndex:%d====\n",nButtonIndex,nTmpSelecIdx,SU100_h_cbt->selectIndex);

    //save in global vari
	SU100_SelectUserIdx = nTmpSelecIdx;

	if(SU100_SelectUserIdx>=0 && SU100_SelectUserIdx<SU100_SEL_A_BTN)
	{
		ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
		ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
	}

	//draw all item
	SU100_Combo_H_DrawPage(0);
	//chpark.24.02.19
	SU100_Set_H_ScrollBar(SU100_SelectUserIdx); //make slide bitmap to move

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

	m_bSU100_Button_Down = true;//chpark.24.03.06

	return true;
}

bool SU100_Combo_H_Button_Up(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	m_bSU100_Button_Down = false;//chpark.24.03.06

	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		printf("Message Showing...\r\n");
		return false;
	}

	int nButtonIndex;
    nButtonIndex = atoi(param);

	//printf("[SU100_Combo_H_Button_Up]nButtonIndex=%d selectIndex:%d SU100_SelectUserIdx:%d====\n",nButtonIndex,SU100_cbt->selectIndex,SU100_SelectUserIdx);

	int nUserCnt = SU100_GetUserCount();

	if(nButtonIndex==0)
    {
		if( (SU100_h_cbt->selectIndex*2)>=nUserCnt)//chpark.24.02.19
		{
			SU100_cbt->selectIndex = SU100_cbt->lastselectIndex;
			SU100_cbt->lastselectIndex = -1;
			//printf("[SU100_Combo_H_Button_Up][ERROR] select error=%d====\n",SU100_cbt->selectIndex*2);	
			return false;
		}
	}
	else
	{	
		if( (SU100_h_cbt->selectIndex*2+1)>=nUserCnt)//chpark.24.02.19
		{
			SU100_cbt->selectIndex = SU100_cbt->lastselectIndex;
			SU100_cbt->lastselectIndex = -1;

			//printf("[SU100_Combo_H_Button_Up][ERROR] select error=%d====\n",SU100_cbt->selectIndex*2+1);	
			return false;
		}
	}

	if(g_nSU100_Mode==SU100_Select_Mode || g_nSU100_Mode==SU100_Delete_Mode1)
	{
		SU100_MenuTabEnter(SU100_SelectUserIdx);  
		
	}
	else
	{
		printf("[SU100_Combo_H_Button_Up][ERROR] g_nSU100_Mode=%d====\n",g_nSU100_Mode);	
	}

	return true;
}


void SU100_Combo_DrawPage(int nIndex)
{
	int nComboItemCount = SU100_cbt->itemcount;

	if(SU100_List_UserCnt>nComboItemCount)
	{
		//printf("[SU100_Combo_DrawPage][+][1]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
		int nAddCount = SU100_List_UserCnt - nComboItemCount;
        ituComboTableAdd(SU100_cbt, nAddCount);
		nComboItemCount = SU100_cbt->itemcount;
		//printf("[SU100_Combo_DrawPage][+][2]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
	}
	else if(SU100_List_UserCnt<nComboItemCount)
	{
		//printf("[SU100_Combo_DrawPage][-][1]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
		
		for(int i=nComboItemCount-1; i>=SU100_List_UserCnt ;i--)
		{
			ituComboTableDelItem(SU100_cbt, i);
		}
		nComboItemCount = SU100_cbt->itemcount;
		//printf("[SU100_Combo_DrawPage][-][2]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
		//printf("[SU100_Combo_DrawPage][-][3]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
	}
	else
	{
		//printf("[SU100_Combo_DrawPage][==][1]SU100_List_UserCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboItemCount);
	}

	ituWidgetUpdate(SU100_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);

	//for (int i=0; i<MAX_SU100_USER_NUM; i++)
	for (int i=0; i<SU100_List_UserCnt; i++)//chpark.24.03.15
	{
        SU100_Combo_DrawItem(i,true);
	}

	SU100_Set_ScrollBar(0);//chpark.24.03.06
}

void SU100_Combo_H_DrawPage(int nIndex)
{
	int nComboItemCount = SU100_h_cbt->itemcount;
	int nComboListCnt = (SU100_List_UserCnt+1)/2;
	if(nComboListCnt>nComboItemCount)
	{
		//printf("[SU100_Combo_H_DrawPage][+][1]SU100_List_UserCnt=%d nComboListCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboListCnt,nComboItemCount);
		SU100_h_cbt->itemcount = nComboListCnt;

		int nAddCount = nComboListCnt - nComboItemCount;
        ituComboTableAdd(SU100_h_cbt, nAddCount);

		nComboItemCount = SU100_h_cbt->itemcount;
		//printf("[SU100_Combo_H_DrawPage][+][2]SU100_List_UserCnt=%d nComboListCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboListCnt,nComboItemCount);
	}
	else if(nComboListCnt<nComboItemCount)
	{
		//printf("[SU100_Combo_H_DrawPage][-][1]SU100_List_UserCnt=%d nComboListCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboListCnt,nComboItemCount);
	
		for(int i=nComboItemCount-1; i>=nComboListCnt ;i--)
		{
			ituComboTableDelItem(SU100_h_cbt, i);
		}
		nComboItemCount = SU100_h_cbt->itemcount;
		//printf("[SU100_Combo_H_DrawPage][-][2]SU100_List_UserCnt=%d nComboListCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboListCnt,nComboItemCount);
	}
	else
	{
		//printf("[SU100_Combo_H_DrawPage][==][1]SU100_List_UserCnt=%d nComboListCnt=%d nComboItemCount=%d\n",SU100_List_UserCnt,nComboListCnt,nComboItemCount);
	}

	ituWidgetUpdate(SU100_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);

	for (int i=0; i<SU100_List_UserCnt+1; i++)//chpark.24.03.15	
	{
        SU100_Combo_H_DrawItem(i,true);
	}

	SU100_Set_H_ScrollBar(0);//chpark.24.03.06
}


void SU100_Combo_DrawItem(int nIndex,bool bShow)
{
	if(nIndex<0)return;
    int i = nIndex;
	int nTmpUserIndex = nIndex;
       
    ITUWidget* tmpObj = NULL;
	
	tmpObj = ituComboTableGetItemChild(SU100_cbt, i, ITU_TEXT, 0);

	if(tmpObj)
	{
		//black
		ituWidgetSetColor(tmpObj,255,0,0,0);
		//Draw Text.
		uint8_t tDestName[50];
		memset(tDestName,0,50);
		SU100_ISO8859_to_UTF8(tDestName,SU100_List_RegInfo[nTmpUserIndex].UserName);

		//printf("[Num=%d]Name:%s SU100_List_RegInfo[nTmpUserIndex].RegFlag=%d\n",nTmpUserIndex,tDestName,SU100_List_RegInfo[nTmpUserIndex].RegFlag);
			
		//(2)BK Color
		ITUWidget* SU100_ItemIcon[9]; //chpark.24.02.13
		SU100_ItemIcon[0] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 0);
		SU100_ItemIcon[1] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 1);
		SU100_ItemIcon[2] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 2);
		SU100_ItemIcon[3] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 3);
		SU100_ItemIcon[4] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 4);
		SU100_ItemIcon[5] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 5);
		SU100_ItemIcon[6] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 6);
		SU100_ItemIcon[7] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 7);
		SU100_ItemIcon[8] = ituComboTableGetItemChild(SU100_cbt, i, ITU_ICON, 8);

		for(int a=0;a<9;a++)//chpark.24.02.13
		{
			ituWidgetSetVisible(SU100_ItemIcon[a], false);
		}
		//chpark.24.02.19
		int nUserCnt = SU100_GetUserCount();
		if(nTmpUserIndex>=nUserCnt)
		{
			ituTextSetString(tmpObj, "");
			return;
		}

		//(1) Text
		ituTextSetString(tmpObj, tDestName);

		//(2) BK Image
		if(g_nSU100_Mode==SU100_Select_Mode)
		{
			if(nTmpUserIndex==SU100_SelectUserIdx)//blue box
			{
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					//admin + blue box
					ituWidgetSetVisible(SU100_ItemIcon[0], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					//standard + blue box 
					ituWidgetSetVisible(SU100_ItemIcon[1], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_R || SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_A)
				{
					//chpark.24.02.13
					//no icon + blue box 
					ituWidgetSetVisible(SU100_ItemIcon[8], true);
				}
			}
			else //no box
			{           
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					//admin + none
					ituWidgetSetVisible(SU100_ItemIcon[2], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					//standard + none
					ituWidgetSetVisible(SU100_ItemIcon[3], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_R || SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_A)
				{
					//chpark.24.02.13
					//no icon + none
				}
			}
		}
		else
		{
			//Delete Mode
			if(nTmpUserIndex==SU100_SelectUserIdx)//blue box
			{
				//Delete mode + select
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white admin + blue box
						ituWidgetSetVisible(SU100_ItemIcon[0], true);
					}
					else
					{
						//gray admin + blue box
						ituWidgetSetVisible(SU100_ItemIcon[6], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white standard + blue box
						ituWidgetSetVisible(SU100_ItemIcon[1], true);
					}
					else
					{
						//gray standard + blue box
						ituWidgetSetVisible(SU100_ItemIcon[7], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_DEL)
				{
					//Deleted User
					// none
					ituTextSetString(tmpObj, "");
				}
			}
			else
			{
				//Delete mode + none
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white admin + none
						ituWidgetSetVisible(SU100_ItemIcon[2], true);
					}
					else
					{
						//gray admin + none
						ituWidgetSetVisible(SU100_ItemIcon[4], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white standard + none
						ituWidgetSetVisible(SU100_ItemIcon[3], true);
					}
					else
					{
						//gray standard + none
						ituWidgetSetVisible(SU100_ItemIcon[5], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_DEL)
				{
					//Deleted User
					// none
					ituTextSetString(tmpObj, "");
				}
			}
		}
	}
	else
	{
		//printf("[SU100_Combo_DrawItem]:Error:tmpObj=NULL [i] = %d \n",i);
	}
}

void SU100_Combo_H_DrawItem(int nIndex,bool bShow)
{
	if(nIndex<0)return;
    int i = nIndex/2;
	int nTmpUserIndex = nIndex;
	
	bool bRight_Combo = nIndex%2;
    bool bLeft_Combo  = !bRight_Combo;

    ITUWidget* tmpObj = NULL;

	if(bLeft_Combo)
	{
		//left
		tmpObj = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_TEXT, 0);
	}
	else
	{	//right
		tmpObj = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_TEXT, 1);
	}

	if(tmpObj)
	{
		//black
		ituWidgetSetColor(tmpObj,255,0,0,0);
		//Draw Text.
		uint8_t tDestName[50];
		memset(tDestName,0,50);
		SU100_ISO8859_to_UTF8(tDestName,SU100_List_RegInfo[nTmpUserIndex].UserName);

		//(2)BK Color
		ITUWidget* SU100_ItemIcon[10]; //chpark.24.02.13
		
		if(bLeft_Combo)
		{
			SU100_ItemIcon[0] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 0);
			SU100_ItemIcon[1] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 1);
			SU100_ItemIcon[2] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 2);
			SU100_ItemIcon[3] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 3);
			SU100_ItemIcon[4] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 4);
			SU100_ItemIcon[5] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 5);
			SU100_ItemIcon[6] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 6);
			SU100_ItemIcon[7] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 7);
			SU100_ItemIcon[8] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 8);
		}
		else
		{
			SU100_ItemIcon[0] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 9);
			SU100_ItemIcon[1] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 10);
			SU100_ItemIcon[2] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 11);
			SU100_ItemIcon[3] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 12);
			SU100_ItemIcon[4] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 13);
			SU100_ItemIcon[5] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 14);
			SU100_ItemIcon[6] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 15);
			SU100_ItemIcon[7] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 16);
			SU100_ItemIcon[8] = ituComboTableGetItemChild(SU100_h_cbt, i, ITU_ICON, 17);
		}

		for(int a=0;a<9;a++)
		{
			ituWidgetSetVisible(SU100_ItemIcon[a], false);
		}

		//chpark.24.02.19
		int nUserCnt = SU100_GetUserCount();
		if(nTmpUserIndex>=nUserCnt)
		{
			ituTextSetString(tmpObj, "");
			return;
		}

		//(1) Text
		ituTextSetString(tmpObj, tDestName);

		//(2) BK Image
		if(g_nSU100_Mode==SU100_Select_Mode)
		{
			if(nTmpUserIndex==SU100_SelectUserIdx)//blue box
			{
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					//admin + blue box
					ituWidgetSetVisible(SU100_ItemIcon[0], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					//standard + blue box 
					ituWidgetSetVisible(SU100_ItemIcon[1], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_R || SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_A)
				{
					//chpark.24.02.13
					//no icon + blue box 
					ituWidgetSetVisible(SU100_ItemIcon[8], true);
				}
			}
			else //no box
			{           
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					//admin + none
					ituWidgetSetVisible(SU100_ItemIcon[2], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					//standard + none
					ituWidgetSetVisible(SU100_ItemIcon[3], true);
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_R || SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_N_A)
				{
					//chpark.24.02.13
					//no icon + none					
				}
			}
		}
		else
		{
			//Delete Mode
			if(nTmpUserIndex==SU100_SelectUserIdx)//blue box
			{
				//Delete mode + select
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white admin + blue box
						ituWidgetSetVisible(SU100_ItemIcon[0], true);
					}
					else
					{
						//gray admin + blue box
						ituWidgetSetVisible(SU100_ItemIcon[6], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white standard + blue box
						ituWidgetSetVisible(SU100_ItemIcon[1], true);
					}
					else
					{
						//gray standard + blue box
						ituWidgetSetVisible(SU100_ItemIcon[7], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_DEL)
				{
					//Deleted User
					// none
					ituTextSetString(tmpObj, "");
					//ituScrollTextSetString(itemIcon, "8");//chpark.01.18
				}
			}
			else
			{
				//Delete mode + none
				if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_AD)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white admin + none
						ituWidgetSetVisible(SU100_ItemIcon[2], true);
					}
					else
					{
						//gray admin + none
						ituWidgetSetVisible(SU100_ItemIcon[4], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_ST)
				{
					if(SU100_List_RegInfo[nTmpUserIndex].bDelSelect)
					{
						//white standard + none
						ituWidgetSetVisible(SU100_ItemIcon[3], true);
					}
					else
					{
						//gray standard + none
						ituWidgetSetVisible(SU100_ItemIcon[5], true);
					}
				}
				else if(SU100_List_RegInfo[nTmpUserIndex].RegFlag==SU100_USER_DEL)
				{
					//Deleted User
					// none
					ituTextSetString(tmpObj, "");
				}
			}
		}		
	}
}

void SU100_Combo_GotoPage(int nIndex)    //start at 0 page: (0,1,2)
{
	//move to page
    ITUWidget* pPageFirstItem = SU100_cbt->itemArr[0];
    int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
    int  first_item_height   = pPageFirstItem->rect.height;
    
	//chpark.24.02.19
	int  nNew_Pos = 0;
	//(1)move to zero position
	ituComboTableMoveXY(SU100_cbt, 0, -first_item_pos);

	//(2)move to selected page postion
	if(nIndex<=0)//chpark.24.05.21
	{
		 //no move
	}
	else if(nIndex==1)
	{
		ituComboTableMoveXY(SU100_cbt, 0, -first_item_height*4);//move to selected page position
	}
	else if(nIndex==2)
	{
		ituComboTableMoveXY(SU100_cbt, 0, -first_item_height*7);//move to selected page position
	}
	else
	{
		printf("[SU100_Combo_GotoPage] Error:%d\n",nIndex);
	}
}

void SU100_Combo_H_GotoPage(int nIndex)    //start at 0 page //start at 0 page: (0,1,2)
{
	//move to page
    ITUWidget* pPageFirstItem = SU100_h_cbt->itemArr[0];
    int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
    int  first_item_height   = pPageFirstItem->rect.height;
	
	//chpark.24.02.19
	int  nNew_Pos = 0;
	//(1)move to zero position
	ituComboTableMoveXY(SU100_h_cbt, 0, -first_item_pos);

	//(2)move to selected page postion
	if(nIndex<=0)//chpark.24.05.21
	{
		 //no move
	}
	else if(nIndex==1)
	{
		ituComboTableMoveXY(SU100_h_cbt, 0, -first_item_height*3);//move to selected page position
	}
	else if(nIndex==2)
	{
		ituComboTableMoveXY(SU100_h_cbt, 0, -first_item_height*6);//move to selected page position
	}
	else
	{
		printf("[SU100_Combo_H_GotoPage] Error:%d\n",nIndex);
	}
}


void SU100_Combo_InitPosition() //start at 0 index
{
	if(SU100_b_VMode)
	{
		ITUWidget* pPageFirstItem  = SU100_cbt->itemArr[0];

		int nLast_Diff=0;
		nLast_Diff = pPageFirstItem->rect.y - 0;//pPageSelectItem->rect.y);
				
		ituComboTableMoveXY(SU100_cbt, 0, -nLast_Diff);
		//printf("[SU100_Combo_InitPosition]nLast_Diff:%d -nLast_Diff:%d\n",nLast_Diff,-nLast_Diff);
	}
	else
	{
		ITUWidget* pPageFirstItem  = SU100_h_cbt->itemArr[0];

		int nLast_Diff=0;
		nLast_Diff = pPageFirstItem->rect.y - 0;//pPageSelectItem->rect.y);
				
		ituComboTableMoveXY(SU100_h_cbt, 0, -nLast_Diff);
		//printf("[SU100_Combo_InitPosition]nLast_Diff:%d -nLast_Diff:%d\n",nLast_Diff,-nLast_Diff);
	}
	
}
//===========================================================


bool SU100_Enter(ITUWidget* widget, char* param)
{
	//======================
	//for test
	#ifdef TEST_USER_SET_ICON		//chpark.24.05.20
		  gUI_Server_Mode = true;
	#endif
	//======================
	
	m_bSU100_Button_Down = false;

	gMB_Language = Lang_Flag[M7_UserIndex];//chpark.24.02.19
	m_bSU100_Start_Check_SliderBar = false;

	//printf("\n[SU100_Enter][START]==[CurUser:%d] [time:%d] [gMB_Language:%d]\n",M7_UserIndex,g_UI1_ChkTime,gMB_Language);
	
	m_bSU100_MoveLayer 			= false;
	m_bSU100_GoTo_SelectMode 	= false;
	m_nGoTo_SelectModeCnt=0;

    SU100_SecTimer 	= gMainTimerCnt_10m;
	SU100_ExitCNT 	= 0;
	SU100_SendCNT	= 0;//1sec
	g_nSU100_PreLayer  = M7_SU100_LAYER;
	SU100_UM5_Rcv_Type = SU100_RCV_INIT;
	m_bSU100_List_Init = true;//chpark.10.30

	if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        SU100_b_VMode = false;
    }
    else
    {   
        SU100_b_VMode = true;
    }

	//User data is updated every 1 second.
	if(g_UI1_ChkTime>0)
	{
		//g_UI1_ChkTime = 0;//
		//show O,X

		//chpark.24.02.15
		//User_RegInfo[0].RegFlag = SU100_USER_DEFAULT;//(SU100_UI2_All_Byte[1] & 0x0F);
		//User_RegInfo[1].RegFlag = SU100_USER_DEFAULT;//(SU100_UI2_All_Byte[3] & 0x0F);
		printf("\n[SU100][Received User Info From CT]==[CurUser:%d] [time:%d]\n",M7_UserIndex,g_UI1_ChkTime);
		g_nSU100_CurUserIdx = M7_UserIndex;
	}
	else
	{
		printf("Local Mode===\n");
		//chpark.24.02.15
		//User_RegInfo[0].RegFlag = SU100_USER_DEFAULT;//(SU100_UI2_All_Byte[1] & 0x0F);
		//User_RegInfo[1].RegFlag = SU100_USER_DEFAULT;//(SU100_UI2_All_Byte[3] & 0x0F);
		
		//chpark.12.12
		for(int i=SU100_LIST_START_USER_IDX;i<MAX_SU100_USER_NUM;i++)
		{
			if(User_RegInfo[i].RegFlag>=2 &&  User_RegInfo[i].RegFlag<=9)
			{
				User_RegInfo[i].bRegister = true;
			}
			else
			{
				User_RegInfo[i].bRegister = false;
				User_RegInfo[i].RegFlag   = SU100_USER_N_R;
			}
		}
		g_nSU100_CurUserIdx = 0;
	}
    //==============================================
	SU100_InitControl();
	//==============================================
	//delete (incomplete  add user info & deleted user info)
	SU100_CleanUserInfo();
	//test user info setting.
	SU100_SetListUserInfo();

    SU100_List_UserCnt = SU100_GetUserCount();//used Jog Control
    //==============================================
 	SU100_SelectUserIdx		=	0;
	g_nSU100_AddUserIdx		=	-1;
	g_nSU100_EditUserIdx	=	0;
    //==============================================
	SU100_SelectUserIdx = -1;//none selected
	//==============================================
	//Start in Select Mode
	SU100_InitMode(SU100_Select_Mode);

	//
	//SU100_Combo_InitPosition();//set combo to 0 position //chpark.24.04.02
	//
	if(SU100_b_VMode)
	{
		ituWidgetSetVisible(SU100_cbt,true);
		ituWidgetSetVisible(SU100_h_cbt,false);
		ituComboTable_SetPageItemCount(5);//chpark.24.05.20
		
		//printf("\n[SU100_Enter][SU100_List_UserCnt=%d SU100_cbt->itemcount=%d][1]===\n",SU100_List_UserCnt,SU100_cbt->itemcount);
		
		int nComboListCnt = SU100_cbt->itemcount;

		if(SU100_List_UserCnt>nComboListCnt)
        {
			//add
			int nAddCount = SU100_List_UserCnt - nComboListCnt;
			ituComboTableAdd(SU100_cbt, nAddCount);
			//printf("[SU100_Enter] [#%d] item add. remain:%d\n",nAddCount,SU100_cbt->itemcount);
        }
        else if(SU100_List_UserCnt<nComboListCnt)
        {
            //delete
            if (nComboListCnt>0)
            {
                for(int i=nComboListCnt-1; i>=SU100_List_UserCnt ;i--)
                {
                    ituComboTableDelItem(SU100_cbt, i);
                    printf("[SU100_Enter]%d item delete. remain:%d\n",i,SU100_cbt->itemcount);
                }       
            }
        }
        else
        {
            //none
        }
		
		printf("[SU100_Enter][SU100_List_UserCnt=%d SU100_cbt->itemcount=%d][2]===\n\n",SU100_List_UserCnt,SU100_cbt->itemcount);
		
		ituWidgetUpdate(SU100_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
		
		SU100_Combo_GotoPage(0);
		//
	}
	else
	{
		ituWidgetSetVisible(SU100_cbt,false);
		ituWidgetSetVisible(SU100_h_cbt,true);
		ituComboTable_SetPageItemCount(3);//chpark.24.05.20

		int nComboListCnt = SU100_h_cbt->itemcount;

		if(SU100_List_UserCnt>nComboListCnt)
        {
			//add
			int nAddCount = SU100_List_UserCnt - nComboListCnt;
			ituComboTableAdd(SU100_h_cbt, nAddCount);
			printf("[SU100_Enter] [#%d] item add. remain:%d\n",nAddCount,SU100_h_cbt->itemcount);
        }
        else if(SU100_List_UserCnt<nComboListCnt)
        {
            //delete
            if (nComboListCnt>0)
            {
                for(int i=nComboListCnt-1; i>=SU100_List_UserCnt ;i--)
                {
                    ituComboTableDelItem(SU100_h_cbt, i);
                    printf("[SU100_Enter]%d item delete. remain:%d\n",i,SU100_h_cbt->itemcount);
                }       
            }
        }
        else
        {
            //none
        }

		ituWidgetUpdate(SU100_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
		
		SU100_Combo_H_GotoPage(0);
		//
	}

	//Draw page item
	SU100_Button_Select(SU100_SEL_A_BTN);
	m_bSU100_ListTouchDown = false;
	//==============================================

	//
	SU100_Send_TX(0);
	//

	m_bSU100_AUI1_Rcv_Error = false;		//chpark.24.02.14
	UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

	//printf("\n[SU100_Enter][START]==[CurUser:%d] [time:%d]\n",M7_UserIndex,g_UI1_ChkTime);

	m_bSU100_Start_Check_SliderBar = true;

    return true;
}

void SU100_SetListUserInfo()
{
	int nIndex = 0;
	
	//chpark.12.13->chpark.24.04.11->move to MenuTop_Layer
	//Set default name  if 'FF' is full of user name.
	
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		char chTmpUserName[50];
		memset(chTmpUserName, 0, sizeof(chTmpUserName));
		memcpy(chTmpUserName, &User_RegInfo[i].UserName[0], sizeof(chTmpUserName));
		int nSizeOfName = strlen(chTmpUserName);
		
		if(SU100_Is_Empty_InFlash(User_RegInfo[i].UserName[0]) || nSizeOfName==0)
		{
			if(i==0)//chpark.24.04.11
			{
				strncpy(&User_RegInfo[i].UserName[0],"Super+ Admin",MAX_SU100_NAME_CNT);
			}
			//else if(i==1 && User_RegInfo[i].RegFlag==SU100_USER_SAD)
			else if(i==1)
			{
				strncpy(&User_RegInfo[i].UserName[0],"Super Admin",MAX_SU100_NAME_CNT);
			}
			else
			{
				char chTmpUserName[50];
				memset(chTmpUserName, 0, sizeof(chTmpUserName));
				memset (&User_RegInfo[i].UserName[0],0,sizeof(User_RegInfo[i].UserName));

				sprintf(chTmpUserName, "default user%d",User_RegInfo[i].RegKeyNum-1);//user idx:2->default user1//chpark.12.19
				strncpy(&User_RegInfo[i].UserName[0],chTmpUserName,MAX_SU100_NAME_CNT);

				if(User_RegInfo[i].AccessAUX == SU600_AUX_TYPE_NONE)//chpark.24.03.27
				{
					User_RegInfo[i].AccessAUX 		= SU600_AUX_TYPE_NO;
					User_RegInfo[i].nTmp_AccessAUX	= SU600_AUX_TYPE_NO;
				}
			
				//printf("[FF Error]No Name in User Info[%d]. Set Default Name\n",i);
			}
		}
		else
		{
			//exception check//chpark.24.04.03
			if(User_RegInfo[i].RegFlag==SU100_USER_AD || User_RegInfo[i].RegFlag==SU100_USER_ST)
			{
				if(User_RegInfo[i].AccessAUX == SU600_AUX_TYPE_NONE)//chpark.24.03.27
				{
					User_RegInfo[i].AccessAUX 		= SU600_AUX_TYPE_NO;
					User_RegInfo[i].nTmp_AccessAUX	= SU600_AUX_TYPE_NO;
				}
			}
			//printf("%d user name = %s\n",i,User_RegInfo[i].UserName[0]);
		}

		//if(i==g_nSU100_CurUserIdx)
		//{
		//	SU100_SetUserTitle(g_nSU100_CurUserIdx);//chpark.24.03.06
			//printf("[%d user][SU100_SetListUserInfo][SU100_SetUserTitle]=%s\n",g_nSU100_CurUserIdx,User_RegInfo[i].UserName[0]);
		//}
	}
	
	

	//reset
	//user1, user2 should not count
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		memset(&SU100_List_RegInfo[i],0,sizeof(SU100_List_RegInfo[i]));	
		SU100_List_RegInfo[i].RegFlag = SU100_USER_INIT;//chpark.24.02.19
	}
	
	SU100_SetUserTitle(g_nSU100_CurUserIdx);//chpark.24.03.06
	
	//reload list data
	for(int i=SU100_LIST_START_USER_IDX;i<MAX_SU100_USER_NUM;i++)
	{
		if(User_RegInfo[i].RegFlag==SU100_USER_AD || User_RegInfo[i].RegFlag==SU100_USER_ST)
		{
			memcpy(&SU100_List_RegInfo[nIndex], &User_RegInfo[i], sizeof(SU100_List_RegInfo[i]));

			//name check.
			//If there is no name, set a default name.
			char chTmpUserName[50];
			memset(chTmpUserName, 0, sizeof(chTmpUserName));
			memcpy(chTmpUserName, &SU100_List_RegInfo[nIndex].UserName[0], sizeof(chTmpUserName));
			int nSizeOfName = strlen(chTmpUserName);

			if(nSizeOfName==0)
			{
				//chpark.12.13
				//sprintf(chTmpUserName, "default user%d",i+1);
				//strncpy(&SU100_List_RegInfo[nIndex].UserName[0],chTmpUserName,30);
				sprintf(chTmpUserName, "default user%d",SU100_List_RegInfo[nIndex].RegKeyNum-1);
				strncpy(&SU100_List_RegInfo[nIndex].UserName[0],chTmpUserName,MAX_SU100_NAME_CNT);
				//printf("[Null Error]No Name in User Info. Set Default Name==\n");
			}
			//
			//printf("List=%d  RegKeyNum=%d Name=%s RegFlag=%d AUX=%d passcode=%s NOD=%d\n",nIndex,SU100_List_RegInfo[nIndex].RegKeyNum, chTmpUserName,SU100_List_RegInfo[nIndex].RegFlag,SU100_List_RegInfo[nIndex].AccessAUX,SU100_List_RegInfo[nIndex].UserPassCode,SU100_List_RegInfo[nIndex].PassNOD);
			//
			nIndex++;
		}	
	}

	//printf("Total Registered User Num=%d ==\n",nIndex);
}

void SU100_CleanUserInfo()
{
	int nIndex=0;
	
	for(int i=SU100_LIST_START_USER_IDX;i<MAX_SU100_USER_NUM;i++)
	{
		//chpark.11.02
		if(User_RegInfo[i].bRegister==false)
		{
			SU100_DeleteUserInfo(i);
		}
		else if(User_RegInfo[i].RegFlag==SU100_USER_DEL ||  User_RegInfo[i].bAddProcessing )
		{
			SU100_DeleteUserInfo(i);
		}
	}
}

void SU100_DeleteUserInfo(int nIndex)
{
	//printf("Delete user info==[#%d user]\n",nIndex);
	memset(&User_RegInfo[nIndex],0,sizeof(User_RegInfo[nIndex]));

	//printf("==================\n");
	//printf("%d %d %d  %d\n",User_RegInfo[nIndex].RegFlag,User_RegInfo[nIndex].PassDigit,User_RegInfo[nIndex].RFStatus,User_RegInfo[nIndex].MacStatus);
	//printf("%s \n",User_RegInfo[nIndex].UserName);
	//printf("%s \n",User_RegInfo[nIndex].UserPassCode);
	//printf("%s \n",User_RegInfo[nIndex].UserMacID);
	//printf("==================\n");

	//key save
	User_RegInfo[nIndex].RegKeyNum 			= nIndex;
}


void SU100_InitControl()
{
	int tIndex=0;//
//Text setting
	/*    
    g_SU100_MSG_TEXT_DEL[SU100_LANG_ENG] = "Select users to delete.";
    g_SU100_MSG_TEXT_DEL[SU100_LANG_GER] = "Select users to delete.";
    g_SU100_MSG_TEXT_DEL[SU100_LANG_FR]  = "Select users to delete.";
    g_SU100_MSG_TEXT_DEL[SU100_LANG_IT]  = "Select users to delete.";
    g_SU100_MSG_TEXT_DEL[SU100_LANG_SP]  = "Select users to delete.";

	g_SU100_MSG_TEXT_CF_DEL[SU100_LANG_ENG] = "Are you sure you want to delete the user?";
    g_SU100_MSG_TEXT_CF_DEL[SU100_LANG_GER] = "Are you sure you want to delete the user?";
    g_SU100_MSG_TEXT_CF_DEL[SU100_LANG_FR]  = "Are you sure you want to delete the user?";
    g_SU100_MSG_TEXT_CF_DEL[SU100_LANG_IT]  = "Are you sure you want to delete the user?";
    g_SU100_MSG_TEXT_CF_DEL[SU100_LANG_SP]  = "Are you sure you want to delete the user?";

	g_SU100_MSG_TEXT_SELF_DEL[SU100_LANG_ENG] = "Your account will be deleted. OK?";
    g_SU100_MSG_TEXT_SELF_DEL[SU100_LANG_GER] = "Your account will be deleted. OK?";
    g_SU100_MSG_TEXT_SELF_DEL[SU100_LANG_FR]  = "Your account will be deleted. OK?";
    g_SU100_MSG_TEXT_SELF_DEL[SU100_LANG_IT]  = "Your account will be deleted. OK?";
    g_SU100_MSG_TEXT_SELF_DEL[SU100_LANG_SP]  = "Your account will be deleted. OK?";
	*/
//==============================================================
	//TRANS WINDOW
	SetWidget(SU100_TRANS_WINDOW,"SU100_BTN_TRANS");//chpark.24.06.17
	//Title
	SetWidget(SU100_TOP_ICON[SU100_ICON_AD],"SU100_T_Icon_Admin");
	SetWidget(SU100_TOP_ICON[SU100_ICON_ST],"SU100_T_Icon_Std");
	SetWidget(SU100_TOP_NAME,"SU100_T_Name");

	//Select Mode UI
	SetWidget(SU100_A_MODE[SU100_CONT],"SU100_CONT_A_MODE");
	SetWidget(SU100_A_MODE[SU100_BTN], "SU100_A_MODE_BTN");
	SetWidget(SU100_A_MODE[SU100_SEL], "SU100_A_MODE_SEL");

	SetWidget(SU100_A_MODE_ICON[SU100_LANG_ENG],"SU100_ADD_ENG");
	SetWidget(SU100_A_MODE_ICON[SU100_LANG_GER],"SU100_ADD_GER");
	SetWidget(SU100_A_MODE_ICON[SU100_LANG_FR], "SU100_ADD_FR");
	SetWidget(SU100_A_MODE_ICON[SU100_LANG_IT], "SU100_ADD_IT");
	SetWidget(SU100_A_MODE_ICON[SU100_LANG_SP], "SU100_ADD_SP");

	for(int i=0; i<5 ;i++)
	{
		ituWidgetSetVisible(SU100_A_MODE_ICON[i],false);
	}

	ituWidgetSetVisible(SU100_A_MODE_ICON[Lang_Flag[M7_UserIndex]],true);

	SetWidget(SU100_D_MODE[SU100_CONT],"SU100_CONT_D_MODE");
	SetWidget(SU100_D_MODE[SU100_BTN], "SU100_D_MODE_BTN");
	SetWidget(SU100_D_MODE[SU100_SEL], "SU100_D_MODE_SEL");

	SetWidget(SU100_D_MODE_ICON[SU100_LANG_ENG],"SU100_DEL_ENG");
	SetWidget(SU100_D_MODE_ICON[SU100_LANG_GER],"SU100_DEL_GER");
	SetWidget(SU100_D_MODE_ICON[SU100_LANG_FR],"SU100_DEL_FR");
	SetWidget(SU100_D_MODE_ICON[SU100_LANG_IT],"SU100_DEL_IT");
	SetWidget(SU100_D_MODE_ICON[SU100_LANG_SP],"SU100_DEL_SP");

	for(int i=0; i<5 ;i++)
	{
		ituWidgetSetVisible(SU100_D_MODE_ICON[i],false);
	}

	ituWidgetSetVisible(SU100_D_MODE_ICON[Lang_Flag[M7_UserIndex]],true);
//==============================================================
	//Delete Mode UI
	SetWidget(SU100_DELETE_MODE[SU100_CONT_DELMODE],"SU100_CONT_DELMODE");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_DEL_MSG],"SU100_CONT_DEL_MSG");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_DEL_BTN],"SU100_CONT_DEL_BTN");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_ASK_MSG],"SU100_CONT_ASK_MSG");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_SELF_ASK_MSG],"SU100_CONT_SELF_ASK_MSG");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_YESNO_BTN],"SU100_CONT_YESNO_BTN");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_RESULT],"SU100_CONT_RESULT");
	SetWidget(SU100_DELETE_MODE[SU100_CONT_ERROR_MSG],"SU100_CONT_ERROR_MSG");

//========================
	//Select the users to delete.
	SetWidget(SU100_DEL_SELECT_MSG[0],"SU100_DEL_MSG_ENG");
	SetWidget(SU100_DEL_SELECT_MSG[1],"SU100_DEL_MSG_GER");
	SetWidget(SU100_DEL_SELECT_MSG[2],"SU100_DEL_MSG_FR");
	SetWidget(SU100_DEL_SELECT_MSG[3],"SU100_DEL_MSG_IT");
	SetWidget(SU100_DEL_SELECT_MSG[4],"SU100_DEL_MSG_SP");
	//Delete
	SetWidget(SU100_DEL_BTN[0],"SU100_DEL_BTN_ENG");
	SetWidget(SU100_DEL_BTN[1],"SU100_DEL_BTN_GER");
	SetWidget(SU100_DEL_BTN[2],"SU100_DEL_BTN_FR");
	SetWidget(SU100_DEL_BTN[3],"SU100_DEL_BTN_IT");
	SetWidget(SU100_DEL_BTN[4],"SU100_DEL_BTN_SP");
	//Delete Rect
	SetWidget(SU100_DEL_SEL,"SU100_DEL_SEL");
//========================
	//Do you really want to create it?
	SetWidget(SU100_DEL_ASK_MSG[0],"SU100_ASK_MSG_ENG");
	SetWidget(SU100_DEL_ASK_MSG[1],"SU100_ASK_MSG_GER");
	SetWidget(SU100_DEL_ASK_MSG[2],"SU100_ASK_MSG_FR");
	SetWidget(SU100_DEL_ASK_MSG[3],"SU100_ASK_MSG_IT");
	SetWidget(SU100_DEL_ASK_MSG[4],"SU100_ASK_MSG_SP");

	//Your account will be deleted. OK?
	SetWidget(SU100_DEL_SELF_ASK_MSG[0],"SU100_SELF_ASK_MSG_ENG");
	SetWidget(SU100_DEL_SELF_ASK_MSG[1],"SU100_SELF_ASK_MSG_GER");
	SetWidget(SU100_DEL_SELF_ASK_MSG[2],"SU100_SELF_ASK_MSG_FR");
	SetWidget(SU100_DEL_SELF_ASK_MSG[3],"SU100_SELF_ASK_MSG_IT");
	SetWidget(SU100_DEL_SELF_ASK_MSG[4],"SU100_SELF_ASK_MSG_SP");
	
	//Yes NO
	SetWidget(SU100_YESNO_SEL[0],"SU100_ASK_YES_SEL");
	SetWidget(SU100_YESNO_SEL[1],"SU100_ASK_NO_SEL");

	SetWidget(SU100_YES_BTN[0],"SU100_ASK_YES_ENG");
	SetWidget(SU100_YES_BTN[1],"SU100_ASK_YES_GER");
	SetWidget(SU100_YES_BTN[2],"SU100_ASK_YES_FR");
	SetWidget(SU100_YES_BTN[3],"SU100_ASK_YES_IT");
	SetWidget(SU100_YES_BTN[4],"SU100_ASK_YES_SP");

	SetWidget(SU100_NO_BTN[0],"SU100_ASK_NO_ENG");
	SetWidget(SU100_NO_BTN[1],"SU100_ASK_NO_GER");
	SetWidget(SU100_NO_BTN[2],"SU100_ASK_NO_FR");
	SetWidget(SU100_NO_BTN[3],"SU100_ASK_NO_IT");
	SetWidget(SU100_NO_BTN[4],"SU100_ASK_NO_SP");

	//========================
	SetWidget(SU100_DEL_ERROR_MSG[0],"SU100_ERROR_MSG_ENG");
	SetWidget(SU100_DEL_ERROR_MSG[1],"SU100_ERROR_MSG_GER");
	SetWidget(SU100_DEL_ERROR_MSG[2],"SU100_ERROR_MSG_FR");
	SetWidget(SU100_DEL_ERROR_MSG[3],"SU100_ERROR_MSG_IT");
	SetWidget(SU100_DEL_ERROR_MSG[4],"SU100_ERROR_MSG_SP");

	SetWidget(SU100_DEL_RESULT_ICON[SU100_SUCCESS],"SU100_OK");
	SetWidget(SU100_DEL_RESULT_ICON[SU100_FAIL],"SU100_NO");

	SetWidget(SU100_Slide_CONT,"SU100_Slide_CONT");
	SetWidget(SU100_SlideBar,"SU100_BAR");

	//chpark.01.18
	SetWidget(SU100_cbt,"SU100_Combo");
	SetWidget(SU100_h_cbt,"SU100_Combo_H");

	SetWidget(SU100_container,"SU100_Combo_Cont");
	SetWidget(SU100_h_container,"SU100_Combo_H_Cont");

	SetWidget(SU100_CONTAIN_YESNO_BTN,"SU100_CONT_YESNO_BTN");
	SetWidget(SU100_CONTAIN_DEL_BTN,"SU100_CONT_DEL_BTN");
}

void SU100_InitMode(int nMode)
{
	//mode save
	g_nSU100_Mode = nMode;

	switch(nMode)
	{
		case SU100_Select_Mode:
		    
			for(int i=0;i<3;i++)
			{
				ituWidgetSetVisible(SU100_A_MODE[i],true);
				ituWidgetSetVisible(SU100_D_MODE[i],true);
			}

			for(int i=0;i<5;i++)
			{
				ituWidgetSetVisible(SU100_A_MODE_ICON[i],false);
				ituWidgetSetVisible(SU100_D_MODE_ICON[i],false);
			}

			ituWidgetSetVisible(SU100_A_MODE_ICON[gMB_Language],true);
			ituWidgetSetVisible(SU100_D_MODE_ICON[gMB_Language],true);

			
			ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);

			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],false);
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT], false);

			ituWidgetSetVisible(SU100_TRANS_WINDOW, false); //chpark.24.06.17
			break;

		case SU100_Add_Mode:
			
			break;

		case SU100_Delete_Mode1:
			//select
			ituWidgetSetVisible(SU100_A_MODE[SU100_CONT],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_CONT],false);
			//delete1
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],true);
			
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DEL_MSG],true);//Select users to delete.
			
			ituWidgetSetVisible(SU100_CONTAIN_DEL_BTN, true);//chpark.24.02.02
			//delete2
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ASK_MSG],false);//Are you sure you want to delete the user?
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_SELF_ASK_MSG],false);//Your account will be delete. OK?//chpark.24.01.31
			
			ituWidgetSetVisible(SU100_CONTAIN_YESNO_BTN, false);//chpark.24.02.02
			//delete3
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT], false);//It is possible that there is no Admin at all.
	
			//==========================
			for(int i=0;i<5;i++)
			{
				ituWidgetSetVisible(SU100_DEL_SELECT_MSG[i],false);
				ituWidgetSetVisible(SU100_DEL_BTN[i],false);
			}
			
			ituWidgetSetVisible(SU100_DEL_SELECT_MSG[gMB_Language],true);	//chpark.11.20
			
			if(SU100_Chk_DelUser()>=1)
			{
				ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DEL_BTN],true);
				ituWidgetSetVisible(SU100_DEL_BTN[gMB_Language],true);
			}
			else
			{
				ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DEL_BTN],true);
				ituWidgetSetVisible(SU100_DEL_BTN[gMB_Language],false);	
			}

			ituWidgetSetVisible(SU100_TRANS_WINDOW, false); //chpark.24.06.17
			//printf("Set [SU100_Delete_Mode1] mode======%d\n",gMB_Language);
			//==========================
			break;	

		case SU100_Delete_Mode2:
		case SU100_Delete_Mode3:
			//select
			ituWidgetSetVisible(SU100_A_MODE[SU100_CONT],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_CONT],false);
			//delete1
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],true);

			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DEL_MSG],false);
	
			ituWidgetSetVisible(SU100_CONTAIN_DEL_BTN, false);//chpark.24.02.02
			//delete2	
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ASK_MSG],true);
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_SELF_ASK_MSG],true);//Your account will be delete. OK?//chpark.24.01.31

			ituWidgetSetVisible(SU100_CONTAIN_YESNO_BTN, true);//chpark.24.02.02

			//==========================
			for(int i=0;i<5;i++)
			{
				ituWidgetSetVisible(SU100_DEL_ASK_MSG[i],false);
				ituWidgetSetVisible(SU100_DEL_SELF_ASK_MSG[i],false);//chpark.24.01.31
				
				ituWidgetSetVisible(SU100_YES_BTN[i],false);
				ituWidgetSetVisible(SU100_NO_BTN[i],false);
			}
		
			if(nMode==SU100_Delete_Mode2)
			{
				ituWidgetSetVisible(SU100_DEL_ASK_MSG[Lang_Flag[M7_UserIndex]],true);
			}
			else if(nMode==SU100_Delete_Mode3)
			{
				ituWidgetSetVisible(SU100_DEL_SELF_ASK_MSG[Lang_Flag[M7_UserIndex]],true);
			}


			ituWidgetSetVisible(SU100_YES_BTN[gMB_Language],true);
			ituWidgetSetVisible(SU100_NO_BTN[gMB_Language],true);
			//==========================

			//delete3
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT], false);
			//printf("Set [SU100_Delete_Mode1] mode======\n");

			ituWidgetSetVisible(SU100_TRANS_WINDOW, true); //chpark.24.06.17
			break;	
		
		case SU100_Delete_Result_Success:
			//select
			ituWidgetSetVisible(SU100_A_MODE[SU100_CONT],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_CONT],false);
			//delete1,delete2	
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],false);
			//delete3
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT],true);
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ERROR_MSG],false);

			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_SUCCESS],true);
			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_FAIL],false);

			ituWidgetSetVisible(SU100_TRANS_WINDOW, true); //chpark.24.06.17
			break;	

		case SU100_Delete_Result_Fail:
			//select
			ituWidgetSetVisible(SU100_A_MODE[SU100_CONT],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_CONT],false);
			//delete1,delete2	
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],false);
			//delete3
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT],true);
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ERROR_MSG],true);

			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_SUCCESS],false);
			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_FAIL],true);

			for(int i=0;i<5;i++)
			{
				ituWidgetSetVisible(SU100_DEL_ERROR_MSG[i],false);
			}

			ituWidgetSetVisible(SU100_TRANS_WINDOW, true); //chpark.24.06.17
			break;	
		
		case SU100_Delete_Result_Fail_NoAdmin: //chpark.24.0318
			//select
			ituWidgetSetVisible(SU100_A_MODE[SU100_CONT],false);
			ituWidgetSetVisible(SU100_D_MODE[SU100_CONT],false);
			//delete1,delete2	
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_DELMODE],false);
			//delete3
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT],true);
			ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ERROR_MSG],true);

			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_SUCCESS],false);
			ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_FAIL],true);

			for(int i=0;i<5;i++)
			{
				ituWidgetSetVisible(SU100_DEL_ERROR_MSG[i],false);
			}

			ituWidgetSetVisible(SU100_DEL_ERROR_MSG[gMB_Language],true);
			break;	
			
		default:
			break;

	}
}


int SU100_GetUserCount()
{
    int nUserCount=0;
    // 0;Not Register 10:Detete User 5:Admin 9:Standard
    for(int i=0;i<MAX_SU100_USER_NUM-2;i++)
	{		    
		if(SU100_List_RegInfo[i].bRegister)
		{
			if(SU100_List_RegInfo[i].RegFlag==SU100_USER_AD || SU100_List_RegInfo[i].RegFlag==SU100_USER_ST)
			{
				nUserCount++;
			}	

			if(SU100_List_RegInfo[i].RegFlag==SU100_USER_N_R || SU100_List_RegInfo[i].RegFlag==SU100_USER_N_A)
			{
				nUserCount++;
			}
		}
    }

    return nUserCount;
}

int SU100_GetAdminCount()
{
    int nUserCount=0;
    // 0;Not Register 10:Detete User 5:Admin 9:Standard
    for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{		    
		if(SU100_List_RegInfo[i].RegFlag==SU100_USER_AD)
        {
            nUserCount++;
        }	
    }

	for(int i=0;i<2;i++)
	{
		if(User_RegInfo[i].RegFlag==SU100_USER_AD)
        {
            nUserCount++;
        }	
	}

    return nUserCount;
}


//==========================================================
void SU100_Button_Select(int nIndex)
{
	//#define SU100_SEL_A_BTN 	20
	//#define SU100_SEL_D_BTN 	21
	//#define SU100_SEL_DEL_BTN 	30 
	//#define SU100_SEL_YES_BTN 	40 
	//#define SU100_SEL_NO_BTN 	41

	switch(g_nSU100_Mode)
	{
		case SU100_Select_Mode:
		{
			if(nIndex==SU100_SEL_A_BTN)
			{
				ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],true);
				ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
				if(SU100_b_VMode)
					SU100_ListBox_Select(-1);
				else
					SU100_ListBox_H_Select(-1);//chpark.01.19
			}
			else if(nIndex==SU100_SEL_D_BTN)
			{
				ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
				ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],true);
				if(SU100_b_VMode)
					SU100_ListBox_Select(-1);
				else
					SU100_ListBox_H_Select(-1);//chpark.01.19
			}
			else
			{
				ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
				ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);

				if(SU100_b_VMode)
					SU100_ListBox_Select(nIndex);
				else
					SU100_ListBox_H_Select(nIndex);//chpark.01.19
			}
		}
		break;

		case SU100_Delete_Mode1:
		{
			if(nIndex==SU100_SEL_DEL_BTN)
			{
				ituWidgetSetVisible(SU100_DEL_SEL,true);
				if(SU100_b_VMode)
					SU100_ListBox_Select(-1);
				else 
					SU100_ListBox_H_Select(-1);//chpark.01.19
			}
			else
			{
				ituWidgetSetVisible(SU100_DEL_SEL,false);
				
				if(SU100_b_VMode)
					SU100_ListBox_Select(nIndex);
				else 
					SU100_ListBox_H_Select(nIndex);//chpark.01.19
			}
		}
		break;

		case SU100_Delete_Mode2:
		case SU100_Delete_Mode3:
		{
			if(nIndex==SU100_SEL_YES_BTN)
			{
				ituWidgetSetVisible(SU100_YESNO_SEL[0],true);
				ituWidgetSetVisible(SU100_YESNO_SEL[1],false);
			}
			else if(nIndex==SU100_SEL_NO_BTN)
			{
				ituWidgetSetVisible(SU100_YESNO_SEL[0],false);
				ituWidgetSetVisible(SU100_YESNO_SEL[1],true);
			}

			if(SU100_b_VMode)
				SU100_ListBox_Select(-1);
			else
				SU100_ListBox_H_Select(-1);//chpark.01.19
		}
		break;
	}

	SU100_SelectUserIdx = nIndex;

}

void SU100_MenuTabNext()
{
	int8_t nTmpBtnIndex = SU100_SelectUserIdx;
	if(g_nSU100_Mode==SU100_Select_Mode )
	{
		SU100_List_UserCnt = SU100_GetUserCount();
		nTmpBtnIndex++;

		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);

		}
		else if(nTmpBtnIndex==SU100_List_UserCnt)
		{
			//List->A button
			nTmpBtnIndex = SU100_SEL_A_BTN;
			SU100_Button_Select(nTmpBtnIndex);	//chpark.01.23	
		}
		else if(nTmpBtnIndex==SU100_SEL_D_BTN)
		{
			//A button->D button
			nTmpBtnIndex=SU100_SEL_D_BTN;
			SU100_Button_Select(nTmpBtnIndex);	//chpark.01.23
		}
		else if(nTmpBtnIndex>=0 && nTmpBtnIndex>SU100_SEL_D_BTN)
		{
			if(SU100_List_UserCnt==0)
			{
				nTmpBtnIndex = SU100_SEL_A_BTN;
				SU100_Button_Select(nTmpBtnIndex);//chpark.01.23
			}
			else
			{
				//D button->List
				SU100_cbt->lastselectIndex = -1;//chpark.01.23
				SU100_h_cbt->lastselectIndex = -1;//chpark.01.23
				SU100_Button_Select(0);	
			}
		}
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode1)
	{
		SU100_List_UserCnt = SU100_GetUserCount();
		nTmpBtnIndex++;
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//SU100_cbt->lastselectIndex = SU100_SelectUserIdx;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);

		}
		else if(nTmpBtnIndex==SU100_List_UserCnt)
		{
			if( SU100_Chk_DelUser()>=1)
			{
				nTmpBtnIndex = SU100_SEL_DEL_BTN;
			}
			else
			{
				//error
				SU100_cbt->lastselectIndex = -1;//chpark.01.23
				SU100_h_cbt->lastselectIndex = -1;//chpark.01.23

				nTmpBtnIndex = 0;
			}

			SU100_Button_Select(nTmpBtnIndex);		
		}
		else if(nTmpBtnIndex>SU100_SEL_DEL_BTN)
		{
			SU100_cbt->lastselectIndex = -1;//chpark.01.23
			SU100_h_cbt->lastselectIndex = -1;//chpark.01.23

			SU100_Button_Select(0);	
		}

	}
	//else if(g_nSU100_Mode==SU100_Delete_Mode2)
	else if(g_nSU100_Mode==SU100_Delete_Mode2 || g_nSU100_Mode==SU100_Delete_Mode3)//chpark.24.04.17
	{
		if(nTmpBtnIndex==SU100_SEL_YES_BTN)
		{
			SU100_SelectUserIdx = SU100_SEL_NO_BTN;
			SU100_Button_Select(SU100_SelectUserIdx);
		}
		else 
		{
			SU100_SelectUserIdx = SU100_SEL_YES_BTN;
			SU100_Button_Select(SU100_SelectUserIdx);	
		}
	}
}

void SU100_MenuTabPrev()
{
	int8_t nTmpBtnIndex = SU100_SelectUserIdx;

	if(g_nSU100_Mode==SU100_Select_Mode )
	{
		SU100_List_UserCnt = SU100_GetUserCount();
		nTmpBtnIndex--;

		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex<0)
		{
			//List->D button
			nTmpBtnIndex=SU100_SEL_D_BTN;
			SU100_Button_Select(nTmpBtnIndex);	
				
		}
		else if(nTmpBtnIndex==SU100_SEL_A_BTN)
		{
			//D button->A button
			nTmpBtnIndex = SU100_SEL_A_BTN;
			SU100_Button_Select(nTmpBtnIndex);	
		}
		else if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_SEL_A_BTN)
		{
			if(SU100_List_UserCnt==0)
			{
				nTmpBtnIndex = SU100_SEL_D_BTN;
				SU100_Button_Select(nTmpBtnIndex);
			}
			else
			{
				//A Button->List
				nTmpBtnIndex=SU100_List_UserCnt-1;//chpark.01.15
				SU100_Button_Select(nTmpBtnIndex);	
			}

		}
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode1)
	{
		SU100_List_UserCnt = SU100_GetUserCount();
		nTmpBtnIndex--;
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex<0)
		{
			if( SU100_Chk_DelUser()>=1)
			{
				nTmpBtnIndex = SU100_SEL_DEL_BTN;
				SU100_Button_Select(nTmpBtnIndex);//chpark.01.23
			}
			else
			{
				nTmpBtnIndex = SU100_List_UserCnt-1;//chpark.01.23
				SU100_Button_Select(nTmpBtnIndex);	
			}
		}
		else if(nTmpBtnIndex>=0 && nTmpBtnIndex<=SU100_SEL_DEL_BTN)
		{
			nTmpBtnIndex=SU100_List_UserCnt-1;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);	
		}
	}
	//else if(g_nSU100_Mode==SU100_Delete_Mode2)
	else if(g_nSU100_Mode==SU100_Delete_Mode2 || g_nSU100_Mode==SU100_Delete_Mode3) //24.04.17
	{
		if(nTmpBtnIndex==SU100_SEL_YES_BTN)
		{
			nTmpBtnIndex = SU100_SEL_NO_BTN;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);
		}
		else 
		{
			nTmpBtnIndex = SU100_SEL_YES_BTN;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);	
		}
	}
}


void SU100_MenuTabDown()
{
	int8_t nTmpBtnIndex = SU100_SelectUserIdx;
	SU100_List_UserCnt = SU100_GetUserCount();
	//(1)In H-mode, In the case of the bottom of the right column of the list, 
	//Do not move any further.
	if(!SU100_b_VMode && nTmpBtnIndex%2==1)
	{
		if(nTmpBtnIndex>=1 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			nTmpBtnIndex = nTmpBtnIndex+2;
			if(nTmpBtnIndex<SU100_List_UserCnt)
				SU100_Button_Select(nTmpBtnIndex);

			return;
		}
	}

	//(2) Calculate the index of the list.
	if(SU100_b_VMode)
	{
		nTmpBtnIndex = nTmpBtnIndex+1;
	}	
	else 
	{
		nTmpBtnIndex = nTmpBtnIndex+2;
	}
	

	if(g_nSU100_Mode==SU100_Select_Mode )
	{
		//(3)In H-mode, In the case of the bottom of the left column of the list, move to 'Add user' button.
		//(4)In V-mode, In the case of the bottom of the list, move to 'Add user' button.
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex>=SU100_List_UserCnt && nTmpBtnIndex<SU100_SEL_A_BTN)
		{
			//List->A button
			nTmpBtnIndex = SU100_SEL_A_BTN;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);		
		}
		else if(nTmpBtnIndex==SU100_SEL_D_BTN)
		{
			//D Button->List
			SU100_Button_Select(0);	
		}
		else if(nTmpBtnIndex>=0 && nTmpBtnIndex>SU100_SEL_A_BTN)
		{
			//A Button->List
			SU100_Button_Select(0);	
		}
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode1)
	{
		//(3)In H-mode, In the case of the bottom of the left column of the list, move to 'Delete' button.
		//(4)In V-mode, In the case of the bottom of the list, move to 'Delete' button.
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);
		}
		//else if(nTmpBtnIndex==SU100_List_UserCnt)
		else if(nTmpBtnIndex>=SU100_List_UserCnt && nTmpBtnIndex< SU100_SEL_DEL_BTN)//chpark.24.04.17
		{
			//List->DELETE button
			if( SU100_Chk_DelUser()>=1)
			{
				nTmpBtnIndex = SU100_SEL_DEL_BTN;
			}
			else
			{
				nTmpBtnIndex = 0;
			}		

			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex>=SU100_SEL_DEL_BTN)
		{
			//DELETE button->List
			SU100_cbt->lastselectIndex = -1;//chpark.01.23
			SU100_h_cbt->lastselectIndex = -1;//chpark.01.23
			SU100_Button_Select(0);	
		}
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode2)
	{
		//Not Move
	}
}


void SU100_MenuTabUp()
{
	int8_t nTmpBtnIndex = SU100_SelectUserIdx;
	SU100_List_UserCnt = SU100_GetUserCount();
	//(1)In H-mode, In the case of the top of the right column of the list, 
	//Do not move any further.
	if(!SU100_b_VMode && nTmpBtnIndex%2==1)
	{
		if(nTmpBtnIndex<=1)
		{
			return;
		}
		else if(nTmpBtnIndex>=3 && nTmpBtnIndex<=SU100_List_UserCnt)
		{
			//List->List
			nTmpBtnIndex = nTmpBtnIndex-2;
			SU100_Button_Select(nTmpBtnIndex);
			return;
		}
	}

	//(2) Calculate the index of the list.
	if(SU100_b_VMode)
	{
		nTmpBtnIndex = nTmpBtnIndex-1;
	}	
	else 
	{
		nTmpBtnIndex = nTmpBtnIndex-2;
	}

	if(g_nSU100_Mode==SU100_Select_Mode )
	{
		//(3)In H-mode, In the case of the top of the left column of the list, move to 'Add user' button.
		//(4)In V-mode, In the case of the top of the list, move to 'Add user' button.
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex<0)
		{
			//List->A button
			nTmpBtnIndex = SU100_SEL_A_BTN;//chpark.01.23
			SU100_Button_Select(nTmpBtnIndex);	
					
		}
		else if(nTmpBtnIndex==SU100_SEL_A_BTN)
		{
			//D button -> A button ->List End
			//Not Move
			nTmpBtnIndex=SU100_List_UserCnt-1;
			SU100_Button_Select(nTmpBtnIndex);	
		}
		else if(nTmpBtnIndex>=SU100_List_UserCnt && nTmpBtnIndex<SU100_SEL_A_BTN)
		{
			//A button->List
			nTmpBtnIndex=SU100_List_UserCnt-1;
			SU100_Button_Select(nTmpBtnIndex);	
		}	
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode1)
	{
		//(3)In H-mode, In the case of the top of the left column of the list, move to 'Delete' button.
		//(4)In V-mode, In the case of the top of the list, move to 'Delete' button.
		if(nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt)
		{
			//List->List
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(nTmpBtnIndex<0)
		{
			//List->DELETE button
			if( SU100_Chk_DelUser()>=1)
			{
				nTmpBtnIndex = SU100_SEL_DEL_BTN; //chpark.01.23
				SU100_Button_Select(nTmpBtnIndex);
			}
			else
			{
				nTmpBtnIndex = SU100_List_UserCnt-1;
				SU100_Button_Select(nTmpBtnIndex);
			}
		}
		else if(nTmpBtnIndex>=SU100_List_UserCnt && nTmpBtnIndex<=SU100_SEL_DEL_BTN)
		{
			//List->DELETE button->List
			nTmpBtnIndex=SU100_List_UserCnt-1;
			SU100_Button_Select(nTmpBtnIndex);	
		}
	}
	else if(g_nSU100_Mode==SU100_Delete_Mode2)
	{
		//Not Move
	}
}

void SU100_MenuTabLeftRight()
{
	int8_t nTmpBtnIndex = SU100_SelectUserIdx;
	SU100_List_UserCnt = SU100_GetUserCount();
	
	if(g_nSU100_Mode==SU100_Select_Mode )
	{
		if(SU100_SelectUserIdx == SU100_SEL_A_BTN)
		{
			nTmpBtnIndex = SU100_SEL_D_BTN;
			SU100_Button_Select(nTmpBtnIndex);
		}
		else if(SU100_SelectUserIdx == SU100_SEL_D_BTN)
		{
			nTmpBtnIndex = SU100_SEL_A_BTN;
			SU100_Button_Select(nTmpBtnIndex);
		}
	}
	//else if(g_nSU100_Mode==SU100_Delete_Mode2 )
	else if(g_nSU100_Mode==SU100_Delete_Mode2 ||g_nSU100_Mode==SU100_Delete_Mode3 ) //chpark.24.04.17
	{
		if(SU100_SelectUserIdx==SU100_SEL_YES_BTN)
		{
			nTmpBtnIndex = SU100_SEL_NO_BTN;
			SU100_Button_Select(nTmpBtnIndex);
		}
		else 
		{
			nTmpBtnIndex = SU100_SEL_YES_BTN;
			SU100_Button_Select(nTmpBtnIndex);	
		}
	}

	//(1)In H-mode, move [left <-> right] in list
	if(!SU100_b_VMode && (nTmpBtnIndex>=0 && nTmpBtnIndex<SU100_List_UserCnt))
	{
		if(nTmpBtnIndex%2==0)
		{
			//even
			//left List-> right List
			nTmpBtnIndex = nTmpBtnIndex+1;
			if(nTmpBtnIndex<SU100_List_UserCnt)
			{
				SU100_Button_Select(nTmpBtnIndex);
			}
			return;
		}
		else
		{
			//odd
			//right list-> left list
			nTmpBtnIndex = nTmpBtnIndex-1;
			SU100_Button_Select(nTmpBtnIndex);
			return;
		}
	}
}


bool SU100_JogWheel(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;

	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		//printf("Message Showing...\r\n");
		return false;
	}

	uint8_t JogDir = 0;
	JogDir = param[0];
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                SU100_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                SU100_MenuTabPrev();
			}
		break;
	}

    return true;
}

bool SU100_JogButton(ITUWidget* widget, char* param)
{
	if(g_nSU100_Mode==SU100_Delete_Result_Success || g_nSU100_Mode==SU100_Delete_Result_Fail)
	{
		return false;
	}
	
	SU100_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                    SU100_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    SU100_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
					SU100_MenuTabLeftRight();
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
             		SU100_MenuTabEnter(SU100_SelectUserIdx);  
        		}
					break;
			}
		break;
	}

    return true;
}

bool SU100_JogSwitch(ITUWidget* widget, char* param)
{
    SU100_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
       SU100_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}
//==========================================================

bool SU100_D7_MouseUp(ITUWidget* widget, char* param)
{
 	SU100_ExitCNT = 0;
	SU100_MenuTabEnter(SU100_SelectUserIdx);

    return true;
}

bool SU100_D6_MouseUp(ITUWidget* widget, char* param)
{
    SU100_ExitCNT = 0;
	#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);//chpark.12.12  JPHong 2024-12-18 
	#else
		HT_Save_Config(1);
	#endif
	//for test
	//printf("[SU100_Enter][SU100_Send_TX] 0byte set =============\n");
	SU100_Send_TX(0);
	//
	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool SU100_D5_MouseUp(ITUWidget* widget, char* param)
{
	SU100_ExitCNT = 0;
	#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);//chpark.12.12  JPHong 2024-12-18 
	#else
		HT_Save_Config(1);
	#endif

	SU100_MenuTabESC();
    return true;
}

//==========================================================

//Focus at current page.
//Draw Red Box
bool SU100_ListBox_Select(int nIndex)
{	
	int nTmpStartIndex=0,nTmpUserIndex=0;
	int nListIndex=0;
	int count = 5; //chpark.01.15

	int nCurrentPageIndex;

    if(nIndex==-1)
    {
        //printf("[V-mode][SU100_ListBox_Select]: %d\n",nIndex);
		SU100_SelectUserIdx = -1;
		SU100_cbt->selectIndex = -1;
		SU100_cbt->lastselectIndex = -1;

		SU100_h_cbt->selectIndex = -1;//chpark.01.23
		SU100_h_cbt->lastselectIndex = -1;

		SU100_Combo_DrawPage(0);

		//printf("[V-mode][SU100_ListBox_Select]: END \n");
 		
        return false;
    }
	else if(nIndex==0)
	{
		SU100_cbt->lastselectIndex = -1;//chaprk.01.23
		SU100_h_cbt->lastselectIndex = -1;//chaprk.01.23
	}
	else
	{
		//(1-1)save last index
		if(SU100_SelectUserIdx>=SU100_SEL_A_BTN)
		{
			SU100_cbt->lastselectIndex = -1;//chpark.01.23
			SU100_h_cbt->lastselectIndex = -1;//chpark.01.23
		}
		else
		{
			SU100_cbt->lastselectIndex = SU100_SelectUserIdx;//chpark.01.23
		}
	}

	//for jog focus
	ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
	ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
	//

	//(1-2)set current page
	if(SU100_SelectUserIdx<count)
		nCurrentPageIndex = 1;
	else
		nCurrentPageIndex = (SU100_SelectUserIdx)/count+1;

	//(2-1)set new page for new index
    int nTmpIndex = nIndex;
	//(2-2)save current index
    SU100_SelectUserIdx = nTmpIndex;

	//(3)set new current page
    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalPageCnt = (SU100_GetUserCount()/count)+1;
    int nPageIndex  = (nTmpIndex)/count+1;
    int nFocusIndex = (nTmpIndex)%count;

	//startIndex
	nTmpStartIndex = (nPageIndex-1)*(count); 
	
	//Call a function that automatically sets the focus 
    //after loading the page.
 	SU100_cbt->selectIndex = SU100_SelectUserIdx;//chpark.01.15

	//(3)
	if(nPageIndex == nCurrentPageIndex)
	{
		//printf("[SU100_ListBox_Select] already current page: \n draw current page [nPageIndex:%d  nCurrentPageIndex:%d SU100_SelectUserIdx:%d]\n",nPageIndex,nCurrentPageIndex,SU100_SelectUserIdx);
		//chpark.24.03.28
		SU100_Combo_SetFocus(SU100_SelectUserIdx);
		SU100_Combo_DrawPage(0);
	}
	else
	{
		//printf("[SU100_ListBox_Select] not current page: \n move to other page [nPageIndex:%d  nCurrentPageIndex:%d SU100_SelectUserIdx:%d]\n",nPageIndex,nCurrentPageIndex,SU100_SelectUserIdx);
		//chpark.24.03.28
		SU100_Combo_SetFocus(SU100_SelectUserIdx);
		SU100_Combo_DrawPage(0);
	}
  
    //printf("\n\n  SU100_ListBox_Select:nIndex=%d  nPageIndex=%d nFocusIndex=%d\n",nIndex,nPageIndex,nFocusIndex);
	Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.01.18

    return true;
}


//Focus at current page.
//Draw Red Box
bool SU100_ListBox_H_Select(int nIndex)
{	
	int nTmpStartIndex=0,nTmpUserIndex=0;
	int nListIndex=0;
	int count = 6;//chpark.02.19

	int nCurrentPageIndex;

    if(nIndex==-1)
    {
        //printf("[V-mode]SU100_ListBox_H_Select: %d\n",nIndex);
		SU100_SelectUserIdx = -1;

		SU100_cbt->selectIndex = -1; //chpark.01.23
		SU100_cbt->lastselectIndex = -1;

		SU100_h_cbt->selectIndex = -1;
		SU100_h_cbt->lastselectIndex = -1;

		SU100_Combo_H_DrawPage(0);
		//printf("[V-mode]SU100_ListBox_H_Select: END \n");
        return false;
    }
	else if(nIndex==0)
	{
		SU100_cbt->lastselectIndex = -1;
		SU100_h_cbt->lastselectIndex = -1;//chaprk.01.23
	}
	else
	{
		//(1-1)save last index
		if(SU100_SelectUserIdx>=SU100_SEL_A_BTN)
		{
			SU100_cbt->lastselectIndex = -1;//chpark.01.23
			SU100_h_cbt->lastselectIndex = -1;//chpark.01.23
		}
		else
		{
			SU100_h_cbt->lastselectIndex = SU100_SelectUserIdx/2;//chpark.01.23
		}
	}

	//for jog focus
	ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
	ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
	//

	//(1-2)set current page
	if(SU100_SelectUserIdx<count)
		nCurrentPageIndex = 1;
	else
		nCurrentPageIndex = (SU100_SelectUserIdx)/count+1;


	//(2-1)set new page for new index
    int nTmpIndex = nIndex;
	//(2-2)save current index
    SU100_SelectUserIdx = nTmpIndex;

	//(3)set new current page
    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalPageCnt = (SU100_GetUserCount()/count)+1;
    int nPageIndex  = (nTmpIndex)/count+1;
    int nFocusIndex = (nTmpIndex)%count;

	//startIndex
	nTmpStartIndex = (nPageIndex-1)*(count); 
	
	//Call a function that automatically sets the focus 
    //after loading the page.
 	SU100_h_cbt->selectIndex = SU100_SelectUserIdx/2;//chpark.01.23

	//(3)+++++
	if(nPageIndex == nCurrentPageIndex)
	{
		//printf("[SU100_ListBox_H_Select] already current page: \n draw current page [nPageIndex:%d  nCurrentPageIndex:%d]\n",nPageIndex,nCurrentPageIndex);
		//chpark.24.03.28
		SU100_Combo_H_SetFocus(SU100_SelectUserIdx);
		SU100_Combo_H_DrawPage(0);
	}
	else
	{
		//printf("[SU100_ListBox_H_Select] not current page: \n move to other page [nPageIndex:%d  nCurrentPageIndex:%d]\n",nPageIndex,nCurrentPageIndex);
		//chpark.24.03.28
		SU100_Combo_H_SetFocus(SU100_SelectUserIdx);
		SU100_Combo_H_DrawPage(0);
	}

	//chpark.01.15
	SU100_Set_H_ScrollBar(SU100_SelectUserIdx);
    //printf("\n\n  SU100_ListBox_H_Select:nIndex=%d  nPageIndex=%d nFocusIndex=%d\n",nIndex,nPageIndex,nFocusIndex);
	Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.01.18

    return true;
}


void SU100_Combo_Check_DragRegion()
{
	//chpark.24.02.20
	int count = 5;
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalUserCnt = SU100_GetUserCount();

	ITUWidget* pPageFirstItem = SU100_cbt->itemArr[0];
	int  first_item_pos       = pPageFirstItem->rect.y;//first item postion
	int  first_item_height    = pPageFirstItem->rect.height;
	
	int nMaxHeight = (nTotalUserCnt - count)*first_item_height;
	//
	if(nTotalUserCnt<=count)
	{
		ituComboTableMoveXY(SU100_cbt, 0, -first_item_pos);
		//printf("[SU100_Set_ScrollBar]Position Over:%d== nTotalUserCnt:%d \n",nMaxHeight,nTotalUserCnt);
	}
	else
	{
		if(abs(first_item_pos)>=nMaxHeight)
		{
			ituComboTableMoveXY(SU100_cbt, 0, -first_item_pos);
			ituComboTableMoveXY(SU100_cbt, 0, -nMaxHeight);
			//printf("[SU100_Set_ScrollBar]Position Over:%d== nTotalUserCnt:%d \n",nMaxHeight,nTotalUserCnt);
		}
	}
}


void SU100_Combo_H_Check_DragRegion()
{
	//chpark.24.02.20
	int count = 3;
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;

	int nTotalUserCnt = SU100_GetUserCount();

	ITUWidget* pPageFirstItem = SU100_h_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	
	int nMaxHeight;
	nMaxHeight = ((nTotalUserCnt+1)/2 - 3)*first_item_height;	
	//
	if(nTotalUserCnt<=count)
	{
		ituComboTableMoveXY(SU100_cbt, 0, -first_item_pos);
		//printf("[SU100_Set_ScrollBar]Position Over:%d== nTotalUserCnt:%d \n",nMaxHeight,nTotalUserCnt);
	}
	else
	{
		if(abs(first_item_pos)>=nMaxHeight)
		{
			ituComboTableMoveXY(SU100_h_cbt, 0, -first_item_pos);
			ituComboTableMoveXY(SU100_h_cbt, 0, -nMaxHeight);
			//printf("[SU100_Set_ScrollBar]Position Over:%d== nTotalUserCnt:%d \n",nMaxHeight,nTotalUserCnt);
		}
	}
}


void SU100_Set_ScrollBar(int nIndex)
{	
	//chpark.24.02.20
	int count = 5;
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalUserCnt = SU100_GetUserCount();

	ITUWidget* pPageFirstItem = SU100_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalUserCnt);

	//printf("[SU100_Set_ScrollBar]first_item_pos=%d first_item_height=%d fMovingRate=%f\n",first_item_pos,first_item_height,fMovingRate);
	int nBar_BackHeight = ituWidgetGetHeight(SU100_Slide_CONT);
	int nBar_Height; 

	int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(SU100_SlideBar, nBar_Height);
		ituWidgetSetY(SU100_SlideBar,  nStart_Margin);//chpark.24.03.28
		return;
	}
	else
	{
		nBar_Height = (nBar_BackHeight/nTotalUserCnt)*5;
		//(1)set scrollbar size
		ituWidgetSetHeight(SU100_SlideBar, nBar_Height);
	}

	int nBar_BackStartPosY  = ituWidgetGetY(SU100_Slide_CONT);
	int nBar_MovingPosY 	= nBar_BackHeight*fMovingRate; //nBar_BackHeight*fMovingRate;
	int nBar_NewPosY  		= nBar_MovingPosY;
	
	//final pos check
	int nBar_FinalPos = nBar_NewPosY + nBar_Height;
	if(nBar_FinalPos>=(nBar_BackHeight-nStart_Margin))
	{
		nBar_NewPosY = nBar_BackHeight - nBar_Height-nStart_Margin;//chpark.24.03.28
	}

	if(first_item_pos==0)
	{
		nBar_NewPosY=nStart_Margin; //chpark.24.03.28
	}
	//(2)set scrollbar position

	ituWidgetSetY(SU100_SlideBar,  nBar_NewPosY);
	//printf("[SU100_Set_ScrollBar]first_item_pos=%d fMovingRate=%f nBar_Height=%d nBar_MovingPosY=%d nBar_NewPosY=%d \n",first_item_pos,fMovingRate,nBar_Height,nBar_MovingPosY,nBar_NewPosY);
}

void SU100_Set_H_ScrollBar(int nIndex) //chpark.01.19
{
	//chpark.24.02.20
	int count = 6;//chpark.01.15
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalUserCnt = SU100_GetUserCount();
	int nTotalRow;
	nTotalRow = (nTotalUserCnt+1)/2;

	ITUWidget* pPageFirstItem = SU100_h_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalRow);
    //
	int nBar_BackHeight = ituWidgetGetHeight(SU100_Slide_CONT);
	int nBar_Height; 

	int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(SU100_SlideBar, nBar_Height);
		ituWidgetSetY(SU100_SlideBar,  nStart_Margin);//chpark.24.03.28 
		return;
	}
	else
	{
		nBar_Height = (nBar_BackHeight/nTotalRow)*3;
		//(1)set scrollbar size
		ituWidgetSetHeight(SU100_SlideBar, nBar_Height);
	}
	
	int nBar_BackStartPosY  = ituWidgetGetY(SU100_Slide_CONT);
	int nBar_MovingPosY 	= nBar_BackHeight*fMovingRate; //nBar_BackHeight*fMovingRate;
	int nBar_NewPosY  =  nBar_MovingPosY;
	
	//final pos check
	int nBar_FinalPos = nBar_NewPosY + nBar_Height;
	if(nBar_FinalPos>=(nBar_BackHeight-nStart_Margin))
	{
		nBar_NewPosY = nBar_BackHeight - nBar_Height -nStart_Margin;//chpark.24.03.28
	}

	if(first_item_pos==0)
	{
		nBar_NewPosY=nStart_Margin; //chpark.24.03.28
	}
	
	//(2)set scrollbar position
	ituWidgetSetY(SU100_SlideBar,  nBar_NewPosY);
	//printf("[SU100_Set_ScrollBar]fMovingRate=%f nBar_Height=%d nBar_MovingPosY=%d nBar_NewPosY=%d  \n",fMovingRate,nBar_Height,nBar_MovingPosY,nBar_NewPosY);
}


void SU100_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];
	
	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD)
	{
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_AD],true);
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_ST],false);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST || User_RegInfo[tUserIdx].RegFlag==SU100_USER_SPAD||User_RegInfo[tUserIdx].RegFlag==SU100_USER_SAD)
	{
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_ST],true);

		ituWidgetSetVisible(SU100_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU100_TOP_NAME, tDestName);
	}
	else
	{
		//printf("[SU100_SetUserTitle] hide icon: RegInfo:%d\n",User_RegInfo[tUserIdx].RegFlag);
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU100_TOP_ICON[SU100_ICON_ST],false);
	}

	if(g_UI1_ChkTime>0)
	{
		memset(tDestName,0,100);
	
		ituWidgetSetVisible(SU100_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU100_TOP_NAME, tDestName);
		//printf("\n[SU100_SetUserTitle] Show:%s==g_UI1_ChkTime=%d\n",tDestName,g_UI1_ChkTime);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		//printf("\n[SU100_SetUserTitle] Hide==g_UI1_ChkTime:%d\n",g_UI1_ChkTime);
		ituWidgetSetVisible(SU100_TOP_NAME,false);
	}
}

void SU100_MenuTabToggle()
{
	if(g_nSU100_Mode==SU100_Delete_Mode1)
	{
		//printf("nTmpMouseUpIndex=%d  SU100_SelectUserIdx=%d\n",nTmpMouseUpIndex,SU100_SelectUserIdx);
		if(SU100_SelectUserIdx<0)
			return;

		if(SU100_List_RegInfo[SU100_SelectUserIdx].RegFlag == SU100_USER_N_A || SU100_List_RegInfo[SU100_SelectUserIdx].RegFlag == SU100_USER_N_R)
		{
			//printf("NA or NR User cannot toggled:%d\n",SU100_List_RegInfo[SU100_SelectUserIdx].RegFlag);
			return;
		}
		//toggle
		SU100_List_RegInfo[SU100_SelectUserIdx].bDelSelect = !SU100_List_RegInfo[SU100_SelectUserIdx].bDelSelect;

		if(SU100_Chk_DelUser()>=1)
		{
			ituWidgetSetVisible(SU100_DEL_BTN[gMB_Language],true);
			//printf("[Draw Delete Button] SU100_Chk_DelUser = %d   gMB_Language=%d\n",SU100_Chk_DelUser(),gMB_Language);
		}
		else
		{
			ituWidgetSetVisible(SU100_DEL_BTN[gMB_Language],false);	
			//printf("[Hide Delete Button] SU100_Chk_DelUser = %d gMB_Language=%d\n",SU100_Chk_DelUser(),gMB_Language);
		}

		//printf("[Num=%d[SU100_MenuTabToggle]==[bSelect:%d]\n",SU100_SelectUserIdx,SU100_List_RegInfo[SU100_SelectUserIdx].bDelSelect);

		if(SU100_b_VMode)
		{
			SU100_Combo_DrawPage(0);
		}
		else
		{
			SU100_Combo_H_DrawPage(0);
		}
	}
}

int SU100_Chk_DelUser()
{
	int nTmpSeletedCnt=0;
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		if(SU100_List_RegInfo[i].bDelSelect)nTmpSeletedCnt++; 
	}

	return nTmpSeletedCnt;
}

//Get lowest empty user number
bool SU100_Chk_AddUser()
{
	bool bFindIdx = false;
	//printf("[SU100_Chk_AddUser]current user=%d level=%d \n\n",g_nSU100_CurUserIdx,User_RegInfo[g_nSU100_CurUserIdx].RegFlag);

	if(User_RegInfo[g_nSU100_CurUserIdx].RegFlag == SU100_USER_AD )
	{
		// TXcan [UM1:18511ACE={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}] 0byte 0bit:1 send to ctrl
		// request addu ser
				
		// standby CAN RX[UM5:18511C8C={}]
		// recieve empty user number
				
		// dev.ops lowest empty user number
		
		for(int i=SU100_LIST_START_USER_IDX;i<MAX_SU100_USER_NUM;i++)
		{
			if(User_RegInfo[i].RegFlag == SU100_USER_N_R && User_RegInfo[i].bRegister == false)//chpark.24.02.13
			{
				g_nSU100_AddUserIdx = i;
				//printf("[ADD Button]Empty User Number = %d (dev.ops)\r\n",g_nSU100_AddUserIdx);
				bFindIdx = true;
				break;
			}
		}

		// User add process start
        if(bFindIdx && g_nSU100_AddUserIdx>=0)
		{
			//S9_UserMode = MaUserAdd_Mode;
			SU100_Button_Select(SU100_SEL_A_BTN);
		}
		else
		{
			//printf("No Empty User Number:%d====\n",g_nSU100_AddUserIdx);
		}

	}

	return bFindIdx;
}

void SU100_MenuTabEnter(int nIndex)
{
	//Returns if moving to another layer.
	if(m_bSU100_MoveLayer) 
		return;
	
	//printf("[SU100_MenuTabEnter]===>%d \n",nIndex);
	if(SU100_SelectUserIdx<0){
		//printf("[SU100_MenuTabEnter]=%d No Focus \n",SU100_SelectUserIdx);
		return;
	}

	SU100_List_UserCnt = SU100_GetUserCount();

	if(nIndex>=0 && nIndex<SU100_List_UserCnt)
	{
		//Only admin can edit user info.
		if(g_nSU100_Mode==SU100_Select_Mode && User_RegInfo[g_nSU100_CurUserIdx].RegFlag == SU100_USER_AD)
		{
			m_bSU100_MoveLayer = true;

			g_nSU100_Mode    = SU100_Edit_Mode;
			//get real user num from list
			g_nSU100_EditUserIdx = SU100_List_RegInfo[nIndex].RegKeyNum;

			User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag   = User_RegInfo[g_nSU100_EditUserIdx].RegFlag; 
			User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX = User_RegInfo[g_nSU100_EditUserIdx].AccessAUX; 

			g_nSU200_InitButtonIndex = 0;//init focus

			ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
    		assert(M7_SU200_EDIT);
    		ituLayerGoto(M7_SU200_EDIT);
		}
		else if(g_nSU100_Mode==SU100_Delete_Mode1)
		{
			SU100_MenuTabToggle();
		}
		else
		{

		}
	}
	else
	{
		switch(nIndex)
		{
			case SU100_SEL_A_BTN:
				//Add mode
				//Checks whether a user can be added
				if(g_UI1_ChkTime>0)
				{
					//network mode
				}
				else
				{
					//local mode
					SU100_Chk_AddUser();
				}
				//only Admin user can add user.
				if(User_RegInfo[g_nSU100_CurUserIdx].RegFlag == SU100_USER_AD && g_nSU100_AddUserIdx>=0)
				{
					m_bSU100_MoveLayer = true;
					//ituWidgetSetVisible(SU100_A_MODE[SU100_SEL],false);
					//ituWidgetSetVisible(SU100_D_MODE[SU100_SEL],false);
					//printf("Go Name Layer=======>>>>\n");
					//go addmode
					g_nSU100_Mode = SU100_Add_Mode;
					
					g_nSU200_InitButtonIndex = 0;

					ITULayer *M7_SU300_NAME = ituSceneFindWidget(&theScene, "M7_SU300_NAME");
    				assert(M7_SU300_NAME);
    				ituLayerGoto(M7_SU300_NAME);
				}
				else
				{
					printf("current user=%d level=%d  new add user num=%d\n\n",g_nSU100_CurUserIdx,User_RegInfo[g_nSU100_CurUserIdx].RegFlag,g_nSU100_AddUserIdx);
				}
				break;

			case SU100_SEL_D_BTN:
				//Delete mode
				//Total user num>2
				//Admin user num>1
				//only Admin user can delete user.
				//printf("user count:%d   admin count:%d\n ",SU100_GetUserCount(),SU100_GetAdminCount() );
				if( SU100_GetUserCount()>0 && User_RegInfo[g_nSU100_CurUserIdx].RegFlag == SU100_USER_AD )
				{
					SU100_SelectUserIdx = -1;

					for(int i=0;i<MAX_SU100_USER_NUM;i++)
					{
						SU100_List_RegInfo[i].bDelSelect = false;
					}
					SU100_InitMode(SU100_Delete_Mode1);
					
					//chpark.01.18
					m_bSU100_List_Init = true;
					
					//chpark.01.18
					if(SU100_b_VMode)
					{
						SU100_Combo_DrawPage(0);
						SU100_Button_Select(0);
					}
					else
					{
						SU100_Combo_H_DrawPage(0);
						SU100_Button_Select(0);
					}
				}
				break;

			case SU100_SEL_DEL_BTN: // master user - [delete mode1] delete btn           
				{
					SU100_InitMode(SU100_Delete_Mode2);
					SU100_Button_Select(SU100_SEL_NO_BTN);
				}
				break;

			case SU100_SEL_YES_BTN: // [delete mode2] yes btn
				{
					if(g_nSU100_Mode==SU100_Delete_Mode2)
					{
						bool bSelfDel=false;
						for(int i=0;i<MAX_SU100_USER_NUM;i++)
						{
							if(SU100_List_RegInfo[i].bDelSelect)
							{
								if(SU100_List_RegInfo[i].RegKeyNum==g_nSU100_CurUserIdx)
								{
									//printf("Cannot Delete User:MySelf:ListIndex:%d curIndex:%d\n",SU100_List_RegInfo[i].RegKeyNum,g_nSU100_CurUserIdx);//chpark.10.26
									bSelfDel = true;
									break;
								}
							}
						}
						//for self delete test
						//bSelfDel = true;

						if(bSelfDel)
						{
							SU100_InitMode(SU100_Delete_Mode3); 
							SU100_Button_Select(SU100_SEL_NO_BTN);
						}
						else
						{
							SU100_InitMode(SU100_Delete_Mode2); //�״�� ����
							SU100_Button_Select(SU100_SEL_YES_BTN);
						
							SU100_UM5_Rcv_Type = SU100_RCV_DEL_USER_CHK;
							SU100_Send_TX(1);
						
							if(!gUI_Server_Mode)
							{
								SU100_Chk_DelResult_FromLocal();
							}
						}
					}
					else if(g_nSU100_Mode==SU100_Delete_Mode3)
					{
						//still mode2
						//only list update
						SU100_InitMode(SU100_Delete_Mode3); //�״�� ����
						SU100_Button_Select(SU100_SEL_YES_BTN);
						
						SU100_UM5_Rcv_Type = SU100_RCV_DEL_USER_CHK;
						SU100_Send_TX(1);
						
						if(!gUI_Server_Mode)
						{
							SU100_Chk_DelResult_FromLocal();
						}
					}
				
				}
				break;

			case SU100_SEL_NO_BTN: // [delete mode2] no btn
				{
					if(g_nSU100_Mode==SU100_Delete_Mode2)//chpark.11.20
					{
						SU100_InitMode(SU100_Delete_Mode1);
						SU100_Button_Select(0);//����Ʈ�� ù��° ���� ����.
					}
					else if(g_nSU100_Mode==SU100_Delete_Mode3)
					{
						SU100_InitMode(SU100_Delete_Mode2); 
						SU100_Button_Select(SU100_SEL_NO_BTN);
					}
				}
				break;

			case SU100_Delete_Result_Success:
				{
					for(int i=0;i<MAX_SU100_USER_NUM;i++)
					{
						if(User_RegInfo[i].RegFlag == SU100_USER_DEL)
						{
							User_RegInfo[i].RegFlag = 0;
						}
						SU100_List_RegInfo[i].bDelSelect = false;
					}
					SU100_InitMode(SU100_Select_Mode);
					SU100_Button_Select(SU100_SEL_A_BTN);
				}
				break;

			case SU100_Delete_Result_Fail:
				{
					for(int i=0;i<MAX_SU100_USER_NUM;i++)
					{
						if(User_RegInfo[i].RegFlag == SU100_USER_DEL)
						{
							User_RegInfo[i].RegFlag = 0;
						}
						SU100_List_RegInfo[i].bDelSelect = false;
					}

					SU100_InitMode(SU100_Select_Mode);
					SU100_Button_Select(SU100_SEL_A_BTN);
				}
				break;

			default:
				break;	
		}
	}
}


void SU100_MenuTabESC()
{
	SU100_ExitCNT = 0;
	switch(g_nSU100_Mode)
	{
		case SU100_Select_Mode:
		{
		  	ITULayer *Menu7_Layer = ituSceneFindWidget(&theScene, "Menu7_Layer");
    		assert(Menu7_Layer);
    		ituLayerGoto(Menu7_Layer);
		}
		break;

		case SU100_Add_Mode:
		{
			
		}
		break;
		
		case SU100_Delete_Mode1:
		case SU100_Delete_Result_Success:
		case SU100_Delete_Result_Fail:
		{
		  	SU100_InitMode(SU100_Select_Mode);
			SU100_Button_Select(SU100_SEL_A_BTN);

			for(int i=0;i<MAX_SU100_USER_NUM;i++)
			{
				SU100_List_RegInfo[i].bDelSelect = false;
			}

			//Redraw page
			//chpark.01.18
			
			if(SU100_b_VMode)
			{
				SU100_Combo_DrawPage(0);
			}
			else
			{
				SU100_Combo_H_DrawPage(0);
			}
		}
		break;

		case SU100_Delete_Mode2:
		{
		  	SU100_InitMode(SU100_Delete_Mode1);
			SU100_Button_Select(0);
		}
		break;

		case SU100_Delete_Mode3:
		{
			SU100_InitMode(SU100_Delete_Mode2); 
			SU100_Button_Select(SU100_SEL_NO_BTN);
		}
		break;

		default:
		{
			
		}
		break;

	}
}



//==============================================================

bool SU100_BTN_MouseDn(ITUWidget* widget, char* param)
{
    uint8_t i,tBTN_Index = 0;
	
	SU100_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);

    //printf("SU100 BTN DOWN = %d\r\n",tBTN_Index);
    switch(tBTN_Index)
	{
		case SU100_SEL_A_BTN:	// master user - user add
		{
			//S9_UserMode = MaUserAdd_Mode;
			SU100_Button_Select(SU100_SEL_A_BTN);
        }
        break;

        case SU100_SEL_D_BTN:    // master user - user delete
        {
			SU100_Button_Select(SU100_SEL_D_BTN);
        }
        break;

		case SU100_SEL_DEL_BTN:    // master user - user delete
        {
			SU100_Button_Select(SU100_SEL_DEL_BTN);
        }
        break;

		case SU100_SEL_YES_BTN:    // master user - user delete
        {
			SU100_Button_Select(SU100_SEL_YES_BTN);
        }
        break;

		 case SU100_SEL_NO_BTN:    // master user - user delete
        {
			SU100_Button_Select(SU100_SEL_NO_BTN);
        }
        break;
    }

    return true;
}

bool SU100_BTN_MouseUp(ITUWidget* widget, char* param)
{
    SU100_ExitCNT = 0;
	uint8_t tBTN_Index = (uint8_t)atoi(param);

    //printf("SU100 BTN UP = %d\r\n",tBTN_Index);
	SU100_MenuTabEnter(tBTN_Index);
	
    return true;
}

void SU100_Chk_DelResult_FromLocal()
{
	//=========================================================
	//check future admin user num after deleting & standard user num in List.
	bool bSuccess=true;

	int nFutureAdminCount=0;
    /*
	// 0;Not Register  1:Admin 2:Standard 3:deleted user
    for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{		    
		if( SU100_List_RegInfo[i].RegFlag==SU100_USER_AD  && !SU100_List_RegInfo[i].bDelSelect)
        {
            nFutureAdminCount++;
        }	
    }
	
	//cannot delete all admin
	if(nFutureAdminCount==0)
	{
		printf("Cannot Delete User:FutureAdminCount=%d\n",nFutureAdminCount);//chpark.10.26
		bSuccess = false;
	}
	

	//cannot delete self
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		if(SU100_List_RegInfo[i].bDelSelect)
		{
			if(SU100_List_RegInfo[i].RegKeyNum==g_nSU100_CurUserIdx)
			{
				printf("Cannot Delete User:MySelf:ListIndex:%d curIndex:%d\n",SU100_List_RegInfo[i].RegKeyNum,g_nSU100_CurUserIdx);//chpark.10.26
				bSuccess = false;
			}
		}
	}
	*/
	//=========================================================
	//success.
	if(bSuccess)
	{
		SU100_InitMode(SU100_Delete_Result_Success);

		for(int i=0;i<MAX_SU100_USER_NUM;i++)
		{
			if(SU100_List_RegInfo[i].bDelSelect)
			{
				SU100_List_RegInfo[i].RegFlag = SU100_USER_DEL;//deleted user.
				int tUserIndexNum = SU100_List_RegInfo[i].RegKeyNum;
				User_RegInfo[tUserIndexNum].RegFlag = SU100_USER_DEL;
			}
		}
		
		SU100_CleanUserInfo(); //remove flag==3 data from real user list.

		SU100_SetListUserInfo();//make temp user list from real user list.
		
		m_bSU100_List_Init = true; //for redraw scroll bar 

		#ifdef USED_SAVE_CONFIG		
			XO_Save_Config(M7_UserIndex,true,0);//chpark.24.04.05 for test  JPHong 2024-12-18 
		#else
			HT_Save_Config(1);
		#endif

		//Redraw current page from temp user list.
		if(SU100_b_VMode)//chpark.01.23
		{
			SU100_Combo_DrawPage(0);
		}
		else
		{
			SU100_Combo_H_DrawPage(0);
		}
	}
	else
	{
		for(int i=0;i<MAX_SU100_USER_NUM;i++)
		{
			SU100_List_RegInfo[i].bDelSelect = false;
		}

		SU100_InitMode(SU100_Delete_Result_Fail);
	}

	//Go to List Mode after 2second.
	SU100_SelectUserIdx = -1;
	m_nGoTo_SelectModeCnt=0;
	m_bSU100_GoTo_SelectMode = true;
}


void SU100_DBG_UserNameCode(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[30];

	memset(tDestName,0,30);
	ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx-1].UserName);

	//printf("User Name %d_[%s]\r\n",tUserIdx,tDestName);
	//printf("SRC HexCode : ");
	for(i=0;i<30;i++)
	{
		if((uint8_t)User_RegInfo[tUserIdx-1].UserName[i] == 0)
			break;

		//printf("%02X ",(uint8_t)User_RegInfo[tUserIdx-1].UserName[i]);
	}

	//printf("\r\n");
	//printf("DST HexCode : ");
	for(i=0;i<30;i++)
	{
		if((uint8_t)tDestName[i] == 0)
			break;
		//printf("%02X ",(uint8_t)tDestName[i]);
	}
	//printf("\r\n");
}

void SU100_ISO8859_to_UTF8(uint8_t *tDest,uint8_t *tSrc)
{
	uint8_t s_idx = 0,d_idx = 0;
	uint8_t ff_cnt= 0;
	
	while(tSrc[s_idx] > 0)
	{
		if(d_idx>=MAX_SU100_USER_NAME-1)
		{
			//printf("[SU100_ISO8859_to_UTF8] : Max return.\n");
			//printf("Src[%02X],Dest[%02X]\r\n",(uint8_t)tSrc[s_idx],(uint8_t)tDest[d_idx]);
			break;//chpark.12.12
		}

		if(tSrc[s_idx] < 0x80)
		{
			tDest[d_idx] = tSrc[s_idx];
			//printf("Src[%02X],Dest[%02X]\r\n",(uint8_t)tSrc[s_idx],(uint8_t)tDest[d_idx]);
			d_idx++;
			s_idx++;
		}
		else if(tSrc[s_idx] >= 0x80)
		{
			tDest[d_idx] = (uint8_t)(0xC0|(tSrc[s_idx]>>6));
			tDest[d_idx+1] = (uint8_t)(0x80|(tSrc[s_idx]&0x3F));
			//printf("Src[%02X],Dest[%02X %02X]\r\n",(uint8_t)tSrc[s_idx],(uint8_t)tDest[d_idx],(uint8_t)tDest[d_idx+1]);

			d_idx+=2;
			s_idx++;
		}

		if(tSrc[s_idx]==0xFF)
		{
			ff_cnt++;
		}
	}

	if(ff_cnt>=MAX_SU100_NAME_CNT-1)//if num of 0xFF character>=20, set null.
	{
		for(int i=0;i<MAX_SU100_USER_NAME;i++)
		{
			tDest[i]=0;
		}
		//printf("0xFF Null Name===\n");
	}
//	printf("\r\n");
}

//chpark.12.13
bool SU100_Is_Empty_InFlash(uint8_t *tSrc)
{
	bool bRet = false;
	uint8_t ff_cnt= 0;
	uint8_t s_idx = 0;
	while(tSrc[s_idx] > 0)
	{
		if(s_idx>=MAX_SU100_USER_NAME-1)
		{
			//printf("[SU100_ISO8859_to_UTF8] : Max return.\n");
			break;//chpark.12.12
		}

		if(tSrc[s_idx]==0xFF)
		{
			ff_cnt++;
		}

		s_idx++;
	}

	if(ff_cnt>=MAX_SU100_NAME_CNT-1)//if num of 0xFF character>=20, set null.
	{
		//printf("0xFF Null Name===\n");
		bRet = true;
	}

	return bRet;
}


void SU100_Chk_DelResult_FromCT(int nSuc)
{
	int nErrorMode = 0;
	//=========================================================
	//check future admin user num after deleting & standard user num
	bool bSuccess=true;

	int nFutureAdminCount=0;
    // 0;Not Register  1:Admin 2:Standard 3:deleted user

	SU100_Chk_DelUser_FromCT();//Get User info from controller
	
	/*
	//chpark.24.03.18
    for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{		    
		if( SU100_List_RegInfo[i].RegFlag==SU100_USER_AD && !SU100_List_RegInfo[i].bDelSelect)
        {
            nFutureAdminCount++;
        }	
    }

	//cannot delete all admin
	if(nFutureAdminCount==0)
	{
		bSuccess = false;
	}
	*/
	
	/*
	//cannot delete self
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		if(SU100_List_RegInfo[i].bDelSelect)
		{
			if(SU100_List_RegInfo[i].RegKeyNum==g_nSU100_CurUserIdx)
				bSuccess = false;
		}
	}
	*/

	//server check
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		if(SU100_List_RegInfo[i].bDelSelect)
		{
			SU100_List_RegInfo[i].RegFlag = SU100_USER_DEL;//deleted user.
			int tUserRealIndex = SU100_List_RegInfo[i].RegKeyNum;
			
			if(SU100_Delete_RES[tUserRealIndex]==1)
			{
				//delete success
				User_RegInfo[tUserRealIndex].RegFlag = SU100_USER_DEL;
				printf("[SU100_Chk_DelResult_FromCT] %d user deleted\n\n",tUserRealIndex);
			}
			else if(SU100_Delete_RES[tUserRealIndex]==2)
			{
				//delete fail
				nErrorMode = 2;
				bSuccess   = false;
				printf("[SU100_Chk_DelResult_FromCT] Server Response Error\n");
				printf("ListUserIdx=%d RealUserIdx=%d DelUserRes=%d  \n",i,tUserRealIndex,SU100_Delete_RES[tUserRealIndex]);
			}
			else if(SU100_Delete_RES[tUserRealIndex]==3)
			{
				//delete fail
				nErrorMode = 3;
				bSuccess = false;
				printf("[SU100_Chk_DelResult_FromCT] Server Response Error\n");
				printf("ListUserIdx=%d RealUserIdx=%d DelUserRes=%d  \n",i,tUserRealIndex,SU100_Delete_RES[tUserRealIndex]);
			}
			else
			{
				//delete fail - ETC
				nErrorMode = 2;
				bSuccess   = false;
				printf("[SU100_Chk_DelResult_FromCT] Server Response Error\n");
				printf("ListUserIdx=%d RealUserIdx=%d DelUserRes=%d  \n",i,tUserRealIndex,SU100_Delete_RES[tUserRealIndex]);
			}
		}
	}

	//=========================================================
	//success.
	m_bSU100_List_Init = true; //for redraw scroll bar 

	if(bSuccess)
	{
		SU100_InitMode(SU100_Delete_Result_Success);

		//chpark.01.18
		SU100_CleanUserInfo(); //remove flag==3 data from real user list.
		SU100_SetListUserInfo();//make  user list from real user list.
		/*
		if(SU100_b_VMode)//chpark.01.23
		{
			SU100_Combo_DrawPage(0);
		}
		else
		{
			SU100_Combo_H_DrawPage(0);
		}
		*/
		#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
			XO_Save_Config(M7_UserIndex,true,0);//chpark.12.12  JPHong 2024-12-18 
		#else
			HT_Save_Config(1);
		#endif
	}
	else
	{
		for(int i=0;i<MAX_SU100_USER_NUM;i++)
		{
			SU100_List_RegInfo[i].bDelSelect = false;
		}
		
		//chpark.24.03.18
		if(nErrorMode==3)
		{
			SU100_InitMode(SU100_Delete_Result_Fail_NoAdmin);
		}
		else
		{
			SU100_InitMode(SU100_Delete_Result_Fail);
		}

		SU100_CleanUserInfo(); //remove flag==3 data from real user list.
		SU100_SetListUserInfo();//make  user list from real user list.
	}

	//Go to List Mode after 2second.
	SU100_SelectUserIdx = -1;
	m_nGoTo_SelectModeCnt=0;
	m_bSU100_GoTo_SelectMode = true;
}


void SU100_Chk_DelUser_FromCT()
{
	memset(SU100_Delete_RES,0,sizeof(SU100_Delete_RES));

	//0:N/A  1:success 2:fail  3:Not deleted yet
	SU100_Delete_RES[0]	 = (SU100_UM5_All_Byte[3]>>0) & 0x03; //user #1
	SU100_Delete_RES[1]	 = (SU100_UM5_All_Byte[3]>>2) & 0x03; //user #2
	SU100_Delete_RES[2]	 = (SU100_UM5_All_Byte[3]>>4) & 0x03; //user #3
	SU100_Delete_RES[3]	 = (SU100_UM5_All_Byte[3]>>6) & 0x03; //user #4

	SU100_Delete_RES[4]	 = (SU100_UM5_All_Byte[4]>>0) & 0x03; //user #5
	SU100_Delete_RES[5]	 = (SU100_UM5_All_Byte[4]>>2) & 0x03; //user #6
	SU100_Delete_RES[6]	 = (SU100_UM5_All_Byte[4]>>4) & 0x03; //user #7
	SU100_Delete_RES[7]	 = (SU100_UM5_All_Byte[4]>>6) & 0x03; //user #8

	SU100_Delete_RES[8]	 = (SU100_UM5_All_Byte[5]>>0) & 0x03; //user #9
	SU100_Delete_RES[9]	 = (SU100_UM5_All_Byte[5]>>2) & 0x03; //user #10
	SU100_Delete_RES[10] = (SU100_UM5_All_Byte[5]>>4) & 0x03; //user #11
	SU100_Delete_RES[11] = (SU100_UM5_All_Byte[5]>>6) & 0x03; //user #12

	SU100_Delete_RES[12] = (SU100_UM5_All_Byte[6]>>0) & 0x03; //user #13
	SU100_Delete_RES[13] = (SU100_UM5_All_Byte[6]>>2) & 0x03; //user #14
	SU100_Delete_RES[14] = (SU100_UM5_All_Byte[6]>>4) & 0x03; //user #15
}

//======================================================
bool SU100_Timer(ITUWidget* widget, char* param)
{
	#ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

	//chpark.24.02.14
	#ifdef RELEASE_VER
	if ((UserPW_Time_AUI1+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		if(!m_bSU100_AUI1_Rcv_Error && !m_bSU100_GoTo_SelectMode && gUI_Server_Mode)
		{
			if(g_nSU100_Mode!=SU100_Select_Mode)
			{
				printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

				//x Show
				ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT],true);
				ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_ERROR_MSG],false);

				ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_SUCCESS],false);
				ituWidgetSetVisible(SU100_DEL_RESULT_ICON[SU100_FAIL],true);
		
				m_bSU100_AUI1_Rcv_Error  = true;
				m_bSU100_GoTo_SelectMode = true;
			}
		}
	}
	#endif

    if((gMainTimerCnt_10m - SU100_SecTimer) >= 20)//100)//200 msec
	{
		if(SU100_ExitCNT < 30*5)//30 sec
			SU100_ExitCNT++;
		else if(SU100_ExitCNT == 30*5)//30 sec
		{
			SU100_ExitCNT = 300;
			//printf("\r\n SU100 Menu 30Sec Exit > goto Main\r\n");
			#ifdef RELEASE_VER
			GotoMain(MAIN_MODE_NORMAL); 
			#endif
		}

		if(m_bSU100_GoTo_SelectMode)
		{
			m_nGoTo_SelectModeCnt++;

			if(m_nGoTo_SelectModeCnt>=2*5)//2 sec
			{
				m_bSU100_GoTo_SelectMode = false;
				m_nGoTo_SelectModeCnt=0;

				SU100_InitMode(SU100_Select_Mode);
				SU100_Button_Select(SU100_SEL_A_BTN);

				if(SU100_b_VMode)//chpark.01.23
				{
					SU100_Combo_GotoPage(0);//chpark.24.02.05
					SU100_Combo_DrawPage(0);
				}
				else
				{
					SU100_Combo_H_GotoPage(0);//chpark.24.02.05
					SU100_Combo_H_DrawPage(0);
				}
				
				//chpark.24.02.14
				if(m_bSU100_AUI1_Rcv_Error)
				{
					ituWidgetSetVisible(SU100_DELETE_MODE[SU100_CONT_RESULT],false);

					#ifdef RELEASE_VER
						//chpark.24.02.29
						ITULayer *Menu7_Layer = ituSceneFindWidget(&theScene, "Menu7_Layer");
						assert(Menu7_Layer);
						ituLayerGoto(Menu7_Layer);

						return true;
					#endif
				}
			}
		}

		//==================================================
        //data received chpark.08.22
		SU100_SendCNT++;
		
		if(SU100_SendCNT>=5)//1sec
		{
			SU100_SendCNT=0;
		}

        if(g_UM5_ChkTime>0 && !m_bSU100_GoTo_SelectMode)
        {
			printf("[SU100_RCV_DEL_USER_CHK] Received 1\n");
            g_UM5_ChkTime = 0;
           
            if(SU100_UM5_Rcv_Type == SU100_RCV_DEL_USER_CHK)
            {
				printf("[SU100_RCV_DEL_USER_CHK] Received 2\n");
                SU100_UM5_Rcv_Type = SU100_RCV_INIT;
                SU100_Chk_DelResult_FromCT(1);
              
            } 
        }
        //==================================================

		//Draw Slider Bar
		//chpark.24.02.19
		if(m_bSU100_Start_Check_SliderBar && m_bSU100_Button_Down)
		{
			if(SU100_b_VMode)
			{
				if(SU100_List_UserCnt>5)
				{
					SU100_Set_ScrollBar(0);
					//printf("[SU100_Set_ScrollBar]\n");
				}
			}
			else
			{
				if(SU100_List_UserCnt>6)
				{
					SU100_Set_H_ScrollBar(0);
					//printf("[SU100_Set_ScrollBar]\n");
				}
			}
		}

		SU100_SecTimer = gMainTimerCnt_10m;
	}

    return true;
}

void SU100_Send_Add_Cancel()//chpark.10.30
{
	uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	//if(g_nSU100_Mode==SU100_Add_Mode)//chpark.12.19
	{
		//End Register
        //UM1
        //0byte:[1bit] 0   0x00000000=>0
		//send [No longer addmode]
        txbuffer[0] = 0; //
				
        CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);
	}
}

bool SU100_Send_TX(int nType)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
	//=====================================================
	//make delete user info
	uint8_t tmpDelUserInfo[8];//5~6byte
	memset(tmpDelUserInfo, 0, sizeof(tmpDelUserInfo));
	//5byte 0bit: user #1
	//5byte 1bit: user #2
	//5byte 2bit: user #3
	//:
	//6byte 0bit: user #7
	//6byte 1bit: user #8

	for (int i=0; i<8; i++)//0-7
	{
		if (SU100_List_RegInfo[i].bDelSelect)
		{
			int nRealUserNum = SU100_List_RegInfo[i].RegKeyNum;
			if(nRealUserNum<8)
			{
				tmpDelUserInfo[5] = tmpDelUserInfo[5] | (0x01 << nRealUserNum);
			}
			else
			{
				nRealUserNum = nRealUserNum-8;
				tmpDelUserInfo[6] = tmpDelUserInfo[6] | (0x01 << nRealUserNum);
			}
		}
	}
	for (int i=0; i<7; i++)//8-14
	{
		if (SU100_List_RegInfo[i+8].bDelSelect)
		{
			int nRealUserNum = SU100_List_RegInfo[i+8].RegKeyNum;
			if(nRealUserNum<8)
			{
				tmpDelUserInfo[5] = tmpDelUserInfo[5] | (0x01 << nRealUserNum);
			}
			else
			{
				nRealUserNum = nRealUserNum-8;
				tmpDelUserInfo[6] = tmpDelUserInfo[6] | (0x01 << nRealUserNum);
			}
		}
	}
	//=========================================================

	//printf("[SU100_Send_TX]tmpDelUserInfo[5]=%x tmpDelUserInfo[6]=%x \n", tmpDelUserInfo[5], tmpDelUserInfo[6]);

    switch(nType)
    {
		case 0:
		  	txbuffer[0] = 0; //
        	CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);
			break;

        case 1:
			//UM1
			//5~6byte : send delete user info
			txbuffer[5] = tmpDelUserInfo[5]; //
			txbuffer[6] = tmpDelUserInfo[6]; //
			CanbusTxDataManagement(gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
            break;
        
        case 2:               
            break;

        default:
            break;
    }
}
//======================================================





