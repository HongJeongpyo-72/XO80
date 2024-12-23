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
#define ERASE_START 1
#define ERASE_SIZE  (512 * 1024 * 1024 / 512)  // 512M, in sector
    int rc = 0;
    int fd = -1;
    uint32_t blocksize, pos;
    static uint8_t buf[512];
    ITPSdErase erase_param;

    /* fill pattern */
    memset((void*)buf, 0x55, 512);

    fd = open(":sd0", O_RDWR, 0);
    if (fd == -1)
    {
        printf("open device error: %d\n", fd);
        goto end;
    }

    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        printf("get block size error\n");
        goto end;
    }

    /* write pattern to sector 0 */
    pos = 0;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    write(fd, buf, 1);
    /* write pattern to sector 1 */
    pos = 1;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    write(fd, buf, 1);
    /* write pattern to sector (ERASE_SIZE - 1) */
    pos = (ERASE_SIZE - 1);
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    write(fd, buf, 1);
    /* write pattern to sector ERASE_SIZE */
    pos = ERASE_SIZE;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    write(fd, buf, 1);

    /* dump sector  0 */
    pos = 0;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  1 */
    pos = 1;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  (ERASE_SIZE - 1) */
    pos = (ERASE_SIZE - 1);
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  ERASE_SIZE */
    pos = ERASE_SIZE;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);


    /* do erase */
    printf("do erase: sector %d ~ %d \n", ERASE_START, ERASE_START + ERASE_SIZE - 1);
    erase_param.pos = ERASE_START;
    erase_param.count = ERASE_SIZE;
    if (ioctl(ITP_DEVICE_SD0, ITP_IOCTL_ERASE, &erase_param))
    {
        printf("erase fail \n"); 
        goto end;
    }

    /* read back */
    /* dump sector  0 */
    pos = 0;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  1 */
    pos = 1;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  (ERASE_SIZE - 1) */
    pos = (ERASE_SIZE - 1);
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    /* dump sector  ERASE_SIZE */
    pos = ERASE_SIZE;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    printf("sector %d: \n", pos);
    dump_block((uint32_t)buf, 16);
    
end:
    while (1);
}

static void Test_Reinit(void)
{
    static uint8_t buf[512] = { 0 };
    int fd = -1;
    uint32_t blocksize, pos;

    fd = open(":sd0", O_RDWR, 0);
    if (fd == -1)
    {
        printf("open device error: %d\n", fd);
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        printf("get block size error\n");
        goto end;
    }

    /* read pattern to sector 0 */
    pos = 0;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    read(fd, buf, 1);
    dump_block((uint32_t)buf+510, 2);

    /* sleep to remove card for test. */
    printf("please remove card. \n");
    sleep(2);
    printf("sleep 5 second...\n");
    sleep(5);
    memset((void*)buf, 0, 512);
    read(fd, buf, 1);
    dump_block((uint32_t)buf + 510, 2);
    printf("please insert sd card! \n");
    sleep(5);

    /* EXIT */
    close(fd);
    fd = -1;
    ioctl(ITP_DEVICE_SD0, ITP_IOCTL_EXIT, NULL);
    printf("SD0: ioctl EXIT! \n");

    /* Re-init SD0 */
    printf("\nRe-init SD0!!\n");
    ioctl(ITP_DEVICE_SD0, ITP_IOCTL_INIT, NULL);

    fd = open(":sd0", O_RDWR, 0);
    if (fd == -1)
    {
        printf("open device error: %d\n", fd);
        goto end;
    }
    if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &blocksize))
    {
        printf("get block size error\n");
        goto end;
    }

    /* read pattern to sector 0 */
    pos = 0;
    if (lseek(fd, pos, SEEK_SET) != pos) { printf("seek to position %d error\n", pos); goto end; }
    memset((void*)buf, 0, 512);
    read(fd, buf, 1);
    dump_block((uint32_t)buf + 510, 2);

end:
    while (1);
}


void* TestFunc(void* arg)
{
    itpInit();

#if 1
    Test_Erase();
#else
    Test_Reinit();
#endif

    return NULL;
}

