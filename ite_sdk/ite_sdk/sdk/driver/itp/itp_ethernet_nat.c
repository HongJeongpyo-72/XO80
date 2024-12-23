/*
 * Copyright (c) 2011 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * PAL Ethernet functions.
 *
 * @author Jim Tan
 * @version 1.0
 */
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/ip4_nat.h"
#include "netif/etharp.h"
#include "itp_cfg.h"
#include "ite/ite_mac.h"
#include "ite/itp.h"

#ifdef __OPENRTOS__
#include "openrtos/FreeRTOSConfig.h"
#define MAC_TASK_PRIORITY 5
#if !defined(CFG_NET_AMEBA_SDIO)
#define ITE_MAC
#endif
#endif // __OPENRTOS__

#define MAC_STACK_SIZE        20480

err_t ethernetif_init(struct netif *netif);
void  ethernetif_shutdown(struct netif *netif);
void  ethernetif_poll(struct netif *netif);

#if (0)
#undef CFG_NET_ETHERNET_MAC_ADDR_DEFAULT
#undef CFG_NET_NAT_LAN_IPADDR
#if (1)
// LOBBY
#define CFG_NET_NAT_LAN_IPADDR            "192.168.190.101"
#define CFG_NET_ETHERNET_MAC_ADDR_DEFAULT 0x02,0x30,0x14,0x30,0x14,0x01
#else
// INDOOR
#define CFG_NET_NAT_LAN_IPADDR            "192.168.190.201"
#define CFG_NET_ETHERNET_MAC_ADDR_DEFAULT 0x02,0x30,0x14,0x30,0x14,0x02
#endif
#endif

#ifdef CFG_NET_ETHERNET_MAC_ADDR_DEFAULT
static uint8_t macAddr[6] = { CFG_NET_ETHERNET_MAC_ADDR_DEFAULT };
#else
static uint8_t macAddr[6];
#endif

#ifdef CFG_GPIO_ETHERNET
static const uint8_t  ioConfig[] = { CFG_GPIO_ETHERNET };
#else
static const uint8_t  ioConfig[10];
#endif


//#if !defined(CFG_NET_ETHERNET_DHCP)
//#error NET_ETHERNET_DHCP must be enabled!
//#endif
static struct xnetif {
    struct netif    netif;
    struct dhcp     dhcp;
    int             useDhcp;
} xnetifs[2];
static struct xnetif* const wanXnetif=xnetifs+0;
static struct xnetif* const lanXnetif=xnetifs+1;

ip4_nat_entry_t natEntry;
#if LWIP_DHCP
static struct timeval tDhcpCoarse;
static struct timeval tDhcpFine;
#endif

static bool           ethInited;
static timer_t        ethTimer;
static bool           ethInPollingFunc;

#ifdef ITE_MAC

#ifdef CFG_NET_ETHERNET_MAC_ADDR_STORAGE

static const char *ethMacVerifyCode = CFG_NET_ETHERNET_MAC_ADDR_VERIFY_CODE;

int MacRead(uint8_t *macaddr)
{
    uint8_t  *buf   = NULL;
    uint32_t pos, blocksize = 0;
    int      ret, i;
    uint32_t offset = 0;

    LOG_INFO "Read MAC address from storage\n" LOG_END

#if defined(CFG_NET_ETHERNET_MAC_ADDR_NAND)
    int fd = open(":nand", O_RDWR, 0);
    LOG_DBG "nand fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_NOR)
    int fd = open(":nor", O_RDWR, 0);
    LOG_DBG "nor fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_SD0)
    int fd = open(":sd0", O_RDWR, 0);
    LOG_DBG "sd0 fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_SD1)
    int fd = open(":sd1", O_RDWR, 0);
    LOG_DBG "sd1 fd: 0x%X\n", fd LOG_END
#endif
    if (fd == -1)
    {
        LOG_ERR "open device error: %d\n", fd LOG_END
        ret = __LINE__;
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        LOG_ERR "get block size error\n" LOG_END
        ret = __LINE__;
        goto end;
    }
    LOG_DBG "blocksize=%d\n", blocksize LOG_END

    pos    = CFG_NET_ETHERNET_MAC_ADDR_POS / blocksize;

        #if defined(CFG_NET_ETHERNET_MAC_ADDR_NAND)
    offset = CFG_NET_ETHERNET_MAC_ADDR_POS % blocksize;
        #endif

    if (lseek(fd, pos, SEEK_SET) != pos)
    {
        LOG_ERR "seek to mac addr position %d(%d) error\n", CFG_NET_ETHERNET_MAC_ADDR_POS, pos LOG_END
        ret = __LINE__;
        goto end;
    }

    assert(blocksize >= 8);
    buf = (uint8_t *)malloc(blocksize);
    if (!buf)
    {
        ret = __LINE__;
        goto end;
    }

    ret = read(fd, buf, 1);
    if (ret != 1)
    {
        LOG_ERR "read mac address error: %d != 1\n", ret LOG_END
        ret = __LINE__;
        goto end;
    }

    // verify number
    if (buf[0 + offset] == ethMacVerifyCode[0] && buf[1 + offset] == ethMacVerifyCode[1])
    {
        memcpy(macaddr, &buf[2 + offset], 6);
        ret = 0;
    }
    else
    {
        LOG_WARN "mac address %02X-%02X-%02X-%02X-%02X-%02X verify code %02X%02X incorrect; expect %02X%02X\n",
        buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[0], buf[1], ethMacVerifyCode[0], ethMacVerifyCode[1]
        LOG_END
    }

end:
    if (fd != -1)
        close(fd);

    if (buf)
        free(buf);

    return ret;
}

void MacWrite(uint8_t *macaddr)
{
    uint8_t  *buf   = NULL;
    uint32_t pos, blocksize = 0;
    int      ret, i;
    uint32_t offset = 0;

    LOG_INFO "Write MAC address to storage...\n" LOG_END

#if defined(CFG_NET_ETHERNET_MAC_ADDR_NAND)
    int fd = open(":nand", O_RDWR, 0);
    LOG_DBG "nand fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_NOR)
    int fd = open(":nor", O_RDWR, 0);
    LOG_DBG "nor fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_SD0)
    int fd = open(":sd0", O_RDWR, 0);
    LOG_DBG "sd0 fd: 0x%X\n", fd LOG_END
#elif defined(CFG_NET_ETHERNET_MAC_ADDR_SD1)
    int fd = open(":sd1", O_RDWR, 0);
    LOG_DBG "sd1 fd: 0x%X\n", fd LOG_END
#endif
    if (fd == -1)
    {
        LOG_ERR "open device error: %d\n", fd LOG_END
        ret = __LINE__;
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        LOG_ERR "get block size error\n" LOG_END
        ret = __LINE__;
        goto end;
    }
    LOG_DBG "blocksize=%d\n", blocksize LOG_END

    pos    = CFG_NET_ETHERNET_MAC_ADDR_POS / blocksize;

        #if defined(CFG_NET_ETHERNET_MAC_ADDR_NAND)
    offset = CFG_NET_ETHERNET_MAC_ADDR_POS % blocksize;
        #endif

    if (lseek(fd, pos, SEEK_SET) != pos)
    {
        LOG_ERR "seek to mac addr position %d(%d) error\n", CFG_NET_ETHERNET_MAC_ADDR_POS, pos LOG_END
        ret = __LINE__;
        goto end;
    }

    assert(blocksize >= 8);
    buf = (uint8_t *)malloc(blocksize);
    if (!buf)
    {
        ret = __LINE__;
        goto end;
    }

        #if defined(CFG_NET_ETHERNET_MAC_ADDR_NAND)
    if (read(fd, buf, 1) != 1)
    {
        LOG_ERR "read storage error: %d != 1\n", ret LOG_END
        ret = __LINE__;
        goto end;
    }

    if (lseek(fd, pos, SEEK_SET) != pos)
    {
        LOG_ERR "seek to mac addr position %d(%d) error\n", CFG_NET_ETHERNET_MAC_ADDR_POS, pos LOG_END
        ret = __LINE__;
        goto end;
    }
        #endif

    buf[0 + offset] = ethMacVerifyCode[0];
    buf[1 + offset] = ethMacVerifyCode[1];
    for (i = 0; i < 6; i++)
    {
        buf[i + 2 + offset] = macaddr[i];
    }

    if (write(fd, buf, 1) != 1)
    {
        LOG_ERR "write mac addr fail\n" LOG_END
        goto end;
    }

end:
    if (fd != -1)
        close(fd);

    if (buf)
        free(buf);
}

#endif // CFG_NET_ETHERNET_MAC_ADDR_STORAGE



static ITE_MAC_CFG_T mac_cfg;

int MacInit(bool initPhy)
{
    int ret, i;

    mac_cfg.flags |= (ITH_COUNT_OF(ioConfig) == ITE_MAC_GRMII_PIN_CNT) ? ITE_MAC_RGMII : 0;
    mac_cfg.clk_inv       = CFG_NET_MAC_CLOCK_INVERSE;
    mac_cfg.clk_delay     = CFG_NET_MAC_CLOCK_DELAY;
    mac_cfg.phyAddr       = CFG_NET_ETHERNET_PHY_ADDR;
    mac_cfg.rxd_delay = CFG_NET_MAC_RXD_DELAY;
    mac_cfg.ioConfig      = ioConfig;
    mac_cfg.linkGpio      = CFG_GPIO_ETHERNET_LINK;
    mac_cfg.phy_link_change = itpPhyLinkChange;
    mac_cfg.linkGpio_isr  = itpPhylinkIsr;
    mac_cfg.phy_link_status = itpPhyLinkStatus;
    mac_cfg.phy_read_mode = itpPhyReadMode;

    ret                   = iteMacInitialize(&mac_cfg);
    if (ret)
        return ret;

    #ifdef CFG_NET_ETHERNET_MAC_ADDR_RANDOM
    LOG_DBG "Random generate default MAC address\n" LOG_END
    srand(ithTimerGetCounter(portTIMER));
    macAddr[0] = 0;
    for (i = 1; i < 6; i++)
        macAddr[i] = (rand() % 256);

    #endif // CFG_NET_ETHERNET_MAC_ADDR_RANDOM

    #ifdef CFG_NET_ETHERNET_MAC_ADDR_STORAGE
    // read mac address from storage
    if (MacRead(macAddr))
    {
        #ifdef CFG_NET_ETHERNET_MAC_ADDR_UPGRADE
        MacWrite(macAddr);
        #endif
    }

    #endif // CFG_NET_ETHERNET_MAC_ADDR_STORAGE

    LOG_INFO "MAC address: %02X-%02X-%02X-%02X-%02X-%02X\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5] LOG_END

    ret = iteMacSetMacAddr((uint8_t *)macAddr);
    if (ret)
        return ret;

    if (initPhy)
        PhyInit(ITE_ETH_REAL);

    return 0;
}

#endif // ITE_MAC

/*
#define XSTR(x) STR(x)
#define STR(x) #x

#pragma message "CFG_NET_NAT_WAN_DHCP: " XSTR(CFG_NET_NAT_WAN_DHCP)
#pragma message "CFG_NET_NAT_WAN_IPADDR: " XSTR(CFG_NET_NAT_WAN_IPADDR)
#pragma message "CFG_NET_NAT_WAN_NETMASK: " XSTR(CFG_NET_NAT_WAN_NETMASK)
#pragma message "CFG_NET_NAT_WAN_GATEWAY: " XSTR(CFG_NET_NAT_WAN_GATEWAY)

#pragma message "CFG_NET_NAT_LAN_DHCP: " XSTR(CFG_NET_NAT_LAN_DHCP)
#pragma message "CFG_NET_NAT_LAN_IPADDR: " XSTR(CFG_NET_NAT_LAN_IPADDR)
#pragma message "CFG_NET_NAT_LAN_NETMASK: " XSTR(CFG_NET_NAT_LAN_NETMASK)
#pragma message "CFG_NET_NAT_LAN_GATEWAY: " XSTR(CFG_NET_NAT_LAN_GATEWAY)
*/
// This function initializes all network interfaces
void itpEthernetLwipInit(void)
{
    ip_addr_t ipaddr, netmask, gw;

    memset(xnetifs, 0, sizeof(xnetifs));
    gettimeofday(&tDhcpCoarse, NULL);
    gettimeofday(&tDhcpFine, NULL);

    /*
     *
     * WAN/LAN 誰該設�???default??
     *
     */
    // WAN netif
#ifdef CFG_NET_NAT_WAN_DHCP
    ip_addr_set_zero(&ipaddr);
    ip_addr_set_zero(&netmask);
    ip_addr_set_zero(&gw);
    netif_set_default(netif_add(&wanXnetif->netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
    netif_set_link_up(&wanXnetif->netif);
    netif_set_up(&wanXnetif->netif);
    dhcp_set_struct(&wanXnetif->netif, &wanXnetif->dhcp);
    int err=dhcp_start(&wanXnetif->netif);   // dhcp->server_ip_addr,offered_ip_addr,offered_sn_mask,offered_gw_addr
ithPrintf("[X] dhcp=%d\n", err);
printf("[X] dhcp=%d\n", err);
    wanXnetif->useDhcp=1;
#else
    ipaddr_aton(CFG_NET_NAT_WAN_IPADDR,  &ipaddr);
    ipaddr_aton(CFG_NET_NAT_WAN_NETMASK, &netmask);
    ipaddr_aton(CFG_NET_NAT_WAN_GATEWAY, &gw);
    netif_add(&wanXnetif->netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
    netif_set_link_up(&wanXnetif->netif);
    netif_set_up(&wanXnetif->netif);
    wanXnetif->useDhcp=0;
#endif
#if ETHARP_SUPPORT_VLAN
    wanXnetif->netif.vids=PVID_WANPORT;
#endif
    printf("[i] wan is up: %d, netif=%08x\n", netif_is_link_up(&wanXnetif->netif), &wanXnetif->netif);

    // LAN netif
#ifdef CFG_NET_NAT_LAN_DHCP
    ip_addr_set_zero(&ipaddr);
    ip_addr_set_zero(&netmask);
    ip_addr_set_zero(&gw);
    netif_set_default(netif_add(&lanXnetif->netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
    netif_set_up(&lanXnetif->netif);
    dhcp_set_struct(&lanXnetif->netif, &lanXnetif->dhcp);
    dhcp_start(&lanXnetif->netif);   // dhcp->server_ip_addr,offered_ip_addr,offered_sn_mask,offered_gw_addr
    lanXnetif->useDhcp=1;
#else
    ipaddr_aton(CFG_NET_NAT_LAN_IPADDR,  &ipaddr);
    ipaddr_aton(CFG_NET_NAT_LAN_NETMASK, &netmask);
    ipaddr_aton(CFG_NET_NAT_LAN_GATEWAY, &gw);
    netif_add(&lanXnetif->netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
    netif_set_link_up(&lanXnetif->netif);
    netif_set_up(&lanXnetif->netif);
    lanXnetif->useDhcp=0;
#endif
#if ETHARP_SUPPORT_VLAN
    lanXnetif->netif.vids=PVID_LANPORTS;
#endif
    printf("[i] lan is up: %d, netif=%08X\n", netif_is_link_up(&lanXnetif->netif), &lanXnetif->netif);

//#   ifdef CFG_NET_ETHERNET_AUTOIP
//    autoip_set_struct(&lanXnetif->netif, &lanXnetif->autoip);
//    autoip_start(&lanXnetif->netif);
//    lanXnetif->useAutoip=1;
//#   else
//    lanXnetif->useAutoip=0;
//#   endif
#if 0 // setup in network_nat.c
    ip4_nat_init();

    natEntry.out_if=&wanXnetif->netif;
    ip_addr_copy(natEntry.dest_net, wanXnetif->netif.ip_addr);
    ip_addr_copy(natEntry.dest_netmask, wanXnetif->netif.netmask);
    natEntry.in_if=&lanXnetif->netif;
    ip_addr_copy(natEntry.source_net, lanXnetif->netif.ip_addr);
    ip_addr_copy(natEntry.source_netmask, lanXnetif->netif.netmask);
    ip4_nat_add(&natEntry);
#endif

    ethInited = true;
}

static void EthernetGetInfo(ITPEthernetInfo *info)
{
    struct netif *netif;

    if (ethInited == false)
    {
        LOG_ERR "ethernet is not init yet\n" LOG_END
    }

    if (info->index >= CFG_NET_ETHERNET_COUNT)
    {
        LOG_ERR "Out of ethernet: %d\n", info->index LOG_END
        return;
    }

    netif = &xnetifs[info->index].netif;

    if (netif->flags & NETIF_FLAG_LINK_UP)
        info->flags |= ITP_ETH_LINKUP;

    if (netif->ip_addr.addr)
        info->flags |= ITP_ETH_ACTIVE;

    info->address = netif->ip_addr.addr;
    info->netmask = netif->netmask.addr;
    sprintf(info->displayName, "Ethernet%d", info->index);
    memcpy((void *)info->hardwareAddress, (void *)macAddr, 6);
    sprintf(info->name, "eth%d", info->index);
}


static void EthernetPoll(void)
{
    int i;

    if (!ethInited)
        return;

    for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++) {
        struct xnetif *xnetif = &xnetifs[i];

        if (xnetif->useDhcp && !xnetif->netif.ip_addr.addr) {
            struct timeval t;
            gettimeofday(&t, NULL);
            if (itpTimevalDiff(&tDhcpFine, &t) >= DHCP_FINE_TIMER_MSECS) {
                dhcp_fine_tmr();
                tDhcpFine=t;
            }
            if (itpTimevalDiff(&tDhcpCoarse, &t) >= DHCP_COARSE_TIMER_MSECS) {
                dhcp_coarse_tmr();
                tDhcpCoarse=t;
            }
        }
    }

    ethernetif_poll(&wanXnetif->netif);
    ethernetif_poll(&lanXnetif->netif);
}

#if CFG_NET_ETHERNET_POLL_INTERVAL > 0

static void EthernetPollHandler(timer_t timerid, int arg)
{
    ethInPollingFunc = true;
    EthernetPoll();
    ethInPollingFunc = false;
}

#endif // CFG_NET_ETHERNET_POLL_INTERVAL > 0

static int EthernetInit(void)
{
    // create mac poll task
#ifdef ITE_MAC
    {
        pthread_t          task;
        pthread_attr_t     attr;
        struct sched_param param;
        int                res;

    #ifdef CFG_NET_MAC_INIT_ON_BOOTLOADER
        res = MacInit(false);
    #else
        res = MacInit(true);
    #endif
        if (res)
        {
            errno = (ITP_DEVICE_ETHERNET << ITP_DEVICE_ERRNO_BIT) | res;
            return -1;
        }

        pthread_attr_init(&attr);
        //pthread_attr_setstacksize(&attr, MAC_STACK_SIZE);
        param.sched_priority = MAC_TASK_PRIORITY;
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&task, &attr, iteMacThreadFunc, NULL);
    }
#endif // ITE_MAC

#if CFG_NET_ETHERNET_POLL_INTERVAL > 0
    {
        struct itimerspec value;
        timer_create(CLOCK_REALTIME, NULL, &ethTimer);
        timer_connect(ethTimer, (VOIDFUNCPTR)EthernetPollHandler, 0);
        value.it_value.tv_sec = value.it_interval.tv_sec  = 0;
        value.it_value.tv_nsec = value.it_interval.tv_nsec = CFG_NET_ETHERNET_POLL_INTERVAL * 1000000;
        timer_settime(ethTimer, 0, &value, NULL);
    }
#endif // CFG_NET_ETHERNET_POLL_INTERVAL > 0

    return 0;
}

static void EthernetExit(void)
{
    int i;

    timer_delete(ethTimer);
    while (ethInPollingFunc)
        usleep(1000);
    for (i = 0; i < CFG_NET_ETHERNET_COUNT; ++i)
    {
        struct netif *netif = &xnetifs[i].netif;
        netif_set_down(netif);
        netif_remove(netif);
    }
}

static void EthernetReset(ITPEthernetSetting *setting)
{
    struct xnetif *xnetif;
    ip_addr_t    ipaddr, netmask, gw;

    if (setting->index >= CFG_NET_ETHERNET_COUNT)
    {
        LOG_ERR "Out of ethernet: %d\n", setting->index LOG_END
        return;
    }

    xnetif = xnetifs+setting->index;

printf("[!] %s:dhcp=%d:%d\n", __FUNCTION__, setting->dhcp, xnetif->useDhcp);
    if (setting->dhcp) {
        if (!xnetif->useDhcp) {
            dhcp_set_struct(&xnetif->netif, &xnetif->dhcp);
            dhcp_start(&xnetif->netif);
            xnetif->useDhcp=true;
            gettimeofday(&tDhcpCoarse, NULL);
            gettimeofday(&tDhcpFine, NULL);
        }
    }
    else if (setting->autoip) {
        printf("[X] autoip: unsupported!\n");
    }
    else if (!setting->autoip){
        if (xnetif->useDhcp) {
            dhcp_stop(&xnetif->netif);
            xnetif->useDhcp=false;
        }
        // why not netif_set_down(&xnetif->netif);
        IP4_ADDR(&ipaddr,  setting->ipaddr[0],  setting->ipaddr[1],  setting->ipaddr[2],  setting->ipaddr[3]);
        IP4_ADDR(&netmask, setting->netmask[0], setting->netmask[1], setting->netmask[2], setting->netmask[3]);
        IP4_ADDR(&gw,      setting->gw[0],      setting->gw[1],      setting->gw[2],      setting->gw[3]);
        netif_set_addr(&xnetif->netif, &ipaddr, &netmask, &gw);
        netif_set_up(&xnetif->netif);
    }

    printf("[!] ip: %s\n", ipaddr_ntoa(&xnetif->netif.ip_addr));
    printf("[!] nm: %s\n", ipaddr_ntoa(&xnetif->netif.netmask));
    printf("[!] gw: %s\n", ipaddr_ntoa(&xnetif->netif.gw));
}

static void EthernetSendArpToDetect(void)
{
    struct xnetif *xnetif;
    ip_addr_t ipaddr;

	xnetif = &xnetifs[CFG_NET_ETHERNET_COUNT - 1];

	etharp_request(&xnetif->netif, &xnetif->netif->ip_addr);
}

static int EthernetIoctl(int file, unsigned long request, void *ptr, void *info)
{
    int i;
    int avail=1;

    switch (request)
    {
    case ITP_IOCTL_POLL:
        EthernetPoll();
        break;

#if !defined(WIN32) && defined(CFG_NET_ETHERNET_DETECT_IP)
	case ITP_IOCTL_ETHERNET_SEND_ARP:
		EthernetSendArpToDetect();
		break;

	case ITP_IOCTL_IP_DUPLICATE:
		return etharp_ip_conflict();
#endif

    case ITP_IOCTL_IS_AVAIL:
        for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++) {
          /*
            printf("[i] netif : %08X\n",    &xnetifs[i].netif);
            printf("[i]   .vids: 0x%04X\n", xnetifs[i].netif.vids);
            printf("[i]   .ip  : %s\n",     ipaddr_ntoa(&xnetifs[i].netif.ip_addr));
            printf("[i]   .nm  : %s\n",     ipaddr_ntoa(&xnetifs[i].netif.netmask));
            printf("[i]   .gw  : %s\n",     ipaddr_ntoa(&xnetifs[i].netif.gw));
            printf("[i]   .ln  : %s\n",     netif_is_link_up(&xnetifs[i].netif)?"UP":"DOWN");
          */
            ithPrintf("[i] netif: 0x%08X-0x%04X-%s\n",    &xnetifs[i].netif, xnetifs[i].netif.vids, ipaddr_ntoa(&xnetifs[i].netif.ip_addr));
            if (!xnetifs[i].netif.ip_addr.addr) {
                avail=0;
            }
        }
        if (!avail) return 0;
        {
          int static initted=0;

          if (!initted) {
            initted=1;

            ip4_nat_init();
            natEntry.out_if=&wanXnetif->netif;
            ip_addr_copy(natEntry.dest_net, wanXnetif->netif.ip_addr);
            ip_addr_copy(natEntry.dest_netmask, wanXnetif->netif.netmask);
            natEntry.in_if=&lanXnetif->netif;
            ip_addr_copy(natEntry.source_net, lanXnetif->netif.ip_addr);
            ip_addr_copy(natEntry.source_netmask, lanXnetif->netif.netmask);
            ip4_nat_add(&natEntry);

            natEntry.out_if=&lanXnetif->netif;
            ip_addr_copy(natEntry.dest_net, lanXnetif->netif.ip_addr);
            ip_addr_copy(natEntry.dest_netmask, lanXnetif->netif.netmask);
            natEntry.in_if=&wanXnetif->netif;
            ip_addr_copy(natEntry.source_net, wanXnetif->netif.ip_addr);
            ip_addr_copy(natEntry.source_netmask, wanXnetif->netif.netmask);
            ip4_nat_add(&natEntry);
            }
        }
        return 1;

    case ITP_IOCTL_IS_CONNECTED:
        for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++) {
            //printf("[!] ip: %s\n", ipaddr_ntoa(&xnetifs[i].netif.ip_addr));
            //printf("[!] nm: %s\n", ipaddr_ntoa(&xnetifs[i].netif.netmask));
            //printf("[!] gw: %s\n", ipaddr_ntoa(&xnetifs[i].netif.gw));
            if (!netif_is_link_up(&xnetifs[i].netif)) {
                return 0;
            }
        }
        return 1;

    case ITP_IOCTL_GET_INFO:
        EthernetGetInfo((ITPEthernetInfo *)ptr);
        break;

    case ITP_IOCTL_INIT:
        return EthernetInit();
        break;

    case ITP_IOCTL_EXIT:
        EthernetExit();
        break;
/*
    case ITP_IOCTL_RESET:
        EthernetReset((ITPEthernetSetting *)ptr);
        break;

    case ITP_IOCTL_RESET_DEFAULT:
        printf("itp ethernet set netif 0 default \n");
        netif_set_default(&xnetifs[0].netif);
        break;

    case ITP_IOCTL_ENABLE:
        for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++) {
            netif_set_link_up(&xnetifs[i].netif);
        }
        break;

    case ITP_IOCTL_DISABLE:
        for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++) {
            netif_set_link_down(&xnetifs[i].netif);
        }
        break;
*/
#if defined(NET_RTL8201FL)
    case ITP_IOCTL_ON:
    #if defined(ITE_MAC)
        if (((int)ptr) == ITP_PHY_WOL)
            PhyWolEnter();
    #endif
        break;

    case ITP_IOCTL_OFF:
    #if defined(ITE_MAC)
        if (((int)ptr) == ITP_PHY_WOL)
            PhyWolExit();
    #endif
        break;
#endif

    case ITP_IOCTL_WIRTE_MAC:
#ifdef CFG_NET_ETHERNET_MAC_ADDR_STORAGE
        MacWrite((uint8_t *)ptr);
#endif
        break;

    default:
        errno = (ITP_DEVICE_ETHERNET << ITP_DEVICE_ERRNO_BIT) | 1;
        return -1;
    }
    return 0;
}

const ITPDevice itpDeviceEthernet =
{
    ":ethernet",
    itpOpenDefault,
    itpCloseDefault,
    itpReadDefault,
    itpWriteDefault,
    itpLseekDefault,
    EthernetIoctl,
    NULL
};
