#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "lwip/ip.h"
#include "ping.h"
#include "curl/curl.h"
#include "ite/itp.h"

//#define TEST_SW_HOTPLUG

#if defined(CFG_USBH_CD_CDCECM) || defined(CFG_USBH_CD_CDCECM_EX)
//#define TEST_HW_HOTPLUG   /* shoule stop network application before remove device */
#endif

#define DHCP_TIMWOUT   (60*1000*1000)

int dhcp_ready = 0;

#if defined(CFG_USB_ECM) || defined(CFG_USBH_CD_CDCECM)
#define ITP_DEVICE_ECM  ITP_DEVICE_ETHERNET
#else // defined(CFG_USB_ECM_EX) || defined(CFG_USBH_CD_CDCECM)
#define ITP_DEVICE_ECM  ITP_DEVICE_ECM_EX
#endif

static ITPEthernetSetting setting = { 0 };

static int NetworkInit(void)
{
    ITPEthernetInfo info = { 0 };
    uint32_t dhcp_timeout = DHCP_TIMWOUT;
    char ip[16] = { 0 };

    /* wait link up */
    while (!ioctl(ITP_DEVICE_ECM, ITP_IOCTL_IS_CONNECTED, NULL)) {
        sleep(1);
        putchar('.');
        fflush(stdout);
    }
    printf("Network connected.. \n");

    /* start dhcp to get ip */
    setting.index = 0;
    setting.dhcp = 1;
    ioctl(ITP_DEVICE_ECM, ITP_IOCTL_RESET, &setting);

    /* wait for get ip ready */
    printf("Wait DHCP ready...\n");
    while (!ioctl(ITP_DEVICE_ECM, ITP_IOCTL_IS_AVAIL, NULL)) {
        usleep(100 * 1000);
        dhcp_timeout -= 100;
        if (!(dhcp_timeout % 1000)) {
            putchar('*');
            fflush(stdout);
        }

        if (dhcp_timeout == 0) {
            printf("[ERR] dhcp timeout................\n");
            return -1;
        }
    }
    printf("Network DHCP ready!\n");

    dhcp_ready = 1;

    /* get IP address */
    info.index = 0;
    ioctl(ITP_DEVICE_ECM, ITP_IOCTL_GET_INFO, &info);
    ipaddr_ntoa_r((const ip_addr_t*)&info.address, ip, sizeof(ip));
    printf("\n\nGet IP: %s \n\n", ip);

    return 0;
}

#if defined(CFG_TEST_PING)

static void DoPing(void)
{
    //printf("\n\nping: 172.217.160,78 \n\n");
    //ping_set_target("172.217.160.78"); // google(172.217.160.78)
    printf("\n\nping: %s \n\n", CFG_PING_IP);
    ping_set_target(CFG_PING_IP);
    ping_init();
}

#endif // #if defined(CFG_TEST_PING)

#if defined(CFG_TEST_HTTP)

//#define HTTP_WRITE_FILE
#define LOCAL_FILE      "A:/test_020m.zip"

#define HTTP_URL	CFG_HTTP_URL

static uint32_t http_file_size;
static uint32_t http_down_size;

/* get length from header */
static size_t HttpGetHeader(void *buffer, size_t size, size_t nmemb, void *stream)
{
    int     length;
    char    *pch;

    (void *)stream;

    pch = strstr(buffer, "Content-Length:");
    if (pch) {
        sscanf(pch + 16, "%d", &length);
        if (length >= 0) {
            printf("\n\n========================================\n\n");
            printf("\n\nHttp download: content length = %d bytes \n\n", length);
            http_file_size = length;
        }
    }

    return (size * nmemb);
}

static size_t HttpWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    size_t s;
    static uint32_t cnt = 0;

    (void *)stream;
    (void *)buffer;

#if defined(HTTP_WRITE_FILE)
    s = fwrite(buffer, size, nmemb, stream);
#else
    s = size * nmemb;
#endif

    http_down_size += s;
    if (!(cnt++ % 100)) {
        putchar('#');
        fflush(stdout);
    }
    if (http_down_size == http_file_size)
        printf("\n\nHttp download finish!! \n\n");

    return s;
}

static void DoHttpDownload(void)
{
    CURL *curl;
    CURLcode res;
    uint32_t startTime, download_time, bit_rate;
    FILE *fd = NULL;

    printf("\n\nDo Http Download....\n\n");

    http_down_size = 0;
    http_file_size = 0;

#if defined(HTTP_WRITE_FILE)
    /* get a FILE * of the same file */
    fd = fopen(LOCAL_FILE, "wb");
#endif

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        printf("curl_easy_init() fail! \n");
        goto end;
    }
    curl_easy_setopt(curl, CURLOPT_URL, HTTP_URL);
    /* disable progress meter, set to 0L to enable and disable debug output */
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    /* get header */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HttpGetHeader);
    /* some servers don't like requests that are made without a user-agent
    field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    startTime = itpGetTickCount();
    res = curl_easy_perform(curl);
    if (res)
        printf("curl_easy_perform() err: %s \n", curl_easy_strerror(res));

    download_time = itpGetTickDuration(startTime); // ms
    bit_rate = (((double)(http_file_size * 8 )/ 1024) / download_time) * 1000;
    printf("Download time: %u ms (%d kbps)\n", download_time, bit_rate);
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();

end:
#if defined(HTTP_WRITE_FILE)
    fclose(fd); /* close the local file */
#endif
    return;
}

#endif

extern int dial(void);
extern int DoFtpUpload(void);

void* Test4G(void* arg)
{
    int rc;

#if defined(CFG_POWER_WAKEUP_DELAY) || defined(TEST_SW_HOTPLUG) || defined(TEST_HW_HOTPLUG)
start:
#endif

#if defined(CFG_USB_OPTION) || defined(CFG_USBH_CD_CDCACM)
    rc = dial();
    if (rc) {
        printf("Dial fail! \n");
        while (1);
    }
#endif

#if defined(CFG_USB_ECM) || defined(CFG_USBH_CD_CDCECM) || defined(CFG_USB_ECM_EX) || defined(CFG_USBH_CD_CDCECM_EX) 
    rc = NetworkInit();
    if (rc) {
        printf("NetworkInit() fail! \n");
        while (1);
    }

#if defined(TEST_HW_HOTPLUG)
    /* shoule stop network application before remove device */
    printf("/************************************************************/\n");
    printf("/**  Should stop network application before remove device. **/\n");
    printf("/************************************************************/\n");
    printf("\n\n please remove device.........\n\n");
    printf("\n\n sleep 8 second...\n\n");
    sleep(8);
    printf("\n\n go to start......\n\n");
    goto start;
#endif

#if defined(CFG_TEST_PING)
    DoPing();
#elif defined(CFG_TEST_HTTP)
    #if !defined(HTTP_WRITE_FILE) && !defined(CFG_POWER_WAKEUP_DELAY) && !defined(TEST_SW_HOTPLUG)
    while (1)
    #endif
        DoHttpDownload();

    #if defined(CFG_POWER_WAKEUP_DELAY)
    printf("wait 3 second....\n");
    sleep(3);
    printf("go to sleep... \n");
    ioctl(ITP_DEVICE_POWER, ITP_IOCTL_SLEEP, NULL);
    goto start;
    #endif

    #if defined(TEST_SW_HOTPLUG)
    sleep(1);
    printf("\nstart software unplug...\n");
    #if defined(CFG_USB_OPTION)
    ioctl(ITP_DEVICE_USBOPTION, ITP_IOCTL_DISABLE, NULL);
    #endif
    #if defined(CFG_USB_ECM) || defined(CFG_USB_ECM_EX)
    /* stop dhcp */
    setting.dhcp = 0;
    ioctl(ITP_DEVICE_ECM, ITP_IOCTL_RESET, &setting);
    /* bring ECM interface down */
    ioctl(ITP_DEVICE_ECM, ITP_IOCTL_NETIF_REMOVE, NULL);
    #endif
    /* stop USB */
    ioctl(ITP_DEVICE_USB, ITP_IOCTL_SLEEP, (void *)0/*USB0*/);
    printf("\nsoftware remove done! \n");
    printf("\n\n ===> please power done and power on 4G module!!! \n");
    sleep(10);
    printf("restart USB! \n");
    ioctl(ITP_DEVICE_USB, ITP_IOCTL_RESUME, (void *)0/*USB0*/);
    printf("application restarting.... \n");
    goto start;
    #endif
    
#elif defined(CFG_TEST_FTP_UPLOAD)
    while (1)
        DoFtpUpload();
#endif
#endif // #if defined(CFG_USB_ECM) || defined(CFG_USBH_CD_CDCECM) || defined(CFG_USB_ECM_EX) || defined(CFG_USBH_CD_CDCECM_EX)

    while (1)
        sleep(1);
}
