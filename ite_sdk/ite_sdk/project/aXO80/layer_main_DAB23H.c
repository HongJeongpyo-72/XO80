
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"
#include "layer_main_FW.h"

static int   m_nDAB3_H_SeviceChCnt=0;//chpark.09.25

int  g_DAB2_H_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.
int  g_DAB3_H_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.

//chpark.
//=====================================================================
static ITUWidget* DAB2_ListBar_Back_H; //chpark.09.11
static ITUWidget* DAB2_ListBar_Bar_H;  //chpark.09.11
//=====================================================================

static bool m_bDAB2_H_Scroll_Touched = false;//chpark.07.18

static bool m_bDAB2_H_ListInit  = false;
static bool m_bDAB2R_H_ListInit  = false;

static int nScroll_Tmp = 0;
bool JogDialKey_Scroll_DAB2_H(char* param);//chpark.06.21
bool JogDialRot_Scroll_DAB2_H(char* param);//chaprk.06.21
extern bool MainMode_RD_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff);//chpark.06.21

extern void DAB2_SetCategory(char* strTitle);//chpark.06.03


static void DAB2_H_ListInit(void);
static void DAB3_H_ListInit(void);


//=====================================================================
bool DAB2ListBoxH_Select(int nIndex);
bool DAB2ListBoxH_SetPTY(int nIndex);//chpark.06.21


bool DAB3ListBoxH_Reflesh();//chpark.07.18

void DAB2ListBoxH_ModeChange(int nDAB_MODE, bool bInfoReq);//chpark.11.27
//====================================================

//議곌?�� ?���?????��?��?��

bool DAB2ListBoxH_Select_UpItem();
bool DAB2ListBoxH_Select_DownItem();
bool DAB2ListBoxH_Select_NextItem();
bool DAB2ListBoxH_Select_PrevItem();
bool DAB2ListBoxH_Select_LeftRightItem();

bool DAB2ListBoxH_Select_NextPageItem();//chpark.24.03.05
bool DAB2ListBoxH_Select_PrevPageItem();//chpark.24.03.05
//======================================================


static char widgetName[30];

//static ITUScrollIconListBox* DAB2IconBox; //chpark.24.03.27
//static ITUScrollIconListBox* DAB2IconBoxR;//chpark.24.03.27

static ITUScrollBar* DAB2ListBar;

//static ITUScrollListBox* DAB2ListBox; //chpark.24.03.27
//static ITUScrollListBox* DAB2ListBoxR;//chpark.24.03.27


//char DAB3PTYNo[MAX_DAB3LIST_COUNT][MAX_DAB3LIST_CHAR];//chpark 05.26

typedef struct
{
    bool selected;
    char* name;
    int  nt_ListNum;//chpark.07.17
} DAB3_H_ListEntry;

static int DAB3_H_SelIndex = -1;//chpark
static int DAB3_H_ListCnt; //text or blank(total)
static int DAB3_H_ListCnt_withText;//text only //chpark.10.16
static DAB3_H_ListEntry DAB3ListArray[MAX_DAB3LIST_COUNT];


typedef struct
{
    bool selected;
    const char* name;
} DAB2_H_ListEntry;


static int DAB2_H_SelIndex=-1; //chpark.
static int DAB2ListCnt;
static DAB2_H_ListEntry DAB2ListArray[MAX_DAB2LIST_COUNT];

bool DAB2_m_b_H_FirstPTY = true;//chpark.09.26

//chpark.01.03
static int DAB2_H_LastSelIndex=-1; //chpark.
static int DAB3_H_LastSelIndex=-1; //chpark.

static ITUComboTable* dab2_h_cbt;
static ITUText* dab2_h_text; //0:left, 1:right
static ITUContainer* dab2_h_container;

void DAB2Combo_H_SetMaxMoveLimit();//chpark.24.05.22
void DAB2Combo_H_SetSlideEnd_AutoFocus();//chpark.24.06.28

bool DAB2Combo_H_PageChanged(ITUWidget* widget, char* param);
bool DAB2Combo_H_MouseUp(ITUWidget* widget, char* param);
bool DAB2Combo_H_Select(ITUWidget* widget, char* param);
bool DAB2Combo_H_Button_Up(ITUWidget* widget, char* param);
bool DAB2Combo_H_Button_Down(ITUWidget* widget, char* param);

void DAB2_Combo_H_DrawBlankItem(int nIndex);
void DAB2_Combo_H_DrawItem(int nIndex);
void DAB3_Combo_H_DrawItem(int nIndex);

void DAB2Combo_H_GotoPage(int nIndex);    //start at 0 page
void DAB2Combo_H_GotoPageTop(int nIndex); //start at 0 index

void DAB2Combo_H_Set_ScrollBar(int nIndex);//chpark.24.02.29

void DAB2Combo_H_GetComboPos();//chpark.24.03.04
void DAB2Combo_H_SetComboPos();//chpark.24.03.04

static bool b_DAB2_H_ComboSelected = false;

bool MainOnDAB2_H(bool bReload)
{
    //printf("\n\n[MainOnDAB2_H]START :  RDV.PTYNo:[%d]=======\n", RDV.PTYNo);

    SetWidget(dab2_h_cbt,"DAB2Combo_H");
    SetWidget(dab2_h_container,"DAB2Combo_H_Cont");

    int j,i=0;
    m_bDAB2_H_ListInit = false;//chpark.11.16
    m_bDAB2R_H_ListInit = false;//chpark.11.16

	SetWidget(DAB2ListBar, "DAB2ListBar_H");

    SetWidget(DAB2_ListBar_Back_H,"DAB2_ListBar_Back_H");//chpark.09.11
    SetWidget(DAB2_ListBar_Bar_H, "DAB2_ListBar_Bar_H"); //chpark.09.11

    //========================
    //chpark.09.26
    
    if(bReload)
    {
        //=============================
        //가로 혹은 세로에서 변화가 생겼기 때문에 리셋이 필요하다.
        //chpark.2024.03.04
        if(DAB2_H_SelIndex != g_DAB2_PreSelect_Index)
        {
            g_DAB2_H_PreSelect_POS = 0;
        }
        //=============================

        //chpark.12.05
        DAB2_H_SelIndex = g_DAB2_PreSelect_Index;
        DAB3_H_SelIndex = g_DAB3_PreSelect_Index;//chpark/12.05

        if(g_bDAB2_DataMode)
        {
            DAB2_H_ListInit();
        }
        else
        { 
            m_nDAB3_H_SeviceChCnt = RDV.DABServiceCnt[DAB2_H_SelIndex];//chpark.12.05
            for (i = 0; i < MAX_DAB2LIST_COUNT; i++)//chpark.24.04.22
            {
                DAB2ListArray[i].selected = false;
                DAB2ListArray[i].name = DAB2_PTY_No[i];
            }

            DAB3_H_ListInit();
        }
    }
    //=========================
    //chpark.01.03
    DAB2Combo_H_GotoPage(0);
    //printf("[MainOnDAB2_H]END :  RDV.PTYNo:[%d]=======\n\n", RDV.PTYNo);

    m_bDAB2_H_ListInit  = true; //chpark.24.03.13
    m_bDAB2R_H_ListInit = true;//chpark.24.03.13
    //

	return true;
}

void DAB2Combo_H_GetComboPos()
{
    ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
    g_DAB2_H_PreSelect_POS = pPageFirstItem->rect.y;//first item postion
    //[DAB2_Combo_H_GetComboPos]g_DAB2_H_PreSelect_POS=%d\n",g_DAB2_H_PreSelect_POS);
}

void DAB2Combo_H_SetComboPos() //chpark.24.08.02
{
    ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
    int nFirstItemPos = pPageFirstItem->rect.y;//current first item postion
    ituComboTableMoveXY(dab2_h_cbt, 0, -nFirstItemPos);//move to zero position

    ituComboTableMoveXY(dab2_h_cbt, 0, g_DAB2_H_PreSelect_POS);//move to Pre Position
    //printf("[DAB2_Combo_H_SetComboPos]g_DAB2_H_PreSelect_POS=%d\n",g_DAB2_H_PreSelect_POS);
}



void DAB2Combo_H_Set_ScrollBar(int nIndex)
{
     //chpark.24.08.06
	int count = 8;
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
	int nTotalUserCnt;
    int nTotalRow;
	
    if(g_bDAB2_DataMode)
    {
        nTotalUserCnt = MAX_DAB2LIST_COUNT;
    }
    else
    {
        nTotalUserCnt = m_nDAB3_H_SeviceChCnt;
    }
    nTotalRow = (nTotalUserCnt+1)/2;

	ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalRow);
	int    nScroll_Pos = round(abs(first_item_pos)*(1.0)/first_item_height); //일의 자리까지 반올림

	//printf("[SU100_Set_ScrollBar]first_item_pos=%d first_item_height=%d fMovingRate=%f\n",first_item_pos,first_item_height,fMovingRate);
	int nBar_BackHeight = ituWidgetGetHeight(DAB2_ListBar_Back_H);
	int nBar_Height; 

	int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar_H, nBar_Height);
		ituWidgetSetY(DAB2_ListBar_Bar_H,  nStart_Margin);//chpark.24.03.28
		return;
	}
	else
	{
		nBar_Height = (nBar_BackHeight/nTotalRow)*(count/2);
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar_H, nBar_Height);
	}

	int nBar_BackStartPosY  = ituWidgetGetY(DAB2_ListBar_Back_H);
	int nBar_MovingPosY 	= nBar_BackHeight*fMovingRate; 
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

	ituWidgetSetY(DAB2_ListBar_Bar_H,  nBar_NewPosY);
	ituScrollBarSetPosition(DAB2ListBar,  nScroll_Pos+1);//chpark.24.08.06//주의
	
	//printf("[DAB2Combo_Set_H_ScrollBar]nScroll_Pos=%d first_item_pos=%d fMovingRate=%f nBar_Height=%d nBar_MovingPosY=%d nBar_NewPosY=%d \n",nScroll_Pos, first_item_pos,fMovingRate,nBar_Height,nBar_MovingPosY,nBar_NewPosY);

    /*
	//chpark.24.02.20
	int count = 4;
	int nCurrentPageIndex;

	int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   

	int nTotalUserCnt;

    if(g_bDAB2_DataMode)
    {
        nTotalUserCnt = MAX_DAB2LIST_COUNT;
    }
    else
    {
        nTotalUserCnt = m_nDAB3_H_SeviceChCnt;
    }

	//ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
	//int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	//int  first_item_height   = pPageFirstItem->rect.height;
	//float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*(nTotalUserCnt/2));    
	//
	int nBar_BackHeight = ituWidgetGetHeight(DAB2_ListBar_Back_H);
	int nBar_Height; 
    int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar_H, nBar_Height);
		ituWidgetSetY(DAB2_ListBar_Bar_H,  0);
		return;
	}
	else
	{
		nBar_Height = (nBar_BackHeight/(nTotalUserCnt/2))*count;
        if(nBar_Height<=2)nBar_Height=2;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar_H, nBar_Height);
	}

    float  fMovingRate = abs(nBar_BackHeight-nBar_Height)*(1.0)/(nTotalUserCnt/2);

	int nBar_BackStartPosY  = ituWidgetGetY(DAB2_ListBar_Back_H);
	//int nBar_MovingPosY 	= nBar_BackHeight*fMovingRate; //nBar_BackHeight*fMovingRate;
	int nBar_NewPosY;
    int nCurSelected_Index;

    if(g_bDAB2_DataMode)
    {
        nCurSelected_Index  = DAB2_H_SelIndex;
    }
    else
    {
        nCurSelected_Index  = DAB3_H_SelIndex;
    }

    if(nCurSelected_Index<0)
    {
        nBar_NewPosY = nBar_BackStartPosY;
    }
    else
    {
        nBar_NewPosY = nBar_BackStartPosY + fMovingRate*(nCurSelected_Index/2);
    }
    
	//final pos check
	int nBar_FinalPos = nBar_NewPosY + nBar_Height;
    int nBar_MaxPos   = nBar_BackStartPosY + nBar_BackHeight;
	//if(nBar_FinalPos >= nBar_MaxPos-12)
    if(nBar_FinalPos >= nBar_MaxPos-12)
	{
		nBar_NewPosY = nBar_BackStartPosY + nBar_BackHeight - nBar_Height - nStart_Margin;
        //printf("[MAX]nBar_FinalPos=%d nBar_MaxPos=%d nBar_NewPosY=%d fMovingRate=%d\n",nBar_FinalPos,nBar_MaxPos,nBar_NewPosY,fMovingRate);
	}
    else
    {
        //printf("[not MAX]nBar_FinalPos=%d nBar_MaxPos=%d nBar_NewPosY=%d fMovingRate=%d\n",nBar_FinalPos,nBar_MaxPos,nBar_NewPosY,fMovingRate);
    }

	//(2)set scrollbar position
	ituWidgetSetY(DAB2_ListBar_Bar_H,  nBar_NewPosY);
    */
}



//==========================================================
//chpark.01.03
bool DAB2Combo_H_PageChanged(ITUWidget* widget, char* param)
{
    int nTouchIndex;
    nTouchIndex = atoi(param);

    if (dab2_h_cbt)
	{
		int vPage = ituComboTableGetCurrentPageNumber(dab2_h_cbt);
		char vStr[10] = "";
		//sprintf(vStr, "%d", vPage);
		//ituTextSetString(text, vStr);
        //printf("[DAB2Combo_H_PageChanged]PageNum:%d =====nTouchIndex:%d\n",vPage,nTouchIndex);
	}
    else
    {
        printf("[DAB2Combo_H_PageChanged]dab2_h_cbt is NULL Error====\n");
    }

    if(nTouchIndex==1000 || nTouchIndex==2000)
    {
        //adjust button
        ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
        ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];//chpark.24.04.03
        int dy =  pPageFirstItem->rect.height;

        int  first_item_pos       = pPageFirstItem->rect.y;//first item postion
        int  re_reitem_pos =  round(first_item_pos/97)*97 - first_item_pos;

        ituComboTableMoveXY(dab2_h_cbt, 0, re_reitem_pos);//move to zero position
        
        //
        if( pPageSelectItem->rect.y>=((4-1)*97) )//chpark.24.04.03
        {
            //move to next line(+ 1 line height & diff)
            //If it does not fit the exact line, adjust within the diff(+ 1 line & diff).
            int nLast_Diff=0;
            nLast_Diff = 4*97 - pPageSelectItem->rect.y;
            if(abs(nLast_Diff)<97)
            {
                ituComboTableMoveXY(dab2_h_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_h_cbt, 0, -dy);
            }
        }
        
        DAB2Combo_H_SetMaxMoveLimit();//chpark.24.05.22

        //chpark.24.06.27
		if(nTouchIndex==2000)
		{
			DAB2Combo_H_SetSlideEnd_AutoFocus();
		}

        
        //printf("[DAB2Combo_H_PageChanged]====Reposition\n");
    }

    b_DAB2_H_ComboSelected = false;

    DAB2Combo_H_Set_ScrollBar(0);//chpark.24.08.06

    return true;
}

//chpark.24.05.22
void DAB2Combo_H_SetMaxMoveLimit()
{
    //Max Limit
    int	nITU_Combo_nTop =  (-1)*(dab2_h_cbt->itemArr[0]->rect.height)*(dab2_h_cbt->itemcount-4);
    if(dab2_h_cbt->itemcount>4 && dab2_h_cbt->itemArr[0]->rect.y<=nITU_Combo_nTop)
    {
        int nLast_Diff = nITU_Combo_nTop - (dab2_h_cbt->itemArr[0]->rect.y);
        ituComboTableMoveXY(dab2_h_cbt, 0, abs(nLast_Diff));//move to re position
        //printf("[DAB2Combo_H_SetMaxMoveLimit]====%d dab2_h_cbt->itemArr[0]->rect.y=%d\n",abs(nLast_Diff),dab2_h_cbt->itemArr[0]->rect.y);
    }
}

//chpark.24.05.22
void DAB2Combo_H_SetSlideEnd_AutoFocus()
{
    ITUWidget* pPageFirstItem  = dab2_h_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];

    int nSelectListIndex;
    int nListCnt;

    if(g_bDAB2_DataMode)
    {   
        nSelectListIndex = DAB2_H_SelIndex;
        nListCnt = DAB2ListCnt;
	}
    else
    {
        nSelectListIndex = DAB3_H_SelIndex;
        nListCnt = DAB3_H_ListCnt;
    }

    if(pPageSelectItem->rect.y>=(4*97))
	{
		//down sliding-> bottom auto focus
		int nIndex;
		if(nSelectListIndex%2==0)
		{
			//even : left
			nIndex= abs(pPageFirstItem->rect.y)/97*2+8-2;
		}
		else
		{
			//odd : right
			nIndex= abs(pPageFirstItem->rect.y)/97*2+8-1;
		}

		if(nIndex>=nListCnt)
		{
			printf("[down sliding]Index Error !!==%d\n",nIndex);
			nIndex = nListCnt-1;
		}
		else
		{
			//printf("[DAB2Combo_H_SetSlideEnd_AutoFocus][1]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);			
		}

		DAB2ListBoxH_Select(nIndex);
	}
	else if(pPageSelectItem->rect.y<0)
	{
		//up sliding-> top auto focus
		int nIndex;
		if(nSelectListIndex%2==0)
		{
			//even : left
			nIndex= abs(pPageFirstItem->rect.y)/97*2;
		}
		else
		{
			//odd : right
			nIndex= abs(pPageFirstItem->rect.y)/97*2+1;
		}

		if(nIndex>=nListCnt || nIndex<=0)
		{
			printf("[up sliding]Index Error !!==%d\n",nIndex);
			nIndex = 0;
		}
		else
		{
			//printf("[DAB2Combo_H_SetSlideEnd_AutoFocus][2]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);
		}

		DAB2ListBoxH_Select(nIndex);
	}
	else
	{
		//printf("[DAB2Combo_H_SetSlideEnd_AutoFocus]pPageSelectItem->rect.y=%d\n",pPageSelectItem->rect.y);
	}

}


//chpark.01.03
//By modifying the sdk,
//Button up actually functions as a button down.
bool DAB2Combo_H_MouseUp(ITUWidget* widget, char* param)
{
    int nTouchIndex;
    
    nTouchIndex = atoi(param);

    //printf("[DAB2Combo_H_MouseUp(Down)][1]nTouchIndex:%d====%d\n",nTouchIndex,dab2_h_cbt->selectIndex);

    //(1)Deselect AUTO Button
    MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	//(2)Make the jog signal to be passed to the scroll.
	MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   
    //(3)Save current index

    //for 30 second
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.24.05.20 

    return true;
}


bool DAB2Combo_H_Button_Down(ITUWidget* widget, char* param)
{
    int nButtonIndex;
    
    nButtonIndex = atoi(param);
    //printf("\n\n[DAB2Combo_H_Button_Down]nButtonIndex=%d selectIndex:%d b_DAB2_ComboSelected:%d====\n",nButtonIndex,dab2_h_cbt->selectIndex,b_DAB2_H_ComboSelected);
    if(g_bDAB2_DataMode)
    {
        //Selectindex is already processed within the combo table.
        int nTmpSelecIdx;
        
        if(nButtonIndex==0)
        {
            //left  
            nTmpSelecIdx = dab2_h_cbt->selectIndex*2;
            if(nTmpSelecIdx>=MAX_DAB2LIST_COUNT)
            {
                return false;
            }
            DAB2_H_LastSelIndex = DAB2_H_SelIndex;
            DAB2_H_SelIndex = nTmpSelecIdx;
        }
        else
        {
            //right
            nTmpSelecIdx = dab2_h_cbt->selectIndex*2+1;
            if(nTmpSelecIdx>=MAX_DAB2LIST_COUNT)
            {
                return false;
            }
            DAB2_H_LastSelIndex = DAB2_H_SelIndex;
            DAB2_H_SelIndex = nTmpSelecIdx;
        }
        //save in global vari.
        
         g_DAB2_PreSelect_Index = DAB2_H_SelIndex;
    }
    else
    {
        //Selectindex is already processed within the combo table.
        int nTmpSelecIdx;
        if(nButtonIndex==0)
        {
            
            nTmpSelecIdx = dab2_h_cbt->selectIndex*2;
            if(nTmpSelecIdx>=m_nDAB3_H_SeviceChCnt)
            {
                return false;
            }
            DAB3_H_LastSelIndex = DAB3_H_SelIndex;
            DAB3_H_SelIndex = nTmpSelecIdx;
        }
        else
        {
           
            nTmpSelecIdx = dab2_h_cbt->selectIndex*2+1;
            if(nTmpSelecIdx>=m_nDAB3_H_SeviceChCnt)
            {
                return false;
            }
            DAB3_H_LastSelIndex = DAB3_H_SelIndex;
            DAB3_H_SelIndex = nTmpSelecIdx;
        }

        //save in global vari.
         g_DAB3_PreSelect_Index = DAB3_H_SelIndex;
    }

    //(4)Redraw Last selected index
		ITUBackground* item_bg = NULL;
		if (dab2_h_cbt->lastselectIndex >= 0)
		{
            if(g_bDAB2_DataMode)
            {
                if(nButtonIndex==0)
                {//left
                    DAB2_Combo_H_DrawItem(DAB2_H_LastSelIndex);
                }
                else
                {//right
                    DAB2_Combo_H_DrawItem(DAB2_H_LastSelIndex);
                }
            }
            else
            {
                if(nButtonIndex==0)
                {//left
                    DAB3_Combo_H_DrawItem(DAB3_H_LastSelIndex);
                }
                else
                {
                    //right
                    DAB3_Combo_H_DrawItem(DAB3_H_LastSelIndex);
                }
            }
		}

    //(5)Redraw Current selected index
		if (dab2_h_cbt->selectIndex >= 0)
		{
            if(g_bDAB2_DataMode)
            {
                DAB2_Combo_H_DrawItem(DAB2_H_SelIndex);
            }
            else
            {
                DAB3_Combo_H_DrawItem(DAB3_H_SelIndex);
            }
		}

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}


bool DAB2Combo_H_Select(ITUWidget* widget, char* param)
{
    int nTouchIndex,nSelectIndex;
    
    nTouchIndex = atoi(param);//
    nSelectIndex = nTouchIndex/10;

    if(nTouchIndex%10==0)
    {
        //printf("[DAB2Combo_H_Select] Left Touch: nSelectIndex=%d\n",nSelectIndex);
    }
    else
    {
        //printf("[DAB2Combo_H_Select] Right Touch: nSelectIndex=%d\n",nSelectIndex);
    }
    
    if (dab2_h_cbt)
	{
        b_DAB2_H_ComboSelected = true;

        //printf("\n[DAB2Combo_H_Select] Page:%d ItemCount:%d  pagemaxitem:%d Last_Idx:%d  Cur_Idx=%d =====\n",dab2_h_cbt->currPage,dab2_h_cbt->itemcount,dab2_h_cbt->pagemaxitem, dab2_h_cbt->lastselectIndex,dab2_h_cbt->selectIndex);
    }    
    else
    {
        //printf("[DAB2Combo_H_Select]dab2_h_cbt is NULL Error====\n");
    }

    return true;
}


bool DAB2Combo_H_Button_Up(ITUWidget* widget, char* param)
{
    int nButtonIndex;
    nButtonIndex = atoi(param);
    //printf("[DAB2Combo_H_Button_Up]nButtonIndex=%d selectIndex:%d b_DAB2_ComboSelected:%d DAB3_H_SelIndex:%d====\n",nButtonIndex,dab2_h_cbt->selectIndex,b_DAB2_H_ComboSelected,DAB3_H_SelIndex);

    //if (dab2_h_cbt && b_DAB2_H_ComboSelected)
    if (dab2_h_cbt) //chpark.24.06.27
	{
        //printf("\n[DAB2Combo_H_Button_Up] Page:%d ItemCount:%d pagemaxitem:%d Last_Idx:%d Cur_Idx=%d =====\n",dab2_h_cbt->currPage,dab2_h_cbt->itemcount,dab2_h_cbt->pagemaxitem, dab2_h_cbt->lastselectIndex,dab2_h_cbt->selectIndex);
        //(1)Save selected index

        if(g_bDAB2_DataMode)
        {
            //select DAB2 Category & change DAB2->DAB3
            DAB2ListBoxH_ModeChange(MAIN_MODE_DAB3,true);
        }
        else
        {
            //select DAB3 Channel & change DAB3->Default(DAB2)
            DAB2ListBoxH_ModeChange(MAIN_MODE_DAB2,true);
        }
	}
    else
    {
        printf("[DAB2Combo_H_Button_Up]dab2_h_cbt is NULL Error====b_DAB2_ComboSelected:%d\n",b_DAB2_H_ComboSelected);
    }

    b_DAB2_H_ComboSelected = false;
    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

void DAB2Combo_H_GotoPage(int nIndex)
{
    if(nIndex<=0)nIndex=0;//chpark.24.05.21
    //move to page
    ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
    int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
    int  first_item_height   = pPageFirstItem->rect.height;
    int  nPage_ItemCount  = 4;
    int  nPage_ItemHeight = nPage_ItemCount*first_item_height;

    //printf("[DAB2Combo_H_GotoPage]page=%d  position=%d\n",nIndex,-nPage_ItemHeight);
    
    ituComboTableMoveXY(dab2_h_cbt, 0, -first_item_pos);//move to zero position
    ituComboTableMoveXY(dab2_h_cbt, 0, -nPage_ItemHeight*nIndex);//move to selected page postion

    DAB2Combo_H_SetMaxMoveLimit();

    DAB2Combo_H_Set_ScrollBar(0);
}

void DAB2Combo_H_GotoPageTop(int nIndex)
{
    if(nIndex<MAX_DAB2LIST_COUNT && nIndex>=0)
    {
        //move to page
        ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
        int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
        int  first_item_height   = pPageFirstItem->rect.height;
        
        int  nSelect_ItemHeight = first_item_height*(nIndex/2);

        //printf("[DAB2Combo_H_GotoPageTop]nIndex=%d  curfirst_pos=%d first_pos=%d sel_pos=%d\n",nIndex,first_item_pos,-first_item_pos,-nSelect_ItemHeight);
        
        ituComboTableMoveXY(dab2_h_cbt, 0, -first_item_pos);//move to zero position
        ituComboTableMoveXY(dab2_h_cbt, 0, -nSelect_ItemHeight);//move to selected page postion

        DAB2Combo_H_Set_ScrollBar(0);
    }
}

//==========================================================

bool MainOnDAB2_H_OnOff(DISP_MODE DispMode)
{
    ituWidgetSetVisible(dab2_h_cbt, DispMode);//chpark.01.03

    ituWidgetSetVisible(DAB2ListBar, DispMode);

    ituWidgetSetVisible(DAB2_ListBar_Back_H, DispMode);//chpark.09.11
    ituWidgetSetVisible(DAB2_ListBar_Bar_H, DispMode);//chpark.09.11

	return true;
}

bool JogDialKey_Scroll_DAB2_H(char* param)
{
    char JD_Buf[2];
	//printf("JogDialKey_Scroll_DAB2_H= '0x%X' '%d'\n",param[0]);
	
    switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_LeftRightItem();//chpark.24.03.25
            //DAB2ListBoxH_Select_PrevPageItem();
            
        }
        break;

        case JOG_DIAL_KEY_RIGHT:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_LeftRightItem();//chpark.24.03.25
            //DAB2ListBoxH_Select_NextPageItem();
        }
        break;

        case JOG_DIAL_KEY_UP:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_UpItem();

        }
        break;

        case JOG_DIAL_KEY_DOWN:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_DownItem();
        }
        break;

        case JOG_DIAL_KEY_ENTER_ON:				//c3Byte0bit
		{
            //printf("JogDialKey_Scroll_DAB2_H=> JOG_DIAL_KEY_ENTER_ON '%d'\n",param[0]);
        }
        break;

        case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        {
            //printf("JogDialKey_Scroll_DAB2_H=> JOG_DIAL_KEY_ENTER_OFF '%d'\n",param[0]);
            
            //chpark.11.16
            if(g_bDAB2_DataMode)
            {
                //select DAB2 Category & change DAB2->DAB3
                DAB2ListBoxH_ModeChange(MAIN_MODE_DAB3,true);   
            }
            else
            {
                //select DAB3 Channel & change DAB3->Default(DAB2)
                DAB2ListBoxH_ModeChange(MAIN_MODE_DAB2,true); 
            }
        }
        break;

        case JOG_DIAL_KEY_NONE:
		{
            //printf("JogDialKey_Scroll_DAB2_H=> JOG_DIAL_KEY_NONE '%d'\n",param[0]);

            //MainMode_DAB2_Select_Index = RD_SEL_AUTO;   //chpark.06.21
            //MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);     //chpark.06.21 
        }
        break;
    }    

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

bool JogDialRot_Scroll_DAB2_H(char* param)
{
    char JD_Buf[2];
	//printf("JogDialKey_Scroll_DAB2_H= '0x%X' '%d'\n",param[0]);
	
    switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_PrevItem();
        }
        break;

        case JOG_DIAL_ROT_RIGHT:				//c3Byte0bit
		{
            DAB2ListBoxH_Select_NextItem();
        }
        break;

        
    }    

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}



//===========================================================================
bool DAB2ListH_OnEnter()
{
	//printf("DAB2ListH_OnEnter Start :  RDV.PTYNo:[%d]\n", RDV.PTYNo);
    ituWidgetSetVisible(dab2_h_cbt, DISP_ON);//chpak.01.03
	
	ituWidgetSetVisible(DAB2ListBar, DISP_ON);

    ituWidgetSetVisible(DAB2_ListBar_Back_H, DISP_ON);  //chpark.09.11
    ituWidgetSetVisible(DAB2_ListBar_Bar_H,  DISP_ON);  //chpark.09.11
	    
    //Select  DAB2 Mode or DAB3 Mode at start
    //====================================
    m_bDAB2_H_ListInit = false;//chpark.11.16
    m_bDAB2R_H_ListInit = false;//chpark.11.16

    m_bDAB2_H_Scroll_Touched = false;//chpark.09.25
    //====================================
    
    if(RDV.PTYNo<0)//chpark.10.24
	{
		RDV.PTYNo = -2;
	}
    //DAB2,DAB3 Data Loading
    
    //chpark.12.05
    DAB2_H_SelIndex = g_DAB2_PreSelect_Index;
    DAB3_H_SelIndex = g_DAB3_PreSelect_Index;//chpark/12.05


    if(g_bDAB2_DataMode)
    {
        //chpark.01.03
        dab2_h_cbt->lastselectIndex = -1;
        dab2_h_cbt->selectIndex = DAB2_H_SelIndex/2;
        //

        DAB2_H_ListInit();
    }
    else
    { 
        //chpark.01.03
        dab2_h_cbt->lastselectIndex = -1;
        dab2_h_cbt->selectIndex = DAB3_H_SelIndex/2;
        //

        m_nDAB3_H_SeviceChCnt = RDV.DABServiceCnt[DAB2_H_SelIndex];//chpark.12.05
        DAB3_H_ListInit();
    }

    RDV.DAB3_PTYNo = 0;//chpark.07.18
    
    //chpark.09.26
    DAB2_m_b_H_FirstPTY = true;
    if(g_bDAB2_DataMode && DAB2_m_b_H_FirstPTY)
    {
        m_nDAB3_H_SeviceChCnt = 0;//chpark.11.15
        DAB3_H_SelIndex = -1;//chpark.07.18
        //==================================
        //chpark.11.16
        int nStartPage;
       
        if(DAB2_H_SelIndex>=0)
        {
            nStartPage =  DAB2_H_SelIndex/8+1;
            
            if(abs(g_DAB2_H_PreSelect_POS)>0)
            {
                DAB2Combo_H_SetComboPos();
            }
            else
            {
                DAB2Combo_H_GotoPage(nStartPage-1);//chpark.24.03.01
            }

            //printf("(1)Focused Page:DAB2_H_SelIndex:%d\n",DAB2_H_SelIndex);
        }
        else if(DAB2_H_SelIndex<0 && RDV.DAB2_PTYNo<0)
        {
            nStartPage = 1;

            DAB2Combo_H_GotoPage(nStartPage-1);//chpark.24.03.01
            //printf("(2)Focused Page:DAB2_H_SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2_H_SelIndex,RDV.DAB2_PTYNo);
        }
        else if(DAB2_H_SelIndex<0 && RDV.DAB2_PTYNo>=0)
        {
            nStartPage = RDV.DAB2_PTYNo/8+1;

            if(DAB2_m_b_H_FirstPTY && RDV.DAB2_PTYNo>=0)
            {
                //printf("[DAB2ListH_OnEnter]DAB2Combo_H_GotoPageTop:DAB2_H_SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2_H_SelIndex,RDV.DAB2_PTYNo);
                DAB2Combo_H_GotoPageTop(RDV.DAB2_PTYNo);//chpark.24.03.27
            }
            else
            {
                //printf("(3)Focused Page:DAB2_H_SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2_H_SelIndex,RDV.DAB2_PTYNo);
                DAB2Combo_H_GotoPage(nStartPage-1);//chpark.24.03.01
            }
        }
        //==================================
        DAB2_m_b_H_FirstPTY = false;

        DAB2_Combo_H_DrawItem(DAB2_H_SelIndex);
    }
    else
    {
        //printf("[H-Mode]==========DAB2ListBoxH_ModeChange Start AT DAB3:DAB2_H_SelIndex:%d\n",DAB2_H_SelIndex);
        //printf("[H-Mode]==========DAB2ListBoxH_ModeChange Start AT DAB3:g_DAB2_PreSelect_Index:%d\n",g_DAB2_PreSelect_Index);
        //chpark.11.16
        //start at DAB3Mode
 
        //DAB2ListBoxH_ModeChange(MAIN_MODE_DAB3,false);
        //Set Channel Title
        char strTmp[50] = {0x20,};
        sprintf(strTmp, "%s",DAB2ListArray[DAB2_H_SelIndex].name);//chpark.06.03
        //printf("[H-Mode]==========DAB2ListBoxH_ModeChange Start AT DAB3:strTmp:%s\n",strTmp);

        DAB2_SetCategory(strTmp);

        //
        int nStartPage;

        if(DAB3_H_SelIndex>=0)
        {
            nStartPage =  (DAB3_H_SelIndex/2)/4+1;
            //printf("(1)Focused Page: nStartPage:%d DAB3_H_SelIndex:%d\n",nStartPage,DAB3_H_SelIndex);
        }
        else 
        {
            nStartPage = 1;
            //printf("(2)Focused Page: nStartPage:%d DAB3_H_SelIndex:%d\n",nStartPage,DAB3_H_SelIndex);
        }

        DAB2Combo_H_GotoPage(nStartPage-1);//chpark.01.03

        DAB3_Combo_H_DrawItem(DAB3_H_SelIndex);
    }

    m_bDAB2_H_ListInit  = true; //chpark.24.03.13
    m_bDAB2R_H_ListInit = true; //chpark.24.03.13

    return true;
}

static void DAB2_H_ListInit()
{
    ituComboTable_SetPageItemCount(4);//chpark.24.05.20
    int i;
    
	//printf("DAB2_H_ListInit:DAB2 Data Loading \n");
    for (i = 0; i < MAX_DAB2LIST_COUNT; i++)
    {
       	DAB2ListArray[i].selected = false;
        DAB2ListArray[i].name = DAB2_PTY_No[i];
    }
    
    RDV.DAB2_PTYNo =  RDV.PTYNo;
    if(RDV.DAB2_PTYNo<0)RDV.DAB2_PTYNo=-2;//chpark.10.24

    DAB2ListCnt = MAX_DAB2LIST_COUNT;

    //DAB2_H_SelIndex = -1;//chpark.07.18

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

     //chpark.01.03
    if(dab2_h_cbt)
	{       
        //printf("[DAB2_H_ListInit][1]dab2_h_cbt->itemcount=%d\n",dab2_h_cbt->itemcount);     
        //printf("[DAB2_H_ListInit][2]dab2_h_cbt->itemcount=%d\n",dab2_h_cbt->itemcount);   
        
        
        int nComboListCnt = dab2_h_cbt->itemcount;
        int nDAB2_H_ComboLineCnt = (MAX_DAB2LIST_COUNT+1)/2;

        if(nDAB2_H_ComboLineCnt>nComboListCnt)
        {
            //add
            int nAddCount = nDAB2_H_ComboLineCnt - nComboListCnt;
            ituComboTableAdd(dab2_h_cbt, nAddCount);
        }
        else if(nDAB2_H_ComboLineCnt<nComboListCnt)
        {
            //delete
            if (nComboListCnt>0)
            {
                for(int i=nComboListCnt-1; i>=nDAB2_H_ComboLineCnt ;i--)
                {
                    ituComboTableDelItem(dab2_h_cbt, i);
                }       
            }
        }
        else
        {
            //none
        }

        ituWidgetUpdate(dab2_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.13

        for (i = 0; i < MAX_DAB2LIST_COUNT; i++)
        {
            //DAB2_Combo_H_DrawBlankItem(i);
            DAB2_Combo_H_DrawItem(i);
        }

        
        DAB2Combo_H_Set_ScrollBar(0);//chpark.24.02.29

        /*
        int nTmpPageCnt=0;
        int nTmpMaxItemInPage=8;
        if(MAX_DAB2LIST_COUNT%nTmpMaxItemInPage==0)
        {
            nTmpPageCnt = MAX_DAB2LIST_COUNT/nTmpMaxItemInPage;
        }
        else
        {
            nTmpPageCnt = MAX_DAB2LIST_COUNT/nTmpMaxItemInPage+1;
        }

        //int nTmpPageCnt = (MAX_DAB2LIST_COUNT + nTmpMaxItemInPage - 1) / nTmpMaxItemInPage;
        //ituScrollBarSetLength(DAB2ListBar,nTmpPageCnt);//chpark.24.07.11
        ituScrollBarSetLength(DAB2ListBar,(MAX_DAB2LIST_COUNT+1)/2);//chpark.24.07.11
        */
        int nMovingLenth; 
	    int nCountInPage;

        nCountInPage = 8;
		nMovingLenth   = (MAX_DAB2LIST_COUNT+1)/2 - (nCountInPage/2) + 1;
		if(nMovingLenth<=1)nMovingLenth=1;
		ituScrollBar_SetInfo((MAX_DAB2LIST_COUNT+1)/2,(nCountInPage/2));//chpark.24.08.06
		ituScrollBarSetLength(DAB2ListBar, nMovingLenth);//chpark.24.08.06
		ituScrollBarSetPosition(DAB2ListBar,1);//chpark.24.08.06
	}
}

//chpark.24.02.26
void DAB2_Combo_H_DrawBlankItem(int nIndex)
{
    //reset one line.
    int i = nIndex/2;
    bool bRight_Combo = nIndex%2;
    bool bLeft_Combo  = !bRight_Combo;
    if(i<0) return;

    ITUWidget* tmpObj = NULL;
    if(bLeft_Combo)
    {
        tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 0);
        if(tmpObj)ituTextSetString(tmpObj, "");
        
        tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 1);
        if(tmpObj)ituTextSetString(tmpObj, "");
    }
    else
    {
        //tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 0);
        //if(tmpObj)ituTextSetString(tmpObj, "");
        
        tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 1);
        if(tmpObj)ituTextSetString(tmpObj, "");
    }
   

    ITUWidget* DAB2_HL_ItemIcon[4]; //
    DAB2_HL_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 0);
    DAB2_HL_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 1);
    DAB2_HL_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 2);
    DAB2_HL_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 3);

    ITUWidget* DAB2_HR_ItemIcon[4]; //
    DAB2_HR_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 4);
    DAB2_HR_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 5);
    DAB2_HR_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 6);
    DAB2_HR_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 7);


        //printf("[DAB2_Combo_H_DrawBlankItem] left combo===++++++++++++\n");
        ituWidgetSetVisible(DAB2_HL_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HL_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[3], false);//none

        ituWidgetSetVisible(DAB2_HR_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HR_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[3], false);//none
    return;



    if(bLeft_Combo)
    {
        //printf("[DAB2_Combo_H_DrawBlankItem] left combo===\n");
        ituWidgetSetVisible(DAB2_HL_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HL_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[3], false);//none

        ituWidgetSetVisible(DAB2_HR_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HR_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[3], false);//none
    }
    else
    {
        //printf("[DAB2_Combo_H_DrawBlankItem] right combo===\n");
        ituWidgetSetVisible(DAB2_HL_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HL_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HL_ItemIcon[3], true);//none

        ituWidgetSetVisible(DAB2_HR_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_HR_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_HR_ItemIcon[3], true);//none
    }
}

void DAB2_Combo_H_DrawItem(int nIndex)
{
    if(nIndex<0)return;

    int i = nIndex/2;
    bool bRight_Combo = nIndex%2;
    bool bLeft_Combo = !bRight_Combo;
    
    ITUWidget* tmpObj = NULL;

                //(1)TEXT
				char strItemText[22];
				//sprintf(strItemText, "%s", i);
                memcpy(strItemText,DAB2_PTY_No[nIndex],22);
                //printf("[DAB2_Combo_H_DrawItem]=[%s] %d \n",strItemText,nIndex);
                 if(bLeft_Combo)
                {//left
				    tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 0);
                }
                else
                {//right
                    tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 1);
                }

				if (tmpObj)
                {
                    if(RDV.DAB2_PTYNo==nIndex)
                    {
                        if(RDV.DABServiceCnt[nIndex]==0)
                        {
                            //if number of channels ==0
                            //Gray
                            ituWidgetSetColor(tmpObj,255,100,100,100);          
                        }
                        else
                        {
                            //black
                            ituWidgetSetColor(tmpObj,255,0,0,0);
                            //printf("[DAB2_Combo_DrawItem] text black: ch num: %d\n",RDV.DABServiceCnt[nIndex]);
                        }
                    }
                    else
                    {
                        if(RDV.DABServiceCnt[nIndex]==0)
                        {
                            //if number of channels ==0
                            //Gray
                            ituWidgetSetColor(tmpObj,255,100,100,100);         
                        }
                        else
                        {
                            //if number of channels  >0
                            //White
                            ituWidgetSetColor(tmpObj,255,255,255,255);
                        }
                    }

                    ituTextSetString(tmpObj, strItemText);
                }
                else
                {
                    printf("[DAB2_Combo_DrawItem] text =>null error\n");
                }
                
                //(2)BK Color
                ITUWidget* DAB2_HL_ItemIcon[4]; //
                DAB2_HL_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 0);
                DAB2_HL_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 1);
                DAB2_HL_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 2);
                DAB2_HL_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 3);

                ITUWidget* DAB2_HR_ItemIcon[4]; //
                DAB2_HR_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 4);
                DAB2_HR_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 5);
                DAB2_HR_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 6);
                DAB2_HR_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 7);
                
                
                if(bLeft_Combo)
                {
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[0], false);//gray + redbox
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[1], false);//gray
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[2], false);//redbox
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[3], false);//none
                    
                    if(nIndex==RDV.DAB2_PTYNo && nIndex==DAB2_H_SelIndex)//PTY & Focus
                    {
                        if(RDV.DABServiceCnt[nIndex]==0)//chpark.24.03.27
                        {
                            //if no service channel, set Focus BK
                            //RedBox
                            ituWidgetSetVisible(DAB2_HL_ItemIcon[2], true);
                            //printf("[RED BOX] i=%d DAB2_H_SelIndex=%d\n",i,DAB2_H_SelIndex);
                        }
                        else
                        {
                            //Silver + RedBox
                            ituWidgetSetVisible(DAB2_HL_ItemIcon[0], true);
                        }
                    }
                    else if(nIndex==RDV.DAB2_PTYNo && nIndex!=DAB2_H_SelIndex)//PTY & not Focus  
                    {
                        //if no service channel, set NONE BK
                        if(RDV.DABServiceCnt[nIndex]==0)
                        {
                            //None
                            ituWidgetSetVisible(DAB2_HL_ItemIcon[3], true);
                        }
                        else
                        {
                            //Silver 
                            ituWidgetSetVisible(DAB2_HL_ItemIcon[1], true);
                            //printf("[DAB2_Combo_DrawItem][LEFT][Draw Silver]  index:%d RDV.DAB2_PTYNo:%d chnum:%d DAB2_H_SelIndex:%d\n",nIndex,RDV.DAB2_PTYNo,RDV.DABServiceCnt[nIndex],DAB2_H_SelIndex);
                        }
                    }           
                    else if(nIndex!=RDV.DAB2_PTYNo && nIndex==DAB2_H_SelIndex)//not PTY&Focus
                    {
                        //RedBox
                        ituWidgetSetVisible(DAB2_HL_ItemIcon[2], true);
                        //printf("[RED BOX] i=%d DAB2_H_SelIndex=%d\n",nIndex,DAB2_H_SelIndex);
                    }
                    else
                    {
                        //None
                        ituWidgetSetVisible(DAB2_HL_ItemIcon[3], true);
                        //printf("[NONE BOX] i=%d DAB2_H_SelIndex=%d\n",nIndex,DAB2_H_SelIndex);
                    }

                    //printf("[DAB2_Combo_DrawItem][LEFT]  index:%d RDV.DAB2_PTYNo:%d chnum:%d DAB2_H_SelIndex:%d\n",nIndex,RDV.DAB2_PTYNo,RDV.DABServiceCnt[nIndex],DAB2_H_SelIndex);
                }
                else
                {
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[0], false);//gray + redbox
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[1], false);//gray
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[2], false);//redbox
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[3], false);//none

                     if(nIndex==RDV.DAB2_PTYNo && nIndex==DAB2_H_SelIndex)//PTY & Focus
                    {
                        if(RDV.DABServiceCnt[nIndex]==0)
                        {

                            //if no service channel, set Focus BK
                            //RedBox
                            ituWidgetSetVisible(DAB2_HR_ItemIcon[2], true);
                            //printf("[RED BOX] nIndex=%d DAB2_H_SelIndex=%d\n",nIndex,DAB2_H_SelIndex);
                        }
                        else
                        {
                            //Silver + RedBox
                            ituWidgetSetVisible(DAB2_HR_ItemIcon[0], true);
                        }
                    }
                    else if(nIndex==RDV.DAB2_PTYNo && nIndex!=DAB2_H_SelIndex)//PTY & not Focus  
                    {
                        //if no service channel, set NONE BK
                        if(RDV.DABServiceCnt[nIndex]==0)
                        {
                            //None
                            ituWidgetSetVisible(DAB2_HR_ItemIcon[3], true);
                        }
                        else
                        {
                            //Silver 
                            ituWidgetSetVisible(DAB2_HR_ItemIcon[1], true);
                        }
                    }           
                    else if(i!=RDV.DAB2_PTYNo && nIndex==DAB2_H_SelIndex)//not PTY&Focus
                    {
                        //RedBox
                        ituWidgetSetVisible(DAB2_HR_ItemIcon[2], true);
                        //printf("[RED BOX] nIndex=%d DAB2_H_SelIndex=%d\n",nIndex,DAB2_H_SelIndex);
                    }
                    else
                    {
                        //None
                        ituWidgetSetVisible(DAB2_HR_ItemIcon[3], true);
                        //printf("[NONE BOX] nIndex=%d DAB2_H_SelIndex=%d\n",nIndex,DAB2_H_SelIndex);
                    }
                    //printf("[DAB2_Combo_DrawItem][RIGHT]  index:%d RDV.DAB2_PTYNo:%d chnum:%d DAB2_H_SelIndex:%d\n",nIndex,RDV.DAB2_PTYNo,RDV.DABServiceCnt[nIndex],DAB2_H_SelIndex);

                }
}

static void DAB3_H_ListInit(void)
{
    ituComboTable_SetPageItemCount(4);//chpark.24.05.20
    
	int i;

    int nTmpCurrentNumChar=0;
    DAB3_H_ListCnt=0;
    DAB3_H_ListCnt_withText=0;//chpark.10.16
    int nIndex=0;
    for (i = 1; i <= 500; i++)//chpark.09.25
 	{
        nTmpCurrentNumChar = strlen(DAB3PTYNo[i]);
	   	
        if( (nTmpCurrentNumChar>0 && i<=m_nDAB3_H_SeviceChCnt)|| (DAB3_H_ListCnt<m_nDAB3_H_SeviceChCnt && i<=m_nDAB3_H_SeviceChCnt))//chpark.10.16
        {
            //sprintf(&DAB3PTYNo[i][0],"KKKKKKKKKKKKKKKKKKKKLLLLLLLLLLLLLLLL");
            ////printf("[%d]Channel Title before:%s\n",i,&DAB3PTYNo[i][0]);
            //memcpy(&DAB3PTYNo_LIMIT[i][0],&DAB3PTYNo[i][0],17);//chpark.10.17
            memcpy(&DAB3PTYNo_LIMIT[i][0],&DAB3PTYNo[i][0],14);//chpark.24.02.26
            //printf("[%d]Channel Title after:%s\n",i,&DAB3PTYNo_LIMIT[i][0]);

	   	    DAB3ListArray[nIndex].selected = false;
	        DAB3ListArray[nIndex].name = &DAB3PTYNo_LIMIT[i][0];//chpark.07.18
           
            DAB3ListArray[nIndex].nt_ListNum = i;
       
            //for real.
            //ꮿ럿 모드 ??? 21?까꽿 display�??¥
            //In landscape mode, up to 21 characters can be displayed.
            //sprintf(DAB3ListArray[nIndex].name,"KKKKKKKKKKKKKKKKKKKK");
            //sprintf(DAB3ListArray[nIndex].name,"KKKKKKKKKKKKKKKKKKKKLLLLLLLLLLLLLLLL");
            
            //printf("[H][%d]Channel Title:%s\n",i,DAB3ListArray[nIndex].name);
            nIndex++;
            DAB3_H_ListCnt++;   
            //chpark.10.16
            if(nTmpCurrentNumChar>0 )
            {
                DAB3_H_ListCnt_withText++;
            }  
        }
        else
        {
            ////printf("[%d]Channel Title new:%s  NO_TITLE\n",i,DAB3ListArray[i].name);
        }
        
	}
    //Each dab3 channel must be memorized according to the category.
    //Set the initial value to 0 by default For Test.
    //Because of the jog function, one channel must be set.
     
    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    //chpark.01.03
    if(dab2_h_cbt)
	{       
            int nComboListCnt = dab2_h_cbt->itemcount;
            int nDAB3_H_ComboLineCnt = (m_nDAB3_H_SeviceChCnt+1)/2;

             //printf("nComboListCnt:%d nDAB3_H_ComboLineCnt:%d m_nDAB3_H_SeviceChCnt:%d\n",nComboListCnt,nDAB3_H_ComboLineCnt,m_nDAB3_H_SeviceChCnt);

            if(nDAB3_H_ComboLineCnt>nComboListCnt)
            {
                //add
                int nAddCount = nDAB3_H_ComboLineCnt - nComboListCnt;
                ituComboTableAdd(dab2_h_cbt, nAddCount);
                //printf("[DAB3_H_ListInit] [#%d] item add. remain:%d\n",nAddCount,dab2_h_cbt->itemcount);
            }
            else if(nDAB3_H_ComboLineCnt<nComboListCnt)
            {
                //delete
                if (nComboListCnt>0)
                {
                    for(int i=nComboListCnt-1; i>=nDAB3_H_ComboLineCnt ;i--)
                    {
                        ituComboTableDelItem(dab2_h_cbt, i);
                        //printf("[DAB3_H_ListInit]%d item delete. remain:%d\n",i,dab2_h_cbt->itemcount);
                    }       
                }
            }
            else
            {
                //none
            }

            ituWidgetUpdate(dab2_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.13

            for (i = 0; i < m_nDAB3_H_SeviceChCnt; i++)//chpark.24.03.27
			{
                DAB2_Combo_H_DrawBlankItem(i);
			}

            for (i = 0; i < m_nDAB3_H_SeviceChCnt; i++)
			{
                DAB3_Combo_H_DrawItem(i);
			}

            DAB2Combo_H_Set_ScrollBar(0);//chpark.24.02.29
        
        /*
        int nTmpPageCnt=0;
        int nTmpMaxItemInPage=8;
        if(DAB3_H_ListCnt%nTmpMaxItemInPage==0)
        {
            nTmpPageCnt = DAB3_H_ListCnt/nTmpMaxItemInPage;
        }
        else
        {
            nTmpPageCnt = DAB3_H_ListCnt/nTmpMaxItemInPage+1;
        }

        //int nTmpPageCnt = (MAX_DAB2LIST_COUNT + nTmpMaxItemInPage - 1) / nTmpMaxItemInPage;
        //ituScrollBarSetLength(DAB2ListBar,nTmpPageCnt);//chpark.24.07.11
        ituScrollBarSetLength(DAB2ListBar,(DAB3_H_ListCnt+1)/2);//chpark.24.07.11
        */
        int nMovingLenth; 
	    int nCountInPage;

        nCountInPage = 8;
		nMovingLenth   = (DAB3_H_ListCnt+1)/2 - (nCountInPage/2) + 1;
		if(nMovingLenth<=1)nMovingLenth=1;
		ituScrollBar_SetInfo((DAB3_H_ListCnt+1)/2,(nCountInPage/2));//chpark.24.08.06
		ituScrollBarSetLength(DAB2ListBar, nMovingLenth);//chpark.24.08.06
		ituScrollBarSetPosition(DAB2ListBar,1);//chpark.24.08.06
	}
}


void DAB3_Combo_H_DrawItem(int nIndex)
{
    int i = nIndex/2;
    bool bRight_Combo = nIndex%2;
    bool bLeft_Combo = !bRight_Combo;
    if(nIndex<0) return;//chpark.24.02.26

    DAB3_H_ListEntry* entry;
    entry = &DAB3ListArray[nIndex];


    ITUWidget* tmpObj = NULL;

                //(1)TEXT
				char strItemText[22];
				//sprintf(strItemText, "%s", i);
                
                //memcpy(strItemText,&DAB3PTYNo[nIndex][0],22);
                if(bLeft_Combo)
                {//left
				    tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 0);
                }
                else
                {//right
                    tmpObj = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_TEXT, 1);
                }

				if (tmpObj)
                {
                    //White
                    ituWidgetSetColor(tmpObj,255,255,255,255);
                    if(entry->name != NULL)
                    {
                        ituTextSetString(tmpObj, (char*)entry->name);
                    }
                    else
                    {
                        ituTextSetString(tmpObj, "");
                    }
                    
                    //printf("[DAB3_Combo_H_DrawItem]i=%d nIndex=%d : %s\n",i,nIndex,(char*)entry->name);
                }

                //(2)BK Color
                ITUWidget* DAB2_HL_ItemIcon[4]; //
                DAB2_HL_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 0);
                DAB2_HL_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 1);
                DAB2_HL_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 2);
                DAB2_HL_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 3);

                ITUWidget* DAB2_HR_ItemIcon[4]; //
                DAB2_HR_ItemIcon[0] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 4);
                DAB2_HR_ItemIcon[1] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 5);
                DAB2_HR_ItemIcon[2] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 6);
                DAB2_HR_ItemIcon[3] = ituComboTableGetItemChild(dab2_h_cbt, i, ITU_ICON, 7);

                if(bLeft_Combo)
                {
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[0], false);//gray + redbox
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[1], false);//gray
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[2], false);//redbox
                    ituWidgetSetVisible(DAB2_HL_ItemIcon[3], false);//none

                    if(nIndex==DAB3_H_SelIndex)//not PTY&Focus
                    {
                        //RedBox
                        ituWidgetSetVisible(DAB2_HL_ItemIcon[2], true);
                        //printf("[RED BOX] nIndex=%d DAB3_H_SelIndex=%d\n",nIndex,DAB3_H_SelIndex);
                    }
                    else
                    {
                        //None
                        ituWidgetSetVisible(DAB2_HL_ItemIcon[3], true);
                        printf("[NONE][L] nIndex=%d DAB3_H_SelIndex=%d\n",nIndex,DAB3_H_SelIndex);
                    }
                }
                else
                {
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[0], false);//gray + redbox
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[1], false);//gray
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[2], false);//redbox
                    ituWidgetSetVisible(DAB2_HR_ItemIcon[3], false);//none

                    if(nIndex==DAB3_H_SelIndex)//not PTY&Focus
                    {
                        //RedBox
                        ituWidgetSetVisible(DAB2_HR_ItemIcon[2], true);
                        //printf("[RED BOX] nIndex=%d DAB3_H_SelIndex=%d\n",nIndex,DAB3_H_SelIndex);
                    }
                    else
                    {
                        //None
                        ituWidgetSetVisible(DAB2_HR_ItemIcon[3], true);
                        printf("[NONE][R] nIndex=%d DAB3_H_SelIndex=%d\n",nIndex,DAB3_H_SelIndex);
                    }
                }
}

bool DAB2_ListBar_H_Changed(ITUWidget* widget, char* param);
bool DAB2_ListBar_H_Changed(ITUWidget* widget, char* param)
{
     //page moving
    int nIndex = atoi(param);
    
    //현재 스크롤바의 위치를 계산해서, 현재 position을 구하는 방법이 있다.
	//첫번째 셀의 위치를 계산해서 현재 position을 구하는 방법이 있다.
	int count = 8;

    int nScroll_Index;

    int nCurSelected_Index;
    int nNewSelected_Index;
    int nMaxListIndex;

    if(g_bDAB2_DataMode)
    {
        nScroll_Index = nIndex-1; 
        nCurSelected_Index  = DAB2_H_SelIndex;
        nMaxListIndex       = DAB2ListCnt-1;
    }
    else
    {
        nScroll_Index = nIndex-1; 
        nCurSelected_Index  = DAB3_H_SelIndex;
        nMaxListIndex       = DAB3_H_ListCnt-1;
    }

    //bar자체를 터치했을 경우
    if(nIndex>=1000)
	{
		if(nCurSelected_Index<0 || nCurSelected_Index>nMaxListIndex)
		{
			//other->list focus
            if(nCurSelected_Index<0)
            {
                //(1)Deselect AUTO Button
                MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	            //(2)Make the jog signal to be passed to the scroll.
	            MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   
            }
            DAB2Combo_H_GotoPage(0);
            DAB2ListBoxH_Select(0);
			
            
		}
        return false;
	}

    if(nCurSelected_Index<0)
    {
        //(1)Deselect AUTO Button
        MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	    //(2)Make the jog signal to be passed to the scroll.
	    MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   
        
        DAB2Combo_H_GotoPage(0);
        DAB2ListBoxH_Select(0);

        return true;
    }
    else
    {

    }

    //printf("[DAB2_ListBar_Changed] nNewSelected_Index=%d nCurSelected_Index=%d\n",nNewSelected_Index,nCurSelected_Index);
    //==========================================================
	ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	
	int nCurrentPos = abs(pPageFirstItem->rect.y) / pPageFirstItem->rect.height;
	int nPrePosIndex;

    int nStartIndex_Page;
	int nEndIndex_Page;
    
	if(nCurrentPos<=0)
	{
		if(nCurSelected_Index%2==0)
        {
            //even
            nPrePosIndex = 0;
			
        }
        else
        {
            //odd
            nPrePosIndex = 1;
        }
	}
	else
	{
		if(nCurSelected_Index%2==0)
        {
            //even
            nPrePosIndex = nCurrentPos*2;
			
        }
        else
        {
            //odd
            nPrePosIndex = nCurrentPos*2+1;
        }
	}

	if(nCurSelected_Index%2==0)
    {
        //even
        nPrePosIndex = nCurrentPos*2;
		nStartIndex_Page = nScroll_Index*2;
		nEndIndex_Page   = nStartIndex_Page+(count-1)-1;
    }
    else
    {
        //odd
        nPrePosIndex = nCurrentPos*2+1;
		nStartIndex_Page = nScroll_Index*2+1;
		nEndIndex_Page   = nStartIndex_Page+(count-1)-1;
    }
	
	if(nStartIndex_Page>=nMaxListIndex)
	{
		nStartIndex_Page = nMaxListIndex;
	}
	if(nEndIndex_Page>=nMaxListIndex)
	{
		nEndIndex_Page = nMaxListIndex;
		//현재 짝수 행에 포커스가 있고, 마지막 인텍스가 홀수라면 1을 빼준다.
		if(nCurSelected_Index%2==0 && nEndIndex_Page%2==1)
		{
			nEndIndex_Page = nEndIndex_Page-1;
		}
		//현재 홀수 행에 포커스가 있고, 마지막 인덱스가 짝수라면 1을 빼준다.
		else if(nCurSelected_Index%2==1 && nEndIndex_Page%2==0)
		{
			nEndIndex_Page = nEndIndex_Page-1;
		}
	}

    int nPageIndex_Diff = nScroll_Index - nCurrentPos;
	int nIndex_Diff = nScroll_Index - nCurrentPos;
	int absDiff_Pos;

    if(nCurSelected_Index%2==0)
	{
		//even
		nIndex_Diff = nIndex_Diff*2;
	}
	else
	{
		//odd
		nIndex_Diff = nIndex_Diff*2+1;
	}

    //printf("[1]pPageFirstItem->rect.y=%d nScroll_Index=%d nPrePosIndex=%d nIndex_Diff=%d nCurrentPos=%d absDiff_Pos=%d\n",pPageFirstItem->rect.y,nScroll_Index,nPrePosIndex,nIndex_Diff,nCurrentPos,absDiff_Pos);

    if(nCurrentPos<nScroll_Index)
	{
		//down
		absDiff_Pos = nPageIndex_Diff*pPageFirstItem->rect.height;
		ituComboTableMoveXY(dab2_h_cbt, 0, -absDiff_Pos);
		nNewSelected_Index = nCurSelected_Index + nIndex_Diff;

		//최대치는 못넘어가게 만든다.
        DAB2Combo_H_SetMaxMoveLimit();

		//선택된 인덱스가 보이지 않는다면 최상단 셀을 선택한다.
		//if(nCurSelected_Index < nStartIndex_Page)
		{
			nNewSelected_Index = nStartIndex_Page;
			printf("[DAB2_ListBar_Changed]nNewSelected_Index=%d nCurSelected_Index=%d nStartIndex_Page=%d\n",nNewSelected_Index,nCurSelected_Index,nStartIndex_Page);
		}
	}
	else if(nCurrentPos>nScroll_Index)
	{
		//up
		absDiff_Pos = (nPageIndex_Diff)*pPageFirstItem->rect.height;
		ituComboTableMoveXY(dab2_h_cbt, 0, -absDiff_Pos);
		nNewSelected_Index = nNewSelected_Index - nIndex_Diff;

		//선택된 인덱스가 보이지 않는다면 최하단의 셀을 선택한다.
		//if(nCurSelected_Index > nEndIndex_Page)
		{
			nNewSelected_Index = nEndIndex_Page;
			printf("[DAB2_ListBar_H_Changed]nNewSelected_Index=%d nCurSelected_Index=%d nEndIndex_Page=%d\n",nNewSelected_Index,nCurSelected_Index,nEndIndex_Page);
		}
	}
	else
	{
		//same
        return false;
	}

	if(nNewSelected_Index>=0 && nNewSelected_Index<=nMaxListIndex)
	{
		//printf("[DAB2_ListBar_H_Changed]nNewSelected_Index=%d\n",nNewSelected_Index);
	}
	else
	{
		//printf("[DAB2_ListBar_H_Changed]nNewSelected_Index=%d\n",nNewSelected_Index);
		nNewSelected_Index = 0;
	}

    DAB2ListBoxH_Select(nNewSelected_Index);

    DAB2Combo_H_Set_ScrollBar(0);//chpark.24.07.11

    /*
    //page moving
    int nIndex = atoi(param);
    
    int nCurSelected_Index;
    int nNewSelected_Index;
    int nMaxListIndex;

    if(g_bDAB2_DataMode)
    {
        nCurSelected_Index  = DAB2_H_SelIndex;
        if(nCurSelected_Index%2==0)
        {
            //even
            nNewSelected_Index = (nIndex-1)*2;
        }
        else
        {
            //odd
            nNewSelected_Index = (nIndex-1)*2+1;
        }
        nMaxListIndex       = DAB2ListCnt-1;
    }
    else
    {
        nCurSelected_Index  = DAB3_H_SelIndex;
        if(nCurSelected_Index%2==0)
        {
            //even
            nNewSelected_Index = (nIndex-1)*2;
        }
        else
        {
            //odd
            nNewSelected_Index = (nIndex-1)*2+1;
        }
        nMaxListIndex       = DAB3_H_ListCnt-1;
    }

    //printf("[DAB2_ListBar_H_Changed] nNewSelected_Index=%d nCurSelected_Index=%d\n",nNewSelected_Index,nCurSelected_Index);

    if(nCurSelected_Index<0)
    {
        //(1)Deselect AUTO Button
        MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	    //(2)Make the jog signal to be passed to the scroll.
	    MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   
        
        DAB2Combo_H_GotoPage(0);
        DAB2ListBoxH_Select(0);
        //DAB2Combo_GotoPage(0);

        return true;
    }
    else
    {
        //터치 부분으로 선택한다.
        ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];
        //printf("[DAB2_ListBar_H_Changed] [1] pPageSelectItem->rect.y=%d dab2_h_cbt->selectIndex=%d\n",pPageSelectItem->rect.y, dab2_h_cbt->selectIndex);
        
        DAB2ListBoxH_Select(nNewSelected_Index);
       
        pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];
        //printf("[DAB2_ListBar_H_Changed] [2] pPageSelectItem->rect.y=%d dab2_h_cbt->selectIndex=%d\n",pPageSelectItem->rect.y, dab2_h_cbt->selectIndex);
    }

    DAB2Combo_H_Set_ScrollBar(0);//chpark.24.07.11
    */
    return true;
}



bool DAB2ListBoxH_Select_UpItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp-=2;
   
        if(nScroll_Tmp<0)
            nScroll_Tmp = DAB2_H_SelIndex;//chpark.24.03.25
            //nScroll_Tmp=0;
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp-=2;
   
        if(nScroll_Tmp<0)
            nScroll_Tmp = DAB3_H_SelIndex;//chpark.24.03.25
            //nScroll_Tmp=0;
    }

    //range error check  
    if(nScroll_Tmp<0)nScroll_Tmp=0;//chpark.24.03.25

    DAB2ListBoxH_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBoxH_Select_DownItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp+=2;
   
        if(nScroll_Tmp>=DAB2ListCnt)//chpark.24.03.25
            nScroll_Tmp = DAB2_H_SelIndex; //chpark.24.03.25

        //range error check    
        if(nScroll_Tmp>DAB2ListCnt)//chpark.24.03.25
            nScroll_Tmp=DAB2ListCnt-1;//chpark.24.03.25
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp+=2;
   
        if(nScroll_Tmp>=DAB3_H_ListCnt)//chpark.24.03.25
            nScroll_Tmp=DAB3_H_SelIndex;//chpark.24.03.25
        
        //range error check   
        if(nScroll_Tmp>DAB3_H_ListCnt)//chpark.24.03.25
            nScroll_Tmp=DAB3_H_ListCnt-1;//chpark.24.03.25
    }

    DAB2ListBoxH_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBoxH_Select_NextItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB3_H_ListCnt)
            nScroll_Tmp=DAB3_H_ListCnt-1;
    }

    DAB2ListBoxH_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBoxH_Select_PrevItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp--;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp--;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }

    DAB2ListBoxH_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBoxH_Select_LeftRightItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 

        if(nScroll_Tmp%2==0)
        {
            //even
            nScroll_Tmp++;
        }
        else
        {
            //odd
            nScroll_Tmp--;
        }
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        
        if(nScroll_Tmp%2==0)
        {
            //even
            nScroll_Tmp++;
        }
        else
        {
            //odd
            nScroll_Tmp--;
        }
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        if(nScroll_Tmp>=DAB3_H_ListCnt)
            nScroll_Tmp=DAB3_H_ListCnt-1;

    }

    DAB2ListBoxH_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBoxH_Select_NextPageItem()//chpark.24.03.05
{
    int nMovingCnt=0;

    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp+=8;
   
        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;

        nMovingCnt = nScroll_Tmp - DAB2_H_SelIndex;//chpark.24.03.05
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp+=8;
   
        if(nScroll_Tmp>=DAB3_H_ListCnt)
            nScroll_Tmp=DAB3_H_ListCnt-1;

        nMovingCnt = nScroll_Tmp - DAB3_H_SelIndex;//chpark.24.03.05
    }

    //chpark.24.03.05
    //DAB2ListBox_Select(nScroll_Tmp);
    if(nMovingCnt<=0)
    {
        //printf("[DAB2ListBoxH_Select_NextPageItem]nMovingPos=%d return false.\n",nMovingCnt);
        return false;
    }
    
    int nTmpIndex = nScroll_Tmp;
    int count = 8;
   
    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2_H_SelIndex==nTmpIndex)
        {
            return false;
        }

        //save last seleted index in combobox
        dab2_h_cbt->lastselectIndex = DAB2_H_SelIndex/2;

        g_DAB2_PreSelect_Index = DAB2_H_SelIndex;
        DAB2_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_h_cbt->selectIndex = DAB2_H_SelIndex/2;
    }
    else
    {
        //if same index->return
        if(DAB3_H_SelIndex==nTmpIndex)
        {
            return false;
        }
        //save last seleted index in combobox
        dab2_h_cbt->lastselectIndex = DAB3_H_SelIndex/2;

        g_DAB3_PreSelect_Index = DAB3_H_SelIndex;
        DAB3_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_h_cbt->selectIndex = DAB3_H_SelIndex/2;
    }

    //Call a function that automatically sets the focus.
    int y = ituWidgetGetHeight(dab2_h_container);
    int nMovingPos=0;

    ITUWidget* pPageFirstItem  = dab2_h_cbt->itemArr[0];
    //ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];

    if(nMovingCnt<8)
    {
        nMovingCnt = nMovingCnt/2;//chpark.24.03.06
    }
    else
    {
         nMovingCnt = 4;
    }

    
    nMovingPos = (pPageFirstItem->rect.height)*nMovingCnt;
    //printf("[DAB2ListBoxH_Select_NextPageItem]height:%d nMovingPos:%d nMovingCnt:%d\n",pPageFirstItem->rect.height,nMovingPos,nMovingCnt); 

    ituComboTableMoveXY(dab2_h_cbt, 0, -nMovingPos);

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBoxH_Select_NextPageItem]lastselectIndex=%d  selectIndex=%d\n",g_DAB2_PreSelect_Index,DAB2_H_SelIndex);
        DAB2_Combo_H_DrawItem(g_DAB2_PreSelect_Index); 
        DAB2_Combo_H_DrawItem(DAB2_H_SelIndex);
    }
    else
    {
        DAB3_Combo_H_DrawItem(g_DAB3_PreSelect_Index); 
        DAB3_Combo_H_DrawItem(DAB3_H_SelIndex);
    }
    //

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

bool DAB2ListBoxH_Select_PrevPageItem()//chpark.24.03.05
{
    int nMovingCnt=0;
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2_H_SelIndex; 
        nScroll_Tmp-=8;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        nMovingCnt = DAB2_H_SelIndex - nScroll_Tmp;//chpark.24.03.05
    }
    else
    {
        nScroll_Tmp = DAB3_H_SelIndex; 
        nScroll_Tmp-=8;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        nMovingCnt = DAB3_H_SelIndex - nScroll_Tmp;//chpark.24.03.05
    }

    if(nMovingCnt<=0)
    {
        printf("[DAB2ListBoxH_Select_PrevPageItem]nMovingPos=%d return false.\n",nMovingCnt);
        return false;
    }

    int nTmpIndex = nScroll_Tmp;
    int count = 8;
   
    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2_H_SelIndex==nTmpIndex)
        {
            return false;
        }

        //save last seleted index in combobox
        dab2_h_cbt->lastselectIndex = (DAB2_H_SelIndex)/2;

        g_DAB2_PreSelect_Index = DAB2_H_SelIndex;
        DAB2_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_h_cbt->selectIndex = (DAB2_H_SelIndex)/2;
    }
    else
    {
        //if same index->return
        if(DAB3_H_SelIndex==nTmpIndex)
        {
            return false;
        }
        //save last seleted index in combobox
        dab2_h_cbt->lastselectIndex = (DAB3_H_SelIndex)/2;

        g_DAB3_PreSelect_Index = DAB3_H_SelIndex;
        DAB3_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_h_cbt->selectIndex = (DAB3_H_SelIndex)/2;
    }

    //Call a function that automatically sets the focus.
    int y = ituWidgetGetHeight(dab2_h_container);
    int nMovingPos=0;

    ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
    //ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];

    if(nMovingCnt<8)
    {
        nMovingCnt = 0;
    }
    else
    {
         nMovingCnt = 4;
    }
    
    nMovingPos = (pPageFirstItem->rect.height)*nMovingCnt;
    //printf("[DAB2ListBoxH_Select_PrevPageItem]height:%d nMovingPos:%d nMovingCnt:%d\n",pPageFirstItem->rect.height,nMovingPos,nMovingCnt); 

    ituComboTableMoveXY(dab2_h_cbt, 0, nMovingPos);

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBoxH_Select_PrevPageItem]lastselectIndex=%d  selectIndex=%d\n",g_DAB2_PreSelect_Index,DAB2_H_SelIndex);
        DAB2_Combo_H_DrawItem(g_DAB2_PreSelect_Index); 
        DAB2_Combo_H_DrawItem(DAB2_H_SelIndex);
    }
    else
    {
        DAB3_Combo_H_DrawItem(g_DAB3_PreSelect_Index); 
        DAB3_Combo_H_DrawItem(DAB3_H_SelIndex);
    }
    //

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}


//Select item in List(DAB2 & DAB3)
bool DAB2ListBoxH_SetPTY(int nIndex)//chpark.11.16
{
    //if no service channel is selected, remove Current PTY.
    //chpark.10.16
    if(nIndex<0)
    {
       printf("[DAB2ListBoxH_SetPTY] Error nIndex=%d\n",nIndex);
       return false;
    }

    int nTmpIndex = nIndex;

    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
    int count = 8; //chpark.24.03.27
    int nPageIndex  = (nTmpIndex/2)/count+1;
    int nFocusIndex = (nTmpIndex/2)%count;

    //Call a function that automatically sets the focus 
    //after loading the page.

    if((m_bDAB2_H_ListInit && m_bDAB2R_H_ListInit) || DAB2_m_b_H_FirstPTY)//chpark.09.26
    {
        //chpark.09.26
        if(RDV.DAB2_PTYNo==nTmpIndex && !DAB2_m_b_H_FirstPTY)
        {
            //printf("[DAB2ListBoxH_SetPTY][Fail] not first...RDV.DAB2_PTYNo=%d nTmpIndex=%d DAB2_m_b_H_FirstPTY==%d \n",RDV.DAB2_PTYNo, nTmpIndex ,DAB2_m_b_H_FirstPTY);
            return false;
        }

        RDV.DAB2_PTYNo = nTmpIndex;

        

        //chpark.09.25
        if(m_bDAB2_H_Scroll_Touched)
        {
            //printf("==DAB2ListBoxH_SetPTY==m_bDAB2_H_Scroll_Touched==%d \n",m_bDAB2_H_Scroll_Touched);
            return false;
        }

        //
        //DAB2Combo_H_GotoPageTop(nIndex);//chpark.01.03//chpark.24.03.27
        //
        
        //printf("\n\n DAB2ListBoxH_SetPTY[SUCCESS]:nIndex=%d  nPageIndex=%d nFocusIndex=%d RDV.DAB2_PTYNo=%d\n",nIndex,nPageIndex,nFocusIndex,RDV.DAB2_PTYNo);
    }
    else
    {
        printf("[DAB2ListBoxH_SetPTY]==ETC Error\n");
        printf("[DAB2ListBoxH_SetPTY]m_bDAB2_H_ListInit=%d  m_bDAB2R_H_ListInit=%d DAB2_m_b_H_FirstPTY=%d\n",m_bDAB2_H_ListInit,m_bDAB2R_H_ListInit,DAB2_m_b_H_FirstPTY);
    }

    return true;
}

//Focus at current page.
//Draw Red Box
bool DAB2ListBoxH_Select(int nIndex)
{
    if(nIndex==-1)
    {
        //printf("[V-mode]DAB2ListBoxH_Select Index=%d RDV.DAB2_PTYNo=%d RDV.DAB3_PTYNo%d\n",nIndex,RDV.DAB2_PTYNo,RDV.DAB3_PTYNo);

        if(g_bDAB2_DataMode)
        {
            if(DAB2_H_SelIndex==-1)return false;//already no focus
            g_DAB2_PreSelect_Index = -1;//chpark.12.11
            DAB2_H_LastSelIndex = DAB2_H_SelIndex;//chpark.01.05
            DAB2_H_SelIndex = -1;
            DAB2_Combo_H_DrawItem(DAB2_H_LastSelIndex); //chpark.01.05
        }
        else
        {
            if(DAB3_H_SelIndex==-1)return false;//already no focus
            g_DAB3_PreSelect_Index = -1;//chpark.12.11
            DAB3_H_LastSelIndex = DAB3_H_SelIndex;//chpark.01.05
            DAB3_H_SelIndex = -1;
            DAB3_Combo_H_DrawItem(DAB3_H_LastSelIndex); //chpark.01.05
        }

        //int nCurrentPageIndex = DAB2ListBox->listbox.pageIndex;//chpark.24.03.27
        //printf("\nDAB2ListBoxH_Select(-1) %d %d==============\n\n",g_DAB2_PreSelect_Index,DAB3_H_SelIndex);

        return false;
    }
    else if(nIndex==3333)
    {
        //set Focus on PTY position.
        //Set to 0 if there is no  PTY.
        if(g_bDAB2_DataMode)
        {
            nIndex = RDV.PTYNo;
            if(nIndex<0)//chpark.11.15
                nIndex=0;

        }
        else
        {
            nIndex = DAB3_H_SelIndex;
            if(nIndex==-1)
                nIndex=0;
        }
    }

    int nTmpIndex = nIndex;

    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
   
     int count = 8;//chpark.24.0327
    int nPageIndex  = (nTmpIndex/2)/count+1;
    int nFocusIndex = (nTmpIndex/2)%count;

    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2_H_SelIndex==nTmpIndex)
        {
            //printf("[DAB2ListBoxH_Select] the same index:return false\n");
            return false;
        }

        //save index
        DAB2_H_LastSelIndex = DAB2_H_SelIndex;
        g_DAB2_PreSelect_Index = nTmpIndex;//chpark.11.15
        DAB2_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        if(DAB2_H_LastSelIndex<0)
            dab2_h_cbt->lastselectIndex = -1;
        else
            dab2_h_cbt->lastselectIndex = DAB2_H_LastSelIndex/2;
        
        dab2_h_cbt->selectIndex = DAB2_H_SelIndex/2;//chpark.01.03
    }
    else
    {
         //if same index->return
        if(DAB3_H_SelIndex==nTmpIndex)
        {
            //printf("[DAB2ListBoxH_Select] the same index:reture false\n");
            return false;
        }

        //save index
        DAB3_H_LastSelIndex = DAB3_H_SelIndex;
        g_DAB3_PreSelect_Index = nTmpIndex;//chpark.12.05
        DAB3_H_SelIndex = nTmpIndex;

        //save new seleted index in combobox
        if(DAB3_H_LastSelIndex<0)
            dab2_h_cbt->lastselectIndex = -1;
        else
            dab2_h_cbt->lastselectIndex = DAB3_H_LastSelIndex/2;

        dab2_h_cbt->selectIndex = DAB3_H_SelIndex/2;//chpark.01.03
    }


    //Call a function that automatically sets the focus 
    //after loading the page.  
    
    //chpark.01.03
    int y = ituWidgetGetHeight(dab2_h_container);
    int dy=0;
    dy = y / 8;

    ITUWidget* pPageFirstItem = dab2_h_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_h_cbt->itemArr[dab2_h_cbt->selectIndex];
    //printf("[DAB2ListBoxH_Select]pPageSelectItem->rect.y:%d\n",pPageSelectItem->rect.y); 
    dy = pPageFirstItem->rect.height;

    //Make sure it's the same page.
    bool bCase1=false;
    bool bCase2=false;
    if(g_bDAB2_DataMode && (DAB2_H_SelIndex>DAB2_H_LastSelIndex))bCase1 =true;
    if(!g_bDAB2_DataMode && (DAB3_H_SelIndex>DAB3_H_LastSelIndex))bCase2=true;

    //if(bCase1 || bCase2)
    if(dab2_h_cbt->selectIndex>dab2_h_cbt->lastselectIndex)
    {
        //when index up
        if(nTmpIndex<8)
        {
            //dy = 0;
            dy =  pPageFirstItem->rect.height;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }
        
        if( pPageSelectItem->rect.y>=((5-1)*97) )
        {
            //move to next line(+ 1 line height & diff)
            //If it does not fit the exact line, adjust within the diff( + 1 line & diff).
            int nLast_Diff=0;
            nLast_Diff = 5*97 - pPageSelectItem->rect.y;
            if(nLast_Diff<97)
            {
                ituComboTableMoveXY(dab2_h_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_h_cbt, 0, -dy);
            }
        }

        ituWidgetUpdate(dab2_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
        
        //if(g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][UP] DAB2_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB2_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
        //if(!g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][UP] DAB3_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB3_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
    else if(dab2_h_cbt->selectIndex==dab2_h_cbt->lastselectIndex)
    {
        //if(g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][SAME] DAB2_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB2_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
        //if(!g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][SAME] DAB2_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB3_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
    else
    {
        //when index down
        if(nTmpIndex<8 && pPageSelectItem->rect.y<(4)*97)
        {
            dy = pPageFirstItem->rect.height;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }
        
        if(pPageSelectItem->rect.y<0  )
        {
            int nLast_Diff=0;
            nLast_Diff = abs(97 + pPageSelectItem->rect.y);
            if(nLast_Diff<97)
	        {
                ituComboTableMoveXY(dab2_h_cbt, 0, dy-nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_h_cbt, 0, dy);
            }
        }

        ituWidgetUpdate(dab2_h_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);

       //if(g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][DN] DAB2_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB2_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
       //if(!g_bDAB2_DataMode)printf("[DAB2ListBoxH_Select][DN] DAB2_H_SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB3_H_SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);

    }

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBox_Select]DAB2_H_LastSelIndex=%d  DAB2_H_SelIndex=%d\n",DAB2_H_LastSelIndex,DAB2_H_SelIndex);
        DAB2_Combo_H_DrawItem(DAB2_H_LastSelIndex); 
        DAB2_Combo_H_DrawItem(DAB2_H_SelIndex);
    }
    else
    {
        DAB3_Combo_H_DrawItem(DAB3_H_LastSelIndex); 
        DAB3_Combo_H_DrawItem(DAB3_H_SelIndex);
    }
    //
    DAB2Combo_H_Set_ScrollBar(0);//chpark.24.08.21
    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}


//=========================================================================================
void DAB2ListBoxH_ModeChange(int nDAB_MODE, bool bInfoReq)//chpark.11.27
{
    if(nDAB_MODE==MAIN_MODE_DAB3)
    {
        //DAB2->DAB3
        //Select Category
        if(DAB2_H_SelIndex>=0)
        {
            if(RDV.DABServiceCnt[DAB2_H_SelIndex]==0)
            {
               printf("No Service Channel:%d\n",RDV.DABServiceCnt[DAB2_H_SelIndex]);
            }
            else
            {
                //memset(DAB3PTYNo, 0, sizeof(DAB3PTYNo[0][0]) * 500 * 80);//chpark.09.21//chpark.11.15
                //printf("ReLoading Chaanel Data In DAB3 Mode.%d\n",RDV.DABServiceCnt[DAB2_H_SelIndex]);
                DAB2Combo_H_GetComboPos();

                //chpark.01.03
                //move to first page
                DAB2Combo_H_GotoPage(0);
                dab2_h_cbt->selectIndex = -1;
                dab2_h_cbt->lastselectIndex = -1;

                //DAB2->DAB3 
                g_bDAB2_DataMode  = false;
                m_bDAB2_H_ListInit  = false;
                m_bDAB2R_H_ListInit = false;
                
                if(bInfoReq)//chpark.11.27
                {
                    //printf("====================RESET DAB3 DATA===========\n");//chpark.11.15
                    memset(DAB3PTYNo, 0, sizeof(DAB3PTYNo[0][0]) * 500 * 80);//chpark.09.21//chpark.11.15
                    m_nDAB3_H_SeviceChCnt = RDV.DABServiceCnt[DAB2_H_SelIndex];

                    //MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);     //chpark.12.05 
                    MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);//chpark.24.11.11
                    
                    //Set First Index.
                    DAB3_H_SelIndex = -1;//chpark.06.21
                    g_DAB3_PreSelect_Index = -1;//chpark.24.02.29
                    RDV.DAB3_PTYNo = 0;

                    DAB3_H_ListInit();
                }
                else
                {
                    DAB2_H_SelIndex = g_DAB2_PreSelect_Index;//chpark/11.13
                    m_nDAB3_H_SeviceChCnt = RDV.DABServiceCnt[DAB2_H_SelIndex];//chpark.11.15
                    DAB3_H_SelIndex = g_DAB3_PreSelect_Index;//chpark/12.05

                    //printf("=========Rotation DAB3 modechange: DAB3_H_SelIndex:%d\n",DAB3_H_SelIndex);//chpark.11.15

                    DAB3_H_ListInit();
                    
                    RDV.DAB3_PTYNo = 0;
                }
             
                //Set Channel Title
                char strTmp[50] = {0x20,};
                sprintf(strTmp, "%s",DAB2ListArray[DAB2_H_SelIndex].name);//chpark.06.03
                DAB2_SetCategory(strTmp);

                Main_Disp_Mode = MAIN_MODE_DAB3; //chpark.24.01.29

                //printf("Modify===\n\n\n");
                if(bInfoReq)//chpark.11.27
                {
                    printf("request DAB3 info===\n\n\n");
                    Main_CLI3[0] = DAB2_H_SelIndex;		//	chpark 2023-09-21
				    Main_CLI3[4] = Main_CLI3[4]|0x02; 	//	chpark 2023-09-21
                }
                else
                {
                    //printf("no request DAB3 info===\n\n\n");
                }
            }
        }
        else
        {
             //printf("[List:R]MouseUp==SelIndex Error:[%d]\n",DAB2_H_SelIndex);
        }

        DAB2Combo_H_Set_ScrollBar(0);//chpark.24.02.29
    }
    else
    {

        //chpark.01.03
        //move to first page
        DAB2Combo_H_GotoPage(0);
        dab2_h_cbt->selectIndex = -1;
        dab2_h_cbt->lastselectIndex = -1;
        //

        //In DAB3 MODE
        //Select Channel 
        uint8_t nRealIndex= DAB3ListArray[DAB3_H_SelIndex].nt_ListNum;

        //chpark.10.04
        int nTmpCurrentNumChar = strlen(DAB3PTYNo[nRealIndex]);
        if(nTmpCurrentNumChar<=0)
        {
            //printf("nTmpCurrentNumChar=%d nRealIndex=%d\n",nTmpCurrentNumChar,nRealIndex);
            //return;
            //printf("\n\n[DAB3 Selected Index]:Empty String===\n\n",nRealIndex);//chpark.12.05
        }
        //

        //printf("\n\n[DAB3 Selected Index]:%d===\n\n",nRealIndex);

        Main_CLI3[4] = Main_CLI3[4]|0x01;			//	JPHong 2023-07-14
        Main_CLI3[6] = (nRealIndex&0xFF00)>>8; 	//	chpark 2023-09-21
        Main_CLI3[5] = nRealIndex&0xFF;			//	chpark 2023-09-21

        Main_Disp_Mode = MAIN_MODE_DAB2; //chpark.24.01.29
        DAB2_Set_JD_Button(RD_SEL_AUTO);//chpark.09.11
    }

}

//=========================================================
//chpark.07.18
bool DAB3ListBoxH_Reflesh()
{
    //DAB2Combo_H_Set_ScrollBar(0);
    int nTmpDAB3_ListNumIndex = 0; 
    int count = 4;

    if(g_bDAB2_DataMode)
    {
        if(g_nDAB2_Data_LastRecvTime==DAB2_DataChkTime)
        {
            //New data.
            //printf("==[Hmode]No New Data== Time:%d \n",g_nDAB2_Data_LastRecvTime);
            return false;
        }
        else
        {
            //printf("==[Hmode]New Data RCV== Time:%d \n",g_nDAB2_Data_LastRecvTime);
            DAB2_H_ListInit();

            //New data received.
            g_nDAB2_Data_LastRecvTime = DAB2_DataChkTime;
        }

        return false;
    }

  
    if(g_nDAB3_Data_LastRecvTime==DAB3_DataChkTime)
    {
        //New data.
        //printf("==No New Data== Time:%d \n",g_nDAB3_Data_LastRecvTime);
        return false;
    }
    else
    {
        //New data received.
        g_nDAB3_Data_LastRecvTime = DAB3_DataChkTime;
    }
    
    
    //chpark.10.16
    if(DAB3_H_ListCnt_withText>=m_nDAB3_H_SeviceChCnt)//find out receive all item
    {
        //If there are more than the number of service channels, no longer receive it.
        //printf("ChannelInfo Over[H Mode]:CH_Count:%d List:%d \n",m_nDAB3_H_SeviceChCnt,DAB3_H_ListCnt);
        //printf("ChannelInfo Over[H Mode]:m_bDAB2_H_ListInit:%d m_bDAB2R_H_ListInit:%d \n",m_bDAB2_H_ListInit,m_bDAB2R_H_ListInit);
        //return false;//chpark.12.04
    }
    
    if(m_bDAB2_H_ListInit && m_bDAB2R_H_ListInit)//chpark.12.05
    {
        //no focus
        //printf("[DAB3ListBoxH_Reflesh]m_bDAB2_H_ListInit:%d  m_bDAB2R_H_ListInit:%d ==========\n",m_bDAB2_H_ListInit,m_bDAB2R_H_ListInit);

        DAB3_H_ListInit();
        //for 30 second 
        Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18
        //
    }
    else if(!m_bDAB2_H_ListInit)
    {
        //printf("Dont ==Refresh List==not init\n");
        return false;
    }
    else
    {
        //printf("Dont ==Refresh List==etc\n");
        return false;
    }
    
	return true;
}


//===========================================================
