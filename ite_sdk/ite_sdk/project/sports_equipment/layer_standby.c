#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "sports_equipment.h"

ITUBackground* standbyBackground;
static ITUBackground* standbyUpdateOKBackground;
static ITUText* standbyCurrentVersionText;
static ITUPopupButton* standbyStartPopupButton;
static ITUPopupButton* standbyUpdateExitButton;

bool StandbyStartPopupButtonOnPress(ITUWidget* widget, char* param)
{
    theProgram.restart = true;
    return true;
}

bool StandbyOnEnter(ITUWidget* widget, char* param)
{
    if (!standbyBackground)
    {
        standbyBackground = ituSceneFindWidget(&theScene, "standbyBackground");
        assert(standbyBackground);

		standbyUpdateOKBackground = ituSceneFindWidget(&theScene, "standbyUpdateOKBackground");
		assert(standbyUpdateOKBackground);

		standbyCurrentVersionText = ituSceneFindWidget(&theScene, "standbyCurrentVersionText");
		assert(standbyCurrentVersionText);
		
		standbyStartPopupButton = ituSceneFindWidget(&theScene, "standbyStartPopupButton");
		assert(standbyStartPopupButton);

		standbyUpdateExitButton = ituSceneFindWidget(&theScene, "standbyUpdateExitButton");
		assert(standbyUpdateExitButton);

        theProgram.pulse = theConfig.pulse;

        SceneSetReady(true);
    }

	if (theConfig.is_first_boot)
	{
		//show mainUpdateOKcontain
		ituWidgetSetVisible(standbyUpdateExitButton, true);
		ituWidgetSetVisible(standbyUpdateOKBackground, true);
		ituTextSetString(standbyCurrentVersionText, theConfig.version);
		ituWidgetDisable(standbyStartPopupButton);
		theConfig.is_first_boot = 0;
		ConfigSave();
	}


    return true;
}

void StandbyReset(void)
{
    standbyBackground = NULL;
}
