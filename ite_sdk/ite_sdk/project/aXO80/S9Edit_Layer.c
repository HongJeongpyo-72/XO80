#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
S9_Edit
S9_E
S9E_D7_BTN
S9E_D6_BTN
S9E_D5_BTN
S9E_A_SEL
S9E_N_SEL
S9E_Y_SEL
S9E_O_SEL
S9E_M_SEL
S9E_BT_Check
S9E_RF_Check
S9E_Dot9
S9E_Dot8
S9E_Dot7
S9E_Dot6
S9E_Dot5
S9E_Dot4
S9E_Dot3
S9E_Dot2
S9E_Dot1
S9_E10
S9_E9
S9_E8
S9_E7
S9_E6
S9_E5
S9_E4
S9_E3
S9_E2
S9_E1
S9E_UserName
S9E_Title_ESP
S9A_ESP_Add
S9A_ESP_A_BTN
S9E_ESP_Nn
S9E_ESP_N_BTN
S9E_ESP_Yn
S9E_ESP_Y_BTN
S9E_ESP_MSG
S9E_ESP_O0
S9E_ESP_O1
S9E_ESP_O_BTN
S9E_ESP_M0
S9E_ESP_M1
S9E_ESP_M_BTN
S9E_Title_ITA
S9A_ITA_Add
S9A_ITA_A_BTN
S9E_ITA_Nn
S9E_ITA_N_BTN
S9E_ITA_Yn
S9E_ITA_Y_BTN
S9E_ITA_MSG
S9E_ITA_O0
S9E_ITA_O1
S9E_ITA_O_BTN
S9E_ITA_M0
S9E_ITA_M1
S9E_ITA_M_BTN
S9E_Title_FRA
S9A_FRA_Add
S9A_FRA_A_BTN
S9E_FRA_Nn
S9E_FRA_N_BTN
S9E_FRA_Yn
S9E_FRA_Y_BTN
S9E_FRA_MSG
S9E_FRA_O0
S9E_FRA_O1
S9E_FRA_O_BTN
S9E_FRA_M0
S9E_FRA_M1
S9E_FRA_M_BTN
S9E_Title_DEU
S9A_DEU_Add
S9A_DEU_A_BTN
S9E_DEU_Nn
S9E_DEU_N_BTN
S9E_DEU_Yn
S9E_DEU_Y_BTN
S9E_DEU_MSG
S9E_DEU_O0
S9E_DEU_O1
S9E_DEU_O_BTN
S9E_DEU_M0
S9E_DEU_M1
S9E_DEU_M_BTN
S9E_Title_ENG
S9A_ENG_Add
S9A_ENG_A_BTN
S9E_ENG_Nn
S9E_ENG_N_BTN
S9E_ENG_Yn
S9E_ENG_Y_BTN
S9E_ENG_MSG
S9E_ENG_O0
S9E_ENG_O1
S9E_ENG_O_BTN
S9E_ENG_M0
S9E_ENG_M1
S9E_ENG_M_BTN
*/

extern uint8_t cAddUserIdx;
extern uint8_t cEditUserIdx;

extern void GotoMenuTop(void);
extern void GotoMenu7(void);

bool S9Edit_Enter(ITUWidget* widget, char* param)
{
	if(cAddUserIdx > 0)
	{
		// marster user - user add mode
	}
	else if(cAddUserIdx == 0)
	{
		if(cEditUserIdx > 0)
		{
			// master user - user edit mode
		}
		else if(cEditUserIdx == 0)
		{
			// operater user - user edit mode
		}
	}

	
    return true;
}

bool S9Edit_Timer(ITUWidget* widget, char* param)
{
    return true;
}

bool S9Edit_JogWheel(ITUWidget* widget, char* param)
{
    return true;
}

bool S9Edit_JogButton(ITUWidget* widget, char* param)
{
    return true;
}

bool S9Edit_JogSwitch(ITUWidget* widget, char* param)
{
    return true;
}

bool S9E_D7_MouseUp(ITUWidget* widget, char* param)
{
    return true;
}

bool S9E_D6_MouseUp(ITUWidget* widget, char* param)
{
    return true;
}

bool S9E_D5_MouseUp(ITUWidget* widget, char* param)
{
	GotoMenu7();
	
    return true;
}

bool S9E_BTN_Dn(ITUWidget* widget, char* param)
{
    return true;
}

bool S9E_BTN_Up(ITUWidget* widget, char* param)
{
    return true;
}

