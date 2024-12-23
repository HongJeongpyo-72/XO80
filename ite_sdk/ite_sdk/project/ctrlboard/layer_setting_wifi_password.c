#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"
#include "wifiMgr.h"
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
#include "wifi_constants.h"
#endif

extern unsigned char  py_ime(unsigned char *input_py_val,unsigned char * get_hanzi,unsigned short*hh);
extern ITUTextBox* settingWiFiMainNameTextBox;
extern ITUTextBox* settingWiFiMainPasswordTextBox;

static ITULayer      *settingWiFiSsidLayer;
static ITULayer      *settingWiFiMainLayer;
static ITUTextBox    *settingWiFiPasswordTextBox;
static ITUKeyboard   *settingWiFiPasswordKeyboard;
static ITUBackground *settingWiFiPasswordEnUpperBackground;
static ITUBackground *settingWiFiPasswordEnLowerBackground;
static ITUBackground *settingWiFiPasswordChsBackground;
static ITUBackground *settingWiFiPasswordSymbolBackground;
static ITUButton     *settingWiFiPasswordChs1Button;
static ITUButton     *settingWiFiPasswordChs2Button;
static ITUButton     *settingWiFiPasswordChs3Button;
static ITUButton     *settingWiFiPasswordChs4Button;
static ITUButton     *settingWiFiPasswordChs5Button;
static ITUButton     *settingWiFiPasswordChs6Button;
static ITUButton     *settingWiFiPasswordChs7Button;
static ITUButton     *settingWiFiPasswordChs8Button;
static ITUButton     *settingWiFiPasswordChs9Button;
static ITUButton     *settingWiFiPasswordChs0Button;
static ITUButton     *settingWiFiPasswordSymbolButton;
static ITUButton     *settingWiFiPasswordEnUpperButton;
static ITUButton     *settingWiFiPasswordEnLowerButton;
static ITUButton     *settingWiFiPasswordChsButton;

static int           settingWiFiPasswordChsIndex;
static unsigned char settingWiFiPasswordChsString[1200];
static unsigned char* settingWiFiPasswordChsPageIndexes[20];
static int settingWiFiPasswordChsPageIndex;
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
static char preSsidName[WIFI_SSID_MAXLEN];
static unsigned long gAPSecurityMode;
#else
static int           gAPSecurityMode;
#endif
ITUButton            **settingWiFiPasswordButtons[] =
{
    &settingWiFiPasswordChs1Button,
    &settingWiFiPasswordChs2Button,
    &settingWiFiPasswordChs3Button,
    &settingWiFiPasswordChs4Button,
    &settingWiFiPasswordChs5Button,
    &settingWiFiPasswordChs6Button,
    &settingWiFiPasswordChs7Button,
    &settingWiFiPasswordChs8Button,
    &settingWiFiPasswordChs9Button,
    &settingWiFiPasswordChs0Button,
};

static ITULayer* settingWiFiPasswordLastLayer;

void _ClearWiFiPasswordButtonString()
{
    int count = sizeof(settingWiFiPasswordButtons) / sizeof(settingWiFiPasswordButtons[0]);
    int i;
    for (i = 0; i < count; ++i)
        ituButtonSetString(*settingWiFiPasswordButtons[i], NULL);
}

void _SetWiFiPasswordButtonString(int i, char* string)
{
    int count = sizeof(settingWiFiPasswordButtons) / sizeof(settingWiFiPasswordButtons[0]);

    if ((!(i & 1)) && ((i >> 1) < count))   // make sure i == 0, 2, 4, ...., 18
    {
        ITUButton *btn = *settingWiFiPasswordButtons[i >> 1];
        ituButtonSetString(btn, string);
    }
}

bool SettingWiFiPasswordEnterButtonOnPress(ITUWidget *widget, char *param)
{
    char buf[512];
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
    unsigned long security;
    char security_ite[3];
#else
    char security[3];
#endif

    strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));

    if (settingWiFiPasswordLastLayer == settingWiFiMainLayer)
    {
        if (settingWiFiPasswordTextBox->textboxFlags & ITU_TEXTBOX_PASSWORD)
            strcpy(theConfig.password, buf);
        else
            strcpy(theConfig.ssid, buf);

        ituLayerGoto(settingWiFiMainLayer);
    }
    else
    {
        memset(theConfig.password, 0, sizeof(theConfig.password));
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
		security = gAPSecurityMode;

		/*printf("====>security: %16s, gAPSecurityMode: 0x%X\n",
			((security == RTW_SECURITY_OPEN ) ? "Open" :
			(security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
			(security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
			(security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
			(security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
			(security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
			(security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
			(security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" : "Unknown"), gAPSecurityMode);*/
#else
        sprintf(security, "%d", gAPSecurityMode);
#endif

    #ifdef CFG_NET_WIFI
        WifiMgr_Sta_Disconnect();
        //Do not set to WIFI_SWITCH_Off
    #endif
        if (gAPSecurityMode > 0)
        {
            strcpy(theConfig.password, buf);
        }

#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
        strcpy(security_ite, WifiMgr_Secu_RTL_To_ITE(security));
        strcpy(theConfig.secumode, security_ite);
        strcpy(theConfig.ssid, preSsidName);
#else
        strcpy(theConfig.secumode, security);
#endif
        ConfigSave();
        printf("SettingWiFiPasswordEnterButtonOnPress  %s\n", buf);
        if (1)
        {
    #ifdef CFG_NET_WIFI
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
            WifiMgr_Sta_Connect(preSsidName, theConfig.password, theConfig.secumode);
            ituLayerGoto(settingWiFiMainLayer);
#else
            WifiMgr_Sta_Connect(theConfig.ssid, theConfig.password, security);
#endif
#endif

#ifndef CFG_NET_WIFI_SDIO
            ituLayerGoto(settingWiFiSsidLayer);
#endif
        }
        else
        {
            ituTextBoxSetString(settingWiFiPasswordTextBox, NULL);
        }
    }
    return true;
}

bool SettingWiFiPasswordBackButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));
        buf[settingWiFiPasswordChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox *)settingWiFiPasswordKeyboard->target, buf);
        settingWiFiPasswordChsIndex      = -1;
        _ClearWiFiPasswordButtonString();
    }
    else
    {
        ituTextBoxBack((ITUTextBox *)settingWiFiPasswordKeyboard->target);
    }
    settingWiFiPasswordChsPageIndex = -1;
    return true;
}

bool SettingWiFiPasswordEnUpperButtonOnPress(ITUWidget *widget, char *param)
{
    settingWiFiPasswordChsPageIndex = -1;
    return true;
}

bool SettingWiFiPasswordChsButtonOnPress(ITUWidget *widget, char *param)
{
    _ClearWiFiPasswordButtonString();
    return true;
}

bool SettingWiFiPasswordPageUpButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsPageIndex > 0)
    {
        unsigned char* ptr;
        wchar_t wc;
        int i;

        ituButtonSetString(settingWiFiPasswordChs1Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs2Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs3Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs4Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs5Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs6Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs7Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs8Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs9Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs0Button, NULL);

        i = 0;
        ptr = settingWiFiPasswordChsPageIndexes[settingWiFiPasswordChsPageIndex - 1];

        do
        {
            int len = strlen(ptr);

            if (len > 0)
                len = mbtowc(&wc, ptr, len);

            if (len > 0)
            {
                ITUButton* btn = NULL;
                char buf[512];
                memcpy(buf, ptr, len);
                buf[len] = '\0';

                if (i == 0)
                    btn = settingWiFiPasswordChs1Button;
                else if (i == 1)
                    btn = settingWiFiPasswordChs2Button;
                else if (i == 2)
                    btn = settingWiFiPasswordChs3Button;
                else if (i == 3)
                    btn = settingWiFiPasswordChs4Button;
                else if (i == 4)
                    btn = settingWiFiPasswordChs5Button;
                else if (i == 5)
                    btn = settingWiFiPasswordChs6Button;
                else if (i == 6)
                    btn = settingWiFiPasswordChs7Button;
                else if (i == 7)
                    btn = settingWiFiPasswordChs8Button;
                else if (i == 8)
                    btn = settingWiFiPasswordChs9Button;
                else if (i == 9)
                    btn = settingWiFiPasswordChs0Button;

                if (btn)
                    ituButtonSetString(btn, buf);
            }
            else
            {
                break;
            }
            i++;
            ptr += len;

        } while (i < 10);

        settingWiFiPasswordChsPageIndex--;
    }
    return true;
}

bool SettingWiFiPasswordPageDownButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsPageIndex >= 0 && settingWiFiPasswordChsPageIndex < ITH_COUNT_OF(settingWiFiPasswordChsPageIndexes) - 1 && settingWiFiPasswordChsPageIndexes[settingWiFiPasswordChsPageIndex + 1])
    {
        unsigned char* ptr;
        wchar_t wc;
        int i;

        ituButtonSetString(settingWiFiPasswordChs1Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs2Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs3Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs4Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs5Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs6Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs7Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs8Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs9Button, NULL);
        ituButtonSetString(settingWiFiPasswordChs0Button, NULL);

        i = 0;
        ptr = settingWiFiPasswordChsPageIndexes[settingWiFiPasswordChsPageIndex + 1];

        do
        {
            int len = strlen(ptr);

            if (len > 0)
                len = mbtowc(&wc, ptr, len);

            if (len > 0)
            {
                ITUButton* btn = NULL;
                char buf[512];
                memcpy(buf, ptr, len);
                buf[len] = '\0';

                if (i == 0)
                    btn = settingWiFiPasswordChs1Button;
                else if (i == 1)
                    btn = settingWiFiPasswordChs2Button;
                else if (i == 2)
                    btn = settingWiFiPasswordChs3Button;
                else if (i == 3)
                    btn = settingWiFiPasswordChs4Button;
                else if (i == 4)
                    btn = settingWiFiPasswordChs5Button;
                else if (i == 5)
                    btn = settingWiFiPasswordChs6Button;
                else if (i == 6)
                    btn = settingWiFiPasswordChs7Button;
                else if (i == 7)
                    btn = settingWiFiPasswordChs8Button;
                else if (i == 8)
                    btn = settingWiFiPasswordChs9Button;
                else if (i == 9)
                    btn = settingWiFiPasswordChs0Button;

                if (btn)
                    ituButtonSetString(btn, buf);
            }
            else
            {
                break;
            }
            i++;
            ptr += len;

        } while (i < 10);

        settingWiFiPasswordChsPageIndex++;

        if (i >= 10)
        {
            settingWiFiPasswordChsPageIndexes[settingWiFiPasswordChsPageIndex + 1] = ptr;
        }
        else
        {
            settingWiFiPasswordChsPageIndexes[settingWiFiPasswordChsPageIndex + 1] = NULL;
        }
    }
	return true;
}

bool SettingWiFiPasswordChsCharButtonOnPress(ITUWidget *widget, char *param)
{
    ITUButton *btn   = (ITUButton *) widget;
    char      *input = ituTextGetString(&btn->text);
    char      buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char *str  = ituTextGetString(settingWiFiPasswordKeyboard->target);
        int  count = str ? strlen(str) : 0;

        if (count < settingWiFiPasswordTextBox->maxLen)
        {
            int len;
            unsigned short hh = 0;

            if (settingWiFiPasswordChsIndex == -1)
            {
                settingWiFiPasswordChsIndex = count;
            }
            if (count - settingWiFiPasswordChsIndex < 6)
                ituTextBoxInput((ITUTextBox *)settingWiFiPasswordKeyboard->target, input);

            str    = ituTextGetString(settingWiFiPasswordKeyboard->target);
            len = strlen(&str[settingWiFiPasswordChsIndex]);
            memset(buf, ' ', 6);
            memcpy(buf, &str[settingWiFiPasswordChsIndex], len);
            buf[6] = '\0';
            memset(settingWiFiPasswordChsString, '\0', sizeof(settingWiFiPasswordChsString));
            if (py_ime(buf, settingWiFiPasswordChsString, &hh))
            {
                unsigned char* ptr;
                wchar_t wc;
                int i;

                _ClearWiFiPasswordButtonString();

                i = 0;
                ptr = settingWiFiPasswordChsString;

                do
                {
                    len = strlen(ptr);

                    if (len > 0)
                        len = mbtowc(&wc, ptr, len);

                    if (len > 0)
                    {
                        ITUButton* btn = NULL;
                        memcpy(buf, ptr, len);
                        buf[len] = '\0';

                        if (i == 0)
                            btn = settingWiFiPasswordChs1Button;
                        else if (i == 1)
                            btn = settingWiFiPasswordChs2Button;
                        else if (i == 2)
                            btn = settingWiFiPasswordChs3Button;
                        else if (i == 3)
                            btn = settingWiFiPasswordChs4Button;
                        else if (i == 4)
                            btn = settingWiFiPasswordChs5Button;
                        else if (i == 5)
                            btn = settingWiFiPasswordChs6Button;
                        else if (i == 6)
                            btn = settingWiFiPasswordChs7Button;
                        else if (i == 7)
                            btn = settingWiFiPasswordChs8Button;
                        else if (i == 8)
                            btn = settingWiFiPasswordChs9Button;
                        else if (i == 9)
                            btn = settingWiFiPasswordChs0Button;

                        if (btn)
                            ituButtonSetString(btn, buf);
                    }
                    else
                    {
                        break;
                    }
                    i++;
                    ptr += len;

                } while (i < 10);

                settingWiFiPasswordChsPageIndex = 0;
                settingWiFiPasswordChsPageIndexes[0] = settingWiFiPasswordChsString;

                if (i >= 10)
                {
                    settingWiFiPasswordChsPageIndexes[1] = ptr;
                }
                else
                {
                    settingWiFiPasswordChsPageIndexes[1] = NULL;
                }
            }
        }
    }
    else
    {
        if (settingWiFiPasswordChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));
            buf[settingWiFiPasswordChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox *)settingWiFiPasswordKeyboard->target, buf);
			settingWiFiPasswordChsIndex = settingWiFiPasswordChsPageIndex = -1;
            _ClearWiFiPasswordButtonString();
        }
    }
    return true;
}

int SettingWiFiPasswordSetData(char *ssid,
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
	unsigned long
#else
	int
#endif
	securityMode)
{
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
    strcpy(preSsidName, ssid);
#else
    strcpy(theConfig.ssid, ssid);
#endif

    gAPSecurityMode = securityMode;
    //printf("SettingWiFiPasswordSetData: %s(%d) \n", ssid, securityMode);
    return 0;
}

bool SettingWiFiPasswordOnEnter(ITUWidget *widget, char *param)
{
    if (!settingWiFiSsidLayer)
    {
        settingWiFiSsidLayer                 = ituSceneFindWidget(&theScene, "settingWiFiSsidLayer");
        assert(settingWiFiSsidLayer);

        settingWiFiMainLayer                 = ituSceneFindWidget(&theScene, "settingWiFiMainLayer");
        assert(settingWiFiMainLayer);

        settingWiFiPasswordTextBox           = ituSceneFindWidget(&theScene, "settingWiFiPasswordTextBox");
        assert(settingWiFiPasswordTextBox);

        settingWiFiPasswordKeyboard          = ituSceneFindWidget(&theScene, "settingWiFiPasswordKeyboard");
        assert(settingWiFiPasswordKeyboard);
        settingWiFiPasswordKeyboard->target  = (ITUWidget *)settingWiFiPasswordTextBox;

        settingWiFiPasswordEnUpperBackground = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnUpperBackground");
        assert(settingWiFiPasswordEnUpperBackground);

        settingWiFiPasswordEnLowerBackground = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnLowerBackground");
        assert(settingWiFiPasswordEnLowerBackground);

        settingWiFiPasswordChsBackground     = ituSceneFindWidget(&theScene, "settingWiFiPasswordChsBackground");
        assert(settingWiFiPasswordChsBackground);

        settingWiFiPasswordSymbolBackground  = ituSceneFindWidget(&theScene, "settingWiFiPasswordSymbolBackground");
        assert(settingWiFiPasswordSymbolBackground);

        settingWiFiPasswordSymbolButton      = ituSceneFindWidget(&theScene, "settingWiFiPasswordSymbolButton");
        assert(settingWiFiPasswordSymbolButton);

        settingWiFiPasswordEnUpperButton     = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnUpperButton");
        assert(settingWiFiPasswordEnUpperButton);

        settingWiFiPasswordEnLowerButton     = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnLowerButton");
        assert(settingWiFiPasswordEnLowerButton);

        settingWiFiPasswordChsButton         = ituSceneFindWidget(&theScene, "settingWiFiPasswordChsButton");
        assert(settingWiFiPasswordChsButton);

        settingWiFiPasswordChs1Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs1Button");
        assert(settingWiFiPasswordChs1Button);

        settingWiFiPasswordChs2Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs2Button");
        assert(settingWiFiPasswordChs2Button);

        settingWiFiPasswordChs3Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs3Button");
        assert(settingWiFiPasswordChs3Button);

        settingWiFiPasswordChs4Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs4Button");
        assert(settingWiFiPasswordChs4Button);

        settingWiFiPasswordChs5Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs5Button");
        assert(settingWiFiPasswordChs5Button);

        settingWiFiPasswordChs6Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs6Button");
        assert(settingWiFiPasswordChs6Button);

        settingWiFiPasswordChs7Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs7Button");
        assert(settingWiFiPasswordChs7Button);

        settingWiFiPasswordChs8Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs8Button");
        assert(settingWiFiPasswordChs8Button);

        settingWiFiPasswordChs9Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs9Button");
        assert(settingWiFiPasswordChs9Button);

        settingWiFiPasswordChs0Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs0Button");
        assert(settingWiFiPasswordChs0Button);
    }

    ituWidgetSetVisible(settingWiFiPasswordEnUpperBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordEnLowerBackground, true);
    ituWidgetSetVisible(settingWiFiPasswordChsBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordSymbolBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordEnUpperButton, false);
    ituWidgetSetVisible(settingWiFiPasswordEnLowerButton, true);
    ituWidgetSetVisible(settingWiFiPasswordChsButton, false);

    if (strcmp(param, "settingWiFiMainLayer") == 0)
    {
        if (ituWidgetIsActive(settingWiFiMainNameTextBox))
        {
            settingWiFiPasswordTextBox->textboxFlags &= ~ITU_TEXTBOX_PASSWORD;
            ituTextBoxSetString(settingWiFiPasswordTextBox, ituTextBoxGetString(settingWiFiMainNameTextBox));
        }
        else
        {
            settingWiFiPasswordTextBox->textboxFlags |= ITU_TEXTBOX_PASSWORD;
            ituTextBoxSetString(settingWiFiPasswordTextBox, ituTextBoxGetString(settingWiFiMainPasswordTextBox));
        }
        settingWiFiPasswordLastLayer = settingWiFiMainLayer;
    }
    else
    {
        settingWiFiPasswordTextBox->textboxFlags |= ITU_TEXTBOX_PASSWORD;
        _ClearWiFiPasswordButtonString();
        settingWiFiPasswordLastLayer = settingWiFiSsidLayer;
        ituTextBoxSetString(settingWiFiPasswordTextBox, NULL);
    }

    settingWiFiPasswordChsIndex = settingWiFiPasswordChsPageIndex = -1;

    return true;
}

void SettingWiFiPasswordReset(void)
{
    settingWiFiSsidLayer = NULL;
}