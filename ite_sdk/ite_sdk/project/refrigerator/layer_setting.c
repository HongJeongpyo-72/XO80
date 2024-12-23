#include <assert.h>
#include "scene.h"
#include <sys/time.h>

extern TimeOut;

static int settingDisplayScreenMaxBrightness;
static ITUWheel *settingWheel[5] = { 0 };
static ITUTrackBar *settingLCDTrackBar = 0;
static ITUTrackBar *settingLightTrackBar = 0;
static ITUTrackBar *settingVolumeTrackBar = 0;
static ITUTrackBar *settingAlarmTimeTrackBar = 0;
static ITUProgressBar *settingLCDProgressBar = 0;
static ITUProgressBar *settingLightProgressBar = 0;
static ITUProgressBar *settingVolumeProgressBar = 0;
static ITUProgressBar *settingAlarmTimeProgressBar = 0;
static ITUSprite *settingLCDSprite = 0;
static ITUSprite *settingLightSprite = 0;
static ITUSprite *settingVolumeSprite = 0;
static ITUSprite *settingAlarmSprite = 0;
static ITUSprite *settingLCDTipSprite = 0;
static ITUSprite *settingLightTipSprite = 0;
static ITUSprite *settingVolumeTipSprite = 0;
static ITUSprite *settingAlarmTipSprite = 0;

bool SettingOnEnter(ITUWidget* widget, char* param)
{
	int tmp;
	struct tm *info;
	struct timeval time;
	if (!settingWheel[0])
	{
		settingWheel[0] = ituSceneFindWidget(&theScene, "settingYearWheel");
		assert(settingWheel[0]);

		settingWheel[1] = ituSceneFindWidget(&theScene, "settingMonthWheel");
		assert(settingWheel[1]);

		settingWheel[2] = ituSceneFindWidget(&theScene, "settingDateWheel");
		assert(settingWheel[2]);

		settingWheel[3] = ituSceneFindWidget(&theScene, "settingHourWheel");
		assert(settingWheel[3]);

		settingWheel[4] = ituSceneFindWidget(&theScene, "settingMinuteWheel");
		assert(settingWheel[4]);

		settingLCDTrackBar = ituSceneFindWidget(&theScene, "settingLCDTrackBar");
		assert(settingLCDTrackBar);

		settingLightTrackBar = ituSceneFindWidget(&theScene, "settingLightTrackBar");
		assert(settingLightTrackBar);

		settingVolumeTrackBar = ituSceneFindWidget(&theScene, "settingVolumeTrackBar");
		assert(settingVolumeTrackBar);

		settingAlarmTimeTrackBar = ituSceneFindWidget(&theScene, "settingAlarmTimeTrackBar");
		assert(settingAlarmTimeTrackBar);

		settingLCDProgressBar = ituSceneFindWidget(&theScene, "settingLCDProgressBar");
		assert(settingLCDProgressBar);

		settingLightProgressBar = ituSceneFindWidget(&theScene, "settingLightProgressBar");
		assert(settingLightProgressBar);

		settingVolumeProgressBar = ituSceneFindWidget(&theScene, "settingVolumeProgressBar");
		assert(settingVolumeProgressBar);

		settingAlarmTimeProgressBar = ituSceneFindWidget(&theScene, "settingAlarmTimeProgressBar");
		assert(settingAlarmTimeProgressBar);

		settingLCDSprite = ituSceneFindWidget(&theScene, "settingLCDSprite");
		assert(settingLCDSprite);

		settingLightSprite = ituSceneFindWidget(&theScene, "settingLightSprite");
		assert(settingLightSprite);

		settingVolumeSprite = ituSceneFindWidget(&theScene, "settingVolumeSprite");
		assert(settingVolumeSprite);

		settingAlarmSprite = ituSceneFindWidget(&theScene, "settingAlarmSprite");
		assert(settingAlarmSprite);

		settingLCDTipSprite = ituSceneFindWidget(&theScene, "settingLCDTipSprite");
		assert(settingLCDTipSprite);

		settingLightTipSprite = ituSceneFindWidget(&theScene, "settingLightTipSprite");
		assert(settingLightTipSprite);

		settingVolumeTipSprite = ituSceneFindWidget(&theScene, "settingVolumeTipSprite");
		assert(settingVolumeTipSprite);

		settingAlarmTipSprite = ituSceneFindWidget(&theScene, "settingAlarmTipSprite");
		assert(settingAlarmTipSprite);

		settingDisplayScreenMaxBrightness = ScreenGetMaxBrightness();
	}

	ituTrackBarSetValue(settingLCDTrackBar, theConfig.brightness + 1);
	ituProgressBarSetValue(settingLCDProgressBar, theConfig.brightness + 1);
	ituSpriteGoto(settingLCDSprite, theConfig.brightness + 1);
	ituSpriteGoto(settingLCDTipSprite, theConfig.brightness + 1);
	switch (theConfig.screensaver_time)
	{
	case 5:
		tmp = 0;
		break;
	case 60:
		tmp = 1;
		break;
	case 120:
		tmp = 2;
		break;
	case 300:
		tmp = 3;
		break;
	case 600:
		tmp = 4;
		break;
	case 900:
		tmp = 5;
		break;
	case 1200:
		tmp = 6;
		break;
	default:
		tmp = 0;
		break;
	}
	ituTrackBarSetValue(settingLightTrackBar, tmp);
	ituProgressBarSetValue(settingLightProgressBar, tmp);
	ituSpriteGoto(settingLightSprite, tmp);
	ituSpriteGoto(settingLightTipSprite, tmp);

	ituTrackBarSetValue(settingVolumeTrackBar, theConfig.audiolevel/10);
	ituProgressBarSetValue(settingVolumeProgressBar, theConfig.audiolevel / 10);
	ituSpriteGoto(settingVolumeSprite, theConfig.audiolevel / 10);
	ituSpriteGoto(settingVolumeTipSprite, theConfig.audiolevel / 10);


	switch (theConfig.alarm/1000)
	{
	case 5:
		tmp = 0;
		break;
	case 60:
		tmp = 1;
		break;
	case 120:
		tmp = 2;
		break;
	case 300:
		tmp = 3;
		break;
	case 600:
		tmp = 4;
		break;
	case 900:
		tmp = 5;
		break;
	case 1200:
		tmp = 6;
		break;
	default:
		tmp = 0;
		break;
	}
	ituTrackBarSetValue(settingAlarmTimeTrackBar, tmp);
	ituProgressBarSetValue(settingAlarmTimeProgressBar, tmp);
	ituSpriteGoto(settingAlarmSprite, tmp);
	ituSpriteGoto(settingAlarmTipSprite, tmp);

	gettimeofday(&time, NULL);
	info = localtime(&time.tv_sec);
	if (info->tm_year + 1900 > 2020 || info->tm_year + 1900 < 2011)
	{
		info->tm_year = 111;
		info->tm_mon = 0;
		info->tm_mday = 1;
		info->tm_hour = 0;
		info->tm_min = 0;
	}	
	ituWheelGoto(settingWheel[0], info->tm_year - 111);
	ituWheelGoto(settingWheel[1], info->tm_mon);
	ituWheelGoto(settingWheel[2], info->tm_mday-1);
	ituWheelGoto(settingWheel[3], info->tm_hour);
	ituWheelGoto(settingWheel[4], info->tm_min);

    return true;
}

bool SettingAlarmTrackBarOnChange(ITUWidget* widget, char* param)
{
	switch (atoi(param))
	{
	case 0:
		TimeOut = 5 * 1000;
		break;
	case 1:
		TimeOut = 60 * 1000;
		break;
	case 2:
		TimeOut = 2 * 60 * 1000;
		break;
	case 3:
		TimeOut = 5 * 60 * 1000;
		break;
	case 4:
		TimeOut = 10 * 60 * 1000;
		break;
	case 5:
		TimeOut = 15 * 60 * 1000;
		break;
	case 6:
		TimeOut = 20 * 60 * 1000;
		break;
	default:
		break;
	}
	theConfig.alarm = TimeOut;
	return true;
}
bool SettingVolumeTrackBarOnChange(ITUWidget* widget, char* param)
{
	int percent = atoi(param);

	theConfig.audiolevel = percent * 10;

	return true;
}
bool SettingLightTrackBarOnChange(ITUWidget* widget, char* param)
{
	switch (atoi(param))
	{
	case 0:
		theConfig.screensaver_time = 5;
		break;
	case 1:
		theConfig.screensaver_time = 60;
		break;
	case 2:
		theConfig.screensaver_time = 2 * 60;
		break;
	case 3:
		theConfig.screensaver_time = 5 * 60;
		break;
	case 4:
		theConfig.screensaver_time = 10 * 60;
		break;
	case 5:
		theConfig.screensaver_time = 15 * 60;;
		break;
	case 6:
		theConfig.screensaver_time = 20 * 60;
		break;
	default:
		break;
	}
	return true;
}
bool SettingLCDTrackBarOnChange(ITUWidget* widget, char* param)
{
	int value = atoi(param) - 1;

	if (value < 0)
		value = 0;
	else if (value > settingDisplayScreenMaxBrightness)
		value = settingDisplayScreenMaxBrightness;

	theConfig.brightness = value;
	ScreenSetBrightness(theConfig.brightness);
	return true;
}
bool SettingOnLeave(ITUWidget* widget, char* param)
{
	struct tm info;
	struct timeval tv_overwrite;

	info.tm_year = atoi(ituTextGetString(ituWheelGetFocusItem(settingWheel[0]))) - 1900;
	info.tm_mon = atoi(ituTextGetString(ituWheelGetFocusItem(settingWheel[1]))) - 1;
	info.tm_mday = atoi(ituTextGetString(ituWheelGetFocusItem(settingWheel[2])));
	info.tm_hour = atoi(ituTextGetString(ituWheelGetFocusItem(settingWheel[3])));
	info.tm_min = atoi(ituTextGetString(ituWheelGetFocusItem(settingWheel[4])));
	info.tm_sec = 0;
	info.tm_isdst = -1;
#ifndef WIN32
	tv_overwrite.tv_sec = mktime(&info);
	tv_overwrite.tv_usec = 0;

	settimeofday(&tv_overwrite, NULL);
#endif WIN32
	ConfigSave();

	return true;
}