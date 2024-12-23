#ifndef __WFCM_SDIO_H__
#define __WFCM_SDIO_H__

#include <stdint.h>
#include "wifi_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

int wfcm_if_sdio_init(struct sdio_func *func);
void wfcm_if_sdio_deinit(void);

void wfcm_mq_free(T_WFC_QBUF *qbuf);
int32_t wfcm_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx);

void wfcm_mem_align_clr(uint8_t *pbuf, uint32_t bufSize);
void wfcm_sdio_cmd_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *param, uint32_t paramSize, uint8_t forISR);
void wfcm_memq_get_qbuf(T_WFC_QBUF *qbuf);
bool wfcm_sdio_send_data(struct sdio_func *func, T_WFC_QBUF *qbuf, uint8_t qbuf_release);
bool wfcm_sdio_receive_cmdrsp(struct sdio_func *func, uint8_t *rx_buf, uint32_t *bufSize);
bool wfcm_sdio_receive_data(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize);
void wfcm_sdio_htx_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *htxptr, uint32_t htxSize);
void wfcm_dump_buf(uint8_t *pbuf, uint32_t bufSize);
bool wfcm_sdio_receive_data_size(struct sdio_func *func, uint32_t *bufSize);
bool wfcm_sdio_receive_data_payload(struct sdio_func *func, uint8_t *pbuf, uint32_t bufSize);


#ifdef __cplusplus
}
#endif


#endif  /* __WFCM_SDIO_H__ */
