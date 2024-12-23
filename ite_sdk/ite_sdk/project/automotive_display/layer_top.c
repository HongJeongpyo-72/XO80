#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"

/* widgets:
topLayer
Background10
topMonSprite
Text48
Text50
Text63
Text62
Text61
Text60
Text59
Text56
Text55
Text54
Text51
Text40
DigitalClock11
topMonText
topMinText
topColonText
topHourText
topDayText
Text37
topWeekSprite
Text35
Text28
Text27
Text26
Text25
Text23
Text22
*/

static ITUContainer* topEngContainer;
static ITUSprite* topMonSprite;
static ITUText* topMonText;
static int month, preMonth;

static ITUContainer* topChtContainer;

static int toplang = 0;
static bool toplangChange = false;


bool TopOnTimer(ITUWidget* widget, char* param)
{
	bool ret = false;

	if (!topMonSprite)
	{
		topMonSprite = ituSceneFindWidget(&theScene, "topMonSprite");
		assert(topMonSprite);

		topMonText = ituSceneFindWidget(&theScene, "topMonText");
		assert(topMonText);

		topEngContainer = ituSceneFindWidget(&theScene, "topEngContainer");
		assert(topEngContainer);

		topChtContainer = ituSceneFindWidget(&theScene, "topChtContainer");
		assert(topChtContainer);

		
		ituSpriteGoto(topMonSprite, 0);
		preMonth = 0;

		if (toplang == 0)
		{
			ituWidgetSetVisible(topEngContainer, true);
			ituWidgetSetVisible(topChtContainer, false);
		}
		else
		{
			ituWidgetSetVisible(topEngContainer, false);
			ituWidgetSetVisible(topChtContainer, true);
		}
	}

	month = atoi(ituTextGetString(topMonText));

	if (preMonth != month)
	{
		ituSpriteGoto(topMonSprite, month - 1);
		preMonth = month;

		ret = true;
	}

	if (toplang != theConfig.lang)
	{
		toplang = theConfig.lang;
		if (toplang == 0)
		{
			ituWidgetSetVisible(topEngContainer, true);
			ituWidgetSetVisible(topChtContainer, false);
		}
		else
		{
			ituWidgetSetVisible(topEngContainer, false);
			ituWidgetSetVisible(topChtContainer, true);
		}

	}
    return ret;
}

