#include <pthread.h>
#include <sys/ioctl.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "test_config.h"

/*=====================WIFI=====================*/
static WIFI_MGR_SETTING     gWifiSetting;
static unsigned int wifi_connect_all_finish = 0;

int CallbackFucntion(int nState)
{
    switch (nState)
    {
        case WIFIMGR_STATE_CALLBACK_CONNECTION_FINISH:
            printf("[Indoor]WifiCallback connection finish \n");
            wifi_connect_all_finish = 1;
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

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_CANCEL:
            printf("[Indoor]WifiCallback connecting end to sleep/cancel \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_SLEEP_SAVE_INFO:
            snprintf(gWifiSetting.ssid , WIFI_SSID_MAXLEN, SSID);
            snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN, PW);
            gWifiSetting.secumode = WifiMgr_Secu_ITE_To_RTL(SEC);
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_SLEEP_CLEAN_INFO:
            printf("[Indoor]WifiCallback clean connecting info \n");
            snprintf(gWifiSetting.ssid , WIFI_SSID_MAXLEN, "");
            snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN, "");
            gWifiSetting.secumode = 0;
        break;

        default:
            printf("[Indoor]WifiCallback unknown %d state  \n",nState);
        break;
    }
}


static void PreSettingWifi(void)
{
    memset(&gWifiSetting.setting, 0, sizeof (ITPEthernetSetting));

    gWifiSetting.setting.index  = 0;
    gWifiSetting.setting.dhcp   = 1;
    gWifiSetting.setting.autoip = 0;

    for (int i = 0; i < 4; i++){
        gWifiSetting.setting.ipaddr[i] = 0;
        gWifiSetting.setting.netmask[i]= 0;
        gWifiSetting.setting.gw[i]     = 0;
    }

#if defined(STA_MODE)
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL);
#elif defined(AP_MODE)
    strcpy(gWifiSetting.ssid, AP_SSID);
    strcpy(gWifiSetting.password, AP_PW);

    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFIAP_ENABLE, NULL);
#endif
}

void* TestAPFunc(void* arg)
{
	//Connect infomation: test_config.h(SSID/PW, and SEC no need)
	printf("====>NGPL TestAPFunc: WIFI_LINK_AP\n");
	WifiMgr_Sta_Connect(SSID, PW, SEC);

	while(!ioctl(ITP_DEVICE_WIFI_NGPL, ITP_IOCTL_IS_AVAIL, NULL)){
        usleep(100*1000);
	}

#if test_connect
    link_info_setting();
#endif

	for (;;)
	{
#if test_dns
        dns_resolve();

        sleep(2);
#endif

#if test_scan_ap
		scan_ap_loop();
#endif

#if test_connect
		link_differnet_ap();
#endif

#if(!(test_scan_ap || test_connect))
		sleep(1);
#endif
	}
	return NULL;
}

void* TestFunc(void* arg)
{
	pthread_t task_ap;
	pthread_attr_t attr_ap;

	// init pal
	printf("====>NGPL: init itp\n");
	itpInit();
	usleep(5*100*1000);

    gWifiSetting.wifiCallback = CallbackFucntion;
    PreSettingWifi();

    WifiMgr_Sta_Switch(WIFI_MODE);

    if (WIFI_MODE == 1){
        WifiMgr_HostAP_First_Start();
    }

    WifiMgr_Init(WIFI_MODE, 0, gWifiSetting);

#ifdef STA_MODE
	pthread_attr_init(&attr_ap);
	pthread_create(&task_ap, &attr_ap, TestAPFunc, NULL);

    while(!ioctl(ITP_DEVICE_WIFI_NGPL, ITP_IOCTL_IS_AVAIL, NULL)){
        usleep(200*1000);
    }

#if (CFG_NET_HTTP && test_iperf)
#if test_iperf_type
#ifdef CFG_NET_LWIP_2
	iperf_test_server(); //Warning!!Only support LWIP 2.1.2
#endif
#else
    iperf_test_client(); //Support LWIP 1.4.1/2.1.2
#endif
#endif

/* ping test */
#if test_ping
    sleep(5);

    ping_main();
#endif

#if tcp_server_test
    printf("==== Server socket ====\n");
    network_tcp_server_main();
#endif
#endif

	for (;;)
	{
#if tcp_client_test
        printf("==== Client socket ====\n");
		network_tcp_client_main();
		printf("====>socket connect/recv failed\n");
#endif

		sleep(1);
	}
}


