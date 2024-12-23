#include "sdio_io.h"
#include "wifi_stub.h"
#include "ite/ite_wifi.h"

#ifdef Test_Stub_RW_Enable
/* Host R/W test */
#define TEST_BLOCK_NUMBER  (1)
#define TEST_SEND_BYTES    (32)

MT7682_4BYTE_ALIGN uint32_t tx_buf[1024]; // 4(uint32_t) * 1024 = 4096Bytes
MT7682_4BYTE_ALIGN uint32_t rx_buf[1024*2];


extern bool sdio_send_pkt(struct sdio_func *func, unsigned char tx_que, uint8_t *tx_buf, int data_length);
extern bool sdio_receive_pkt(struct sdio_func *func, uint8_t *rx_buf);
/* ========================================================== */
extern struct mt7682_sdio_st  mt7682; //Defined this struct in sdio_main.c
extern msdc_config_t msdc_config;

#if CREATE_SDIO_RW_TASKLET
int sdio_rx_process(struct sdio_func *func)
{
    hal_sdio_status_t status;
    u32 whisr_reg = 0x0;
    u32 buff_len = 0;
    bool rx_exit = false;
    int i;

    sdio_func1_rd(func, SDIO_IP_WASR, &whisr_reg, 4);
    printf("Rx_Test: sdio receive data start(0x%X) =====>\r\n", whisr_reg);

    while (!rx_exit) {
        //whisr_reg = sdio_read_isr();
        sdio_func1_rd(func, SDIO_IP_WHISR, &whisr_reg, 4);
        printf("Rx_Test: whisr_reg(0x%X)\r\n", whisr_reg);

        dump_reg_desc(whisr_reg, 8);

        if (whisr_reg & RX0_REG_MASK && !(whisr_reg & ABNORMAL_MASK)) {
            memset(rx_buf, 0, sizeof(rx_buf));
            if (false == sdio_receive_pkt(func, (void *)rx_buf)) {
            printf("Rx_Test: sdio host receive error!\r\n");
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

            dump_hex_data((void *)rx_buf, 512, "test pkt");
            printf("\n");

            //rx_exit = true;
            //usleep(500*1000);
            printf("Rx_Test: <====sdio host receive ok.\r\n\n");
        }

        usleep(10*1000);
    }
    printf("Rx_Test: sdio receive data end by ERROR <=====\r\n");

    return -1;
}


int sdio_tx_process(struct sdio_func *func)
{
    hal_sdio_status_t status;
    tx_header *tx_buffer_header;
    u32 *tx_buffer;
    u32 whisr_reg = 0xFF;
    u32 buff_len = 0;
    bool tx_exit = false;
    int i;

    sleep(1);
    printf("Tx_Test: sdio host send start =====>\r\n");

    while(!tx_exit) {
        whisr_reg = sdio_read_isr();

    if (whisr_reg & TX_REG_MASK) {
        /*tx done*/
            memset(&tx_buf, 0, sizeof(tx_buf));

        tx_buffer_header = (tx_header *)tx_buf;
        tx_buffer        = (u32 *)tx_buf;

            tx_buffer_header->length = (sizeof(uint32_t) * TEST_BLOCK_NUMBER * TEST_SEND_BYTES) + sizeof(tx_header);
        tx_buffer_header->reserved = 0x1FFE;
        tx_buffer_header->tx_type = 0x7;
        tx_buffer += (sizeof(tx_header) / 4);

            printf("====>size: tx_buf(%d), tx_header(%d), tx_buffer_header->length(%d)\n",
                sizeof(tx_buf), sizeof(tx_header), tx_buffer_header->length);

            //dump_send_data((void *)tx_buffer_header, 512*TEST_BLOCK_NUMBER, "tx_buffer_header");

            for (i = 0; i < (TEST_SEND_BYTES * TEST_BLOCK_NUMBER); i++) {
                *tx_buffer++ = (0xaabbcc00 + i);
        }

            dump_send_data((void *)tx_buf, tx_buffer_header->length, "sdio_tx_process: tx_buf");

        if(false == sdio_send_pkt(func, 0, (uint8_t *)&tx_buf, tx_buffer_header->length)) {
            printf("Tx_Test: sdio host send error!\r\n");
                tx_exit = true;
        }

            printf("Tx_Test: sdio host send ok (%d bytes).\r\n", i*sizeof(uint32_t) + sizeof(tx_header));
            //usleep(100*1000);
        }
    }
    printf("Tx_Test: sdio host send end by ERROR <=====\r\n");

    return -1;
}


#else

int sdio_rw_process(struct sdio_func *func)
{
    int                 i = 0;
    hal_sdio_status_t   status;
    tx_header           *tx_buffer_header;
    uint32_t            *tx_buffer;
    uint32_t            whisr_reg = 0xFF;

#if Rx_Task

    while(!sdio_get_interrupt_flag());
    sdio_reset_interrupt_flag();

    printf("[Test Host] sdio receive data start....\r\n");

    /*read whisr*/
    //sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
    whisr_reg = sdio_read_isr();

    if (whisr_reg & RX0_REG_MASK) {
       /*rx done*/
        memset(&rx_buf, 0, sizeof(rx_buf));
        if (false == sdio_receive_pkt(func, (uint8_t *)&rx_buf)) {
            printf("[Test Host] sdio host receive error!\r\n");
            return -3;
        }

        for (i = 0; i < (128 * TEST_BLOCK_NUMBER); i++) {
            dump_hex_data((void *)rx_buf, 512, "test pkt");
        }
       printf("[Test Host] sdio host receive done.\r\n");
    }

#endif

#if Tx_Task

    while(!sdio_get_interrupt_flag());
    sdio_reset_interrupt_flag();

    /*read whisr*/
    //sdio_func1_rd(func, SDIO_IP_WHISR, (uint8_t *)&whisr_reg, 4);
    whisr_reg = sdio_read_isr();

    do {
        printf("/=========================>/\n[Test Host] sdio send data start(0x%X)....\r\n", whisr_reg);
        /*tx done*/
        memset(&tx_buf, 0, sizeof(tx_buf));
        tx_buffer_header = (tx_header *)tx_buf;
        tx_buffer = (uint32_t *)tx_buf;
        tx_buffer_header->length = (512 * TEST_BLOCK_NUMBER) + sizeof(tx_header);
        tx_buffer += (sizeof(tx_header) / 4);
        for (i = 0; i < (128 * TEST_BLOCK_NUMBER); i++) {
        *tx_buffer++ = (0x0 + i);
        }
        dump_send_data((void *)tx_buf, (512 * TEST_BLOCK_NUMBER) + sizeof(tx_header), "tx_buf");
        if(false == sdio_send_pkt(func, 0, (uint8_t *)tx_buf, (512 * TEST_BLOCK_NUMBER) + sizeof(tx_header))) {
            printf("[Test Host] sdio host send error!\r\n");
            return -5;
        } else {
        //printf("[Test Host] sdio host send succes!\r\n");
        }

        printf("[Test Host] sdio host send done.\r\n/<=========================/\n\n");
    }while (whisr_reg == TX_REG_MASK);
#endif
    return 0;
}

#endif
#endif
/* ========================================================== */


