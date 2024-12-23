#ifndef __USBLP_H__
#define __USBLP_H__
#include "usblp/ioctl.h"

/* ioctls: */
enum {
  USBLP_IOCTL_GET_ID,
  USBLP_IOCTL_GET_PORT_STATUS,
  USBLP_IOCTL_SOFT_RESET,
  USBLP_IOCTL_HP_SET_CHANNEL,
  USBLP_IOCTL_GET_VID_PID,
  USBLP_IOCTL_GET_BUS_ADDRESS,
  USBLP_IOCTL_GET_PROTOCOLS,
  USBLP_IOCTL_SET_PROTOCOL,
};

int usblpGetNumHandles();
void* usblpGetHandle(int pos);
int usblpRead(void* h, char* buffer, unsigned int len);
int usblpReadNonblock(void* h, char* buffer, unsigned int len);
int usblpWrite(void* h, char* buffer, unsigned int len);
int usblpWriteNonblock(void* h, char* buffer, unsigned int len);
int usblpIoctl(void* h, unsigned int code, void* buf, unsigned int len);

#endif /*__USBLP_H__*/