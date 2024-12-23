#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include "ite/itp.h"

extern int uvc_main();

void LCD(void)
{
    uint16_t       *addr = (uint16_t *) ithLcdGetBaseAddrA();
    uint32_t       col = ithLcdGetWidth();
    uint32_t       row = ithLcdGetHeight();
    uint32_t       x, y, i = 0;
    const uint16_t colors[] = { ITH_RGB565(255, 0, 0), ITH_RGB565(0, 255, 0), ITH_RGB565(0, 0, 255) };

    ioctl(ITP_DEVICE_SCREEN,    ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET,      NULL);

    uint16_t *base = ithMapVram((uint32_t) addr, ithLcdGetPitch() * ithLcdGetHeight(), ITH_VRAM_WRITE);
    uint16_t color = colors[0];
    uint16_t *ptr  = base;
    for (y = 0; y < row; y++)
        for (x = 0; x < col; x++)
            *ptr++ = color;
    ithFlushDCacheRange(base, row * col * 2);
}

void *TestFunc(void *arg)
{
    // init pal
    malloc_stats();

    itpInit();

    malloc_stats();
    LCD();
    usleep(2 * 1000 * 1000);
    uvc_main();

    return 0;
}
