#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "ite/ite_sd.h"



#define TEST_SD_NUM     CFG_SD_INDEX
//#define USB_IP_Pulse_Latch_Offset

#define SD_BASE     ((TEST_SD_NUM == 0) ? ITH_SD0_BASE : ITH_SD1_BASE)

static void scan_window(void);

void* TestFunc(void* arg)
{
    scan_window();
    while (1);
}



static uint8_t data[512*512];

#define K  (1024/512)   // bytes to sector
static uint32_t rSector[10] = { 0 * K, 256 * K, 512 * K, 800 * K, 1024 * K, 1200 * K, 1400 * K, 1800 * K, 2000 * K, 2100 * K };
static uint32_t rSectorCnt[10] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

#if defined(USB_IP_Pulse_Latch_Offset)
#define DELAY_CNT   (0x4+1)
#else
#define DELAY_CNT   (4)
#endif
#define CLK_CNT     2

struct success_info {
    int success;
    uint32_t success_cunt;
};
static struct success_info info[CLK_CNT][DELAY_CNT];

static int do_test(uint32_t *success_cnt)
{
#define TEST_SIZE       4*1024*1024
    int index, i, loopCnt, rc = 0;
    int success = 1;

    for (index = 0; index < 10; index++)
    {
        loopCnt = TEST_SIZE / (rSectorCnt[index] * 512);

        for (i = 0; i < loopCnt; i++) {
            rc = iteSdReadMultipleSectorEx(TEST_SD_NUM, rSector[index], rSectorCnt[index], data);
            if (rc) {
                success = 0;
                goto end;
            }

            rc = iteSdWriteMultipleSectorEx(TEST_SD_NUM, rSector[index], rSectorCnt[index], data);
            if (rc) {
                success = 0;
                goto end;
            }
            (*success_cnt)++;
        }
    }
end:
    return success;
}

static void scan_window(void)
{
    int clk_idx, delay_idx;
    uint32_t timing;
    uint32_t delay_offset;

#if 1//defined(CFG_MMC_ENABLE)
{
    SD_CARD_INFO card_info = { 0 };

    iteSdcInitialize(TEST_SD_NUM, &card_info);
	if ((card_info.type != SD_TYPE_MMC) && (card_info.type != SD_TYPE_SD)) {
        printf("iteSdcInitialize() no available card type! %d \n", card_info.type);
        while (1);
    }
}
#endif
	iteSdInitializeEx(TEST_SD_NUM);

    for (clk_idx = 0; clk_idx < CLK_CNT; clk_idx++) {
        for (delay_idx = 0; delay_idx < DELAY_CNT; delay_idx++) {
        #if defined(USB_IP_Pulse_Latch_Offset)
            timing = (clk_idx << 16) | (delay_idx << 8);
            ithWriteRegMaskA((SD_BASE + 0x100), timing, 0x00013F00);
            printf("0x%08X = 0x%08X \n", (SD_BASE + 0x100), ithReadRegA(SD_BASE + 0x100));
        #else
            timing = (clk_idx << 16);
            ithWriteRegMaskA((SD_BASE + 0x100), timing, 0x00010000);
            printf("0x%08X = 0x%08X \n", (SD_BASE + 0x100), ithReadRegA(SD_BASE + 0x100));

            delay_offset = (TEST_SD_NUM == 0) ? 16 : 18;
            ithWriteRegMaskA(0xD8400014, (delay_idx << delay_offset), (0x3 << delay_offset));
            printf("0xD8400014 = 0x%08X \n", ithReadRegA(0xD8400014));
        #endif

            info[clk_idx][delay_idx].success = do_test(&info[clk_idx][delay_idx].success_cunt);
        }
    }
    printf("\n\n  \n\n");

    for (clk_idx = 1; clk_idx >= 0; clk_idx--) {
        for (delay_idx = 0; delay_idx < DELAY_CNT; delay_idx++) {
            printf("edge_sel: %d, delay: 0x%02X ==> %s (%d) \n", clk_idx, delay_idx,
                (info[clk_idx][delay_idx].success ? "Pass" : "Fail"),
                info[clk_idx][delay_idx].success_cunt);
        }
    }

    printf("\n\n Scan Done!!! \n\n");

    return;
}
