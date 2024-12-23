#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "fat/api/api_fat.h"
#include "ite/itp.h"
#include "ite/ite_sd.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"


//#define LOW_LEVEL_TEST

#if defined(LOW_LEVEL_TEST)
void SD_LowLevelTest(void);
#endif

#if defined(WIN32)
#define err()   do { while(1); }while(0)
#else
#define err()  do { vTaskSuspendAll(); while(1); } while(0)
#endif


extern F_DRIVER * mmcsd_initfunc(unsigned long driver_param);

static uint32_t fileNum[2];
static uint8_t  filePath[2][128];

#define LOOP_NUM    8*2
#define BUF_SIZE    128*1024

static uint8_t*  pattern[2];
static uint8_t*  readBuf[2];

/** in itp_fat.c */
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


void SD_FileReadWriteTest(int taskId)
{
    int res;
    uint32_t i, diff;
    FN_FILE* file = NULL;
//if(taskId==0) return; // 0: disable sd0, 1: disable sd2
    //sprintf(filePath[taskId], "%s:\\sd_%04X.dat", (taskId ? "B" : "A"), fileNum[taskId]++);
    sprintf(filePath[taskId], "%s:\\sd_%04X.dat", (taskId ? "B" : "A"), fileNum[taskId]);
    printf("%s \n", filePath[taskId]);

    pattern[taskId] = malloc(BUF_SIZE+32)+1;
    readBuf[taskId] = malloc(BUF_SIZE+32)+2;
    printf(" %X %X \n", pattern[taskId], readBuf[taskId]);

    /***************/
    /* initialize  */
    /***************/
#if 1//defined(CFG_MMC_ENABLE)
	{
		SD_CARD_INFO card_info = { 0 };
		iteSdcInitialize(taskId, &card_info);
		if ((card_info.type != SD_TYPE_MMC) && (card_info.type != SD_TYPE_SD)) {
			printf("iteSdcInitialize() no available card type! %d \n", card_info.type);
			while (1);
		}
	}
#endif
	iteSdInitializeEx(taskId);
    res = f_initvolume(taskId, mmcsd_initfunc, (unsigned long)&sdDrvParams[taskId]);
    if(res)
    {
        printf(" T-%s: initvolume fail! \n", (taskId ? "B" : "A"));
        err();
    }
    printf(" T-%s: initvolume ok! \n", (taskId ? "B" : "A"));

    for(i=0; i<BUF_SIZE; i++) /** fill pattern */
        pattern[taskId][i] = (fileNum[taskId]+i) % 0x100;

    /***************/
    /* Write       */
    /***************/
    file = f_open(filePath[taskId], "w");
    if(!file)
    {
        printf(" T-%s: open %s for write fail! \n\n", (taskId ? "B" : "A"), filePath[taskId]);
        err();
    }
    printf(" T-%s: write data.... \n", (taskId ? "B" : "A"));
    for(i=0; i<LOOP_NUM; i++)
    {
		printf("T-%s:@\n", (taskId ? "B" : "A"));
        res = f_write(pattern[taskId], 1, BUF_SIZE, file);
        if(res != BUF_SIZE)
        {
            printf(" T-%s: real write size 0x%X != write size 0x%X \n", (taskId ? "B" : "A"), res, BUF_SIZE);
            err();
        }
    }
    res = 0;
    f_flush(file);
    printf(" T-%s: Write finish!\n", (taskId ? "B" : "A"));
    if(file)
    {
        f_close(file);
        file = NULL;
    }

    /*******************/
    /* Read & Compare  */
    /*******************/
    printf(" T-%s: compare data! \n", (taskId ? "B" : "A"));
    file = f_open(filePath[taskId], "r");
    if(!file)
    {
        printf(" T-%s: open %s for read fail! \n\n", (taskId ? "B" : "A"), filePath[taskId]);
        err();
    }
	printf(" T-%s: read data from card.. \n", (taskId ? "B" : "A"));
    for(i=0; i<LOOP_NUM; i++)
    {
        printf("T-%s:#\n", (taskId ? "B" : "A"));
        memset(readBuf[taskId], 0x55, BUF_SIZE);
        res = f_read(readBuf[taskId], 1, BUF_SIZE, file);
        if(res != BUF_SIZE)
        {
            printf(" T-%s: read back size 0x%X != read size 0x%X \n", (taskId ? "B" : "A"), res, BUF_SIZE);
            err();
        }
        diff = memcmp((void*)pattern[taskId], (void*)readBuf[taskId], BUF_SIZE);
        if(diff)
            printf(" T-%s: diff = %d, readBuf = 0x%X \n", (taskId ? "B" : "A"), diff, readBuf[taskId]);

        if(diff)
		{
			uint32_t j = 0, count=0;
			printf(" i = %d \n", i);
			for(j=0; j<BUF_SIZE; j++)
			{
				if(pattern[taskId][j] != readBuf[taskId][j])
                {
					printf(" T-%s: write buffer[%X] = %02X, read buffer1[%X] = %02X \n", (taskId ? "B" : "A"), j, pattern[taskId][j], j, readBuf[taskId][j]);
                    //if(count++ > 50)
                    //    while(1);
                }
			}
			err();
		}
    }
    res = 0;
    printf("T-%s: Compare Data End!!\n\n\n", (taskId ? "B" : "A"));
    if(file)
    {
        f_close(file);
        file = NULL;
    }
    res = f_delvolume(taskId);
    if(res)
        printf(" delvolume fail! \n");

    free(pattern[taskId]-1);
    free(readBuf[taskId]-2);

    iteSdcTerminate(taskId);
    //usleep(2*1000*1000);
}

static void* TaskFunc(void* arg)
{
    int taskId = *(int*)arg;
    uint32_t loopCnt = 0;

    f_enterFS(); 

    while(1)
    {
        if(ithCardInserted((taskId==SD_0) ? ITH_CARDPIN_SD0 : ITH_CARDPIN_SD1))
        {
            loopCnt++;
            SD_FileReadWriteTest(taskId);
        }
        usleep(10*1000);
    }
}

void* TestFunc(void* arg)
{
#define TEST_STACK_SIZE 10240
    pthread_t task[2];
    pthread_attr_t attr[2];
    int i;
    int taskId[2];

#if defined(LOW_LEVEL_TEST)
	SD_LowLevelTest();
	printf("stop and while(1)\n");
	while (1);
#endif


    fs_init(); 
    fs_start();

    for(i=0; i<2; i++)
    {
        taskId[i] = i;
        pthread_attr_init(&attr[i]);
        pthread_attr_setstacksize(&attr[i], TEST_STACK_SIZE);
        pthread_create(&task[i], &attr[i], TaskFunc, &taskId[i]);
    }

    while(1) 
        sleep(1);
}


#if defined(LOW_LEVEL_TEST)

void SD_LowLevelTest(void)
{
	static uint8_t buf0[512] = { 0 };
	static uint8_t buf1[512] = { 0 };
#if 1//defined(CFG_MMC_ENABLE)
	{
		SD_CARD_INFO card_info = { 0 };
		iteSdcInitialize(0, &card_info);
		if ((card_info.type != SD_TYPE_MMC) && (card_info.type != SD_TYPE_SD)) {
			printf("iteSdcInitialize() no available card type! %d \n", card_info.type);
			while (1);
		}
	}
	printf("... \n");

	{
		SD_CARD_INFO card_info = { 0 };
		iteSdcInitialize(1, &card_info);
		if ((card_info.type != SD_TYPE_MMC) && (card_info.type != SD_TYPE_SD)) {
			printf("iteSdcInitialize() no available card type! %d \n", card_info.type);
			while (1);
		}
	}
#endif

	iteSdInitializeEx(0);
	iteSdInitializeEx(1);
	iteSdReadMultipleSectorEx(0, 0, 1, buf0);
	iteSdReadMultipleSectorEx(1, 0, 1, buf1);
	iteSdWriteMultipleSectorEx(0, 0, 1, buf0);
	iteSdWriteMultipleSectorEx(1, 0, 1, buf1);

}

#endif
