#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "ite/itp_usb_hid.h"
#include "usbhcc/config/hid/ite_usbd_hid_config.h"

static int usbd_ghid_out_ntf ( uint8_t unit, uint16_t ntf, uint32_t param )
{
    int       rc = 0;
    uint8_t   i;
    uint8_t read_data_buff[64] = {0};
    ithPrintf( "HID device read ntf callback, unit: %i ntf: %i param: %i\r\n", unit, ntf, param );
    int len = itp_usbd_hid_read_data(USBD_HID_OUT_REPORT_ID_BASE, read_data_buff);
    if (len > 0)
    {
        int i;
        printf("Read len(%d):\n", len);
        ithPrintVram8((uint32_t)read_data_buff, len);
    }
    return rc;
}

void *TestFunc(void *arg)
{
    uint8_t write_data_buff[64] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4};
    ithPrintf("%s(%d)\n", __FILE__, __LINE__);

    for (;;)
    {
        while(!itp_usbd_hid_is_connected()) {
            sleep(1);
        }
        
        itp_usbd_hid_reg_read_ntf(USBD_HID_OUT_REPORT_ID_BASE, usbd_ghid_out_ntf);
        
        while(itp_usbd_hid_is_connected()) {
            itp_usbd_hid_write_data(USBD_HID_IN_REPORT_ID_BASE, write_data_buff, sizeof(write_data_buff));
            ithPrintf( "HID device write to host !\n");
            sleep(2);
        }
    }
}
