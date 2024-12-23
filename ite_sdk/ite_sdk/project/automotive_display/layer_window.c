#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"

/* widgets:
windowLayer
windowBackBtAnimation
Button51
Button50
windowBackMenuBtAnimation
Button24
Button26
windowTextAnimation
Text21
Text20
Background295
windowsLeftBackAnimataion
Icon292
Icon293
windowsLeftFrontAnimataion
Icon289
Icon290
windowsRightBackAnimation
Icon286
Icon287
windowsRightFrontAnimation
Icon255
Icon256
Background251
Button252
Button253
Background248
Button249
Button250
Background245
Button246
Button247
Background242
Button244
Button243
Background5
*/
static ITUAnimation* windowLeftBackAnimation;
static ITUAnimation* windowLeftFrontAnimation;
static ITUAnimation* windowRightBackAnimation;
static ITUAnimation* windowRightFrontAnimation;

static  bool leftBackUp = false;
static  bool leftFrontUp = false;
static  bool rightBackUp = false;
static  bool rightFrontUp = false;

bool WindowOnEnter(ITUWidget* widget, char* param)
{
	if (!windowLeftBackAnimation)
	{
		windowLeftBackAnimation = ituSceneFindWidget(&theScene, "windowLeftBackAnimation");
		assert(windowLeftBackAnimation);

		windowLeftFrontAnimation = ituSceneFindWidget(&theScene, "windowLeftFrontAnimation");
		assert(windowLeftFrontAnimation);

		windowRightBackAnimation = ituSceneFindWidget(&theScene, "windowRightBackAnimation");
		assert(windowRightBackAnimation);

		windowRightFrontAnimation = ituSceneFindWidget(&theScene, "windowRightFrontAnimation");
		assert(windowRightFrontAnimation);
	}

	if (leftBackUp)
		ituAnimationGoto(windowLeftBackAnimation, 1);
	else
		ituAnimationGoto(windowLeftBackAnimation, 0);

	if (leftFrontUp)
		ituAnimationGoto(windowLeftFrontAnimation, 1);
	else
		ituAnimationGoto(windowLeftFrontAnimation, 0);

	if (rightBackUp)
		ituAnimationGoto(windowRightBackAnimation, 1);
	else
		ituAnimationGoto(windowRightBackAnimation, 0);

	if (rightFrontUp)
		ituAnimationGoto(windowRightFrontAnimation, 1);
	else
		ituAnimationGoto(windowRightFrontAnimation, 0);


    return true;
}

bool WindowLeftBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	int frame = windowLeftBackAnimation->frame;
	int totalframe = windowLeftBackAnimation->totalframe;
	bool playing = windowLeftBackAnimation->playing;

	if (!leftBackUp)
	{
		ituAnimationPlay(windowLeftBackAnimation, 0);
		leftBackUp = true;
	}
	else
	{
		ituAnimationReversePlay(windowLeftBackAnimation, 1);
		leftBackUp = false;
	}
	if (playing)
		ituAnimationGotoFrame(windowLeftBackAnimation, totalframe - frame);
    return true;
}

bool WindowLeftFrontBtnOnMouseUp(ITUWidget* widget, char* param)
{
	int frame = windowLeftFrontAnimation->frame;
	int totalframe = windowLeftFrontAnimation->totalframe;
	bool playing = windowLeftFrontAnimation->playing;

	if (!leftFrontUp)
	{
		ituAnimationPlay(windowLeftFrontAnimation, 0);
		leftFrontUp = true;
	}
	else
	{
		ituAnimationReversePlay(windowLeftFrontAnimation, 1);
		leftFrontUp = false;
	}
	if (playing)
		ituAnimationGotoFrame(windowLeftFrontAnimation, totalframe - frame);
    return true;
}

bool WindowRightBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	int frame = windowRightBackAnimation->frame;
	int totalframe = windowRightBackAnimation->totalframe;
	bool playing = windowRightBackAnimation->playing;

	if (!rightBackUp)
	{
		ituAnimationPlay(windowRightBackAnimation, 0);
		rightBackUp = true;
	}
	else
	{
		ituAnimationReversePlay(windowRightBackAnimation, 1);
		rightBackUp = false;
	}
	if (playing)
		ituAnimationGotoFrame(windowRightBackAnimation, totalframe - frame);
    return true;
}

bool WindowRightFrontBtnOnMouseUp(ITUWidget* widget, char* param)
{
	int frame = windowRightFrontAnimation->frame;
	int totalframe = windowRightFrontAnimation->totalframe;
	bool playing = windowRightFrontAnimation->playing;

	if (!rightFrontUp)
	{
		ituAnimationPlay(windowRightFrontAnimation, 0);
		rightFrontUp = true;
	}
	else
	{
		ituAnimationReversePlay(windowRightFrontAnimation, 1);
		rightFrontUp = false;
	}
	if (playing)
		ituAnimationGotoFrame(windowRightFrontAnimation, totalframe- frame);
    return true;
}

