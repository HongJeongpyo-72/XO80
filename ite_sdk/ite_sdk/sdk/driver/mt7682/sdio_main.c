
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "lwip/pbuf.h"

#include "sdio_io.h"
#include "wifi_stub.h"
#include "wifi_host_os_api.h"
#include "ite/ite_wifi.h"

#define DRV_NAME "MT_7682"
#define WIFI_SDIO_PORT HAL_SDIO_PORT_0
#ifdef Test_Stub_RW_Enable
#define WFCM_SDIO_HOST_INTERRUPT_PROJECT
#endif

#define WFCM_STUB_TASK_NAME        "MT7682"
#define WFCM_STUB_TASK_STACK_SIZE   512
#define WFCM_STUB_TASK_PRIORITY       7

/* Declaire struct of Sdio function and Netif */
struct mt7682_sdio_st  mt7682;
struct netif xnetif[MT7682_NET_IF_NUM];

static const struct sdio_device_id sdio_ids[] = {
	{ SDIO_DEVICE(0x037A, 0x7682),.driver_data = NULL}
};


/* Declaration */
static int drv_init(struct sdio_func *func, const struct sdio_device_id *id);
static void dev_remove(struct sdio_func *func);

struct sdio_drv_priv {
	struct sdio_driver wifi_drv;
	int drv_registered;
};

static struct sdio_drv_priv sdio_drvpriv = {
	.wifi_drv.probe = drv_init,
	.wifi_drv.remove = dev_remove,
	.wifi_drv.name = (char*)DRV_NAME,
	.wifi_drv.id_table = sdio_ids,
};


static TaskHandle_t  wifi_stub_task_handle = NULL;
static TaskHandle_t  wifi_stub_rx_task_handle = NULL;
int wifi_host_init(void)
{
    struct sdio_func *func = mt7682.func;
    int rc;
    hal_sdio_status_t status;


    /* Enable function & Get IO ready */
    sdio_claim_host(func);
    rc = sdio_enable_func(func);
    sdio_release_host(func);

    if (rc) {
        printf("[%s] enable function fail! rc = 0x%08X \n", __FUNCTION__, rc);
        return -1;
    }

    // WFC Host Task
    rc = wifi_host_wfcm_init();

    if (rc < 0) {
        printf("[%s] Host WFCM init fail!\n", __FUNCTION__);
        return rc;
    }
#ifdef Test_Stub_RW_Enable

#if CREATE_SDIO_RW_TASKLET
    sleep(1);
    sdio_create_rw_tasklet();
#else
    /* For simple SDIO R/W test */
    sdio_rw_process(func);
#endif

#else

    if ( wifi_os_task_create((TaskFunction_t)wifi_stub_task,
                            WFCM_STUB_TASK_NAME,
                            WFCM_STUB_TASK_STACK_SIZE*4,
                            NULL,
                            WFCM_STUB_TASK_PRIORITY,
                            &wifi_stub_task_handle) != 0 )
    {
    	printf( "WFC Task Create Fail!\r\n");
    	return -1;
    }

    if ( wifi_os_task_create((TaskFunction_t)wifi_stub_recv_task,
                            "MT7682 Rx",
                            WFCM_STUB_TASK_STACK_SIZE*4,
                            NULL,
                            WFCM_STUB_TASK_PRIORITY,
                            &wifi_stub_rx_task_handle) != 0 )
    {
    	printf( "WFC Rx Task Create Fail!\r\n");
    	return -1;
    }

#endif

#ifdef WFCM_SDIO_HOST_INTERRUPT_PROJECT
    /* Enable sdio slave interrupt */
    sdio_hif_enable_interrupt(func);
#endif

    /* Set network interface from LWIP */
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFI_ADD_NETIF, &xnetif[0]);

    if (!(xnetif[0].flags & NETIF_FLAG_LINK_UP))
        xnetif[0].flags |= NETIF_FLAG_LINK_UP; //Link Layer has been ready.

    return 0;
}

/* ========================================================== */





/* ========================= Initial SDIO ========================= */

extern int sdio_register_driver(struct sdio_driver *);
SDIO_BUS_OPS ite_sdio_bus_ops = {
	NULL,
	NULL,
	sdio_enable_func,
	sdio_disable_func,
	sdio_register_driver,//reg_driver
	NULL, //sdio_unregister_driver,//unreg_driver
	sdio_claim_irq,
	sdio_release_irq,
	sdio_claim_host,
	sdio_release_host,
	sdio_readb,
	sdio_readw,
	sdio_readl,
	sdio_writeb,
	sdio_writew,
	sdio_writel,
	sdio_memcpy_fromio,
	sdio_memcpy_toio
};

static int drv_init(struct sdio_func *func, const struct sdio_device_id *id)
{
    hal_sdio_status_t status = HAL_SDIO_STATUS_OK;

    printf("====>drv_init: hook sdio_func!! vendor=0x%04x device=0x%04x class=0x%02x\n",
    	func->vendor, func->device, func->class);
    mt7682.func = NULL;
    mt7682.func = func;

    //printf("====>drv_init: mt7682.func(0x%X), func(0x%X)\n", mt7682.func, func);

    return 0;
}

static void dev_remove(struct sdio_func *func)
{
    printf("====>dev_remove()\n");
    return;
}

int mmpMTWifiDriverRegister(void)
{
	int ret;

	ret = sdio_register_driver(&sdio_drvpriv.wifi_drv);
	printf("====>Do mmpMTWifiDriverRegister(%d)....\n", ret);

	return ret;
}

int mmpMT7682WifiDriverOn(void)
{
    printf("====>MT7682 Wifi Driver On \n");
    if (wifi_host_init() == 0) {
        /* This is for hal_examples auto-test */
        printf("example project test success.\n\n");

        return 0;
    } else {
        /* This is for hal_examples auto-test */
        printf("example project test fail.\n\n");

        return -1;
    }
}

/* ========================================================== */


