#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"
#include "HT_DispDdefine.h"
#include "Menu_Layer.h"


#define INI_FILENAME "ctrlboard.ini"

Config theConfig;
static dictionary* cfgIni;
static bool cfgIsSaving;
static int cfgSavingCount;
static pthread_mutex_t cfgMutex  = PTHREAD_MUTEX_INITIALIZER;



bool ConfigInit(void)
{
	theConfig.screensaver_type = SCREENSAVER_NONE;

}

void ConfigExit(void)
{

}



static void* XO_ConfigSaveTask(void* arg)
{
    int savingCount;
    cfgIsSaving = true;

    do
    {
        savingCount = cfgSavingCount;

        pthread_mutex_lock(&cfgMutex);

		XO_ConfigSavePublic();

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



void XO_ConfigSave(void)
{
    pthread_t task;
    pthread_attr_t attr;

    cfgSavingCount++;

    if (cfgIsSaving)
        return;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, XO_ConfigSaveTask, NULL);

}

