
#include "tuya_hal_rtc.h"
#include "ite/itp.h"


void tuya_hal_rtc_init(void)
{
    printf("%s %d \n",__FUNCTION__, __LINE__);
    // rtc_init();
    // rtc_write(0);
}


void tuya_hal_rtc_set_time(time_t write_time) 
{
    struct timeval tv;
    printf("%s %d \n",__FUNCTION__, __LINE__);

    tv.tv_sec = write_time;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
}


time_t tuya_hal_rtc_get_time(void)
{
    struct timeval tv;
    printf("%s %d \n",__FUNCTION__, __LINE__);

    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}


