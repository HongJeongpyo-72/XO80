#include "ite/itu.h"

extern bool StandbyOnEnter(ITUWidget* widget, char* param);
extern bool StandbyStartPopupButtonOnPress(ITUWidget* widget, char* param);
extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnLeave(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool MainStopPopupButtonOnPress(ITUWidget* widget, char* param);
extern bool MainPlayPopupButtonOnPress(ITUWidget* widget, char* param);
extern bool MainPausePopupButtonOnPress(ITUWidget* widget, char* param);
extern bool MainFanBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool MainSpeedWheelOnChanged(ITUWidget* widget, char* param);
extern bool MainSlopeWheelOnChanged(ITUWidget* widget, char* param);
extern bool MainProgressBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool MainStatusBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool MainTypeBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool updateOnEnter(ITUWidget* widget, char* param);
extern bool updateOnLeave(ITUWidget* widget, char* param);
extern bool UpdateDownloadButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingOnEnter(ITUWidget* widget, char* param);
extern bool SettingOnLeave(ITUWidget* widget, char* param);
extern bool SettingUnitWeightRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingUnitDistanceRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingLanguageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDateWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingBrightnessTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingVolumeTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingPulseWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingWeightWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingAgeWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingDateTimeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingExitButtonOnPress(ITUWidget* widget, char* param);
extern bool ProgramOnEnter(ITUWidget* widget, char* param);
extern bool ProgramOnLeave(ITUWidget* widget, char* param);
extern bool ProgramDialogExitButtonOnPress(ITUWidget* widget, char* param);
extern bool ProgramTypeRadioBoxOnPress(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "StandbyOnEnter", StandbyOnEnter,
    "StandbyStartPopupButtonOnPress", StandbyStartPopupButtonOnPress,
    "MainOnEnter", MainOnEnter,
    "MainOnLeave", MainOnLeave,
    "MainOnTimer", MainOnTimer,
    "MainStopPopupButtonOnPress", MainStopPopupButtonOnPress,
    "MainPlayPopupButtonOnPress", MainPlayPopupButtonOnPress,
    "MainPausePopupButtonOnPress", MainPausePopupButtonOnPress,
    "MainFanBackgroundButtonOnPress", MainFanBackgroundButtonOnPress,
    "MainSpeedWheelOnChanged", MainSpeedWheelOnChanged,
    "MainSlopeWheelOnChanged", MainSlopeWheelOnChanged,
    "MainProgressBackgroundButtonOnPress", MainProgressBackgroundButtonOnPress,
    "MainStatusBackgroundButtonOnPress", MainStatusBackgroundButtonOnPress,
    "MainTypeBackgroundButtonOnPress", MainTypeBackgroundButtonOnPress,
    "updateOnEnter", updateOnEnter,
    "updateOnLeave", updateOnLeave,
    "UpdateDownloadButtonOnPress", UpdateDownloadButtonOnPress,
    "SettingOnEnter", SettingOnEnter,
    "SettingOnLeave", SettingOnLeave,
    "SettingUnitWeightRadioBoxOnPress", SettingUnitWeightRadioBoxOnPress,
    "SettingUnitDistanceRadioBoxOnPress", SettingUnitDistanceRadioBoxOnPress,
    "SettingLanguageRadioBoxOnPress", SettingLanguageRadioBoxOnPress,
    "SettingDateWheelOnChanged", SettingDateWheelOnChanged,
    "SettingBrightnessTrackBarOnChanged", SettingBrightnessTrackBarOnChanged,
    "SettingVolumeTrackBarOnChanged", SettingVolumeTrackBarOnChanged,
    "SettingPulseWheelOnChanged", SettingPulseWheelOnChanged,
    "SettingWeightWheelOnChanged", SettingWeightWheelOnChanged,
    "SettingAgeWheelOnChanged", SettingAgeWheelOnChanged,
    "SettingDateTimeRadioBoxOnPress", SettingDateTimeRadioBoxOnPress,
    "SettingExitButtonOnPress", SettingExitButtonOnPress,
    "ProgramOnEnter", ProgramOnEnter,
    "ProgramOnLeave", ProgramOnLeave,
    "ProgramDialogExitButtonOnPress", ProgramDialogExitButtonOnPress,
    "ProgramTypeRadioBoxOnPress", ProgramTypeRadioBoxOnPress,
    NULL, NULL
};
