/*********************************************************************************
  *Copyright(C),2015-2020,  www.tuya.comm
  *FileName: tuya_linux_wifi_demo.c
  *
  * File Description:
  * 1. WIFI operation API 
  *
  * Developer work：
  * 1. Connect to WIFI based on SSID and PASSWD.
  * 2. Grab the image and identify the QR code.
  * 3. Specific WIFI chip adaptation.
  *
**********************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <pthread.h>
#include "tuya_iot_config.h"
#include "tuya_hal_wifi.h"
#include "ite/itp.h"

#if defined(WIFI_GW) && (WIFI_GW==1)

#include "tuya_ipc_system_control_demo.h"

OPERATE_RET hwl_wf_get_ip(IN CONST WF_IF_E wf,OUT NW_IP_S *ip)
{
    if(NULL == ip)
    {
        return OPRT_INVALID_PARM;
    }

#ifdef CFG_NET_WIFI
    ITPWifiInfo wifiInfo;
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &wifiInfo);
    ithPrintf(">>>>>>>>>[tuya_hal_wifi_get_ip]=(%s, %s)\n", ipaddr_ntoa((const ip_addr_t*)&wifiInfo.address), ipaddr_ntoa((const ip_addr_t*)&wifiInfo.netmask)); 
    memcpy(ip->ip,ipaddr_ntoa((const ip_addr_t*)&wifiInfo.address), sizeof(ip->ip));
    memcpy(ip->mask,ipaddr_ntoa((const ip_addr_t*)&wifiInfo.netmask), sizeof(ip->mask));
#endif
    ithPrintf("WIFI[%d] Get IP:%s\r\n", wf, ip->ip);
    return OPRT_OK;
}

/*
wlp3s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.3.14  netmask 255.255.255.0  broadcast 192.168.3.255
        ether a4:02:b9:b1:99:50  txqueuelen 1000  (Ethernet)
。。。。。。
。。。。。。
*/
OPERATE_RET hwl_wf_get_mac(IN CONST WF_IF_E wf,INOUT NW_MAC_S *mac)
{
    if(NULL == mac)
    {
        return OPRT_INVALID_PARM;
    }

#ifdef CFG_NET_WIFI
        //wifiMgr_get_Mac_address(mac->mac);
        ITPWifiInfo wifiInfo;
        ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &wifiInfo);
        memcpy(mac->mac, &wifiInfo.hardwareAddress, sizeof(mac->mac));
        ithPrintf(">>>>>>>>>>%s\n", mac->mac);
#endif 

    ithPrintf("WIFI Get MAC %02X-%02X-%02X-%02X-%02X-%02X\r\n",
           mac->mac[0],mac->mac[1],mac->mac[2],mac->mac[3],mac->mac[4],mac->mac[5]);

    return OPRT_OK;
}

#endif

