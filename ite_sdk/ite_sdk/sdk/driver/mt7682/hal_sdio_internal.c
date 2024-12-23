#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ite/itp.h" //declared pthread
//#include "ite/ite_sd.h"
#include "wifi_stub.h"
#include "sdio_io.h"


/* Private macro -------------------------------------------------------------*/
#define WIFI_SDIO_PORT HAL_SDIO_PORT_0
#define DUMP_ISR       0


/* Private val ---------------------------------------------------------------*/
bool sdio_interrupt_flg = false;
uint32_t sdio_interrupt_reg = 0x0;
MT7682_4BYTE_ALIGN uint32_t rx_buf[1024*2];


/* Private functions ---------------------------------------------------------*/


/* Global structures -------------------------------------------------------- */
extern struct mt7682_sdio_st  mt7682;
msdc_config_t msdc_config;


/* ======================== SDIO Interrupt ======================== */

bool sdio_hif_enable_interrupt(struct sdio_func *func)
{
    uint32_t value = 0;

    /*
            1. TX_DONE_INT_EN
            2. RX0_DONE_INT_EN
            3. RX1_DONE_INT_EN
            4. FW_OWN_BACK_INT_EN
      */
    value = 0x000000087;

    /* WLAN HIF Interrupt Enable Register */
    if (0 != sdio_func1_wr(func, SDIO_IP_WHIER, &value, 4))
        return false;
    else
        printf("[%s] WLAN HIF interrupt Enable Register success\n", __FUNCTION__);

    return true;
}

bool sdio_get_interrupt_flag(void)
{
    return sdio_interrupt_flg;
}

void sdio_push_rx_notify(void)
{
    wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt
}

uint32_t sdio_get_interrupt_reg(void)
{
    return sdio_interrupt_reg;
}

void sdio_reset_interrupt_flag(void)
{
    if (sdio_interrupt_flg)
        sdio_interrupt_flg = false;
}

/* WLAN HIF Interrupt Status Register */
uint32_t sdio_read_isr(void)
{
    struct sdio_func *func = mt7682.func;
    int whisr = -1;
    u32 reg = 0xFFFF0000;

    whisr = sdio_func1_rd(func, SDIO_IP_WHISR, &reg, 4);

#if DUMP_ISR
    if (whisr == 0) {
        dump_reg_desc(reg, 8);
    }
#endif

    return reg;
}

hal_sdio_status_t hal_sdio_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback, void *user_data)
{
    if (!sdio_callback) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_config.msdc_sdio_interrupt_callback = sdio_callback;
    return HAL_SDIO_STATUS_OK;
}

extern bool wfcm_sdio_receive_data(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize);
void sdio_interrupt_callback(hal_sdio_int_callback_event_t event, void *user_data)
{
    switch (event)
    {
        case HAL_SDIO_INT_RX_SUCCESS:
            // DMA transfer OK.
            printf("====>Get Rx Notify from Slave(MT7682).\n");
            /*{
                struct sdio_func *func = mt7682.func;
                uint8_t       p[512] = {0};

                wfcm_sdio_receive_data(func, p, sizeof(p));
                //dump_hex_data((void *)p, 512, "Receive data");;

            }*/
            sdio_interrupt_flg = true;
            break;
    }
}

hal_sdio_status_t sdio_interrupt_callback_init(void)
{
    hal_sdio_status_t status = HAL_SDIO_STATUS_OK;

    /* register sdio interrupt callback for standard SDIO ISR */
    status = hal_sdio_register_callback(WIFI_SDIO_PORT, sdio_interrupt_callback, NULL);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio register callback error. \r\n");
        return status;
    } else {
        printf("sdio register callback success. \r\n");
    }

    return HAL_SDIO_STATUS_OK;
}


#ifdef MT7682_GET_INTR_FROM_SDIO_POLLING
/* Polling interrupt task */
void* sdio_get_interrupt_tasklet(void *arg)
{
    struct sdio_func *func = mt7682.func;
    uint32_t whisr_reg = 0xFF;

    printf("Set sdio_get_interrupt_tasklet.\r\n");

    for(;;) {
        /* Get interrupt of data from Tx */
        whisr_reg = sdio_read_isr();
        if (0 == (whisr_reg & ABNORMAL_MASK)){
            if (whisr_reg & RX0_REG_MASK)
            msdc_config.msdc_sdio_interrupt_callback(HAL_SDIO_INT_RX_SUCCESS, (void *)0);
        }

        usleep(100*1000);
    }

    return NULL;
}


/* Create SDIO host interrupt tasklet thread */
int sdio_create_interrupt_tasklet(void)
{
    int res;

    pthread_t          task;
    pthread_attr_t     attr;
    struct sched_param param;
    pthread_attr_init(&attr);
    //pthread_attr_setstacksize(&attr, 10*1024);
    param.sched_priority = 5;
    //pthread_attr_setschedparam(&attr, &param);
    res = pthread_create(&task, &attr, sdio_get_interrupt_tasklet, NULL);
    if (res) {
        printf("Create SDIO host interrupt thread fail! 0x%08X \n");
        return -1;
    } else
        printf("Create SDIO host interrupt thread success.\n");

    return 0;
}

#endif

#ifdef MT7682_GET_INTR_FROM_GPIO
static int intr_gpio = 29; //GPIO 29: INTR

static bool sdio_recv_data(struct sdio_func *func, uint8_t *rx_buf)
{
    uint32_t total_len = 0;

    sdio_func1_rd(func, SDIO_IP_WRPLR, &total_len, 4); //get  packet length

    printf("[sdio_recv_data] packet length(%d)\n", (int)total_len);

    if (sdio_func1_rd(func, SDIO_IP_WRDR0, rx_buf, total_len & 0x0000ffff) != 0) {
        return false;
    }

    dump_hex_data((void *)rx_buf, total_len, "Recv Data");

    return true;
}

void sdio_rx_process_from_isr(void* arg, uint32_t value)
{
    struct sdio_func *func = (struct sdio_func *)arg;
    hal_sdio_status_t status;
    u32 whisr_reg = 0x0;
    u32 buff_len = 0;
    int i;

    while(!sdio_get_interrupt_flag());
    sdio_reset_interrupt_flag();

    //printf("\n[sdio_rx_process_from_isr] ====>sdio host receive go.\r\n\n");

    wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt

    //printf("\n[sdio_rx_process_from_isr] <====sdio host receive ok.\r\n\n");
}

static void Mt7682GpioIntrHandler(unsigned int pin, void *arg)
{
    if (pin != 29) {
        printf("Gpio Intr Handler: Intr gpio error! %d \n", pin);
        return;
    }

    //Can receive
    if (!sdio_get_interrupt_flag()) {
        sdio_interrupt_flg = true;
        printf("\n==== [MT7682 GPIO ISR] ====\n");

#ifdef MT7682_RECV_CALL_FROM_ISR
        itpPendFunctionCallFromISR(sdio_rx_process_from_isr, arg, (uint32_t)sdio_interrupt_flg);
#endif
    }

    return;
}

void gpio_set_interrupt_request(struct sdio_func *func)
{
    printf("==== Set GPIO [%d] Interrupt: Start ====\n", intr_gpio);
    ithGpioEnable(intr_gpio);
    ithGpioSetIn(intr_gpio);
    ithGpioClearIntr(intr_gpio);

    ithGpioRegisterIntrHandler(intr_gpio, Mt7682GpioIntrHandler, (void *)func);

    ithGpioCtrlEnable(intr_gpio, ITH_GPIO_INTR_LEVELTRIGGER);  /* use level trigger mode */
    ithGpioCtrlEnable(intr_gpio, ITH_GPIO_INTR_TRIGGERFALLING); /* low active */
    ithGpioEnableIntr(intr_gpio);
    printf("==== Set GPIO [%d] Interrupt: End ====\n", intr_gpio);
}

#else
/* Get intr from standard SDIO */
static void sdio_get_irq(struct sdio_func *func)
{
    uint32_t whisr_reg = 0x0;

    /* Get interrupt request */
    sdio_func1_rd(func, SDIO_IP_WHISR, &whisr_reg, 4);
    //printf("==== sdio_get_irq:  0x%X ====\r\n", whisr_reg);
    //ithPrintf("sdio_get_irq\r\n");

    if (whisr_reg &(RX0_REG_MASK | RX1_REG_MASK))
        //Can receive
        msdc_config.msdc_sdio_interrupt_callback(HAL_SDIO_INT_RX_SUCCESS, (void *)0);
    else
        //Other registers
        msdc_config.msdc_sdio_interrupt_callback(HAL_SDIO_INT_NORMAL, (void *)0);

    sdio_interrupt_reg = whisr_reg;
}


/* Set SDIO host interrupt request */
int  sdio_set_interrupt_request(struct sdio_func *func)
{
    int rc;

    /* enable irq later */
    sdio_claim_host(func);
    rc = sdio_claim_irq(func, sdio_get_irq);
    if (rc)
        printf("==== Set claim irq fail! rc = 0x%08X ====\n", rc);
    else
        printf("==== Set claim irq success ====\n");
    sdio_release_host(func);

    return rc;
}
#endif

/* ========================================================== */
