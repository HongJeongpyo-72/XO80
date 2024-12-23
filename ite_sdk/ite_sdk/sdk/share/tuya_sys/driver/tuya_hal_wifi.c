/***********************************************************
*  File: wifi_hwl.c
*  Author: nzy
*  Date: 20170914
***********************************************************/
#include "tuya_hal_wifi.h"

#include <string.h>
#include "ite/itp.h"
#include "uni_log.h"
#include "stdio.h"
#include <sys/socket.h>

#define LOGD(...) printf("[WIFI DEBUG]" __VA_ARGS__)
#define LOGN(...) printf("[WIFI NOTICE]" __VA_ARGS__)
#define LOGI(...) do{ \
                    printf("[WIFI INFO]" __VA_ARGS__); \
                    sleep(1); \
                    }while(0)
#define LOGE(...) printf("[WIFI ERROR]" __VA_ARGS__)



#define IP_LEN 16
#define MAX_AP_SEARCH 64

BYTE_T sniffer_channel;


unsigned char tuya_mac_addr[6] = {0};

char tuya_ip_addr[4]    = {192, 168, 66, 1};
char tuya_msk_addr[4]   = {255, 255, 255, 0};
char tuya_gw_addr[4]    = {192, 168, 66, 1};
char tuya_dhcp_start[4] = {192, 168, 66, 100};
char tuya_dhcp_end[4] = {192, 168, 66, 255};

unsigned char tuya_wifi_state = WSS_IDLE;


//rda59xx_sta_info r_sta_info;
/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

STATIC CHAR_T g_scan_flag = 0;
STATIC UCHAR_T g_scan_num = 0; 
STATIC AP_IF_S *g_ap_ary = NULL;

/***********************************************************
*  Function: wf_all_ap_scan
*  Input: none
*  Output: ap_ary->scan ap info array
*          num->scan ap nums
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_all_ap_scan(AP_IF_S **ap_ary, uint32_t *num)
{
    int cnt;

    printf("%s %d \n",__FUNCTION__, __LINE__);
#if 0
    WIFI_MGR_SCANAP_LIST pList[64];

    if(NULL == ap_ary || NULL == num) {
        return OPRT_INVALID_PARM;
    }

    g_ap_ary = *ap_ary = tuya_hal_internal_malloc(sizeof(AP_IF_S) * MAX_AP_SEARCH);
    memset(g_ap_ary,0 ,sizeof(AP_IF_S) * MAX_AP_SEARCH);

    g_scan_num = 0;
    g_scan_flag = 0;

#ifdef CFG_NET_WIFI
    g_scan_num = wifiMgr_get_scan_ap_info(pList);
    *num = g_scan_num;
    for(cnt=0; cnt<g_scan_num; cnt++)
    {
        memcpy(g_ap_ary[cnt].bssid,pList[cnt].apMacAddr,6);
        memcpy(g_ap_ary[cnt].ssid,pList[cnt].ssidName,33);
        g_ap_ary[cnt].channel = pList[cnt].channelId;
        g_ap_ary[cnt].rssi = pList[cnt].rfQualityRSSI;
        g_ap_ary[cnt].s_len = strlen(pList[cnt].ssidName);
    }
#endif

    LOGI("scan finish\n");
#endif
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_assign_ap_scan
*  Input: ssid->assign ap ssid
*  Output: ap->ap info
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_assign_ap_scan(const char *ssid, AP_IF_S **ap)
{
#if 0    
    int i;
    WIFI_MGR_SCANAP_LIST pList[30];

    printf("%s %d \n",__FUNCTION__, __LINE__);

    if(NULL == ap || NULL == ssid) {
        return OPRT_INVALID_PARM;
    }

    g_scan_num = 0;
    g_scan_flag = 0;

#ifdef CFG_NET_WIFI
    g_scan_num = wifiMgr_get_scan_ap_info(pList);
    for(i=0; i<g_scan_num; i++)
    {
        if(!strcmp(pList[i].ssidName, ssid))
        {
            g_ap_ary = *ap = tuya_hal_internal_malloc(sizeof(AP_IF_S));
            memset(g_ap_ary,0 ,sizeof(AP_IF_S)); 

            memcpy(g_ap_ary->bssid,pList[i].apMacAddr,6);
            memcpy(g_ap_ary->ssid,pList[i].ssidName,33);
            g_ap_ary->channel = pList[i].channelId;
            g_ap_ary->rssi = pList[i].rfQualityRSSI;
            g_ap_ary->s_len = strlen(pList[i].ssidName);
        }
    }
#endif
    LOGI("scan finish\n");
#endif
    return OPRT_OK;	
}

/***********************************************************
*  Function: wf_release_ap
*  Input: ap
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_release_ap(AP_IF_S *ap)
{
    printf("%s %d \n", __FUNCTION__, __LINE__);

    //if (NULL == ap)
    //{
    //    return OPRT_INVALID_PARM;
    //}
    //tuya_hal_internal_free((void *)ap);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_set_cur_channel
*  Input: chan
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
static BYTE_T g_chan = 0;
int tuya_hal_wifi_set_cur_channel(const uint8_t chan)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);

    if(g_chan == 0)
    {
        return OPRT_OK;
    }

	sniffer_channel = chan;
    LOGI("%s\n", __FUNCTION__);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_get_cur_channel
*  Input: none
*  Output: chan
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_get_cur_channel(uint8_t *chan)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);

    if(NULL == chan) {
        return OPRT_INVALID_PARM;
    }
    *chan = sniffer_channel;
    LOGI("%s\n", __FUNCTION__);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_sniffer_set
*  Input: en->TRUE/FALSE
*         cb->sniffer callback
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
static VOID (*frame_cb)(IN CONST BYTE_T *buf,IN CONST USHORT_T len) = NULL;

/***********************************************************
*  Function: wf_get_ip
*  Input: wf->WF_IF_E
*  Output: ip
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_get_ip(const WF_IF_E wf, NW_IP_S *ip)
{
    ithPrintf("############%s %d \n",__FUNCTION__, __LINE__);
    //if(NULL == ip) {
    //    return OPRT_INVALID_PARM;
    //}
#ifdef CFG_NET_WIFI
    ITPWifiInfo wifiInfo;
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &wifiInfo);
    ithPrintf(">>>>>>>>>[tuya_hal_wifi_get_ip]=(%s, %s)\n", ipaddr_ntoa((const ip_addr_t*)&wifiInfo.address), ipaddr_ntoa((const ip_addr_t*)&wifiInfo.netmask)); 
    memcpy(ip->ip,ipaddr_ntoa((const ip_addr_t*)&wifiInfo.address), sizeof(ip->ip));
    memcpy(ip->mask,ipaddr_ntoa((const ip_addr_t*)&wifiInfo.netmask), sizeof(ip->mask));
#endif
#if 0
    if (NetworkIsReady())
    {
        if (EthernetIsReady())
        {
            memcpy(ip->ip,theConfig.ethernet_ipaddr,sizeof(ip->ip));
            memcpy(ip->mask,theConfig.ethernet_netmask,sizeof(ip->mask));
            memcpy(ip->gw,theConfig.ethernet_gw,sizeof(ip->gw));
        }
        else
        {
            memcpy(ip->ip,theConfig.wifi_ipaddr,sizeof(ip->ip));
            memcpy(ip->mask,theConfig.wifi_netmask,sizeof(ip->mask));
            memcpy(ip->gw,theConfig.wifi_gw,sizeof(ip->gw));
        }
    }
    else
    {
        memcpy(ip->ip,"0.0.0.0",sizeof(ip->ip));
        memcpy(ip->mask,"0.0.0.0",sizeof(ip->mask));
        memcpy(ip->gw,"0.0.0.0",sizeof(ip->gw));
    }    
#endif
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_get_mac
*  Input: wf->WF_IF_E
*  Output: mac
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_get_mac(const WF_IF_E wf, NW_MAC_S *mac)
{
    int i;

    ithPrintf("############%s %d \n",__FUNCTION__, __LINE__);

    //if(NULL == mac) {
    //    return OPRT_INVALID_PARM;
    //}

#ifdef CFG_NET_WIFI
    //wifiMgr_get_Mac_address(mac->mac);
    ITPWifiInfo wifiInfo;
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &wifiInfo);
    ithPrintf("############%s %d \n",__FUNCTION__, __LINE__);
    mac->mac[0] = wifiInfo.hardwareAddress[0];
    mac->mac[1] = wifiInfo.hardwareAddress[1];
    mac->mac[2] = wifiInfo.hardwareAddress[2];
    mac->mac[3] = wifiInfo.hardwareAddress[3];
    mac->mac[4] = wifiInfo.hardwareAddress[4];
    mac->mac[5] = wifiInfo.hardwareAddress[5];
    //memcpy(mac->mac, &wifiInfo.hardwareAddress, sizeof(mac->mac));
    ithPrintf(">>>>>>>>>>%x, %x, %x\n", mac->mac[0], mac->mac[1], mac->mac[2]);
#endif 
    //LOGI("%s\n", __FUNCTION__);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_set_mac
*  Input: wf->WF_IF_E
*         mac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_set_mac(const WF_IF_E wf, const NW_MAC_S *maca)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
	#if 0
	char *mdata, mac[6], i;

	mdata = &(maca->mac[0]);

    if(!mac_is_valid(maca->mac)){
        LOGI("mac if valid\r\n");
        return -1;
    }

    memcpy(tuya_mac_addr, maca->mac, 6);

	//LOGI("%02x:%02x:%02x:%02x:%02x:%02x\r\n",maca->mac[0],maca->mac[1],maca->mac[2],maca->mac[3],maca->mac[4],maca->mac[5]);
    rda59xx_set_macaddr(tuya_mac_addr, 0);
	#endif
    LOGI("hwl_wf_set_mac\r\n");
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_wk_mode_set
*  Input: mode->WF_WK_MD_E
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
static WF_WK_MD_E g_mode = WWM_STATION;
int tuya_hal_wifi_set_work_mode(const WF_WK_MD_E mode)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_wk_mode_get
*  Input: none
*  Output: mode
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_get_work_mode(WF_WK_MD_E *mode)
{
    //printf("%s %d \n",__FUNCTION__, __LINE__);

    unsigned char m_ww_softap = WWM_STATION;    //��ʾ��ǰ�Ѿ�����������.������Ҫ������������.
    *mode = m_ww_softap;
    return OPRT_OK;
}

static BOOL_T sniffer_on = FALSE;
BOOL_T is_sniffer_on()
{
    LOGI("%s\n", __FUNCTION__);
    return sniffer_on;
}

static void sniffer_callback(unsigned char *buf, unsigned int len, void* userdata)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    //printf("get data:%d\n",len);
    /*int i = 0;
    for(i; i< len;i++)
    {
        printf("%2x ", buf[i]);
        if(!(i % 32))
        {
            printf("\n");
        }
    }
	printf("\n");*/

	if(buf)
	{
        frame_cb(buf ,len);
	}
	//printf("release data:%d\n",len);
}


int tuya_hal_wifi_sniffer_set(const bool en, const SNIFFER_CALLBACK cb)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    return OPRT_OK;
}


#if defined(ENABLE_AP_FAST_CONNECT) && (ENABLE_AP_FAST_CONNECT==1)
/***********************************************************
*  Function: hwl_wf_fast_station_connect
*  Input: none
*  Output: fast_ap_info
*  Return: OPERATE_RET
***********************************************************/
int hwl_wf_get_connected_ap_info(FAST_WF_CONNECTED_AP_INFO_S *fast_ap_info)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    return OPRT_OK;
}

/***********************************************************
*  Function: hwl_wf_fast_station_connect
*  Input: ssid passwd
*  Output: mode
*  Return: OPERATE_RET
***********************************************************/
int hwl_wf_fast_station_connect(FAST_WF_CONNECTED_AP_INFO_S *fast_ap_info)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    return OPRT_OK;

}
#endif
/***********************************************************
*  Function: wf_station_connect
*  Input: ssid
*         passwd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_station_connect(const char *ssid, const char *passwd)
{
    printf("%s %d Callback.\n",__FUNCTION__, __LINE__);
    #if 0
#ifdef CFG_NET_WIFI
    wifiMgr_clientMode_disconnect();
    sleep(1);
    WifiSetConnectStatus(WSS_CONNECTING);
    wifiMgr_clientMode_connect_ap(ssid, passwd, "");
#endif
    LOGI("%s \n", __FUNCTION__);
    #endif

	return OPRT_OK;
}

/***********************************************************
*  Function: wf_station_disconnect
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_station_disconnect(void)
{
// #ifdef CFG_NET_WIFI
//     wifiMgr_clientMode_disconnect();
// #endif
    LOGI("%s\n", __FUNCTION__);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_station_get_conn_ap_rssi
*  Input: none
*  Output: rssi
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_station_get_conn_ap_rssi(int8_t *rssi)
{
	*rssi = 100;
    LOGI("%s rssi:%d \n", __FUNCTION__,*rssi);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_station_stat_get
*  Input: none
*  Output: stat
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_station_get_status(WF_STATION_STAT_E *stat)
{
    if (NetworkIsReady())
    {
        *stat = 6;
    }
    else
    {
        *stat = 0;
    }
    
    // unsigned char n_ret = WifiGetConnectStatus();   
    // *stat = n_ret;
    
    // LOGI("%s stat:%d\n", __FUNCTION__,*stat);
    return OPRT_OK;
}

/***********************************************************
*  Function: wf_ap_start
*  Input: cfg
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_ap_start(const WF_AP_CFG_IF_S *cfg)
{
#if 0
	int ret;
	rda59xx_ap_info r_ap_info;
	char addrtemp[NSAPI_IPv4_SIZE];

	if(NULL == cfg) {
        return OPRT_INVALID_PARM;
    }

	LOGI("ssid=%s,passwd=%s,channel=%d,ssid_hidden=%d\r\n",(char *)cfg->ssid,(char *)cfg->passwd,cfg->chan,cfg->ssid_hidden);

    //Start AP
    memset((void*)&r_ap_info, 0, sizeof(rda59xx_ap_info));
	memcpy(r_ap_info.ssid, (char *)cfg->ssid, strlen((char *)cfg->ssid));
    memcpy(r_ap_info.pw, (char *)cfg->passwd, strlen((char *)cfg->passwd));
    r_ap_info.ssid[strlen((char *)cfg->ssid)] = r_ap_info.pw[strlen((char *)cfg->passwd)] = '\0';
	if (cfg->chan>0 && cfg->chan<14) {
        r_ap_info.channel = (unsigned char)cfg->chan;
    }
	else {
		r_ap_info.channel = 1;	//must set a default value
	}
    r_ap_info.hidden = cfg->ssid_hidden;
	r_ap_info.link_num = 1;	//tuya default request to set 1, if not set default connect 8

    memcpy(addrtemp, inet_ntoa(*(unsigned int *)tuya_ip_addr), NSAPI_IPv4_SIZE);
    r_ap_info.ip = inet_addr(addrtemp);
    memcpy(addrtemp, inet_ntoa(*(unsigned int *)tuya_msk_addr), NSAPI_IPv4_SIZE);
	r_ap_info.netmask = inet_addr(addrtemp);
    memcpy(addrtemp, inet_ntoa(*(unsigned int *)tuya_gw_addr), NSAPI_IPv4_SIZE);
    r_ap_info.gateway= inet_addr(addrtemp);
    memcpy(addrtemp, inet_ntoa(*(unsigned int *)tuya_dhcp_start), NSAPI_IPv4_SIZE);
    r_ap_info.dhcps= inet_addr(addrtemp);
    memcpy(addrtemp, inet_ntoa(*(unsigned int *)tuya_dhcp_end), NSAPI_IPv4_SIZE);
    r_ap_info.dhcpe= inet_addr(addrtemp);

	ret = mbed_lwip_startap_inf(&r_ap_info);
	#endif
    LOGI("%s\n", __FUNCTION__);
    return 0;
}

/***********************************************************
*  Function: wf_ap_stop
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_ap_stop(void)
{
#if 0
	int ret;
	ret = mbed_lwip_stopap(0);
	#endif
    LOGI("%s\n", __FUNCTION__);
    return 0;
}
static COUNTRY_CODE_E cur_country_code = COUNTRY_CODE_CN;
/***********************************************************
*  Function: hwl_wf_set_country_code
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_hal_wifi_set_country_code(const char *p_country_code)
{
	#if 0
    LOGD("#set country code:%s#", p_country_code);
    if (!strcmp(p_country_code, "JP")) {
        rda59xx_set_country_code(JP);
        cur_country_code = COUNTRY_CODE_JP;
    }else if (!strcmp(p_country_code, "US")) {
        rda59xx_set_country_code(NA);
        cur_country_code = COUNTRY_CODE_US;
    }else {
        rda59xx_set_country_code(EU);
        cur_country_code = COUNTRY_CODE_EU;
    }
#endif
    return OPRT_OK;
}
static UINT_T lp_rcnt = 0;

int tuya_hal_wifi_lowpower_disable(void)
{
	#if 0
    if(!tuya_get_lp_mode()) {
        //LOGE("it's in normal mode");
        return OPRT_COM_ERROR;
    }

    if(!lp_rcnt++) {
        //pmu_acquire_wakelock(PMU_DEV_USER_BASE); //acquire wakelock
    }
	#endif
    return OPRT_OK;
}

int tuya_hal_wifi_lowpower_enable(void)
{
	#if 0
    if(!tuya_get_lp_mode()) {
        //LOGE("it's in normal mode");
        return OPRT_COM_ERROR;
    }

    if(lp_rcnt > 0) {
        lp_rcnt--;
    }

    if(!lp_rcnt) {
        //pmu_release_wakelock(PMU_DEV_USER_BASE);
    }
#endif
    return OPRT_OK;
}

COUNTRY_CODE_E tuya_hal_wifi_get_cur_country_code(void)
{
    return cur_country_code;
}

bool tuya_hal_wifi_get_rf_cal_flag(void)
{
	#if 0
    int ret = 0;    
    unsigned int calflag = 0;
    ret = rda5981_flash_read_cal_flag(&calflag);
    if (ret != 0) {
        LOGD("read cal flag fail");
        return FALSE;  
    }     
    LOGD("read cal flag sucess is %d", calflag);
    if (calflag) {
        return TRUE;
    } else {
        return FALSE;
    }
	#endif
	return FALSE;
}

int tuya_hal_wifi_send_mgnt(const uint8_t *buf, const uint32_t len)
{
    return -1;
}

int tuya_hal_wifi_close_concurrent_ap(void)
{
	return 0;
}

int tuya_hal_wifi_station_get_ap_mac(NW_MAC_S *mac)
{
	return 0;
}

int tuya_hal_wifi_register_recv_mgnt_callback(bool enable, WIFI_REV_MGNT_CB recv_cb)
{
	return 0;
}
