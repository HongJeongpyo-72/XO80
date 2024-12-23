#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S11
M7S11_BG
S11_TopUserName
S11_TopStandard
S11_TopAdmin
S11_D7_BTN
S11_D6_BTN
S11_D5_BTN
S11_AUX12_OffHold
S11_AUX12_Off
S11_AUX12_On
S11_AUX12_BTN
S11_AUX2_OffHold
S11_AUX2_Off
S11_AUX2_On
S11_AUX2_BTN
S11_AUX1_OffHold
S11_AUX1_Off
S11_AUX1_On
S11_AUX1_BTN
S11_AUX12
S11_AUX2
S11_AUX1
*/

static ITUWidget* S11_TEXT_CONT; //chpark.24.04.15
static ITUWidget* S11_TEXT[5];   //chpark.24.04.15

static ITUButton* S11_AUX1;
static ITUButton* S11_AUX2;
static ITUButton* S11_AUX12;

static ITUIcon* S11_AUX1_Sel;
static ITUIcon* S11_AUX2_Sel;
static ITUIcon* S11_AUX12_Sel;

static ITUIcon* S11_TopAdmin;
static ITUIcon* S11_TopStandard;
static ITUIcon* S11_TopUserName;

uint8_t S11_Auto1_Flag = 0;			// N/A    6byte,6~7bit
uint8_t S11_Auto2_Flag = 0;			// N/A    7byte,0~1bit
uint8_t S11_Auto12_Flag = 0;			// N/A    7byte 2~3bit

static uint32_t S11_SecTimer;
static uint8_t S11_ExitCNT = 0;
static uint8_t S11_SelStatus = 0;

bool S11_UserDisplay(void);
void S11_Auto1_Display(uint8_t tDisplayFlag);
void S11_Auto2_Display(uint8_t tDisplayFlag);
void S11_Auto12_Display(uint8_t tDisplayFlag);

void S11_SelReposition(uint8_t tSelIdx);
void S11_SelectDisplay(uint8_t tSelBTN);
void S11_SelectClear(void);

bool S11_BTN_MouseUp(ITUWidget* widget, char* param);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);



bool S11_Enter(ITUWidget* widget, char* param)
{
	// dev.opt
	//S11_Auto1_Flag = 1;			// N/A    6byte,6~7bit
	//S11_Auto2_Flag = 1;			// N/A    7byte,0~1bit
	//S11_Auto12_Flag = 1;			// N/A    7byte 2~3bit
	SetWidget(S11_TEXT_CONT,"S11_CONT_TEXT");

	SetWidget(S11_TEXT[Lang_ENG],"S11_TEXT_ENG");
	SetWidget(S11_TEXT[Lang_DEU],"S11_TEXT_GER");
	SetWidget(S11_TEXT[Lang_FRA], "S11_TEXT_FR");
	SetWidget(S11_TEXT[Lang_ITA], "S11_TEXT_IT");
	SetWidget(S11_TEXT[Lang_ESP], "S11_TEXT_SP");

	for(int i=0; i<5; i++)
	{
		ituWidgetSetVisible(S11_TEXT[i], false);
	}   
	ituWidgetSetVisible(S11_TEXT[Lang_Flag[M7_UserIndex]], true);


	SetWidget(S11_AUX1,"S11_AUX1");
	SetWidget(S11_AUX2,"S11_AUX2");
	SetWidget(S11_AUX12,"S11_AUX12");
	
	SetWidget(S11_AUX1_Sel,"S11_AUX1_Sel");
	SetWidget(S11_AUX2_Sel,"S11_AUX2_Sel");
	SetWidget(S11_AUX12_Sel,"S11_AUX12_Sel");

	SetWidget(S11_TopAdmin,"S11_TopAdmin");
	SetWidget(S11_TopStandard,"S11_TopStandard");
	SetWidget(S11_TopUserName,"S11_TopUserName");
	

	S11_SelectClear();

	if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
	{
		S11_SelectDisplay(1);
	}
	else
	{
		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
		{
			S11_SelectDisplay(2);
		}
		else
		{
			if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
			{
				S11_SelectDisplay(3);
			}
		}
	}

	S11_Auto1_Display(S11_Auto1_Flag);
	S11_Auto2_Display(S11_Auto2_Flag);
	S11_Auto12_Display(S11_Auto12_Flag);

	S11_UserDisplay();
	
	S11_SecTimer = gMainTimerCnt_10m;
	S11_ExitCNT = 0;

    return true;
}

bool S11_Timer(ITUWidget* widget, char* param)
{
	static uint8_t b_Auto1_flag=3,b_Auto2_flag=3,b_Auto12_flag=3;

	M7_AP_Display(M7_AP_OptFlag);		// JPHong 2023-03-15  14091-1
		
	if((gMainTimerCnt_10m - S11_SecTimer) >= 100)
	{
		if(S11_ExitCNT<30)
			S11_ExitCNT++;

		if(S11_ExitCNT == 30)
		{
			S11_ExitCNT = 100;
			printf("\r\nM7S11 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}

		S11_SecTimer = gMainTimerCnt_10m;
	}	

	if(b_Auto1_flag != S11_Auto1_Flag)
	{
		b_Auto1_flag = S11_Auto1_Flag;
		
		S11_Auto1_Display(S11_Auto1_Flag);
	}

	if(b_Auto2_flag != S11_Auto2_Flag)
	{
		b_Auto2_flag = S11_Auto2_Flag;
		
		S11_Auto2_Display(S11_Auto2_Flag);
	}

	if(b_Auto12_flag != S11_Auto12_Flag)
	{
		b_Auto12_flag = S11_Auto12_Flag;
		
		S11_Auto12_Display(S11_Auto12_Flag);
	}

	S11_SelReposition(S11_SelStatus);
	S11_UserDisplay();	
	
    return true;
}

bool S11_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tSel_Idx = 0;

	S11_ExitCNT = 0;

	tJogWheel = param[0];
	printf("S11 JogWheel = %d\r\n",tJogWheel);
	
	tSel_Idx = S11_SelStatus;
	if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		S11_SelectClear();

		tSel_Idx++;
		
		if((tSel_Idx == 4)||(tSel_Idx == 1))
		{
			tSel_Idx = 1;

			if((S11_Auto1_Flag == 2)||(S11_Auto1_Flag == 3))
			{
				if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
				{
					S11_SelectDisplay(2);
				}
				else
				{
					if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
					{
						S11_SelectDisplay(3);
					}
				}
			}
			else
			{
				S11_SelectDisplay(1);
			}
		}
		else if(tSel_Idx == 2)
		{
			if((S11_Auto2_Flag == 2)||(S11_Auto2_Flag == 3))
			{
				if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
				{
					S11_SelectDisplay(3);
				}
				else
				{
					if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
					{
						S11_SelectDisplay(1);
					}
				}
			}
			else
			{
				S11_SelectDisplay(2);
			}
		}
		else if(tSel_Idx == 3)
		{
			if((S11_Auto12_Flag == 2)||(S11_Auto12_Flag == 3))
			{
				if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
				{
					S11_SelectDisplay(1);
				}
				else
				{
					if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
					{
						S11_SelectDisplay(2);
					}
				}
			}
			else
			{
				S11_SelectDisplay(3);
			}
		}
	}

	else if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		S11_SelectClear();

		if(tSel_Idx > 0)
			tSel_Idx--;
			
		if(tSel_Idx == 0)
		{
			tSel_Idx = 3;
			
			if((S11_Auto12_Flag == 2)||(S11_Auto12_Flag == 3))
			{
				if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
				{
					S11_SelectDisplay(2);
				}
				else
				{
					if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
					{
						S11_SelectDisplay(1);
					}
				}
			}
			else
			{
				S11_SelectDisplay(3);
			}
		}
		else if(tSel_Idx == 2)
		{
			if((S11_Auto2_Flag == 2)||(S11_Auto2_Flag == 3))
			{
				if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
				{
					S11_SelectDisplay(1);
				}
				else
				{
					if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
					{
						S11_SelectDisplay(3);
					}
				}
			}
			else
			{
				S11_SelectDisplay(2);
			}
		}
		else if(tSel_Idx == 1)
		{
			if((S11_Auto1_Flag == 2)||(S11_Auto1_Flag == 3))
			{
				if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
				{
					S11_SelectDisplay(3);
				}
				else
				{
					if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
					{
						S11_SelectDisplay(2);
					}
				}
			}
			else
			{
				S11_SelectDisplay(1);
			}
		}
	}

    return true;
}

bool S11_JogButton(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	uint8_t tJogButton = 0;
	uint8_t tSel_Idx = 0;

	S11_ExitCNT = 0;

	tJogButton = param[0];
	printf("S11 JogButton = %d\r\n",tJogButton);

	tSel_Idx = S11_SelStatus;

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_DOWN:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				S11_SelectClear();
      
      	if(tSel_Idx == 1)
      	{
      		tSel_Idx = 2;
      		
      		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      		{
      			S11_SelectDisplay(2);
      		}
      		else
      		{
      			if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
      			{
      				S11_SelectDisplay(3);
      			}
      			else
      			{
      				S11_SelectDisplay(1);
      			}
      		}
      	}
      	else if(tSel_Idx == 2)
      	{
      		tSel_Idx = 3;
      		
      		if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
      		{
      			S11_SelectDisplay(3);
      		}
      		else
      		{
      			if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      			{
      				S11_SelectDisplay(1);
      			}
      			else
      			{
      				S11_SelectDisplay(2);
      			}
      		}
      	}
      	else if(tSel_Idx == 3)
      	{
      		tSel_Idx = 1;
      		
      		if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      		{
      			S11_SelectDisplay(1);
      		}
      		else
      		{
      			if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      			{
      				S11_SelectDisplay(2);
      			}
      			else
      			{
      				S11_SelectDisplay(3);
      			}
      		}
      	}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				S11_SelectClear();

				if(tSel_Idx == 1)
				{
					tSel_Idx = 2;

      		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      		{
      			S11_SelectDisplay(2);
      		}
      		else
      		{
      			if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      			{
      				S11_SelectDisplay(1);
      			}
      			else
      			{
      				S11_SelectDisplay(2);
      			}
      		}
				}
				else if(tSel_Idx == 2)
				{
					tSel_Idx = 1;
					
					if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
					{
						S11_SelectDisplay(1);
					}
					else
					{
						if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
						{
							S11_SelectDisplay(2);
						}
						else
						{
							S11_SelectDisplay(1);
						}
					}
				}
			}
		}
		break;

		case JOG_DIAL_KEY_UP:
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				S11_SelectClear();
      
      	if(tSel_Idx == 1)
      	{
      		tSel_Idx = 3;
      		
      		if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
      		{
      			S11_SelectDisplay(3);
      		}
      		else
      		{
      			if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      			{
      				S11_SelectDisplay(2);
      			}
      			else
      			{
      				S11_SelectDisplay(1);
      			}
      		}
      	}
      	else if(tSel_Idx == 2)
      	{
      		tSel_Idx = 1;
      		
      		if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      		{
      			S11_SelectDisplay(1);
      		}
      		else
      		{
      			if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
      			{
      				S11_SelectDisplay(3);
      			}
      			else
      			{
      				S11_SelectDisplay(2);
      			}
      		}
      	}
      	else if(tSel_Idx == 3)
      	{
      		tSel_Idx = 2;
      		
      		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      		{
      			S11_SelectDisplay(2);
      		}
      		else
      		{
      			if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      			{
      				S11_SelectDisplay(1);
      			}
      			else
      			{
      				S11_SelectDisplay(3);
      			}
      		}
      	}
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				S11_SelectClear();

				if((tSel_Idx == 1)||(tSel_Idx == 3))
				{
					tSel_Idx = 2;

      		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
      		{
      			S11_SelectDisplay(2);
      		}
      		else
      		{
      			if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      			{
      				S11_SelectDisplay(1);
      			}
      			else
      			{
      				S11_SelectDisplay(2);
      			}
      		}
				}
				else if(tSel_Idx == 2)
				{
					tSel_Idx = 1;
					
					if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
					{
						S11_SelectDisplay(1);
					}
					else
					{
						if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
						{
							S11_SelectDisplay(2);
						}
						else
						{
							S11_SelectDisplay(1);
						}
					}
				}
			}
		}
		break;

		case JOG_DIAL_KEY_LEFT:
		{
/*			
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
			}
			else
*/			
			if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				S11_SelectClear();

				if((tSel_Idx == 1)||(tSel_Idx == 2))
				{
					tSel_Idx = 3;

      		if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
      		{
      			S11_SelectDisplay(3);
      		}
      		else
      		{
     				S11_SelectDisplay(1);
      		}
				}
				else if(tSel_Idx == 3)
				{
					tSel_Idx = 1;
					
					if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
					{
						S11_SelectDisplay(1);
					}
					else
					{
						S11_SelectDisplay(3);
					}
				}
			}
		}			
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
/*			
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
			}
			else
*/			
			if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				S11_SelectClear();

				if(tSel_Idx == 3)
				{
					tSel_Idx = 1;

      		if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
      		{
      			S11_SelectDisplay(1);
      		}
      		else
      		{
     				S11_SelectDisplay(3);
      		}
				}
				else if((tSel_Idx == 2)||(tSel_Idx == 1))
				{
					tSel_Idx = 3;
					
					if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
					{
						S11_SelectDisplay(3);
					}
					else
					{
						S11_SelectDisplay(1);
					}
				}
			}
		}			
		break;

		case JOG_DIAL_KEY_ENTER_ON:
		{
			switch(S11_SelStatus)
			{
				case 1:
				{
					if(S11_Auto1_Flag == 0)		// off
					{
						S11_SelectDisplay(1);
					}
					else if(S11_Auto1_Flag == 1)		// on
					{
						S11_SelectDisplay(1);
					}
				}
				break;
				
				case 2:
				{
					if(S11_Auto2_Flag == 0)		// off
					{
						S11_SelectDisplay(2);
					}
					else if(S11_Auto2_Flag == 1)		// on
					{
						S11_SelectDisplay(2);
					}
				}
				break;
				
				case 3:
				{
					if(S11_Auto12_Flag == 0)		// off
					{
						S11_SelectDisplay(3);
					}
					else if(S11_Auto12_Flag == 1)		// on
					{
						S11_SelectDisplay(3);
					}
				}
				break;
			}
		}
		break;

		case JOG_DIAL_KEY_ENTER_OFF:
		{
			char tBTN_Num[5];
			
			memset(tBTN_Num,0,5);
			sprintf(tBTN_Num,"%d",S11_SelStatus);
			
			S11_BTN_MouseUp(NULL,tBTN_Num);
		}
		break;
	}
	
    return true;
}

bool S11_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;

	S11_ExitCNT = 0;
	
	tJogSwitch = param[0];
	printf("S11 JogSwitch = %d\r\n",tJogSwitch);
	
	if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		GotoMenu7();
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
	
    return true;
}

bool S11D7_BTNMouseUp(ITUWidget* widget, char* param)
{
	char tBTN_Num[5];

	S11_ExitCNT = 0;
			
	memset(tBTN_Num,0,5);
	sprintf(tBTN_Num,"%d",S11_SelStatus);
			
	S11_BTN_MouseUp(NULL,tBTN_Num);

    return true;
}

bool S11D7_BTNMouseDn(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	S11_ExitCNT = 0;
	
	switch(S11_SelStatus)
	{
		case 1:
		{
			if(S11_Auto1_Flag == 0)		// off
			{
				S11_SelectDisplay(1);
			}
			else if(S11_Auto1_Flag == 1)		// on
			{
				S11_SelectDisplay(1);
			}
		}
		break;
		
		case 2:
		{
			if(S11_Auto2_Flag == 0)		// off
			{
				S11_SelectDisplay(2);
			}
			else if(S11_Auto2_Flag == 1)		// on
			{
				S11_SelectDisplay(2);
			}
		}
		break;
		
		case 3:
		{
			if(S11_Auto12_Flag == 0)		// off
			{
				S11_SelectDisplay(3);
			}
			else if(S11_Auto12_Flag == 1)		// on
			{
				S11_SelectDisplay(3);
			}
		}
		break;
	}
	
    return true;
}

bool S11D6_BTNMouseUp(ITUWidget* widget, char* param)
{

	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool S11D5_BTNMouseUp(ITUWidget* widget, char* param)
{
	
	GotoMenu7();
	
    return true;
}

bool S11_BTN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Num;
	
	S11_ExitCNT = 0;

	tBTN_Num = (uint8_t)atoi(param);

	S11_SelectClear();
	
	if(tBTN_Num == 1)		// aux1 auto pressure
	{
		if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))		// off/on
		{
			S11_SelectDisplay(1);
		}
	}
	else if(tBTN_Num == 2)		// aux2 auto pressure
	{
		if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))		// off/on
		{
			S11_SelectDisplay(2);
		}
	}
	else if(tBTN_Num == 3)		// aux1/2 auto pressure
	{
		if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))		// off/on
		{
			S11_SelectDisplay(3);
		}
	}

    return true;
}

bool S11_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	uint8_t tBTN_Num;
	uint8_t tDisp_State = 3;

	S11_ExitCNT = 0;

	tBTN_Num = (uint8_t)atoi(param);
	
	if(tBTN_Num == 1)		// aux1 auto pressure
	{
		if(S11_Auto1_Flag == 0)		// off
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1B 00 00 00 01 01 00 00 00
			txbuffer[0] = 0x1B;		// aux1 auto pressure
			txbuffer[4] = 0x01;		// enable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
		else if(S11_Auto1_Flag == 1)		// on
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1B 00 00 00 00 01 00 00 00
			txbuffer[0] = 0x1B;		// aux1 auto pressure
			txbuffer[4] = 0x00;		// disable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
	}
	else if(tBTN_Num == 2)		// aux2 auto pressure
	{
		if(S11_Auto2_Flag == 0)		// off
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1C 00 00 00 01 01 00 00 00
			txbuffer[0] = 0x1C;		// aux2 auto pressure
			txbuffer[4] = 0x01;		// enable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
		else if(S11_Auto2_Flag == 1)		// on
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1C 00 00 00 00 01 00 00 00
			txbuffer[0] = 0x1C;		// aux2 auto pressure
			txbuffer[4] = 0x00;		// disable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
	}
	else if(tBTN_Num == 3)		// aux1/2 auto pressure
	{
		if(S11_Auto12_Flag == 0)		// off
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1D 00 00 00 01 01 00 00 00
			txbuffer[0] = 0x1D;		// aux12 auto pressure
			txbuffer[4] = 0x01;		// enable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
		else if(S11_Auto12_Flag == 1)		// on
		{
			// CAN TX CAN[FNS1;0x1850E9CE]: 1D 00 00 00 00 01 00 00 00
			txbuffer[0] = 0x1D;		// aux12 auto pressure
			txbuffer[4] = 0x00;		// disable
			txbuffer[5] = 0x01;
			txbuffer[6] = 0x00;
				
			CanbusTxDataManagement(gMainTimerCnt_10m,FNS1,txbuffer);
		}
	}

    return true;
}

void S11_SelReposition(uint8_t tSelIdx)
{
	switch(tSelIdx)
	{
		case 1:
		{
			if((S11_Auto1_Flag == 2)||(S11_Auto1_Flag == 3))
			{
				S11_SelectClear();
				
				if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
				{
					S11_SelectDisplay(2);
				}
				else
				{
					if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
					{
						S11_SelectDisplay(3);
					}
				}
			}
			else
			{
				S11_SelectDisplay(1);
			}
		}
		break;
		
		case 2:
		{
			if((S11_Auto2_Flag == 2)||(S11_Auto2_Flag == 3))
			{
				S11_SelectClear();
				
				if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
				{
					S11_SelectDisplay(1);
				}
				else
				{
					if((S11_Auto12_Flag == 0)||(S11_Auto12_Flag == 1))
					{
						S11_SelectDisplay(3);
					}
				}
			}
			else
			{
				S11_SelectDisplay(2);
			}
		}
		break;
	
		case 3:
		{
			if((S11_Auto12_Flag == 2)||(S11_Auto12_Flag == 3))
			{
				S11_SelectClear();
				
				if((S11_Auto1_Flag == 0)||(S11_Auto1_Flag == 1))
				{
					S11_SelectDisplay(1);
				}
				else
				{
					if((S11_Auto2_Flag == 0)||(S11_Auto2_Flag == 1))
					{
						S11_SelectDisplay(2);
					}
				}
			}
			else
			{
				S11_SelectDisplay(3);
			}
		}
		break;
	}
}

void S11_SelectDisplay(uint8_t tSelBTN)
{


	S11_SelectClear();
	
	if(tSelBTN == 1)
	{
		S11_SelStatus = 1;
		ituWidgetSetVisible(S11_AUX1_Sel, true);
	}
	else if(tSelBTN == 2)
	{
		S11_SelStatus = 2;
		ituWidgetSetVisible(S11_AUX2_Sel, true);
	}
	else if(tSelBTN == 3)
	{
		S11_SelStatus = 3;
		ituWidgetSetVisible(S11_AUX12_Sel, true);
	}
}

void S11_SelectClear(void)
{
	if(ituWidgetIsVisible(S11_AUX1_Sel) == true)
	{
		ituWidgetSetVisible(S11_AUX1_Sel, false);
	}

	if(ituWidgetIsVisible(S11_AUX2_Sel) == true)
	{
		ituWidgetSetVisible(S11_AUX2_Sel, false);
	}

	if(ituWidgetIsVisible(S11_AUX12_Sel) == true)
	{
		ituWidgetSetVisible(S11_AUX12_Sel, false);
	}
}

void S11_Auto1_Display(uint8_t tDisplayFlag)
{
	ITUWidget *S11_AUX1_On = ituSceneFindWidget(&theScene, "S11_AUX1_On");
	assert(S11_AUX1_On);
	ITUWidget *S11_AUX1_Off = ituSceneFindWidget(&theScene, "S11_AUX1_Off");
	assert(S11_AUX1_Off);
	ITUWidget *S11_AUX1_OffHold = ituSceneFindWidget(&theScene, "S11_AUX1_OffHold");
	assert(S11_AUX1_OffHold);

	if(tDisplayFlag == 0)			// off
	{
		ituWidgetSetVisible(S11_AUX1_On,false);
		ituWidgetSetVisible(S11_AUX1_OffHold,false);
		ituWidgetSetVisible(S11_AUX1_Off,true);
		ituWidgetSetVisible(S11_AUX1,true);
	}
	else if(tDisplayFlag == 1)		// on
	{
		ituWidgetSetVisible(S11_AUX1_OffHold,false);
		ituWidgetSetVisible(S11_AUX1_Off,false);
		ituWidgetSetVisible(S11_AUX1_On,true);
		ituWidgetSetVisible(S11_AUX1,true);
	}
	else if(tDisplayFlag == 2)		// off hold
	{
		ituWidgetSetVisible(S11_AUX1_Off,false);
		ituWidgetSetVisible(S11_AUX1_On,false);
		ituWidgetSetVisible(S11_AUX1_OffHold,true);
		ituWidgetSetVisible(S11_AUX1,true);
	}
	else if(tDisplayFlag == 3)		// no display
	{
		ituWidgetSetVisible(S11_AUX1_Off,false);
		ituWidgetSetVisible(S11_AUX1_On,false);
		ituWidgetSetVisible(S11_AUX1_OffHold,false);
		ituWidgetSetVisible(S11_AUX1,false);
	}
}

void S11_Auto2_Display(uint8_t tDisplayFlag)
{
	ITUWidget *S11_AUX2_On = ituSceneFindWidget(&theScene, "S11_AUX2_On");
	assert(S11_AUX2_On);
	ITUWidget *S11_AUX2_Off = ituSceneFindWidget(&theScene, "S11_AUX2_Off");
	assert(S11_AUX2_Off);
	ITUWidget *S11_AUX2_OffHold = ituSceneFindWidget(&theScene, "S11_AUX2_OffHold");
	assert(S11_AUX2_OffHold);

	if(tDisplayFlag == 0)			// off
	{
		ituWidgetSetVisible(S11_AUX2_On,false);
		ituWidgetSetVisible(S11_AUX2_OffHold,false);
		ituWidgetSetVisible(S11_AUX2_Off,true);
		ituWidgetSetVisible(S11_AUX2,true);
	}
	else if(tDisplayFlag == 1)		// on
	{
		ituWidgetSetVisible(S11_AUX2_OffHold,false);
		ituWidgetSetVisible(S11_AUX2_Off,false);
		ituWidgetSetVisible(S11_AUX2_On,true);
		ituWidgetSetVisible(S11_AUX2,true);
	}
	else if(tDisplayFlag == 2)		// off hold
	{
		ituWidgetSetVisible(S11_AUX2_Off,false);
		ituWidgetSetVisible(S11_AUX2_On,false);
		ituWidgetSetVisible(S11_AUX2_OffHold,true);
		ituWidgetSetVisible(S11_AUX2,true);
	}
	else if(tDisplayFlag == 3)		// no display
	{
		ituWidgetSetVisible(S11_AUX2_Off,false);
		ituWidgetSetVisible(S11_AUX2_On,false);
		ituWidgetSetVisible(S11_AUX2_OffHold,false);
		ituWidgetSetVisible(S11_AUX2,false);
	}
}

void S11_Auto12_Display(uint8_t tDisplayFlag)
{
	ITUWidget *S11_AUX12_On = ituSceneFindWidget(&theScene, "S11_AUX12_On");
	assert(S11_AUX12_On);
	ITUWidget *S11_AUX12_Off = ituSceneFindWidget(&theScene, "S11_AUX12_Off");
	assert(S11_AUX12_Off);
	ITUWidget *S11_AUX12_OffHold = ituSceneFindWidget(&theScene, "S11_AUX12_OffHold");
	assert(S11_AUX12_OffHold);

	if(tDisplayFlag == 0)			// off
	{
		ituWidgetSetVisible(S11_AUX12_On,false);
		ituWidgetSetVisible(S11_AUX12_OffHold,false);
		ituWidgetSetVisible(S11_AUX12_Off,true);
		ituWidgetSetVisible(S11_AUX12,true);
	}
	else if(tDisplayFlag == 1)		// on
	{
		ituWidgetSetVisible(S11_AUX12_OffHold,false);
		ituWidgetSetVisible(S11_AUX12_Off,false);
		ituWidgetSetVisible(S11_AUX12_On,true);
		ituWidgetSetVisible(S11_AUX12,true);
	}
	else if(tDisplayFlag == 2)		// off hold
	{
		ituWidgetSetVisible(S11_AUX12_Off,false);
		ituWidgetSetVisible(S11_AUX12_On,false);
		ituWidgetSetVisible(S11_AUX12_OffHold,true);
		ituWidgetSetVisible(S11_AUX12,true);
	}
	else if(tDisplayFlag == 3)		// no display
	{
		ituWidgetSetVisible(S11_AUX12_Off,false);
		ituWidgetSetVisible(S11_AUX12_On,false);
		ituWidgetSetVisible(S11_AUX12_OffHold,false);
		ituWidgetSetVisible(S11_AUX12,false);
	}
}

bool S11_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(S11_TopStandard,false);
		ituWidgetSetVisible(S11_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(S11_TopAdmin,false);
		ituWidgetSetVisible(S11_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(S11_TopAdmin,false);
		ituWidgetSetVisible(S11_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(S11_TopUserName, tDestName);

		ituWidgetSetVisible(S11_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(S11_TopUserName,false);
	}

}

