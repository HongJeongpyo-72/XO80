#ifndef __GC2145_H__
#define __GC2145_H__

#include "ite/itp.h"
#include "ith/ith_defs.h"
#include "ite/ith.h"
#include "mmp_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//X10LightDriver_t1.h
typedef struct GC2145DriverStruct *GC2145SensorDriver;
SensorDriver GC2145SensorDriver_Create();
static void GC2145SensorDriver_Destory(SensorDriver base);
void GC2145Initialize(uint16_t Mode);
void GC2145Terminate(void);
void GC2145OutputPinTriState(uint8_t flag);
uint16_t GC2145GetProperty(MODULE_GETPROPERTY property);
uint8_t GC2145GetStatus(MODULE_GETSTATUS Status);
void GC2145PowerDown(uint8_t enable);
uint8_t GC2145IsSignalStable(uint16_t Mode);
//end of X10LightDriver_t1.h

#ifdef __cplusplus
}
#endif

#endif