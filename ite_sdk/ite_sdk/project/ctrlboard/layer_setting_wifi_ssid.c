﻿#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/ioctl.h"
#include "scene.h"
#include "ctrlboard.h"
#include "wifiMgr.h"

static ITUScrollListBox *settingWiFiSsidNameScrollListBox;
static ITUScrollListBox *settingWiFiSsidStatusScrollListBox;
static ITUScrollListBox *settingWiFiSsidSignalScrollListBox;
static ITULayer         *settingWiFiPasswordLayer;
static ITULayer         *settingWiFiNetworkLayer;
static bool              settingWiFiReturnLayer = false;

/*
static const char* settingWiFiSsidNameArray[] =
{
    "A SSID",
    "B SSID",
    "C SSID",
    "D SSID",
    "E SSID",
    "F SSID",
    "G SSID",
};
*/
static WIFI_MGR_SCANAP_LIST pList[64] = {0};
static int                  gnApCount = 0;

#define MAX_SSID_COUNT 15 // (sizeof(settingWiFiSsidNameArray) / sizeof(settingWiFiSsidNameArray[0]))

static int getMaxSsidCount()
{
    if (gnApCount <= 0)
    {
        return MAX_SSID_COUNT;
    }
    else
    {
        return gnApCount;
    }
}

bool SettingWiFiSsidNameScrollListBoxOnLoad(ITUWidget *widget, char *param)
{
    ITUListBox          *listbox = (ITUListBox *) widget;
    ITUScrollListBox    *slistbox = (ITUScrollListBox *) listbox;
    ITCTree             *node;
    int                 i, j, count;

    assert(listbox);

    count               = ituScrollListBoxGetItemCount(slistbox);
    node                = ituScrollListBoxGetLastPageItem(slistbox);

    listbox->pageCount  = getMaxSsidCount() ? (getMaxSsidCount() + count - 1) / count : 1;

    printf("SettingWiFiSsidNameScrollListBoxOnLoad,      SSID count per page = %d,   Need %d pages to show all counts \n", count, listbox->pageCount);

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex  = 1;
        listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText *scrolltext = (ITUScrollText *) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i   = 0;
    j   = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < getMaxSsidCount(); j++)
    {
        ITUScrollText   *scrolltext = (ITUScrollText *) node;
        char            buf[32];
        const char      *entry;//= settingWiFiSsidNameArray[j];

        if (theConfig.wifi_on_off == WIFIMGR_SWITCH_ON)
        {
            sprintf(buf, "%s", pList[j].ssidName);
        }
        else if (theConfig.wifi_on_off == WIFIMGR_SWITCH_OFF)
        {
            /*CANCEL PANEL DISPALY*/
            memset(pList[j].ssidName, 0, 32*sizeof(unsigned char));
            pList[j].rfQualityQuant = 0;

            sprintf(buf,    "%s",   pList[j].ssidName);
            sprintf(buf,    "%s",   pList[j].rfQualityQuant);
        }

        //printf("ssid %s \n",buf);

        ituScrollTextSetString(scrolltext, (char *)buf);
        ituWidgetSetCustomData(scrolltext, j);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText *scrolltext = (ITUScrollText *) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        listbox->itemCount = i % count;
        if (listbox->itemCount == 0)
            listbox->itemCount = count;
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingWiFiSsidStatusScrollListBoxOnLoad(ITUWidget *widget, char *param)
{
    ITUListBox          *listbox = (ITUListBox *) widget;
    ITUScrollListBox    *slistbox = (ITUScrollListBox *) listbox;
    ITCTree             *node;
    int                 i, j, count;
    int                 nRet;
    int                 nConnect    = 0;
    int                 nWiFiConnState = 0, nWiFiConnEcode = 0;

    assert(listbox);

    count               = ituScrollListBoxGetItemCount(slistbox);
    node                = ituScrollListBoxGetLastPageItem(slistbox);

    listbox->pageCount  = getMaxSsidCount() ? (getMaxSsidCount() + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex  = 1;
        listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText *scrolltext = (ITUScrollText *) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i   = 0;
    j   = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

#ifdef CFG_NET_WIFI
    nRet     = WifiMgr_Get_Connect_State(&nWiFiConnState, &nWiFiConnEcode);
    nConnect = WifiMgr_Get_Sta_Connect_Complete();
    printf("====>WifiMgr_Get_Connect_State(%d), nWiFiConnState(%d), nWiFiConnState(%d), nConnect(%d)\n",
        nRet, nWiFiConnState, nWiFiConnEcode, nConnect);
#endif
    printf("[%s] Connect Ecode(%d),   Max Ssid Count(%d),   WIFI is %s\n",
        __FUNCTION__, nWiFiConnEcode, getMaxSsidCount(), theConfig.wifi_on_off ? "ON":"OFF");

    for (; j < getMaxSsidCount(); j++)
    {
        ITUScrollText *scrolltext = (ITUScrollText *) node;

        if (!strncmp(theConfig.ssid, pList[j].ssidName, strlen(pList[j].ssidName)))
        {
            if ((nWiFiConnEcode == WIFIMGR_ECODE_OK) && nConnect)
            {
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
                if (settingWiFiReturnLayer){
                    ituScrollTextSetString(scrolltext, "");
                    return true;
                }
#endif
                ituScrollTextSetString(scrolltext, (char *)StringGetWiFiConnected());
                printf("SsidStatus %s \n", theConfig.ssid);
                nConnect++;
            }
            else
            {
                ituScrollTextSetString(scrolltext, "");
            }
        }
        else
        {
            ituScrollTextSetString(scrolltext, "");
        }

        ituWidgetSetCustomData(scrolltext, j);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText *scrolltext = (ITUScrollText *) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        listbox->itemCount = i % count;
        if (listbox->itemCount == 0)
            listbox->itemCount = count;
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingWiFiSsidSignalScrollListBoxOnLoad(ITUWidget *widget, char *param)
{
    ITUListBox          *listbox = (ITUListBox *) widget;
    ITUScrollListBox    *slistbox = (ITUScrollListBox *) listbox;
    ITCTree             *node;
    int                 i, j, count;
    assert(listbox);

    count               = ituScrollListBoxGetItemCount(slistbox);
    node                = ituScrollListBoxGetLastPageItem(slistbox);

    listbox->pageCount  = getMaxSsidCount() ? (getMaxSsidCount() + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex  = 1;
        listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText *scrolltext = (ITUScrollText *) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i   = 0;
    j   = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < getMaxSsidCount(); j++)
    {
        ITUScrollText   *scrolltext = (ITUScrollText *) node;
        char            buf[8];

        sprintf(buf, "%d%%", pList[j].rfQualityQuant);

        ituScrollTextSetString(scrolltext, buf);

        ituWidgetSetCustomData(scrolltext, j);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText *scrolltext = (ITUScrollText *) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        listbox->itemCount = i % count;
        if (listbox->itemCount == 0)
            listbox->itemCount = count;
    }
    else
        listbox->itemCount = count;

    return true;
}

#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
extern int SettingWiFiPasswordSetData(char *ssid, unsigned long securityMode);
#else
extern int SettingWiFiPasswordSetData(char *ssid, int securityMode);
#endif
bool SettingWiFiSsidScrollListBoxOnSelect(ITUWidget *widget, char *param)
{
    ITUListBox              *listbox    = (ITUListBox *) widget;
    ITUScrollIconListBox    *silistbox  = (ITUScrollIconListBox *) listbox;
    ITUScrollText           *scrolltext = (ITUScrollText *)ituListBoxGetFocusItem(listbox);
    int                     nIndex;
    int                     nRet, nWiFiConnState = 0, nWiFiConnEcode = 0;

    if (scrolltext)
    {
        nIndex = (int)ituWidgetGetCustomData(scrolltext);
#ifdef CFG_NET_WIFI
        nRet = WifiMgr_Get_Connect_State(&nWiFiConnState, &nWiFiConnEcode);
#endif
        if (!strncmp(theConfig.ssid, pList[nIndex].ssidName, strlen(pList[nIndex].ssidName)) && (nWiFiConnEcode != WIFIMGR_ECODE_CONNECT_ERROR))
        {
            printf("[%s]\n", __FUNCTION__);
            //ituLayerGoto(settingWiFiNetworkLayer);
        }
        else
        {
            //printf("SettingWiFiSsidScrollListBoxOnSelect ssid %s %d %d \n",scrolltext->text.string,listbox->pageIndex,listbox->focusIndex);
            SettingWiFiPasswordSetData(pList[nIndex].ssidName, pList[nIndex].securityMode);
            ituLayerGoto(settingWiFiPasswordLayer);
        }
    }
    return true;
}

bool SettingWiFiSsidOnEnter(ITUWidget *widget, char *param)
{
    int nRet;
    int nWiFiConnState = 0, nWiFiConnEcode = 0;

#ifdef CFG_NET_WIFI
    nRet = WifiMgr_Get_Connect_State(&nWiFiConnState, &nWiFiConnEcode);

#ifdef CFG_NET_WIFI_USB
    /* For USB type WIFI(wifimgr) */
    if (nWiFiConnState == WIFIMGR_CONNSTATE_CONNECTING)
    {
        printf("SettingWiFiSsidOnEnter connecting ,wait ....................\n");
        usleep(500000);
    }
#endif

    if (theConfig.wifi_on_off == WIFIMGR_SWITCH_ON)
    {
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
        //Check wifimgr ecode,  (-1) means something happened that cause connection fail
        if (nWiFiConnEcode  == WIFIMGR_ECODE_OK
            || nWiFiConnEcode  == WIFIMGR_ECODE_SET_DISCONNECT
            || nWiFiConnEcode  == WIFIMGR_ECODE_CONNECT_ERROR
            || nWiFiConnEcode  == -1){
            //Check WIFI connection state from wifimgr
            if(nWiFiConnState == WIFIMGR_CONNSTATE_STOP){
                printf("[%s] press scan button, wait ....................\n", __FUNCTION__);
				settingWiFiReturnLayer = false;
                gnApCount = WifiMgr_Get_Scan_AP_Info(pList);
            }else if (nWiFiConnState == WIFIMGR_CONNSTATE_CONNECTING){
                printf("[%s] connecting ,wait ....................\n", __FUNCTION__);
                if (strcmp(pList[0].ssidName, "") == 0)
                    gnApCount = WifiMgr_Get_Scan_AP_Info(pList); //If device can not get DHCP before set power-off, get new list again while power-on

                settingWiFiReturnLayer = true;
                usleep(500*1000);
            }else if (nWiFiConnState == WIFIMGR_CONNSTATE_SCANNING){
                printf("[%s] scanning ,wait ....................\n", __FUNCTION__);
				settingWiFiReturnLayer = true;
                usleep(500*1000);
            }
        }else{
            printf("[%s] WifiMgr is doing something(%d) ,wait ....................\n", __FUNCTION__, nWiFiConnEcode);
			if (nWiFiConnEcode == WIFIMGR_ECODE_DHCP_ERROR)
				settingWiFiReturnLayer = true;

            usleep(500*1000);
        }
#else
        gnApCount = WifiMgr_Get_Scan_AP_Info(pList);
#endif
    }
    else
    {
        gnApCount = 0;
        memset(pList, 0, 64*sizeof(WIFI_MGR_SCANAP_LIST));
        printf("Wifi is closed, no SSID list...\n");
    }
#endif
    if (!settingWiFiSsidNameScrollListBox)
    {
        settingWiFiSsidNameScrollListBox    = ituSceneFindWidget(&theScene, "settingWiFiSsidNameScrollListBox");
        assert( settingWiFiSsidNameScrollListBox);

        settingWiFiSsidStatusScrollListBox  = ituSceneFindWidget(&theScene, "settingWiFiSsidStatusScrollListBox");
        assert( settingWiFiSsidStatusScrollListBox);

        settingWiFiSsidSignalScrollListBox  = ituSceneFindWidget(&theScene, "settingWiFiSsidSignalScrollListBox");
        assert( settingWiFiSsidSignalScrollListBox);

        settingWiFiPasswordLayer            = ituSceneFindWidget(&theScene, "settingWiFiPasswordLayer");
        assert( settingWiFiPasswordLayer);

        settingWiFiNetworkLayer             = ituSceneFindWidget(&theScene, "settingWiFiNetworkLayer");
        assert( settingWiFiNetworkLayer);
    }

    return true;
}

bool SettingWiFiSsidOnLeave(ITUWidget *widget, char *param)
{
    return true;
}

void SettingWiFiSsidReset(void)
{
    settingWiFiSsidNameScrollListBox = NULL;
}