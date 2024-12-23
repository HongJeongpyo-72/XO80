
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"
#include "layer_main_FW.h"

static int m_nDAB3_SeviceChCnt=0; //chpark.09.25
//=====================================================================
int  g_DAB2_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.
int  g_DAB3_PreSelect_POS; //chpark.24.03.01  V<->H Mode change.

static ITUScrollBar* DAB2ListBar;

static ITUWidget* DAB2_ListBar_Back; //chpark.09.11
static ITUWidget* DAB2_ListBar_Bar;

uint32_t g_nDAB2_Data_LastRecvTime=0;   //chpark.12.05
uint32_t g_nDAB3_Data_LastRecvTime=0;   //chpark.12.05

static bool m_bDAB2_Scroll_Touched = false;//chpark.07.18

int g_DAB2_PreSelect_Index;//chpark.11.15
int g_DAB3_PreSelect_Index;//chpark.12.05

static bool m_bDAB2_ListInit = false;

static int nScroll_Tmp =0;
//=====================================================================
bool JogDialKey_Scroll_DAB2(char* param);//chpark.06.21
bool JogDialRot_Scroll_DAB2(char* param);//chpark.06.21
//=====================================================================
extern bool MainMode_RD_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff);//chpark.06.21
extern void DAB2_SetCategory(char* strTitle);//chpark.06.03
//=====================================================================
static void DAB2ListInit(void);
static void DAB3ListInit(void);
//=====================================================================
bool DAB2ListBox_Select(int nIndex);
bool DAB2ListBox_SetPTY(int nIndex);//chpark.06.21
//=====================================================================
void DAB2ListBox_ModeChange(int nDAB_MODE, bool bInfoReq);//chpark.11.27
//=====================================================================
bool DAB2ListBox_Select_UpItem();
bool DAB2ListBox_Select_DownItem();
bool DAB2ListBox_Select_NextItem();
bool DAB2ListBox_Select_PrevItem();
bool DAB2ListBox_Select_NextPageItem();
bool DAB2ListBox_Select_PrevPageItem();
//=====================================================================
bool DAB3ListBox_Reflesh();//chpark.07.18
//=====================================================================
static char widgetName[30];


char DAB3PTYNo[MAX_DAB3LIST_COUNT][MAX_DAB3LIST_CHAR];
char DAB3PTYNo_LIMIT[MAX_DAB3LIST_COUNT][MAX_DAB3LIST_CHAR_LIMIT];

typedef struct
{
    bool selected;
    char* name;
    int  nt_ListNum;//chpark.07.17
} DAB3ListEntry;

static int DAB3SelIndex = -1;//chpark.06.21

static int DAB3ListCnt; //text or blank(total)
static int DAB3ListCnt_withText;//text only //chpark.10.16
static DAB3ListEntry DAB3ListArray[MAX_DAB3LIST_COUNT];

typedef struct
{
    bool selected;
    bool PTY_selected;
    bool Focus_selected;
    const char* name;
} DAB2ListEntry;

static int DAB2SelIndex=-1; //chpark.06.21
static int DAB2ListCnt;
static DAB2ListEntry DAB2ListArray[MAX_DAB2LIST_COUNT];

bool DAB2_m_bFirstPTY = true;//chpark.09.26

//chpark.01.03
static int DAB2_LastSelIndex=-1; //chpark.
static int DAB3_LastSelIndex=-1; //chpark.

static ITUComboTable* dab2_cbt = NULL;
static ITUText* dab2_text = NULL;
static ITUContainer* dab2_container = NULL;

void DAB2Combo_SetMaxMoveLimit();//chpark.24.05.22
void DAB2Combo_SetSlideEnd_AutoFocus();//chpark.24.06.28

bool DAB2Combo_PageChanged(ITUWidget* widget, char* param);
bool DAB2Combo_MouseUp(ITUWidget* widget, char* param);
bool DAB2Combo_Select(ITUWidget* widget, char* param);
bool DAB2Combo_Button_Up(ITUWidget* widget, char* param);

void DAB2_Combo_DrawItem(int nIndex);
void DAB3_Combo_DrawItem(int nIndex);

void DAB2Combo_GotoPage(int nIndex);    //start at 0 page
void DAB2Combo_GotoPageTop(int nIndex); //start at 0 index

void DAB2Combo_Set_ScrollBar(int nIndex);//chpark.24.02.29

void DAB2Combo_GetComboPos();
void DAB2Combo_SetComboPos();

static bool b_DAB2_ComboSelected = false;

bool MainOnDAB2(bool bReload)
{
   
    printf("\n\n[MainOnDAB2]START :  RDV.PTYNo:[%d]=======\n", RDV.PTYNo);
    //chpark.01.03
    SetWidget(dab2_cbt,"DAB2Combo");
    SetWidget(dab2_container,"DAB2Combo_Cont");

	int j,i=0;
    m_bDAB2_ListInit = false;//chpark.11.16

	SetWidget(DAB2ListBar,"DAB2ListBar");

    SetWidget(DAB2_ListBar_Back,"DAB2_ListBar_Back");//chpark.09.11
    SetWidget(DAB2_ListBar_Bar, "DAB2_ListBar_Bar"); //chpark.09.11

    //===================================
    //chpark.09.26
    if(bReload)
    {
        //=============================
        // need to reset (H<->V mode change)
        //chpark.2024.03.04
        if(DAB2SelIndex != g_DAB2_PreSelect_Index)
        {
            g_DAB2_PreSelect_POS = 0;
        }
        //=============================
        DAB2SelIndex = g_DAB2_PreSelect_Index;
        DAB3SelIndex = g_DAB3_PreSelect_Index;//chpark.12.05

        if(g_bDAB2_DataMode)
        {
            DAB2ListInit();
        }
        else
        {
            m_nDAB3_SeviceChCnt = RDV.DABServiceCnt[DAB2SelIndex];//chpark.12.05
            for (i = 0; i < MAX_DAB2LIST_COUNT; i++)//chpark.24.04.22
            {
                DAB2ListArray[i].selected = false;
                DAB2ListArray[i].name = DAB2_PTY_No[i];
            }

            DAB3ListInit();
        }
    }
    //====================================
    //chpark.01.03
    DAB2Combo_GotoPage(0);

    m_bDAB2_ListInit = true;//chpark.24.03.13

	return true;
}

void DAB2Combo_GetComboPos()
{
    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    g_DAB2_PreSelect_POS = pPageFirstItem->rect.y;//first item postion
    //printf("[DAB2_Combo_GetComboPos]g_DAB2_PreSelect_POS=%d\n",g_DAB2_PreSelect_POS);
}

void DAB2Combo_SetComboPos()
{
    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    int nFirstItemPos = pPageFirstItem->rect.y;//current first item postion
    ituComboTableMoveXY(dab2_cbt, 0, -nFirstItemPos);//move to zero position

    ituComboTableMoveXY(dab2_cbt, 0, g_DAB2_PreSelect_POS);//move to Pre Position
    //printf("[DAB2_Combo_SetComboPos]g_DAB2_PreSelect_POS=%d\n",g_DAB2_PreSelect_POS);
}

void DAB2Combo_Set_ScrollBar(int nIndex)
{
    //chpark.24.08.26
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
        nTotalUserCnt = m_nDAB3_SeviceChCnt;
    }

	ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalUserCnt);
	int    nScroll_Pos = round(abs(first_item_pos)*(1.0)/first_item_height); //일의 자리까지 반올림

	//printf("[SU100_Set_ScrollBar]first_item_pos=%d first_item_height=%d fMovingRate=%f\n",first_item_pos,first_item_height,fMovingRate);
	int nBar_BackHeight = ituWidgetGetHeight(DAB2_ListBar_Back);
	int nBar_Height; 

	int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar, nBar_Height);
		ituWidgetSetY(DAB2_ListBar_Bar,  nStart_Margin);//chpark.24.03.28

		return;
	}
	else
	{
        nTotalRow = nTotalUserCnt;
	
        nBar_Height = (nBar_BackHeight/nTotalRow)*count;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar, nBar_Height);
	}

	int nBar_BackStartPosY  = ituWidgetGetY(DAB2_ListBar_Back);
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

	ituWidgetSetY(DAB2_ListBar_Bar,  nBar_NewPosY);
	ituScrollBarSetPosition(DAB2ListBar,  nScroll_Pos+1);//chpark.24.08.06//주의
	
	//printf("[DAB2Combo_Set_ScrollBar]nScroll_Pos=%d first_item_pos=%d fMovingRate=%f nBar_Height=%d nBar_MovingPosY=%d nBar_NewPosY=%d \n",nScroll_Pos, first_item_pos,fMovingRate,nBar_Height,nBar_MovingPosY,nBar_NewPosY);

    /*
    //chpark.24.07.11
	int count = 7;
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
        nTotalUserCnt = m_nDAB3_SeviceChCnt;
    }

	//ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
	//int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	//int  first_item_height   = pPageFirstItem->rect.height;
	//float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalUserCnt);
    //float  fMovingRate = abs(first_item_pos)*(1.0)/(first_item_height*nTotalUserCnt);
	int nBar_BackHeight = ituWidgetGetHeight(DAB2_ListBar_Back);
	int nBar_Height; 
    int nStart_Margin = 2;//start line //chpark.24.03.28 

	if(nTotalUserCnt<=count)
	{
		nBar_Height = nBar_BackHeight;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar, nBar_Height);
		ituWidgetSetY(DAB2_ListBar_Bar,  0);
		return;
	}
	else
	{
		nBar_Height = (nBar_BackHeight/nTotalUserCnt)*count;
        if(nBar_Height<=2)nBar_Height=2;
		//(1)set scrollbar size
		ituWidgetSetHeight(DAB2_ListBar_Bar, nBar_Height);
	}

    float  fMovingRate = abs(nBar_BackHeight-nBar_Height)*(1.0)/(nTotalUserCnt-1);


	int nBar_BackStartPosY  = ituWidgetGetY(DAB2_ListBar_Back);
	//int nBar_MovingPosY 	= nBar_BackHeight*fMovingRate; //nBar_BackHeight*fMovingRate;
	int nBar_NewPosY;
    int nCurSelected_Index;

    if(g_bDAB2_DataMode)
    {
        nCurSelected_Index  = DAB2SelIndex;
    }
    else
    {
        nCurSelected_Index  = DAB3SelIndex;
    }

    if(nCurSelected_Index<0)
    {
        nBar_NewPosY = nBar_BackStartPosY;
    }
    else
    {
        nBar_NewPosY = nBar_BackStartPosY + fMovingRate*nCurSelected_Index;
    }
    
	//final pos check
	int nBar_FinalPos = nBar_NewPosY + nBar_Height;
    int nBar_MaxPos   = nBar_BackStartPosY + nBar_BackHeight;
	if(nBar_FinalPos >= nBar_MaxPos)
	{
		nBar_NewPosY = nBar_BackStartPosY + nBar_BackHeight - nBar_Height - nStart_Margin;
        //printf("[MAX]nBar_FinalPos=%d nBar_MaxPos=%d nBar_NewPosY=%d fMovingRate=%d\n",nBar_FinalPos,nBar_MaxPos,nBar_NewPosY,fMovingRate);
	}
    else
    {
        //printf("[not MAX]nBar_FinalPos=%d nBar_MaxPos=%d nBar_NewPosY=%d fMovingRate=%d\n",nBar_FinalPos,nBar_MaxPos,nBar_NewPosY,fMovingRate);
    }

	//(2)set scrollbar position
	ituWidgetSetY(DAB2_ListBar_Bar,  nBar_NewPosY);
    */

}



//==========================================================
//chpark.01.03
bool DAB2Combo_PageChanged(ITUWidget* widget, char* param)
{
    int nTouchIndex = atoi(param);
    if (dab2_cbt)
	{
		int vPage = ituComboTableGetCurrentPageNumber(dab2_cbt);
		char vStr[10] = "";
		//ituTextSetString(text, vStr);
        //printf("[DAB2Combo_PageChanged]PageNum:%d ===== %d\n",vPage,atoi(param),nTouchIndex);
		//sprintf(vStr, "%d", vPage);
	}
    else
    {
        //printf("[DAB2Combo_PageChanged]dab2_cbt is NULL Error====\n");
    }

    if(nTouchIndex==1000 || nTouchIndex==2000)
    {
        //adjust button
        ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
        //chpark.24.04.03
        ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];

        int  first_item_pos       = pPageFirstItem->rect.y;//first item postion
        int  re_reitem_pos =  round(first_item_pos/97)*97 - first_item_pos;
        
        //printf("[DAB2Combo_PageChanged]==== first_item_pos:%d re_reitem_pos:(1):%d\n",first_item_pos,re_reitem_pos);
       
        ituComboTableMoveXY(dab2_cbt, 0, re_reitem_pos);//move to zero position
        
        //
        int  dy =  pPageFirstItem->rect.height;

        if( pPageSelectItem->rect.y>((8-1)*97) )
        {
            int nLast_Diff=0;
            nLast_Diff = (8*97 - pPageSelectItem->rect.y);
            //printf("[DAB2Combo_PageChanged]==== pPageSelectItem->rect.y:%d nLast_Diff:%d -dy+nLast_Diff:(2):%d\n",pPageSelectItem->rect.y,nLast_Diff,-dy+nLast_Diff);
            if(abs(nLast_Diff)<97)
            {
                ituComboTableMoveXY(dab2_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_cbt, 0, -dy);
            }
        }
        
        DAB2Combo_SetMaxMoveLimit();
        //       
        //printf("[DAB2Combo_PageChanged]==== first_item_pos:%d re_reitem_pos:(2):%d\n",first_item_pos,re_reitem_pos);
        //chpark.24.06.27
		if(nTouchIndex==2000)
		{
			DAB2Combo_SetSlideEnd_AutoFocus();
		}
       
    }

    m_bDAB2_Scroll_Touched = false;
    
    DAB2Combo_Set_ScrollBar(0);//chpark.24.08.06

    printf("[DAB2Combo_PageChanged]m_bDAB2_Scroll_Touched:%d====\n",m_bDAB2_Scroll_Touched);

    return true;
}


//chpark.24.05.22
void DAB2Combo_SetMaxMoveLimit()
{
    //Max Limit
    int	nITU_Combo_nTop =  (-1)*(dab2_cbt->itemArr[0]->rect.height)*(dab2_cbt->itemcount-8);
    if(dab2_cbt->itemcount>8 && dab2_cbt->itemArr[0]->rect.y<=nITU_Combo_nTop)
    {
        int nLast_Diff = nITU_Combo_nTop - (dab2_cbt->itemArr[0]->rect.y);
        ituComboTableMoveXY(dab2_cbt, 0, abs(nLast_Diff));//move to re position
        printf("[DAB2Combo_SetMaxMoveLimit]====%d dab2_cbt->itemArr[0]->rect.y=%d\n",abs(nLast_Diff),dab2_cbt->itemArr[0]->rect.y);
    }
}

//chpark.24.06.28
void DAB2Combo_SetSlideEnd_AutoFocus()
{
    ITUWidget* pPageFirstItem  = dab2_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];

	if( pPageSelectItem->rect.y>=(8*97) )
	{
		//down sliding-> bottom auto focus
		int nIndex = abs(pPageFirstItem->rect.y)/97+8-1;
		if(nIndex>=dab2_cbt->itemcount)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = dab2_cbt->itemcount-1;
		}
		else
		{
			printf("[DAB2Combo_SetSlideEnd_AutoFocus]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);				
		}
					
		DAB2ListBox_Select(nIndex);
	}
	else if(pPageSelectItem->rect.y<0)
	{
		//up sliding-> top auto focus
		int nIndex;
		
		nIndex= abs(pPageFirstItem->rect.y)/97;
		
		if(nIndex>=dab2_cbt->itemcount || nIndex<=0)
		{
			printf("Index Error !!\n",nIndex);
			nIndex = 0;
		}
		else
		{
			printf("[DAB2Combo_SetSlideEnd_AutoFocus][2]pPageSelectItem->rect.y=%d nIndex=%d\n",pPageSelectItem->rect.y,nIndex);
						
		}

		DAB2ListBox_Select(nIndex);
	}
	else
	{
		printf("[DAB2Combo_SetSlideEnd_AutoFocus]pPageSelectItem->rect.y=%d\n",pPageSelectItem->rect.y);
	}
}


//chpark.01.03
//By modifying the sdk,
//Mouse up actually functions as a mouse down.
bool DAB2Combo_MouseUp(ITUWidget* widget, char* param)
{
    int nTouchIndex;   
    nTouchIndex = atoi(param);

    //printf("[DAB2Combo_MouseUp(Down)][1]nTouchIndex:%d  selectIndex: %d====\n",nTouchIndex,dab2_cbt->selectIndex);

    //(1)Deselect AUTO Button
    MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	//(2)Make the jog signal to be passed to the scroll.
	MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   

    //(3)Save current index
    if(g_bDAB2_DataMode)
    {
        //Selectindex is already processed within the combo table.
        DAB2SelIndex = dab2_cbt->selectIndex;
        g_DAB2_PreSelect_Index = DAB2SelIndex;
    }
    else
    {
        //Selectindex is already processed within the combo table.
        int nTmpSelecIdx;
        nTmpSelecIdx = dab2_cbt->selectIndex;
        if(nTmpSelecIdx>=m_nDAB3_SeviceChCnt)
        {
            dab2_cbt->selectIndex = dab2_cbt->lastselectIndex;
            return false;;
        }

        DAB3SelIndex = dab2_cbt->selectIndex;
        g_DAB3_PreSelect_Index = DAB3SelIndex;
    }

    //(4)Redraw Last selected index
    ITUBackground* item_bg = NULL;
    if (dab2_cbt->lastselectIndex >= 0)
    {
        if(g_bDAB2_DataMode)
        {
            DAB2_Combo_DrawItem(dab2_cbt->lastselectIndex);
        }
        else
        {
            DAB3_Combo_DrawItem(dab2_cbt->lastselectIndex);
        }
    }

    //(5)Redraw Current selected index
    if (dab2_cbt->selectIndex >= 0)
    {
        if(g_bDAB2_DataMode)
        {
            DAB2_Combo_DrawItem(DAB2SelIndex);
        }
        else
        {
            DAB3_Combo_DrawItem(DAB3SelIndex);
        }
    }

    printf("[DAB2Combo_MouseUp(Down)][2]nTouchIndex:%d====\n",dab2_cbt->selectIndex);
    
    //for 30 second
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.24.05.20 

    return true;
}


bool DAB2Combo_Select(ITUWidget* widget, char* param)
{
    int nTouchIndex,nSelectIndex;
    
    nTouchIndex = atoi(param);
    nSelectIndex = nTouchIndex/10;

    if(nTouchIndex%10==0)
    {
        //printf("[DAB2Combo_Select] Left Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }
    else
    {
        //printf("[DAB2Combo_Select] Right Touch: nSelectIndex=%d\n",nTouchIndex,nSelectIndex);
    }
    
    if (dab2_cbt)
	{
        b_DAB2_ComboSelected = true;

        printf("\n[DAB2Combo_Select] Page:%d ItemCount:%d  pagemaxitem:%d Last_Idx:%d  Cur_Idx=%d =====\n",dab2_cbt->currPage,dab2_cbt->itemcount,dab2_cbt->pagemaxitem, dab2_cbt->lastselectIndex,dab2_cbt->selectIndex);
    }    
    else
    {
        printf("[DAB2Combo_Select]dab2_cbt is NULL Error====\n");
    }

    return true;
}

bool DAB2Combo_Button_Up(ITUWidget* widget, char* param)
{
    int nButtonIndex;
    nButtonIndex = atoi(param);

    //printf("[DAB2Combo_Button_Up]nButtonIndex=%d selectIndex:%d b_DAB2_ComboSelected:%d====\n",nButtonIndex,dab2_cbt->selectIndex,b_DAB2_ComboSelected);

    //if (dab2_cbt && b_DAB2_ComboSelected))
    if (dab2_cbt)
	{
        printf("\n[DAB2Combo_Button_Up] Page:%d ItemCount:%d  pagemaxitem:%dLast_Idx:%d  Cur_Idx=%d =====\n",dab2_cbt->currPage,dab2_cbt->itemcount,dab2_cbt->pagemaxitem, dab2_cbt->lastselectIndex,dab2_cbt->selectIndex);
        //(1)Save selected index

        if(g_bDAB2_DataMode)
        {
            //select DAB2 Category & change DAB2->DAB3
            //DAB2SelIndex = dab2_cbt->selectIndex;
            DAB2ListBox_ModeChange(MAIN_MODE_DAB3,true);
        }
        else
        {
            //select DAB3 Channel & change DAB3->Default(DAB2)
            //DAB3SelIndex = dab2_cbt->selectIndex;
            DAB2ListBox_ModeChange(MAIN_MODE_DAB2,true);
        }
	}
    else
    {
        printf("[DAB2Combo_Button_UP]dab2_cbt is NULL Error====b_DAB2_ComboSelected:%d\n",b_DAB2_ComboSelected);
    }

    b_DAB2_ComboSelected = false;
    m_bDAB2_Scroll_Touched = false;

    //for 30 seconds
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.24.05.20 

    return true;
}

void DAB2Combo_GotoPage(int nIndex)
{
    if(nIndex<=0)nIndex=0;//chpark.24.05.21
    //move to page
    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
    int  first_item_height   = pPageFirstItem->rect.height;
    int  nPage_ItemCount  = 8;
    int  nPage_ItemHeight = nPage_ItemCount*first_item_height;

    printf("[DAB2Combo_GotoPage]page=%d  position=%d\n",nIndex,-nPage_ItemHeight);
    
    ituComboTableMoveXY(dab2_cbt, 0, -first_item_pos);//move to zero position
    ituComboTableMoveXY(dab2_cbt, 0, -nPage_ItemHeight*nIndex);//move to selected page postion

    DAB2Combo_SetMaxMoveLimit();
    
    DAB2Combo_Set_ScrollBar(0);//chpark.24.08.06
}

void DAB2Combo_GotoPageTop(int nIndex)
{
    if(nIndex<MAX_DAB2LIST_COUNT && nIndex>=0)
    {
        //move to page
        ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
        int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
        int  first_item_height   = pPageFirstItem->rect.height;
        
        int  nSelect_ItemHeight = first_item_height*nIndex;

        //printf("[DAB2Combo_GotoPageTop]nIndex=%d  position=%d\n",nIndex,-nSelect_ItemHeight);
        
        ituComboTableMoveXY(dab2_cbt, 0, -first_item_pos);//move to zero position
        ituComboTableMoveXY(dab2_cbt, 0, -nSelect_ItemHeight);//move to selected page postion

        DAB2Combo_Set_ScrollBar(0);//chpark.24.08.06
    }
}

//==========================================================

bool MainOnDAB2_OnOff(DISP_MODE DispMode)
{
    ituWidgetSetVisible(dab2_cbt, DispMode);//chpark.01.03

    ituWidgetSetVisible(DAB2ListBar, DispMode);

    ituWidgetSetVisible(DAB2_ListBar_Back, DispMode);//chpark.09.11
    ituWidgetSetVisible(DAB2_ListBar_Bar,  DispMode);//chpark.09.11
   
	return true;
}

bool JogDialKey_Scroll_DAB2(char* param)
{
    char JD_Buf[2];
	
    switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
            DAB2ListBox_Select_PrevPageItem();
        }
        break;

        case JOG_DIAL_KEY_RIGHT:				//c3Byte0bit
		{
            DAB2ListBox_Select_NextPageItem();
        }
        break;

        case JOG_DIAL_KEY_UP:				//c3Byte0bit
		{
            DAB2ListBox_Select_UpItem();
        }
        break;

        case JOG_DIAL_KEY_DOWN:				//c3Byte0bit
		{
            DAB2ListBox_Select_DownItem();
        }
        break;

        case JOG_DIAL_KEY_ENTER_ON:				//c3Byte0bit
		{
          
        }
        break;

        case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        case JOG_DIAL_KEY_NONE:
		{            
            //chpark.11.16
            if(g_bDAB2_DataMode)
            {
                //select DAB2 Category & change DAB2->DAB3
                DAB2ListBox_ModeChange(MAIN_MODE_DAB3,true);    
            }
            else
            {
                //select DAB3 Channel & change DAB3->Default(DAB2)
                DAB2ListBox_ModeChange(MAIN_MODE_DAB2,true);   
            }
        }
        break;
    }    

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

bool JogDialRot_Scroll_DAB2(char* param)
{
    char JD_Buf[2];
	
    switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:				//c3Byte0bit
		{
             DAB2ListBox_Select_PrevItem();
        }
        break;

        case JOG_DIAL_ROT_RIGHT:				//c3Byte0bit
		{
            DAB2ListBox_Select_NextItem();
        }
        break;
    }    

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}



//===========================================================================
bool DAB2ListOnEnter()
{	
    //============================
    printf("[DAB2ListOnEnter]g_bDAB2_DataMode:%d\n",g_bDAB2_DataMode);
    //chpark.01.03
    ituWidgetSetVisible(dab2_cbt, DISP_ON); 

	ituWidgetSetVisible(DAB2ListBar, DISP_ON);

    ituWidgetSetVisible(DAB2_ListBar_Back, DISP_ON);  //chpark.09.11
    ituWidgetSetVisible(DAB2_ListBar_Bar,  DISP_ON);  //chpark.09.11	   
    //Select  DAB2 Mode or DAB3 Mode at start
    //====================================

    m_bDAB2_Scroll_Touched = false;//chpark.09.25
    m_bDAB2_ListInit = false;//chpark.12.05
    //====================================
    if(RDV.PTYNo<0)//chpark.09.26
	{
		RDV.PTYNo=-2;
	}

    //chpark.12.05
    DAB2SelIndex = g_DAB2_PreSelect_Index;
    DAB3SelIndex = g_DAB3_PreSelect_Index;//chpark.12.05

    //move to first page when select [auto button] in main
    //chpark.01.03
    if(g_bDAB2_DataMode)
    {
        //chpark.01.03
        dab2_cbt->lastselectIndex = -1;
        dab2_cbt->selectIndex = DAB2SelIndex;
        //
        DAB2ListInit();
    }
    else
    {
        //chpark.01.03
        dab2_cbt->lastselectIndex = -1;
        dab2_cbt->selectIndex = DAB3SelIndex;
        //
        m_nDAB3_SeviceChCnt = RDV.DABServiceCnt[DAB2SelIndex];//chpark.12.05
        DAB3ListInit();
    }

    RDV.DAB3_PTYNo = 0;//chpark.07.18//chpark.11.15
  
    DAB2_m_bFirstPTY = true;//chpark.09.26

    //chpark.09.26
    if(g_bDAB2_DataMode && DAB2_m_bFirstPTY)
    {
        m_nDAB3_SeviceChCnt = 0;//chpark.11.15

        DAB3SelIndex = -1; //chpark.07.18
        //==================================
        //chpark.11.16
        int nStartPage;

        //Focused Page Priority
        //(1) Touch sell
        //(2) PTY sell
        if(DAB2SelIndex>=0)
        {
            nStartPage =  DAB2SelIndex/8+1;
            //printf("(1)Focused Page:DAB2SelIndex:%d\n",DAB2SelIndex);
            if(abs(g_DAB2_PreSelect_POS)>0)
            {
                DAB2Combo_SetComboPos();
                //printf("(1-1)Focused Page:DAB2SelIndex:%d\n",DAB2SelIndex);
            }
            else
            {   
                DAB2Combo_GotoPage(nStartPage-1);//chpark.24.03.01
            }
        }
        else if(DAB2SelIndex<0 && RDV.DAB2_PTYNo<0)
        {
            nStartPage = 1;
            //printf("(2)Focused Page:DAB2SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2SelIndex,RDV.DAB2_PTYNo);
            DAB2Combo_GotoPage(nStartPage-1);//chpark.24.03.01
        }
        else if(DAB2SelIndex<0 && RDV.DAB2_PTYNo>=0)
        {
            nStartPage = RDV.DAB2_PTYNo/8+1;
           
            if(DAB2_m_bFirstPTY && RDV.DAB2_PTYNo>=0)
            {
                //printf("[DAB2ListOnEnter]DAB2Combo_GotoPageTop:DAB2SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2SelIndex,RDV.DAB2_PTYNo);
                DAB2Combo_GotoPageTop(RDV.DAB2_PTYNo);//chpark.24.03.27
            }
            else
            {
                //printf("(3)Focused Page:DAB2SelIndex:%d  RDV.DAB2_PTYNo:%d\n",DAB2SelIndex,RDV.DAB2_PTYNo);
                DAB2Combo_GotoPage(nStartPage-1);//chpark.24.03.01
            }
        }
        else
        {
            DAB2Combo_GotoPage(0);//chpark.24.03.01
        }
        
        DAB2_m_bFirstPTY = false;
    }
    else
    {
        //printf("[V-Mode]==========DAB2ListBox_ModeChange Start AT DAB3:%d\n",g_DAB3_PreSelect_Index);//chpark.11.15
        //chpark.11.16
        //start at DAB3Mode        
        char strTmp[50] = {0x20,};
        sprintf(strTmp, "%s",DAB2ListArray[DAB2SelIndex].name);//chpark.06.03
        DAB2_SetCategory(strTmp);
        //
        int nStartPage;

        if(DAB3SelIndex>=0)
        {
            nStartPage =  DAB3SelIndex/8+1;
            //printf("(1)Focused Page: nStartPage:%d DAB3SelIndex:%d\n",nStartPage,DAB3SelIndex);
        }
        else 
        {
            nStartPage = 1;
            //printf("(2)Focused Page: nStartPage:%d DAB3SelIndex:%d\n",nStartPage,DAB3SelIndex);
        }
       
        DAB2Combo_GotoPage(nStartPage-1);//chpark.01.03
    }

    m_bDAB2_ListInit = true;//chpark.24.03.13

    //printf("DAB2ListOnEnter End : DAB2_m_bFirstPTY:%d= m_bDAB2_ListInit=%d\n",DAB2_m_bFirstPTY,m_bDAB2_ListInit);
    return true;
}

static void DAB2ListInit(void)
{
    //printf("=======DAB2ListInit------g_bDAB2_DataMode:%d \n",g_bDAB2_DataMode);
    ituComboTable_SetPageItemCount(8);//chpark.24.05.20
    int i;

	//printf("DAB2ListInit \n");
    for (i = 0; i < MAX_DAB2LIST_COUNT; i++)
    {
       	DAB2ListArray[i].selected = false;
        DAB2ListArray[i].name = DAB2_PTY_No[i];
        //printf("==Ch[%02d]: Cnt=%d \n", i,RDV.DABServiceCnt[i]);//chpark.24.03.08
    }

    RDV.DAB2_PTYNo =  RDV.PTYNo;
    if(RDV.DAB2_PTYNo<0)RDV.DAB2_PTYNo=-2;//chpark.09.26
    //printf("RDV.DAB2_PTYNo=%d\n",RDV.DAB2_PTYNo);

    DAB2ListCnt = MAX_DAB2LIST_COUNT;
    
    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    //chpark.02.26
    if(dab2_cbt)
	{          
        int i = 0;
        int nComboListCnt = dab2_cbt->itemcount;

        if(MAX_DAB2LIST_COUNT>nComboListCnt)
        {
            //add
            int nAddCount = MAX_DAB2LIST_COUNT - nComboListCnt;
            ituComboTableAdd(dab2_cbt, nAddCount);
            //printf("[DAB2ListInit] current:%d / [#%d] item add.\n",nComboListCnt,nAddCount);
        }
        else if(MAX_DAB2LIST_COUNT<nComboListCnt)
        {
            //delete
            if (nComboListCnt>0)
            {
                for(int i=nComboListCnt-1; i>=MAX_DAB2LIST_COUNT ;i--)
                {
                    ituComboTableDelItem(dab2_cbt, i);
                    //printf("[DAB2ListInit]i item delete. remain:%d\n",i,nComboListCnt);
                }       
            }
        }
        else
        {
            //none
        }

        ituWidgetUpdate(dab2_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.13

        //printf("[DAB2ListInit] final current:%d \n",dab2_cbt->itemcount);

        for (i = 0; i < MAX_DAB2LIST_COUNT; i++)
        {
            //DAB2_Combo_DrawBlankItem(i);
            DAB2_Combo_DrawItem(i);
        }

        DAB2Combo_Set_ScrollBar(0);//chpark.24.03.08

        /*
        int nTmpPageCnt=0;
        int nTmpMaxItemInPage=7;
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
        ituScrollBarSetLength(DAB2ListBar,MAX_DAB2LIST_COUNT);//chpark.24.07.11
        */
        int nMovingLenth; 
	    int nCountInPage;

        nCountInPage = 8;
		nMovingLenth   = MAX_DAB2LIST_COUNT - nCountInPage + 1;
		if(nMovingLenth<=1)nMovingLenth=1;
		ituScrollBar_SetInfo(MAX_DAB2LIST_COUNT,nCountInPage);//chpark.24.08.06
		ituScrollBarSetLength(DAB2ListBar, nMovingLenth);//chpark.24.08.06
		ituScrollBarSetPosition(DAB2ListBar,1);//chpark.24.08.06
		
		printf("[DAB2ListInit] nMovingLenth=%d\n",nMovingLenth);
        
	}
}


void DAB2_Combo_DrawBlankItem(int nIndex)
{
    int i = nIndex;
    ITUWidget* tmpObj = NULL;
    tmpObj = ituComboTableGetItemChild(dab2_cbt, i, ITU_TEXT, 0);
    if (tmpObj)
    {
        ituTextSetString(tmpObj, "");

        ITUWidget* DAB2_ItemIcon[4]; //
        DAB2_ItemIcon[0] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 0);
        DAB2_ItemIcon[1] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 1);
        DAB2_ItemIcon[2] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 2);
        DAB2_ItemIcon[3] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 3);

        ituWidgetSetVisible(DAB2_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_ItemIcon[3], true);//none
    }
    else
    {
        printf("DAB2_Combo_DrawBlankItem[%d]:Error====>\n",i);
    }
}


void DAB2_Combo_DrawItem(int nIndex)
{
    if(nIndex<0)return;
    int i = nIndex;
       
    ITUWidget* tmpObj = NULL;

    //(1)TEXT
    char strItemText[22];
    //sprintf(strItemText, "%s", i);
    memcpy(strItemText,DAB2_PTY_No[i],22);
    //printf("[DAB2_Combo_DrawItem]=[%s] %d \n",strItemText,nIndex);

    tmpObj = ituComboTableGetItemChild(dab2_cbt, i, ITU_TEXT, 0);
    if (tmpObj)
    {
        if(RDV.DAB2_PTYNo==i)
        {
            if(RDV.DABServiceCnt[i]==0)
            {
                //if number of channels ==0
                //Gray
                ituWidgetSetColor(tmpObj,255,100,100,100);          
            }
            else
            {
                //black
                ituWidgetSetColor(tmpObj,255,0,0,0);
            }
        }
        else
        {
            if(RDV.DABServiceCnt[i]==0)
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
        return;
    }
    
    //(2)BK Color
    ITUWidget* DAB2_ItemIcon[4]; //
    DAB2_ItemIcon[0] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 0);
    DAB2_ItemIcon[1] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 1);
    DAB2_ItemIcon[2] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 2);
    DAB2_ItemIcon[3] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 3);

    if(DAB2_ItemIcon[0] && DAB2_ItemIcon[1] && DAB2_ItemIcon[2] &&  DAB2_ItemIcon[3])
    {
        //if(i<3)
        //printf("[DAB2_Combo_DrawItem] icon =>find\n");
    }
    else
    {
        //if(i<3)
        //printf("[DAB2_Combo_DrawItem] icon =>null error\n");
        return;
    }

    ituWidgetSetVisible(DAB2_ItemIcon[0], false);//gray + redbox
    ituWidgetSetVisible(DAB2_ItemIcon[1], false);//gray
    ituWidgetSetVisible(DAB2_ItemIcon[2], false);//redbox
    ituWidgetSetVisible(DAB2_ItemIcon[3], false);//none

    
    int nTmpCurrentIndex = i;
    if(i==RDV.DAB2_PTYNo && i==DAB2SelIndex)//PTY & Focus
    {
        if(RDV.DABServiceCnt[i]==0)
        {
            //if no service channel, set Focus BK
            //RedBox
            ituWidgetSetVisible(DAB2_ItemIcon[2], true);
            //printf("[RED BOX] i=%d DAB2SelIndex=%d\n",i,DAB2SelIndex);
        }
        else
        {
            //Silver + RedBox
            ituWidgetSetVisible(DAB2_ItemIcon[0], true);
        }
    }
    else if(i==RDV.DAB2_PTYNo && i!=DAB2SelIndex)//PTY & not Focus  
    {
        //if no service channel, set NONE BK
        if(RDV.DABServiceCnt[i]==0)
        {
            //None
            ituWidgetSetVisible(DAB2_ItemIcon[3], true);
        }
        else
        {
            //Silver 
            ituWidgetSetVisible(DAB2_ItemIcon[1], true);
        }
    }           
    else if(i!=RDV.DAB2_PTYNo && i==DAB2SelIndex)//not PTY&Focus
    {
        //RedBox
        ituWidgetSetVisible(DAB2_ItemIcon[2], true);
        //printf("[RED BOX] i=%d DAB2SelIndex=%d\n",i,DAB2SelIndex);
    }
    else
    {
        //None
        ituWidgetSetVisible(DAB2_ItemIcon[3], true);
        //printf("[NONE BOX] i=%d DAB2SelIndex=%d\n",i,DAB2SelIndex);
    }                
}

static void DAB3ListInit(void)
{
    //printf("[V-Mode][DAB3ListInit] Start.. m_nDAB3_SeviceChCnt=%d\n",m_nDAB3_SeviceChCnt);
	ituComboTable_SetPageItemCount(8);//chpark.24.05.20
    int i;

    //chpark.07.17
    int nTmpCurrentNumChar=0;
    DAB3ListCnt=0;
    DAB3ListCnt_withText=0;//chpark.10.16
    int nIndex=0;

    //m_nDAB3_SeviceChCnt = 500; //for test
	//m_nDAB3_SeviceChCnt = 28;

    for (i = 1; i <= 500; i++)//chpark.09.25
	{
        nTmpCurrentNumChar = strlen(DAB3PTYNo[i]);
	   	
        if( (nTmpCurrentNumChar>0 &&  i<=m_nDAB3_SeviceChCnt) || (DAB3ListCnt<m_nDAB3_SeviceChCnt && i<=m_nDAB3_SeviceChCnt))//chpark.10.16
        {
            //sprintf(&DAB3PTYNo[i][0],"KKKKKKKKKKKKKKKKKKKKLLLLLLLLLLPPPPPPPPPP");
            //printf("[%d]Channel Title before:%s\n",i,&DAB3PTYNo[i][0]);
            //memcpy(&DAB3PTYNo_LIMIT[i][0],&DAB3PTYNo[i][0],22);//chpark.10.17
            memcpy(&DAB3PTYNo_LIMIT[i][0],&DAB3PTYNo[i][0],18);//chpark.24.02.26
            //printf("[%d]Channel Title after:%s\n",i,&DAB3PTYNo_LIMIT[i][0]);

            DAB3ListArray[nIndex].selected = false;
	        DAB3ListArray[nIndex].name = &DAB3PTYNo_LIMIT[i][0];//chpark.07.18
            DAB3ListArray[nIndex].nt_ListNum = i;

            //printf("[V][%d]Channel Title new:%s  listNum=%d\n",i,DAB3ListArray[i].name,DAB3ListArray[i].nt_ListNum );
            nIndex++;
            DAB3ListCnt++;     
             //chpark.10.16
            if(nTmpCurrentNumChar>0 )
            {
                DAB3ListCnt_withText++;
            }   
        }
        else
        {
            //printf("[%d]Channel Title new:%s  NO_TITLE\n",i,DAB3ListArray[i].name);
        } 
	}

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18
    //==================================
    //chpark.01.03
    if(dab2_cbt)
	{
        int i = 0;
        int nComboListCnt = dab2_cbt->itemcount;
        //printf("\n\n[DAB3ListInit]m_nDAB3_SeviceChCnt=%d nComboListCnt=%d=====\n\n",m_nDAB3_SeviceChCnt,nComboListCnt);
        
        if(m_nDAB3_SeviceChCnt>nComboListCnt)
        {
            //add
            int nAddCount = m_nDAB3_SeviceChCnt - nComboListCnt;
            ituComboTableAdd(dab2_cbt, nAddCount);
            //printf("[DAB3ListInit] [#%d] item add. remain:%d\n",nAddCount,dab2_cbt->itemcount);
        }
        else if(m_nDAB3_SeviceChCnt<nComboListCnt)
        {
            //delete
            if (nComboListCnt>0)
            {
                for(int i=nComboListCnt-1; i>=m_nDAB3_SeviceChCnt ;i--)
                {
                    ituComboTableDelItem(dab2_cbt, i);
                    //printf("[DAB3ListInit]%d item delete. remain:%d(m_nDAB3_SeviceChCnt:%d)\n",i,dab2_cbt->itemcount,m_nDAB3_SeviceChCnt);
                }       
            }
        }
        else
        {
            //none
        }

        ituWidgetUpdate(dab2_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);//chpark.24.03.13

        //chpark.24.05.21
        //for (i = 0; i < m_nDAB3_SeviceChCnt; i++)
        //{
            //DAB2_Combo_DrawBlankItem(i);
        //}

        //for (i = 0; i < m_nDAB3_SeviceChCnt-1; i++)//chpark.24.05.21. for test
        for (i = 0; i < m_nDAB3_SeviceChCnt; i++)//chpark.24.05.21. for test 
        {
            DAB3_Combo_DrawItem(i);
        }

        DAB2Combo_Set_ScrollBar(0);//chpark.24.03.08

        /*
        int nTmpPageCnt=0;
        int nTmpMaxItemInPage=8;
        if(DAB3ListCnt%nTmpMaxItemInPage==0)
        {
            nTmpPageCnt = DAB3ListCnt/nTmpMaxItemInPage;
        }
        else
        {
            nTmpPageCnt = DAB3ListCnt/nTmpMaxItemInPage+1;
        }

        //int nTmpPageCnt = (MAX_DAB2LIST_COUNT + nTmpMaxItemInPage - 1) / nTmpMaxItemInPage;
        //ituScrollBarSetLength(DAB2ListBar,nTmpPageCnt);//chpark.24.07.11
        ituScrollBarSetLength(DAB2ListBar,DAB3ListCnt);//chpark.24.07.11
        */
        int nMovingLenth; 
	    int nCountInPage;

        nCountInPage = 8;
		nMovingLenth   = DAB3ListCnt - nCountInPage + 1;
		if(nMovingLenth<=1)nMovingLenth=1;
		ituScrollBar_SetInfo(DAB3ListCnt,nCountInPage);//chpark.24.08.06
		ituScrollBarSetLength(DAB2ListBar, nMovingLenth);//chpark.24.08.06
		ituScrollBarSetPosition(DAB2ListBar,1);//chpark.24.08.06
		
		printf("[DAB3ListInit] nMovingLenth=%d\n",nMovingLenth);
	}
}


void DAB3_Combo_DrawItem(int nIndex)
{
    int i = nIndex;
    if(i<0) return;

    DAB3ListEntry* entry;
    entry = &DAB3ListArray[nIndex];

    ITUWidget* tmpObj = NULL;

    //(1)TEXT
    char strItemText[22];
    //sprintf(strItemText, "%s", i);
    
    memcpy(strItemText,&DAB3PTYNo[i][0],22);
    tmpObj = ituComboTableGetItemChild(dab2_cbt, i, ITU_TEXT, 0);
    if (tmpObj)
    {
        //White
        ituWidgetSetColor(tmpObj,255,255,255,255);
        //ituTextSetString(tmpObj, strItemText);
        //printf("[DAB3_Combo_DrawItem]%d  : %s\n",i,strItemText);
        if(entry->name != NULL)
        {
            ituTextSetString(tmpObj, (char*)entry->name);
        }
        else
        {
            ituTextSetString(tmpObj, "");
        }
        
        //printf("[DAB3_Combo_DrawItem]%d  : %s\n",i,(char*)entry->name);
    
        //(2)BK Color
        ITUWidget* DAB2_ItemIcon[4]; //
        DAB2_ItemIcon[0] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 0);
        DAB2_ItemIcon[1] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 1);
        DAB2_ItemIcon[2] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 2);
        DAB2_ItemIcon[3] = ituComboTableGetItemChild(dab2_cbt, i, ITU_ICON, 3);

        ituWidgetSetVisible(DAB2_ItemIcon[0], false);//gray + redbox
        ituWidgetSetVisible(DAB2_ItemIcon[1], false);//gray
        ituWidgetSetVisible(DAB2_ItemIcon[2], false);//redbox
        ituWidgetSetVisible(DAB2_ItemIcon[3], false);//none

        if(i==DAB3SelIndex)//not PTY&Focus
        {
            //RedBox
            ituWidgetSetVisible(DAB2_ItemIcon[2], true);
            //printf("[RED BOX] i=%d DAB3SelIndex=%d\n",i,DAB3SelIndex);
        }
        else
        {
            //None
            ituWidgetSetVisible(DAB2_ItemIcon[3], true);
        }
    }
    else
    {
         printf("[DAB3_Combo_DrawItem]can't find  : %s\n",i,strItemText);
    }
}


bool DAB2_ListBar_Changed(ITUWidget* widget, char* param);
bool DAB2_ListBar_Changed(ITUWidget* widget, char* param)
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
        nCurSelected_Index  = DAB2SelIndex;
        nMaxListIndex       = DAB2ListCnt-1;
    }
    else
    {
        nScroll_Index = nIndex-1; 
        nCurSelected_Index  = DAB3SelIndex;
        nMaxListIndex       = DAB3ListCnt-1;
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
            DAB2Combo_GotoPage(0);
            DAB2ListBox_Select(0);
			
            
		}
        return false;
	}

    if(nCurSelected_Index<0)
    {
        //(1)Deselect AUTO Button
        MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
	    //(2)Make the jog signal to be passed to the scroll.
	    MainMode_DAB2_Select_Index = RD_SEL_SCROLL;   
        
        DAB2Combo_GotoPage(0);
        DAB2ListBox_Select(0);

        return true;
    }
    else
    {

    }

    //printf("[DAB2_ListBar_Changed] nNewSelected_Index=%d nCurSelected_Index=%d\n",nNewSelected_Index,nCurSelected_Index);
    //==========================================================
	ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
	int  first_item_pos      = pPageFirstItem->rect.y;//first item postion
	int  first_item_height   = pPageFirstItem->rect.height;
	
	int nCurrentPos = abs(pPageFirstItem->rect.y) / pPageFirstItem->rect.height;
	int nPrePosIndex;
	if(nCurrentPos<=0)
	{
		nPrePosIndex=nCurSelected_Index;
	}
	else
	{
		nPrePosIndex=(8-1)+nCurrentPos;
	}

    int nStartIndex_Page;
	int nEndIndex_Page;

	nStartIndex_Page = nScroll_Index;
	nEndIndex_Page   = nStartIndex_Page+(count-1);
	
	if(nStartIndex_Page>=nMaxListIndex)
	{
		nStartIndex_Page = nMaxListIndex;
	}
	if(nEndIndex_Page>=nMaxListIndex)
	{
		nEndIndex_Page = nMaxListIndex;
	}

    int nPageIndex_Diff = nScroll_Index - nCurrentPos;
	int nIndex_Diff = nScroll_Index - nCurrentPos;
	int absDiff_Pos;

    //printf("[1]pPageFirstItem->rect.y=%d nScroll_Index=%d nPrePosIndex=%d nIndex_Diff=%d nCurrentPos=%d absDiff_Pos=%d\n",pPageFirstItem->rect.y,nScroll_Index,nPrePosIndex,nIndex_Diff,nCurrentPos,absDiff_Pos);

    if(nCurrentPos<nScroll_Index)
	{
		//down
		absDiff_Pos = nPageIndex_Diff*pPageFirstItem->rect.height;
		ituComboTableMoveXY(dab2_cbt, 0, -absDiff_Pos);
		nNewSelected_Index = nCurSelected_Index + nIndex_Diff;

		//최대치는 못넘어가게 만든다.
        SU100_Combo_SetMaxMoveLimit();

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
		ituComboTableMoveXY(dab2_cbt, 0, -absDiff_Pos);
		nNewSelected_Index = nNewSelected_Index - nIndex_Diff;

		//선택된 인덱스가 보이지 않는다면 최하단의 셀을 선택한다.
		//if(nCurSelected_Index > nEndIndex_Page)
		{
			nNewSelected_Index = nEndIndex_Page;
			printf("[DAB2_ListBar_Changed]nNewSelected_Index=%d nCurSelected_Index=%d nEndIndex_Page=%d\n",nNewSelected_Index,nCurSelected_Index,nEndIndex_Page);
		}
	}
	else
	{
		//same
	}

	if(nNewSelected_Index>=0 && nNewSelected_Index<=nMaxListIndex)
	{
		//printf("[DAB2_ListBar_Changed]nNewSelected_Index=%d\n",nNewSelected_Index);
	}
	else
	{
		//printf("[DAB2_ListBar_Changed]nNewSelected_Index=%d\n",nNewSelected_Index);
		nNewSelected_Index = 0;
	}

    DAB2ListBox_Select(nNewSelected_Index);

    DAB2Combo_Set_ScrollBar(0);//chpark.24.07.11
/*
    if(dab2_cbt->selectIndex>=0)
    {
        ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
        //printf("[DAB2_ListBar_Changed] pPageSelectItem->rect.y=%d\n",pPageSelectItem->rect.y);
        if( nNewSelected_Index>nCurSelected_Index )
        {
            //슬라이드바의 아래쪽 터치->최상단 포커스
            //새로 선택된 셀이 보이는 영역에 있다면, 같은 페이지 이므로 셀을 이동하지 않는다.
            //int nDiff_Index = nNewSelected_Index - nCurSelected_Index;
           
            if(pPageSelectItem->rect.y>=0 && pPageSelectItem->rect.y<=49*(7-1))
            {
                //dont' move. same page.
                //printf("[3]dont' move. same page.==\n");
            }
            else
            {
                //시야에 보이지 않는 다른 페이지로 이동된 상태이므로 일단 시야에 들어오게 한다.
                //ituComboTableMoveXY(dab2_cbt, 0, -(nDiff_Index-1)*49);

                //다른페이지로 이동한 상태이므로 
                //최상단(zero position)에 셀을 맞춘다.
                pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
                int nSelectedIndex_Pos = pPageSelectItem->rect.y;

                if(nSelectedIndex_Pos!=0)
                {
                    //printf("[DAB2_ListBar_Changed] [down][4-1] nSelectedIndex_Pos=%d dab2_cbt->selectIndex=%d\n",nSelectedIndex_Pos,dab2_cbt->selectIndex);
                    ituComboTableMoveXY(dab2_cbt, 0, -nSelectedIndex_Pos);

                    pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
                    nSelectedIndex_Pos = pPageSelectItem->rect.y;
                    //printf("[DAB2_ListBar_Changed] [down][4-2] nSelectedIndex_Pos=%d dab2_cbt->selectIndex=%d\n",nSelectedIndex_Pos,dab2_cbt->selectIndex);
                
                    //최대치는 못넘어가게 만든다.
                    DAB2Combo_SetMaxMoveLimit();
                }
            }
        }
        else if(nNewSelected_Index<nCurSelected_Index)
        {
            //슬라이드바의 위쪽 터치->최하단 포커스
            //새로 선택된 셀이 보이는 영역에 있다면, 같은 페이지 이므로 셀을 이동하지 않는다.
            //int nDiff_Index = nCurSelected_Index - nNewSelected_Index;
            bool bMinusPos = false;
            if(dab2_cbt->selectIndex>=1)
            {
                pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex-1];
                if(pPageSelectItem->rect.y<0)
                {
                    bMinusPos = true;
                }
            }

            if(pPageSelectItem->rect.y>=0 && pPageSelectItem->rect.y<=49*(7-1) && !bMinusPos)
            {
                //dont' move. same page.
                //printf("[3]dont' move. same page.==\n");
                
                if(dab2_cbt->selectIndex>=1)
                {
                    pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex-1];
                    //printf("[3]dont' move. index=%d pos=%d==\n",dab2_cbt->selectIndex-1,pPageSelectItem->rect.y);
                    
                    pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
                    //printf("[3]dont' move. index=%d pos=%d==\n",dab2_cbt->selectIndex,pPageSelectItem->rect.y);
                }

                if(dab2_cbt->selectIndex+1<=nMaxListIndex)
                {    
                    pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex+1];
                    //printf("[3]dont' move. index=%d pos=%d==\n",dab2_cbt->selectIndex+1,pPageSelectItem->rect.y);
                }
            }
            else
            {
                //시야에 보이지 않는 다른 페이지로 이동된 상태이므로 일단 시야에 들어오게 한다.
                //마이너스의 영역은 좌표의 기준이 다르다.
                //0번째는 0         0번째는 0
                //1번째는 -49       1번째는 49
                //2번째는 -98       2번째는 98
                //따라서 -1 * 49의 계산을 한다.
                //ituComboTableMoveXY(dab2_cbt, 0, (nDiff_Index-1)*49);

                //다른페이지로 이동한 상태이므로 
                //zero포지션으로 이동 후, 최하단에 셀을 맞춘다.
                pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
                int nSelectedIndex_Pos  = pPageSelectItem->rect.y;
                int nFinalMoving_Pos    = -nSelectedIndex_Pos+49*(7-1);

                //printf("[DAB2_ListBar_Changed] [up][4-1] -nSelectedIndex_Pos=%d nFinalMoving_Pos=%d dab2_cbt->selectIndex=%d\n",-nSelectedIndex_Pos,nFinalMoving_Pos,dab2_cbt->selectIndex);
                
                if(nSelectedIndex_Pos!=0)
                {
                    //printf("[DAB2_ListBar_Changed] [up][4-1] nSelectedIndex_Pos=%d dab2_cbt->selectIndex=%d\n",nSelectedIndex_Pos,dab2_cbt->selectIndex);
                    ituComboTableMoveXY(dab2_cbt, 0, nFinalMoving_Pos);

                    pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
                    nSelectedIndex_Pos = pPageSelectItem->rect.y;
                    //printf("[DAB2_ListBar_Changed] [up][4-2] nFinalMoving_Pos=%d nSelectedIndex_Pos=%d dab2_cbt->selectIndex=%d\n",nFinalMoving_Pos,nSelectedIndex_Pos,dab2_cbt->selectIndex);
                }
            }
        }
        else
        {
            //printf("[DAB2_ListBar_Changed] [focus]not move\n");
        }
    }
    */
    
    return true;
}


bool DAB2ListBox_Select_UpItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp--;

        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp--;

        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }

    DAB2ListBox_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBox_Select_DownItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB3ListCnt)
            nScroll_Tmp=DAB3ListCnt-1;
    }

    DAB2ListBox_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBox_Select_NextItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp++;
   
        if(nScroll_Tmp>=DAB3ListCnt)
            nScroll_Tmp=DAB3ListCnt-1;
    }

    DAB2ListBox_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBox_Select_PrevItem()
{
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp--;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp--;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;
    }

    DAB2ListBox_Select(nScroll_Tmp);

    return true;
}

bool DAB2ListBox_Select_NextPageItem()
{
    int nMovingCnt=0;

    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp+=8;
   
        if(nScroll_Tmp>=DAB2ListCnt)
            nScroll_Tmp=DAB2ListCnt-1;

        nMovingCnt = nScroll_Tmp - DAB2SelIndex;//chpark.24.03.05
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp+=8;
   
        if(nScroll_Tmp>=DAB3ListCnt)
            nScroll_Tmp=DAB3ListCnt-1;

        nMovingCnt = nScroll_Tmp - DAB3SelIndex;//chpark.24.03.05
    }

    //chpark.24.03.05
    if(nMovingCnt<=0)
    {
        //printf("[DAB2ListBox_Select_NextPageItem]nMovingPos=%d return false.\n",nMovingCnt);
        return false;
    }
    
    int nTmpIndex = nScroll_Tmp;
    int count = 8;
   
    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2SelIndex==nTmpIndex)
        {
            return false;
        }

        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB2SelIndex;

        g_DAB2_PreSelect_Index = DAB2SelIndex;
        DAB2SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB2SelIndex;
    }
    else
    {
        //if same index->return
        if(DAB3SelIndex==nTmpIndex)
        {
            return false;
        }
        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB3SelIndex;

        g_DAB3_PreSelect_Index = DAB3SelIndex;
        DAB3SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB3SelIndex;
    }

    //Call a function that automatically sets the focus.
    int y = ituWidgetGetHeight(dab2_container);
    int nMovingPos=0;

    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
    
    nMovingPos = (pPageFirstItem->rect.height)*nMovingCnt;
    //printf("[DAB2ListBox_Select_NextPageItem]height:%d nMovingPos:%d nMovingCnt:%d\n",pPageFirstItem->rect.height,nMovingPos,nMovingCnt); 

    ituComboTableMoveXY(dab2_cbt, 0, -nMovingPos);

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBox_Select_NextPageItem]lastselectIndex=%d  selectIndex=%d\n",dab2_cbt->lastselectIndex,dab2_cbt->selectIndex);
        DAB2_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB2_Combo_DrawItem(dab2_cbt->selectIndex);
    }
    else
    {
        DAB3_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB3_Combo_DrawItem(dab2_cbt->selectIndex);
    }

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

bool DAB2ListBox_Select_PrevPageItem()
{
    int nMovingCnt=0;
    if(g_bDAB2_DataMode)
    {
        nScroll_Tmp = DAB2SelIndex; 
        nScroll_Tmp-=8;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        nMovingCnt = DAB2SelIndex - nScroll_Tmp;//chpark.24.03.05
    }
    else
    {
        nScroll_Tmp = DAB3SelIndex; 
        nScroll_Tmp-=8;
    
        if(nScroll_Tmp<0)
            nScroll_Tmp=0;

        nMovingCnt = DAB3SelIndex - nScroll_Tmp;//chpark.24.03.05
    }

    if(nMovingCnt<=0)
    {
        //printf("[DAB2ListBox_Select_PrevPageItem]nMovingPos=%d return false.\n",nMovingCnt);
        return false;
    }
    
    //chpark.24.03.05    
    int nTmpIndex = nScroll_Tmp;
    int count = 8;
   
    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2SelIndex==nTmpIndex)
        {
            return false;
        }

        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB2SelIndex;

        g_DAB2_PreSelect_Index = DAB2SelIndex;
        DAB2SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB2SelIndex;
    }
    else
    {
        //if same index->return
        if(DAB3SelIndex==nTmpIndex)
        {
            return false;
        }
        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB3SelIndex;

        g_DAB3_PreSelect_Index = DAB3SelIndex;
        DAB3SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB3SelIndex;
    }

    //Call a function that automatically sets the focus.
    int y = ituWidgetGetHeight(dab2_container);
    int nMovingPos=0;

    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
    
    nMovingPos = (pPageFirstItem->rect.height)*nMovingCnt;
    //printf("[DAB2ListBox_Select_PrevPageItem]height:%d nMovingPos:%d nMovingCnt:%d\n",pPageFirstItem->rect.height,nMovingPos,nMovingCnt); 

    ituComboTableMoveXY(dab2_cbt, 0, nMovingPos);

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBox_Select_PrevPageItem]lastselectIndex=%d  selectIndex=%d\n",dab2_cbt->lastselectIndex,dab2_cbt->selectIndex);
        DAB2_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB2_Combo_DrawItem(dab2_cbt->selectIndex);
    }
    else
    {
        DAB3_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB3_Combo_DrawItem(dab2_cbt->selectIndex);
    }
    //

    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}

//Select item in List(DAB2 & DAB3)
bool DAB2ListBox_SetPTY(int nIndex)
{
    //if no service channel is selected, remove Current PTY.
    //chpark.10.16
    if(nIndex<0)
    {
       printf("[DAB2ListBox_SetPTY] Error nIndex=%d\n",nIndex);
       return false;
    }

    int nTmpIndex = nIndex;

    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
 
    int count = 8;//chpark.24.03.27
    int nPageIndex  = (nTmpIndex)/count+1;
    int nFocusIndex = (nTmpIndex)%count;

    //Call a function that automatically sets the focus 
    //after loading the page.
    if(m_bDAB2_ListInit || DAB2_m_bFirstPTY)//chpark.09.26
    {
        //chpark.09.26
        if(RDV.DAB2_PTYNo==nTmpIndex && !DAB2_m_bFirstPTY)
        {
            //printf("==DAB2ListBox_SetPTY=SAME=old=%d  new=%d\n",RDV.DAB2_PTYNo,nTmpIndex);
            return false;
        }

        RDV.DAB2_PTYNo = nTmpIndex;
        
        //chpark.09.25
        if(m_bDAB2_Scroll_Touched)
        {
            //printf("==DAB2ListBox_SetPTY==m_bDAB2_Scroll_Touched==%d \n",m_bDAB2_Scroll_Touched);
            return false;
        }
       
        //update at reflesh
        //DAB2Combo_GotoPageTop(nIndex);//chpark.01.03 //chpark.24.03.27
        //
    }
    else
    {
       
    }

    return true;
}


//Focus at current page.
//Draw Red Box
bool DAB2ListBox_Select(int nIndex)
{
    if(nIndex==-1)
    {
        //printf("[V-mode]DAB2ListBox_Select Index=%d RDV.DAB2_PTYNo=%d RDV.DAB3_PTYNo%d\n",nIndex,RDV.DAB2_PTYNo,RDV.DAB3_PTYNo);

        if(g_bDAB2_DataMode)
        {   
             if(DAB2SelIndex==-1)return false;//already no focus
            DAB2_LastSelIndex = DAB2SelIndex;//chpark.01.05
            DAB2SelIndex = -1;
            g_DAB2_PreSelect_Index = -1;//chpark.12.11
            DAB2_Combo_DrawItem(DAB2_LastSelIndex); //chpark.01.05

            //return false;
        }
        else
        {
            if(DAB3SelIndex==-1)return false;//already no focus
            DAB3_LastSelIndex = DAB3SelIndex;//chpark.01.05
            DAB3SelIndex = -1;
            g_DAB3_PreSelect_Index = -1;//chpark.12.11
            DAB3_Combo_DrawItem(DAB3_LastSelIndex); //chpark.01.05
            //return false;

        }
    
        //chpark.01.03
        dab2_cbt->lastselectIndex = -1;
        dab2_cbt->selectIndex = -1;
        DAB2_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB2_Combo_DrawItem(dab2_cbt->selectIndex);
        //

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
            nIndex = DAB3SelIndex;
            if(nIndex==-1)
                nIndex=0;
        }
    }

    int nTmpIndex = nIndex;

    int nOld_selectIndex;
    int nOld_pageIndex;
    int nOld_focusIndex;
    int count = 8;//chpark.24.03.27
   
    int nPageIndex  = (nTmpIndex)/count+1;
    int nFocusIndex = (nTmpIndex)%count;

    if(g_bDAB2_DataMode)
    {
        //if same index->return
        if(DAB2SelIndex==nTmpIndex)
        {
           //printf("[V-mode]DAB2ListBox_Select: SAME return false: NewIndex=%d DAB2SelIndex:%d\n",nIndex,DAB2SelIndex);
            return false;
        }
        //printf("[V-mode]DAB2ListBox_Select: NewIndex=%d DAB2SelIndex:%d\n",nIndex,DAB2SelIndex);

        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB2SelIndex;//chpark.01.03

        g_DAB2_PreSelect_Index = nTmpIndex;//chpark.11.15
        DAB2SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB2SelIndex;//chpark.01.03
    }
    else
    {
        //if same index->return
        if(DAB3SelIndex==nTmpIndex)
        {
            return false;
        }
        //save last seleted index in combobox
        dab2_cbt->lastselectIndex = DAB3SelIndex;//chpark.01.03

        g_DAB3_PreSelect_Index = nTmpIndex;//chpark.12.05
        DAB3SelIndex = nTmpIndex;

        //save new seleted index in combobox
        dab2_cbt->selectIndex = DAB3SelIndex;//chpark.01.03
    }

    //Call a function that automatically sets the focus 
    //after loading the page.

    //chpark.01.03
    int y = ituWidgetGetHeight(dab2_container);
    int dy=0;
     dy = y / 8;

    ITUWidget* pPageFirstItem = dab2_cbt->itemArr[0];
    ITUWidget* pPageSelectItem = dab2_cbt->itemArr[dab2_cbt->selectIndex];
    //printf("[DAB2ListBox_Select]pPageFirstItem->rect.height:%d\n",pPageFirstItem->rect.height); 
    dy = pPageFirstItem->rect.height;

    if(g_bDAB2_DataMode)
    {
        if(nTmpIndex<8)
        {
            dy = 0;
        }
        else if( nTmpIndex>(MAX_DAB2LIST_COUNT-8+1) )
        {
            dy = 0; //dy = (MAX_DAB2LIST_COUNT-8+1)*dy;
        }
        else
        {
            //dy=dy;//dy = dy*nTmpIndex;
        }
    }
    else
    {
        if(nTmpIndex<8)
        {
            //dy = 0;
             dy = dy*nTmpIndex;
        }
        else if( nTmpIndex>(m_nDAB3_SeviceChCnt-8+1) )
        {
            dy = (m_nDAB3_SeviceChCnt-8+1)*dy;
        }
        else
        {
            dy = dy*nTmpIndex;
        }
    }

    //Make sure it's the same page.
    if(dab2_cbt->selectIndex>dab2_cbt->lastselectIndex)
    {
        //when index up
        if(nTmpIndex<8)
        {
            dy = 0;
        }
        else 
        {
            dy =  pPageFirstItem->rect.height;
        }

        if( pPageSelectItem->rect.y>((8-1)*97) )
        {
            int nLast_Diff=0;
            nLast_Diff = (8*97 - pPageSelectItem->rect.y);
            if(abs(nLast_Diff)<97)//chpark.24.04.03
            {
                ituComboTableMoveXY(dab2_cbt, 0, -dy+nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_cbt, 0, -dy);
            }

        }
        ituWidgetUpdate(dab2_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);
        //printf("[DAB2ListBox_Select] DAB2SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB2SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }
    else
    {
        //when index down
        if(nTmpIndex<8 && pPageSelectItem->rect.y<(8-1)*97)
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
            nLast_Diff = (97 + pPageSelectItem->rect.y);
            if(abs(nLast_Diff)<97)
	        {
                ituComboTableMoveXY(dab2_cbt, 0, dy-nLast_Diff);
            }
            else
            {
                ituComboTableMoveXY(dab2_cbt, 0, dy);
            }
        }
        ituWidgetUpdate(dab2_cbt, ITU_EVENT_LAYOUT, 0, 0, 0);

        //printf("[DAB2ListBox_Select] DAB2SelIndex=%d  pPageFirstItem->rect.y=%d  pPageSelectItem->rect.y:%d  dy=%d\n",DAB2SelIndex,pPageFirstItem->rect.y,pPageSelectItem->rect.y,dy);
    }

    if(g_bDAB2_DataMode)
    {
        //printf("[DAB2ListBox_Select]lastselectIndex=%d  selectIndex=%d\n",dab2_cbt->lastselectIndex,dab2_cbt->selectIndex);
        DAB2_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB2_Combo_DrawItem(dab2_cbt->selectIndex);
    }
    else
    {
        DAB3_Combo_DrawItem(dab2_cbt->lastselectIndex); 
        DAB3_Combo_DrawItem(dab2_cbt->selectIndex);
    }
    //
    DAB2Combo_Set_ScrollBar(0);//chpark.24.08.21
    
    //for 30 second 
    Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18

    return true;
}


void DAB2ListBox_ModeChange(int nDAB_MODE, bool bInfoReq)//chpark.11.27
{
    if(nDAB_MODE==MAIN_MODE_DAB3)
    {
        //DAB2->DAB3
        //Select Category
        if(DAB2SelIndex>=0)
        {
            if(RDV.DABServiceCnt[DAB2SelIndex]==0)
            {
               //printf("No Service Channel:%d\n",RDV.DABServiceCnt[DAB2SelIndex]);
               printf("DAB2ListBox_ModeChange Error======%d  %d\n",DAB2SelIndex,RDV.DABServiceCnt[DAB2SelIndex]);
            }
            else
            {
                //memset(DAB3PTYNo, 0, sizeof(DAB3PTYNo[0][0]) * 500 * 80);//chpark.09.21//chpark.11.15
                //printf("ReLoading Channel Data In DAB3 Mode == Index=%d  Channel count=%d\n",DAB2SelIndex,RDV.DABServiceCnt[DAB2SelIndex]);
                
                //save DAB2 POS
                //chpark.24.03.01
                DAB2Combo_GetComboPos();

                //chpark.01.03
                //move to first page
                DAB2Combo_GotoPage(0);
                dab2_cbt->selectIndex = -1;
                dab2_cbt->lastselectIndex = -1;

                //DAB2->DAB3    
                g_bDAB2_DataMode  = false;//chpark.11.15
                m_bDAB2_ListInit  = false;

                //chpark.09.25
                if(bInfoReq)//chpark.11.27
                {
                    memset(DAB3PTYNo, 0, sizeof(DAB3PTYNo[0][0]) * 500 * 80);//chpark.09.21//chpark.11.15
                    m_nDAB3_SeviceChCnt = RDV.DABServiceCnt[DAB2SelIndex];
                    //MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);     //chpark.12.05
				    MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);//chpark.24.11.11

                    //Set First Index.
                    DAB3SelIndex = -1;//chpark.06.21
                    g_DAB3_PreSelect_Index = -1;//chpark.24.02.29
                    RDV.DAB3_PTYNo = 0;//chpark.11.15

                    //=================
                    //chpark.12.05
                    DAB3ListInit();
                    //==================
                }
                else
                {
                    //===========================
                    //chpark.12.05
                    DAB3SelIndex = g_DAB3_PreSelect_Index;//chpark/12.05
                    DAB2SelIndex = g_DAB2_PreSelect_Index;//chpark/11.13
                    m_nDAB3_SeviceChCnt = RDV.DABServiceCnt[DAB2SelIndex];//chpark.11.15

                    printf("=========Rotation DAB3 modechange: DAB3SelIndex:%d g_DAB2_PreSelect_Index:%d\n",DAB3SelIndex,g_DAB2_PreSelect_Index);

                    RDV.DAB3_PTYNo = 0;

                    DAB3ListInit();

                }              

                //Set Channel Title
                char strTmp[50] = {0x20,};
                sprintf(strTmp, "%s",DAB2ListArray[DAB2SelIndex].name); //chpark.06.03
                DAB2_SetCategory(strTmp);

                Main_Disp_Mode = MAIN_MODE_DAB3; //chpark.24.01.29
               
                if(bInfoReq)//chpark.11.27
                {
                    //printf("request DAB3 info===\n\n\n");
                    Main_CLI3[0] = DAB2SelIndex;		//chpark 2023-09-21
				    Main_CLI3[4] = Main_CLI3[4]|0x02; 	//chpark 2023-07-21
                }
                else
                {
                    //printf("no request DAB3 info===\n\n\n");
                }
            }
        }
        else
        {
            //printf("DAB2ListBoxH_MouseUp==SelIndex Error:[%d]\n",DAB2SelIndex);
        }

        //
        DAB2Combo_Set_ScrollBar(0);//chpark.24.02.29
        //
    }
    else
    {
        //chpark.01.03
        //move to first page
        DAB2Combo_GotoPage(0);
        dab2_cbt->selectIndex = -1;
        dab2_cbt->lastselectIndex = -1;
        //

        //In DAB3 MODE
        //Select Channel &  go to DAB Mode
        Main_CLI3[4] = Main_CLI3[4]|0x01;			//	JPHong 2023-07-14

        uint8_t nRealIndex= DAB3ListArray[DAB3SelIndex].nt_ListNum;

        //chpark.10.04
        int nTmpCurrentNumChar = strlen(DAB3PTYNo[nRealIndex]);
        if(nTmpCurrentNumChar<=0)
        {
            //printf("\n\n[DAB3 Selected Index]:Empty String===\n\n",nRealIndex);//chpark.12.05
        }
        //
        //printf("[DAB3 Select]nTmpCurrentNumChar=%d nRealIndex=%d\n",nTmpCurrentNumChar,nRealIndex);

        printf("\n\n[DAB3 Selected Index]:%d===\n\n",nRealIndex);
        
        Main_CLI3[6] = (nRealIndex&0xFF00)>>8; 	//	chpark 2023-09-21
        Main_CLI3[5] = nRealIndex&0xFF;			//	chpark 2023-09-21

        Main_Disp_Mode = MAIN_MODE_DAB2; //chpark.24.01.29
        DAB2_Set_JD_Button(RD_SEL_AUTO);//chpark.09.09
    }
}



//chpark.07.18
//When there is a change in the list, it redraws the scroll.
//Changes are checked as global parameters.
bool DAB3ListBox_Reflesh()
{
    //chpark.24.08.21
    //DAB2Combo_Set_ScrollBar(0);//chpark.24.02.29

    int nTmpDAB3_ListNumIndex = 0;
    
    int count = 8;

    if(g_bDAB2_DataMode)
    {
        if(g_nDAB2_Data_LastRecvTime==DAB2_DataChkTime)
        {
            //New data.
            //printf("==No New Data== Time:%d \n",g_nDAB2_Data_LastRecvTime);
            return false;
        }
        else
        {
            //printf("[DAB2]==New Data RCV== Time:%d \n",g_nDAB2_Data_LastRecvTime);
            DAB2ListInit();

            //New data received.
            g_nDAB2_Data_LastRecvTime = DAB2_DataChkTime;
            Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.24.05.20
        }

        return false;
    }


    if(g_nDAB3_Data_LastRecvTime==DAB3_DataChkTime)
    {
        //No New data.
        //printf("==No New Data== Time:%d \n",g_nDAB3_Data_LastRecvTime);
        return false;
    }
    else
    {
        //printf("[DAB3]==New Data RCV== Time:%d \n",g_nDAB3_Data_LastRecvTime);
        //New data received.
        g_nDAB3_Data_LastRecvTime = DAB3_DataChkTime;
    }



    //chpark.10.16
    if(DAB3ListCnt_withText>=m_nDAB3_SeviceChCnt)
    {
        //If there are more than the number of service channels, no longer receive it.
        //printf("ChannelInfo Over:CH_Count:%d List:%d \n",m_nDAB3_SeviceChCnt,DAB3ListCnt);
        //return false;//chpark.12.04
    }
   
   
    //printf("Timer Delay OK11=%d \n\n",Media_RunTime_Cnt);
    if(m_bDAB2_ListInit)
    {
        //�ƹ� �͵� ���ϰ� ������ ���� ���?.
        //no focus
        //listbox->pageIndex
        int nCurrentPageIndex; //= DAB2ListBox->listbox.pageIndex;

        //printf("[DAB3ListBox_Reflesh] nCurrentPageIndex = %d  ======== \n\n",nCurrentPageIndex);

        DAB3ListInit();

        //for 30 second 
        Media_RunTime_Cnt = gMainTimerCnt_10m;//chpark.07.18
        //

        //printf("Timer Delay OK=%d \n\n",Media_RunTime_Cnt);
    }
    else if(!m_bDAB2_ListInit)
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


//=========================================================================================
