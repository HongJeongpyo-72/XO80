#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"
#include "i2s/i2s.h"
#include "flower/flower.h"
#include "config.h"

IteFlower *call = NULL;

void *TestFunc(void *arg)
{
    CALL_STATE call_state = CALL_IDLE;
    itpInit();
    NetworkInit();
    sleep(2);
    call=IteStreamInit();
    flow_set_dac_level(80);
    flow_set_adc_level(80);
    
    while(1){
    
        while(1){
            if(NetworkIsReady()) {
                call_state = NetworkCheckIncoming();
                printf("call_state=%d\n",call_state);
                if(call_state==CALL_EARLYMEDIA){
                    NetworkCallSend(CALL_ACK);
                    flow_start_udp_audiostream(call, 8000, ULAW, "192.168.190.132", 6666);
                    break;
                }
                usleep(10000);
            }else{
            printf("wait\n"); 
            sleep(1);
            }
        
        }
        
         while(1) {
             /*connect state*/
             sleep(1);
             if(!NetworkIsReady()){
                 flow_stop_udp_audiostream(call);
                 call_state = CALL_IDLE;
                 break;
             }
         }
    }
   


    return NULL;
}