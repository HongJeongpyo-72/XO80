#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "test_itv.h"      //for VideoInit()
#include "ith/ith_video.h" //for VideoInit()
#include "capture_types.h"
#include "mmp_capture.h"
#include "hdmirx/mmp_hdmirx.h"
#include "i2s/i2s.h"
#include "wavheader.h"
#include "audio_ctrl.h"
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

typedef struct {
    int          Capinterlanced;
    unsigned int CapOutWidth;
    unsigned int CapOutHeight;
    unsigned int CapPitchY;
    unsigned int CapPitchUV;
    unsigned int CapaddrY;
    unsigned int CapaddrUV;
} CAPQUEUEINFO;

static CAPTURE_HANDLE gCapDev0;
pthread_t             display_task;
static bool           cap0_get_err = false;
state_control         state        = NO_SIGNAL;
static struct timeval pollS_time1;
#ifdef __OPENRTOS__
QueueHandle_t         CaptoDisplayQueue;
#endif

#if defined(CFG_I2S_ENABLE) && defined(CFG_AUDIO_ENABLE)

//========audio parameter==================
    #define ADC_MEMORY_SIZE (256 * 1024 * 5)
    #define ADC_BUFFER_SIZE (256 * 1024)
    #define DAC_BUFFER_SIZE (256 * 1024)
static STRC_I2S_SPEC spec_daI = {0};
static STRC_I2S_SPEC spec_adI = {0};
uint8_t              *dac_buf;
uint8_t              *adc_buf;
uint8_t              *HDMI_Ibuf[4];
uint8_t              *HDMI_Obuf[4];
unsigned char        *recdata;
uint32_t             sizecount = 0;

//=====================================
//========audio function====================
void initAD()
{
    /* init ADC */
    int i;
    adc_buf                        = (uint8_t *)malloc(ADC_BUFFER_SIZE);
    memset((uint8_t *) adc_buf, 0, ADC_BUFFER_SIZE);
    memset((void *)&spec_adI,   0, sizeof(STRC_I2S_SPEC));
    spec_adI.channels              = 2;
    spec_adI.sample_rate           = 44100;
    spec_adI.buffer_size           = ADC_BUFFER_SIZE;

    spec_adI.num_hdmi_audio_buffer = 4;

    for (i = 0; i < 4; i++)
    {
        if (i < spec_adI.num_hdmi_audio_buffer)
        {
            HDMI_Ibuf[i] = (uint8_t *)malloc(ADC_BUFFER_SIZE);
            memset(HDMI_Ibuf[i], (i * 10 + 10), ADC_BUFFER_SIZE);
        }
        else
        {
            HDMI_Ibuf[i] = NULL;
        }
    }
    spec_adI.base_hdmi[0]  = HDMI_Ibuf[0];
    spec_adI.base_hdmi[1]  = HDMI_Ibuf[1];
    spec_adI.base_hdmi[2]  = HDMI_Ibuf[2];
    spec_adI.base_hdmi[3]  = HDMI_Ibuf[3];

    spec_adI.is_big_endian = 0;
    spec_adI.base_i2s      = (uint8_t *) adc_buf;
    spec_adI.sample_size   = 16;
    spec_adI.record_mode   = 1;
    spec_adI.from_LineIN   = 0;
    spec_adI.from_MIC_IN   = 1;
    //i2s_init_ADC(&spec_adI);
    audio_init_AD(&spec_adI);
    audio_pause_AD(1);
}

void initDA()
{
    /* init DAC */
    dac_buf                           = (uint8_t *)malloc(DAC_BUFFER_SIZE);
    memset((uint8_t *) dac_buf, 0, DAC_BUFFER_SIZE);
    memset((void *)&spec_daI,   0, sizeof(STRC_I2S_SPEC));
    spec_daI.channels                 = 2;
    spec_daI.sample_rate              = 44100;
    spec_daI.buffer_size              = DAC_BUFFER_SIZE;
    spec_daI.is_big_endian            = 0;
    spec_daI.base_i2s                 = (uint8_t *) dac_buf;
    spec_daI.sample_size              = 16;
    spec_daI.num_hdmi_audio_buffer    = 0;
    spec_daI.is_dac_spdif_same_buffer = 1;
    spec_daI.enable_Speaker           = 1;
    spec_daI.enable_HeadPhone         = 1;
    spec_daI.postpone_audio_output    = 1;
    spec_daI.base_spdif               = (uint8_t *) dac_buf;
    //i2s_init_DAC(&spec_daI);
    audio_init_DA(&spec_daI);
    audio_pause_DA(1);
}

void *AudioThread(void *arg)
{
    initDA();
    initAD();
    audio_AD_volume_set(60);  //set rec 0~100 (faraday : level 60= 0db )
    audio_DA_volume_set(100); //set volume 0~100 (faraday : level 100= 0db )
    printf("audio init done\n");

    uint16_t bsize = audio_get_bsize_from_bps(&spec_daI);
    uint8_t  *psrc = NULL;
    psrc    = (uint8_t *)malloc(bsize);
    recdata = (uint8_t *)malloc(ADC_MEMORY_SIZE);
    audio_pause_AD(0);

    char     *filename = "a:/record.wav";
    playdata *d;
    d = INITplaydata();
    openfilewb(d, filename);

    #if 0
    while (1)
    {
        if (audio_HDMIRX_Data_Recv(&spec_adI, psrc, bsize, 0))
        {
            audio_TX_Data_Send(&spec_daI, psrc, bsize);
        }
        usleep(10);
    }
    #else
    while (sizecount < ADC_MEMORY_SIZE)
    {
        if ((sizecount + bsize) > ADC_MEMORY_SIZE) break;
        if (audio_HDMIRX_Data_Recv(&spec_adI, psrc, bsize, 0))
        {
            memcpy(recdata + sizecount, psrc, bsize);
            sizecount += bsize;
        }

        audio_AD_wait_time(&spec_adI, bsize);
    }
    audio_pause_AD(1);
    if (d->fd)
    {
        write_wav_header(d, &spec_adI, ADC_MEMORY_SIZE);
        fwrite(recdata, ADC_MEMORY_SIZE, 1, d->fd);
        fclose(d->fd);
        printf("save file in a:/record.wav\n");
        printf("now check USB disk file\n");
    }
    else
        printf("open file fail.\n");
    #endif
}

#endif
//=====================================

static uint32_t
_GET_DURATION(
    struct timeval *startT)
{
    struct timeval currT         = {0};
    uint64_t       duration_time = 0;

    gettimeofday(&currT, NULL);
    duration_time  = (currT.tv_sec - startT->tv_sec) * 1000;     // sec to ms
    duration_time += ((currT.tv_usec - startT->tv_usec) / 1000); // us to ms
    return (uint32_t)duration_time;
}

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
    ;
    CAPQUEUEINFO   capqueueinfo;
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
            capindex                    = ithCapReturnWrBufIndex(&gCapDev0);
            capqueueinfo.CapOutWidth    = (unsigned int)gCapDev0.cap_info.outinfo.OutWidth;
            capqueueinfo.CapOutHeight   = (unsigned int)gCapDev0.cap_info.outinfo.OutHeight;
            capqueueinfo.CapPitchY      = (unsigned int)gCapDev0.cap_info.ininfo.PitchY;
            capqueueinfo.CapPitchUV     = (unsigned int)gCapDev0.cap_info.ininfo.PitchUV;
            capqueueinfo.CapaddrY       = (unsigned int)gCapDev0.cap_info.OutAddrY[capindex];
            capqueueinfo.CapaddrUV      = (unsigned int)gCapDev0.cap_info.OutAddrUV[capindex];
            capqueueinfo.Capinterlanced = gCapDev0.cap_info.ininfo.Interleave;

            xQueueSendToBackFromISR(CaptoDisplayQueue, &capqueueinfo, &gHigherPriorityTaskWoken);

            //clear cap0 interrupt
            ithCapClearInterrupt(&gCapDev0, MMP_FALSE);
        }
        portYIELD_FROM_ISR(gHigherPriorityTaskWoken);
    }
#endif
    return;
}

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
    if(Capctxt->bMatchResolution)
    {
        ithCapFire(ptDev, MMP_TRUE);
        printf("Capture Fire! (%d)\n", ptDev->cap_id);
    }
    else
        return 1;// get timing fail 

    return 0;//sucess
}

static void *_DisplayThread()
{
#ifdef __OPENRTOS__
    uint8_t           *dbuf        = NULL;
    ITV_DBUF_PROPERTY dbufprop     = {0};
    CAPQUEUEINFO      CAPQUEUEINFO = {0};
    //gettimeofday(&pollS_time1, NULL);
    while (1)
    {
        if (xQueueReceive(CaptoDisplayQueue, &CAPQUEUEINFO, 0))
        {


            dbuf = itp_itv_get_dbuf_anchor();
            if (dbuf != NULL)
            {
                if (CAPQUEUEINFO.Capinterlanced)
                    itp_itv_enable_isp_feature(MMP_ISP_DEINTERLACE);

                dbufprop.src_w    = CAPQUEUEINFO.CapOutWidth;
                dbufprop.src_h    = CAPQUEUEINFO.CapOutHeight;
                dbufprop.pitch_y  = CAPQUEUEINFO.CapPitchY;
                dbufprop.pitch_uv = CAPQUEUEINFO.CapPitchUV;

                dbufprop.format   = MMP_ISP_IN_NV12;
                dbufprop.ya       = (uint8_t  *)CAPQUEUEINFO.CapaddrY;
                dbufprop.ua       = (uint8_t  *)CAPQUEUEINFO.CapaddrUV;
                dbufprop.va       = (uint8_t  *)CAPQUEUEINFO.CapaddrUV;
                //printf("fps %d\n",_GET_DURATION(&pollS_time1));
                //gettimeofday(&pollS_time1, NULL);
                itp_itv_update_dbuf_anchor(&dbufprop);
            }
        }
        usleep(1000);
    }
#endif
}

//hdmi test Run
void *TestFunc(void *arg)
{
    printf("hdmi test cap start\n");
    itpInit();

    CaptoDisplayQueue = xQueueCreate(10, (unsigned portBASE_TYPE) sizeof(CAPQUEUEINFO));
    pthread_create(&display_task, NULL, _DisplayThread, NULL);

#ifdef CFG_LCD_ENABLE
    /*lcd panel int*/
    ioctl(ITP_DEVICE_SCREEN,    ITP_IOCTL_POST_RESET, NULL);
    #ifdef CFG_BACKLIGHT_ENABLE
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET,      NULL);
    #endif
    /*itv int*/
    itp_itv_init();
#endif

    ithCapPowerUp();

    /*capture init*/
    ithCapInitialize();

    /*Hdmi init*/
    ithCapDeviceInitialize();

    CAPTURE_SETTING mem_modeset = {MMP_CAP_DEV_HDMIRX, MMP_FALSE,  MMP_TRUE, 1920, 1080};
    ithCapConnect(&gCapDev0, mem_modeset);
    ithCapRegisterIRQ(_CAP_ISR0, &gCapDev0);

#if defined(CFG_I2S_ENABLE) && defined(CFG_AUDIO_ENABLE)
    /*========audio thread========*/
    sleep(2);//wait usb mount on system
    pthread_t      task;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&task, &attr, AudioThread, NULL);
#endif

    /*main loop*/
    for (;;)
    {
        FSM_STATE(ithCapDeviceIsSignalStable());

        switch (state)
        {
        case NO_SIGNAL:
            if (cap0_get_err)
            {
                printf("###CAPTURE ERROR###\n");
                ithCapWaitEngineIdle(&gCapDev0);
                ithCapEngineReset();
                cap0_get_err = false;
            }
            break;
        case NO_SIGNAL_TO_SIGNAL_STABLE:
            printf("###NO_SIGNAL_TO_SIGNAL_STABLE###\n");
            ithCapGetDeviceInfo(&gCapDev0);
            ithCapParameterSetting(&gCapDev0);
            if (_SIGNALCHECK_FIRE(&gCapDev0))
            {
                ithCapWaitEngineIdle(&gCapDev0);
                ithCapEngineReset();                
                FSM_STATE(false);
            }
                
            break;
        case SIGNAL_STABLE:
            if (cap0_get_err)
            {
                printf("###CAPTURE ERROR###\n");
                ithCapWaitEngineIdle(&gCapDev0);
                ithCapEngineReset();
                cap0_get_err = false;
                FSM_STATE(false);
            }
            break;
        default:
            break;
        }
        usleep(200 * 1000);
    }
}