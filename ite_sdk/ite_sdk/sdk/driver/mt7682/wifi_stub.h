#ifndef __WIFI_STUB_CONF_H__
#define __WIFI_STUB_CONF_H__

#include <stdint.h>

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"

#include "wifi_host_os_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MT7682_4BYTE_ALIGN       __attribute__ ((__aligned__(4)))

#define MT7682_GET_INTR_FROM_GPIO       /* If undef, then get intr from standard SDIO */
//#define MT7682_GET_INTR_FROM_SDIO_POLLING

#define MT7682_RECV_CALL_FROM_ISR
#ifndef MT7682_RECV_CALL_FROM_ISR
#define MT7682_STUB_RECV_TASK
#endif


#define WFC_MEMQ_INVALID          0xFFFF

#define MT7682_IP_DATA            0x13
#define MT7682_SET_LINK_INFO      0x0D
#define MT7682_NET_IF_NUM         1

/* TEST OPETATION */
//#define Test_Stub_RW_Enable
#ifdef Test_Stub_RW_Enable
#define CREATE_SDIO_RW_TASKLET                1

#define Rx_Task 1
#define Tx_Task 0
#endif


typedef struct {
    uint32_t  length: 16;
    uint32_t  reserved: 13;
    uint32_t  tx_type: 3;
} tx_header, tx_header_t;

typedef struct {
    tx_header_t header;
    uint32_t *data;
} sdio_tx_data_t;  // 4byte tx header

typedef struct {
	uint16_t cmd;
	uint16_t pkt_len;
	uint16_t seq_num;
	uint16_t reserve;
}iot_pkt_header_t;

typedef struct{
	uint32_t seq:16;
	uint32_t pkt_num:8;
	uint32_t pkt_id:8;
	iot_pkt_header_t iot_pkt_header;
}sdio_pkt_header_t;

typedef struct {
    union {
        struct {
            uint32_t  length: 16;
            uint32_t  reserved: 13;
            uint32_t  tx_type: 3;
        } bits;
        uint32_t         asUINT32;
    } u;
} brom_sdio_tx_sdu_header_t;  // 4byte tx header

typedef struct {
	uint16_t    RxPktReady;
	uint8_t     status;
	uint8_t     type;
	uint16_t    len;
	uint16_t    seq;
    uint32_t    spim;

    void        *p;
} hal_sdio_slave_rxinfo_t;


/* === ENUM === */
typedef enum E_WHISR_DESC
{
    TX_DONE_INT = 0,
    RX0_DONE_INT,
    RX1_DONE_INT,
    RX2_DONE_INT,
    RX3_DONE_INT,
    RX4_DONE_INT,
    ABNORMAL_INT,
    FW_OWN_BACK_INT //end
} WHISR_DESC;

typedef enum E_WFC_SIM_MODE
{
    WFC_MODE_HIP_SIP  = 1,    //Host Lwip   <--> Slave Lwip
    WFC_MODE_HIP_SWIFI,       //Host Lwip   <--> Slave WIFI
    WFC_MODE_HIP_SBUF,        //Host Lwip   <--> Slave Buffer
    WFC_MODE_HBU_SBUF,        //Host Buffer <--> Slave Buffer
} T_WFC_SIM_MODE;

//#define WFC_SIM_MODE  WFC_MODE_HIP_SIP
#define WFC_SIM_MODE  WFC_MODE_HIP_SWIFI

typedef enum E_MEMQ_CTRL
{
    MEMQ_CTRL_CMD  = 1,
    MEMQ_CTRL_HOST_TX,
    MEMQ_CTRL_HOST_RX,
    MEMQ_CTRL_SLAVE_EVT,
    MEMQ_CTRL_MAX_NUM
} T_MEMQ_CTRL;

typedef enum E_WFC_CMD_TYPE
{
    WFC_CT_SYSTEM,
    WFC_CT_USER
} T_WFC_CMD_TYPE;


enum E_WFC_CMD
{
    WFC_CMD_WFC_READY,
    WFC_CMD_IP_READY,
    WFC_CMD_OPMODE,
    WFC_CMD_STA_SSID,
    WFC_CMD_STA_PASSWD,
    WFC_CMD_RELOAD,
    WFC_CMD_DTW_DATA,  /* 6. Tx */
    WFC_CMD_DTH_READY, /* 7. Rx */
    WFC_CMD_AT,
    WFC_CMD_API,
    WFC_CMD_EVT,
    WFC_CMD_BWCS,
    WFC_CMD_WAKEUP_ACK,
    WFC_CMD_OS_INFO    = 99,
    WFC_CMD_REMOTE_IPERF = 100,
    WFC_CMD_REMOTE_IPERF_CLIENT= 101,
    WFC_CMD_TASK_DEINIT = 102,
    WFC_CMD_MAX_SUPPORT = 0xFFFF
};


enum E_WFC_EVT
{
    WFC_EVT_WIFI_CONNECTED  = 0x01,
    WFC_EVT_WIFI_DISCONNECT,
    WFC_EVT_IP_READY,
    WFC_EVT_SCAN_RESULTS,
    WFC_EVT_INIT_DONE,
    WFC_EVT_TASK_DEINIT,
};


enum E_WFC_OPCODE
{
    WFC_OP_HOST_CMD,
    WFC_OP_DTH_SET,
    WFC_OP_DTH_DATA,
};

/*
enum E_WFC_OPCODE
{
    WFC_OP_WIFI_READY,
    WFC_OP_IP_READY,
    WFC_OP_OPMODE,
    WFC_OP_STA_SSID,
    WFC_OP_STA_PASSWD,
    WFC_OP_RELOAD,
    WFC_OP_STANDBY,
    //WFC_OP_TIMEOUT,
    WFC_OP_DTW_CMD,
    WFC_OP_DTW_DATA,
    WFC_OP_DTH_SET,
    WFC_OP_DTH_READY,
    WFC_OP_DTH_DATA,
    //...............
    WFC_OP_OS_INFO    = 99,
    WFC_OP_REMOTE_IPERF = 100,
    WFC_OP_MAX_SUPPORT = 0xffff,
};
*/

enum E_WFC_RSPCODE
{
    WFC_RSP_OK             = 1,
    WFC_RSP_NO_HRX,
    WFC_RSP_INVALID_OPCODE,
    WFC_RSP_QUEUE_ERR,
    WFC_RSP_TIMEOUT,
    WFC_RSP_DTH_SET,
    //WFC_RSP_DTH_DATA,
    WFC_RSP_NULL_OS_BUF,
    WFC_RSP_PARAM_ERR,
    WFC_RSP_UNKNOWN_ERR,
    WFC_RSP_DTH_DATA  = 0x8000,
    //......
};

typedef struct S_WFC_QBUF
{
    uint32_t   ctrl;
    uint32_t   qidx;
    uint32_t   size;
    uint8_t   *ptr;
    struct S_WFC_QBUF *next;
} T_WFC_QBUF;


typedef struct S_WFC_CONFIG
{
    uint8_t         type;
    uint16_t        op;
    uint8_t         auto_rsp;
} T_WFC_CONFIG;


typedef struct S_WFC_OPCODE
{
    uint16_t  opcode;
    uint16_t  opcmd;
    uint16_t *rspcode;
    uint8_t  *txptr;
    uint32_t  txsize;
    uint8_t  *rxptr;
    uint32_t  rxsize;
    uint32_t *rspsize;

    T_WFC_CONFIG *pwfg;
    T_WFC_QBUF   qbuf;
    struct S_WFC_OPCODE *next;
} T_WFC_OPCODE;


typedef struct S_WFC_EVT
{
    uint16_t  eid;
    uint8_t  *param;
    uint32_t  size;
} T_WFC_EVT;

typedef struct S_WFC_IP_OP
{
    uint16_t  opcode;
    uint8_t  *ptr;
    uint32_t  size;
} T_WFC_IP_OP;


uint16_t wfcm_to_slave(uint16_t op, void *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR);

int  wifi_host_wfcm_init(void);
void wifi_stub_task(void);
void wifi_stub_recv_task(void);

void dump_send_data(void *p, int len, char* name);
void dump_hex_data(void* ptr, int len, char* name);
void dump_reg_desc(uint32_t reg, int bits);

#ifdef Test_Stub_RW_Enable
int sdio_create_rw_tasklet(void);
#endif


//Low level input
//extern void mt7682_low_level_input(struct netif *netif, struct pbuf *p);


#ifdef __cplusplus
}
#endif

#define DEBUG_ON 1

#endif  /* __WIFI_STUB_CONF_H__ */
