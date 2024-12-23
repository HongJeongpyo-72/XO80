#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"

/* widgets:
seatLayer
seatBackBtAnimation
Button51
Button50
seatBackMenuBtAnimation
Button24
Button26
seatTextAnimation
Text21
Text20
Background296
seatLegSprite
Background260
Background253
Background261
Background254
Background255
Background256
Background257
Background258
Background259
Background252
seatBackSprite
Icon246
Icon245
Icon244
Icon243
Icon242
Icon241
Text166
Text165
Text164
Text163
Button162
Button161
Button160
Button159
Icon157
Icon156
Background5
*/

static ITUSprite* seatBackTextSprite;
static ITUSprite* seatLegTextSprite;
static ITUIcon* seatBackIcon[6];
static ITUIcon* seatLegIcon[10];

static int backIndex = 0;
static int legIndex = 0;
bool SeatOnEnter(ITUWidget* widget, char* param)
{
	int i;
	char tmp[32];
	if (!seatBackTextSprite)
	{
		seatBackTextSprite = ituSceneFindWidget(&theScene, "seatBackTextSprite");
		assert(seatBackTextSprite);

		seatLegTextSprite = ituSceneFindWidget(&theScene, "seatLegTextSprite");
		assert(seatLegTextSprite);

		for (i = 0; i < 6; i++)
		{
			sprintf(tmp, "seatBackIcon%d", i);
			seatBackIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(seatBackIcon[i]);
		}

		for (i = 0; i < 10; i++)
		{
			sprintf(tmp, "seatLegIcon%d", i);
			seatLegIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(seatLegIcon[i]);
		}
	}

	for (i = 0; i < 6; i++)
	{
		ituWidgetSetVisible(seatBackIcon[i], false);
	}
	ituWidgetSetVisible(seatBackIcon[backIndex], true);
	ituSpriteGoto(seatBackTextSprite, backIndex);

	for (i = 0; i < 10; i++)
	{
		ituWidgetSetVisible(seatLegIcon[i], false);
	}
	ituWidgetSetVisible(seatLegIcon[legIndex], true);
	ituSpriteGoto(seatLegTextSprite, legIndex);

    return true;
}

bool SeatBackDeBtnOnMouseUp(ITUWidget* widget, char* param)
{
	if (backIndex > 0)
	{
		ituWidgetHide(seatBackIcon[backIndex], ITU_EFFECT_FADE, 20);
		backIndex--;
		ituWidgetShow(seatBackIcon[backIndex], ITU_EFFECT_FADE, 20);
	}
	ituSpriteGoto(seatBackTextSprite, backIndex);
    return true;
}

bool SeatBackAddBtnOnMouseUp(ITUWidget* widget, char* param)
{
	if (backIndex < 5)
	{
		ituWidgetHide(seatBackIcon[backIndex], ITU_EFFECT_FADE, 20);
		backIndex++;
		ituWidgetShow(seatBackIcon[backIndex], ITU_EFFECT_FADE, 20);
	}
	ituSpriteGoto(seatBackTextSprite, backIndex);
    return true;
}

bool SeatLegDeBtnOnMouseUp(ITUWidget* widget, char* param)
{
	if (legIndex > 0)
	{
		ituWidgetHide(seatLegIcon[legIndex], ITU_EFFECT_FADE, 20);
		legIndex--;
		ituWidgetShow(seatLegIcon[legIndex], ITU_EFFECT_FADE, 20);
	}
	ituSpriteGoto(seatLegTextSprite, legIndex);
    return true;
}

bool SeatLegAddBtnOnMouseUp(ITUWidget* widget, char* param)
{
	if (legIndex < 9)
	{
		ituWidgetHide(seatLegIcon[legIndex], ITU_EFFECT_FADE, 20);
		legIndex++;
		ituWidgetShow(seatLegIcon[legIndex], ITU_EFFECT_FADE, 20);
	}
	ituSpriteGoto(seatLegTextSprite, legIndex);
    return true;
}

