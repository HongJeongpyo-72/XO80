
#ifndef ITE_MT7682_H
#define ITE_MT7682_H

#include "ite/ith.h"
//#include "../driver/mt7682/sdio_reg.h"
//#include "../driver/mt7682/hal_sdio.h"


#ifdef __cplusplus
extern "C" {
#endif

#define Rx_Test 1
#define Tx_Test 1
#define Interrupt_Test 0

typedef unsigned int u32;
typedef signed int   s32;


int  wifi_host_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ITE_MT7682_H */
