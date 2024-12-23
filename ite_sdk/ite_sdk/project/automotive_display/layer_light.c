#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"

/* widgets:
lightLayer
lightBackIcon
lightFrontIcon
Animation71
Background72
Text73
Button74
Background75
Text76
Button78
Animation70
Background58
Text64
Button60
Background66
Text68
Button69
lightBackBtAnimation
Button51
Button50
lightBackMenuBtAnimation
Button24
Button26
lightTextAnimation
Text21
Text20
Background294
lightBgAnimation
Background154
Background81
Background82
Background5
*/
static ITUIcon* lightFrontIcon;
static ITUIcon* lightBackIcon;

static bool frontLight = false;
static bool backLight = false;

bool LightOnEnter(ITUWidget* widget, char* param)
{
	if (!lightFrontIcon)
	{
		lightFrontIcon = ituSceneFindWidget(&theScene, "lightFrontIcon");
		assert(lightFrontIcon);

		lightBackIcon = ituSceneFindWidget(&theScene, "lightBackIcon");
		assert(lightBackIcon);
	}

	ituWidgetSetVisible(lightFrontIcon, frontLight);
	ituWidgetSetVisible(lightBackIcon, backLight);

    return true;
}

bool LightBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	backLight = !backLight;
	ituWidgetSetVisible(lightBackIcon, backLight);
    return true;
}

bool LightFrontBtnOnMouseUp(ITUWidget* widget, char* param)
{
	frontLight = !frontLight;
	ituWidgetSetVisible(lightFrontIcon, frontLight);
    return true;
}

