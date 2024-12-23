#include "test_config.h"

static WIFI_MGR_SCANAP_LIST pList[64];
#if test_scan_ap
void scan_ap_loop(void)
{
    int get_scan_count = 0;
	sleep(test_scan_interval);

	//ioctl(ITP_DEVICE_WIFI_NGPL, ITP_IOCTL_SCAN, NULL);
	get_scan_count = WifiMgr_Get_Scan_AP_Info(pList);
    printf("====>Scan count(%d)\n", get_scan_count);
};
#endif

static int link_loop_time = 0;
#if test_connect
static struct connect_info Connect_Info[2];
static void link_info_setting(void)
{
	snprintf(Connect_Info[0].ssid,      WIFI_SSID_MAXLEN,       SSID_1);
	snprintf(Connect_Info[0].password,  WIFI_PASSWORD_MAXLEN,   PW_1);
	snprintf(Connect_Info[0].secumode,  WIFI_SECUMODE_MAXLEN,   SEC_1);

	snprintf(Connect_Info[1].ssid,      WIFI_SSID_MAXLEN,       SSID_2);
	snprintf(Connect_Info[1].password,  WIFI_PASSWORD_MAXLEN,   PW_2);
	snprintf(Connect_Info[1].secumode,  WIFI_SECUMODE_MAXLEN,   SEC_2);
};

//wifi_connect_all_finish has reomved
void link_differnet_ap(void)
{
    link_info_setting();

	if(wifi_connect_all_finish){
		if (WifiMgr_Sta_Disconnect() == WIFIMGR_ECODE_OK)
		    wifi_connect_all_finish = 0;
	}

	usleep(1000*1000);

	++link_loop_time;
	printf("\n========>[%d times] Connect to [%s]\n", link_loop_time, Connect_Info[link_loop_time%2].ssid);
	WifiMgr_Sta_Connect(Connect_Info[link_loop_time%2].ssid, Connect_Info[link_loop_time%2].password, *Connect_Info[link_loop_time%2].secumode);

	sleep(test_connect_interval);
};
#endif

