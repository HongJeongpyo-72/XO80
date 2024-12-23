#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "fat/api/api_fat.h"
#include "ite/itp.h"
#include "ite/ith.h"
#include "ite/ite_sd.h"

//#define TEST_ERASE
//#define LOW_LEVEL_PERFORMANCE
//#define TEST_UNALIGN
#if defined(TEST_UNALIGN)
#define UNALIGN_OFFSET_SRC		1
#define UNALIGN_OFFSET_DST		2
#else
#define UNALIGN_OFFSET_SRC		0
#define UNALIGN_OFFSET_DST		0
#endif

#define TEST_SD_NUM     CFG_SD_INDEX

extern F_DRIVER * mmcsd_initfunc(unsigned long driver_param);

static uint32_t fileNum;// = 0xB6;
static uint8_t  filePath[128];

#define LOOP_NUM    8*2
#define BUF_SIZE    512*1024
//#define LOOP_NUM    8*2

struct
{
    int sd;                 // sd0 or sd1
    int removable;          // removable or not
    unsigned long reserved; // reserved size
} sdDrvParams[2] =
{
    {
        0,
    #ifdef CFG_SD0_STATIC
        0,
        CFG_SD0_RESERVED_SIZE
    #else
        1,
        0
    #endif // CFG_SD0_STATIC
    },
    {
        1,
    #ifdef CFG_SD1_STATIC
        0,
        CFG_SD1_RESERVED_SIZE
    #else
        1,
        0
    #endif // CFG_SD1_STATIC
    },
};

void SD_FileReadWriteTest(void)
{
    int res;
    uint32_t i, diff;
    FN_FILE* file = NULL;
    uint8_t*  pattern = NULL;
    uint8_t*  readBuf = NULL;

	pattern = (uint8_t*)malloc(BUF_SIZE + 128) + UNALIGN_OFFSET_SRC;
	readBuf = (uint8_t*)malloc(BUF_SIZE + 128) + UNALIGN_OFFSET_DST;
	if(!pattern || !readBuf)
	{
		printf(" [SD] pattern %p, readBuf %p \n", pattern, readBuf);
		while(1);
	}

    /***************/
    /* initialize  */
    /***************/
    fs_init(); 
    fs_start();
    f_enterFS(); 
    
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
    res = f_initvolume(0, mmcsd_initfunc, (unsigned long)&sdDrvParams[TEST_SD_NUM]);
    if(res)
    {
        printf(" initvolume fail! \n");
        while(1);
    }
    for(i=0; i<BUF_SIZE; i++) /** fill pattern */
        pattern[i] = (fileNum+i) % 0x100;

    /***************/
    /* Write       */
    /***************/
    file = f_open(filePath, "w");
    if(!file)
    {
        printf(" open %s for write fail! \n\n", filePath);
        while(1);
    }
    printf(" write data.... \n");
    for(i=0; i<LOOP_NUM; i++)
    {
		printf("@\n");
        res = f_write(pattern, 1, BUF_SIZE, file);
        if(res != BUF_SIZE)
        {
            printf(" real write size 0x%X != write size 0x%X \n", res, BUF_SIZE);
            while(1);
        }
    }
    res = 0;
    f_flush(file);
    printf(" Write finish!\n");
    if(file)
    {
        f_close(file);
        file = NULL;
    }

    /*******************/
    /* Read & Compare  */
    /*******************/
    printf(" compare data! \n");
    file = f_open(filePath, "r");
    if(!file)
    {
        printf(" open %s for read fail! \n\n", filePath);
        while(1);
    }
	printf(" read data from card.. \n");
    for(i=0; i<LOOP_NUM; i++)
    {
		printf("@\n");
        memset(readBuf, 0x55, BUF_SIZE);
        res = f_read(readBuf, 1, BUF_SIZE, file);
        if(res != BUF_SIZE)
        {
            printf(" read back size 0x%X != read size 0x%X \n", res, BUF_SIZE);
            while(1);
        }
        diff = memcmp((void*)pattern, (void*)readBuf, BUF_SIZE);
        if(diff)
			printf(" diff = %d, readBuf: 0x%X \n", diff, readBuf);

        if(diff)
		{
			uint32_t j = 0, count=0;
			printf(" i = %d, w_buf:%p, r_buf:%p [SD]\n", i,pattern,readBuf);
			for(j=0; j<BUF_SIZE; j++)
			{
				if(pattern[j] != readBuf[j])
                {
					printf(" write buffer[%X] = %02X, read buffer1[%X] = %02X \n", j, pattern[j], j, readBuf[j]);
                    if(count++ > 50)
                        while(1);
                }
			}
            while(1);
		}
    }
    res = 0;
    printf("Compare Data End!!\n\n\n");
    if(file)
    {
        f_close(file);
        file = NULL;
    }
    res = f_delvolume(0);
    if(res)
        printf(" delvolume fail! \n");

	free(pattern - UNALIGN_OFFSET_SRC);
	free(readBuf - UNALIGN_OFFSET_DST);

    iteSdcTerminate(TEST_SD_NUM);
    //usleep(500*1000);
}

static void* Func_Perf(void);
static void* Test_Erase(void);

void* TestFunc(void* arg)
{
    /* include fs_init(); fs_start(); f_enterFS(); iteSdInitiazlie(); */
    //itpInit();

#if defined(LOW_LEVEL_PERFORMANCE)
	Func_Perf();
#elif defined(TEST_ERASE)
    Test_Erase();
#else
    while(1)
    {
        //sprintf(filePath, "A:\\sd_%04X.dat", fileNum++);
		sprintf(filePath, "A:\\sd_%04X.dat", TEST_SD_NUM);
        printf("%s, loop %d \n", filePath, fileNum++);
        SD_FileReadWriteTest();
    }
#endif

    return NULL;
}



static uint8_t data[512*512];
static uint8_t data1[512*512];
#define MB  1024
static uint32_t rSector[10] = { 200*MB, 200*MB, 200*MB, 200*MB, 200*MB, 200*MB, 200*MB, 200*MB, 200*MB, 200*MB };
static uint32_t rSectorCnt[10] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

void _timerStart(ITHTimer timer)
{
	ithTimerReset(timer);
	ithTimerCtrlEnable(timer, ITH_TIMER_UPCOUNT);
	ithTimerIsrEnable(timer);
	ithTimerSetLoad(timer, 0);
	ithTimerSetCounter(timer, 0);
	ithTimerEnable(timer);
}

uint32_t _timerStop(ITHTimer timer)
{
	ithTimerDisable(timer);
	return ithTimerGetTime(timer);
}
static void* Func_Perf(void)
{
#define TEST_SIZE       8*1024*1024
    int index, i, loopCnt;
    uint32_t time;

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

    usleep(2*1000*1000);

    printf("\n\n Read perfomance.....\r\n");
    for(index=0; index<10; index++)
    {
        loopCnt = TEST_SIZE/(rSectorCnt[index]*512);

        _timerStart(ITH_TIMER3);

        for(i=0; i<loopCnt; i++)
            iteSdReadMultipleSectorEx(TEST_SD_NUM, rSector[index], rSectorCnt[index], data);
        
        time = _timerStop(ITH_TIMER3);
        printf(" %d Read: %f MBps \r\n", rSectorCnt[index], ((double)(TEST_SIZE/(1024*1024))/time)*1000000);
    }

    printf("\n\n Write perfomance.....\r\n");
    for(index=0; index<10; index++)
    {
        loopCnt = TEST_SIZE/(rSectorCnt[index]*512);
        iteSdReadMultipleSectorEx(TEST_SD_NUM, rSector[index], rSectorCnt[index], data);

        _timerStart(ITH_TIMER3);

        for(i=0; i<loopCnt; i++)
            iteSdWriteMultipleSectorEx(TEST_SD_NUM, rSector[index], rSectorCnt[index], data);
        
        time = _timerStop(ITH_TIMER3);
        printf(" %d Write: %f MBps \r\n", rSectorCnt[index], ((double)(TEST_SIZE/(1024*1024))/time)*1000000);
    }
    while(1);

    return NULL;
}

static void dump_block(uint32_t addr, uint32_t size)
{
    uint8_t *mem;
    unsigned int i, j;

    if (size == 0)
        return;

    mem = (uint8_t *)addr;

    j = 0;
    for (i = 0; i < size; ++i)
    {
        printf(" %02X", *mem);

        if (j >= 15)
        {
            printf("\r\n");
            j = 0;
        }
        else
            j++;

        addr++;
        mem++;
    }

    if (j > 0)
        printf("\r\n");

}

static void* Test_Erase(void)
{
#define ERASE_START 0
//#define ERASE_SIZE  (1 * 1024 * 1024 * 1024 / 512)  // 1G, in sector
#define ERASE_SIZE  (512 * 1024 * 1024 / 512)  // 512M, in sector
    int rc = 0;
    SD_CARD_INFO card_info = { 0 };

    memset((void*)data, 0x55, 512);

    iteSdcInitialize(TEST_SD_NUM, &card_info);
    if ((card_info.type != SD_TYPE_MMC) && (card_info.type != SD_TYPE_SD)) {
        printf("iteSdcInitialize() no available card type! %d \n", card_info.type);
        while (1);
    }

    rc = iteSdInitializeEx(TEST_SD_NUM);
    if (rc) {
        printf("iteSdInitializeEx() fail! rc = %d \n", rc);
        while (1);
    }

    /* write pattern */
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, 0, 1, data);
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, 1, 1, data);
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1), 1, data);
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 1, 1, data);
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 2, 1, data);
    iteSdWriteMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 3, 1, data);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, 0, 1, data);
    printf("sector 0: \n");
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, 1, 1, data);
    printf("sector 1: \n");
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1), 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1));
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1)+1, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1)+1);
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 2, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1) + 2);
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 3, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1) + 3);
    dump_block((uint32_t)data, 16);

    /* rease 1G, from secotr 0*/
    printf("do erase: sector %d ~ %d \n", ERASE_START, ERASE_START + ERASE_SIZE - 1);
    rc = iteSdEraseEx(TEST_SD_NUM, ERASE_START, ERASE_SIZE);
    if (rc) {
        printf("iteSdEraseEx() fail! rc = %d \n", rc);
        while (1);
    }

    /* read back check */
    iteSdReadMultipleSectorEx(TEST_SD_NUM, 0, 1, data);
    printf("sector 0: \n");
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, 1, 1, data);
    printf("sector 1: \n");
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1), 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1));
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 1, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1) + 1);
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 2, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1) + 2);
    dump_block((uint32_t)data, 16);
    iteSdReadMultipleSectorEx(TEST_SD_NUM, (ERASE_SIZE - 1) + 3, 1, data);
    printf("sector %d: \n", (ERASE_SIZE - 1) + 3);
    dump_block((uint32_t)data, 16);

    while (1);
}

