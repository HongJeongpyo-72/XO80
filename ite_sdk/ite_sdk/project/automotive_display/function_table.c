#include "ite/itu.h"

extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool mainStallsAnimationOnStop(ITUWidget* widget, char* param);
extern bool MainMusicAnimationOnStop(ITUWidget* widget, char* param);
extern bool MainBeamAnimationOnStop(ITUWidget* widget, char* param);
extern bool MainReTestButtonOnPress(ITUWidget* widget, char* param);
extern bool MainNextBGButtonOnPress(ITUWidget* widget, char* param);
extern bool MainOnLeave(ITUWidget* widget, char* param);


extern bool DigitalTypeOnEnter(ITUWidget* widget, char* param);
extern bool DigitalTypeOnTimer(ITUWidget* widget, char* param);
extern bool DigitalTypeOnLeave(ITUWidget* widget, char* param);
extern bool DigitalTypeChangeUIBtOnMouseUp(ITUWidget* widget, char* param);
extern bool DigitalTypeTempAnOnStopped(ITUWidget* widget, char* param);
extern bool DigitalTypInfoUpSpriteOnStopped(ITUWidget* widget, char* param);
extern bool DigitalTypInfoDownSpriteOnStopped(ITUWidget* widget, char* param);
extern bool DigitalTypeInfoAnOnStopped(ITUWidget* widget, char* param);
extern bool DigitalTypePlayBtOnMouseUp(ITUWidget* widget, char* param);
//extern bool DigitalTypeBackButtonOnMouseUp(ITUWidget* widget, char* param);

//extern bool DigitalTypeWiFiButtonOnPress(ITUWidget* widget, char* param);

extern bool ProjectOnEnter(ITUWidget* widget, char* param);
extern bool ProjectOnTimer(ITUWidget* widget, char* param);
extern bool ProjectOnLeave(ITUWidget* widget, char* param);
extern bool ProjectChangeUIBtOnMouseUp(ITUWidget* widget, char* param);
extern bool ProjectTempAnOnStopped(ITUWidget* widget, char* param);
extern bool ProjectPlayBtOnMouseUp(ITUWidget* widget, char* param);
extern bool ProjectWiFiButtonOnPress(ITUWidget* widget, char* param);
extern bool ProjectShowReverseBtOnMouseUp(ITUWidget* widget, char* param);
extern bool projectGoHomeBtOnMouseUp(ITUWidget* widget, char* param);

extern bool DashboardCamOnEnter(ITUWidget* widget, char* param);
extern bool DashboardCamOnTimer(ITUWidget* widget, char* param);
extern bool DashboardCamOnLeave(ITUWidget* widget, char* param);
extern bool DashboardCamPlayBtOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamPlaybackViewBackBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashbaordCamPlaybackVideoBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamPlaybackNextBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamPlaybackPrevBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamLoopRadioBoxOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamPlaybackMenuBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamRecordStopBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamMenuBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool DashboardCamPlaybackMenuBackBtnOnMouseUp(ITUWidget* widget, char* param);

extern bool OpeningOnEnter(ITUWidget* widget, char* param);
extern bool OpeningOnTimer(ITUWidget* widget, char* param);
extern bool OpenOnLeave(ITUWidget* widget, char* param);

extern bool digitalSpeedButtonOnPress(ITUWidget* widget, char* param);
extern bool digitalDelayButtonOnPress(ITUWidget* widget, char* param);

extern bool BackupCamOnEnter(ITUWidget* widget, char* param);
extern bool BackupCamOnLeave(ITUWidget* widget, char* param);
extern bool BackupCamOnTimer(ITUWidget* widget, char* param);
extern bool BackupCamReverseBtOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamLeftBtOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamRearBtOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackViewBackBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackVideoBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackNextBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackPrevBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamLoopRadioBoxOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackMenuBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamRecordStopBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamMenuBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool BackupCamPlaybackMenuBackBtnOnMouseUp(ITUWidget* widget, char* param);

extern bool SurveillanceOnEnter(ITUWidget* widget, char* param);
extern bool SurveillanceOnLeave(ITUWidget* widget, char* param);
extern bool SurveillanceOnTimer(ITUWidget* widget, char* param);
extern bool SurveillanceReverseBtOnMouseUp(ITUWidget* widget, char* param);
extern bool SurveillanceQuadBtOnMouseUp(ITUWidget* widget, char* param);
extern bool SurveillanceRearBtOnMouseUp(ITUWidget* widget, char* param);

extern bool LightOnEnter(ITUWidget* widget, char* param);
extern bool LightBackBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool LightFrontBtnOnMouseUp(ITUWidget* widget, char* param);

extern bool WindowOnEnter(ITUWidget* widget, char* param);
extern bool WindowLeftBackBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool WindowLeftFrontBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool WindowRightBackBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool WindowRightFrontBtnOnMouseUp(ITUWidget* widget, char* param);


extern bool SeatOnEnter(ITUWidget* widget, char* param);
extern bool SeatBackDeBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool SeatBackAddBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool SeatLegDeBtnOnMouseUp(ITUWidget* widget, char* param);
extern bool SeatLegAddBtnOnMouseUp(ITUWidget* widget, char* param);

extern bool SettingOnEnter(ITUWidget* widget, char* param);
extern bool SettingOnLeave(ITUWidget* widget, char* param);
extern bool SettingProtectTimeTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingProtectTimeUpOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingProtectTimeDownOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingVoiceTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingVoiceUpOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingVoiceDownOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingLightTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingLightUpOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingLightDownOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingChtRadioBoxOnMouseUp(ITUWidget* widget, char* param);
extern bool SettingEngRadioBoxOnMouseUp(ITUWidget* widget, char* param);

extern bool TopOnTimer(ITUWidget* widget, char* param);
ITUActionFunction actionFunctions[] =
{
    "MainOnEnter", MainOnEnter,
    "MainOnTimer", MainOnTimer,
    "mainStallsAnimationOnStop", mainStallsAnimationOnStop,
    "MainMusicAnimationOnStop", MainMusicAnimationOnStop,
    "MainBeamAnimationOnStop", MainBeamAnimationOnStop,
    "MainReTestButtonOnPress", MainReTestButtonOnPress,
    "MainNextBGButtonOnPress", MainNextBGButtonOnPress,
	"MainOnLeave", MainOnLeave,

	"DigitalTypeOnEnter", DigitalTypeOnEnter,
	"DigitalTypeOnTimer", DigitalTypeOnTimer,
	"DigitalTypeOnLeave", DigitalTypeOnLeave,
	"DigitalTypeChangeUIBtOnMouseUp", DigitalTypeChangeUIBtOnMouseUp,
	"DigitalTypeTempAnOnStopped", DigitalTypeTempAnOnStopped,
	"DigitalTypInfoUpSpriteOnStopped", DigitalTypInfoUpSpriteOnStopped,
	"DigitalTypInfoDownSpriteOnStopped", DigitalTypInfoDownSpriteOnStopped,
	"DigitalTypeInfoAnOnStopped", DigitalTypeInfoAnOnStopped,
	"DigitalTypePlayBtOnMouseUp", DigitalTypePlayBtOnMouseUp,
	//"DigitalTypeBackButtonOnMouseUp", DigitalTypeBackButtonOnMouseUp,
	"digitalSpeedButtonOnPress", digitalSpeedButtonOnPress,
	"digitalDelayButtonOnPress", digitalDelayButtonOnPress,
	//"DigitalTypeWiFiButtonOnPress", DigitalTypeWiFiButtonOnPress,


	"ProjectOnEnter", ProjectOnEnter,
	"ProjectOnTimer", ProjectOnTimer,
	"ProjectOnLeave", ProjectOnLeave,
	"ProjectChangeUIBtOnMouseUp", ProjectChangeUIBtOnMouseUp,
	"ProjectTempAnOnStopped", ProjectTempAnOnStopped,
	"ProjectPlayBtOnMouseUp", ProjectPlayBtOnMouseUp,
	"ProjectWiFiButtonOnPress", ProjectWiFiButtonOnPress,
	"ProjectShowReverseBtOnMouseUp", ProjectShowReverseBtOnMouseUp,
	"projectGoHomeBtOnMouseUp", projectGoHomeBtOnMouseUp,

	"DashboardCamOnEnter", DashboardCamOnEnter,
	"DashboardCamOnTimer", DashboardCamOnTimer,
	"DashboardCamOnLeave", DashboardCamOnLeave,
	"DashboardCamPlayBtOnMouseUp", DashboardCamPlayBtOnMouseUp,
	"DashboardCamPlaybackViewBackBtnOnMouseUp", DashboardCamPlaybackViewBackBtnOnMouseUp,
	"DashbaordCamPlaybackVideoBtnOnMouseUp", DashbaordCamPlaybackVideoBtnOnMouseUp,
	"DashboardCamPlaybackNextBtnOnMouseUp", DashboardCamPlaybackNextBtnOnMouseUp,
	"DashboardCamPlaybackPrevBtnOnMouseUp", DashboardCamPlaybackPrevBtnOnMouseUp,
	"DashboardCamLoopRadioBoxOnMouseUp", DashboardCamLoopRadioBoxOnMouseUp,
	"DashboardCamPlaybackMenuBtnOnMouseUp", DashboardCamPlaybackMenuBtnOnMouseUp,
	"DashboardCamRecordStopBtnOnMouseUp", DashboardCamRecordStopBtnOnMouseUp,
	"DashboardCamMenuBtnOnMouseUp", DashboardCamMenuBtnOnMouseUp,
	"DashboardCamPlaybackMenuBackBtnOnMouseUp", DashboardCamPlaybackMenuBackBtnOnMouseUp,
	

	"OpeningOnEnter", OpeningOnEnter,
	"OpeningOnTimer", OpeningOnTimer,
	"OpenOnLeave", OpenOnLeave,

	"BackupCamOnEnter", BackupCamOnEnter,
	"BackupCamOnLeave", BackupCamOnLeave,
	"BackupCamOnTimer", BackupCamOnTimer,
	"BackupCamReverseBtOnMouseUp", BackupCamReverseBtOnMouseUp,
	"BackupCamLeftBtOnMouseUp", BackupCamLeftBtOnMouseUp,
	"BackupCamRearBtOnMouseUp", BackupCamRearBtOnMouseUp,
	"BackupCamPlaybackViewBackBtnOnMouseUp", BackupCamPlaybackViewBackBtnOnMouseUp,
	"BackupCamPlaybackVideoBtnOnMouseUp", BackupCamPlaybackVideoBtnOnMouseUp,
	"BackupCamPlaybackNextBtnOnMouseUp", BackupCamPlaybackNextBtnOnMouseUp,
	"BackupCamPlaybackPrevBtnOnMouseUp", BackupCamPlaybackPrevBtnOnMouseUp,
	"BackupCamLoopRadioBoxOnMouseUp", BackupCamLoopRadioBoxOnMouseUp,
	"BackupCamPlaybackMenuBtnOnMouseUp", BackupCamPlaybackMenuBtnOnMouseUp,
	"BackupCamRecordStopBtnOnMouseUp", BackupCamRecordStopBtnOnMouseUp,
	"BackupCamMenuBtnOnMouseUp", BackupCamMenuBtnOnMouseUp,
	"BackupCamPlaybackMenuBackBtnOnMouseUp", BackupCamPlaybackMenuBackBtnOnMouseUp,

	"SurveillanceOnEnter", SurveillanceOnEnter,
	"SurveillanceOnLeave", SurveillanceOnLeave,
	"SurveillanceOnTimer", SurveillanceOnTimer,
	"SurveillanceReverseBtOnMouseUp", SurveillanceReverseBtOnMouseUp,
	"SurveillanceQuadBtOnMouseUp", SurveillanceQuadBtOnMouseUp,
	"SurveillanceRearBtOnMouseUp", SurveillanceRearBtOnMouseUp,

	"LightOnEnter", LightOnEnter,
	"LightBackBtnOnMouseUp", LightBackBtnOnMouseUp,
	"LightFrontBtnOnMouseUp", LightFrontBtnOnMouseUp,

	"WindowOnEnter", WindowOnEnter,
	"WindowLeftBackBtnOnMouseUp", WindowLeftBackBtnOnMouseUp,
	"WindowLeftFrontBtnOnMouseUp", WindowLeftFrontBtnOnMouseUp,
	"WindowRightBackBtnOnMouseUp", WindowRightBackBtnOnMouseUp,
	"WindowRightFrontBtnOnMouseUp", WindowRightFrontBtnOnMouseUp,

	"SeatOnEnter", SeatOnEnter,
	"SeatBackDeBtnOnMouseUp", SeatBackDeBtnOnMouseUp,
	"SeatBackAddBtnOnMouseUp", SeatBackAddBtnOnMouseUp,
	"SeatLegDeBtnOnMouseUp", SeatLegDeBtnOnMouseUp,
	"SeatLegAddBtnOnMouseUp", SeatLegAddBtnOnMouseUp,

	"SettingOnEnter", SettingOnEnter,
	"SettingOnLeave", SettingOnLeave,
	"SettingProtectTimeTrackBarOnChanged", SettingProtectTimeTrackBarOnChanged,
	"SettingProtectTimeUpOnMouseUp", SettingProtectTimeUpOnMouseUp,
	"SettingProtectTimeDownOnMouseUp", SettingProtectTimeDownOnMouseUp,
	"SettingVoiceTrackBarOnChanged", SettingVoiceTrackBarOnChanged,
	"SettingVoiceUpOnMouseUp", SettingVoiceUpOnMouseUp,
	"SettingVoiceDownOnMouseUp", SettingVoiceDownOnMouseUp,
	"SettingLightTrackBarOnChanged", SettingLightTrackBarOnChanged,
	"SettingLightUpOnMouseUp", SettingLightUpOnMouseUp,
	"SettingLightDownOnMouseUp", SettingLightDownOnMouseUp,
	"SettingChtRadioBoxOnMouseUp", SettingChtRadioBoxOnMouseUp,
	"SettingEngRadioBoxOnMouseUp", SettingEngRadioBoxOnMouseUp,

	"TopOnTimer", TopOnTimer,


    NULL, NULL
};
