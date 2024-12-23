#include <sys/ioctl.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "curl/curl.h"
#include "bootloader.h"
#include "config.h"

#define DHCP_TIMEOUT_MSEC (60 * 1000) //60sec

static struct timeval tvStart = { 0, 0 }, tvEnd = { 0, 0 };
static bool networkIsReady, networkToReset;

#ifdef CFG_NET_WIFI
#include "wifiMgr.h"
#include "ite/itp.h"
#include <time.h>
#include <sys/ioctl.h>
#include <pthread.h>

static WIFI_MGR_SETTING gWifiSetting;
static int gInit = 0; // wifi init

static void* NetworkTask(void* arg)
{
	int nTemp;

	for (;;)
	{
		gettimeofday(&tvEnd, NULL);

		nTemp = itpTimevalDiff(&tvStart, &tvEnd);
		if (nTemp>5000 && gInit == 0){
			printf("[%s] Init wifimgr \n", __FUNCTION__);

#ifndef CFG_NET_WIFI_SDIO
			while (!ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_IS_DEVICE_READY, NULL)){
				sleep(1);
				printf("wait ITP_IOCTL_IS_DEVICE_READY \n");
			}
#endif

			WifiMgr_Sta_Switch(true);

			ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
			nTemp = WifiMgr_Init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);

			gInit = 1;
		}
		else if (gInit == 1){
			networkIsReady = WifiMgr_Sta_Is_Available(&nTemp);
			networkIsReady = (bool)nTemp;
#ifdef CFG_POWER_SLEEP
#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
			if (ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFI_SLEEP_STATUS, NULL) == sleep_to_wakeup){
				// ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFI_ADD_NETIF, NULL);
				ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL);

				ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_SLEEP, (void *)default_no_sleep_or_wakeup);
				ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
				nTemp = WifiMgr_Init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);
			}
#endif
#endif
		}

		if (networkToReset)
			networkToReset = false;
		sleep(1);
	}
	return NULL;
}

static int wifiCallbackFucntion(int nState)
{
	switch (nState)
	{
	case WIFIMGR_STATE_CALLBACK_CONNECTION_FINISH:
		printf("[Indoor]WifiCallback connection finish \n");
		break;

	case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_DISCONNECT_30S:
		printf("[Indoor]WifiCallback connection disconnect 30s \n");
		break;

	case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_RECONNECTION:
		printf("[Indoor]WifiCallback connection reconnection \n");
		break;

	case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_TEMP_DISCONNECT:
		printf("[Indoor]WifiCallback connection temp disconnect \n");
		break;

	case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_FAIL:
		printf("[Indoor]WifiCallback connecting fail, please check ssid,password,secmode \n");
		WifiMgr_Sta_Disconnect();
		break;

	default:
		printf("[Indoor]WifiCallback unknown %d state  \n", nState);
		break;
	}
}

void NetworkInit(void)
{
	pthread_t task;

	networkIsReady = false;
	networkToReset = false;

	snprintf(gWifiSetting.ssid, WIFI_SSID_MAXLEN, CFG_NET_WIFI_MP_SSID);
	snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN, CFG_NET_WIFI_MP_PASSWORD);
	snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN, CFG_NET_WIFI_MP_SECURITY);
	gWifiSetting.wifiCallback = wifiCallbackFucntion;
	memset(&gWifiSetting.setting, 0, sizeof (ITPEthernetSetting));

	gWifiSetting.setting.index = 0;

	// dhcp
	gWifiSetting.setting.dhcp = 1;

	// autoip
	gWifiSetting.setting.autoip = 0;

	gettimeofday(&tvStart, NULL);

	pthread_create(&task, NULL, NetworkTask, NULL);
}
#endif

static ITCArrayStream arrayStream;
extern char tftppara[128];


struct FtpBuf
{
    uint8_t* buf;
    uint32_t pos;
};

static size_t FtpWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpBuf* out = (struct FtpBuf*)stream;
    size_t s;

    //LOG_DBG "FtpWrite(0x%X,%d,%d,0x%X)\n", buffer, size, nmemb, stream LOG_END

    assert(out->buf);
    s = size * nmemb;
    memcpy(&out->buf[out->pos], buffer, s);
    out->pos += s;

	LOG_DBG "FtpWrite(size: %d, pos: %d)\n", (int)s, out->pos LOG_END
    return s;
}

ITCStream* OpenRecoveryPackage(void)
{
    CURL *curl;
    CURLcode res;
    struct FtpBuf ftpBuf;

	// init wifi device
#if defined (CFG_NET_WIFI)
#if defined(CFG_USB0_ENABLE) || defined(CFG_USB1_ENABLE)
	if (ioctl(ITP_DEVICE_USB, ITP_IOCTL_IS_AVAIL, NULL))
#endif
	{
		itpRegisterDevice(ITP_DEVICE_WIFI, &itpDeviceWifi);
		printf("====>itpInit itpRegisterDevice(ITP_DEVICE_WIFI)\n");
#if defined(CFG_NET_WIFI_USB)
		ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_INIT, NULL);
#endif
	}

#if defined(CFG_NET_WIFI_SDIO_NGPL) || defined(CFG_NET_WIFI_SDIO_NGPL_8723DS)
	{
		itpRegisterDevice(ITP_DEVICE_WIFI_NGPL, &itpDeviceWifiNgpl);
		printf("====>itpInit itpRegisterDevice(ITP_DEVICE_WIFI_NGPL)\n");
		ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_INIT, NULL);
	}
#endif
#endif

	//usleep(5 * 100 * 1000);

	NetworkInit();
	while (!networkIsReady)
		sleep(1);

	ftpBuf.buf = malloc(0x1000000);
	if (!ftpBuf.buf)
	{
		LOG_ERR "malloc fail.\n" LOG_END
		return NULL;
	}
    ftpBuf.pos = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl)
    {
        LOG_ERR "curl_easy_init() fail.\n" LOG_END
        goto error;
    }
#if defined (CFG_UPGRADE_RECOVERY_TFTP)
    curl_easy_setopt(curl, CURLOPT_URL, "tftp://" CFG_UPGRADE_RECOVERY_SERVER_ADDR "/" CFG_UPGRADE_FILENAME);
#elif defined (CFG_ENABLE_UART_CLI)
	printf("\ntftppara=%s\n", tftppara);
	curl_easy_setopt(curl, CURLOPT_URL, tftppara);
#else
    curl_easy_setopt(curl, CURLOPT_URL, "ftp://" CFG_UPGRADE_RECOVERY_SERVER_ADDR "/" CFG_UPGRADE_FILENAME);
#endif
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FtpWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpBuf);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
    res = curl_easy_perform(curl);
    /* always cleanup */
    curl_easy_cleanup(curl);
    if (CURLE_OK != res)
    {
        LOG_ERR "curl fail: %d\n", res LOG_END
        goto error;
    }
    curl_global_cleanup();
    itcArrayStreamOpen(&arrayStream, ftpBuf.buf, ftpBuf.pos);
    return &arrayStream.stream;

error:
    curl_global_cleanup();
    return NULL;
}