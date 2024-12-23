#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"
#include "usbhcc/api/api_usbh_printer.h"
#include "usbhcc/api/api_usbh_printer_pdl_pcl5.h"
#include "usbhcc/api/api_usbh_printer_pdl_escpos.h"
#include "usbhcc/config/config_usbh_printer.h"

#include "PP7_patterns.h"

int newlyConnected[USBH_PRINTER_MAX_UNITS]={0};

static int printer_event ( t_usbh_unit_id uid, t_usbh_ntf ntf )
{
  if ( ntf == USBH_NTF_CONNECT )
  {
    ithPrintf( "[i] Printer uid:%d connected.\r\n", uid );
    newlyConnected[uid]=1;
  }
  else if ( ntf == USBH_NTF_DISCONNECT )
  {
    ithPrintf( "[i] Printer uid:%d disconnected.\r\n",uid );
    newlyConnected[uid]=0;
  }
  else
  {
    ithPrintf( "[X] Printer ntf:%d, uid:%d.\r\n", ntf, uid );
  }

  return 0;
} /* printer_event */

static void print_test_page ( t_usbh_unit_id uid )
{
  uint32_t              r;
  pcl5_buffer_t         pcl5_buf;
  static unsigned char  buf_prn[1024];

  pcl5_buf.start = (char *)buf_prn;
  pcl5_buf.index = 0;
  pcl5_buf.length = sizeof( buf_prn );

  pcl5_buf.start = (char *)buf_prn;
  pcl5_buf.index = 0;
  pcl5_buf.length = sizeof( buf_prn );

  {
    if (( usbh_prn_present( uid ) == TRUE ))
    {
      /* Print only one page after printer was connected. */

      (void)usbh_prn_read_id( uid, buf_prn, 64, &r );
      if ( r == 0 )
      {
        ithPrintf( "Cannot read printer identification.\r\n" );
        return;
      }

      buf_prn[r] = '\0';
      buf_prn[0] = ' ';
      buf_prn[1] = ' ';

      /* Generate PCL data. */
      pcl5_start_job( &pcl5_buf );

      pcl5_set_col( &pcl5_buf, 10 );
      pcl5_print_text( &pcl5_buf, "Demo page" );

      pcl5_set_col( &pcl5_buf, 0 );
      pcl5_set_row( &pcl5_buf, 3 );

      pcl5_print_text( &pcl5_buf, "This is row 3 col 0" );

      pcl5_set_l_margin( &pcl5_buf, 5 );
      pcl5_set_r_margin( &pcl5_buf, 55 );

      pcl5_print_text( &pcl5_buf, "\r\nThis is a text after margin set to 5 - 55. This \r\nmeans we can print 50 characters in a row.\r\n" );
      pcl5_print_text( &pcl5_buf, "->2345678901234567890123456789012345678901234567<-\r\n" );

      pcl5_fixed_spacing( &pcl5_buf );
      pcl5_set_fixed_pitch( &pcl5_buf, 20 );
      pcl5_print_text( &pcl5_buf, "20 cpi are fixed spacing characters.\r\n" );

      pcl5_set_fixed_pitch( &pcl5_buf, 10 );
      pcl5_print_text( &pcl5_buf, "10 pitch sized fixed spacing characters.\r\n" );

      pcl5_prportional_spacing( &pcl5_buf );
      pcl5_set_proportional_height( &pcl5_buf, 3 );
      pcl5_print_text( &pcl5_buf, "3 point proportional spacing characters.\r\n" );

      pcl5_set_proportional_height( &pcl5_buf, 15 );
      pcl5_print_text( &pcl5_buf, "15 point proportional spacing characters." );

      pcl5_set_line_spacing( &pcl5_buf, lpi_4 );
      pcl5_set_proportional_height( &pcl5_buf, 20 );
      pcl5_print_text( &pcl5_buf, "\r\n20 point proportional spacing characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_compr );
      pcl5_print_text( &pcl5_buf, "compressed characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_cond );
      pcl5_print_text( &pcl5_buf, "condensed characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_condita );
      pcl5_print_text( &pcl5_buf, "condensed and italic characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_expand );
      pcl5_print_text( &pcl5_buf, "expanded characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_inline );
      pcl5_print_text( &pcl5_buf, "inline characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_ita );
      pcl5_print_text( &pcl5_buf, "italic characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_outline );
      pcl5_print_text( &pcl5_buf, "outile characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_outsha );
      pcl5_print_text( &pcl5_buf, "outile and shadowed characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_sha );
      pcl5_print_text( &pcl5_buf, "shadowed characters.\r\n" );

      pcl5_set_style( &pcl5_buf, chrs_normal );
      pcl5_print_text( &pcl5_buf, "back to normal characters.\r\n" );

      pcl5_underline_on( &pcl5_buf );
      pcl5_print_text( &pcl5_buf, "underline text\r\n" );
      pcl5_underline_off( &pcl5_buf );

      pcl5_set_weight( &pcl5_buf, -7 );
      pcl5_print_text( &pcl5_buf, "weight -7 characters.\r\n" );

      pcl5_set_weight( &pcl5_buf, 7 );
      pcl5_print_text( &pcl5_buf, "weight 7 characters.\r\n" );

      pcl5_set_weight( &pcl5_buf, 0 );
      pcl5_print_text( &pcl5_buf, "weight 0 characters.\r\n" );

      pcl5_print_text( &pcl5_buf, "Transparent characters\r\n" );
      pcl5_print_transparent( &pcl5_buf, "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xa\xb\xc\xd\xe\xf", 16 );
      pcl5_print_text( &pcl5_buf, "\r\nEnd of page" );

      pcl5_end_job( &pcl5_buf );
      if ( usbh_prn_print( uid, (uint8_t *)pcl5_buf.start, pcl5_buf.index ) == 0 )
      {
        ithPrintf( "Print successful\r\n" );
      }
      else
      {
        ithPrintf( "Print failed\r\n" );
      }
    }
  }
} /* print_test_page */

static void printer_test(t_usbh_unit_id uid)
{
  int r;
  uint32_t len;
  t_usbh_port_hdl port_hdl;
  t_usbh_port_inf port_inf;
  uint8_t id[512];
  uint8_t status;

  ithPrintf("[i] uid=%d\n", uid);

  // get VID&PID
  if ((port_hdl=usbh_prn_get_port_hdl(uid))==USBH_PORT_HDL_INVALID) {
    ithPrintf("[X] usbh_prn_get_port_hdl(%d)\n", uid);
  }
  else {
    if ((r=usbh_get_port_inf(port_hdl, &port_inf))!=USBH_SUCCESS) {
      ithPrintf("[X] usbh_get_port_inf(%d)=%d\n", port_hdl, r);
    }
    else {
      ithPrintf("[i] VID=%04X, PID=%04X\n", port_inf.vid, port_inf.pid);
    }
  }

  // get IEEE 1284 ID string
  memset(id, 0, sizeof(id));
  if ((r=usbh_prn_read_id(uid, id, sizeof(id)-1, &len))!=USBH_SUCCESS) {
    ithPrintf("[X] usbh_prn_read_id()=%d\n", r);
  }
  else {
    ithPrintf("[i] id=%s\n", id+2);
  }

  // send soft reset command to the printer
  if ((r=usbh_prn_soft_reset(uid))!=USBH_SUCCESS) {
    ithPrintf("[X] usbh_prn_soft_reset()=%d\n", r);
  }
  else {
    ithPrintf("[i] usbh_prn_soft_reset() OK.\n");
  }

  // get printer status
  if ((r=usbh_prn_get_port_status(uid, &status))!=USBH_SUCCESS) {
    ithPrintf("[X] usbh_prn_get_port_status()=%d\n", r);
  }
  else {
    ithPrintf("[i] usbh_prn_get_port_status() OK, status=%X\n", status);
  }

  // write data to the printer
  if ((r=usbh_prn_send(uid, usblpTestPattern_pp7, sizeof(usblpTestPattern_pp7), NULL))!=USBH_SUCCESS) {
    ithPrintf("[X] usbh_prn_send()=%d\n", r);
  }
  else {
    ithPrintf("[i] usbh_prn_send() OK.\n");
  }

l_exit:
  return;
}


static void* main_task(void* arg)
{
  int n;
  int i;
  int prn_uid = 0;
  int rc;

  itpInit();

  memset(newlyConnected, 0, sizeof(newlyConnected));
  while ( ( prn_uid < USBH_PRINTER_MAX_UNITS ) )
  {
    rc = usbh_prn_register_ntf( prn_uid, USBH_NTF_CONNECT, printer_event );
    ithPrintf("[%c] usbh_prn_register_ntf(%d)=%d\n", (rc==USBH_SUCCESS)?'O':'X', prn_uid, rc);
    prn_uid++;
  }

  while (1) {
    for (i=0;i<USBH_PRINTER_MAX_UNITS;i++) {
      if (newlyConnected[i]) {
        if (usbh_prn_present(i)) {
          printer_test(i);
          //print_test_page(i);
        }
        newlyConnected[i]=0;
      }
    }

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
