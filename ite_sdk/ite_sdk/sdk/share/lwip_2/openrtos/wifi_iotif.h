#ifndef __MT7682_NETIF_H
#define __MT7682_NETIF_H

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include <stdint.h>

typedef char            int8;   /**< 8-bit integer type */
typedef unsigned char   uint8;  /**< 8-bit unsigned integer type */
typedef short           int16;  /**< 16-bit integer type */
typedef unsigned short  uint16; /**< 16-bit unsigned integer type */
typedef int             int32;  /**< 32-bit integer type */
typedef unsigned int    uint32; /**< 32-bit unsigned integer type */
typedef unsigned int    uint;   /**< Unsigned integer type */

typedef enum {
    STA_IP_MODE_STATIC   = 0,
    STA_IP_MODE_DHCP     = 1
} sta_ip_mode_t;

typedef enum {
    NETIF_TYPE_LOOPBACK = 0,
    NETIF_TYPE_AP       = 1,
    NETIF_TYPE_STA      = 2
} netif_type_t;

#if !defined(CFG_NET_LWIP_2)
#define ip4_addr_t ip_addr_t
#endif

typedef struct {
    int32_t ip_mode;

    ip4_addr_t sta_addr;
    ip4_addr_t sta_mask;
    ip4_addr_t sta_gateway;

    ip4_addr_t ap_addr;
    ip4_addr_t ap_mask;
    ip4_addr_t ap_gateway;
} lwip_tcpip_config_t;

void  lwip_tcpip_init(lwip_tcpip_config_t *tcpip_config, uint8_t opmode);
int   lwip_get_netif_name(netif_type_t netif_type, char *name);

struct netif
     *netif_find_by_type(netif_type_t netif_type);

void  mt7682_low_level_input(struct netif *netif, struct pbuf *p);
err_t mt7682_netif_init1(struct netif *netif);

#endif /* __MT7682_NETIF_H */

