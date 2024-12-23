#include <assert.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "scene.h"
#include "sys/ioctl.h"

/* widgets:
settingLayer
settingBackBtAnimation
Button24
Button26
Background439
Animation524
Background485
Container472
Text484
Icon473
Icon474
Text475
settingScreenProtectTimeTrackBar
Button477
settingScreenProtectTimeProgressBar
Icon479
Button480
Button481
Text482
Icon483
Container460
Icon461
Icon462
Text463
settingVoiceTrackBar
Button465
settingVoiceProgressBar
Icon467
Button468
Button469
Text470
Icon471
Container459
Icon455
Icon454
Text457
settingLightTrackBar
Button483
settinLightProgressBar
Icon110
settingLightUpButton
settingLightDownButton
Text453
Icon452
Background486
Container487
Text488
Icon489
Icon490
Text491
TrackBar492
Button493
ProgressBar494
Icon495
Button496
Button497
Text498
Icon499
Container500
Icon501
Icon502
Text503
TrackBar504
Button505
ProgressBar506
Icon507
Button508
Button509
Text510
Icon511
Container512
Icon513
Icon514
Text515
TrackBar516
Button517
ProgressBar518
Icon519
Button520
Button521
Text522
Icon523
Animation451
Background445
Text444
Text443
RadioBox442
RadioBox440
Background446
Text447
Text448
RadioBox449
RadioBox450
*/
static ITUProgressBar* settinLightProgressBar;
static ITUTrackBar* settingLightTrackBar;

static ITUProgressBar* settingVoiceProgressBar;
static ITUTrackBar* settingVoiceTrackBar;

static ITUProgressBar* settingScreenProtectTimeProgressBar;
static ITUTrackBar* settingScreenProtectTimeTrackBar;

static ITURadioBox* settingLangChtRadioBox;
static ITURadioBox* settingLangEngRadioBox;

static int settingBrightnessOld;
static int settingKeylevelOld;
static int settingScreensaverTimeOld;
static void SettingSystemChangeLangCommand(ITUWidget* widget, char* param)
{
	ITULayer* layer = ituGetLayer(widget);
	assert(layer);

	if (layer)
	{
		ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.lang, 0, 0);
		ituWidgetUpdate(layer, ITU_EVENT_LAYOUT, 0, 0, 0);
	}

	ConfigSave();
}

bool SettingOnEnter(ITUWidget* widget, char* param)
{
	if (!settinLightProgressBar)
	{
		settinLightProgressBar = ituSceneFindWidget(&theScene, "settinLightProgressBar");
		assert(settinLightProgressBar);

		settingLightTrackBar = ituSceneFindWidget(&theScene, "settingLightTrackBar");
		assert(settingLightTrackBar);

		settingVoiceProgressBar = ituSceneFindWidget(&theScene, "settingVoiceProgressBar");
		assert(settingVoiceProgressBar);

		settingVoiceTrackBar = ituSceneFindWidget(&theScene, "settingVoiceTrackBar");
		assert(settingVoiceTrackBar);

		settingScreenProtectTimeProgressBar = ituSceneFindWidget(&theScene, "settingScreenProtectTimeProgressBar");
		assert(settingScreenProtectTimeProgressBar);

		settingScreenProtectTimeTrackBar = ituSceneFindWidget(&theScene, "settingScreenProtectTimeTrackBar");
		assert(settingScreenProtectTimeTrackBar);

		settingLangChtRadioBox = ituSceneFindWidget(&theScene, "settingLangChtRadioBox");
		assert(settingLangChtRadioBox);

		settingLangEngRadioBox = ituSceneFindWidget(&theScene, "settingLangEngRadioBox");
		assert(settingLangEngRadioBox);
	}

	ituProgressBarSetValue(settinLightProgressBar, theConfig.brightness);
	ituTrackBarSetValue(settingLightTrackBar, theConfig.brightness);

	ituProgressBarSetValue(settingVoiceProgressBar, theConfig.keylevel);
	ituTrackBarSetValue(settingVoiceTrackBar, theConfig.keylevel);

	ituProgressBarSetValue(settingScreenProtectTimeProgressBar, theConfig.screensaver_time);
	ituTrackBarSetValue(settingScreenProtectTimeTrackBar, theConfig.screensaver_time);

	settingBrightnessOld = theConfig.brightness;
	settingKeylevelOld = theConfig.keylevel;
	settingScreensaverTimeOld = theConfig.screensaver_time;

	// current settings
	if (theConfig.lang == LANG_CHT)
		ituRadioBoxSetChecked(settingLangChtRadioBox, true);
	else
		ituRadioBoxSetChecked(settingLangEngRadioBox, true);

    return true;
}

bool SettingOnLeave(ITUWidget* widget, char* param)
{
	if (settingBrightnessOld != theConfig.brightness ||
		settingKeylevelOld != theConfig.keylevel ||
		settingScreensaverTimeOld != theConfig.screensaver_time)
		ConfigSave();
	return true;
}

bool SettingProtectTimeTrackBarOnChanged(ITUWidget* widget, char* param)
{
	theConfig.screensaver_time = settingScreenProtectTimeTrackBar->value;
    return true;
}

bool SettingProtectTimeUpOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingScreenProtectTimeTrackBar->value;
	value = value + 5;
	if (value <= 30)
	{
		theConfig.screensaver_time = value;
		ituProgressBarSetValue(settingScreenProtectTimeProgressBar, theConfig.screensaver_time);
		ituTrackBarSetValue(settingScreenProtectTimeTrackBar, theConfig.screensaver_time);
	}
    return true;
}

bool SettingProtectTimeDownOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingScreenProtectTimeTrackBar->value;
	value = value - 5;
	if (value >= 5)
	{
		theConfig.screensaver_time = value;
		ituProgressBarSetValue(settingScreenProtectTimeProgressBar, theConfig.screensaver_time);
		ituTrackBarSetValue(settingScreenProtectTimeTrackBar, theConfig.screensaver_time);
	}
    return true;
}

bool SettingVoiceTrackBarOnChanged(ITUWidget* widget, char* param)
{
	theConfig.keylevel = settingVoiceTrackBar->value;
	AudioSetKeyLevel(theConfig.keylevel);
    return true;
}

bool SettingVoiceUpOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingVoiceTrackBar->value;
	value++;
	if (value <= 20)
	{
		theConfig.keylevel = value;
		ituProgressBarSetValue(settingVoiceProgressBar, theConfig.keylevel);
		ituTrackBarSetValue(settingVoiceTrackBar, theConfig.keylevel);
		AudioSetKeyLevel(theConfig.keylevel);
	}
    return true;
}

bool SettingVoiceDownOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingVoiceTrackBar->value;
	value--;
	if (value >= 0)
	{
		theConfig.keylevel = value;
		ituProgressBarSetValue(settingVoiceProgressBar, theConfig.keylevel);
		ituTrackBarSetValue(settingVoiceTrackBar, theConfig.keylevel);
		AudioSetKeyLevel(theConfig.keylevel);
	}
    return true;
}

bool SettingLightTrackBarOnChanged(ITUWidget* widget, char* param)
{
	theConfig.brightness = settingLightTrackBar->value;
	ScreenSetBrightness(theConfig.brightness - 1);
    return true;
}

bool SettingLightUpOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingLightTrackBar->value;
	value++;
	if (value <= 10)
	{
		theConfig.brightness = value;
		ituProgressBarSetValue(settinLightProgressBar, theConfig.brightness);
		ituTrackBarSetValue(settingLightTrackBar, theConfig.brightness);
		ScreenSetBrightness(theConfig.brightness - 1);
	}
    return true;
}

bool SettingLightDownOnMouseUp(ITUWidget* widget, char* param)
{
	int value = settingLightTrackBar->value;
	value--;
	if (value >= 1)
	{
		theConfig.brightness = value;
		ituProgressBarSetValue(settinLightProgressBar, theConfig.brightness);
		ituTrackBarSetValue(settingLightTrackBar, theConfig.brightness);
		ScreenSetBrightness(theConfig.brightness - 1);
	}
    return true;
}
bool SettingChtRadioBoxOnMouseUp(ITUWidget* widget, char* param)
{
	if (theConfig.lang == LANG_CHT)
		return false;

	theConfig.lang = LANG_CHT;
	ituSceneExecuteCommand(&theScene, 1, SettingSystemChangeLangCommand, widget);

	return true;
}
bool SettingEngRadioBoxOnMouseUp(ITUWidget* widget, char* param)
{
	if (theConfig.lang == LANG_ENG)
		return false;

	theConfig.lang = LANG_ENG;
	ituSceneExecuteCommand(&theScene, 1, SettingSystemChangeLangCommand, widget);


	return true;
}
