#include <assert.h>
#include "scene.h"

int TimeOut = 5000;

static char widgetName[30];
static bool mainContainerIsVisible = false;
static bool mainOpenTimeOut[3] = { false };
static bool IsOpen[3] = { false };
static bool makeIce = false, iceFinish = false;
static uint32_t lastTick[3], lastAlarm, lastMakeIce;
static ITUContainer *mainOnContainer[3][2] = { 0, 0 };
static ITUAnimation *mainNCloseAnimation[3][2] = { 0, 0 };
static ITUAnimation *mainOpenSetAnimation = 0;
static ITUSprite *mainTempSprite[3][2][2][3] = {0, 0, 0, 0 };
static ITUSprite *mainMidMakeIceSprite[2] = { 0 };
static ITUSprite *mainDoorSprite[6] = { 0 };
static ITUButton *mainMidIceOffButton = 0;
static ITUButton *mainMidIceOnButton = 0;
static ITUButton *mainDemoButton[12] = { 0 };

bool MainOnEnter(ITUWidget* widget, char* param)
{
	if (!mainOnContainer[0][0])
	{
		int i, j, k, l;
		mainOnContainer[0][0] = ituSceneFindWidget(&theScene, "mainTopOnContainer0");
		assert(mainOnContainer[0][0]);

		mainOnContainer[0][1] = ituSceneFindWidget(&theScene, "mainTopOnContainer1");
		assert(mainOnContainer[0][1]);

		mainOnContainer[1][0] = ituSceneFindWidget(&theScene, "mainMidOnContainer0");
		assert(mainOnContainer[1][0]);

		mainOnContainer[1][1] = ituSceneFindWidget(&theScene, "mainMidOnContainer1");
		assert(mainOnContainer[1][1]);

		mainOnContainer[2][0] = ituSceneFindWidget(&theScene, "mainBotOnContainer0");
		assert(mainOnContainer[2][0]);

		mainOnContainer[2][1] = ituSceneFindWidget(&theScene, "mainBotOnContainer1");
		assert(mainOnContainer[2][1]);

		mainOpenSetAnimation = ituSceneFindWidget(&theScene, "mainOpenSetAnimation");
		assert(mainOpenSetAnimation);
		
		mainNCloseAnimation[0][0] = ituSceneFindWidget(&theScene, "mainTopNCloseAnimation0");
		assert(mainNCloseAnimation[0][0]);

		mainNCloseAnimation[0][1] = ituSceneFindWidget(&theScene, "mainTopNCloseAnimation1");
		assert(mainNCloseAnimation[0][1]);

		mainNCloseAnimation[1][0] = ituSceneFindWidget(&theScene, "mainMidNCloseAnimation0");
		assert(mainNCloseAnimation[1][0]);

		mainNCloseAnimation[1][1] = ituSceneFindWidget(&theScene, "mainMidNCloseAnimation1");
		assert(mainNCloseAnimation[1][1]);

		mainNCloseAnimation[2][0] = ituSceneFindWidget(&theScene, "mainBotNCloseAnimation0");
		assert(mainNCloseAnimation[2][0]);

		mainNCloseAnimation[2][1] = ituSceneFindWidget(&theScene, "mainBotNCloseAnimation1");
		assert(mainNCloseAnimation[2][1]);

		mainMidMakeIceSprite[0] = ituSceneFindWidget(&theScene, "mainMidMakeIceSprite0");
		assert(mainMidMakeIceSprite[0]);

		mainMidMakeIceSprite[1] = ituSceneFindWidget(&theScene, "mainMidMakeIceSprite1");
		assert(mainMidMakeIceSprite[1]);

		mainMidIceOffButton = ituSceneFindWidget(&theScene, "mainMidIceOffButton");
		assert(mainMidIceOffButton);

		mainMidIceOnButton = ituSceneFindWidget(&theScene, "mainMidIceOnButton");
		assert(mainMidIceOnButton);

		mainDemoButton[0] = ituSceneFindWidget(&theScene, "mainTriButton0");
		assert(mainDemoButton[0]);

		mainDemoButton[1] = ituSceneFindWidget(&theScene, "mainTriButton1");
		assert(mainDemoButton[1]);

		mainDemoButton[2] = ituSceneFindWidget(&theScene, "mainTriButton2");
		assert(mainDemoButton[2]);

		mainDemoButton[3] = ituSceneFindWidget(&theScene, "mainTriButton3");
		assert(mainDemoButton[3]);

		mainDemoButton[4] = ituSceneFindWidget(&theScene, "mainTriButton4");
		assert(mainDemoButton[4]);

		mainDemoButton[5] = ituSceneFindWidget(&theScene, "mainTriButton5");
		assert(mainDemoButton[5]);

		mainDemoButton[6] = ituSceneFindWidget(&theScene, "mainTopOffButton");
		assert(mainDemoButton[6]);

		mainDemoButton[7] = ituSceneFindWidget(&theScene, "mainTopOnButton");
		assert(mainDemoButton[7]);

		mainDemoButton[8] = ituSceneFindWidget(&theScene, "mainMidOffButton");
		assert(mainDemoButton[8]);

		mainDemoButton[9] = ituSceneFindWidget(&theScene, "mainMidOnButton");
		assert(mainDemoButton[9]);

		mainDemoButton[10] = ituSceneFindWidget(&theScene, "mainBotOffButton");
		assert(mainDemoButton[10]);

		mainDemoButton[11] = ituSceneFindWidget(&theScene, "mainBotOnButton");
		assert(mainDemoButton[11]);

		mainDoorSprite[0] = ituSceneFindWidget(&theScene, "mainTopSprite");
		assert(mainDoorSprite[0]);

		mainDoorSprite[1] = ituSceneFindWidget(&theScene, "mainTopRSprite");
		assert(mainDoorSprite[1]);

		mainDoorSprite[2] = ituSceneFindWidget(&theScene, "mainMidSprite");
		assert(mainDoorSprite[2]);

		mainDoorSprite[3] = ituSceneFindWidget(&theScene, "mainMidRSprite");
		assert(mainDoorSprite[3]);

		mainDoorSprite[4] = ituSceneFindWidget(&theScene, "mainBotSprite");
		assert(mainDoorSprite[4]);

		mainDoorSprite[5] = ituSceneFindWidget(&theScene, "mainBotRSprite");
		assert(mainDoorSprite[5]);


		for (i = 0; i < 3; i++){
			for (j = 0; j < 2; j++){
				for (k = 0; k < 2; k++){
					for (l = 0; l < 3; l++){
						sprintf(widgetName, "main%dTemp%dSprite%d%d", i, j, k, l);
						mainTempSprite[i][j][k][l] = ituSceneFindWidget(&theScene, widgetName);
						assert(mainTempSprite[i][j][k][l]);
					}
				}
			}
		}

		for (i = 0; i < 2; i++){
			for (j = 0; j < 2; j++){
				ituSpriteGoto(mainTempSprite[0][i][j][2], 6);
				ituSpriteGoto(mainTempSprite[1][i][j][0], 1);
				ituSpriteGoto(mainTempSprite[1][i][j][1], 2);
				ituSpriteGoto(mainTempSprite[1][i][j][2], 0);
				ituSpriteGoto(mainTempSprite[2][i][j][2], 3);
			}
		}
	}

	TimeOut = theConfig.alarm;

    return true;
}

bool MainOnTimer(ITUWidget* widget, char* param)
{
	int i;
	uint32_t diff[3], alarmDiff, makeIceDiff, tick = SDL_GetTicks();
	bool updated = false;

	for (i = 0; i < 3; i++){
		if (tick >= lastTick[i])
			diff[i] = tick - lastTick[i];
		else
			diff[i] = 0xFFFFFFFF - lastTick[i] + tick;

		if (ituWidgetIsVisible(mainOnContainer[i][0]) && diff[i]>TimeOut) 
		{
			if (!mainOpenTimeOut[i])
			{
				ituWidgetSetVisible(mainNCloseAnimation[i][0], true);
				ituAnimationPlay(mainNCloseAnimation[i][0], 0);
				ituWidgetSetVisible(mainNCloseAnimation[i][1], true);
				ituAnimationPlay(mainNCloseAnimation[i][1], 0);
				mainOpenTimeOut[i] = true;
				IsOpen[i] = true;
			}
			updated = true;
		}
	}

	if (IsOpen[0] || IsOpen[1] || IsOpen[2])
	{
		if (tick >= lastAlarm)
			alarmDiff = tick - lastAlarm;
		else
			alarmDiff = 0xFFFFFFFF - lastAlarm + tick;
		if (alarmDiff > 3000){
			AudioPlayMusic("A:/sounds/unclosed.mp3", NULL);
			lastAlarm = tick;
		}
		ScreenSaverRefresh();
	}

	if (makeIce)
	{
		if (tick >= lastMakeIce)
			makeIceDiff = tick - lastMakeIce;
		else
			makeIceDiff = 0xFFFFFFFF - lastMakeIce + tick;
		if (makeIceDiff > 2500){
			if (iceFinish){
				AudioPlayMusic("A:/sounds/finishIce.mp3", NULL);
				makeIce = false;
			}
			if (makeIceDiff > 10000){
				iceFinish = true;
				AudioPlayMusic("A:/sounds/finishIce.mp3", NULL);
				lastMakeIce = tick;
				ituWidgetSetVisible(mainMidIceOffButton, true);
				ituWidgetSetVisible(mainMidIceOnButton, false);
				for (i = 0; i < 2; i++){
					ituWidgetSetVisible(mainMidMakeIceSprite[i], false);
					ituSpriteStop(mainMidMakeIceSprite[i]);
				}
			}
		}
		ScreenSaverRefresh();
	}
	if (mainOpenSetAnimation->playing == 1 
		|| mainDoorSprite[0]->playing == 1
		|| mainDoorSprite[1]->playing == 1
		|| mainDoorSprite[2]->playing == 1
		|| mainDoorSprite[3]->playing == 1
		|| mainDoorSprite[4]->playing == 1
		|| mainDoorSprite[5]->playing == 1)
	{
		for (i = 0; i < 12; i++)
			ituWidgetDisable(mainDemoButton[i]);
	}
	else
	{
		for (i = 0; i < 12; i++)
			ituWidgetEnable(mainDemoButton[i]);
	}
		

	return updated;
}

bool MainTopSpriteOnStop(ITUWidget* widget, char* param)
{
	mainOpenTimeOut[0] = false;
	lastTick[0] = SDL_GetTicks();
	return true;
}

bool MainMidSpriteOnStop(ITUWidget* widget, char* param)
{
	mainOpenTimeOut[1] = false;
	lastTick[1] = SDL_GetTicks();
	return true;
}

bool MainBotSpriteOnStop(ITUWidget* widget, char* param)
{
	mainOpenTimeOut[2] = false;
	lastTick[2] = SDL_GetTicks();
	return true;
}

bool MainTopOnButtonOnPress(ITUWidget* widget, char* param)
{
	lastTick[0] = SDL_GetTicks();
	IsOpen[0] = false;
	ituWidgetSetVisible(mainNCloseAnimation[0][0], false);
	ituAnimationStop(mainNCloseAnimation[0][0]);
	ituWidgetSetVisible(mainNCloseAnimation[0][1], false);
	ituAnimationStop(mainNCloseAnimation[0][1]);
	return true;
}

bool MainMidOnButtonOnPress(ITUWidget* widget, char* param)
{
	lastTick[1] = SDL_GetTicks();
	IsOpen[1] = false;
	ituWidgetSetVisible(mainNCloseAnimation[1][0], false);
	ituAnimationStop(mainNCloseAnimation[1][0]);
	ituWidgetSetVisible(mainNCloseAnimation[1][1], false);
	ituAnimationStop(mainNCloseAnimation[1][1]);
	return true;
}

bool MainBotOnButtonOnPress(ITUWidget* widget, char* param)
{
	lastTick[2] = SDL_GetTicks();
	IsOpen[2] = false;
	ituWidgetSetVisible(mainNCloseAnimation[2][0], false);
	ituAnimationStop(mainNCloseAnimation[2][0]);
	ituWidgetSetVisible(mainNCloseAnimation[2][1], false);
	ituAnimationStop(mainNCloseAnimation[2][1]);
	return true;
}

bool MainMidIceOffButtonOnPress(ITUWidget* widget, char* param)
{
	makeIce = true;
	iceFinish = false;
	lastMakeIce = SDL_GetTicks();
	return true;
}

bool MainMidIceOnButtonOnPress(ITUWidget* widget, char* param)
{
	makeIce = false;
	iceFinish = false;
	return true;
}