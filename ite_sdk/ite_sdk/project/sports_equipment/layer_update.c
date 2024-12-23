#include <assert.h>
#include <stdlib.h>
#include "scene.h"
#include "sports_equipment.h"
/* widgets:
updateLayer
updateBackground
updateDialogBackground
DownLoadButton
IsLatestContainer
Text192
LatestVersionContainer
Text188
LatestVersionText
Text187
Text186
updateExitButton
*/

static ITUContainer* updateLatestVersionContainer;
static ITUContainer* updateIsLatestContainer;
static ITUText* updateLatestVersionText; // the latest version
static ITUText* updateVersionText; //the current version
static ITUText* updateProgressText;
static ITUButton* updateDownloadButton;
static ITUButton* updateExitButton;
ITUProgressBar* updateDownloadProgressBar;
static ITUBackground* updateLoadBackground;


bool updateOnEnter(ITUWidget* widget, char* param)
{

	if (!updateLatestVersionContainer)
	{
		updateLatestVersionContainer = ituSceneFindWidget(&theScene, "updateLatestVersionContainer");
		assert(updateLatestVersionContainer);

		updateIsLatestContainer = ituSceneFindWidget(&theScene, "updateIsLatestContainer");
		assert(updateIsLatestContainer);

		updateLatestVersionText = ituSceneFindWidget(&theScene, "updateLatestVersionText");
		assert(updateLatestVersionText);

		updateVersionText = ituSceneFindWidget(&theScene, "updateVersionText");
		assert(updateVersionText);

		updateProgressText = ituSceneFindWidget(&theScene, "updateProgressText");
		assert(updateProgressText);
		
		updateDownloadButton = ituSceneFindWidget(&theScene, "updateDownloadButton");
		assert(updateDownloadButton);

		updateExitButton = ituSceneFindWidget(&theScene, "updateExitButton");
		assert(updateExitButton);

		updateDownloadProgressBar = ituSceneFindWidget(&theScene, "updateDownloadProgressBar");
		assert(updateDownloadProgressBar);

		updateLoadBackground = ituSceneFindWidget(&theScene, "updateLoadBackground");
		assert(updateLoadBackground);
	}

	//show the current version num
	ituTextSetString(updateVersionText, theConfig.version);
#ifndef CFG_ESP32_SDIO_AT
	//no file on server
	ituWidgetSetVisible(updateIsLatestContainer, true);
	ituWidgetSetVisible(updateLatestVersionContainer, false);
	ituWidgetSetColor(updateDownloadButton, 255, 200, 200, 200);
	ituWidgetSetColor(&(updateDownloadButton->text), 255, 180, 180, 180);
	ituWidgetDisable(updateDownloadButton);
#else
	/*Todo:
	connect to server and search*/
	if (UpgradeCheckFW())
	{
		if (1)//strcmp(theConfig.version, LatestVersion) && LatestVersion > CurrentVersion ) 
		{
			ituTextSetString(updateLatestVersionText, "new");//LatestVersion);
			ituWidgetSetVisible(updateLatestVersionContainer, true);
			ituWidgetSetVisible(updateIsLatestContainer, false);
		}
		else// now is the latest version
		{
			ituWidgetSetVisible(updateIsLatestContainer, true);
			ituWidgetSetVisible(updateLatestVersionContainer, false);
			ituWidgetSetColor(updateDownloadButton, 255,200,200,200);
			ituWidgetSetColor(&(updateDownloadButton->text), 255, 180, 180, 180);
			ituWidgetDisable(updateDownloadButton);
		}
	}
	else //no file on server
	{
		ituWidgetSetVisible(updateIsLatestContainer, true);
		ituWidgetSetVisible(updateLatestVersionContainer, false);
		ituWidgetSetColor(updateDownloadButton, 255,200,200,200);
		ituWidgetSetColor(&(updateDownloadButton->text), 255, 180, 180, 180);
		ituWidgetDisable(updateDownloadButton);
	}
#endif
	

    return true;
}

bool updateOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

bool UpdateDownloadButtonOnPress(ITUWidget* widget, char* param)
{
	int value = 0;

	ituWidgetSetVisible(updateLoadBackground, true);
	ituProgressBarSetValue(updateDownloadProgressBar, value);
	ituWidgetSetColor(updateDownloadButton, 255,200,200,200);
	ituWidgetSetColor(&(updateDownloadButton->text), 255, 180, 180, 180);

#ifdef CFG_ESP32_SDIO_AT
	UpgradeGetPKG();
#endif
	/*Todo:
	start download the firmware
	progress bar start run
	show updateProgressText
	*/
	return true;
}
