#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itu.h"

#define GESTURE_THRESHOLD 40
#define MS_PER_FRAME 17
#define FPS_ENABLE

static ITUScene scene;

ITUComboTable* cbt = NULL;
ITUText* text = NULL;

bool PageChanged(ITUWidget* widget, char* param)
{
	if (cbt && text)
	{
		int vPage = ituComboTableGetCurrentPageNumber(cbt);
		char vStr[10] = "";
		sprintf(vStr, "%d", vPage);
		ituTextSetString(text, vStr);
	}

    return true;
}

bool DelSelect(ITUWidget* widget, char* param)
{
	if (cbt)
	{
		if (cbt->selectIndex >= 0)
			ituComboTableDelItem(cbt, cbt->selectIndex);
	}
	return true;
}

bool ItemSelected(ITUWidget* widget, char* param)
{
	if (cbt)
	{
		ITUBackground* item_bg = NULL;
		if (cbt->lastselectIndex >= 0)
		{
			ITUColor color = { 255, 255, 255, 255 };
			ITUWidget* target = ituComboTableGetItemChild(cbt, cbt->lastselectIndex, ITU_BACKGROUND, 0);
			ituWidgetSetColor(target, color.alpha, color.red, color.green, color.blue);
		}
		if (cbt->selectIndex >= 0)
		{
			ITUColor color = { 255, 255, 0, 0 };
			ITUWidget* target = ituComboTableGetItemChild(cbt, cbt->selectIndex, ITU_BACKGROUND, 0);
			ituWidgetSetColor(target, color.alpha, color.red, color.green, color.blue);
		}
	}
	return true;
}

bool MainOnTimer(ITUWidget* widget, char* param)
{
	return false;
}

bool MainOnEnter(ITUWidget* widget, char* param)
{
	if (!cbt)
	{
		cbt = (ITUComboTable*)ituSceneFindWidget(&scene, "ComboTable1");

		if (cbt)
		{
			ITUWidget* tmpObj = NULL;
			int i = 0;
			for (i = 0; i < cbt->itemcount; i++)
			{
				char str[12];
				sprintf(str, "Item %d", i);
				tmpObj = ituComboTableGetItemChild(cbt, i, ITU_TEXT, 1);
				if (tmpObj)
					ituTextSetString(tmpObj, str);
			}
		}
	}

	if (!text)
		text = (ITUText*)ituSceneFindWidget(&scene, "Text1");

	return true;
}

static ITUActionFunction actionFunctions[] =
{
	"PageChanged", PageChanged,
	"DelSelect", DelSelect,
	"ItemSelected", ItemSelected,
	"MainOnEnter", MainOnEnter,
	"MainOnTimer", MainOnTimer,
    NULL,         NULL
};

int SDL_main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Event ev;
    int done, delay, lastx, lasty, frames;
    uint32_t tick, dblclk, lasttick;

    // wait mouting USB storage
#ifndef _WIN32
    sleep(3);
#endif

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return (1);
    }
    window = SDL_CreateWindow("ITU ComboTable Test",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1024, 600, 0);
    if (!window)
    {
        printf("Couldn't create 1024x600 window: %s\n",
                SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    // init itu
    ituLcdInit();
    
#ifdef CFG_M2D_ENABLE
    ituM2dInit();
#else
    ituSWInit();
#endif

    ituFtInit();
    ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/DroidSans.ttf", ITU_GLYPH_8BPP);

    // load itu file
    tick = SDL_GetTicks();

    ituSceneInit(&scene, NULL);
    ituSceneSetFunctionTable(&scene, actionFunctions);
    ituSceneLoadFile(&scene, CFG_PRIVATE_DRIVE ":/combotable.itu");

    printf("loading time: %dms\n", SDL_GetTicks() - tick);

    scene.leftKey   = SDLK_LEFT;
    scene.upKey     = SDLK_UP;
    scene.rightKey  = SDLK_RIGHT;
    scene.downKey   = SDLK_DOWN;


	

    /* Watch keystrokes */
    done = dblclk = lastx = lasty = lasttick = frames = 0;
    while (!done)
    {
        bool result = false;

        tick = SDL_GetTicks();

#ifdef FPS_ENABLE
		frames++;
		if (tick - lasttick >= 1000)
		{
			printf("fps: %d\n", frames);
			frames = 0;
			lasttick = tick;
		}
#endif // FPS_ENABLE

        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_MOUSEMOTION:
                result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEMOVE, ev.button.button, ev.button.x, ev.button.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
                {
                    uint32_t t = SDL_GetTicks();
                    if (t - dblclk <= 300)
                    {
                        result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEDOUBLECLICK, ev.button.button, ev.button.x, ev.button.y);
                        dblclk = 0;
                    }
                    else
                    {
						result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
                        dblclk = t;
                        lastx = ev.button.x;
                        lasty = ev.button.y;
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                if (SDL_GetTicks() - dblclk <= 300)
                {
                    int xdiff = abs(ev.button.x - lastx);
                    int ydiff = abs(ev.button.y - lasty);

                    if (xdiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.button.x > lastx)
                        {
                            printf("mouse: slide to right %d %d\n", ev.button.x, ev.button.y);
                            result |= ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.button.x, ev.button.y);
                        }
                        else
                        {
                            printf("mouse: slide to left %d %d\n", ev.button.x, ev.button.y);
                            result |= ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.button.x, ev.button.y);
                        }
                    }
                    else if (ydiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.button.y > lasty)
                        {
                            printf("mouse: slide to down %d %d\n", ev.button.x, ev.button.y);
                            result |= ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDEDOWN, 1, ev.button.x, ev.button.y);
                        }
                        else
                        {
                            printf("mouse: slide to up %d %d\n", ev.button.x, ev.button.y);
                            result |= ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDEUP, 1, ev.button.x, ev.button.y);
                        }
                    }
                }
                break;

            case SDL_FINGERDOWN:
                printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_FINGERMOTION:
                printf("touch: move %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEMOVE, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_FINGERUP:
                printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&scene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_SLIDEGESTURE:
                switch (ev.dgesture.gestureId)
                {
                case SDL_TG_LEFT:
                    printf("touch: slide to left %d %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_UP:
                    printf("touch: slide to up %d %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDEUP, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_RIGHT:
                    printf("touch: slide to right %d %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_DOWN:
                    printf("touch: slide to down %d %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&scene, ITU_EVENT_TOUCHSLIDEDOWN, 1, ev.dgesture.x, ev.dgesture.y);
                    break;
                }
                break;

            case SDL_QUIT:
                done = 1;
                break;
            }
        }

        result |= ituSceneUpdate(&scene, ITU_EVENT_TIMER, 0, 0, 0);
        if (result)
        {
            ituSceneDraw(&scene, ituGetDisplaySurface());
            ituFlip(ituGetDisplaySurface());
        }

		delay = MS_PER_FRAME - (SDL_GetTicks() - tick);
        if (delay > 0)
        {
            SDL_Delay(delay);
        }
    }

    SDL_Quit();
    return (0);
}
