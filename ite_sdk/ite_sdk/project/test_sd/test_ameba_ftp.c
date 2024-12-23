#include <pthread.h>
#include <unistd.h>
#include "curl/curl.h"
#include "ite/ith.h"
#include "ite/itp.h"

#define FTP_URL  "ftp://192.168.1.100/a.zip"
#define FTP_ACCOUNT  "irene:irene"


static size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data)
{
    (void)ptr;
    (void)data;
    /* we are not interested in the headers itself,
    so we only return the size we would have saved ... */

    return (size_t)(size * nmemb);
}

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    (void)contents;
    (void)userp;

    return (size_t)(size * nmemb);
}


static int GetFileSize(char* ftpurl)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    double filesize = 0.0;
    uint32_t g_time = 0;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ftpurl);
    curl_easy_setopt(curl, CURLOPT_USERPWD, FTP_ACCOUNT);
    //curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, throw_away);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    /* some servers don't like requests that are made without a user-agent
    field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 15L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    ithTimerCtrlEnable(ITH_TIMER3, ITH_TIMER_UPCOUNT);
    ithTimerSetCounter(ITH_TIMER3, 0);
    ithTimerEnable(ITH_TIMER3);
#if defined(CFG_DBG_TRACE_ANALYZER) && defined(CFG_DBG_TRACE)
    uiTraceStart();
#endif
    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }
#if defined(CFG_DBG_TRACE_ANALYZER) && defined(CFG_DBG_TRACE)
    vTraceStop();
#endif
    g_time = ithTimerGetTime(ITH_TIMER3);
    ithTimerDisable(ITH_TIMER3);

    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize);
    if ((CURLE_OK == res) && (filesize > 0.0))
    {
        printf("filesize: %0.0f bytes\n", filesize);
    }
    else
    {
        printf("curl_easy_getinfo(CURLINFO_CONTENT_LENGTH_DOWNLOAD) fail: %d, filesize: %0.0f bytes\n", res, filesize);
        filesize = 0.0;
        goto end;
    }
    printf("\r\n file size: %0.0f, total time: %d ms\n", filesize, (g_time / 1000));
    printf(" ==> %f Mbps \n\n", ((double)((filesize * 8) / (1024 * 1024)) / g_time) * 1000000);

end:
    if (curl)
        curl_easy_cleanup(curl);

    return (int)filesize;
}

static void DoFtp(void)
{
    int filesize;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    printf("ftp url: %s\n", FTP_URL);

    filesize = GetFileSize(FTP_URL);
    if (filesize <= 0) {
        printf("GetFileSize() fail! \r\n");
        goto end;
    }


end:
    curl_global_cleanup();
}


#if defined(CFG_TEST_SD_AMEBA)
void* Ameba_Test(void* arg)
#else
void* TestFunc(void* arg)
#endif
{
    pthread_t task;
    pthread_attr_t attr;
    ITPEthernetSetting setting = {0};

    // init pal
    #if !defined(CFG_TEST_SD_AMEBA)
    itpInit();
    #endif

    setting.dhcp = 0;
    // ipaddr
    setting.ipaddr[0] = 192;
    setting.ipaddr[1] = 168;
    setting.ipaddr[2] = 1;
    setting.ipaddr[3] = 249;
    // netmask
    setting.netmask[0] = 255;
    setting.netmask[1] = 255;
    setting.netmask[2] = 255;
    setting.netmask[3] = 0;
    // gateway
    setting.gw[0] = 192;
    setting.gw[1] = 168;;
    setting.gw[2] = 1;
    setting.gw[3] = 254;

    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);

    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL)) {
        putchar('.');
        fflush(stdout);
        sleep(1);
    }

    printf("sleep 5 ....\n");
    sleep(5);
    printf("leave sleep \n");

    while (1)
    DoFtp();

    // do the test
    for (;;)
    {
        sleep(1);
    }
    
    return NULL;
}

