//=============================================================================
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "iic/mmp_iic.h"
#include "GC2145.h"

//=============================================================================
//                Constant Definition
//=============================================================================
static uint8_t IICADDR = 0x78 >> 1;   /* please assign IIC ADDRESS */
#ifdef CFG_SENSOR_ENABLE
static uint8_t IICPORT = CFG_SENSOR_IIC_PORT;  /* please assign IIC PORT      */
#else
static uint8_t IICPORT = IIC_PORT_2;
#endif
//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
typedef struct _REGPAIR
{
    uint8_t  addr;
    uint16_t value;
} REGPAIR;

typedef struct GC2145DriverStruct
{
    SensorDriverStruct base;
} GC2145DriverStruct;

//=============================================================================
//                IIC API FUNCTION START
//=============================================================================
uint8_t GC_ReadI2c_Byte(uint8_t RegAddr)
{
    int        result;
    uint8_t    dbuf[16];
    uint8_t    *pdbuf     = dbuf;
    char       portname[4];
    ITPI2cInfo evt;
    int        gMasterDev = 0;

    sprintf(portname, ":i2c%d", IICPORT);
    gMasterDev         = open(portname, 0);

    dbuf[0]            = (uint8_t)(RegAddr);
    pdbuf++;

    evt.slaveAddress   = IICADDR;
    evt.cmdBuffer      = dbuf;
    evt.cmdBufferSize  = 1;
    evt.dataBuffer     = pdbuf;
    evt.dataBufferSize = 1;

    result             = read(gMasterDev, &evt, 1);
    if (result != 0)
    {
        ithPrintf("ReadI2c_Byte read address 0x%02x error!\n", RegAddr);
    }
    //printf("Reg = %x, value[0] = %x ,value[1] = %x\n",RegAddr, dbuf[0],dbuf[1]);
    return dbuf[1];
}

uint32_t GC_WriteI2c_Byte(uint8_t RegAddr, uint8_t data)
{
    int        result     = 0;
    uint8_t    dbuf[16];
    uint8_t    *pdbuf     = dbuf;
    char       portname[4];
    ITPI2cInfo evt;
    int        gMasterDev = 0;

    sprintf(portname, ":i2c%d", IICPORT);
    gMasterDev        = open(portname, 0);

    *pdbuf++          = (uint8_t)(RegAddr & 0xff);
    *pdbuf            = (uint8_t)(data);

    evt.slaveAddress  = IICADDR;     //對接裝置salve address
    evt.cmdBuffer     = dbuf;        //欲傳送給slave data buffer
    evt.cmdBufferSize = 2;           //傳送data size,單位為byte

    if (0 != (result = write(gMasterDev, &evt, 1)))
    {
        printf("WriteI2c_Byte Write Error, reg=%02x val=%02x\n", RegAddr, data);
    }
    return result;
}

uint32_t GC_WriteI2c_ByteMask(uint8_t RegAddr, uint8_t data, uint8_t mask)
{
    uint8_t  value;
    uint32_t flag;

    value = ReadI2c_Byte(RegAddr);
    value = ((value & ~mask) | (data & mask));
    flag  = WriteI2c_Byte(RegAddr, value);

    return flag;
}

//=============================================================================
//                IIC API FUNCTION END
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                GC2145 IMPLEMENT FUNCTION START
//=============================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////////

//X10LightDriver_t1.c
void GC2145Initialize(uint16_t Mode)
{
    /* Please implement initial code here */
	
#ifdef CFG_SENSOR_RESETPIN_ENABLE
	printf("[GC2145]reset\n");
	ithGpioSetOut(CFG_SN1_GPIO_RST);
	ithGpioSetMode(CFG_SN1_GPIO_RST, ITH_GPIO_MODE0);
	ithGpioSet(CFG_SN1_GPIO_RST);
	usleep(100*1000);
#endif

	printf("chip id = (%x,",GC_ReadI2c_Byte(0xf0)); //0x21
	printf("%x)\n",GC_ReadI2c_Byte(0xf1)); //0x45
#if 1
	GC_WriteI2c_Byte(0xfe,0xf0);
	GC_WriteI2c_Byte(0xfe,0xf0);
	GC_WriteI2c_Byte(0xfe,0xf0);
	GC_WriteI2c_Byte(0xfc,0x06);
	GC_WriteI2c_Byte(0xf6,0x00);
	GC_WriteI2c_Byte(0xf7,0x1d);
	GC_WriteI2c_Byte(0xf8,0x83);
	GC_WriteI2c_Byte(0xfa,0x10);
	GC_WriteI2c_Byte(0xf9,0xfe);
	GC_WriteI2c_Byte(0xfd,0x00);
	GC_WriteI2c_Byte(0xc2,0x00);
	GC_WriteI2c_Byte(0xf2,0x0f);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0x03,0x04);
	GC_WriteI2c_Byte(0x04,0x62);
	GC_WriteI2c_Byte(0x05,0x01);
	GC_WriteI2c_Byte(0x06,0x56);
	GC_WriteI2c_Byte(0x09,0x00);
	GC_WriteI2c_Byte(0x0a,0x00);
	GC_WriteI2c_Byte(0x0b,0x00);
	GC_WriteI2c_Byte(0x0c,0x00);
	GC_WriteI2c_Byte(0x0d,0x04);
	GC_WriteI2c_Byte(0x0e,0xc0);
	GC_WriteI2c_Byte(0x0f,0x06);
	GC_WriteI2c_Byte(0x10,0x52);
	GC_WriteI2c_Byte(0x12,0x2e);
	GC_WriteI2c_Byte(0x17,0x14);
	GC_WriteI2c_Byte(0x18,0x22);
	GC_WriteI2c_Byte(0x19,0x0f);
	GC_WriteI2c_Byte(0x1a,0x01);
	GC_WriteI2c_Byte(0x1b,0x4b);
	GC_WriteI2c_Byte(0x1c,0x07);
	GC_WriteI2c_Byte(0x1d,0x10);
	GC_WriteI2c_Byte(0x1e,0x88);
	GC_WriteI2c_Byte(0x1f,0x78);
	GC_WriteI2c_Byte(0x20,0x03);
	GC_WriteI2c_Byte(0x21,0x40);
	GC_WriteI2c_Byte(0x22,0xf0);
	GC_WriteI2c_Byte(0x24,0xf3);
	GC_WriteI2c_Byte(0x25,0x01);
	GC_WriteI2c_Byte(0x26,0x10);
	GC_WriteI2c_Byte(0x2d,0x60);
	GC_WriteI2c_Byte(0x30,0x01);
	GC_WriteI2c_Byte(0x31,0x90);
	GC_WriteI2c_Byte(0x33,0x06);
	GC_WriteI2c_Byte(0x34,0x01);
	GC_WriteI2c_Byte(0x80,0xff);
	GC_WriteI2c_Byte(0x81,0x24);
	GC_WriteI2c_Byte(0x82,0xfa);
	GC_WriteI2c_Byte(0x83,0x00);
	GC_WriteI2c_Byte(0x84,0x02);
	GC_WriteI2c_Byte(0x86,0x06); //0x02
	GC_WriteI2c_Byte(0x88,0x03);
	GC_WriteI2c_Byte(0x89,0x03);
	GC_WriteI2c_Byte(0x85,0x30);
	GC_WriteI2c_Byte(0x8a,0x00);
	GC_WriteI2c_Byte(0x8b,0x00);
	GC_WriteI2c_Byte(0xb0,0x55);
	GC_WriteI2c_Byte(0xc3,0x00);
	GC_WriteI2c_Byte(0xc4,0x80);
	GC_WriteI2c_Byte(0xc5,0x90);
	GC_WriteI2c_Byte(0xc6,0x38);
	GC_WriteI2c_Byte(0xc7,0x40);
	GC_WriteI2c_Byte(0xec,0x06);
	GC_WriteI2c_Byte(0xed,0x04);
	GC_WriteI2c_Byte(0xee,0x60);
	GC_WriteI2c_Byte(0xef,0x90);
	GC_WriteI2c_Byte(0xb6,0x01);
	GC_WriteI2c_Byte(0x90,0x01);
	GC_WriteI2c_Byte(0x91,0x00);
	GC_WriteI2c_Byte(0x92,0x00);
	GC_WriteI2c_Byte(0x93,0x00);
	GC_WriteI2c_Byte(0x94,0x00);
	GC_WriteI2c_Byte(0x95,0x04);
	GC_WriteI2c_Byte(0x96,0xb0);
	GC_WriteI2c_Byte(0x97,0x06);
	GC_WriteI2c_Byte(0x98,0x40);
	GC_WriteI2c_Byte(0x18,0x02);
	GC_WriteI2c_Byte(0x40,0x42);
	GC_WriteI2c_Byte(0x41,0x00);
	GC_WriteI2c_Byte(0x43,0x54);
	GC_WriteI2c_Byte(0x5e,0x00);
	GC_WriteI2c_Byte(0x5f,0x00);
	GC_WriteI2c_Byte(0x60,0x00);
	GC_WriteI2c_Byte(0x61,0x00);
	GC_WriteI2c_Byte(0x62,0x00);
	GC_WriteI2c_Byte(0x63,0x00);
	GC_WriteI2c_Byte(0x64,0x00);
	GC_WriteI2c_Byte(0x65,0x00);
	GC_WriteI2c_Byte(0x66,0x20);
	GC_WriteI2c_Byte(0x67,0x20);
	GC_WriteI2c_Byte(0x68,0x20);
	GC_WriteI2c_Byte(0x69,0x20);
	GC_WriteI2c_Byte(0x76,0x00);
	GC_WriteI2c_Byte(0x6a,0x00);
	GC_WriteI2c_Byte(0x6b,0x00);
	GC_WriteI2c_Byte(0x6c,0x3e);
	GC_WriteI2c_Byte(0x6d,0x3e);
	GC_WriteI2c_Byte(0x6e,0x3f);
	GC_WriteI2c_Byte(0x6f,0x3f);
	GC_WriteI2c_Byte(0x70,0x00);
	GC_WriteI2c_Byte(0x71,0x00);
	GC_WriteI2c_Byte(0x76,0x00);
	GC_WriteI2c_Byte(0x72,0xf0);
	GC_WriteI2c_Byte(0x7e,0x3c);
	GC_WriteI2c_Byte(0x7f,0x00);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x48,0x15);
	GC_WriteI2c_Byte(0x49,0x00);
	GC_WriteI2c_Byte(0x4b,0x0b);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x01,0x04);
	GC_WriteI2c_Byte(0x02,0xc0);
	GC_WriteI2c_Byte(0x03,0x04);
	GC_WriteI2c_Byte(0x04,0x90);
	GC_WriteI2c_Byte(0x05,0x30);
	GC_WriteI2c_Byte(0x06,0x90);
	GC_WriteI2c_Byte(0x07,0x20);
	GC_WriteI2c_Byte(0x08,0x70);
	GC_WriteI2c_Byte(0x09,0x00);
	GC_WriteI2c_Byte(0x0a,0xc2);
	GC_WriteI2c_Byte(0x0b,0x11);
	GC_WriteI2c_Byte(0x0c,0x10);
	GC_WriteI2c_Byte(0x13,0x40);
	GC_WriteI2c_Byte(0x17,0x00);
	GC_WriteI2c_Byte(0x1c,0x11);
	GC_WriteI2c_Byte(0x1e,0x61);
	GC_WriteI2c_Byte(0x1f,0x30);
	GC_WriteI2c_Byte(0x20,0x40);
	GC_WriteI2c_Byte(0x22,0x80);
	GC_WriteI2c_Byte(0x23,0x20);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x0f,0x04);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x12,0x35);
	GC_WriteI2c_Byte(0x15,0x50);
	GC_WriteI2c_Byte(0x10,0x31);
	GC_WriteI2c_Byte(0x3e,0x28);
	GC_WriteI2c_Byte(0x3f,0xe0);
	GC_WriteI2c_Byte(0x40,0x20);
	GC_WriteI2c_Byte(0x41,0x0f);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x0f,0x05);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x90,0x6c);
	GC_WriteI2c_Byte(0x91,0x03);
	GC_WriteI2c_Byte(0x92,0xc8);
	GC_WriteI2c_Byte(0x94,0x66);
	GC_WriteI2c_Byte(0x95,0xb5);
	GC_WriteI2c_Byte(0x97,0x64);
	GC_WriteI2c_Byte(0xa2,0x11);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x80,0xc1);
	GC_WriteI2c_Byte(0x81,0x08);
	GC_WriteI2c_Byte(0x82,0x08);
	GC_WriteI2c_Byte(0x83,0x08);
	GC_WriteI2c_Byte(0x84,0x0a);
	GC_WriteI2c_Byte(0x86,0xf0);
	GC_WriteI2c_Byte(0x87,0x50);
	GC_WriteI2c_Byte(0x88,0x15);
	GC_WriteI2c_Byte(0x89,0x50);
	GC_WriteI2c_Byte(0x8a,0x30);
	GC_WriteI2c_Byte(0x8b,0x10);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x21,0x14);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0xa3,0x40);
	GC_WriteI2c_Byte(0xa4,0x20);
	GC_WriteI2c_Byte(0xa5,0x40);
	GC_WriteI2c_Byte(0xa6,0x80);
	GC_WriteI2c_Byte(0xab,0x40);
	GC_WriteI2c_Byte(0xae,0x0c);
	GC_WriteI2c_Byte(0xb3,0x34);
	GC_WriteI2c_Byte(0xb4,0x44);
	GC_WriteI2c_Byte(0xb6,0x38);
	GC_WriteI2c_Byte(0xb7,0x02);
	GC_WriteI2c_Byte(0xb9,0x30);
	GC_WriteI2c_Byte(0x3c,0x08);
	GC_WriteI2c_Byte(0x3d,0x30);
	GC_WriteI2c_Byte(0x4b,0x0d);
	GC_WriteI2c_Byte(0x4c,0x20);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x10,0x10);
	GC_WriteI2c_Byte(0x11,0x15);
	GC_WriteI2c_Byte(0x12,0x1a);
	GC_WriteI2c_Byte(0x13,0x1f);
	GC_WriteI2c_Byte(0x14,0x2c);
	GC_WriteI2c_Byte(0x15,0x39);
	GC_WriteI2c_Byte(0x16,0x45);
	GC_WriteI2c_Byte(0x17,0x54);
	GC_WriteI2c_Byte(0x18,0x69);
	GC_WriteI2c_Byte(0x19,0x7d);
	GC_WriteI2c_Byte(0x1a,0x8f);
	GC_WriteI2c_Byte(0x1b,0x9d);
	GC_WriteI2c_Byte(0x1c,0xa9);
	GC_WriteI2c_Byte(0x1d,0xbd);
	GC_WriteI2c_Byte(0x1e,0xcd);
	GC_WriteI2c_Byte(0x1f,0xd9);
	GC_WriteI2c_Byte(0x20,0xe3);
	GC_WriteI2c_Byte(0x21,0xea);
	GC_WriteI2c_Byte(0x22,0xef);
	GC_WriteI2c_Byte(0x23,0xf5);
	GC_WriteI2c_Byte(0x24,0xf9);
	GC_WriteI2c_Byte(0x25,0xff);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x26,0x0f);
	GC_WriteI2c_Byte(0x27,0x14);
	GC_WriteI2c_Byte(0x28,0x19);
	GC_WriteI2c_Byte(0x29,0x1e);
	GC_WriteI2c_Byte(0x2a,0x27);
	GC_WriteI2c_Byte(0x2b,0x33);
	GC_WriteI2c_Byte(0x2c,0x3b);
	GC_WriteI2c_Byte(0x2d,0x45);
	GC_WriteI2c_Byte(0x2e,0x59);
	GC_WriteI2c_Byte(0x2f,0x69);
	GC_WriteI2c_Byte(0x30,0x7c);
	GC_WriteI2c_Byte(0x31,0x89);
	GC_WriteI2c_Byte(0x32,0x98);
	GC_WriteI2c_Byte(0x33,0xae);
	GC_WriteI2c_Byte(0x34,0xc0);
	GC_WriteI2c_Byte(0x35,0xcf);
	GC_WriteI2c_Byte(0x36,0xda);
	GC_WriteI2c_Byte(0x37,0xe2);
	GC_WriteI2c_Byte(0x38,0xe9);
	GC_WriteI2c_Byte(0x39,0xf3);
	GC_WriteI2c_Byte(0x3a,0xf9);
	GC_WriteI2c_Byte(0x3b,0xff);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0xd1,0x30);
	GC_WriteI2c_Byte(0xd2,0x30);
	GC_WriteI2c_Byte(0xd3,0x45);
	GC_WriteI2c_Byte(0xdd,0x14);
	GC_WriteI2c_Byte(0xde,0x86);
	GC_WriteI2c_Byte(0xed,0x01);
	GC_WriteI2c_Byte(0xee,0x28);
	GC_WriteI2c_Byte(0xef,0x30);
	GC_WriteI2c_Byte(0xd8,0xd8);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0xa1,0x80);
	GC_WriteI2c_Byte(0xa2,0x80);
	GC_WriteI2c_Byte(0xa4,0x00);
	GC_WriteI2c_Byte(0xa5,0x00);
	GC_WriteI2c_Byte(0xa6,0x70);
	GC_WriteI2c_Byte(0xa7,0x00);
	GC_WriteI2c_Byte(0xa8,0x77);
	GC_WriteI2c_Byte(0xa9,0x77);
	GC_WriteI2c_Byte(0xaa,0x1f);
	GC_WriteI2c_Byte(0xab,0x0d);
	GC_WriteI2c_Byte(0xac,0x19);
	GC_WriteI2c_Byte(0xad,0x24);
	GC_WriteI2c_Byte(0xae,0x0e);
	GC_WriteI2c_Byte(0xaf,0x1d);
	GC_WriteI2c_Byte(0xb0,0x12);
	GC_WriteI2c_Byte(0xb1,0x0c);
	GC_WriteI2c_Byte(0xb2,0x06);
	GC_WriteI2c_Byte(0xb3,0x13);
	GC_WriteI2c_Byte(0xb4,0x10);
	GC_WriteI2c_Byte(0xb5,0x0c);
	GC_WriteI2c_Byte(0xb6,0x6a);
	GC_WriteI2c_Byte(0xb7,0x46);
	GC_WriteI2c_Byte(0xb8,0x40);
	GC_WriteI2c_Byte(0xb9,0x0b);
	GC_WriteI2c_Byte(0xba,0x04);
	GC_WriteI2c_Byte(0xbb,0x00);
	GC_WriteI2c_Byte(0xbc,0x53);
	GC_WriteI2c_Byte(0xbd,0x37);
	GC_WriteI2c_Byte(0xbe,0x2d);
	GC_WriteI2c_Byte(0xbf,0x0a);
	GC_WriteI2c_Byte(0xc0,0x0a);
	GC_WriteI2c_Byte(0xc1,0x14);
	GC_WriteI2c_Byte(0xc2,0x34);
	GC_WriteI2c_Byte(0xc3,0x22);
	GC_WriteI2c_Byte(0xc4,0x18);
	GC_WriteI2c_Byte(0xc5,0x23);
	GC_WriteI2c_Byte(0xc6,0x0f);
	GC_WriteI2c_Byte(0xc7,0x3c);
	GC_WriteI2c_Byte(0xc8,0x20);
	GC_WriteI2c_Byte(0xc9,0x1f);
	GC_WriteI2c_Byte(0xca,0x17);
	GC_WriteI2c_Byte(0xcb,0x2d);
	GC_WriteI2c_Byte(0xcc,0x12);
	GC_WriteI2c_Byte(0xcd,0x20);
	GC_WriteI2c_Byte(0xd0,0x61);
	GC_WriteI2c_Byte(0xd1,0x2f);
	GC_WriteI2c_Byte(0xd2,0x39);
	GC_WriteI2c_Byte(0xd3,0x45);
	GC_WriteI2c_Byte(0xd4,0x2c);
	GC_WriteI2c_Byte(0xd5,0x21);
	GC_WriteI2c_Byte(0xd6,0x64);
	GC_WriteI2c_Byte(0xd7,0x2d);
	GC_WriteI2c_Byte(0xd8,0x30);
	GC_WriteI2c_Byte(0xd9,0x42);
	GC_WriteI2c_Byte(0xda,0x27);
	GC_WriteI2c_Byte(0xdb,0x13);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x4f,0x00);
	GC_WriteI2c_Byte(0x4f,0x00);
	GC_WriteI2c_Byte(0x4b,0x01);
	GC_WriteI2c_Byte(0x4f,0x00);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x6f);
	GC_WriteI2c_Byte(0x4e,0x02);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x70);
	GC_WriteI2c_Byte(0x4e,0x02);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x8f);
	GC_WriteI2c_Byte(0x4e,0x02);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x90);
	GC_WriteI2c_Byte(0x4e,0x02);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xed);
	GC_WriteI2c_Byte(0x4e,0x33);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xcd);
	GC_WriteI2c_Byte(0x4e,0x33);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xec);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x6c);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x6d);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x6e);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x8c);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x8d);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0x8e);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xab);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xac);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xad);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xae);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xcb);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xcc);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xce);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xeb);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xec);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xee);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x0c);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x0d);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xea);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xaf);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xcf);
	GC_WriteI2c_Byte(0x4e,0x03);
	GC_WriteI2c_Byte(0x4c,0x01);
	GC_WriteI2c_Byte(0x4d,0xca);
	GC_WriteI2c_Byte(0x4e,0x04);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x0b);
	GC_WriteI2c_Byte(0x4e,0x05);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xc8);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xa8);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xa9);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x89);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x69);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0x6a);
	GC_WriteI2c_Byte(0x4e,0x06);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xc7);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xe7);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x07);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xe8);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x02);
	GC_WriteI2c_Byte(0x4d,0xe9);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x08);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x09);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x27);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x28);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x29);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x47);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x48);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x49);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x67);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x68);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4c,0x03);
	GC_WriteI2c_Byte(0x4d,0x69);
	GC_WriteI2c_Byte(0x4e,0x07);
	GC_WriteI2c_Byte(0x4f,0x01);
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x50,0x80);
	GC_WriteI2c_Byte(0x51,0xa8);
	GC_WriteI2c_Byte(0x52,0x57);
	GC_WriteI2c_Byte(0x53,0x38);
	GC_WriteI2c_Byte(0x54,0xc7);
	GC_WriteI2c_Byte(0x56,0x0e);
	GC_WriteI2c_Byte(0x58,0x08);
	GC_WriteI2c_Byte(0x5b,0x00);
	GC_WriteI2c_Byte(0x5c,0x74);
	GC_WriteI2c_Byte(0x5d,0x8b);
	GC_WriteI2c_Byte(0x61,0xd3);
	GC_WriteI2c_Byte(0x62,0xb5);
	GC_WriteI2c_Byte(0x63,0x00);
	GC_WriteI2c_Byte(0x65,0x04);
	GC_WriteI2c_Byte(0x67,0xb2);
	GC_WriteI2c_Byte(0x68,0xac);
	GC_WriteI2c_Byte(0x69,0x00);
	GC_WriteI2c_Byte(0x6a,0xb2);
	GC_WriteI2c_Byte(0x6b,0xac);
	GC_WriteI2c_Byte(0x6c,0xb2);
	GC_WriteI2c_Byte(0x6d,0xac);
	GC_WriteI2c_Byte(0x6e,0x40);
	GC_WriteI2c_Byte(0x6f,0x18);
	GC_WriteI2c_Byte(0x73,0x00);
	GC_WriteI2c_Byte(0x70,0x10);
	GC_WriteI2c_Byte(0x71,0xe8);
	GC_WriteI2c_Byte(0x72,0xc0);
	GC_WriteI2c_Byte(0x74,0x01);
	GC_WriteI2c_Byte(0x75,0x01);
	GC_WriteI2c_Byte(0x7f,0x08);
	GC_WriteI2c_Byte(0x76,0x70);
	GC_WriteI2c_Byte(0x77,0x58);
	GC_WriteI2c_Byte(0x78,0xa0);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0xc0,0x01);
	GC_WriteI2c_Byte(0xc1,0x50);
	GC_WriteI2c_Byte(0xc2,0xf9);
	GC_WriteI2c_Byte(0xc3,0x00);
	GC_WriteI2c_Byte(0xc4,0xe8);
	GC_WriteI2c_Byte(0xc5,0x48);
	GC_WriteI2c_Byte(0xc6,0xf0);
	GC_WriteI2c_Byte(0xc7,0x50);
	GC_WriteI2c_Byte(0xc8,0xf2);
	GC_WriteI2c_Byte(0xc9,0x00);
	GC_WriteI2c_Byte(0xca,0xe0);
	GC_WriteI2c_Byte(0xcb,0x45);
	GC_WriteI2c_Byte(0xcc,0xec);
	GC_WriteI2c_Byte(0xcd,0x45);
	GC_WriteI2c_Byte(0xce,0xf0);
	GC_WriteI2c_Byte(0xcf,0x00);
	GC_WriteI2c_Byte(0xe3,0xf0);
	GC_WriteI2c_Byte(0xe4,0x45);
	GC_WriteI2c_Byte(0xe5,0xe8);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xf2,0x0f);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xf7,0x1d);
	GC_WriteI2c_Byte(0xfa,0x00);
	GC_WriteI2c_Byte(0xf8,0x84); // 0x83
	GC_WriteI2c_Byte(0x03,0x04);
	GC_WriteI2c_Byte(0x04,0xe2);
	GC_WriteI2c_Byte(0x05,0x01);
	GC_WriteI2c_Byte(0x06,0x56); // 0x3e
	GC_WriteI2c_Byte(0x07,0x00);
	GC_WriteI2c_Byte(0x08,0x32); // 0x14
	GC_WriteI2c_Byte(0xfe,0x01);
	GC_WriteI2c_Byte(0x25,0x00);
	GC_WriteI2c_Byte(0x26,0xfa); // 0x66
	GC_WriteI2c_Byte(0x27,0x04);
	GC_WriteI2c_Byte(0x28,0x62);
	GC_WriteI2c_Byte(0x29,0x04);
	GC_WriteI2c_Byte(0x2a,0x62);
	GC_WriteI2c_Byte(0x2b,0x04);
	GC_WriteI2c_Byte(0x2c,0x62);
	GC_WriteI2c_Byte(0x2d,0x04);
	GC_WriteI2c_Byte(0x2e,0x62);
	GC_WriteI2c_Byte(0x3c,0x00);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0xfe,0x00);
	GC_WriteI2c_Byte(0x18,0x22);
	GC_WriteI2c_Byte(0xfe,0x02);
	GC_WriteI2c_Byte(0x40,0xbf);
	GC_WriteI2c_Byte(0x46,0xcf);
	GC_WriteI2c_Byte(0xfe,0x00);	
#endif
}

void GC2145Terminate(void)
{
    /* Please implement terminate code here */
}

void GC2145OutputPinTriState(uint8_t flag)
{
    if (flag == true)
    {
        /* Please implement outputpintristate code here */
    }
    else
    {
        /* Please implement disable outputpintristate code here */
    }
}

uint8_t GC2145IsSignalStable(uint16_t Mode)
{
    bool isStable = true;
    /* Please implement checking signal stable code here */
    /* stable return true else return false                       */
    return isStable;
}

uint16_t GC2145GetProperty(MODULE_GETPROPERTY property)
{
    /* Please implement get information from device code here */
    switch (property)
    {
    //case GetTopFieldPolarity:
    case GetHeight:
		return 1200;
    case GetWidth:
		return 1600;
    //frame rate
    case Rate:
		return 2397;
    case GetModuleIsInterlace:
		return 0;
    case matchResolution:
		return 1;
	case GetIsAnalogDecoder:
		return 0;
    default:
        return 0;
        break;
    }
}

uint8_t GC2145GetStatus(MODULE_GETSTATUS Status)
{
    /* Please implement get status from device code here */
    switch (Status)
    {
    //case IsPowerDown:
    //case IsSVideoInput:
    default:
        return 0;
        break;
    }
}

void GC2145SetProperty(MODULE_SETPROPERTY Property, uint16_t Value)
{
    /* Please implement set property to device code here */
}

void GC2145PowerDown(uint8_t enable)
{
    /* Please implement power down code here */
#ifdef CFG_SENSOR_MCLKPIN_ENABLE
    ithGpioSetOut(CFG_SN1_GPIO_MCLK);
    ithGpioSetMode(CFG_SN1_GPIO_MCLK, ITH_GPIO_MODE2);
	usleep(100*1000);
#endif

#ifdef CFG_SENSOR_POWERDOWNPIN_ENABLE
    //Power ON
    if(enable == false)
    {
    	printf("[GC2145]power on\n");
    	ithGpioClear(CFG_SN1_GPIO_PWN);
    	ithGpioSetOut(CFG_SN1_GPIO_PWN);
    	ithGpioSetMode(CFG_SN1_GPIO_PWN, ITH_GPIO_MODE0);
    }
	else
	{
    	printf("[GC2145]power off\n");
    	ithGpioSet(CFG_SN1_GPIO_PWN);
    	ithGpioSetOut(CFG_SN1_GPIO_PWN);
    	ithGpioSetMode(CFG_SN1_GPIO_PWN, ITH_GPIO_MODE0);
#ifdef CFG_SENSOR_RESETPIN_ENABLE
		ithGpioClear(CFG_SN1_GPIO_RST);
		ithGpioSetOut(CFG_SN1_GPIO_RST);
		ithGpioSetMode(CFG_SN1_GPIO_RST, ITH_GPIO_MODE0);
#endif
	}
#endif
}

//=============================================================================
//                GC2145 IMPLEMENT FUNCTION END
//=============================================================================
static void GC2145SensorDriver_Destory(SensorDriver base)
{
    SensorDriver self = (SensorDriver)base;
    if (self)
        free(self);
}

/* assign callback funciton */
static SensorDriverInterfaceStruct interface =
{
    GC2145Initialize,
    GC2145Terminate,
    GC2145OutputPinTriState,
    GC2145IsSignalStable,
    GC2145GetProperty,
    GC2145GetStatus,
    GC2145SetProperty,
    GC2145PowerDown,
    GC2145SensorDriver_Destory,
};

SensorDriver GC2145SensorDriver_Create()
{
    GC2145SensorDriver self = calloc(1, sizeof(GC2145DriverStruct));
    self->base.vtable = &interface;
    self->base.type   = "GC2145";
    return (SensorDriver)self;
}

//end of X10LightDriver_t1.c