﻿#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"

#define INI_FILENAME "ctrlboard.ini"

Config theConfig;
static dictionary* cfgIni;
static bool cfgIsSaving;
static int cfgSavingCount;
static pthread_mutex_t cfgMutex  = PTHREAD_MUTEX_INITIALIZER;

void ConfigInit(void)
{
    cfgIni = iniparser_load(CFG_PUBLIC_DRIVE ":/" INI_FILENAME);
    if (!cfgIni)
    {
        cfgIni = dictionary_new(0);
        assert(cfgIni);

        dictionary_set(cfgIni, "tcpip", NULL);
        dictionary_set(cfgIni, "ctrlboard", NULL);
    }

    // network
    theConfig.dhcp = iniparser_getboolean(cfgIni, "tcpip:dhcp", 1);
    strncpy(theConfig.ipaddr, iniparser_getstring(cfgIni, "tcpip:ipaddr", "192.168.1.1"), sizeof (theConfig.ipaddr) - 1);
    strncpy(theConfig.netmask, iniparser_getstring(cfgIni, "tcpip:netmask", "255.255.255.0"), sizeof (theConfig.netmask) - 1);
    strncpy(theConfig.gw, iniparser_getstring(cfgIni, "tcpip:gw", "192.168.1.254"), sizeof (theConfig.gw) - 1);
    strncpy(theConfig.dns, iniparser_getstring(cfgIni, "tcpip:dns", "192.168.1.254"), sizeof (theConfig.dns) - 1);

    // display
    theConfig.lang = iniparser_getint(cfgIni, "ctrlboard:lang", LANG_ENG);
    theConfig.brightness = iniparser_getint(cfgIni, "ctrlboard:brightness", 8);
    theConfig.screensaver_time = iniparser_getint(cfgIni, "ctrlboard:screensaver_time", 30);
    theConfig.screensaver_type = iniparser_getint(cfgIni, "ctrlboard:screensaver_type", SCREENSAVER_BLANK);
    theConfig.mainmenu_type = iniparser_getint(cfgIni, "ctrlboard:mainmenu_type", MAINMENU_COVERFLOW);

    // sound
    strcpy(theConfig.keysound, iniparser_getstring(cfgIni, "ctrlboard:keysound", "key1.wav"));
    theConfig.keylevel = iniparser_getint(cfgIni, "ctrlboard:keylevel", 80);
    theConfig.audiolevel = iniparser_getint(cfgIni, "ctrlboard:audiolevel", 80);

    // photo
    theConfig.photo_interval = iniparser_getint(cfgIni, "ctrlboard:photo_interval", 10);

	// login
    strncpy(theConfig.user_id, iniparser_getstring(cfgIni, "ctrlboard:user_id", "admin"), sizeof (theConfig.user_id) - 1);
    strncpy(theConfig.user_password, iniparser_getstring(cfgIni, "ctrlboard:user_password", "admin"), sizeof (theConfig.user_password) - 1);

    // SSID
    snprintf(theConfig.ssid, 64, iniparser_getstring(cfgIni, "wifi:ssid", ""));
    // Password
    snprintf(theConfig.password, 256, iniparser_getstring(cfgIni,  "wifi:password", ""));
    // Security mode
    snprintf(theConfig.secumode, 3, iniparser_getstring(cfgIni, "wifi:secumode", "NA"));

	// wifi mode, client:0  softAP:1
    theConfig.wifi_mode = iniparser_getint(cfgIni, "wifi:wifi_mode", WIFI_CLIENT);
    // wifi switch on/off
    theConfig.wifi_on_off = iniparser_getint(cfgIni, "wifi:wifi_on_off", 0);

	// HFS IP
	snprintf(theConfig.hfs, 16, iniparser_getstring(cfgIni, "wifi:hfs", ""));
	// HFS port
	snprintf(theConfig.port, 8, iniparser_getstring(cfgIni, "wifi:port", ""));

    // misc
    theConfig.touch_calibration = iniparser_getint(cfgIni, "ctrlboard:touch_calibration", 1);

	// alarm
	theConfig.alarm = iniparser_getint(cfgIni, "ctrlboard:alarm", 5000);

	strncpy(theConfig.version, iniparser_getstring(cfgIni, "ctrlboard:swversion", "0.0.0.0"), sizeof (theConfig.version) - 1);
	theConfig.is_first_boot = iniparser_getboolean(cfgIni, "ctrlboard:firstboot", 0);
    cfgSavingCount = 0;
}

void ConfigExit(void)
{
    iniparser_freedict(cfgIni);
    cfgIni = NULL;
}

static void ConfigSavePublic(void)
{
    FILE* f;
    char buf[8];

    // network
    iniparser_set(cfgIni, "tcpip:dhcp", theConfig.dhcp ? "y" : "n");
    iniparser_set(cfgIni, "tcpip:ipaddr", theConfig.ipaddr);
    iniparser_set(cfgIni, "tcpip:netmask", theConfig.netmask);
    iniparser_set(cfgIni, "tcpip:gw", theConfig.gw);
    iniparser_set(cfgIni, "tcpip:dns", theConfig.dns);

    // display
    sprintf(buf, "%d", theConfig.lang);
    iniparser_set(cfgIni, "ctrlboard:lang", buf);

    sprintf(buf, "%d", theConfig.brightness);
    iniparser_set(cfgIni, "ctrlboard:brightness", buf);
    sprintf(buf, "%d", theConfig.screensaver_time);
    iniparser_set(cfgIni, "ctrlboard:screensaver_time", buf);
    sprintf(buf, "%d", theConfig.screensaver_type);
    iniparser_set(cfgIni, "ctrlboard:screensaver_type", buf);

    sprintf(buf, "%d", theConfig.mainmenu_type);
    iniparser_set(cfgIni, "ctrlboard:mainmenu_type", buf);

    // sound
    iniparser_set(cfgIni, "ctrlboard:keysound", theConfig.keysound);

    sprintf(buf, "%d", theConfig.keylevel);
    iniparser_set(cfgIni, "ctrlboard:keylevel", buf);

    sprintf(buf, "%d", theConfig.audiolevel);
    iniparser_set(cfgIni, "ctrlboard:audiolevel", buf);

	// login
    iniparser_set(cfgIni, "ctrlboard:user_id", theConfig.user_id);
    iniparser_set(cfgIni, "ctrlboard:user_password", theConfig.user_password);

    // Wifi SSID
    iniparser_set(cfgIni, "wifi:ssid", theConfig.ssid);
    // Password
    iniparser_set(cfgIni, "wifi:password", theConfig.password);
    // Security mode
    iniparser_set(cfgIni, "wifi:secumode", theConfig.secumode);
    // wifi mode
    sprintf(buf, "%d", theConfig.wifi_mode);
    iniparser_set(cfgIni, "wifi:wifi_mode", buf);
	// wifi switch on/off
    sprintf(buf, "%d", theConfig.wifi_on_off);
    iniparser_set(cfgIni, "wifi:wifi_on_off", buf);
	// HFS IP
	iniparser_set(cfgIni, "wifi:hfs", theConfig.hfs);
	// HFS Port
	iniparser_set(cfgIni, "wifi:port", theConfig.port);

    // photo
    sprintf(buf, "%d", theConfig.photo_interval);
    iniparser_set(cfgIni, "ctrlboard:photo_interval", buf);

    // misc
    sprintf(buf, "%d", theConfig.touch_calibration);
    iniparser_set(cfgIni, "ctrlboard:touch_calibration", buf);

	// alram
	sprintf(buf, "%d", theConfig.alarm);
	iniparser_set(cfgIni, "ctrlboard:alarm", buf);

	iniparser_set(cfgIni, "ctrlboard:swversion", theConfig.version);
	iniparser_set(cfgIni, "ctrlboard:firstboot", theConfig.is_first_boot ? "y" : "n");

    // save to file
    f = fopen(CFG_PUBLIC_DRIVE ":/" INI_FILENAME, "wb");
	if (!f)
    {
	    printf("cannot open ini file: %s\n", CFG_PUBLIC_DRIVE ":/" INI_FILENAME);
        return;
    }

    iniparser_dump_ini(cfgIni, f);
    fclose(f);
}

static void* ConfigSaveTask(void* arg)
{
    char* filepath = CFG_PUBLIC_DRIVE ":/" INI_FILENAME;
    int savingCount;

    cfgIsSaving = true;

    do
    {
        savingCount = cfgSavingCount;

        pthread_mutex_lock(&cfgMutex);

        ConfigSavePublic();

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
        UpgradeSetFileCrc(filepath);
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
        BackupSave();
    #else
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
#endif

        pthread_mutex_unlock(&cfgMutex);

    } while (savingCount != cfgSavingCount);

    cfgIsSaving = false;

    return NULL;
}

void ConfigUpdateCrc(char* filepath)
{
#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    pthread_mutex_lock(&cfgMutex);

    if (filepath)
        UpgradeSetFileCrc(filepath);
    else
        UpgradeSetFileCrc(CFG_PUBLIC_DRIVE ":/" INI_FILENAME);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    pthread_mutex_unlock(&cfgMutex);
#endif // CFG_CHECK_FILES_CRC_ON_BOOTING
}

void ConfigSave(void)
{
    pthread_t task;
    pthread_attr_t attr;

    cfgSavingCount++;

    if (cfgIsSaving)
        return;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, ConfigSaveTask, NULL);
}
