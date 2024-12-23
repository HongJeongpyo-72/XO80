/***************************************************************************
 *
 *            Copyright (c) 2011-2017 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 76
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/


#ifndef _API_USBD_PRN_H_
#define _API_USBD_PRN_H_

#include "../version/ver_usbd_prn.h"
#include <stdint.h>
#include <stddef.h>


#if VER_USBD_PRN_MAJOR != 2 || VER_USBD_PRN_MINOR != 3
 #error "Invalid USBD_PRN version"
#endif

#define USBD_PRN_SUCCESS         0
#define USBD_PRN_ERROR           1
#define USBD_PRN_BUSY            2

#define USBDPRN_PST_PEPEER_EMPTY ( 1u << 5 )
#define USBDPRN_PST_SELECT       ( 1u << 4 )
#define USBDPRN_PST_NOT_ERROR    ( 1u << 3 )

int usbd_prn_set_port_state ( uint8_t state_mask );
int usbd_prn_get_port_state ( uint8_t * state_mask );

int usbdprn_receive ( void * buffer, uint32_t length, uint32_t * read );
uint32_t usbdprn_get_rx_num ( void );

void  usbdprn_set_rst_cb( void ( * fn )( uint32_t param ), uint32_t param );
void  usbdprn_set_start_stop_cb( void ( * fn )( uint32_t param ), uint32_t param );

int usbdprn_is_active ( void );

int usbdprn_init ( void );
int usbdprn_start ( void );
int usbdprn_stop ( void );
int usbdprn_delete ( void );

#endif /* _API_USBD_PRN_H_ */
