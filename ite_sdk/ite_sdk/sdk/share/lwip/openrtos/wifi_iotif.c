#include <stdio.h>
#include <string.h>

#include "lwip/opt.h"
#include <stdint.h>
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include "lwip/ethip6.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/netif.h"

#include "wifi_iotif.h"

#include <openrtos/task.h>
//#include "wifi_host_api.h"
//#include "wifi_host_private_api.h"

#define IFNAME00 's'
#define IFNAME01 't'
#define IFNAME10 'a'
#define IFNAME11 'p'
#define IFNAME20 'l'
#define IFNAME21 'o'
/* WLAN Netif name */
#define WIFNAME00 'w'
#define WIFNAME01 '0'
/* Slave to Host */
#define MT7682_S2H_HEADER_LEN  12


struct netif sta_if, ap_if;

#if LWIP_IGMP
static err_t lwip_igmp_mac_filter( struct netif *netif, ip_addr_t *group, u8_t action );
#endif
static int32_t mt7682_low_level_output(struct netif *netif, struct pbuf *p);
static void mt7682_low_level_init1(struct netif *netif);
static void mt7682_low_level_init2(struct netif *netif);
//static err_t mt7682_netif_init1(struct netif *netif);
static err_t mt7682_netif_init2(struct netif *netif);
static void mt7682_low_level_free(struct pbuf *p);

/*static void
mt7682_ethernetif_init(ip4_addr_t *sta_ip_addr, ip4_addr_t *sta_net_mask, ip4_addr_t *sta_gw,
                             ip4_addr_t *ap_ip_addr, ip4_addr_t *ap_net_mask, ip4_addr_t *ap_gw,
                             uint8_t opmode);*/
void low_level_set_mac_addr(struct netif *netif, uint8_t *mac_addr);

uint16_t wfcm_to_slave(uint16_t op, uint8_t *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR);

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

/*
 * Compact form for string representation of MAC address
 * To be used, e.g., for constructing dbus paths for P2P Devices
 */
#define COMPACT_MACSTR "%02x%02x%02x%02x%02x%02x"
#endif

//#define DUMP_HEX_DATA
#ifdef DUMP_HEX_DATA
#define DUMP_TX     0
#define DUMP_RX     0
#endif

#ifdef DUMP_HEX_DATA
static void dump_hex_data(void* ptr, int len, char* name);
static void dump_hex_data(void* ptr, int len, char* name)
{
    unsigned char *data = ((unsigned char*)ptr);
    int i;
    printf("[%s] \r\n[%04d]\n", name, len);

    for(i=0; i < len; i++)
    {
        if(!(i % 0x10))
            printf("\n");
        printf("%02x ", data[i]);
}
    printf("\n\n");
}

#endif


#if LWIP_IGMP
/**
 * Interface between LwIP IGMP MAC filter and MAC filter
 */
static err_t lwip_igmp_mac_filter( struct netif *netif, ip_addr_t *group, u8_t action )
{
#if 0 //To be impletement
    uint8 multicast_mac[6];

	//Multicast IP to MAC
	multicast_mac[0] = 0x01;
	multicast_mac[1] = 0x00;
	multicast_mac[2] = 0x5e;
	multicast_mac[3] = ((uint8*)group)[1] & 0x7F;
	multicast_mac[4] = ((uint8*)group)[2];
	multicast_mac[5] = ((uint8*)group)[3];

    switch ( action )
    {
        case IGMP_ADD_MAC_FILTER:
            if ( wlan_wifi_register_multicast_address( multicast_mac ) != 0 )
            {
                return ERR_VAL;
            }
            break;

        case IGMP_DEL_MAC_FILTER:
            if ( wlan_wifi_unregister_multicast_address( multicast_mac ) != 0 )
            {
                return ERR_VAL;
            }
            break;

        default:
            return ERR_VAL;
    }

    return ERR_OK;
#else
	return ERR_OK;
#endif
}
#endif

static void
mt7682_low_level_free(struct pbuf *p)
{
    pbuf_free(p);
    p = NULL;
    LINK_STATS_INC(link.drop);
}

static void
mt7682_low_level_memset(struct pbuf *p)
{
    hal_sdio_slave_rxinfo_t *pbuf_s2h_pkt;
    uint8_t  *pbuf_s;
    uint32_t  p_len;

    //dump_hex_data((void *)p, p->tot_len, "mt7682_low_level_memset 1");
    pbuf_s       = (uint8_t*)p->payload;
    pbuf_s2h_pkt = (hal_sdio_slave_rxinfo_t *)pbuf_s;

    p_len = pbuf_s2h_pkt->len;
    //printf("[low_level_memset] p_len: %d\n", (int)p_len);
    memcpy(p->payload, pbuf_s+12, p_len-12);
    //dump_hex_data((void *)p, p_len-12 +16, "mt7682_low_level_memset 2");
}


void
mt7682_low_level_input(struct netif *netif, struct pbuf *p)
{
    struct eth_hdr *ethhdr=NULL;
    hal_sdio_slave_rxinfo_t *pbuf_s2h_pkt;
    uint8_t        *pbuf_s;
    uint32_t        pbufSize;

    if (p == NULL) return;

    /* Check MT7682 SDIO Rx header */
    pbuf_s  = (uint8_t*)p->payload;
    pbuf_s2h_pkt = (hal_sdio_slave_rxinfo_t *)pbuf_s;
    pbufSize     = pbuf_s2h_pkt->len;

    memcpy(p->payload, pbuf_s + MT7682_S2H_HEADER_LEN, pbufSize - MT7682_S2H_HEADER_LEN);

    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = (struct eth_hdr*)(p->payload);
#if DUMP_RX
    dump_hex_data((void *)ethhdr, pbufSize - MT7682_S2H_HEADER_LEN, "mt7682_low_level_input payload");
#endif

    if(netif == NULL)
    {
        // Received a packet for a network interface is not initialised
        // Cannot do anything with packet - just drop it.
        mt7682_low_level_free(p);
        return;
    }

    LINK_STATS_INC(link.recv);

    switch (htons(ethhdr->type)) {
    /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
    /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
        /* full packet send to tcpip_thread to process */
        if (tcpip_input(p, netif)!=ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));

            pbuf_free(p);
            p = NULL;
        }

        break;

    default:

        LINK_STATS_INC(link.proterr);
        pbuf_free(p);
        p = NULL;
        break;
    }
}

static int32_t
mt7682_low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint32_t total_len = 0;

    for(q = p; q != NULL; q = q->next) {
        total_len = total_len + (q->len);
    }

    printf("Host Lwip(%u) -> Slave\r\n", total_len);

#if DUMP_TX
    dump_hex_data((void *)p->payload, total_len, "mt7682_low_level_output");
#endif
    wfcm_to_slave(6, (void *)p, total_len, NULL, NULL, 0);  //WFC_CMD_DTW_DATA = 6

    return ERR_OK;
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
mt7682_low_level_init1(struct netif *netif)
{
  /*uint8_t netaddr[6];

  //As same as MAC of mt7682(7c:a7:b0:c6:20:1f)
  netaddr[0] = 0x7c;
  netaddr[1] = 0xa7;
  netaddr[2] = 0xb0;
  netaddr[3] = 0xc6;
  netaddr[4] = 0x20;
  netaddr[5] = 0x1f;

  memcpy(&netif->hwaddr, netaddr, ETHARP_HWADDR_LEN);*/

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

#if LWIP_IGMP
  netif->flags |= NETIF_FLAG_IGMP;
  netif_set_igmp_mac_filter(netif, lwip_igmp_mac_filter);
#endif
  /* Do whatever else is needed to initialize interface. */
}

static void
mt7682_low_level_init2(struct netif *netif)
{

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
    netif_set_igmp_mac_filter(netif, lwip_igmp_mac_filter);
#endif
  /* Do whatever else is needed to initialize interface. */
}


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
mt7682_netif_init1(struct netif *netif)
{
  //struct ethernetif *ethernetif;
  static int ethernetif_index;
  uint8_t sta_mac_address[6] = {0x7c, 0xa7, 0xb0, 0xc6, 0x20, 0x1f};

  int local_index;
  SYS_ARCH_DECL_PROTECT(lev);
  SYS_ARCH_PROTECT(lev);
  local_index = ethernetif_index++;
  SYS_ARCH_UNPROTECT(lev);

  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_ARP
  printf("=> %s\n", __FUNCTION__);
#endif

  /*ethernetif = (struct ethernetif *)mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    printf("ethernetif_init: out of memory\n");
    return ERR_MEM;
  }

  printf("%s : malloc ok : %p\n", __FUNCTION__, ethernetif);*/
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = NULL;
  netif->name[0] = WIFNAME00;
  netif->name[1] = (char)(WIFNAME01 + local_index);
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  #if LWIP_ARP
  netif->output = etharp_output;
  #else /* LWIP_ARP */
  netif->output = NULL; /* not used for PPPoE */
  #endif /* LWIP_ARP */
  #if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
  #endif /* LWIP_IPV6 */
  netif->linkoutput = (netif_linkoutput_fn)mt7682_low_level_output;

  //ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  low_level_set_mac_addr(netif, sta_mac_address);
  printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(netif->hwaddr));

  /* initialize the hardware */
  mt7682_low_level_init1(netif);

#if 0
  /* Create a task that simulates an interrupt in a real system.  This will
  block waiting for packets, then send a message to the uIP task when data
  is available. */

  printf("Create RX task\n\r");
  xTaskCreate(InterruptSimulator, "RX", 400, (void *)netif, 3, NULL );
#endif
  return ERR_OK;
}

err_t
mt7682_netif_init2(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  //LOG_I(lwip, "=> %s\n", __FUNCTION__);

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

  //LOG_I(lwip, "%s : malloc ok : %p\n", __FUNCTION__, ethernetif);
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME10;
  netif->name[1] = IFNAME11;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  #if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
  #endif /* LWIP_IPV6 */
  netif->linkoutput = (netif_linkoutput_fn)mt7682_low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  mt7682_low_level_init2(netif);

#if 0
  /* Create a task that simulates an interrupt in a real system.  This will
  block waiting for packets, then send a message to the uIP task when data
  is available. */

  LOG_I(lwip, "Create RX task\n\r");
  xTaskCreate(InterruptSimulator, "RX", 400, (void *)netif, 3, NULL );
#endif
  return ERR_OK;
}

void low_level_set_mac_addr(struct netif *netif, uint8_t *mac_addr)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, mac_addr, ETHARP_HWADDR_LEN);
}

/*void lwip_tcpip_init(lwip_tcpip_config_t *tcpip_config, uint8_t opmode)
{
    mt7682_ethernetif_init(&tcpip_config->sta_addr,
                           &tcpip_config->sta_mask,
                           &tcpip_config->sta_gateway,
                           &tcpip_config->ap_addr,
                           &tcpip_config->ap_mask,
                           &tcpip_config->ap_gateway,
                           opmode);
}*/


int lwip_get_netif_name(netif_type_t netif_type, char *name)
{
    struct netif *netif = NULL;

    if (name == NULL) {
        return 0;
    }

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif_type == NETIF_TYPE_AP &&
                IFNAME10 == netif->name[0] &&
                IFNAME11 == netif->name[1]) {
            name[0] = IFNAME10;
            name[1] = IFNAME11;
            name[2] = '0' + netif->num;
            return 1;
        } else if (netif_type == NETIF_TYPE_STA &&
                   IFNAME00 == netif->name[0] &&
                   IFNAME01 == netif->name[1]) {
            name[0] = IFNAME00;
            name[1] = IFNAME01;
            name[2] = '0' + netif->num;
            return 1;
        } else if (netif_type == NETIF_TYPE_LOOPBACK &&
                   IFNAME20 == netif->name[0] &&
                   IFNAME21 == netif->name[1]) {
            name[0] = IFNAME20;
            name[1] = IFNAME21;
            name[2] = '0' + netif->num;
            return 1;
        }
    }

    return 0;
}


struct netif *netif_find_by_type(netif_type_t netif_type)
{
    char name[4] = {0};

    if (lwip_get_netif_name(netif_type, (char *)name) == 1) {
        return netif_find(name);
    } else {
        return NULL;
    }
}
