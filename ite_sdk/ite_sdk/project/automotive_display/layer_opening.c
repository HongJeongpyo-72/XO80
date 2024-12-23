#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"

ITUAnimation* menuBackupCamAnimation;

static ITUBackground* openingBg;
static ITUBackground* openingBackground[6];
static ITUCircleProgressBar* openCircleProgressBar;
static ITUMeter* openMeter;
static ITULayer* menuLayer;

static bool open_progress_done = false;
/* widgets:
openingLayer
digitalTypeOpeningBackground
digitalTypeOpeningBackground0
Icon16
digitalTypeOpeningBackground1
digitalTypeOpenMeter
Icon2
digitalTypeOpenCircleProgressBar
Icon47
digitalTypeOpeningBackground2
Icon58
Icon57
digitalTypeOpeningBackground3
Icon118
digitalTypeOpeningBackground4
Icon106
Icon70
digitalTypeOpeningBackground5
Icon112
Icon113
*/

void ShowOpening(int index)
{
	printf("[digital] ShowOpening %d\n", index);

	if (index == 2)
	{
		ituWidgetShow(openingBackground[index], ITU_EFFECT_FADE, 30);
		ituWidgetShow(openingBackground[index + 1], ITU_EFFECT_SCROLL_UP_FADE, 30);
	}
	else if (index == 1)
	{
		ituWidgetSetVisible(openingBackground[index], true);
	}
	else if (index == 6)
	{
		ituLayerGoto(menuLayer);
		ScreenSaverRefresh();
	}
	else
		ituWidgetShow(openingBackground[index], ITU_EFFECT_FADE, 30);
}

bool OpeningOnEnter(ITUWidget* widget, char* param)
{
	int i = 0;
	char tmp[64];

	if (!openingBg)
	{
		openingBg = ituSceneFindWidget(&theScene, "openingBackground");
		assert(openingBg);

		openCircleProgressBar = ituSceneFindWidget(&theScene, "openCircleProgressBar");
		assert(openCircleProgressBar);

		openMeter = ituSceneFindWidget(&theScene, "openMeter");
		assert(openMeter);

		menuLayer = ituSceneFindWidget(&theScene, "menuLayer");
		assert(menuLayer);

		menuBackupCamAnimation = ituSceneFindWidget(&theScene, "menuBackupCamAnimation");
		assert(menuBackupCamAnimation);

		for (i = 0; i < 6; i++)
		{
			sprintf(tmp, "openingBackground%d", i);
			openingBackground[i] = ituSceneFindWidget(&theScene, tmp);
			assert(openingBackground[i]);
		}
	}

	open_progress_done = false;

	ituCircleProgressBarSetValue(openCircleProgressBar, 100);
	ituMeterSetValue(openMeter, 100);

	ituWidgetShow(openingBackground[0], ITU_EFFECT_FADE, 30);
	ituSceneExecuteCommand(&theScene, 20, ShowOpening, 1);
	printf("[digital] ituSceneExecuteCommand(&theScene, 20, ShowOpening, 1);\n");
    return true;
}

bool OpeningOnTimer(ITUWidget* widget, char* param)
{
	bool ret = false;
	static int open_progress_value = 100;
	int i = 0;

	if (ituWidgetIsVisible(openingBackground[1]) && open_progress_done == false)
	{
		open_progress_value--;


		if (open_progress_value < 0)
		{
			for (i = 2; i < 7; i++)
			{
				if (i != 3 && i != 6)
					ituSceneExecuteCommand(&theScene, (i - 1) * 20, ShowOpening, i);//i==6, ituWidgetSetVisible(digitalTypeOpeningBg, false);
				else if (i == 6)
					ituSceneExecuteCommand(&theScene, 150, ShowOpening, i);
				else
					continue;
			}
			open_progress_value = 100;
			open_progress_done = true;
		}
		else
		{
			ituCircleProgressBarSetValue(openCircleProgressBar, open_progress_value);
			ituMeterSetValue(openMeter, open_progress_value);
		}

		ret = true;
	}
    return ret;
}

bool OpenOnLeave(ITUWidget* widget, char* param)
{
	int i = 0;
	for (i = 0; i < 6; i++)
		ituWidgetSetVisible(openingBackground[i], false);
	ituWidgetSetVisible(openingBg, true);
	return true;
}

