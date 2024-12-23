#ifndef _IW_HANDLER_H
#define _IW_HANDLER_H

/************************** DOCUMENTATION **************************/
/*
 * Initial driver API (1996 -> onward) :
 * -----------------------------------
 * The initial API just sends the IOCTL request received from user space
 * to the driver (via the driver ioctl handler). The driver has to
 * handle all the rest...
 *
 * The initial API also defines a specific handler in struct net_device
 * to handle wireless statistics.
 *
 * The initial APIs served us well and has proven a reasonably good design.
 * However, there is a few shortcommings :
 *	o No events, everything is a request to the driver.
 *	o Large ioctl function in driver with gigantic switch statement
 *	  (i.e. spaghetti code).
 *	o Driver has to mess up with copy_to/from_user, and in many cases
 *	  does it unproperly. Common mistakes are :
 *		* buffer overflows (no checks or off by one checks)
 *		* call copy_to/from_user with irq disabled
 *	o The user space interface is tied to ioctl because of the use
 *	  copy_to/from_user.
 *
 * New driver API (2002 -> onward) :
 * -------------------------------
 * The new driver API is just a bunch of standard functions (handlers),
 * each handling a specific Wireless Extension. The driver just export
 * the list of handler it supports, and those will be called apropriately.
 *
 * I tried to keep the main advantage of the previous API (simplicity,
 * efficiency and light weight), and also I provide a good dose of backward
 * compatibility (most structures are the same, driver can use both API
 * simultaneously, ...).
 * Hopefully, I've also addressed the shortcomming of the initial API.
 *
 * The advantage of the new API are :
 *	o Handling of Extensions in driver broken in small contained functions
 *	o Tighter checks of ioctl before calling the driver
 *	o Flexible commit strategy (at least, the start of it)
 *	o Backward compatibility (can be mixed with old API)
 *	o Driver doesn't have to worry about memory and user-space issues
 * The last point is important for the following reasons :
 *	o You are now able to call the new driver API from any API you
 *		want (including from within other parts of the kernel).
 *	o Common mistakes are avoided (buffer overflow, user space copy
 *		with irq disabled and so on).
 *
 * The Drawback of the new API are :
 *	o bloat (especially kernel)
 *	o need to migrate existing drivers to new API
 * My initial testing shows that the new API adds around 3kB to the kernel
 * and save between 0 and 5kB from a typical driver.
 * Also, as all structures and data types are unchanged, the migration is
 * quite straightforward (but tedious).
 *
 * ---
 *
 * The new driver API is defined below in this file. User space should
 * not be aware of what's happening down there...
 *
 * A new kernel wrapper is in charge of validating the IOCTLs and calling
 * the appropriate driver handler. This is implemented in :
 *	# net/core/wireless.c
 *
 * The driver export the list of handlers in :
 *	# include/linux/netdevice.h (one place)
 *
 * The new driver API is available for WIRELESS_EXT >= 13.
 * Good luck with migration to the new API ;-)
 */

/* ---------------------- THE IMPLEMENTATION ---------------------- */
/*
 * Some of the choice I've made are pretty controversials. Defining an
 * API is very much weighting compromises. This goes into some of the
 * details and the thinking behind the implementation.
 *
 * Implementation goals :
 * --------------------
 * The implementation goals were as follow :
 *	o Obvious : you should not need a PhD to understand what's happening,
 *		the benefit is easier maintainance.
 *	o Flexible : it should accomodate a wide variety of driver
 *		implementations and be as flexible as the old API.
 *	o Lean : it should be efficient memory wise to minimise the impact
 *		on kernel footprint.
 *	o Transparent to user space : the large number of user space
 *		applications that use Wireless Extensions should not need
 *		any modifications.
 *
 * Array of functions versus Struct of functions
 * ---------------------------------------------
 * 1) Having an array of functions allow the kernel code to access the
 * handler in a single lookup, which is much more efficient (think hash
 * table here).
 * 2) The only drawback is that driver writer may put their handler in
 * the wrong slot. This is trivial to test (I set the frequency, the
 * bitrate changes). Once the handler is in the proper slot, it will be
 * there forever, because the array is only extended at the end.
 * 3) Backward/forward compatibility : adding new handler just require
 * extending the array, so you can put newer driver in older kernel
 * without having to patch the kernel code (and vice versa).
 *
 * All handler are of the same generic type
 * ----------------------------------------
 * That's a feature !!!
 * 1) Having a generic handler allow to have generic code, which is more
 * efficient. If each of the handler was individually typed I would need
 * to add a big switch in the kernel (== more bloat). This solution is
 * more scalable, adding new Wireless Extensions doesn't add new code.
 * 2) You can use the same handler in different slots of the array. For
 * hardware, it may be more efficient or logical to handle multiple
 * Wireless Extensions with a single function, and the API allow you to
 * do that. (An example would be a single record on the card to control
 * both bitrate and frequency, the handler would read the old record,
 * modify it according to info->cmd and rewrite it).
 *
 * Functions prototype uses union iwreq_data
 * -----------------------------------------
 * Some would have prefered functions defined this way :
 *	static int mydriver_ioctl_setrate(struct net_device *dev,
 *					  long rate, int auto)
 * 1) The kernel code doesn't "validate" the content of iwreq_data, and
 * can't do it (different hardware may have different notion of what a
 * valid frequency is), so we don't pretend that we do it.
 * 2) The above form is not extendable. If I want to add a flag (for
 * example to distinguish setting max rate and basic rate), I would
 * break the prototype. Using iwreq_data is more flexible.
 * 3) Also, the above form is not generic (see above).
 * 4) I don't expect driver developper using the wrong field of the
 * union (Doh !), so static typechecking doesn't add much value.
 * 5) Lastly, you can skip the union by doing :
 *	static int mydriver_ioctl_setrate(struct net_device *dev,
 *					  struct iw_request_info *info,
 *					  struct iw_param *rrq,
 *					  char *extra)
 * And then adding the handler in the array like this :
 *        (iw_handler) mydriver_ioctl_setrate,             // SIOCSIWRATE
 *
 * Using functions and not a registry
 * ----------------------------------
 * Another implementation option would have been for every instance to
 * define a registry (a struct containing all the Wireless Extensions)
 * and only have a function to commit the registry to the hardware.
 * 1) This approach can be emulated by the current code, but not
 * vice versa.
 * 2) Some drivers don't keep any configuration in the driver, for them
 * adding such a registry would be a significant bloat.
 * 3) The code to translate from Wireless Extension to native format is
 * needed anyway, so it would not reduce significantely the amount of code.
 * 4) The current approach only selectively translate Wireless Extensions
 * to native format and only selectively set, whereas the registry approach
 * would require to translate all WE and set all parameters for any single
 * change.
 * 5) For many Wireless Extensions, the GET operation return the current
 * dynamic value, not the value that was set.
 *
 * This header is <net/iw_handler.h>
 * ---------------------------------
 * 1) This header is kernel space only and should not be exported to
 * user space. Headers in "include/linux/" are exported, headers in
 * "include/net/" are not.
 *
 * Mixed 32/64 bit issues
 * ----------------------
 * The Wireless Extensions are designed to be 64 bit clean, by using only
 * datatypes with explicit storage size.
 * There are some issues related to kernel and user space using different
 * memory model, and in particular 64bit kernel with 32bit user space.
 * The problem is related to struct iw_point, that contains a pointer
 * that *may* need to be translated.
 * This is quite messy. The new API doesn't solve this problem (it can't),
 * but is a step in the right direction :
 * 1) Meta data about each ioctl is easily available, so we know what type
 * of translation is needed.
 * 2) The move of data between kernel and user space is only done in a single
 * place in the kernel, so adding specific hooks in there is possible.
 * 3) In the long term, it allows to move away from using ioctl as the
 * user space API.
 *
 * So many comments and so few code
 * --------------------------------
 * That's a feature. Comments won't bloat the resulting kernel binary.
 */

/***************************** INCLUDES *****************************/
#include "wireless.h"   		/* IOCTL user space API */
#include "ite_skbuf.h"
#include "netdev_features.h"

/***************************** VERSION *****************************/
/*
 * This constant is used to know which version of the driver API is
 * available. Hopefully, this will be pretty stable and no changes
 * will be needed...
 * I just plan to increment with each new version.
 */
#define IW_HANDLER_VERSION	8

/*
 * Changes :
 *
 * V2 to V3
 * --------
 *	- Move event definition in <linux/wireless.h>
 *	- Add Wireless Event support :
 *		o wireless_send_event() prototype
 *		o iwe_stream_add_event/point() inline functions
 * V3 to V4
 * --------
 *	- Reshuffle IW_HEADER_TYPE_XXX to map IW_PRIV_TYPE_XXX changes
 *
 * V4 to V5
 * --------
 *	- Add new spy support : struct iw_spy_data & prototypes
 */

/**************************** CONSTANTS ****************************/

/* Enable enhanced spy support. Disable to reduce footprint */
#define IW_WIRELESS_SPY
#define IW_WIRELESS_THRSPY

/* Special error message for the driver to indicate that we
 * should do a commit after return from the iw_handler */
#define EIWCOMMIT	EINPROGRESS

/* Flags available in struct iw_request_info */
#define IW_REQUEST_FLAG_COMPAT	0x0001	/* Compat ioctl call */

/* Type of headers we know about (basically union iwreq_data) */
#define IW_HEADER_TYPE_NULL	0	/* Not available */
#define IW_HEADER_TYPE_CHAR	2	/* char [IFNAMSIZ] */
#define IW_HEADER_TYPE_UINT	4	/* __u32 */
#define IW_HEADER_TYPE_FREQ	5	/* struct iw_freq */
#define IW_HEADER_TYPE_ADDR	6	/* struct sockaddr */
#define IW_HEADER_TYPE_POINT	8	/* struct iw_point */
#define IW_HEADER_TYPE_PARAM	9	/* struct iw_param */
#define IW_HEADER_TYPE_QUAL	10	/* struct iw_quality */

/* Handling flags */
/* Most are not implemented. I just use them as a reminder of some
 * cool features we might need one day ;-) */
#define IW_DESCR_FLAG_NONE	0x0000	/* Obvious */
/* Wrapper level flags */
#define IW_DESCR_FLAG_DUMP	0x0001	/* Not part of the dump command */
#define IW_DESCR_FLAG_EVENT	0x0002	/* Generate an event on SET */
#define IW_DESCR_FLAG_RESTRICT	0x0004	/* GET : request is ROOT only */
				/* SET : Omit payload from generated iwevent */
#define IW_DESCR_FLAG_NOMAX	0x0008	/* GET : no limit on request size */
/* Driver level flags */
#define IW_DESCR_FLAG_WAIT	0x0100	/* Wait for driver event */

/****************************** TYPES ******************************/

/* ----------------------- WIRELESS HANDLER ----------------------- */
/*
 * A wireless handler is just a standard function, that looks like the
 * ioctl handler.
 * We also define there how a handler list look like... As the Wireless
 * Extension space is quite dense, we use a simple array, which is faster
 * (that's the perfect hash table ;-).
 */

/*
 * Meta data about the request passed to the iw_handler.
 * Most handlers can safely ignore what's in there.
 * The 'cmd' field might come handy if you want to use the same handler
 * for multiple command...
 * This struct is also my long term insurance. I can add new fields here
 * without breaking the prototype of iw_handler...
 */
struct iw_request_info
{
	u16_t		cmd;		/* Wireless Extension command */
	u16_t		flags;		/* More to come ;-) */
};

#if 1 //eason moved from ite_ndis.h
typedef void (*input_fn)(void *arg, void *packet, int len);

struct net_device_config {
#define WLAN_MODE_OFF               0
#define WLAN_MODE_STA               1
#define WLAN_MODE_ADHOC             2
	int operationMode;	    /*Operation Type*/
    int channelId;          /*Channel ID*/

    char ssidName[32];      /*SSID name*/

    struct
    {
#define WLAN_SEC_NOSEC              0
#define WLAN_SEC_WEP                1
#define WLAN_SEC_WPAPSK             2  /** default is TKIP */
#define WLAN_SEC_WPA2PSK            3  /** default is AES */
#define WLAN_SEC_WPAPSK_AES         4
#define WLAN_SEC_WPA2PSK_TKIP       5
#define WLAN_SEC_WPAPSK_MIX         6
#define WLAN_SEC_WPS                7
#define WLAN_SEC_EAP                8

    	int securityMode;	    /*Sec. Mode*/

#define WLAN_AUTH_OPENSYSTEM        0
#define WLAN_AUTH_SHAREDKEY         1
#define WLAN_AUTH_AUTO              2
        int authMode;           /*Auth. Mode*/

        int         defaultKeyId;       /*WEP default KEYID, from 0 to 3*/
    	unsigned char		wepKeys[4][32];     /*WEP*/
        unsigned char       preShareKey[64];    /*WPAPSK/WPA2PSK*/

#define WLAN_WPS_PIN                0 /** not support */
#define WLAN_WPS_PBC                1
#define WLAN_WPS_PIN_START          2 /** not support */
#define WLAN_WPS_STOP               3
        int wpsMode;
    } securitySuit;

    void (*write_config_cb)(void* config);  /*for WPS write config to file*/

#define WLAN_FEA_ANYSSID    		(1<<0)  /*Any SSID*/
	unsigned int feaFlags;		    /*Feature flags*/

};

struct net_device_info {
#define WLAN_INFO_LINK      0
#define WLAN_INFO_AP        1
#define WLAN_INFO_SCAN      2
#define WLAN_INFO_SCAN_GET  3
#define WLAN_INFO_WIFI_STATE 4
#define WLAN_INFO_LINK_HIDDEN 5

    int infoType;

    /*Link Information*/
#define WLAN_LINK_OFF       0
#define WLAN_LINK_ON        1
    int linkInfo;

    /*Wifi driver current state*/
#define WLAN_NULL_STATE		0x00000000
#define	WLAN_ASOC_STATE		0x00000001		// Under Linked state...
#define WLAN_REASOC_STATE	0x00000002
#define	WLAN_SLEEP_STATE	0x00000004
#define	WLAN_STATION_STATE	0x00000008
#define	WLAN_AP_STATE	    0x00000010
#define	WLAN_ADHOC_STATE	0x00000020
#define WLAN_ADHOC_MASTER_STATE 0x00000040
#define WLAN_UNDER_LINKING		0x00000080
#define WLAN_UNDER_CMD			0x00000200
#define WLAN_SITE_MONITOR		0x00000800		//to indicate the station is under site surveying

    unsigned int driverState;

    unsigned char staMacAddr[6+2];
	unsigned int staMaxRate;
	unsigned int staCount;

	/*AP Information*/
    unsigned char apMacAddr[6+2];
    int channelId;
    unsigned char ssidName[32];
	unsigned int avgQuant;		//Percent : 0~100
	int avgRSSI;		//RSSI
	char hidden_ssid[32];
    bool connect_hidden_ssid_enable;

    /*Scan Information*/
    int apCnt;
    struct {
        unsigned char name[16];
        unsigned char apMacAddr[6+2];
        int channelId;
        unsigned char ssidName[32];

#define WLAN_SCAN_OPMODE_INFRA      0
#define WLAN_SCAN_OPMODE_ADHOC      1
#define WLAN_SCAN_OPMODE_UNKNOW     2
        int operationMode;

#define WLAN_SCAN_SECURITY_OFF      0
#define WLAN_SCAN_SECURITY_ON       1
        int securityOn;

        unsigned char rfQualityQuant;	//Percent : 0~100
        signed char  rfQualityRSSI;	//RSSI
        unsigned char reserved[2];
        int   bitrate;
        int   securityMode;	    /*Sec. Mode*/
    } apList[64 /*MAX_LEN_OF_BSS_TABLE*/];
};

struct net_device_stats
{
	unsigned long   rx_packets;             /* total packets received       */
	unsigned long   tx_packets;             /* total packets transmitted    */
	unsigned long   rx_bytes;               /* total bytes received         */
	unsigned long   tx_bytes;               /* total bytes transmitted      */
	unsigned long   rx_errors;              /* bad packets received         */
	unsigned long   tx_errors;              /* packet transmit problems     */
	unsigned long   rx_dropped;             /* no space in linux buffers    */
	unsigned long   tx_dropped;             /* no space available in linux  */
	unsigned long   multicast;              /* multicast packets received   */
	unsigned long   collisions;

	/* detailed rx_errors: */
	unsigned long   rx_length_errors;
	unsigned long   rx_over_errors;         /* receiver ring buff overflow  */
	unsigned long   rx_crc_errors;          /* recved pkt with crc error    */
	unsigned long   rx_frame_errors;        /* recv'd frame alignment error */
	unsigned long   rx_fifo_errors;         /* recv'r fifo overrun          */
	unsigned long   rx_missed_errors;       /* receiver missed packet       */

	/* detailed tx_errors */
	unsigned long   tx_aborted_errors;
	unsigned long   tx_carrier_errors;
	unsigned long   tx_fifo_errors;
	unsigned long   tx_heartbeat_errors;
	unsigned long   tx_window_errors;

	/* for cslip etc */
	unsigned long   rx_compressed;
	unsigned long   tx_compressed;
};

struct net_device
{
	char				name[IFNAMSIZ];
	unsigned long		state;
	unsigned short	    flags;  /* interface flags (a la BSD)   */
	unsigned short	    type;	/* interface hardware type	*/
	void				*priv;  /* pointer to private data      */
	unsigned char		dev_addr[MAX_ADDR_LEN]; /* hw address   */
	unsigned long		last_rx;	/* Time of last Rx	*/

	int				(*open)(struct net_device *dev);
	int				(*stop)(struct net_device *dev);
	int				(*hard_start_xmit) (struct sk_buff *skb, struct net_device *dev);
	int				(*set_mac_address) (struct net_device *dev, void *p);
	int				(*do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
	struct net_device_stats*	(*get_stats)(struct net_device *dev);

	struct iw_statistics*	(*get_wireless_stats)(struct net_device *dev);
    int                     (*wireless_control)(struct net_device *dev, struct net_device_config *netDeviceConfig);
    int                     (*wireless_information)(struct net_device *dev, struct net_device_info *netDeviceInfo);

	int watchdog_timeo;
	struct iw_handler_def*  wireless_handlers;
	const struct net_device_ops *netdev_ops;

#ifdef LWIP_INTERFACE
    void           *rxNetif;
    input_fn       rxCallBack;
#endif

    netdev_features_t	features;
};
#endif


/*
 * This is how a function handling a Wireless Extension should look
 * like (both get and set, standard and private).
 */
typedef int (*iw_handler)(struct net_device *dev, struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra);

/*
 * This define all the handler that the driver export.
 * As you need only one per driver type, please use a static const
 * shared by all driver instances... Same for the members...
 * This will be linked from net_device in <linux/netdevice.h>
 */
struct iw_handler_def
{
	/* Number of handlers defined (more precisely, index of the
	 * last defined handler + 1) */
	u16_t			num_standard;
	u16_t			num_private;
	/* Number of private arg description */
	u16_t			num_private_args;

	/* Array of handlers for standard ioctls
	 * We will call dev->wireless_handlers->standard[ioctl - SIOCSIWNAME]
	 */
	iw_handler *		standard;

	/* Array of handlers for private ioctls
	 * Will call dev->wireless_handlers->private[ioctl - SIOCIWFIRSTPRIV]
	 */
	iw_handler *		private;

	/* Arguments of private handler. This one is just a list, so you
	 * can put it in any order you want and should not leave holes...
	 * We will automatically export that to user space... */
	struct iw_priv_args *	private_args;

	/* New location of get_wireless_stats, to de-bloat struct net_device.
	 * The old pointer in struct net_device will be gradually phased
	 * out, and drivers are encouraged to use this one... */
	struct iw_statistics*	(*get_wireless_stats)(struct net_device *dev);
};

/* ---------------------- IOCTL DESCRIPTION ---------------------- */
/*
 * One of the main goal of the new interface is to deal entirely with
 * user space/kernel space memory move.
 * For that, we need to know :
 *	o if iwreq is a pointer or contain the full data
 *	o what is the size of the data to copy
 *
 * For private IOCTLs, we use the same rules as used by iwpriv and
 * defined in struct iw_priv_args.
 *
 * For standard IOCTLs, things are quite different and we need to
 * use the stuctures below. Actually, this struct is also more
 * efficient, but that's another story...
 */

/*
 * Describe how a standard IOCTL looks like.
 */
struct iw_ioctl_description
{
	u8_t	header_type;		/* NULL, iw_point or other */
	u8_t	token_type;		/* Future */
	u16_t	token_size;		/* Granularity of payload */
	u16_t	min_tokens;		/* Min acceptable token number */
	u16_t	max_tokens;		/* Max acceptable token number */
	u32_t	flags;			/* Special handling of the request */
};

/* Need to think of short header translation table. Later. */

/* --------------------- ENHANCED SPY SUPPORT --------------------- */
/*
 * In the old days, the driver was handling spy support all by itself.
 * Now, the driver can delegate this task to Wireless Extensions.
 * It needs to include this struct in its private part and use the
 * standard spy iw_handler.
 */

/*
 * Instance specific spy data, i.e. addresses spied and quality for them.
 */
struct iw_spy_data
{
#ifdef IW_WIRELESS_SPY
	/* --- Standard spy support --- */
	int			spy_number;
	u_char			spy_address[IW_MAX_SPY][ETH_ALEN];
	struct iw_quality	spy_stat[IW_MAX_SPY];
#ifdef IW_WIRELESS_THRSPY
	/* --- Enhanced spy support (event) */
	struct iw_quality	spy_thr_low;	/* Low threshold */
	struct iw_quality	spy_thr_high;	/* High threshold */
	u_char			spy_thr_under[IW_MAX_SPY];
#endif /* IW_WIRELESS_THRSPY */
#endif /* IW_WIRELESS_SPY */
};

/**************************** PROTOTYPES ****************************/
/*
 * Functions part of the Wireless Extensions (defined in net/core/wireless.c).
 * Those may be called only within the kernel.
 */
#if 0 //Irene porting
/* First : function strictly used inside the kernel */

/* Handle /proc/net/wireless, called in net/code/dev.c */
extern int dev_get_wireless_info(char * buffer, char **start, off_t offset,
				 int length);

/* Handle IOCTLs, called in net/code/dev.c */
extern int wireless_process_ioctl(struct ifreq *ifr, unsigned int cmd);

/* Second : functions that may be called by driver modules */

/* Send a single event to user space */
extern void wireless_send_event(struct net_device *	dev,
				unsigned int		cmd,
				union iwreq_data *	wrqu,
				char *			extra);

/* We may need a function to send a stream of events to user space.
 * More on that later... */

/* Standard handler for SIOCSIWSPY */
extern int iw_handler_set_spy(struct net_device *	dev,
			      struct iw_request_info *	info,
			      union iwreq_data *	wrqu,
			      char *			extra);
/* Standard handler for SIOCGIWSPY */
extern int iw_handler_get_spy(struct net_device *	dev,
			      struct iw_request_info *	info,
			      union iwreq_data *	wrqu,
			      char *			extra);
/* Standard handler for SIOCSIWTHRSPY */
extern int iw_handler_set_thrspy(struct net_device *	dev,
				 struct iw_request_info *info,
				 union iwreq_data *	wrqu,
				 char *			extra);
/* Standard handler for SIOCGIWTHRSPY */
extern int iw_handler_get_thrspy(struct net_device *	dev,
				 struct iw_request_info *info,
				 union iwreq_data *	wrqu,
				 char *			extra);
/* Driver call to update spy records */
extern void wireless_spy_update(struct net_device *	dev,
				unsigned char *		address,
				struct iw_quality *	wstats);
#endif
/************************* INLINE FUNTIONS *************************/
/*
 * Function that are so simple that it's more efficient inlining them
 */

static inline int iwe_stream_lcp_len(struct iw_request_info *info)
{
#ifdef CONFIG_COMPAT
	if (info->flags & IW_REQUEST_FLAG_COMPAT)
		return IW_EV_COMPAT_LCP_LEN;
#endif
	return IW_EV_LCP_LEN;
}

static inline int iwe_stream_point_len(struct iw_request_info *info)
{
#ifdef CONFIG_COMPAT
	if (info->flags & IW_REQUEST_FLAG_COMPAT)
		return IW_EV_COMPAT_POINT_LEN;
#endif
	return IW_EV_POINT_LEN;
}

static inline int iwe_stream_event_len_adjust(struct iw_request_info *info,
					      int event_len)
{
#ifdef CONFIG_COMPAT
	if (info->flags & IW_REQUEST_FLAG_COMPAT) {
		event_len -= IW_EV_LCP_LEN;
		event_len += IW_EV_COMPAT_LCP_LEN;
	}
#endif

	return event_len;
}

/*------------------------------------------------------------------*/
/*
 * Wrapper to add an Wireless Event to a stream of events.
 */
#define likely(x)	(x)
#define unlikely(x)	(x)

static inline char *
iwe_stream_add_event(struct iw_request_info *info, char *stream, char *ends,
		     struct iw_event *iwe, int event_len)
{
	int lcp_len = iwe_stream_lcp_len(info);

	event_len = iwe_stream_event_len_adjust(info, event_len);

	/* Check if it's possible */
	if(likely((stream + event_len) < ends)) {
		iwe->len = event_len;
		/* Beware of alignement issues on 64 bits */
		memcpy(stream, (char *) iwe, IW_EV_LCP_PK_LEN);
		memcpy(stream + lcp_len, &iwe->u,
		       event_len - lcp_len);
		stream += event_len;
	}
	return stream;
}

/*------------------------------------------------------------------*/
/*
 * Wrapper to add an short Wireless Event containing a pointer to a
 * stream of events.
 */
static inline char *
iwe_stream_add_point(struct iw_request_info *info, char *stream, char *ends,
		     struct iw_event *iwe, char *extra)
{
	int event_len = iwe_stream_point_len(info) + iwe->u.data.length;
	int point_len = iwe_stream_point_len(info);
	int lcp_len   = iwe_stream_lcp_len(info);

	/* Check if it's possible */
	if(likely((stream + event_len) < ends)) {
		iwe->len = event_len;
		memcpy(stream, (char *) iwe, IW_EV_LCP_PK_LEN);
		memcpy(stream + lcp_len,
		       ((char *) &iwe->u) + IW_EV_POINT_OFF,
		       IW_EV_POINT_PK_LEN - IW_EV_LCP_PK_LEN);
		memcpy(stream + point_len, extra, iwe->u.data.length);
		stream += event_len;
	}
	return stream;
}

/*------------------------------------------------------------------*/
/*
 * Wrapper to add a value to a Wireless Event in a stream of events.
 * Be careful, this one is tricky to use properly :
 * At the first run, you need to have (value = event + IW_EV_LCP_LEN).
 */
static inline char *
iwe_stream_add_value(struct iw_request_info *info, char *event, char *value,
		     char *ends, struct iw_event *iwe, int event_len)
{
	int lcp_len = iwe_stream_lcp_len(info);

	/* Don't duplicate LCP */
	event_len -= IW_EV_LCP_LEN;

	/* Check if it's possible */
	if(likely((value + event_len) < ends)) {
		/* Add new value */
		memcpy(value, &iwe->u, event_len);
		value += event_len;
		/* Patch LCP */
		iwe->len = value - event;
		memcpy(event, (char *) iwe, lcp_len);
	}
	return value;
}

#endif	/* _IW_HANDLER_H */

