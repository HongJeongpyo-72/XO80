/*********************************************************************************
  *Copyright(C),2015-2020, www.tuya.comm
  *FileName:    tuya_ipc_wired_demo
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "tuya_hal_wired.h"


// 可能的网卡名称列表
const char* s_net_devices[] = {"eth0", "eth1", "enp0s3", "enp2s0", "em1", "em2"};


/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

static int check_net_dev_valid(const char* net_dev)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("check_net_dev_valid, socket create failed!\n");
        return -1;
    }

    struct ifreq ifr = {};
    strncpy(ifr.ifr_name, net_dev, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock,SIOCGIFHWADDR, &ifr) < 0) {
        //printf("check_net_dev_valid, ioctl error(%d)\n", errno);
        close(sock);
        return -1;
    }

    close(sock);
    return 0;
}

static const char* get_valid_net_device()
{
    static const char* s_valid_net_device = NULL;
    if (s_valid_net_device) {
        return s_valid_net_device;
    }

    int i = 0;
    for (i = 0; i < sizeof(s_net_devices) / sizeof(s_net_devices[0]); ++i) {
        int ret = check_net_dev_valid(s_net_devices[i]);
        if (ret == 0) {
            s_valid_net_device = s_net_devices[i];
            printf("get_valid_net_device() find valid net device(%s)\n", s_valid_net_device);
            return s_valid_net_device;
        }
    }

    printf("get_valid_net_device() no valid net device found!!!\n");
    s_valid_net_device = "";
    return s_valid_net_device;
}

// Obtain the ip address of the port
int tuya_hal_wired_get_ip(NW_IP_S *ip)
{
    int sock;
    char ipaddr[50];

    struct   sockaddr_in *sin;
    struct   ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         printf("socket create failse...GetLocalIp!\n");
         return -1;
    }

    const char* net_dev = get_valid_net_device();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, net_dev, sizeof(ifr.ifr_name) - 1);

    if( ioctl( sock, SIOCGIFADDR, &ifr) < 0 ) {
         printf("tuya_hal_wired_get_ip, ioctl error(%d)\n", errno);
         close(sock);
         return -1;
    }

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    strcpy(ip->ip,inet_ntoa(sin->sin_addr));
    close(sock);

    return 0;
}

// Get the connection status of the port
bool tuya_hal_wired_station_conn(void)
{
    int sock;
    struct   sockaddr_in *sin;
    struct   ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         printf("socket create failse...GetLocalIp!\n");
         return false;
    }

    const char* net_dev = get_valid_net_device();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, net_dev, sizeof(ifr.ifr_name) - 1);

    if(ioctl(sock,SIOCGIFFLAGS,&ifr) < 0) {
        printf("tuya_hal_wired_station_conn, ioctl error(%d)\n", errno);
        close(sock);
        return false;
    }
    close(sock);

    if(0 == (ifr.ifr_flags & IFF_UP)) { 
        return false;
    }  

    return true;
}

/* If the gateway is in wifi+wired mode, the user needs to implement the wifi connection function.
When the network is deployed, the app will pass the ssid and passwd of the specified route.*/
// SDK will be called automatically
int tuya_hal_wired_wifi_set_station_connect(const char *ssid, const char *passwd)
{
    return -1;
}

/* If the gateway is in wifi+wired mode, it returns true. At this time, 
the app will call “hwl_bnw_set_station_connect” to pass the ssid and passwd of the specified route.*/
bool tuya_hal_wired_wifi_need_cfg(void)
{
    return false;
}

/* If the gateway is in wifi+wired mode, the interface returns the actual connection signal 
value of wifi and router, which needs to be implemented by the user.*/
int tuya_hal_wired_wifi_station_get_conn_ap_rssi(int8_t *rssi)
{
    *rssi = 99;

    return 0;
}

// get MAC of device
int tuya_hal_wired_get_mac(NW_MAC_S *mac)
{
    int sock;
    struct   sockaddr_in *sin;
    struct   ifreq ifr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
         printf("socket create failse...GetLocalIp!\n");
         return -1;
    }

    const char* net_dev = get_valid_net_device();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, net_dev, sizeof(ifr.ifr_name) - 1);

    if(ioctl(sock,SIOCGIFHWADDR,&ifr) < 0) {
        printf("tuya_hal_wired_get_mac, ioctl error(%d)\n", errno);
        close(sock);
        return -1;
    }
    memcpy(mac->mac,ifr.ifr_hwaddr.sa_data,sizeof(mac->mac));

    printf("WIFI Get MAC %02X-%02X-%02X-%02X-%02X-%02X\r\n",
             mac->mac[0],mac->mac[1],mac->mac[2],mac->mac[3],mac->mac[4],mac->mac[5]);
    close(sock);

    return 0;
}

// No need to implement
int tuya_hal_wired_set_mac(const NW_MAC_S *mac)
{
    if(NULL == mac) {
        return -1;
    }
    printf("WIFI Set MAC\r\n");

    return 0;
}

int tuya_hal_wired_if_connect_internet(bool *status)
{
    *status = true;
    return 0;
}
