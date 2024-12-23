#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "mmp_capture.h"
#ifdef __OPENRTOS__
    #include "openrtos/FreeRTOS.h"
    #include "openrtos/queue.h"
#endif

typedef enum state_control
{
    NO_SIGNAL                  = 0,
    NO_SIGNAL_TO_SIGNAL_STABLE = 1,
    SIGNAL_STABLE              = 2,
} state_control;

static CAPTURE_HANDLE gCapDev0 = {0};
static bool           cap0_get_err = false;
state_control         state        = NO_SIGNAL;

int  test_count  = 5; 

static void
FSM_STATE(bool signal)
{
    if (signal)
    {
        if (state == NO_SIGNAL)
            state = NO_SIGNAL_TO_SIGNAL_STABLE;
        else if (state == NO_SIGNAL_TO_SIGNAL_STABLE)
            state = SIGNAL_STABLE;
        else if (state == SIGNAL_STABLE)
            state = SIGNAL_STABLE;
    }
    else
    {
        state = NO_SIGNAL;
    }
}

static void
_CAP_ISR0(
    void *arg)
{
#ifdef __OPENRTOS__
    uint32_t       capture0state = 0, capindex = 0;
    BaseType_t     gHigherPriorityTaskWoken = (BaseType_t)0;
    CAPTURE_HANDLE *ptDev                   = (CAPTURE_HANDLE *) arg;

    capture0state = ithCapGetEngineErrorStatus(&gCapDev0, MMP_CAP_LANE0_STATUS);
    //ithPrintf("cap0_isr 0x%x\n", capture0state);

    if (capture0state >> 31)
    {
        if ((capture0state >> 8) & 0xF)
        {
            ithPrintf("_CAP_ISR0=0x%x\n", capture0state);
            cap0_get_err = true;
            //clear cap0 interrupt and reset error status
            ithCapClearInterrupt(&gCapDev0, MMP_TRUE);
        }
        else
        {
            //ithPrintf("cap0_isr frame end\n");
            //clear cap0 interrupt
            ithCapClearInterrupt(&gCapDev0, MMP_FALSE);
        }
        portYIELD_FROM_ISR(gHigherPriorityTaskWoken);
    }
#endif
    return;
}

#if 0
static int
_SIGNALCHECK_FIRE(
    CAPTURE_HANDLE *ptDev)
{
    int         timeout  = 0;
    CAP_CONTEXT *Capctxt = &ptDev->cap_info;

    while ((ithCapGetEngineErrorStatus(ptDev, MMP_CAP_LANE0_STATUS) & 0x3) != 0x3)
    {
        if (++timeout > 100)
            return 1;
        printf("Hsync or Vsync not stable!\n");
        usleep(10 * 1000);
    }

    ithCapFire(ptDev, MMP_TRUE);
    printf("Capture Fire! (%d)\n", ptDev->cap_id);

    return 0;
}
#endif
//Modules test Run
void *TestFunc(void *arg)
{
	int i = 0;
    printf("Scan Windos start\n");   
    itpInit();

    ithCapPowerUp();
    /*capture init*/
    ithCapInitialize();
    /*modules init*/
    ithCapDeviceInitialize();

    CAPTURE_SETTING mem_modeset = {MMP_CAP_DEV_SENSOR, MMP_FALSE,  MMP_TRUE, 1280, 720};
    ithCapConnect(&gCapDev0, mem_modeset);
    ithCapRegisterIRQ(_CAP_ISR0, &gCapDev0);

    /*main loop*/
    for (;;)
    {
        FSM_STATE(ithCapDeviceIsSignalStable());

        switch(state)
        {
            case NO_SIGNAL:
                printf("###NO_SIGNAL###\n");
                break;
            case SIGNAL_STABLE:
				for(i = 0; i < test_count; i++)
				{
		            printf("=======scan (%d) times=======\n", i);
                	ithCapSearchClkDelay(&gCapDev0);
					sleep(1);
				}
				test_count = 0;
                break;
            default:
                break;
        }                
        usleep(200*1000);
    }
}
