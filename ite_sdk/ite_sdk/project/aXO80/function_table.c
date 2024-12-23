#include "ite/itu.h"

extern bool LogoOnEnter(ITUWidget* widget, char* param);
extern bool LogoOnTimer(ITUWidget* widget, char* param);
extern bool LogoOnLeave(ITUWidget* widget, char* param);
extern bool LogoJDR(ITUWidget* widget, char* param);
extern bool LogoJDK(ITUWidget* widget, char* param);
extern bool LogoJDS(ITUWidget* widget, char* param);
extern bool Logo_USB_Inserted(ITUWidget* widget, char* param);
extern bool USER_PW_DnBt_Func(ITUWidget* widget, char* param);
extern bool USER_PW_UpBt_Func(ITUWidget* widget, char* param);
extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool MainOnLeave(ITUWidget* widget, char* param);
extern bool JogDialRotation(ITUWidget* widget, char* param);
extern bool JogDialKey(ITUWidget* widget, char* param);
extern bool JogDialSwitch(ITUWidget* widget, char* param);
extern bool Main_USB_Inserted(ITUWidget* widget, char* param);
extern bool MainCamera(ITUWidget* widget, char* param);
extern bool CamColorMenu(ITUWidget* widget, char* param);
extern bool CamColorNight(ITUWidget* widget, char* param);
extern bool CamColorDay(ITUWidget* widget, char* param);
extern bool CamColorSha(ITUWidget* widget, char* param);
extern bool CamColorHue(ITUWidget* widget, char* param);
extern bool CamColorSat(ITUWidget* widget, char* param);
extern bool CamColorCon(ITUWidget* widget, char* param);
extern bool CamColorBr(ITUWidget* widget, char* param);
extern bool CamColorDown(ITUWidget* widget, char* param);
extern bool CamColorUp(ITUWidget* widget, char* param);
extern bool MC100_D6_MouseUp(ITUWidget* widget, char* param);
extern bool T400_D7_MouseUp(ITUWidget* widget, char* param);
extern bool T400_D5_MouseUp(ITUWidget* widget, char* param);
extern bool T_BTN_Press(ITUWidget* widget, char* param);
extern bool BuzzerButton(ITUWidget* widget, char* param);
extern bool UI_OK_Bt_Func(ITUWidget* widget, char* param);
extern bool UI_AUTO_Bt_Func(ITUWidget* widget, char* param);
extern bool Main_SPK_Bt_Func(ITUWidget* widget, char* param);
extern bool Main_RadioBt_Func(ITUWidget* widget, char* param);
extern bool Main_ACBt_Func(ITUWidget* widget, char* param);
extern bool MainMenu(ITUWidget* widget, char* param);
extern bool MainHome(ITUWidget* widget, char* param);
extern bool Main_ATTBt_Func(ITUWidget* widget, char* param);
extern bool Main_BarBt_Func(ITUWidget* widget, char* param);
extern bool Main_PwrBt_Func(ITUWidget* widget, char* param);
extern bool Main_HMeterBt_Func(ITUWidget* widget, char* param);
extern bool Main_TimeDateBt_Func(ITUWidget* widget, char* param);
extern bool Main_WarringBt_Func(ITUWidget* widget, char* param);
extern bool W_01_B_Stop(ITUWidget* widget, char* param);
extern bool F_01_B_Stop(ITUWidget* widget, char* param);
extern bool RD_MAIN_DnBt(ITUWidget* widget, char* param);
extern bool RD_MAIN_UpBt(ITUWidget* widget, char* param);
extern bool DAB2Combo_H_PageChanged(ITUWidget* widget, char* param);
extern bool DAB2Combo_H_MouseUp(ITUWidget* widget, char* param);
extern bool DAB2Combo_H_Select(ITUWidget* widget, char* param);
extern bool DAB2Combo_H_Button_Up(ITUWidget* widget, char* param);
extern bool DAB2Combo_H_Button_Down(ITUWidget* widget, char* param);
extern bool DAB2_ListBar_H_Changed(ITUWidget* widget, char* param);
extern bool DAB2_ListBar_Changed(ITUWidget* widget, char* param);
extern bool DAB2Combo_PageChanged(ITUWidget* widget, char* param);
extern bool DAB2Combo_MouseUp(ITUWidget* widget, char* param);
extern bool DAB2Combo_Select(ITUWidget* widget, char* param);
extern bool DAB2Combo_Button_Up(ITUWidget* widget, char* param);
extern bool AC_MAIN_DnBt(ITUWidget* widget, char* param);
extern bool AC_MAIN_UpBt(ITUWidget* widget, char* param);
extern bool EngnCoolOnEnter(ITUWidget* widget, char* param);
extern bool EngnCoolOnTimer(ITUWidget* widget, char* param);
extern bool EngnCoolOnLeave(ITUWidget* widget, char* param);
extern bool MenuTop_Enter(ITUWidget* widget, char* param);
extern bool MenuTop_Timer(ITUWidget* widget, char* param);
extern bool MenuTop_JogWheel(ITUWidget* widget, char* param);
extern bool MenuTop_JogButton(ITUWidget* widget, char* param);
extern bool MenuTop_JogSwitch(ITUWidget* widget, char* param);
extern bool MenuTop_Leave(ITUWidget* widget, char* param);
extern bool MTD7_MouseUp(ITUWidget* widget, char* param);
extern bool MT_DeepCheck(ITUWidget* widget, char* param);
extern bool MTD6_MouseUp(ITUWidget* widget, char* param);
extern bool MTD5_MouseUp(ITUWidget* widget, char* param);
extern bool M1_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M1_8_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool M1_7_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_6_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_5_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_4_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_3_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_2_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_1b_ANIStopped(ITUWidget* widget, char* param);
extern bool M1_1a_ANIStopped(ITUWidget* widget, char* param);
extern bool M2_Enter(ITUWidget* widget, char* param);
extern bool M2_Timer(ITUWidget* widget, char* param);
extern bool M2_JogWheel(ITUWidget* widget, char* param);
extern bool M2_JogButton(ITUWidget* widget, char* param);
extern bool M2_JogSwitch(ITUWidget* widget, char* param);
extern bool M2D7_MouseUp(ITUWidget* widget, char* param);
extern bool M2D7_MouseDn(ITUWidget* widget, char* param);
extern bool M2D6_MouseUp(ITUWidget* widget, char* param);
extern bool M2D5_MouseUp(ITUWidget* widget, char* param);
extern bool M2_MouseDn(ITUWidget* widget, char* param);
extern bool M2_MouseUp(ITUWidget* widget, char* param);
extern bool M3_Enter(ITUWidget* widget, char* param);
extern bool M3_Timer(ITUWidget* widget, char* param);
extern bool M3_JogWheel(ITUWidget* widget, char* param);
extern bool M3_JogButton(ITUWidget* widget, char* param);
extern bool M3_JogSwitch(ITUWidget* widget, char* param);
extern bool M3D7_MouseUp(ITUWidget* widget, char* param);
extern bool M3D6_MouseUp(ITUWidget* widget, char* param);
extern bool M3D5_MouseUp(ITUWidget* widget, char* param);
extern bool M4_Enter(ITUWidget* widget, char* param);
extern bool M4_Timer(ITUWidget* widget, char* param);
extern bool M4_JogWheel(ITUWidget* widget, char* param);
extern bool M4_JogButton(ITUWidget* widget, char* param);
extern bool M4_JpgSwitch(ITUWidget* widget, char* param);
extern bool M4_ZoomBTN_MouseDn(ITUWidget* widget, char* param);
extern bool M4_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool M4_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M4_12_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_11_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_10_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_9_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_8_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_7_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_6_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_5_ANIStopped(ITUWidget* widget, char* param);
extern bool M4_Up_MouseDn(ITUWidget* widget, char* param);
extern bool M4_Up_MouseUp(ITUWidget* widget, char* param);
extern bool M4_Dn_MouseDn(ITUWidget* widget, char* param);
extern bool M4_Dn_MouseUp(ITUWidget* widget, char* param);
extern bool M4D7_MouseDn(ITUWidget* widget, char* param);
extern bool M4D7_MouseUp(ITUWidget* widget, char* param);
extern bool M4D6_MouseUp(ITUWidget* widget, char* param);
extern bool M4D5_MouseUp(ITUWidget* widget, char* param);
extern bool M4D5_MouseDn(ITUWidget* widget, char* param);
extern bool M5_Enter(ITUWidget* widget, char* param);
extern bool M5_Timer(ITUWidget* widget, char* param);
extern bool M5_JogWheel(ITUWidget* widget, char* param);
extern bool M5_JogButton(ITUWidget* widget, char* param);
extern bool M5_JogSwitch(ITUWidget* widget, char* param);
extern bool M5D7_MouseUp(ITUWidget* widget, char* param);
extern bool M5D6_MouseUp(ITUWidget* widget, char* param);
extern bool M5D5_MouseUp(ITUWidget* widget, char* param);
extern bool M6_Enter(ITUWidget* widget, char* param);
extern bool M6_Timer(ITUWidget* widget, char* param);
extern bool M6_JogWheel(ITUWidget* widget, char* param);
extern bool M6_JogButton(ITUWidget* widget, char* param);
extern bool M6_JogSwitch(ITUWidget* widget, char* param);
extern bool M6_Date_MouseDn(ITUWidget* widget, char* param);
extern bool M6_Date_MouseUp(ITUWidget* widget, char* param);
extern bool M6_DnUp_MouseDn(ITUWidget* widget, char* param);
extern bool M6_DnUp_MouseUp(ITUWidget* widget, char* param);
extern bool M6_ZoomMouseDn(ITUWidget* widget, char* param);
extern bool M6_10_ANIStopped(ITUWidget* widget, char* param);
extern bool M6_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool M6_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M6_9_ANIStopped(ITUWidget* widget, char* param);
extern bool M6_8_ANIStopped(ITUWidget* widget, char* param);
extern bool M6_12_ANIStopped(ITUWidget* widget, char* param);
extern bool M6_11_ANIStopped(ITUWidget* widget, char* param);
extern bool M6D7_MouseDn(ITUWidget* widget, char* param);
extern bool M6D7_MouseUp(ITUWidget* widget, char* param);
extern bool M6D6_MouseUp(ITUWidget* widget, char* param);
extern bool M6D5_MouseDn(ITUWidget* widget, char* param);
extern bool M6D5_MouseUp(ITUWidget* widget, char* param);
extern bool M7_Enter(ITUWidget* widget, char* param);
extern bool M7_Timer(ITUWidget* widget, char* param);
extern bool M7_JogWheel(ITUWidget* widget, char* param);
extern bool M7_JogButton(ITUWidget* widget, char* param);
extern bool M7_JogSwitch(ITUWidget* widget, char* param);
extern bool M7_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool M7_ZoomMouseUp(ITUWidget* widget, char* param);
extern bool M7_13_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M7_12_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_11_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_4_ANIStopped(ITUWidget* widget, char* param);
extern bool M7D7_MouseUp(ITUWidget* widget, char* param);
extern bool M7D6_MouseUp(ITUWidget* widget, char* param);
extern bool M7D5_MouseUp(ITUWidget* widget, char* param);
extern bool M7_9_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_7_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_6_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_5_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_2b_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_2a_ANIStopped(ITUWidget* widget, char* param);
extern bool M7_1_ANIStopped(ITUWidget* widget, char* param);
extern bool M7S1_Enter(ITUWidget* widget, char* param);
extern bool M7S1_Timer(ITUWidget* widget, char* param);
extern bool M7S1_JogWheel(ITUWidget* widget, char* param);
extern bool M7S1_JogButton(ITUWidget* widget, char* param);
extern bool M7S1_JogSwitch(ITUWidget* widget, char* param);
extern bool S1D7_MouseDn(ITUWidget* widget, char* param);
extern bool S1D7_MouseUp(ITUWidget* widget, char* param);
extern bool S1D6_MouseUp(ITUWidget* widget, char* param);
extern bool S1D5_MouseDn(ITUWidget* widget, char* param);
extern bool S1D5_MouseUp(ITUWidget* widget, char* param);
extern bool S1_WayZoom_Dn(ITUWidget* widget, char* param);
extern bool S1_17_ANIStopped(ITUWidget* widget, char* param);
extern bool S1_WayOpt_MouseDn(ITUWidget* widget, char* param);
extern bool S1_WayOpt_MouseUp(ITUWidget* widget, char* param);
extern bool S1_16_ANIStopped(ITUWidget* widget, char* param);
extern bool S1_15_ANIStopped(ITUWidget* widget, char* param);
extern bool S1_UP_MouseDn(ITUWidget* widget, char* param);
extern bool S1_UP_MouseUp(ITUWidget* widget, char* param);
extern bool S1_DN_MouseDn(ITUWidget* widget, char* param);
extern bool S1_DN_MouseUp(ITUWidget* widget, char* param);
extern bool S1_10_MouseDn(ITUWidget* widget, char* param);
extern bool S1_10_MouseUp(ITUWidget* widget, char* param);
extern bool S1_Zoom_MouseDn(ITUWidget* widget, char* param);
extern bool S1_3_ANIStopped(ITUWidget* widget, char* param);
extern bool S1_P_MouseDn(ITUWidget* widget, char* param);
extern bool S1_P_MouseUp(ITUWidget* widget, char* param);
extern bool S1_2_ANIStopped(ITUWidget* widget, char* param);
extern bool S1_1_ANIStopped(ITUWidget* widget, char* param);
extern bool S1ATT_Enter(ITUWidget* widget, char* param);
extern bool S1ATT_Timer(ITUWidget* widget, char* param);
extern bool S1ATT_JogWheel(ITUWidget* widget, char* param);
extern bool S1ATT_JogButton(ITUWidget* widget, char* param);
extern bool S1ATT_JogSwitch(ITUWidget* widget, char* param);
extern bool ATT10_ANIStop(ITUWidget* widget, char* param);
extern bool S1ATT_MouseDn(ITUWidget* widget, char* param);
extern bool S1ATT_MouseUp(ITUWidget* widget, char* param);
extern bool ATT9_ANIStop(ITUWidget* widget, char* param);
extern bool ATT8_ANIStop(ITUWidget* widget, char* param);
extern bool ATT7_ANIStop(ITUWidget* widget, char* param);
extern bool ATT6_ANIStop(ITUWidget* widget, char* param);
extern bool ATT5_ANIStop(ITUWidget* widget, char* param);
extern bool ATT4_ANIStop(ITUWidget* widget, char* param);
extern bool ATT3_ANIStop(ITUWidget* widget, char* param);
extern bool ATT2_ANIStop(ITUWidget* widget, char* param);
extern bool ATT1_ANIStop(ITUWidget* widget, char* param);
extern bool S1ATTD7_MouseDn(ITUWidget* widget, char* param);
extern bool S1ATTD7_MouseUp(ITUWidget* widget, char* param);
extern bool S1ATTD6_MouseUp(ITUWidget* widget, char* param);
extern bool S1ATTD5_MouseDn(ITUWidget* widget, char* param);
extern bool S1ATTD5_MouseUp(ITUWidget* widget, char* param);
extern bool M7S2_Enter(ITUWidget* widget, char* param);
extern bool M7S2_Timer(ITUWidget* widget, char* param);
extern bool M7S2_JogWheel(ITUWidget* widget, char* param);
extern bool M7S2_JogButton(ITUWidget* widget, char* param);
extern bool M7S2_JogSwitch(ITUWidget* widget, char* param);
extern bool S2D7_MouseDn(ITUWidget* widget, char* param);
extern bool S2D7_MouseUp(ITUWidget* widget, char* param);
extern bool S2D6_MouseUp(ITUWidget* widget, char* param);
extern bool S2D5_MouseDn(ITUWidget* widget, char* param);
extern bool S2D5_MouseUp(ITUWidget* widget, char* param);
extern bool S2_UP_MouseDn(ITUWidget* widget, char* param);
extern bool S2_UP_MouseUp(ITUWidget* widget, char* param);
extern bool S2_DN_MouseDn(ITUWidget* widget, char* param);
extern bool S2_DN_MouseUp(ITUWidget* widget, char* param);
extern bool M7S5_Enter(ITUWidget* widget, char* param);
extern bool M7S5_Timer(ITUWidget* widget, char* param);
extern bool M7S5_JogWheel(ITUWidget* widget, char* param);
extern bool M7S5_JogButton(ITUWidget* widget, char* param);
extern bool M7S5_JogSwitch(ITUWidget* widget, char* param);
extern bool S5D7_MouseDn(ITUWidget* widget, char* param);
extern bool S5D7_MouseUp(ITUWidget* widget, char* param);
extern bool S5D6_MouseUp(ITUWidget* widget, char* param);
extern bool S5D5_MouseDn(ITUWidget* widget, char* param);
extern bool S5D5_MouseUp(ITUWidget* widget, char* param);
extern bool S5_L_MouseDn(ITUWidget* widget, char* param);
extern bool S5_L_MouseUp(ITUWidget* widget, char* param);
extern bool M7S6_Enter(ITUWidget* widget, char* param);
extern bool M7S6_Timer(ITUWidget* widget, char* param);
extern bool M7S6_JogWheel(ITUWidget* widget, char* param);
extern bool M7S6_JogButton(ITUWidget* widget, char* param);
extern bool M7S6_JogSwitch(ITUWidget* widget, char* param);
extern bool S6D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S6D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S6D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S6_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool S6_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M7S7_Enter(ITUWidget* widget, char* param);
extern bool M7S7_Timer(ITUWidget* widget, char* param);
extern bool M7S7_JogWheel(ITUWidget* widget, char* param);
extern bool M7S7_JogButton(ITUWidget* widget, char* param);
extern bool M7S7_JogSwitch(ITUWidget* widget, char* param);
extern bool S7D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S7D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S7D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S7_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool S7_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool M7S4_Enter(ITUWidget* widget, char* param);
extern bool M7S4_Timer(ITUWidget* widget, char* param);
extern bool M7S4_JogWheel(ITUWidget* widget, char* param);
extern bool M7S4_JogButton(ITUWidget* widget, char* param);
extern bool M7S4_JogSwitch(ITUWidget* widget, char* param);
extern bool M7S4_D7_MouseUp(ITUWidget* widget, char* param);
extern bool M7S4_D7_MouseDn(ITUWidget* widget, char* param);
extern bool M7S4_D6_MouseUp(ITUWidget* widget, char* param);
extern bool M7S4_D5_MouseUp(ITUWidget* widget, char* param);
extern bool M7S4_D5_MouseDn(ITUWidget* widget, char* param);
extern bool M7S4_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool M7S4_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool S11_Enter(ITUWidget* widget, char* param);
extern bool S11_Timer(ITUWidget* widget, char* param);
extern bool S11_JogWheel(ITUWidget* widget, char* param);
extern bool S11_JogButton(ITUWidget* widget, char* param);
extern bool S11_JogSwitch(ITUWidget* widget, char* param);
extern bool S11D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S11D7_BTNMouseDn(ITUWidget* widget, char* param);
extern bool S11D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S11D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool S11_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool S11_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool Menu8_Enter(ITUWidget* widget, char* param);
extern bool Menu8_Timer(ITUWidget* widget, char* param);
extern bool Menu8_Leave(ITUWidget* widget, char* param);
extern bool Menu8_JogWheel(ITUWidget* widget, char* param);
extern bool Menu8_JogButton(ITUWidget* widget, char* param);
extern bool Menu8_JogSwitch(ITUWidget* widget, char* param);
extern bool M8D7_MouseUp(ITUWidget* widget, char* param);
extern bool M8D6_MouseUp(ITUWidget* widget, char* param);
extern bool M8D5_MouseUp(ITUWidget* widget, char* param);
extern bool PastEngErr_MouseUp(ITUWidget* widget, char* param);
extern bool PastEngErr_MouseDn(ITUWidget* widget, char* param);
extern bool PastCarErr_MouseUp(ITUWidget* widget, char* param);
extern bool PastCarErr_MouseDn(ITUWidget* widget, char* param);
extern bool ACErr_MouseUp(ITUWidget* widget, char* param);
extern bool ACErr_MouseDn(ITUWidget* widget, char* param);
extern bool EngErr_MouseUp(ITUWidget* widget, char* param);
extern bool EngErr_MouseDn(ITUWidget* widget, char* param);
extern bool CarErr_MouseUp(ITUWidget* widget, char* param);
extern bool CarErr_MouseDn(ITUWidget* widget, char* param);
extern bool Menu9_Enter(ITUWidget* widget, char* param);
extern bool Menu9_Timer(ITUWidget* widget, char* param);
extern bool Menu9_JogWheel(ITUWidget* widget, char* param);
extern bool Menu9_JogButton(ITUWidget* widget, char* param);
extern bool Menu9_JogSwitch(ITUWidget* widget, char* param);
extern bool M9_UserNo(ITUWidget* widget, char* param);
extern bool M9_UserYes(ITUWidget* widget, char* param);
extern bool M9D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool M9D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool M9D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool M9_UserReset(ITUWidget* widget, char* param);
extern bool MenuBTop_Enter(ITUWidget* widget, char* param);
extern bool MenuBTop_Timer(ITUWidget* widget, char* param);
extern bool MenuBTop_JogWheel(ITUWidget* widget, char* param);
extern bool MenuBTop_JogButton(ITUWidget* widget, char* param);
extern bool MenuBTop_JogSwitch(ITUWidget* widget, char* param);
extern bool MenuBTop_Leave(ITUWidget* widget, char* param);
extern bool MBT_D7_MouseUp(ITUWidget* widget, char* param);
extern bool MBT_DeepCheck(ITUWidget* widget, char* param);
extern bool MBT_D6_MouseUp(ITUWidget* widget, char* param);
extern bool MBT_D5_MouseUp(ITUWidget* widget, char* param);
extern bool MB1_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool MB1_4_ANIStopped(ITUWidget* widget, char* param);
extern bool MB1_BTN_MouseDown(ITUWidget* widget, char* param);
extern bool MB1_3_ANIStopped(ITUWidget* widget, char* param);
extern bool MB1_2_ANIStopped(ITUWidget* widget, char* param);
extern bool MB1_1b_ANIStopped(ITUWidget* widget, char* param);
extern bool MB1_1a_ANIStopped(ITUWidget* widget, char* param);
extern bool MenuB100_Enter(ITUWidget* widget, char* param);
extern bool MenuB100_Timer(ITUWidget* widget, char* param);
extern bool MenuB100_JogWheel(ITUWidget* widget, char* param);
extern bool MenuB100_JogButton(ITUWidget* widget, char* param);
extern bool MenuB100_JogSwitch(ITUWidget* widget, char* param);
extern bool MB100_D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB100_D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB100_D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MenuB200_Enter(ITUWidget* widget, char* param);
extern bool MenuB200_Timer(ITUWidget* widget, char* param);
extern bool MenuB200_JogWheel(ITUWidget* widget, char* param);
extern bool MenuB200_JogButton(ITUWidget* widget, char* param);
extern bool MenuB200_JogSwitch(ITUWidget* widget, char* param);
extern bool MB200_D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB200_D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB200_D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MenuB300_Enter(ITUWidget* widget, char* param);
extern bool MenuB300_JogWheel(ITUWidget* widget, char* param);
extern bool MenuB300_JogButton(ITUWidget* widget, char* param);
extern bool MenuB300_JogSwitch(ITUWidget* widget, char* param);
extern bool MenuB300_Timer(ITUWidget* widget, char* param);
extern bool MB300_D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB300_D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB300_D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool MB300_C_BTN_MouseDown(ITUWidget* widget, char* param);
extern bool MB300_C_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool MenuB400_Enter(ITUWidget* widget, char* param);
extern bool MenuB400_Timer(ITUWidget* widget, char* param);
extern bool MenuB400_JogWheel(ITUWidget* widget, char* param);
extern bool MenuB400_JogButton(ITUWidget* widget, char* param);
extern bool MenuB400_JogSwitch(ITUWidget* widget, char* param);
extern bool MenuB400_Event_Cal(ITUWidget* widget, char* param);
extern bool MB400_D7_MouseUp(ITUWidget* widget, char* param);
extern bool MB400_D7_MouseDn(ITUWidget* widget, char* param);
extern bool MB400_D6_MouseUp(ITUWidget* widget, char* param);
extern bool MB400_D5_MouseUp(ITUWidget* widget, char* param);
extern bool MB400_C_BTN_MouseDown(ITUWidget* widget, char* param);
extern bool MB400_C_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_Enter(ITUWidget* widget, char* param);
extern bool SU100_Timer(ITUWidget* widget, char* param);
extern bool SU100_JogWheel(ITUWidget* widget, char* param);
extern bool SU100_JogButton(ITUWidget* widget, char* param);
extern bool SU100_JogSwitch(ITUWidget* widget, char* param);
extern bool SU100_Combo_H_PageChanged(ITUWidget* widget, char* param);
extern bool SU100_Combo_H_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_Combo_H_Select(ITUWidget* widget, char* param);
extern bool SU100_Combo_H_Button_Up(ITUWidget* widget, char* param);
extern bool SU100_Combo_H_Button_Down(ITUWidget* widget, char* param);
extern bool SU100_Combo_PageChanged(ITUWidget* widget, char* param);
extern bool SU100_Combo_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_Combo_Select(ITUWidget* widget, char* param);
extern bool SU100_Combo_Button_Up(ITUWidget* widget, char* param);
extern bool SU100_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU100_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool SU100_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool SU200_Enter(ITUWidget* widget, char* param);
extern bool SU200_Timer(ITUWidget* widget, char* param);
extern bool SU200_JogWheel(ITUWidget* widget, char* param);
extern bool SU200_JogButton(ITUWidget* widget, char* param);
extern bool SU200_JogSwitch(ITUWidget* widget, char* param);
extern bool SU200_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU200_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU200_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU200_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool SU200_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool SU300_Enter(ITUWidget* widget, char* param);
extern bool SU300_Timer(ITUWidget* widget, char* param);
extern bool SU300_JogWheel(ITUWidget* widget, char* param);
extern bool SU300_JogButton(ITUWidget* widget, char* param);
extern bool SU300_JogSwitch(ITUWidget* widget, char* param);
extern bool SU300_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU300_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU300_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU300_KeyDn(ITUWidget* widget, char* param);
extern bool SU300_KeyUp(ITUWidget* widget, char* param);
extern bool SU400_Enter(ITUWidget* widget, char* param);
extern bool SU400_Timer(ITUWidget* widget, char* param);
extern bool SU400_JogWheel(ITUWidget* widget, char* param);
extern bool SU400_JogButton(ITUWidget* widget, char* param);
extern bool SU400_JogSwitch(ITUWidget* widget, char* param);
extern bool SU400_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU400_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool SU400_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU400_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU400_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool SU500_Enter(ITUWidget* widget, char* param);
extern bool SU500_Timer(ITUWidget* widget, char* param);
extern bool SU500_JogWheel(ITUWidget* widget, char* param);
extern bool SU500_JogButton(ITUWidget* widget, char* param);
extern bool SU500_JogSwitch(ITUWidget* widget, char* param);
extern bool SU500_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool SU500_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool SU500_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU500_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU500_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU600_Enter(ITUWidget* widget, char* param);
extern bool SU600_Timer(ITUWidget* widget, char* param);
extern bool SU600_JogWheel(ITUWidget* widget, char* param);
extern bool SU600_JogButton(ITUWidget* widget, char* param);
extern bool SU600_JogSwitch(ITUWidget* widget, char* param);
extern bool SU600_D7_MouseUp(ITUWidget* widget, char* param);
extern bool SU600_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool SU600_D6_MouseUp(ITUWidget* widget, char* param);
extern bool SU600_D5_MouseUp(ITUWidget* widget, char* param);
extern bool SU600_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool MenuIM_Enter(ITUWidget* widget, char* param);
extern bool MenuIM_Timer(ITUWidget* widget, char* param);
extern bool MenuIM_JogWheel(ITUWidget* widget, char* param);
extern bool MenuIM_JogButton(ITUWidget* widget, char* param);
extern bool MenuIM_JogSwitch(ITUWidget* widget, char* param);
extern bool IM_D7_BTNMouseUp(ITUWidget* widget, char* param);
extern bool IM_D6_BTNMouseUp(ITUWidget* widget, char* param);
extern bool IM_D5_BTNMouseUp(ITUWidget* widget, char* param);
extern bool IM_BTN_MouseUp(ITUWidget* widget, char* param);
extern bool IM_ANIStopped(ITUWidget* widget, char* param);
extern bool IM_BTN_MouseDown(ITUWidget* widget, char* param);
extern bool PW100_Enter(ITUWidget* widget, char* param);
extern bool PW100_Timer(ITUWidget* widget, char* param);
extern bool PW100_JogWheel(ITUWidget* widget, char* param);
extern bool PW100_JogButton(ITUWidget* widget, char* param);
extern bool PW100_JogSwitch(ITUWidget* widget, char* param);
extern bool PW100_D7_MouseUp(ITUWidget* widget, char* param);
extern bool PW100_BTN_MouseDn(ITUWidget* widget, char* param);
extern bool PW100_D6_MouseUp(ITUWidget* widget, char* param);
extern bool PW100_D5_MouseUp(ITUWidget* widget, char* param);
extern bool PW100_BTN_MouseUp(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "LogoOnEnter", LogoOnEnter,
    "LogoOnTimer", LogoOnTimer,
    "LogoOnLeave", LogoOnLeave,
    "LogoJDR", LogoJDR,
    "LogoJDK", LogoJDK,
    "LogoJDS", LogoJDS,
    "Logo_USB_Inserted", Logo_USB_Inserted,
    "USER_PW_DnBt_Func", USER_PW_DnBt_Func,
    "USER_PW_UpBt_Func", USER_PW_UpBt_Func,
    "MainOnEnter", MainOnEnter,
    "MainOnTimer", MainOnTimer,
    "MainOnLeave", MainOnLeave,
    "JogDialRotation", JogDialRotation,
    "JogDialKey", JogDialKey,
    "JogDialSwitch", JogDialSwitch,
    "Main_USB_Inserted", Main_USB_Inserted,
    "MainCamera", MainCamera,
    "CamColorMenu", CamColorMenu,
    "CamColorNight", CamColorNight,
    "CamColorDay", CamColorDay,
    "CamColorSha", CamColorSha,
    "CamColorHue", CamColorHue,
    "CamColorSat", CamColorSat,
    "CamColorCon", CamColorCon,
    "CamColorBr", CamColorBr,
    "CamColorDown", CamColorDown,
    "CamColorUp", CamColorUp,
    "MC100_D6_MouseUp", MC100_D6_MouseUp,
    "T400_D7_MouseUp", T400_D7_MouseUp,
    "T400_D5_MouseUp", T400_D5_MouseUp,
    "T_BTN_Press", T_BTN_Press,
    "BuzzerButton", BuzzerButton,
    "UI_OK_Bt_Func", UI_OK_Bt_Func,
    "UI_AUTO_Bt_Func", UI_AUTO_Bt_Func,
    "Main_SPK_Bt_Func", Main_SPK_Bt_Func,
    "Main_RadioBt_Func", Main_RadioBt_Func,
    "Main_ACBt_Func", Main_ACBt_Func,
    "MainMenu", MainMenu,
    "MainHome", MainHome,
    "Main_ATTBt_Func", Main_ATTBt_Func,
    "Main_BarBt_Func", Main_BarBt_Func,
    "Main_PwrBt_Func", Main_PwrBt_Func,
    "Main_HMeterBt_Func", Main_HMeterBt_Func,
    "Main_TimeDateBt_Func", Main_TimeDateBt_Func,
    "Main_WarringBt_Func", Main_WarringBt_Func,
    "W_01_B_Stop", W_01_B_Stop,
    "F_01_B_Stop", F_01_B_Stop,
    "RD_MAIN_DnBt", RD_MAIN_DnBt,
    "RD_MAIN_UpBt", RD_MAIN_UpBt,
    "DAB2Combo_H_PageChanged", DAB2Combo_H_PageChanged,
    "DAB2Combo_H_MouseUp", DAB2Combo_H_MouseUp,
    "DAB2Combo_H_Select", DAB2Combo_H_Select,
    "DAB2Combo_H_Button_Up", DAB2Combo_H_Button_Up,
    "DAB2Combo_H_Button_Down", DAB2Combo_H_Button_Down,
    "DAB2_ListBar_H_Changed", DAB2_ListBar_H_Changed,
    "DAB2_ListBar_Changed", DAB2_ListBar_Changed,
    "DAB2Combo_PageChanged", DAB2Combo_PageChanged,
    "DAB2Combo_MouseUp", DAB2Combo_MouseUp,
    "DAB2Combo_Select", DAB2Combo_Select,
    "DAB2Combo_Button_Up", DAB2Combo_Button_Up,
    "AC_MAIN_DnBt", AC_MAIN_DnBt,
    "AC_MAIN_UpBt", AC_MAIN_UpBt,
    "EngnCoolOnEnter", EngnCoolOnEnter,
    "EngnCoolOnTimer", EngnCoolOnTimer,
    "EngnCoolOnLeave", EngnCoolOnLeave,
    "MenuTop_Enter", MenuTop_Enter,
    "MenuTop_Timer", MenuTop_Timer,
    "MenuTop_JogWheel", MenuTop_JogWheel,
    "MenuTop_JogButton", MenuTop_JogButton,
    "MenuTop_JogSwitch", MenuTop_JogSwitch,
    "MenuTop_Leave", MenuTop_Leave,
    "MTD7_MouseUp", MTD7_MouseUp,
    "MT_DeepCheck", MT_DeepCheck,
    "MTD6_MouseUp", MTD6_MouseUp,
    "MTD5_MouseUp", MTD5_MouseUp,
    "M1_BTN_MouseUp", M1_BTN_MouseUp,
    "M1_8_ANIStopped", M1_8_ANIStopped,
    "M1_BTN_MouseDn", M1_BTN_MouseDn,
    "M1_7_ANIStopped", M1_7_ANIStopped,
    "M1_6_ANIStopped", M1_6_ANIStopped,
    "M1_5_ANIStopped", M1_5_ANIStopped,
    "M1_4_ANIStopped", M1_4_ANIStopped,
    "M1_3_ANIStopped", M1_3_ANIStopped,
    "M1_2_ANIStopped", M1_2_ANIStopped,
    "M1_1b_ANIStopped", M1_1b_ANIStopped,
    "M1_1a_ANIStopped", M1_1a_ANIStopped,
    "M2_Enter", M2_Enter,
    "M2_Timer", M2_Timer,
    "M2_JogWheel", M2_JogWheel,
    "M2_JogButton", M2_JogButton,
    "M2_JogSwitch", M2_JogSwitch,
    "M2D7_MouseUp", M2D7_MouseUp,
    "M2D7_MouseDn", M2D7_MouseDn,
    "M2D6_MouseUp", M2D6_MouseUp,
    "M2D5_MouseUp", M2D5_MouseUp,
    "M2_MouseDn", M2_MouseDn,
    "M2_MouseUp", M2_MouseUp,
    "M3_Enter", M3_Enter,
    "M3_Timer", M3_Timer,
    "M3_JogWheel", M3_JogWheel,
    "M3_JogButton", M3_JogButton,
    "M3_JogSwitch", M3_JogSwitch,
    "M3D7_MouseUp", M3D7_MouseUp,
    "M3D6_MouseUp", M3D6_MouseUp,
    "M3D5_MouseUp", M3D5_MouseUp,
    "M4_Enter", M4_Enter,
    "M4_Timer", M4_Timer,
    "M4_JogWheel", M4_JogWheel,
    "M4_JogButton", M4_JogButton,
    "M4_JpgSwitch", M4_JpgSwitch,
    "M4_ZoomBTN_MouseDn", M4_ZoomBTN_MouseDn,
    "M4_BTN_MouseDn", M4_BTN_MouseDn,
    "M4_BTN_MouseUp", M4_BTN_MouseUp,
    "M4_12_ANIStopped", M4_12_ANIStopped,
    "M4_11_ANIStopped", M4_11_ANIStopped,
    "M4_10_ANIStopped", M4_10_ANIStopped,
    "M4_9_ANIStopped", M4_9_ANIStopped,
    "M4_8_ANIStopped", M4_8_ANIStopped,
    "M4_7_ANIStopped", M4_7_ANIStopped,
    "M4_6_ANIStopped", M4_6_ANIStopped,
    "M4_5_ANIStopped", M4_5_ANIStopped,
    "M4_Up_MouseDn", M4_Up_MouseDn,
    "M4_Up_MouseUp", M4_Up_MouseUp,
    "M4_Dn_MouseDn", M4_Dn_MouseDn,
    "M4_Dn_MouseUp", M4_Dn_MouseUp,
    "M4D7_MouseDn", M4D7_MouseDn,
    "M4D7_MouseUp", M4D7_MouseUp,
    "M4D6_MouseUp", M4D6_MouseUp,
    "M4D5_MouseUp", M4D5_MouseUp,
    "M4D5_MouseDn", M4D5_MouseDn,
    "M5_Enter", M5_Enter,
    "M5_Timer", M5_Timer,
    "M5_JogWheel", M5_JogWheel,
    "M5_JogButton", M5_JogButton,
    "M5_JogSwitch", M5_JogSwitch,
    "M5D7_MouseUp", M5D7_MouseUp,
    "M5D6_MouseUp", M5D6_MouseUp,
    "M5D5_MouseUp", M5D5_MouseUp,
    "M6_Enter", M6_Enter,
    "M6_Timer", M6_Timer,
    "M6_JogWheel", M6_JogWheel,
    "M6_JogButton", M6_JogButton,
    "M6_JogSwitch", M6_JogSwitch,
    "M6_Date_MouseDn", M6_Date_MouseDn,
    "M6_Date_MouseUp", M6_Date_MouseUp,
    "M6_DnUp_MouseDn", M6_DnUp_MouseDn,
    "M6_DnUp_MouseUp", M6_DnUp_MouseUp,
    "M6_ZoomMouseDn", M6_ZoomMouseDn,
    "M6_10_ANIStopped", M6_10_ANIStopped,
    "M6_BTN_MouseDn", M6_BTN_MouseDn,
    "M6_BTN_MouseUp", M6_BTN_MouseUp,
    "M6_9_ANIStopped", M6_9_ANIStopped,
    "M6_8_ANIStopped", M6_8_ANIStopped,
    "M6_12_ANIStopped", M6_12_ANIStopped,
    "M6_11_ANIStopped", M6_11_ANIStopped,
    "M6D7_MouseDn", M6D7_MouseDn,
    "M6D7_MouseUp", M6D7_MouseUp,
    "M6D6_MouseUp", M6D6_MouseUp,
    "M6D5_MouseDn", M6D5_MouseDn,
    "M6D5_MouseUp", M6D5_MouseUp,
    "M7_Enter", M7_Enter,
    "M7_Timer", M7_Timer,
    "M7_JogWheel", M7_JogWheel,
    "M7_JogButton", M7_JogButton,
    "M7_JogSwitch", M7_JogSwitch,
    "M7_BTN_MouseDn", M7_BTN_MouseDn,
    "M7_ZoomMouseUp", M7_ZoomMouseUp,
    "M7_13_ANIStopped", M7_13_ANIStopped,
    "M7_BTN_MouseUp", M7_BTN_MouseUp,
    "M7_12_ANIStopped", M7_12_ANIStopped,
    "M7_11_ANIStopped", M7_11_ANIStopped,
    "M7_4_ANIStopped", M7_4_ANIStopped,
    "M7D7_MouseUp", M7D7_MouseUp,
    "M7D6_MouseUp", M7D6_MouseUp,
    "M7D5_MouseUp", M7D5_MouseUp,
    "M7_9_ANIStopped", M7_9_ANIStopped,
    "M7_7_ANIStopped", M7_7_ANIStopped,
    "M7_6_ANIStopped", M7_6_ANIStopped,
    "M7_5_ANIStopped", M7_5_ANIStopped,
    "M7_2b_ANIStopped", M7_2b_ANIStopped,
    "M7_2a_ANIStopped", M7_2a_ANIStopped,
    "M7_1_ANIStopped", M7_1_ANIStopped,
    "M7S1_Enter", M7S1_Enter,
    "M7S1_Timer", M7S1_Timer,
    "M7S1_JogWheel", M7S1_JogWheel,
    "M7S1_JogButton", M7S1_JogButton,
    "M7S1_JogSwitch", M7S1_JogSwitch,
    "S1D7_MouseDn", S1D7_MouseDn,
    "S1D7_MouseUp", S1D7_MouseUp,
    "S1D6_MouseUp", S1D6_MouseUp,
    "S1D5_MouseDn", S1D5_MouseDn,
    "S1D5_MouseUp", S1D5_MouseUp,
    "S1_WayZoom_Dn", S1_WayZoom_Dn,
    "S1_17_ANIStopped", S1_17_ANIStopped,
    "S1_WayOpt_MouseDn", S1_WayOpt_MouseDn,
    "S1_WayOpt_MouseUp", S1_WayOpt_MouseUp,
    "S1_16_ANIStopped", S1_16_ANIStopped,
    "S1_15_ANIStopped", S1_15_ANIStopped,
    "S1_UP_MouseDn", S1_UP_MouseDn,
    "S1_UP_MouseUp", S1_UP_MouseUp,
    "S1_DN_MouseDn", S1_DN_MouseDn,
    "S1_DN_MouseUp", S1_DN_MouseUp,
    "S1_10_MouseDn", S1_10_MouseDn,
    "S1_10_MouseUp", S1_10_MouseUp,
    "S1_Zoom_MouseDn", S1_Zoom_MouseDn,
    "S1_3_ANIStopped", S1_3_ANIStopped,
    "S1_P_MouseDn", S1_P_MouseDn,
    "S1_P_MouseUp", S1_P_MouseUp,
    "S1_2_ANIStopped", S1_2_ANIStopped,
    "S1_1_ANIStopped", S1_1_ANIStopped,
    "S1ATT_Enter", S1ATT_Enter,
    "S1ATT_Timer", S1ATT_Timer,
    "S1ATT_JogWheel", S1ATT_JogWheel,
    "S1ATT_JogButton", S1ATT_JogButton,
    "S1ATT_JogSwitch", S1ATT_JogSwitch,
    "ATT10_ANIStop", ATT10_ANIStop,
    "S1ATT_MouseDn", S1ATT_MouseDn,
    "S1ATT_MouseUp", S1ATT_MouseUp,
    "ATT9_ANIStop", ATT9_ANIStop,
    "ATT8_ANIStop", ATT8_ANIStop,
    "ATT7_ANIStop", ATT7_ANIStop,
    "ATT6_ANIStop", ATT6_ANIStop,
    "ATT5_ANIStop", ATT5_ANIStop,
    "ATT4_ANIStop", ATT4_ANIStop,
    "ATT3_ANIStop", ATT3_ANIStop,
    "ATT2_ANIStop", ATT2_ANIStop,
    "ATT1_ANIStop", ATT1_ANIStop,
    "S1ATTD7_MouseDn", S1ATTD7_MouseDn,
    "S1ATTD7_MouseUp", S1ATTD7_MouseUp,
    "S1ATTD6_MouseUp", S1ATTD6_MouseUp,
    "S1ATTD5_MouseDn", S1ATTD5_MouseDn,
    "S1ATTD5_MouseUp", S1ATTD5_MouseUp,
    "M7S2_Enter", M7S2_Enter,
    "M7S2_Timer", M7S2_Timer,
    "M7S2_JogWheel", M7S2_JogWheel,
    "M7S2_JogButton", M7S2_JogButton,
    "M7S2_JogSwitch", M7S2_JogSwitch,
    "S2D7_MouseDn", S2D7_MouseDn,
    "S2D7_MouseUp", S2D7_MouseUp,
    "S2D6_MouseUp", S2D6_MouseUp,
    "S2D5_MouseDn", S2D5_MouseDn,
    "S2D5_MouseUp", S2D5_MouseUp,
    "S2_UP_MouseDn", S2_UP_MouseDn,
    "S2_UP_MouseUp", S2_UP_MouseUp,
    "S2_DN_MouseDn", S2_DN_MouseDn,
    "S2_DN_MouseUp", S2_DN_MouseUp,
    "M7S5_Enter", M7S5_Enter,
    "M7S5_Timer", M7S5_Timer,
    "M7S5_JogWheel", M7S5_JogWheel,
    "M7S5_JogButton", M7S5_JogButton,
    "M7S5_JogSwitch", M7S5_JogSwitch,
    "S5D7_MouseDn", S5D7_MouseDn,
    "S5D7_MouseUp", S5D7_MouseUp,
    "S5D6_MouseUp", S5D6_MouseUp,
    "S5D5_MouseDn", S5D5_MouseDn,
    "S5D5_MouseUp", S5D5_MouseUp,
    "S5_L_MouseDn", S5_L_MouseDn,
    "S5_L_MouseUp", S5_L_MouseUp,
    "M7S6_Enter", M7S6_Enter,
    "M7S6_Timer", M7S6_Timer,
    "M7S6_JogWheel", M7S6_JogWheel,
    "M7S6_JogButton", M7S6_JogButton,
    "M7S6_JogSwitch", M7S6_JogSwitch,
    "S6D7_BTNMouseUp", S6D7_BTNMouseUp,
    "S6D6_BTNMouseUp", S6D6_BTNMouseUp,
    "S6D5_BTNMouseUp", S6D5_BTNMouseUp,
    "S6_BTN_MouseDn", S6_BTN_MouseDn,
    "S6_BTN_MouseUp", S6_BTN_MouseUp,
    "M7S7_Enter", M7S7_Enter,
    "M7S7_Timer", M7S7_Timer,
    "M7S7_JogWheel", M7S7_JogWheel,
    "M7S7_JogButton", M7S7_JogButton,
    "M7S7_JogSwitch", M7S7_JogSwitch,
    "S7D7_BTNMouseUp", S7D7_BTNMouseUp,
    "S7D6_BTNMouseUp", S7D6_BTNMouseUp,
    "S7D5_BTNMouseUp", S7D5_BTNMouseUp,
    "S7_BTN_MouseDn", S7_BTN_MouseDn,
    "S7_BTN_MouseUp", S7_BTN_MouseUp,
    "M7S4_Enter", M7S4_Enter,
    "M7S4_Timer", M7S4_Timer,
    "M7S4_JogWheel", M7S4_JogWheel,
    "M7S4_JogButton", M7S4_JogButton,
    "M7S4_JogSwitch", M7S4_JogSwitch,
    "M7S4_D7_MouseUp", M7S4_D7_MouseUp,
    "M7S4_D7_MouseDn", M7S4_D7_MouseDn,
    "M7S4_D6_MouseUp", M7S4_D6_MouseUp,
    "M7S4_D5_MouseUp", M7S4_D5_MouseUp,
    "M7S4_D5_MouseDn", M7S4_D5_MouseDn,
    "M7S4_BTN_MouseDn", M7S4_BTN_MouseDn,
    "M7S4_BTN_MouseUp", M7S4_BTN_MouseUp,
    "S11_Enter", S11_Enter,
    "S11_Timer", S11_Timer,
    "S11_JogWheel", S11_JogWheel,
    "S11_JogButton", S11_JogButton,
    "S11_JogSwitch", S11_JogSwitch,
    "S11D7_BTNMouseUp", S11D7_BTNMouseUp,
    "S11D7_BTNMouseDn", S11D7_BTNMouseDn,
    "S11D6_BTNMouseUp", S11D6_BTNMouseUp,
    "S11D5_BTNMouseUp", S11D5_BTNMouseUp,
    "S11_BTN_MouseDn", S11_BTN_MouseDn,
    "S11_BTN_MouseUp", S11_BTN_MouseUp,
    "Menu8_Enter", Menu8_Enter,
    "Menu8_Timer", Menu8_Timer,
    "Menu8_Leave", Menu8_Leave,
    "Menu8_JogWheel", Menu8_JogWheel,
    "Menu8_JogButton", Menu8_JogButton,
    "Menu8_JogSwitch", Menu8_JogSwitch,
    "M8D7_MouseUp", M8D7_MouseUp,
    "M8D6_MouseUp", M8D6_MouseUp,
    "M8D5_MouseUp", M8D5_MouseUp,
    "PastEngErr_MouseUp", PastEngErr_MouseUp,
    "PastEngErr_MouseDn", PastEngErr_MouseDn,
    "PastCarErr_MouseUp", PastCarErr_MouseUp,
    "PastCarErr_MouseDn", PastCarErr_MouseDn,
    "ACErr_MouseUp", ACErr_MouseUp,
    "ACErr_MouseDn", ACErr_MouseDn,
    "EngErr_MouseUp", EngErr_MouseUp,
    "EngErr_MouseDn", EngErr_MouseDn,
    "CarErr_MouseUp", CarErr_MouseUp,
    "CarErr_MouseDn", CarErr_MouseDn,
    "Menu9_Enter", Menu9_Enter,
    "Menu9_Timer", Menu9_Timer,
    "Menu9_JogWheel", Menu9_JogWheel,
    "Menu9_JogButton", Menu9_JogButton,
    "Menu9_JogSwitch", Menu9_JogSwitch,
    "M9_UserNo", M9_UserNo,
    "M9_UserYes", M9_UserYes,
    "M9D7_BTNMouseUp", M9D7_BTNMouseUp,
    "M9D6_BTNMouseUp", M9D6_BTNMouseUp,
    "M9D5_BTNMouseUp", M9D5_BTNMouseUp,
    "M9_UserReset", M9_UserReset,
    "MenuBTop_Enter", MenuBTop_Enter,
    "MenuBTop_Timer", MenuBTop_Timer,
    "MenuBTop_JogWheel", MenuBTop_JogWheel,
    "MenuBTop_JogButton", MenuBTop_JogButton,
    "MenuBTop_JogSwitch", MenuBTop_JogSwitch,
    "MenuBTop_Leave", MenuBTop_Leave,
    "MBT_D7_MouseUp", MBT_D7_MouseUp,
    "MBT_DeepCheck", MBT_DeepCheck,
    "MBT_D6_MouseUp", MBT_D6_MouseUp,
    "MBT_D5_MouseUp", MBT_D5_MouseUp,
    "MB1_BTN_MouseUp", MB1_BTN_MouseUp,
    "MB1_4_ANIStopped", MB1_4_ANIStopped,
    "MB1_BTN_MouseDown", MB1_BTN_MouseDown,
    "MB1_3_ANIStopped", MB1_3_ANIStopped,
    "MB1_2_ANIStopped", MB1_2_ANIStopped,
    "MB1_1b_ANIStopped", MB1_1b_ANIStopped,
    "MB1_1a_ANIStopped", MB1_1a_ANIStopped,
    "MenuB100_Enter", MenuB100_Enter,
    "MenuB100_Timer", MenuB100_Timer,
    "MenuB100_JogWheel", MenuB100_JogWheel,
    "MenuB100_JogButton", MenuB100_JogButton,
    "MenuB100_JogSwitch", MenuB100_JogSwitch,
    "MB100_D7_BTNMouseUp", MB100_D7_BTNMouseUp,
    "MB100_D6_BTNMouseUp", MB100_D6_BTNMouseUp,
    "MB100_D5_BTNMouseUp", MB100_D5_BTNMouseUp,
    "MenuB200_Enter", MenuB200_Enter,
    "MenuB200_Timer", MenuB200_Timer,
    "MenuB200_JogWheel", MenuB200_JogWheel,
    "MenuB200_JogButton", MenuB200_JogButton,
    "MenuB200_JogSwitch", MenuB200_JogSwitch,
    "MB200_D7_BTNMouseUp", MB200_D7_BTNMouseUp,
    "MB200_D6_BTNMouseUp", MB200_D6_BTNMouseUp,
    "MB200_D5_BTNMouseUp", MB200_D5_BTNMouseUp,
    "MenuB300_Enter", MenuB300_Enter,
    "MenuB300_JogWheel", MenuB300_JogWheel,
    "MenuB300_JogButton", MenuB300_JogButton,
    "MenuB300_JogSwitch", MenuB300_JogSwitch,
    "MenuB300_Timer", MenuB300_Timer,
    "MB300_D7_BTNMouseUp", MB300_D7_BTNMouseUp,
    "MB300_D6_BTNMouseUp", MB300_D6_BTNMouseUp,
    "MB300_D5_BTNMouseUp", MB300_D5_BTNMouseUp,
    "MB300_C_BTN_MouseDown", MB300_C_BTN_MouseDown,
    "MB300_C_BTN_MouseUp", MB300_C_BTN_MouseUp,
    "MenuB400_Enter", MenuB400_Enter,
    "MenuB400_Timer", MenuB400_Timer,
    "MenuB400_JogWheel", MenuB400_JogWheel,
    "MenuB400_JogButton", MenuB400_JogButton,
    "MenuB400_JogSwitch", MenuB400_JogSwitch,
    "MenuB400_Event_Cal", MenuB400_Event_Cal,
    "MB400_D7_MouseUp", MB400_D7_MouseUp,
    "MB400_D7_MouseDn", MB400_D7_MouseDn,
    "MB400_D6_MouseUp", MB400_D6_MouseUp,
    "MB400_D5_MouseUp", MB400_D5_MouseUp,
    "MB400_C_BTN_MouseDown", MB400_C_BTN_MouseDown,
    "MB400_C_BTN_MouseUp", MB400_C_BTN_MouseUp,
    "SU100_Enter", SU100_Enter,
    "SU100_Timer", SU100_Timer,
    "SU100_JogWheel", SU100_JogWheel,
    "SU100_JogButton", SU100_JogButton,
    "SU100_JogSwitch", SU100_JogSwitch,
    "SU100_Combo_H_PageChanged", SU100_Combo_H_PageChanged,
    "SU100_Combo_H_MouseUp", SU100_Combo_H_MouseUp,
    "SU100_Combo_H_Select", SU100_Combo_H_Select,
    "SU100_Combo_H_Button_Up", SU100_Combo_H_Button_Up,
    "SU100_Combo_H_Button_Down", SU100_Combo_H_Button_Down,
    "SU100_Combo_PageChanged", SU100_Combo_PageChanged,
    "SU100_Combo_MouseUp", SU100_Combo_MouseUp,
    "SU100_Combo_Select", SU100_Combo_Select,
    "SU100_Combo_Button_Up", SU100_Combo_Button_Up,
    "SU100_D7_MouseUp", SU100_D7_MouseUp,
    "SU100_D6_MouseUp", SU100_D6_MouseUp,
    "SU100_D5_MouseUp", SU100_D5_MouseUp,
    "SU100_BTN_MouseDn", SU100_BTN_MouseDn,
    "SU100_BTN_MouseUp", SU100_BTN_MouseUp,
    "SU200_Enter", SU200_Enter,
    "SU200_Timer", SU200_Timer,
    "SU200_JogWheel", SU200_JogWheel,
    "SU200_JogButton", SU200_JogButton,
    "SU200_JogSwitch", SU200_JogSwitch,
    "SU200_D7_MouseUp", SU200_D7_MouseUp,
    "SU200_D6_MouseUp", SU200_D6_MouseUp,
    "SU200_D5_MouseUp", SU200_D5_MouseUp,
    "SU200_BTN_MouseDn", SU200_BTN_MouseDn,
    "SU200_BTN_MouseUp", SU200_BTN_MouseUp,
    "SU300_Enter", SU300_Enter,
    "SU300_Timer", SU300_Timer,
    "SU300_JogWheel", SU300_JogWheel,
    "SU300_JogButton", SU300_JogButton,
    "SU300_JogSwitch", SU300_JogSwitch,
    "SU300_D7_MouseUp", SU300_D7_MouseUp,
    "SU300_D6_MouseUp", SU300_D6_MouseUp,
    "SU300_D5_MouseUp", SU300_D5_MouseUp,
    "SU300_KeyDn", SU300_KeyDn,
    "SU300_KeyUp", SU300_KeyUp,
    "SU400_Enter", SU400_Enter,
    "SU400_Timer", SU400_Timer,
    "SU400_JogWheel", SU400_JogWheel,
    "SU400_JogButton", SU400_JogButton,
    "SU400_JogSwitch", SU400_JogSwitch,
    "SU400_D7_MouseUp", SU400_D7_MouseUp,
    "SU400_BTN_MouseDn", SU400_BTN_MouseDn,
    "SU400_D6_MouseUp", SU400_D6_MouseUp,
    "SU400_D5_MouseUp", SU400_D5_MouseUp,
    "SU400_BTN_MouseUp", SU400_BTN_MouseUp,
    "SU500_Enter", SU500_Enter,
    "SU500_Timer", SU500_Timer,
    "SU500_JogWheel", SU500_JogWheel,
    "SU500_JogButton", SU500_JogButton,
    "SU500_JogSwitch", SU500_JogSwitch,
    "SU500_BTN_MouseDn", SU500_BTN_MouseDn,
    "SU500_BTN_MouseUp", SU500_BTN_MouseUp,
    "SU500_D7_MouseUp", SU500_D7_MouseUp,
    "SU500_D6_MouseUp", SU500_D6_MouseUp,
    "SU500_D5_MouseUp", SU500_D5_MouseUp,
    "SU600_Enter", SU600_Enter,
    "SU600_Timer", SU600_Timer,
    "SU600_JogWheel", SU600_JogWheel,
    "SU600_JogButton", SU600_JogButton,
    "SU600_JogSwitch", SU600_JogSwitch,
    "SU600_D7_MouseUp", SU600_D7_MouseUp,
    "SU600_BTN_MouseDn", SU600_BTN_MouseDn,
    "SU600_D6_MouseUp", SU600_D6_MouseUp,
    "SU600_D5_MouseUp", SU600_D5_MouseUp,
    "SU600_BTN_MouseUp", SU600_BTN_MouseUp,
    "MenuIM_Enter", MenuIM_Enter,
    "MenuIM_Timer", MenuIM_Timer,
    "MenuIM_JogWheel", MenuIM_JogWheel,
    "MenuIM_JogButton", MenuIM_JogButton,
    "MenuIM_JogSwitch", MenuIM_JogSwitch,
    "IM_D7_BTNMouseUp", IM_D7_BTNMouseUp,
    "IM_D6_BTNMouseUp", IM_D6_BTNMouseUp,
    "IM_D5_BTNMouseUp", IM_D5_BTNMouseUp,
    "IM_BTN_MouseUp", IM_BTN_MouseUp,
    "IM_ANIStopped", IM_ANIStopped,
    "IM_BTN_MouseDown", IM_BTN_MouseDown,
    "PW100_Enter", PW100_Enter,
    "PW100_Timer", PW100_Timer,
    "PW100_JogWheel", PW100_JogWheel,
    "PW100_JogButton", PW100_JogButton,
    "PW100_JogSwitch", PW100_JogSwitch,
    "PW100_D7_MouseUp", PW100_D7_MouseUp,
    "PW100_BTN_MouseDn", PW100_BTN_MouseDn,
    "PW100_D6_MouseUp", PW100_D6_MouseUp,
    "PW100_D5_MouseUp", PW100_D5_MouseUp,
    "PW100_BTN_MouseUp", PW100_BTN_MouseUp,
    NULL, NULL
};
