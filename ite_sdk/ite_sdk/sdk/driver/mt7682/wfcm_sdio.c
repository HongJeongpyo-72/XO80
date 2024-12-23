
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openrtos/FreeRTOS.h>
#include <openrtos/semphr.h>

/* hal includes */
#include "lwip/pbuf.h"
#include "wifi_stub.h"
#include "sdio_io.h"
#include "sdio_reg.h"
#include "wifi_host_os_api.h"

#define MEM_QUEUE_SIZE              8
#define WFCM_HTX_DATA_SIZE          1900 //1600 increase the buffer size to 1900, because of the wifi initial setting is 1870 bytes.
#define WFCM_HRX_DATA_SIZE          1600

SemaphoreHandle_t g_sem_qbuf        = NULL;
SemaphoreHandle_t g_wfcm_sem_htx    = NULL;
hal_sdio_port_t   sdio_port         = 0;
uint16_t          tx_count          = 0;


#ifndef WIFI_BLK_SIZE
#define WIFI_BLK_SIZE (256)
#endif

#ifndef ALIGN_TO_BLOCK_SIZE
#define ALIGN_TO_BLOCK_SIZE(_value)     (((_value) + (WIFI_BLK_SIZE - 1)) & ~(WIFI_BLK_SIZE - 1))
#endif

extern uint8_t *wifi_hrx_data_alloc(uint32_t bufSize);
extern uint8_t *wifi_hrx_data_buffer(uint8_t *pbuf);
bool wfcm_sdio_receive_data_size(struct sdio_func *func, uint32_t *bufSize);
bool wfcm_sdio_receive_data_payload(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize);
void wifi_hrx_handler(uint8_t *pbuf, uint32_t bufSize);
bool wfcm_sdio_receive_data(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize);

void dump_send_data(void *p, int len, char* name);



uint8_t g_memq_list[MEMQ_CTRL_MAX_NUM][MEM_QUEUE_SIZE];

MT7682_4BYTE_ALIGN        uint8_t g_host_tx_buf[MEM_QUEUE_SIZE][WFCM_HTX_DATA_SIZE];
MT7682_4BYTE_ALIGN static uint8_t g_host_rx_buf[MEM_QUEUE_SIZE][WFCM_HRX_DATA_SIZE];

int32_t wfcm_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx)
{
	uint32_t idx;
    wifi_os_semphr_take(g_sem_qbuf, NULL);

    for (idx=0; idx<(MEM_QUEUE_SIZE-1); idx++)
    {
    	if (g_memq_list[ctrl_idx][idx] == 0)
    	{
    		g_memq_list[ctrl_idx][idx] = 1; //allocated
    		*qidx = idx;

            wifi_os_semphr_give(g_sem_qbuf);
    		return idx;
    	}
    }
    *qidx = WFC_MEMQ_INVALID;

    wifi_os_semphr_give(g_sem_qbuf);
    return -1;
}

void wfcm_mq_free(T_WFC_QBUF *qbuf)
{
    wifi_os_semphr_take(g_sem_qbuf, NULL);
    g_memq_list[qbuf->ctrl][qbuf->qidx] = 0; //free
    qbuf->qidx = WFC_MEMQ_INVALID;
    qbuf->size = 0;
    wifi_os_semphr_give(g_sem_qbuf);
}

void wfcm_memq_get_qbuf(T_WFC_QBUF *qbuf)
{
    // Get free index from Host TX receive queue buffer
    while ( wfcm_mq_get_buf(qbuf->ctrl, &(qbuf->qidx)) < 0)
    {
        usleep(10 * 1000);
    }
}

void wfcm_mem_align_clr(uint8_t *pbuf, uint32_t bufSize)
{
	uint32_t sidx = bufSize;
	uint32_t eidx;

	//sidx = g_hrx_free_qbuf.size;
	eidx = ALIGN_TO_BLOCK_SIZE(bufSize);
	if (eidx > sidx)
	{
	   memset(pbuf+sidx, 0, eidx-sidx);
    }
}

void wfcm_dma_memcpy(void *dst, void *src, unsigned int len)
{
    //Not implement
}

void wfcm_sdio_cmd_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *param, uint32_t paramSize, uint8_t forISR)
{
    uint16_t *op;
    uint8_t  *pdst;

    qbuf->size = paramSize + sizeof(opcode); //opcode(2)

    if (forISR)
    {
        qbuf->qidx = MEM_QUEUE_SIZE-1;
    }
    // Clear HTX Buffer (After Buffer Size)
    wfcm_mem_align_clr(&(g_host_tx_buf[qbuf->qidx][0]), qbuf->size + sizeof(brom_sdio_tx_sdu_header_t));

    // Assign OpCode
    op = (uint16_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ sizeof(brom_sdio_tx_sdu_header_t));
    *op  = opcode;

    // Copy Cmd Parameter
    if (param && paramSize)
    {
        pdst = (uint8_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ sizeof(brom_sdio_tx_sdu_header_t));
        pdst = pdst + sizeof(opcode);        //opcode(2)
        memcpy(pdst, param, paramSize);
    }
}


void wfcm_sdio_htx_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *htxptr, uint32_t htxSize)
{
    uint16_t *op;
    struct pbuf *q;
    uint8_t  *pdst;

    qbuf->size = htxSize+ sizeof(sdio_pkt_header_t); //opcode(2)+2, 4 bytes alignment


    // Clear HTX Buffer (After Buffer Size 4 Bytes)
    wfcm_mem_align_clr(&(g_host_tx_buf[qbuf->qidx][0]), qbuf->size + sizeof(sdio_pkt_header_t));

    // Assign OpCode
    op = (uint16_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ 4);
    *op  = opcode;

    // Copy HTX Data, shift 4 bytes
    pdst = ((uint8_t*)op)+sizeof(sdio_pkt_header_t); //opcode(2)+2, 4 bytes alignment

    // Set sdio_pkt_header_t value
    tx_count++;
    *(op+2) = MT7682_IP_DATA;
    *(op+3) = qbuf->size;
    *(op+4) = tx_count;

    for(q = (struct pbuf *)htxptr; q != NULL; q = q->next) {
      SMEMCPY(pdst, (uint8_t*)q->payload, q->len);
      pdst += q->len;
    }
}

bool wfcm_sdio_check_init(struct sdio_func *func)
{
    uint32_t      wcir_reg = 0;

    do {
        //usleep(1000);
    } while (sdio_func1_rd(func, SDIO_IP_WCIR, (uint8_t *)&wcir_reg, 4) != 0); /*waiting Boot ROM release*/

    printf("[wfcm_sdio_check_init] WLAN Chip ID Register ok (0x%X)\r\n", wcir_reg);

    return true;
}

bool wifi_bus_init(struct sdio_func *func)
{
    uint32_t whcr = 0x10000;

	/*register callback*/
    //(MT7682: sdio_init)
	if (HAL_SDIO_STATUS_OK != sdio_interrupt_callback_init()) {
        printf("sdio init error. \r\n");
        return false;
    }

    if(sdio_func1_wr(func, SDIO_IP_WHCR, (uint8_t *)&whcr, 4) != 0) {
        return HAL_SDIO_STATUS_ERROR;
    }

    return true;
}


bool wfcm_sdio_hif_get_driver_own(struct sdio_func *func)
{
    uint32_t value ;
    uint32_t cnt = 50;

    printf("[wfcm_sdio_hif_get_driver_own]<==========>\r\n");

    //Set driver own
    value = W_FW_OWN_REQ_CLR ;
    if (sdio_func1_wr(func, SDIO_IP_WHLPCR, (uint8_t *)&value, 4) != 0) {
        return false;
    }

    usleep(10*1000);

    while (cnt--) {
        if (sdio_func1_rd(func, SDIO_IP_WHLPCR, (uint8_t *)&value, 4) != 0) {
            return false ;
        } else {
            printf("====>Read SDIO_IP_WHLPCR: 0x%X(0x%X)\n", value, (value & W_DRV_OWN_STATUS));
        }

        if (value & W_DRV_OWN_STATUS) {
            printf("====>Get driver own\n");
            return true;
        }

        usleep(50*1000);
    }

    return false;
}


int wfcm_if_sdio_init(struct sdio_func *func)
{
    hal_sdio_status_t status = 0;
    uint32_t whcr = 0x10000;

    if(g_sem_qbuf == NULL) {
        g_sem_qbuf  = wifi_os_semphr_create_binary();
    }
    wifi_os_semphr_give(g_sem_qbuf);

    if(g_wfcm_sem_htx == NULL) {
        g_wfcm_sem_htx  = wifi_os_semphr_create_binary();
    }
    wifi_os_semphr_give(g_wfcm_sem_htx);

    if(wifi_bus_init(func) == false) {
        return HAL_SDIO_STATUS_ERROR;
    }

    if (false == wfcm_sdio_hif_get_driver_own(func)) {
        printf("[%s] Get driver own fail. \r\n", __FUNCTION__);
        return HAL_SDIO_STATUS_ERROR;
    } else {
        printf("[%s] Get driver own success. \r\n", __FUNCTION__);
    }

    memset(g_host_rx_buf, 0, MEM_QUEUE_SIZE*WFCM_HRX_DATA_SIZE);
    memset(g_host_tx_buf, 0, MEM_QUEUE_SIZE*WFCM_HTX_DATA_SIZE);

    return HAL_SDIO_STATUS_OK;
}

void wfcm_if_sdio_deinit(void)
{
	if(g_sem_qbuf != NULL) {
		wifi_os_semphr_delete(g_sem_qbuf);
		g_sem_qbuf = NULL;
	}
	if(g_wfcm_sem_htx != NULL) {
		wifi_os_semphr_delete(g_wfcm_sem_htx);
		g_wfcm_sem_htx = NULL;
	}
}

bool wfcm_sdio_send_data(struct sdio_func *func, T_WFC_QBUF *qbuf, uint8_t qbuf_release)
{

    uint32_t      whisr_reg = 0;
    uint32_t      read_isr = 0xFF, read_cnt = 0;
    uint32_t      hrxSize;
    bool          ret       = true;

    /* ===== MT7682 pbuf header ===== */
    brom_sdio_tx_sdu_header_t *tx_header = \
        (brom_sdio_tx_sdu_header_t *)&(g_host_tx_buf[qbuf->qidx][0]);

    tx_header->u.asUINT32 = 0;

    tx_header->u.bits.length = qbuf->size + sizeof(brom_sdio_tx_sdu_header_t);
    /* ========================== */

    //Waiting TX_DONE_INT
    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
        //printf("[%s] === reg: 0x%X ===\n", __FUNCTION__, whisr_reg);
    } while (0 == (whisr_reg & TX_REG_MASK)); /*waiting TX_DONE_INT*/

    //Clear TX_DONE
    sdio_func1_rd(func, SDIO_IP_WTSR0, (uint8_t *)&whisr_reg, 4);
    //printf("[%s] WLAN TX Status Register: 0x%X \n", __FUNCTION__, whisr_reg);
    if (sdio_func1_wr(func, SDIO_IP_WTDR1, &(g_host_tx_buf[qbuf->qidx][0]), (qbuf->size + sizeof(brom_sdio_tx_sdu_header_t))) != 0) {
        printf("[ERR] sdio_send_pkt => wifi_write_bus 0x%08x len=%lu error\r\n", SDIO_IP_WTDR1, (qbuf->size + sizeof(brom_sdio_tx_sdu_header_t)));
        ret = false;
    }

    printf("[%s] SDIO send data: %s\n", __FUNCTION__, (ret == true) ? "success":"false");


#if 0
    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
        printf("[%s] === 0x%X ===\n", __FUNCTION__, whisr_reg);
        usleep(100*1000);
    } while (0 == (whisr_reg & RX0_REG_MASK)); /*waiting RX_DONE_INT*/

    {
        uint8_t       p[512] = {0};
        wfcm_sdio_receive_data(func, p, 256);
        dump_hex_data((void *)p, 256, "p");
    }
#endif

    //Free HTX Buffer
    if (qbuf_release){
        wfcm_mq_free(qbuf);
    }

    return ret;
}

bool wfcm_sdio_receive_cmdrsp(struct sdio_func *func, uint8_t *rx_buf, uint32_t *bufSize)
{
    uint32_t whisr_reg = 0;
    uint32_t wrplr_reg = 0;

    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
    } while (0 == (whisr_reg & RX0_REG_MASK)); /*wait RX0_DONE_INT */

    sdio_func1_rd(func, SDIO_IP_WRPLR, (uint8_t *)&wrplr_reg, 4);
    *bufSize = (wrplr_reg & RX_BYTE_MASK);

    if (sdio_func1_rd(func, SDIO_IP_WRDR0, rx_buf, (wrplr_reg & RX_BYTE_MASK)) != 0) {
        printf("[ERR] sdio_receive_pkt: SDIO_IP_WRDR0 fail\r\n");
        return false;
    }

    return true;
}

bool wfcm_sdio_receive_data(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize)
{
    uint32_t whisr_reg = 0;
    uint32_t wrplr_reg = 0;

    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
    } while (0 == (whisr_reg & RX0_REG_MASK)); /*wait RX0_DONE_INT*/

    sdio_func1_rd(func, SDIO_IP_WRPLR, (uint8_t *)&wrplr_reg, 4);

    bufSize = wrplr_reg;

    if (sdio_func1_rd(func, SDIO_IP_WRDR0, pbuf, bufSize)  != 0) {
        printf("[ERR] sdio_receive_pkt: SDIO_IP_WRDR0 fail\r\n");
        return false;
    }

    return true;
}


bool wfcm_sdio_receive_data_size(struct sdio_func *func, uint32_t *bufSize)
{
    uint32_t whisr_reg = 0;
    uint32_t wrplr_reg = 0;

    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4); 
        dump_reg_desc(whisr_reg, 8);
        //printf("[%s] whisr_reg: 0x%X\n", __FUNCTION__, whisr_reg);
    } while (0 == (whisr_reg & RX0_REG_MASK));

    sdio_func1_rd(func, SDIO_IP_WRPLR, (uint8_t *)&wrplr_reg, 4);

    *bufSize = wrplr_reg & RX_BYTE_MASK;

    return true;
}


bool wfcm_sdio_receive_data_payload(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize)
{
    if (sdio_func1_rd(func, SDIO_IP_WRDR0, pbuf, bufSize) != 0) {
        printf("[ERR] sdio_receive_pkt: SDIO_IP_WRDR0 fail\r\n");
        return false;
    }

    return true;
}


