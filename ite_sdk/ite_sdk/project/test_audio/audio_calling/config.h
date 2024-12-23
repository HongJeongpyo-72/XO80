#ifndef CONFIG_H
#define CONFIG_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "ite/itu.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum{
	CALL_EARLYMEDIA = 0,
	CALL_ANSWERED,
	CALL_STOP,
	CALL_ACK,
	CALL_IDLE,
}CALL_STATE;

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */