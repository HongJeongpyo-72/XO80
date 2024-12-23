#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"
//#include "usbhcc/api/api_usbd.h"
#include "usbhcc/api/api_usbd_prn.h"

static uint8_t  usbd_printer_buf[1024];

#define HEXDUMP_COLS 16
static hexdump(char* msg, void *mem, unsigned int len)
{
        unsigned int i, j;
//return;
    ithPrintf(msg);
        for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
        {
                /* print offset */
                if(i % HEXDUMP_COLS == 0)
                {
                        printf("0x%06x: ", i);
                }
 
                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }
                
                /* print ASCII dump */
                if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
                {
                        for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isalnum(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);        
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
    ithPrintf("\n");
}

static void usbd_reset( uint32_t param )
{
  ithPrintf("\n[i] reset!\n");
}

static void usbd_start_stop( uint32_t param )
{
  if ( usbdprn_is_active() ) {
    ithPrintf("\n[i] started!\n");
  }
  else {
    ithPrintf("\n[i] stopped!\n");
  }
}

static void usbd_printer_demo ( void )
{
  int  rc;

  if ( usbdprn_is_active() )
  {
    while ( usbdprn_get_rx_num() > 0 )
    {
      uint32_t  read;
      rc = usbdprn_receive( usbd_printer_buf, sizeof( usbd_printer_buf ), &read );
      if ( ( rc == 0 ) && ( read > 0 ) )
      {
        usbd_printer_buf[read] = 0;
        //ithPrintf( "<<(%4d) %s\r\n",  read, usbd_printer_buf);
        ithPrintf( "<< %4d bytes received..\n", read);
        hexdump("", usbd_printer_buf, read);
      }
    }
  }
} /* usbd_printer_demo */


static void* main_task(void* arg)
{
  int n;
  int i;
  int prn_uid = 0;
  int rc;

  itpInit();

  usbdprn_set_rst_cb(usbd_reset, 0);
  usbdprn_set_start_stop_cb(usbd_start_stop, 0);

  ithPrintf("[i] sleep 10s..\n"); sleep(10);
  while (1) {
    usbd_printer_demo();
    ithPrintf("#");
    usleep(500*1000);
  }
    
  return NULL;
}

int main(void)
{
  pthread_t task;
  pthread_attr_t attr;
  
  pthread_attr_init(&attr);
  pthread_create(&task, &attr, main_task, NULL);

  /* Now all the tasks have been started - start the scheduler. */
  vTaskStartScheduler();

  /* Should never reach here! */
  return 0;
}
