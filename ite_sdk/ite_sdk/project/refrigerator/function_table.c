#include "ite/itu.h"

extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool MainBotOnButtonOnPress(ITUWidget* widget, char* param);
extern bool MainBotSpriteOnStop(ITUWidget* widget, char* param);
extern bool MainMidOnButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMidSpriteOnStop(ITUWidget* widget, char* param);
extern bool MainTopOnButtonOnPress(ITUWidget* widget, char* param);
extern bool MainTopSpriteOnStop(ITUWidget* widget, char* param);
extern bool MainMidIceOnButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMidIceOffButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingOnEnter(ITUWidget* widget, char* param);
extern bool SettingOnLeave(ITUWidget* widget, char* param);
extern bool SettingAlarmTrackBarOnChange(ITUWidget* widget, char* param);
extern bool SettingVolumeTrackBarOnChange(ITUWidget* widget, char* param);
extern bool SettingLightTrackBarOnChange(ITUWidget* widget, char* param);
extern bool SettingLCDTrackBarOnChange(ITUWidget* widget, char* param);
extern bool TopOnEnter(ITUWidget* widget, char* param);
extern bool TopDownloadButtonOnPress(ITUWidget* widget, char* param);
extern bool TopUpdateButtonOnPress(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "MainOnEnter", MainOnEnter,
    "MainOnTimer", MainOnTimer,
    "MainBotOnButtonOnPress", MainBotOnButtonOnPress,
    "MainBotSpriteOnStop", MainBotSpriteOnStop,
    "MainMidOnButtonOnPress", MainMidOnButtonOnPress,
    "MainMidSpriteOnStop", MainMidSpriteOnStop,
    "MainTopOnButtonOnPress", MainTopOnButtonOnPress,
    "MainTopSpriteOnStop", MainTopSpriteOnStop,
    "MainMidIceOnButtonOnPress", MainMidIceOnButtonOnPress,
    "MainMidIceOffButtonOnPress", MainMidIceOffButtonOnPress,
    "SettingOnEnter", SettingOnEnter,
    "SettingOnLeave", SettingOnLeave,
    "SettingAlarmTrackBarOnChange", SettingAlarmTrackBarOnChange,
    "SettingVolumeTrackBarOnChange", SettingVolumeTrackBarOnChange,
    "SettingLightTrackBarOnChange", SettingLightTrackBarOnChange,
    "SettingLCDTrackBarOnChange", SettingLCDTrackBarOnChange,
    "TopOnEnter", TopOnEnter,
    "TopDownloadButtonOnPress", TopDownloadButtonOnPress,
    "TopUpdateButtonOnPress", TopUpdateButtonOnPress,
    NULL, NULL
};
