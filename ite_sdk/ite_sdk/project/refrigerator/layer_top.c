#include <assert.h>
#include <stdlib.h>
#include "scene.h"
#include "ite/itp.h"

static ITUContainer* topLatestVersionContainer;
static ITUContainer* topIsLatestContainer;
static ITUText* topLatestVersionText; // the latest version
static ITUText* topVersionText; //the current version
static ITUText* topProgressText;
static ITUButton* topDownloadButton;

ITUProgressBar* topDownloadProgressBar;
static ITUBackground* topLoadBackground;

static ITUButton*  topDialogExitButton;
static ITUBackground* topDialogBackground;

static ITUButton* topUpdateButton;
static ITUButton* topSettingButton;

//when first boot 
static ITUBackground* topUpdateOKBackground;
static ITUText*  topCurrentVersionText;
static ITUButton* topUpdateExitButton;

static ITUBackgroundButton *BackgroundButton7;

#ifdef CFG_ESP32_SDIO_AT
static const char *atc0[] = {
	"AT+CIPSTART=\"TCP\",\"192.168.191.102\",80\r\n",
	"AT+CIPSENDEX=30\r\n",
	"GET /ITEPKG03.PKG HTTP/1.1\r\n\r\n",
	"AT+CIPCLOSE\r\n", 
	"ATE0\r\n"
};
#endif

static void UpgradeFWInit(void);

bool TopOnEnter(ITUWidget* widget, char* param)
{
	if (!topLatestVersionContainer)
	{
		topLatestVersionContainer = ituSceneFindWidget(&theScene, "topLatestVersionContainer");
		assert(topLatestVersionContainer);

		topIsLatestContainer = ituSceneFindWidget(&theScene, "topIsLatestContainer");
		assert(topIsLatestContainer);

		topLatestVersionText = ituSceneFindWidget(&theScene, "topLatestVersionText");
		assert(topLatestVersionText);

		topVersionText = ituSceneFindWidget(&theScene, "topVersionText");
		assert(topVersionText);

		topProgressText = ituSceneFindWidget(&theScene, "topProgressText");
		assert(topProgressText);

		topDownloadButton = ituSceneFindWidget(&theScene, "topDownloadButton");
		assert(topDownloadButton);

		topDownloadProgressBar = ituSceneFindWidget(&theScene, "topDownloadProgressBar");
		assert(topDownloadProgressBar);

		topLoadBackground = ituSceneFindWidget(&theScene, "topLoadBackground");
		assert(topLoadBackground);

		topDialogExitButton = ituSceneFindWidget(&theScene, "topDialogExitButton");
		assert(topDialogExitButton);

		topDialogBackground = ituSceneFindWidget(&theScene, "topDialogBackground");
		assert(topDialogBackground);

		topUpdateOKBackground = ituSceneFindWidget(&theScene, "topUpdateOKBackground");
		assert(topUpdateOKBackground);

		topCurrentVersionText = ituSceneFindWidget(&theScene, "topCurrentVersionText");
		assert(topCurrentVersionText);

		topUpdateExitButton = ituSceneFindWidget(&theScene, "topUpdateExitButton");
		assert(topUpdateExitButton);

		topUpdateButton = ituSceneFindWidget(&theScene, "topUpdateButton");
		assert(topUpdateButton);

		topSettingButton = ituSceneFindWidget(&theScene, "topSettingButton");
		assert(topSettingButton);

		BackgroundButton7 = ituSceneFindWidget(&theScene, "BackgroundButton7");
		assert(BackgroundButton7);
	}
	

	if (theConfig.is_first_boot) //first boot, show updateOK
	{
		ituWidgetSetVisible(topUpdateExitButton, true);
		ituWidgetSetVisible(topUpdateOKBackground, true);
		ituTextSetString(topCurrentVersionText, theConfig.version);
		ituWidgetDisable(topUpdateButton);
		ituWidgetDisable(topSettingButton);
		ituWidgetShow(BackgroundButton7, 0,0);
		
		theConfig.is_first_boot = 0;
		ConfigSave();
	}
	return true;
}

bool TopDownloadButtonOnPress(ITUWidget* widget, char* param)
{
	int value = 0;

	ituProgressBarSetValue(topDownloadProgressBar, value);
	ituWidgetSetColor(topDownloadButton, 255, 200, 200, 200);
	ituWidgetSetColor(&(topDownloadButton->text), 255, 180, 180, 180);
#ifdef CFG_ESP32_SDIO_AT
	UpgradeGetPKG();
#endif
	
	/*Todo:
	start download the firmware
	progress bar start run
	*/
    return true;
}

bool TopUpdateButtonOnPress(ITUWidget* widget, char* param)
{
	ituTextSetString(topVersionText, theConfig.version);
#ifndef CFG_ESP32_SDIO_AT
	//no file on server
	ituWidgetSetVisible(topIsLatestContainer, true);
	ituWidgetSetVisible(topLatestVersionContainer, false);
	ituWidgetSetColor(topDownloadButton, 255, 200, 200, 200);
	ituWidgetSetColor(&(topDownloadButton->text), 255, 180, 180, 180);
	ituWidgetDisable(topDownloadButton);

#else
	/*Todo:
	connect to server and search*/


	if (UpgradeCheckFW())//have file on server)
	{
		if (1)//strcmp(theConfig.version, LatestVersion) && LatestVersion > CurrentVersion )
		{
			ituTextSetString(topLatestVersionText, "new");//LatestVersion);
			ituWidgetSetVisible(topLatestVersionContainer, true);
			ituWidgetSetVisible(topIsLatestContainer, false);
		}
		else// now is the latest version
		{
			ituWidgetSetVisible(topIsLatestContainer, true);
			ituWidgetSetVisible(topLatestVersionContainer, false);
			ituWidgetSetColor(topDownloadButton, 255,200,200,200);
			ituWidgetSetColor(&(topDownloadButton->text), 255, 180, 180, 180);
			ituWidgetDisable(topDownloadButton);
		}
	}
	else //no file on server
	{
			ituWidgetSetVisible(topIsLatestContainer, true);
			ituWidgetSetVisible(topLatestVersionContainer, false);
			ituWidgetSetColor(topDownloadButton, 255,200,200,200);
			ituWidgetSetColor(&(topDownloadButton->text), 255, 180, 180, 180);
			ituWidgetDisable(topDownloadButton);
	}
#endif
	return true;
}