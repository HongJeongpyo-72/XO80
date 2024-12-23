#include "ite/itu.h"

extern bool LogoOnEnter(ITUWidget* widget, char* param);
extern bool SettingPowerPriceButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingPowerCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingTimeAnimationOnStop(ITUWidget* widget, char* param);
extern bool SettingTimeWeekCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingTimePowerOffCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingTimePowerOnCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingTimeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncAnimationOnStop(ITUWidget* widget, char* param);
extern bool SettingFuncSystemButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncPowerOffCleanCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncSavingCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncHeatLowCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncHorizontalCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncVerticalCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncHeatHighCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncCleanCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncCoolingCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingFuncCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemTimeButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemDateButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemLightCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemBrightnessTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingSystemSoundCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnLeave(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool MainLockBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool MainAnimationOnStop(ITUWidget* widget, char* param);
extern bool MainBackgroundButtonOnLongPress(ITUWidget* widget, char* param);
extern bool MainWindTrackBarOnChange(ITUWidget* widget, char* param);
extern bool MainWindTrackButtonOnMouseUp(ITUWidget* widget, char* param);
extern bool MainPowerTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool MainModePopupRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool MainTemperatureWheelOnChanged(ITUWidget* widget, char* param);
extern bool MainPureOnButtonOnPress(ITUWidget* widget, char* param);
extern bool MainPureOffButtonOnPress(ITUWidget* widget, char* param);
extern bool MainAutoWindButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingOnEnter(ITUWidget* widget, char* param);
extern bool SettingSystemTimeConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemDateConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemDateWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingSystemScreenSaverRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingTimePowerOffConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTimePowerOnConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingPowerResetConfirmButton(ITUWidget* widget, char* param);
extern bool SettingPowerPriceConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerOnEnter(ITUWidget* widget, char* param);
extern bool VideoPlayerOnLeave(ITUWidget* widget, char* param);
extern bool VideoPlayerOnTimer(ITUWidget* widget, char* param);
extern bool VideoPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool VideoPlayerBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool EngineerOnEnter(ITUWidget* widget, char* param);
extern bool EngineerOnTimer(ITUWidget* widget, char* param);
extern bool EngineerOnLeave(ITUWidget* widget, char* param);
extern bool EngineerWarnTableListBoxOnLoad(ITUWidget* widget, char* param);
extern bool EngineerPowerOnEnter(ITUWidget* widget, char* param);
extern bool EngineerPowerOnTimer(ITUWidget* widget, char* param);
extern bool EngineerPowerOnLeave(ITUWidget* widget, char* param);
extern bool EngineerPowerMemoryOffButtonOnPress(ITUWidget* widget, char* param);
extern bool EngineerPowerMemoryOnButtonOnPress(ITUWidget* widget, char* param);
extern bool EngineerMachineOnEnter(ITUWidget* widget, char* param);
extern bool EngineerMachineOnTimer(ITUWidget* widget, char* param);
extern bool EngineerMachineOnLeave(ITUWidget* widget, char* param);
extern bool EngineerTryOnEnter(ITUWidget* widget, char* param);
extern bool EngineerTryOnTimer(ITUWidget* widget, char* param);
extern bool EngineerTryOnLeave(ITUWidget* widget, char* param);
extern bool EngineerTryWarnTableListBoxOnLoad(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "LogoOnEnter", LogoOnEnter,
    "SettingPowerPriceButtonOnPress", SettingPowerPriceButtonOnPress,
    "SettingPowerCheckBoxOnPress", SettingPowerCheckBoxOnPress,
    "SettingTimeAnimationOnStop", SettingTimeAnimationOnStop,
    "SettingTimeWeekCheckBoxOnPress", SettingTimeWeekCheckBoxOnPress,
    "SettingTimePowerOffCheckBoxOnPress", SettingTimePowerOffCheckBoxOnPress,
    "SettingTimePowerOnCheckBoxOnPress", SettingTimePowerOnCheckBoxOnPress,
    "SettingTimeCheckBoxOnPress", SettingTimeCheckBoxOnPress,
    "SettingFuncAnimationOnStop", SettingFuncAnimationOnStop,
    "SettingFuncSystemButtonOnPress", SettingFuncSystemButtonOnPress,
    "SettingFuncPowerOffCleanCheckBoxOnPress", SettingFuncPowerOffCleanCheckBoxOnPress,
    "SettingFuncSavingCheckBoxOnPress", SettingFuncSavingCheckBoxOnPress,
    "SettingFuncHeatLowCheckBoxOnPress", SettingFuncHeatLowCheckBoxOnPress,
    "SettingFuncHorizontalCheckBoxOnPress", SettingFuncHorizontalCheckBoxOnPress,
    "SettingFuncVerticalCheckBoxOnPress", SettingFuncVerticalCheckBoxOnPress,
    "SettingFuncHeatHighCheckBoxOnPress", SettingFuncHeatHighCheckBoxOnPress,
    "SettingFuncCleanCheckBoxOnPress", SettingFuncCleanCheckBoxOnPress,
    "SettingFuncCoolingCheckBoxOnPress", SettingFuncCoolingCheckBoxOnPress,
    "SettingFuncCheckBoxOnPress", SettingFuncCheckBoxOnPress,
    "SettingSystemTimeButtonOnPress", SettingSystemTimeButtonOnPress,
    "SettingSystemDateButtonOnPress", SettingSystemDateButtonOnPress,
    "SettingSystemLightCheckBoxOnPress", SettingSystemLightCheckBoxOnPress,
    "SettingSystemBrightnessTrackBarOnChanged", SettingSystemBrightnessTrackBarOnChanged,
    "SettingSystemSoundCheckBoxOnPress", SettingSystemSoundCheckBoxOnPress,
    "MainOnEnter", MainOnEnter,
    "MainOnLeave", MainOnLeave,
    "MainOnTimer", MainOnTimer,
    "MainLockBackgroundButtonOnPress", MainLockBackgroundButtonOnPress,
    "MainAnimationOnStop", MainAnimationOnStop,
    "MainBackgroundButtonOnLongPress", MainBackgroundButtonOnLongPress,
    "MainWindTrackBarOnChange", MainWindTrackBarOnChange,
    "MainWindTrackButtonOnMouseUp", MainWindTrackButtonOnMouseUp,
    "MainPowerTrackBarOnChanged", MainPowerTrackBarOnChanged,
    "MainModePopupRadioBoxOnPress", MainModePopupRadioBoxOnPress,
    "MainTemperatureWheelOnChanged", MainTemperatureWheelOnChanged,
    "MainPureOnButtonOnPress", MainPureOnButtonOnPress,
    "MainPureOffButtonOnPress", MainPureOffButtonOnPress,
    "MainAutoWindButtonOnPress", MainAutoWindButtonOnPress,
    "SettingOnEnter", SettingOnEnter,
    "SettingSystemTimeConfirmButtonOnPress", SettingSystemTimeConfirmButtonOnPress,
    "SettingSystemDateConfirmButtonOnPress", SettingSystemDateConfirmButtonOnPress,
    "SettingSystemDateWheelOnChanged", SettingSystemDateWheelOnChanged,
    "SettingSystemScreenSaverRadioBoxOnPress", SettingSystemScreenSaverRadioBoxOnPress,
    "SettingTimePowerOffConfirmButtonOnPress", SettingTimePowerOffConfirmButtonOnPress,
    "SettingTimePowerOnConfirmButtonOnPress", SettingTimePowerOnConfirmButtonOnPress,
    "SettingPowerResetConfirmButton", SettingPowerResetConfirmButton,
    "SettingPowerPriceConfirmButtonOnPress", SettingPowerPriceConfirmButtonOnPress,
    "VideoPlayerOnEnter", VideoPlayerOnEnter,
    "VideoPlayerOnLeave", VideoPlayerOnLeave,
    "VideoPlayerOnTimer", VideoPlayerOnTimer,
    "VideoPlayerPlayCheckBoxOnPress", VideoPlayerPlayCheckBoxOnPress,
    "VideoPlayerTrackBarOnChanged", VideoPlayerTrackBarOnChanged,
    "VideoPlayerBackgroundButtonOnPress", VideoPlayerBackgroundButtonOnPress,
    "EngineerOnEnter", EngineerOnEnter,
    "EngineerOnTimer", EngineerOnTimer,
    "EngineerOnLeave", EngineerOnLeave,
    "EngineerWarnTableListBoxOnLoad", EngineerWarnTableListBoxOnLoad,
    "EngineerPowerOnEnter", EngineerPowerOnEnter,
    "EngineerPowerOnTimer", EngineerPowerOnTimer,
    "EngineerPowerOnLeave", EngineerPowerOnLeave,
    "EngineerPowerMemoryOffButtonOnPress", EngineerPowerMemoryOffButtonOnPress,
    "EngineerPowerMemoryOnButtonOnPress", EngineerPowerMemoryOnButtonOnPress,
    "EngineerMachineOnEnter", EngineerMachineOnEnter,
    "EngineerMachineOnTimer", EngineerMachineOnTimer,
    "EngineerMachineOnLeave", EngineerMachineOnLeave,
    "EngineerTryOnEnter", EngineerTryOnEnter,
    "EngineerTryOnTimer", EngineerTryOnTimer,
    "EngineerTryOnLeave", EngineerTryOnLeave,
    "EngineerTryWarnTableListBoxOnLoad", EngineerTryWarnTableListBoxOnLoad,
    NULL, NULL
};
