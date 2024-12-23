#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lwip/pbuf.h"
#include "lwip/mem.h"

#include "sdio_io.h"
#include "wifi_stub.h"
#include "wfcm_sdio.h"
#include "wifi_host_os_api.h"
#include "ite/ite_wifi.h"


/* Private macro ------------------------------------------------------------- */
#define WIFI_MAGIC_NUM            0x21436587
#define WIFI_MAGIC_LEN            4
#define WIFI_HTX_DATA_SIZE        1900 //1600 increase the buffer size to 1900, because of the wifi initial setting is 1870 bytes.
#define WIFI_HRX_DATA_SIZE        1600
#define MAX_CMD_RSP_LENGTH        256
#define MAX_PARAM_LENGTH          256
#define WIFI_QUEUE_SIZE           20

/* For test or dump debug message */
#define SDIO_RECV_DEBUG 1



/* Private define ------------------------------------------------------------ */
extern struct mt7682_sdio_st  mt7682;
extern struct netif xnetif[MT7682_NET_IF_NUM];
MT7682_4BYTE_ALIGN uint32_t rx_buf[1024*2];


/* Private val -------------------------------------------------------------- */
/* Tx Queue */
static TaskHandle_t  wfcm_evt_handle        = NULL;
static QueueHandle_t wfcm_queue             = NULL;
static QueueHandle_t wfcm_evt_queue         = NULL;
os_semaphore_t       g_wfcm_sem_rsp         = NULL;


/* Private functions --------------------------------------------------------- */
void mt7682_low_level_input(struct netif *netif, struct pbuf *p);
bool wfcm_sdio_check_init(struct sdio_func *func);
bool sdio_send_pkt(struct sdio_func *func, uint8_t tx_que, uint8_t *tx_buf, int data_length);
int  sdio_create_rw_tasklet(void);


#if 0
MT7682_4BYTE_ALIGN uint32_t tx_buf[128];
int sdio_tx_process(struct sdio_func *func)
{
    hal_sdio_status_t status;
    tx_header *tx_buffer_header;
    u32 *tx_buffer;
    u32 whisr_reg = 0xFF;
    u32 buff_len = 0;
    bool tx_exit = false;
    int i;

    printf("Tx_Test: sdio host send start =====>\r\n");

    while(!tx_exit) {
        whisr_reg = sdio_read_isr();
        printf("Tx_Test: whisr_reg(0x%X).\r\n", whisr_reg);

        if (whisr_reg & TX_REG_MASK) {
            /*tx done*/
            memset(&tx_buf, 0, sizeof(tx_buf));

            tx_buffer_header = (tx_header *)tx_buf;
            tx_buffer        = (u32 *)tx_buf;

            tx_buffer_header->length = (128 * 4) + sizeof(tx_header);
            tx_buffer += (sizeof(tx_header) / 4);

            for (i = 0; i < (16 * 4); i++) {
                *tx_buffer++ = (0xaa55aa55 + i);
            }

            if(false == sdio_send_pkt(func, 0, (uint8_t *)&tx_buf, 128 * 4)) {
                printf("Tx_Test: sdio host send error!\r\n");
                tx_exit = true;
            }

            sleep(1);

            printf("Tx_Test: sdio host send ok.\r\n");
        }
    }
    printf("Tx_Test: sdio host send end by ERROR <=====\r\n");

    return -1;
}
#endif


/* Handle functions =========================================== */
//true - no wait response, false - need wait response from slave
T_WFC_CONFIG g_wfcm_stub_opcode[] =
{
	//WFC System Cmd
    {WFC_CT_SYSTEM, WFC_CMD_DTH_READY,         false},   //Need wait HRX data size
    {WFC_CT_SYSTEM, WFC_CMD_DTW_DATA,          false},   //No Response Need
    {WFC_CT_SYSTEM, WFC_CMD_TASK_DEINIT,       false},   //No Response Need

    //WiFi Configure Cmd
    {WFC_CT_USER, WFC_CMD_WFC_READY,          false},

    //OS Cmd
    //{WFC_CT_USER, WFC_CMD_IP_READY,            false},
    //{WFC_CT_USER, WFC_CMD_OS_INFO,             true},

    //Platform Cmd
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF,        true},
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF_CLIENT, true},


    //{WFC_CT_USER, WFC_CMD_API,                  false},
    //{WFC_CT_USER, WFC_CMD_BWCS,                 false},

    {WFC_CT_USER, WFC_CMD_MAX_SUPPORT,         true},
};

T_WFC_CONFIG *wfcm_get_config(uint16_t opcode)
{
    int            idx = 0;
    T_WFC_CONFIG  *pstub;

    pstub = &g_wfcm_stub_opcode[idx];

    while (pstub->op != WFC_CMD_MAX_SUPPORT)
    {
        if (pstub->op == opcode )
        {
            return pstub;
        }
        pstub = &g_wfcm_stub_opcode[++idx];
    }

    printf("Get g_wfcm_stub_opcode fail.\n");
    return NULL;
}
/* ====================================================== */



/* Rx functions ============================================== */
uint8_t *wifi_hrx_data_alloc(uint32_t bufSize)
{
    //return pbuf_alloc(PBUF_RAW, bufSize, PBUF_RAM);
    return (uint8_t*)pbuf_alloc(PBUF_RAW, bufSize, PBUF_POOL);
}

uint8_t *wifi_hrx_data_buffer(uint8_t *pbuf)
{
    return (pbuf)?((struct pbuf *)pbuf)->payload:NULL;
}

bool sdio_receive_pkt(struct sdio_func *func, uint8_t *rx_buf)
{
    uint32_t total_len = 0;

    sdio_func1_rd(func, SDIO_IP_WRPLR, &total_len, 4); //get  packet length

    printf("[sdio_receive_pkt] packet length(%d)\n", (int)total_len);

    if (sdio_func1_rd(func, SDIO_IP_WRDR0, rx_buf, total_len & 0x0000ffff) != 0) {
        return false;
    }

    return true;
}

void wifi_hrx_handler(uint8_t *pbuf, uint32_t bufSize)
{
    struct pbuf  *p;
    struct netif *sta_if;
    uint32_t    *pmagic_s;
    uint32_t    *pmagic_e;

    if (!pbuf)
    {
        printf("Null Packet Received!\n");
        return;
    }

    p = (struct pbuf *)pbuf;

    //Check if "Event Packet" or "Data Packet"
    pmagic_s = (uint32_t*)p->payload;
    if ( (*pmagic_s) == WIFI_MAGIC_NUM )
    {
        //Handle the Event Packet and send those into queue
        pmagic_e = (uint32_t*)(p->payload + bufSize - WIFI_MAGIC_LEN);
        if ((*pmagic_e) == WIFI_MAGIC_NUM)
        {
            uint8_t  *ptr, *param=NULL;
            uint16_t *peid;
            uint32_t  ptrSize;

            ptr = (uint8_t *)(p->payload + WIFI_MAGIC_LEN);
            peid = (uint16_t*)ptr;
            ptr += sizeof(uint16_t);
            ptrSize = bufSize - WIFI_MAGIC_LEN*2 - sizeof(uint16_t);
            if (ptrSize)
            {
                //Allocate Event Parameter, Need to be free by event handler
                param = (uint8_t*)wifi_os_malloc(ptrSize+1);
                if (param == NULL)
                {
                    printf("malloc fail!\n");
                    return;
                }
                //printf("Event alloc buf:0x%x, size=%d\n",  param,  ptrSize+1);
                memcpy(param, ptr, ptrSize);
                param[ptrSize] = 0;
            }

            // Send Event to Event Queue
            //TODO

            //Free SDIO received Buffer
            pbuf_free(p);

            return;
        }
    }

    //sta_if = netif_find_by_type(2);  //NETIF_TYPE_STA = 2

    //printf("lwip <- S(%d)\r\n", bufSize);

    printf("HRX data, size = %d.\n",bufSize);

    mt7682_low_level_input(xnetif, p);
    pbuf_free(p);
}

/* ====================================================== */


/* Tx functions ============================================== */
bool sdio_send_pkt(struct sdio_func *func, uint8_t tx_que, uint8_t *tx_buf, int data_length)
{
    uint32_t whisr_reg = 0xFF;
    uint32_t total_len = 0;

    //Waiting TX_DONE_INT
    do {
        sdio_func1_rd(func, SDIO_IP_WHISR, &whisr_reg, 4);
        printf("[%s] whisr_reg: 0x%X\n", __FUNCTION__, whisr_reg);
        usleep(1000);
    } while (0 == (whisr_reg & TX_REG_MASK)); /*waiting TX_DONE_INT*/

    /* Clear TX_DONE */
    sdio_func1_rd(func, SDIO_IP_WTSR0, &whisr_reg, 4); //WLAN TX Status Register

    if (sdio_func1_wr(func, SDIO_IP_WTDR1, tx_buf, data_length) != 0) {
        return false;
    }

    return true;
}


T_WFC_OPCODE *plist_htx = NULL;
extern SemaphoreHandle_t g_wfcm_sem_htx;
uint8_t sdio_tx_buff[WIFI_HTX_DATA_SIZE];

void wfcm_push_htx_buff(T_WFC_OPCODE *qbuf)
{
    T_WFC_OPCODE   *pnext;
    wifi_os_semphr_take(g_wfcm_sem_htx, NULL);
    qbuf->next = NULL;

    if (!plist_htx) {
        plist_htx = qbuf;
    } else {
        pnext = plist_htx;
        while (pnext->next) {
            pnext = pnext->next;
        }
        pnext->next = qbuf;
    }

    wifi_os_semphr_give(g_wfcm_sem_htx);
}

T_WFC_OPCODE  *wfcm_pop_htx_buff(void)
{
    T_WFC_OPCODE *pdst;
    wifi_os_semphr_take(g_wfcm_sem_htx, NULL);
    pdst = plist_htx;
    if (pdst) {
        plist_htx = pdst->next;
    }
    wifi_os_semphr_give(g_wfcm_sem_htx);

    return pdst;
}

uint16_t wfcm_send_cmd_to_slave(struct sdio_func *func, T_WFC_OPCODE *pwfc, uint8_t qbuf_release)
{
    uint16_t      *op;
    uint16_t       rsp;
    uint32_t       rspSize;
    uint8_t        rspbuf[MAX_CMD_RSP_LENGTH];
    T_WFC_CONFIG  *pwfg;

    pwfg    = pwfc->pwfg;
    op      = (uint16_t*)&rspbuf[0];
    rspSize = MAX_CMD_RSP_LENGTH;

    SMEMCPY(rspbuf, 0, MAX_CMD_RSP_LENGTH);


    // Send Host Command
    if(wfcm_sdio_send_data(func, &(pwfc->qbuf), qbuf_release) == false){
        printf("WFCM send CMD fail.\n");
    }

    // Receive Command Response
    if(wfcm_sdio_receive_cmdrsp(func, rspbuf, &rspSize) == false){
        printf("WFCM rcv CMD rsp fail\n");
    }

    // Get Response Code
    rsp = *op;

    if ( !pwfg->auto_rsp )
    {
    	// Copy Parameter Value
        SMEMCPY(pwfc->rxptr, &rspbuf[2], rspSize-2);
        *(pwfc->rspsize) = rspSize-2;
        //wfcm_dump_buf(rspbuf, rspSize);
    }

    return rsp;
}

uint16_t wfcm_send_data_to_slave(struct sdio_func *func, T_WFC_QBUF *qbuf)
{
    if(wfcm_sdio_send_data(func, qbuf, true) != true){
        printf("WFCM send data fail.\n");
        return WFC_RSP_PARAM_ERR;
    }

    return WFC_RSP_OK;
}

uint16_t wfcm_to_slave(uint16_t op, void *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR)
{
    uint16_t      rspcode = WFC_RSP_OK;
    T_WFC_OPCODE  wfcm_dth;
    T_WFC_CONFIG *pwfg;

    if (!wfcm_queue || !g_wfcm_sem_rsp)
    {
    	printf("[WFCM]: To Slave Fail - %u, %u.\n", (unsigned int)wfcm_queue, (unsigned int)g_wfcm_sem_rsp);
    	return WFC_RSP_QUEUE_ERR;
    }
    // Get wifi configure opcode information
    pwfg = wfcm_get_config(op);
    if (!pwfg)
    {
        printf("Invalid OpCode.\n");
        return WFC_RSP_INVALID_OPCODE;
    }

    if (fromISR)
    {
        wfcm_dth.opcode  = op;
        wfcm_dth.txptr   = ptr;
        wfcm_dth.txsize  = ptrSize;
        wfcm_dth.rxptr   = rsp;
        wfcm_dth.rspsize = rspSize;
        wfcm_dth.pwfg    = pwfg;
        wfcm_dth.rspcode = &rspcode;
        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm_dth.qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_dth.qbuf.qidx = WFC_MEMQ_INVALID;
        if ( wifi_os_queue_send_from_isr(wfcm_queue, &wfcm_dth) != 0 )
        {
            printf("Send op[%u] fail from ISR.\n", wfcm_dth.opcode);
            return WFC_RSP_QUEUE_ERR;
        } else {
            printf("Send op[%u] success from ISR.\n", wfcm_dth.opcode);
        }
        //wifi_os_switch_context_from_isr();
        wifi_os_switch_context();
    }
    else
    {
        /* Send data into queue -> wfcm_queue_handler()  */
        T_WFC_OPCODE  *wfcm;

        wfcm = (T_WFC_OPCODE *)wifi_os_malloc(sizeof(T_WFC_OPCODE));
        memset(wfcm, 0, sizeof(T_WFC_OPCODE));

        wfcm->opcode  = op;
        wfcm->txptr   = ptr;
        wfcm->txsize  = ptrSize;
        wfcm->rxptr   = rsp;
        wfcm->rspsize = rspSize;
        wfcm->pwfg    = pwfg;
        wfcm->rspcode = &rspcode;

        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm->qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_memq_get_qbuf(&(wfcm->qbuf));
        if ( op == WFC_CMD_DTW_DATA)
        {
            // Copy Host TX data into HTX buffer
            wfcm_sdio_htx_cp(WFC_CMD_DTW_DATA, &(wfcm->qbuf), ptr, ptrSize); //Copy Host TX Data to SDIO TX Buffer
        }
        else
        {
            // Copy Cmd Parameter into HTX buffer
            wfcm_sdio_cmd_cp(op, &(wfcm->qbuf), ptr, ptrSize, 0);
        }

        wfcm_push_htx_buff(wfcm); // push data of SDIO TX buffer into queue

        printf("Push buffer into Queue safely\n");

        // send rsp info into queue
        wfcm_dth.opcode  = WFC_CMD_DTW_DATA;
        wfcm_dth.txptr   = NULL;
        wfcm_dth.txsize  = 0;
        wfcm_dth.rxptr   = NULL;
        wfcm_dth.rspsize = 0;
        wfcm_dth.pwfg    = wfcm_get_config(WFC_CMD_DTW_DATA);
        wfcm_dth.rspcode = &rspcode;

        // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
        wfcm_dth.qbuf.ctrl = MEMQ_CTRL_HOST_TX;

        wfcm_dth.qbuf.qidx = WFC_MEMQ_INVALID;
        if ( wifi_os_queue_send(wfcm_queue, &wfcm_dth, 0) != 0 )
        {
            printf("Queue send fail.\r\n");

            return WFC_RSP_QUEUE_ERR;
        } else {
            printf("Queue send success [type(%u)/op(%u)/auto_rsp(%u)].\n",
                wfcm_dth.pwfg->type, wfcm_dth.pwfg->op, wfcm_dth.pwfg->auto_rsp);
        }

        // Send WiFi Configure Response Code (Response No Wait)
        if ( !pwfg->auto_rsp && (op!=WFC_CMD_DTH_READY) && (op!=WFC_CMD_DTW_DATA) )
        {
            wifi_os_semphr_take(g_wfcm_sem_rsp, NULL);
            return rspcode;
        }
    }
    return WFC_RSP_OK;
}


void wfcm_data_free(struct pbuf *p)
{
	pbuf_free(p);
}

void wfcm_tx_handler(struct sdio_func *func, T_WFC_OPCODE *wfcm_qbuf)
{
    if((wfcm_qbuf->pwfg->type == WFC_CT_SYSTEM) && (wfcm_qbuf->opcode == WFC_CMD_DTW_DATA)) {
        //printf("Send data to slave,size = %d.\n", wfcm_qbuf->txsize);
        wfcm_send_data_to_slave(func, &(wfcm_qbuf->qbuf));
    }else if(wfcm_qbuf->pwfg->type != WFC_CT_SYSTEM){
        //printf("Send CMD to slave.\n");
        *(wfcm_qbuf->rspcode) = wfcm_send_cmd_to_slave(func, wfcm_qbuf, true);
        if (!(wfcm_qbuf->pwfg->auto_rsp))
        {
            wifi_os_semphr_give(g_wfcm_sem_rsp);
        }
    }else{
        printf("TX unkown type.%d,%d.\n", wfcm_qbuf->pwfg->type,wfcm_qbuf->opcode);
    }
}

/* =========================================================== */


void wfcm_queue_handler(struct sdio_func *func)
{
    uint8_t       bExit = false;
    uint8_t       rspbuf[MAX_CMD_RSP_LENGTH];
    uint32_t      rspSize;
    int32_t       queue_recv;
    struct timeval tv, *timeout;
    T_WFC_OPCODE *wfcm_buff;

    T_WFC_OPCODE  wfcq;
    T_WFC_CONFIG *pwfg;

    wfcm_queue = wifi_os_queue_create(WIFI_QUEUE_SIZE, sizeof(T_WFC_OPCODE));
    if (wfcm_queue==NULL)
    {
        printf("Create wfcm_queue Fail!\n");
        vTaskDelete(NULL);
        return;
    } else {
        printf("Create wfcm_queue Success!\n");
    }

    g_wfcm_sem_rsp = wifi_os_semphr_create_binary();

    memset(rspbuf, 0, sizeof(rspbuf));

    tv.tv_usec = 1000;
    tv.tv_sec  = 0;
    timeout    = &tv;

    while (!bExit)
    {
        //printf("Start receive wfcm_queue\n");
        queue_recv = wifi_os_queue_receive(wfcm_queue, &wfcq, timeout);
        if (queue_recv == 0)
        {
            printf("Queue recv op:%d.\n", wfcq.opcode);
            pwfg = wfcq.pwfg;

            if (pwfg->type == WFC_CT_SYSTEM)
            {
                // OpCode Process
                switch (wfcq.opcode)
                {
                	//WFC System Cmd
                	case WFC_CMD_DTW_DATA:
                    {
                        wfcm_buff = wfcm_pop_htx_buff();
                        wfcm_tx_handler(func, wfcm_buff);
                        wfcm_mq_free(&(wfcm_buff->qbuf));
                        wifi_os_free(wfcm_buff);
                    }
                    break;

                    case WFC_CMD_DTH_READY:
                    {
                    	uint8_t    *p;
                    	uint32_t    hrxSize;

                        //Get Host RX Data
                        wfcm_sdio_receive_data_size(func, &hrxSize);

                        //printf("[WFC_CMD_DTH_READY] RCV data size = %d.\n",(int)hrxSize);
                        //Allocate Data Buffer (pbuf/mem)
                        p = wifi_hrx_data_alloc(hrxSize);
                        if (!p)
                            printf("Data Buffer Allocate Fail!,size = %d.\n", (int)hrxSize);

                        while(!p) {
                            p = wifi_hrx_data_alloc(hrxSize);
                            vTaskDelay(10);
                        }

                        wfcm_sdio_receive_data_payload(func, (uint8_t*)wifi_hrx_data_buffer(p), hrxSize);

                        //Host RX Receiver
                        wifi_hrx_handler(p, hrxSize);
                    }
                    break;

                    case WFC_CMD_TASK_DEINIT:
                    {
                        bExit = true;
                        wfcm_buff = wfcm_pop_htx_buff();
                        while(wfcm_buff != NULL) {
                            wfcm_mq_free(&(wfcm_buff->qbuf));
                            wifi_os_free(wfcm_buff);
                            wfcm_buff = wfcm_pop_htx_buff();
                        }
                        if(wfcm_queue != NULL) {
                            wifi_os_queue_delete(wfcm_queue);
                            wfcm_queue = NULL;
                        }
                        if(g_wfcm_sem_rsp != NULL) {
                            wifi_os_semphr_delete(g_wfcm_sem_rsp);
                            g_wfcm_sem_rsp = NULL;
                        }
                        printf("Delete wfcm_stub_task task.\r\n");
                    }
                    break;

                    default:
                    	break;
                }
            }
        }else {
            //printf("Start receive wfcm_queue failed (%d)\n", queue_recv);
        }
    }

    printf("wfcm_queue_handler: Task Del\n");
    vTaskDelete(NULL);
}


int wifi_host_wfcm_init(void)
{
    struct sdio_func *func = mt7682.func;
    int  ret, tasklet;

    /*Waiting Boot ROM release*/
    ret = wfcm_sdio_check_init(func);
    printf("====>Boot ROM check %s.\n", ret ? "success":"failed");

    ret = wfcm_if_sdio_init(func);
    printf("Host interface init %s.\n", (ret == HAL_SDIO_STATUS_OK) ? "success":"failed");

#ifdef MT7682_GET_INTR_FROM_GPIO
    gpio_set_interrupt_request(func);
#else
    /* Set standard SDIO host interrupt request */
    sdio_set_interrupt_request(func);
#endif

#ifdef MT7682_GET_INTR_FROM_SDIO_POLLING
    /* Create SDIO host interrupt tasklet thread */
    tasklet = sdio_create_interrupt_tasklet();
    if (tasklet != HAL_SDIO_STATUS_OK)
        return -1;
#endif

    if (ret)
    {
        printf("Host Interface Init Fail!\n");
        return -1;
    }

	printf("Host interface init done.\n");

    return ret;
}


int wfcm_sdio_rx_process(struct sdio_func *func)
{
    hal_sdio_status_t status;
    u32 whisr_reg = 0x0;
    u32 buff_len = 0;
    bool rx_exit = false;
    int i;

    sdio_func1_rd(func, SDIO_IP_WASR, &whisr_reg, 4);
    printf("[wfcm_sdio_rx_process] sdio receive data start(0x%X) =====>\r\n", whisr_reg);

    while (!rx_exit) {
        while(!sdio_get_interrupt_flag());
        sdio_reset_interrupt_flag();

        sdio_func1_rd(func, SDIO_IP_WHISR, &whisr_reg, 4);
        printf("[wfcm_sdio_rx_process] whisr_reg(0x%X)\r\n", whisr_reg);

        dump_reg_desc(whisr_reg, 8);

        if (whisr_reg & RX0_REG_MASK && !(whisr_reg & ABNORMAL_MASK)) {
            memset(rx_buf, 0, sizeof(rx_buf));
            if (false == sdio_receive_pkt(func, (void *)rx_buf)) {
                printf("[wfcm_sdio_rx_process] sdio host receive error!\r\n");
                rx_exit = true;
            }

            /*for (i = 0; i < 64; i++) {
                if (rx_buf[i] != (0xaabbcc00 + i)) {
                    sdio_func1_rd(func, SDIO_IP_WRPLR, &buff_len, 4); //get  packet length
                    printf("receive data error, data = %x, i = %d, len = %d\r\n", rx_buf[i], i, (int)buff_len);
                } else {
                    sdio_func1_rd(func, SDIO_IP_WRPLR, &buff_len, 4); //get  packet length
                    printf("receive data success, data = %x, i = %d, len = %d\r\n", rx_buf[i], i, (int)buff_len);
                }
            }*/

            dump_hex_data((void *)rx_buf, 256, "Rx pkt");
            printf("\n");

            //rx_exit = true;
            //usleep(500*1000);
            printf("[wfcm_sdio_rx_process] <====sdio host receive ok.\r\n\n");
        }

        usleep(10*1000);
    }
    printf("Rx_Test: sdio receive data end by ERROR <=====\r\n");

    return -1;
}

void wifi_stub_task(void)
{
    struct sdio_func *func = mt7682.func;

    //vTaskDelay(1000);
    wfcm_queue_handler(func);
    printf("vTaskDelete: wifi_stub_task.\n");

    vTaskDelete(NULL);
}

void wifi_stub_recv_task(void)
{
    struct sdio_func *func = mt7682.func;
    int  ret = -1;

    //vTaskDelay(1000);
#ifdef MT7682_STUB_RECV_TASK
    ret = wfcm_sdio_rx_process(func);
#endif

    if (ret < 0) {
        printf("vTaskDelete: wifi_stub_recv_task.\n");
        vTaskDelete(NULL);
    }
}


#ifdef Test_Stub_RW_Enable
static TaskHandle_t  wifi_stub_tx_task_handle = NULL;
static TaskHandle_t  wifi_stub_rx_task_handle = NULL;
void* wifi_stub_rx_task(void* arg)
{
    struct sdio_func *func = mt7682.func;
    int  ret, tasklet;

    ret = sdio_rx_process(func);

    if (ret < 0) {
        printf("vTaskDelete.\n");
        vTaskDelete(NULL);
    }
}


void* wifi_stub_tx_task(void* arg)
{
    struct sdio_func *func = mt7682.func;
    int  ret, tasklet;

    ret = sdio_tx_process(func);

    if (ret < 0) {
        printf("vTaskDelete.\n");
        vTaskDelete(NULL);
    }
}

int sdio_create_rw_tasklet(void)
{
    int res;

    pthread_t          rx_task, tx_task;
    pthread_attr_t     rx_attr, tx_attr;
    struct sched_param rx_param, tx_param;

    //Rx Task
#if Rx_Task
    pthread_attr_init(&rx_attr);
    pthread_attr_setstacksize(&rx_attr, 1024);
    rx_param.sched_priority = 5;
    //pthread_attr_setschedparam(&attr, &param);
    res = pthread_create(&rx_task, &rx_attr, wifi_stub_rx_task, NULL);
    if (res) {
        printf("Create SDIO host Rx test thread fail! 0x%08X \n");
        return -1;
    } else
        printf("Create SDIO host Rx test thread success.\n");
#endif


    //Tx Task
#if Tx_Task
    pthread_attr_init(&tx_attr);
    pthread_attr_setstacksize(&tx_attr, 1024);
    tx_param.sched_priority = 5;
    //pthread_attr_setschedparam(&attr, &param);
    res = pthread_create(&tx_task, &tx_attr, wifi_stub_tx_task, NULL);
    if (res) {
        printf("Create SDIO host Tx test thread fail! 0x%08X \n");
        return -1;
    } else
        printf("Create SDIO host Tx test thread success.\n");
#endif

    return 0;
}

#endif


#if SDIO_RECV_DEBUG
void dump_hex_data(void* ptr, int len, char* name)
{
    unsigned char *tx_data = ((unsigned char*)ptr);
    int i;
    printf("[%s] \r\n[%04d] \n", name, len);

    for(i=0; i < len; i++)
    {
        if(!(i % 0x10))
            printf("\n");

        printf("%02x ", tx_data[i]);
    }
    printf("\n\n");
}

void dump_send_data(void *p, int len, char* name)
{
    sdio_pkt_header_t *sdio_pkt_header = (sdio_pkt_header_t *)p;

    dump_hex_data((unsigned char*)sdio_pkt_header, len, name);

	//printf("dump_send_data: seq = %d\r\n",sdio_pkt_header->seq);
	//printf("dump_send_data: cmd = 0x%X\r\n",sdio_pkt_header->iot_pkt_header.cmd);
	//printf("dump_send_data: pkt_len = %d\r\n",sdio_pkt_header->iot_pkt_header.pkt_len);
}

void dump_reg_desc(uint32_t reg, int bits)
{
    //printf("Dump ISR: 0x%X.\n", reg);
    for (int i = 0 ; i <= bits - 1; i++) {

        if (reg & (1 << i)) {
            //printf("[Num %d. Bit] ", i);
            switch (i) {
                case TX_DONE_INT:
                    //printf("WTSR0 or WTSR1 is not 0\n");
                    break;

                case RX0_DONE_INT:
                    //printf("Any of the RX length data of RX0 is existed in HIF RX length FIFO\n");
                    break;

                case RX1_DONE_INT:
                    //printf("Any of the RX length data of RX1 is existed in HIF RX length FIFO\n");
                    break;

                case RX2_DONE_INT:
                    //printf("Any of the RX length data of RX2 is existed in HIF RX length FIFO\n");
                    break;

                case RX3_DONE_INT:
                    //printf("Any of the RX length data of RX3 is existed in HIF RX length FIFO\n");
                    break;

                case RX4_DONE_INT:
                    //printf("[%d] RX4 useless\n", i);
                    break;

                case ABNORMAL_INT:
                    {
                        struct sdio_func *func = mt7682.func;
                        uint32_t reg = 0xFF;
                        int whisr = -1;

                        whisr = sdio_func1_rd(func, SDIO_IP_WASR, &reg, 4);

                        printf("[%d] The abnormal Data overflow of WLAN TX0 and TX1 port, WASR(0x%X) descript about %s.\n\n",
                                i, reg,
                               ((reg & 0x1) ? "Data overflow of WLAN TX0 port" :
                                (reg & 0x2) ? "Data overflow of WLAN TX1 port" :
                                (reg & 0x100) ? "Data underflow of WLAN RX0 port" :
                                (reg & 0x200) ? "Data underflow of WLAN RX1 port" :
                                (reg & 0x400) ? "Data underflow of WLAN RX2 port" :
                                (reg & 0x800) ? "Data underflow of WLAN RX3 port" :
                                (reg & 0x10000) ? "It is purely for host driver debug purpose" : "None"));
                    }
                    break;

                case FW_OWN_BACK_INT:
                    printf("[%d] Firmware has returned the ownership to host driver. This field is set with driver own-back only.\n\n", i);
                    break;
            }
        }

    }
}

#endif


