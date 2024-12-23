#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"
#include "usbhcc/api/api_usbh_rndis.h"




static void* main_task(void* arg)
{
  int n;
  int i;
  int rc;

  itpInit();

  while (1) {
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
