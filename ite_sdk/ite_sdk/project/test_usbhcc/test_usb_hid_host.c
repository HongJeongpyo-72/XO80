#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "ite/itp_usb_hid.h"

void itp_usbh_hid_connect_cb(uint8_t uid)
{
    ithPrintf("uid(%d) %s(%d)\n", uid, __FUNCTION__, __LINE__);
}

void itp_usbh_hid_disconnect_cb(uint8_t uid)
{
    ithPrintf("uid(%d) %s(%d)\n", uid, __FUNCTION__, __LINE__);
}

void itp_usbh_hid_receive_report_cb(uint8_t uid, uint8_t report_id, uint8_t *report_data, uint32_t report_size)
{
    ithPrintf("uid(%d) rid(%d) size(%d) %s(%d)\n", uid, report_id, report_size, __FUNCTION__, __LINE__);
    ithPrintVram8((uint32_t)report_data, report_size);
    
    uint8_t write_data_buff[64] = {2,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3};
    itp_usbh_hid_out_ep_write_report(uid, write_data_buff, sizeof(write_data_buff));
    ithPrintf("HID host out ep write to device !\n");
    
}

void *TestFunc(void *arg)
{
    ithPrintf("%s(%d)\n", __FILE__, __LINE__);
    for (;;)
    {
        usleep(1000);
    }
}
