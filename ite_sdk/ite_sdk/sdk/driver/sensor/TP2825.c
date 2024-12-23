/**
 *
 */

#if defined(CFG_SENSOR_ENABLE)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "iic/mmp_iic.h"
#include "TP2825.h"

#include "tp2802_def.h"
#include "tp2825B.h"




//BIT0 - BIT31
#define C_BIT0                                            0x01
#define C_BIT1                                            0x02
#define C_BIT3                                            0x08
#define C_BIT2                                            0x04
#define C_BIT4                                            0x10
#define C_BIT5                                            0x20
#define C_BIT6                                            0x40
#define C_BIT7                                            0x80


#define KKM_AUTO_SCAN_ENABLE            0

//[JAVISTECH_20191128 KKM(debug)
#if 1 
#define CAM_DISP(X) ithPrintf("%s(%d)_%s\n",__FUNCTION__,__LINE__,X)
#define CAM_NUM_DISP(X) ithPrintf("%s(%d)_%d\n",__FUNCTION__,__LINE__,X)
#define LAW_FILE_TRACE  	//ithPrintf("[LAW]CALL_ENTRY(%d,%d)\r\n", i,__LINE__);

#else
#define CAM_DISP(X) 
#define CAM_NUM_DISP(X) 
#endif 
//]JAVISTECH_20191128 KKM_End(debug)

#define TP2825_DUMP 	0
#define TP2825_DEBUG   	0         //printk debug information on/off
#define CVBS_960H       1



#define DEFAULT_FORMAT      TP2802_720P30V2		// TP2802_720P30V2  TP2802_NTSC


static int HDC_enable = 1;
static int mode = DEFAULT_FORMAT;
static int chips = 1;
static int output[] = {
	//EMB422_16BIT, //for TP2825B
    //MIPI_2LANES,  //for TP2850
   	SEP656_8BIT
};

static unsigned int id[MAX_CHIPS];

//#define TP2825B_I2C_A 	0x88
//#define TP2825B_I2C_B 	0x8A

unsigned char tp2802_i2c_addr[] = { TP2825B_I2C_A, TP2825B_I2C_B };


#define TP2802_I2C_ADDR(chip_id)    (tp2802_i2c_addr[chip_id])


typedef struct
{
    unsigned int	count[CHANNELS_PER_CHIP];
    unsigned int	mode[CHANNELS_PER_CHIP];
    unsigned int    scan[CHANNELS_PER_CHIP];
    unsigned int	gain[CHANNELS_PER_CHIP][4];
    unsigned int    std[CHANNELS_PER_CHIP];
    unsigned int    state[CHANNELS_PER_CHIP];
    unsigned int    force[CHANNELS_PER_CHIP];
    unsigned char 	addr;
} tp2802wd_info;


static tp2802wd_info watchdog_info[MAX_CHIPS];
volatile static unsigned int watchdog_state = 0;

#define WATCHDOG_EXIT    0
#define WATCHDOG_RUNNING 1
#define WDT              1



//=============================================================================
//                Constant Definition
//=============================================================================
//static uint8_t   TP2825_IICADDR = 0x88 >> 1;
static uint8_t   TP2825_IIC_PORT = SENSOR_IIC_3;  /* please assign IIC PORT      */

static pthread_mutex_t gSensorMutex  = PTHREAD_MUTEX_INITIALIZER;
static bool _lockStatus = false;
//=============================

#define STATE_PRE_DETECTION	0xFF
#define STATE_IDLE   		0
#define STATE_INITIALIZING  1
#define STATE_DETECTING	  	2
#define STATE_LOCKING	 	3
#define STATE_RUNNING	  	4

static uint8_t control_state = STATE_PRE_DETECTION;

//=================================
static uint8_t request_channel; //channel request by application

static uint32_t Time_Current_Tick = 0;
static uint32_t Time_Last_Tick = 0; 

static int _tp28xx_isr = 0;

static void TP2825GpioInit(void);

//=============================================================================
//                Macro Definition
//=============================================================================
//#define REGFLAG_DELAY        0X00
#define REGFLAG_END_OF_TABLE   0xFF
#define END_MARKER             {REGFLAG_END_OF_TABLE, 0x00},

//=============================================================================
//                Structure Definition
//=============================================================================


struct CAMERA_INPUT_FORAMT
{
    unsigned int  mode;
    unsigned int  std;
    unsigned int  state;
	unsigned int  reserved;
};


#define TP2825_USED_CHANNEL     1	//2	//2 // USE THIS for REAL DESIGN

static struct CAMERA_INPUT_FORAMT input_format[TP2825_USED_CHANNEL];
//static uint8_t cam_format[8]={INVALID_FORMAT}; // 8 channels


typedef struct _REGPAIR
{
    uint8_t  addr;
    uint16_t value;
} REGPAIR;


typedef struct TP2825SensorDriverStruct
{
	SensorDriverStruct base;
} TP2825SensorDriverStruct;

//=============================================================================
//                IIC API FUNCTION START
//=============================================================================
static uint8_t tp28xx_byte_read(unsigned char chip, unsigned char addr)
{
	unsigned char chip_addr;
	chip_addr = TP2802_I2C_ADDR(chip) >> 1;

	int     result;
	uint8_t dbuf[256];
	uint8_t *pdbuf = dbuf;
	uint8_t value;
	char        portname[4];
	ITPI2cInfo  evt;
	int         gMasterDev = 0;
	pthread_mutex_lock(&gSensorMutex);

	sprintf(portname, ":i2c%d", TP2825_IIC_PORT);
	
	
//	printf("TP2825_i2c Port : %s , %d\n",portname, TP2825_IIC_PORT);
	gMasterDev = open(portname, 0);

	*pdbuf++ = (uint8_t)(addr);
    
	evt.slaveAddress   = chip_addr;       //弱띷????°salve address
	evt.cmdBuffer	   = dbuf;				   //轝꿨궠�걿±slave???°ｿªdata buffer(�똿�맜cmooand)
	evt.cmdBufferSize	= 1;				   //�궠�갺ata size,�뼫鵝띸궨byte
	evt.dataBuffer	   = pdbuf; 			   //轝꿩?�뵸slave???°ｿªdata buffer
	evt.dataBufferSize	= 1;				   //�렏�뵸data size,�뼫鵝띸궨byte
	
	result = read(gMasterDev, &evt, 1);

	if (result != 0)
	{
		printf("TP2825_ReadI2C_Byte read address 0x%02x error!\n", addr);

		pthread_mutex_unlock(&gSensorMutex);
		return 0;
	}

	value = (dbuf[1] & 0xFF);

	//printf("value = %d",value);
    
	pthread_mutex_unlock(&gSensorMutex);
	
    return value;
	
}


static unsigned int ConvertACPV1Data(unsigned char dat)
{
    unsigned int i, tmp=0;
    for(i = 0; i < 8; i++)
    {
        tmp <<= 3;

        if(0x01 & dat) tmp |= 0x06;
        else tmp |= 0x04;

        dat >>= 1;
    }
    return tmp;
}


static int tp28xx_byte_write(unsigned char chip, unsigned char addr, unsigned char data)
{
	unsigned char chip_addr;
	chip_addr = TP2802_I2C_ADDR(chip) >> 1;
	int     result;
	uint8_t dbuf[256];
	uint8_t *pdbuf = dbuf;
	char        portname[4];
	ITPI2cInfo  evt;
	int         gMasterDev = 0;
	pthread_mutex_lock(&gSensorMutex);

	sprintf(portname, ":i2c%d", TP2825_IIC_PORT);
	gMasterDev = open(portname, 0);    

	*pdbuf++ = (uint8_t)(addr);
	*pdbuf   = (uint8_t)(data);
    
	evt.slaveAddress   = chip_addr;	   //弱띷????°salve address
	evt.cmdBuffer	   = dbuf;				   //轝꿨궠�걿±slave???°ｿªdata buffer(�똿�맜cmooand)
	evt.cmdBufferSize  = 2; 				   //�궠�갺ata size,�뼫鵝띸궨byte
	
	if(0 != (result = write(gMasterDev, &evt, 1)))
	{
		printf("TP2825_WriteI2C_8Bit I2c Write Error, reg=%02x val=%02x\n", addr, data);
	}

	pthread_mutex_unlock(&gSensorMutex);

	return result;
}


static void tp2802_write_table(unsigned char chip,
                               unsigned char addr, unsigned char *tbl_ptr, unsigned char tbl_cnt)
{
    unsigned char i = 0;
    for(i = 0; i < tbl_cnt; i ++)
    {
        tp28xx_byte_write(chip, (addr + i), *(tbl_ptr + i));
    }
}






//=============================================================================
//                IIC API FUNCTION END
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
//148.5 M for 1080p
static REGPAIR TP2825B_SYSCLK_V1_tbl[] = 
{
	{0x40, 0x08},
	{0x13, 0x04},
	{0x14, 0x04},
	{0x40, 0x00},
	{REGFLAG_END_OF_TABLE, 0x00},
};

//[JAVISTECH_20190828 KKM(cam_tuning)
#if 1 

//unotel
static REGPAIR TP2825_1080p_half[] = 
{
	//TP2825B dataset
	{0x40, 0x00}, 
	{0x35, 0x25}, 
	{0x4e, 0x05}, //0x4e, 0x0d, //unotel 8bit
	{0xf5, 0x10},
	{0xfd, 0x80},
	{0xf6, 0x00}, //16bit output                  
	{0xf7, 0x00}, //0xf7, 0x44, //unotel 8bit     
	{0xfa, 0x88}, //0xfa, 0xbb, //unotel 8bit     
	{0x4f, 0x01}, // IRQ pin otuput enable        
	{0xbe, 0x80}, // IRQCLRMD                     
	//reset default v1                         
	{0x40, 0x08}, 
	{0x13, 0x04}, 
	{0x14, 0x04}, 
	{0x40, 0x00}, 
	{0x07, 0xc0}, 
	{0x0b, 0xc0}, 
	{0x26, 0x04}, 
	//v1 end           

	//A1080p30
	//V1
	{0x40, 0x08},
	{0x13, 0x04},
	{0x14, 0x04},  
	{0x40, 0x00},  
	//format dataset
	{0x02, 0xcc},   
	{0x0c, 0x03},    
	{0x0d, 0x72},    
	{0x15, 0x01},    
	{0x16, 0xf0},    
	{0x17, 0x80},    
	{0x18, 0x29},    
	{0x19, 0x38},    
	{0x1a, 0x47},    
	{0x1c, 0x08},    
	{0x1d, 0x98},    
	{0x20, 0x38},    
	{0x21, 0x46},    
	{0x22, 0x36},    
	{0x23, 0x3c},    
	{0x25, 0xfe},    
	{0x26, 0x0d},    
	{0x27, 0x2d},    
	{0x28, 0x00},    
	{0x2b, 0x60},    
	{0x2c, 0x3a},    
	{0x2d, 0x54},    
	{0x2e, 0x40},    
	{0x30, 0xa5},    
	{0x31, 0x95},    
	{0x32, 0xe0},    
	{0x33, 0x60},    
	{0x35, 0x05},    
	{0x36, 0xdc},    
	{0x00, 0x00},    
	{0x39, 0x1c},    
	{0x3a, 0x32},  
	//half mode
	{0x35,0x45}, 
	{0xFA,0x0B}, //0x03
	{REGFLAG_END_OF_TABLE, 0x00}
};
#endif 
//]JAVISTECH_20190828 KKM_End(cam_tuning)




static void tp2802_set_work_mode_1080p25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_1080p25_raster, 9);
}

static void tp2802_set_work_mode_1080p30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_1080p30_raster, 9);
}


static void tp2802_set_work_mode_720p25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p25_raster, 9);
}

static void tp2802_set_work_mode_720p30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p30_raster, 9);
}

static void tp2802_set_work_mode_720p50(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p50_raster, 9);
}

static void tp2802_set_work_mode_720p60(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p60_raster, 9);
}

static void tp2802_set_work_mode_PAL(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_PAL_raster, 9);
}

static void tp2802_set_work_mode_NTSC(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_NTSC_raster, 9);
}
static void tp2802_set_work_mode_3M(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_3M_raster, 9);
}

static void tp2802_set_work_mode_5M(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_5M_raster, 9);
}
static void tp2802_set_work_mode_4M(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4M_raster, 9);
}
static void tp2802_set_work_mode_3M20(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_3M20_raster, 9);
}
static void tp2802_set_work_mode_4M12(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4M12_raster, 9);
}
static void tp2802_set_work_mode_6M10(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_6M10_raster, 9);
}
static void tp2802_set_work_mode_QHDH30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QHDH30_raster, 9);
}
static void tp2802_set_work_mode_QHDH25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QHDH25_raster, 9);
}
static void tp2802_set_work_mode_QHD15(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QHD15_raster, 9);
}
static void tp2802_set_work_mode_QXGAH30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QXGAH30_raster, 9);
}
static void tp2802_set_work_mode_QXGAH25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QXGAH25_raster, 9);
}
static void tp2802_set_work_mode_QHD30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QHD30_raster, 9);
}
static void tp2802_set_work_mode_QHD25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QHD25_raster, 9);
}
static void tp2802_set_work_mode_QXGA30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QXGA30_raster, 9);
}
static void tp2802_set_work_mode_QXGA25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_QXGA25_raster, 9);
}
/*
static void tp2802_set_work_mode_4M30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4M30_raster, 9);
}
static void tp2802_set_work_mode_4M25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4M25_raster, 9);
}
*/
static void tp2802_set_work_mode_5M20(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_5M20_raster, 9);
}
static void tp2802_set_work_mode_5MH20(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_5MH20_raster, 9);
}
/*
static void tp2802_set_work_mode_4MH30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4MH30_raster, 9);
}
static void tp2802_set_work_mode_4MH25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_4MH25_raster, 9);
}
*/
static void tp2802_set_work_mode_8M15(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_8M15_raster, 9);
}
static void tp2802_set_work_mode_8MH15(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_8MH15_raster, 9);
}
static void tp2802_set_work_mode_8M12(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_8M12_raster, 9);
}
static void tp2802_set_work_mode_8MH12(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_8MH12_raster, 9);
}
static void tp2802_set_work_mode_720p30HDR(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p30HDR_raster, 9);
}
static void tp2802_set_work_mode_6M20(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_6M20_raster, 9);
}


static void TP2825B_Set_REG0X02(unsigned char chip, unsigned char data )
{
	if(MUX656_8BIT == output[chip] || SEP656_8BIT == output[chip])
	{
		tp28xx_byte_write(chip, 0x02, data);
	}
	else
	{
		tp28xx_byte_write(chip, 0x02, data&0x7f);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

static void TP2826_C1080P25_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x50);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa2);

}

static void TP2826_C720P25_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3a);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0x36);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x67);
    tp28xx_byte_write(chip, 0x32, 0x6f);
    tp28xx_byte_write(chip, 0x33, 0x33);

}
static void TP2826_C720P50_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3a);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x42);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa3);

}
static void TP2826_C1080P30_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3c);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x4c);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa4);

}
static void TP2826_C720P30_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x30);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x37);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x67);
    tp28xx_byte_write(chip, 0x32, 0x6f);
    tp28xx_byte_write(chip, 0x33, 0x30);

}
static void TP2826_C720P60_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x30);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x37);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa0);

}
//////////////////////////////////////////////////////////////////////////////
//tp2825b table set
//////////////////////////////////////////////////////////////////////////////

static void TP2825B_output(unsigned char chip)
{

    if( MUX656_8BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
		tp28xx_byte_write(chip, 0x4C, 0x40);
		tp28xx_byte_write(chip, 0x4e, 0x05); //port1
		tp28xx_byte_write(chip, 0xf6, 0x00);
		tp28xx_byte_write(chip, 0xf7, 0x00);

    }
    else if( SEP656_8BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
		tp28xx_byte_write(chip, 0x4C, 0x33);
		tp28xx_byte_write(chip, 0x4e, 0x05); //port1
		tp28xx_byte_write(chip, 0xf6, 0x00);
		tp28xx_byte_write(chip, 0xf7, 0x00);
    }
    else if( EMB422_16BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
		tp28xx_byte_write(chip, 0x4C, 0x40);
		tp28xx_byte_write(chip, 0x4e, 0x0d);
		tp28xx_byte_write(chip, 0xf6, 0x00);
		tp28xx_byte_write(chip, 0xf7, 0x44);

    }
    else if( SEP422_16BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
        tp28xx_byte_write(chip, 0x4C, 0x43);
        tp28xx_byte_write(chip, 0x4e, 0x0d);
        tp28xx_byte_write(chip, 0xf6, 0x00);
        tp28xx_byte_write(chip, 0xf7, 0x44);
    }

}
static void TP2850_output(unsigned char chip)
{

    if( MUX656_8BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
        tp28xx_byte_write(chip, 0x4C, 0x40);
        tp28xx_byte_write(chip, 0x4e, 0x05); //port1
        tp28xx_byte_write(chip, 0xf6, 0x00);
        tp28xx_byte_write(chip, 0xf7, 0x00);

    }
    else if( SEP656_8BIT == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x00, 0x00);
		tp28xx_byte_write(chip, 0x08, 0xf0);
		tp28xx_byte_write(chip, 0x40, 0x00);
        tp28xx_byte_write(chip, 0x4C, 0x33);
        tp28xx_byte_write(chip, 0x4e, 0x05); //port1
        tp28xx_byte_write(chip, 0xf6, 0x00);
        tp28xx_byte_write(chip, 0xf7, 0x00);
    }
    else if( MIPI_2LANES == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x01, 0xf8);
		tp28xx_byte_write(chip, 0x02, 0x01);
		tp28xx_byte_write(chip, 0x08, 0x03);
		tp28xx_byte_write(chip, 0x20, 0x12);
		tp28xx_byte_write(chip, 0x40, 0x00);
        tp28xx_byte_write(chip, 0x4e, 0x00);

    }

}
static void TP2825B_SYSCLK_V2(unsigned char chip)
{
    if(MIPI_2LANES == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x02);
		tp28xx_byte_write(chip, 0x13, 0x24);
		tp28xx_byte_write(chip, 0x14, 0x46);
		tp28xx_byte_write(chip, 0x15, 0x09);

		tp28xx_byte_write(chip, 0x25, 0x08);
		tp28xx_byte_write(chip, 0x26, 0x01);
		tp28xx_byte_write(chip, 0x27, 0x0e);

		tp28xx_byte_write(chip, 0x10, 0x88);
		tp28xx_byte_write(chip, 0x10, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x00);
		tp28xx_byte_write(chip, 0x40, 0x00);
    }
    else
    {
//[JAVISTECH_20191216 KKM(PCLK_Change)
#if 0 
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x13, 0x24);
		tp28xx_byte_write(chip, 0x14, 0x73);
		tp28xx_byte_write(chip, 0x40, 0x00);
		//tp28xx_byte_write(chip, 0x35, 0x25);
#else
		tp28xx_byte_write(chip, 0xfa, 0x88);
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x13, 0x24);
		tp28xx_byte_write(chip, 0x14, 0x04);
		tp28xx_byte_write(chip, 0x40, 0x00);
#endif 
//]JAVISTECH_20191216 KKM_End(PCLK_Change)
    }

}
static void TP2825B_SYSCLK_V1(unsigned char chip)
{
    if(MIPI_2LANES == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x02);
		tp28xx_byte_write(chip, 0x13, 0x04);
		tp28xx_byte_write(chip, 0x14, 0x46);
		tp28xx_byte_write(chip, 0x15, 0x09);

		tp28xx_byte_write(chip, 0x25, 0x10);
		tp28xx_byte_write(chip, 0x26, 0x02);
		tp28xx_byte_write(chip, 0x27, 0x16);

		tp28xx_byte_write(chip, 0x10, 0x88);
		tp28xx_byte_write(chip, 0x10, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x00);
		tp28xx_byte_write(chip, 0x40, 0x00);
    }
    else
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x13, 0x04);
//[JAVISTECH_20191128 KKM(kkm_modify)
#if 1 
		tp28xx_byte_write(chip, 0x14, 0x04);
#else
		tp28xx_byte_write(chip, 0x14, 0x73);
#endif 
//]JAVISTECH_20191128 KKM_End(kkm_modify)
		tp28xx_byte_write(chip, 0x40, 0x00);
		//tp28xx_byte_write(chip, 0x35, 0x05);
    }


}
static void TP2825B_SYSCLK_V3(unsigned char chip)
{
    unsigned char tmp;

    if(MIPI_2LANES == output[chip] )
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x02);
		tp28xx_byte_write(chip, 0x13, 0x04);
		tp28xx_byte_write(chip, 0x14, 0x05);
		tp28xx_byte_write(chip, 0x15, 0x04);

		tp28xx_byte_write(chip, 0x25, 0x10);
		tp28xx_byte_write(chip, 0x26, 0x06);
		tp28xx_byte_write(chip, 0x27, 0x16);

		tp28xx_byte_write(chip, 0x10, 0x88);
		tp28xx_byte_write(chip, 0x10, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x00);
		tp28xx_byte_write(chip, 0x40, 0x00);
    }
    else
    {
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x13, 0x04);
		tp28xx_byte_write(chip, 0x14, 0x73);
		tp28xx_byte_write(chip, 0x40, 0x00);

        if(MUX656_8BIT == output[chip] || SEP656_8BIT == output[chip])
        {
            tmp = tp28xx_byte_read(chip, 0x35);
            tmp |= 0x40;
            tp28xx_byte_write(chip, 0x35, 0x05);
        }
    }
}


static void TP2825B_RX_init(unsigned char chip, unsigned char mod)
{

    int i, index=0;
    unsigned char regA7=0x00;
    unsigned char regA8=0x00;

    //regC9~D7
    static const unsigned char PTZ_RX_dat[][15]=
    {
        {0x00,0x00,0x07,0x08,0x00,0x00,0x04,0x00,0x00,0x60,0x10,0x06,0xbe,0x39,0x27}, //TVI command
        {0x00,0x00,0x07,0x08,0x09,0x0a,0x04,0x00,0x00,0x60,0x10,0x06,0xbe,0x39,0x27}, //TVI burst
        {0x00,0x00,0x06,0x07,0x08,0x09,0x05,0xbf,0x11,0x60,0x0b,0x04,0xf0,0xd8,0x2f}, //ACP1 0.525
        {0x00,0x00,0x06,0x07,0x08,0x09,0x02,0xdf,0x88,0x60,0x10,0x04,0xf0,0xd8,0x17}, //ACP2 0.6
        {0x00,0x00,0x06,0x07,0x08,0x09,0x04,0xec,0xe9,0x60,0x10,0x04,0xf0,0xd8,0x17}, //ACP3 0.35
        {0x00,0x00,0x06,0x07,0x08,0x09,0x03,0x52,0x53,0x60,0x10,0x04,0xf0,0xd8,0x17}  //test
    };

        if(PTZ_RX_TVI_CMD == mod)
        {
            index = 0;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_TVI_BURST == mod)
        {
            index = 1;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_ACP1 == mod)
        {
            index = 2;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_ACP2 == mod)
        {
            index = 3;
            regA7 = 0x27;
            regA8 = 0x0f;
        }
        else if(PTZ_RX_ACP3 == mod)
        {
            index = 4;
            regA7 = 0x27;
            regA8 = 0x0f;
        }
        else if(PTZ_RX_TEST == mod)
        {
            index = 5;
            regA7 = 0x03;
            regA8 = 0x00;
        }

        for(i = 0; i < 15; i++)
        {
            tp28xx_byte_write(chip, 0xc9+i, PTZ_RX_dat[index][i]);
            tp28xx_byte_write(chip, 0xa8, regA8);
            tp28xx_byte_write(chip, 0xa7, regA7);
        }

}
static void TP2825B_reset_default(unsigned char chip, unsigned char ch)
{
    unsigned int tmp;
    tp28xx_byte_write(chip, 0x07, 0xC0);
    tp28xx_byte_write(chip, 0x0B, 0xC0);
    tmp = tp28xx_byte_read(chip, 0x26);
    tmp &= 0xfe;
    tp28xx_byte_write(chip, 0x26, tmp);
    tmp = tp28xx_byte_read(chip, 0xa7);
    tmp &= 0xfe;
    tp28xx_byte_write(chip, 0xa7, tmp);
    tmp = tp28xx_byte_read(chip, 0x06);
    tmp &= 0xfb;
    tp28xx_byte_write(chip, 0x06, tmp);
}
//////////////////////////////////////////////////////////////
static void TP2825B_NTSC_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x70);
    tp28xx_byte_write(chip, 0x2c, 0x2a);
    tp28xx_byte_write(chip, 0x2d, 0x68);
    tp28xx_byte_write(chip, 0x2e, 0x57);

    tp28xx_byte_write(chip, 0x30, 0x62);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x96);
    tp28xx_byte_write(chip, 0x33, 0xc0);
    tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x04);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x18, 0x12);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
static void TP2825B_PAL_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x51);

    tp28xx_byte_write(chip, 0x20, 0x48);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x37);
    tp28xx_byte_write(chip, 0x23, 0x3f);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x70);
    tp28xx_byte_write(chip, 0x2c, 0x2a);
    tp28xx_byte_write(chip, 0x2d, 0x64);
    tp28xx_byte_write(chip, 0x2e, 0x56);

    tp28xx_byte_write(chip, 0x30, 0x7a);
    tp28xx_byte_write(chip, 0x31, 0x4a);
    tp28xx_byte_write(chip, 0x32, 0x4d);
    tp28xx_byte_write(chip, 0x33, 0xf0);
    tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x04);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x18, 0x17);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
static void TP2825B_V1_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x30);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x30);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x2e);
    tp28xx_byte_write(chip, 0x33, 0x90);
    tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x1c);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
static void TP2825B_V2_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x30);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x30);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x2e);
    tp28xx_byte_write(chip, 0x33, 0x90);
    tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x18);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}

/////HDA QHD30
static void TP2825B_AQHDP30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tp28xx_byte_write(chip, 0x15, 0x23);
    tp28xx_byte_write(chip, 0x16, 0x16);
    tp28xx_byte_write(chip, 0x18, 0x32);

    tp28xx_byte_write(chip, 0x20, 0x80);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x36);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0xa0);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x6a);
    tp28xx_byte_write(chip, 0x32, 0xbe);
    tp28xx_byte_write(chip, 0x33, 0x80);
    //tp28xx_byte_write(chip, 0x35, 0x15);
    tp28xx_byte_write(chip, 0x39, 0x40);
}

/////HDA QHD25
static void TP2825B_AQHDP25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tp28xx_byte_write(chip, 0x15, 0x23);
    tp28xx_byte_write(chip, 0x16, 0x16);
    tp28xx_byte_write(chip, 0x18, 0x32);

    tp28xx_byte_write(chip, 0x20, 0x80);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x36);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0xa0);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x6f);
    tp28xx_byte_write(chip, 0x32, 0xb5);
    tp28xx_byte_write(chip, 0x33, 0x80);
    //tp28xx_byte_write(chip, 0x35, 0x15);
    tp28xx_byte_write(chip, 0x39, 0x40);
}

/////HDA QXGA30
static void TP2825B_AQXGAP30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x00);

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x90);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0xa0);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x68);
    tp28xx_byte_write(chip, 0x32, 0xbe);
    tp28xx_byte_write(chip, 0x33, 0x80);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x40);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);
}

/////HDA QXGA25
static void TP2825B_AQXGAP25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x60;
    tp28xx_byte_write(chip, 0x14, tmp);
    tp28xx_byte_write(chip, 0x13, 0x00);

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x90);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0xa0);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x6c);
    tp28xx_byte_write(chip, 0x32, 0xbe);
    tp28xx_byte_write(chip, 0x33, 0x80);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x40);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);
}


/////HDC QHD30
static void TP2825B_CQHDP30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);
    tp28xx_byte_write(chip, 0x15, 0x13);
    tp28xx_byte_write(chip, 0x16, 0xfa);
    tp28xx_byte_write(chip, 0x18, 0x38);
    tp28xx_byte_write(chip, 0x1c, 0x0c);
    tp28xx_byte_write(chip, 0x1d, 0x80);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x38);

    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);

    tp28xx_byte_write(chip, 0x2d, 0x6c);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x31);

    tp28xx_byte_write(chip, 0x39, 0x48);
}

/////HDC QHD25
static void TP2825B_CQHDP25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);
    tp28xx_byte_write(chip, 0x15, 0x13);
    tp28xx_byte_write(chip, 0x16, 0xd8);
    tp28xx_byte_write(chip, 0x18, 0x30);
    tp28xx_byte_write(chip, 0x1c, 0x0c);
    tp28xx_byte_write(chip, 0x1d, 0x80);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x38);

    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);

    tp28xx_byte_write(chip, 0x2d, 0x6c);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x3b);

    tp28xx_byte_write(chip, 0x39, 0x48);
}

///////HDA QHD15
static void TP2825B_AQHDP15_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);
    tp28xx_byte_write(chip, 0x13, 0x00);

    tp28xx_byte_write(chip, 0x20, 0x38);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);

    tp28xx_byte_write(chip, 0x2d, 0x44);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x29);
    tp28xx_byte_write(chip, 0x31, 0x68);
    tp28xx_byte_write(chip, 0x32, 0x78);
    tp28xx_byte_write(chip, 0x33, 0x10);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x40);
    tp28xx_byte_write(chip, 0x3a, 0x12);
}

/////HDA QXGA18
static void TP2825B_AQXGAP18_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);
    tp28xx_byte_write(chip, 0x13, 0x00);

    tp28xx_byte_write(chip, 0x15, 0x13);
    tp28xx_byte_write(chip, 0x16, 0x10);
    tp28xx_byte_write(chip, 0x18, 0x68);

    tp28xx_byte_write(chip, 0x20, 0x48);
    tp28xx_byte_write(chip, 0x21, 0x46);
    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x05);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0x52);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x29);
    tp28xx_byte_write(chip, 0x31, 0x65);
    tp28xx_byte_write(chip, 0x32, 0x2b);
    tp28xx_byte_write(chip, 0x33, 0xd0);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x40);
    tp28xx_byte_write(chip, 0x3a, 0x12);
}

/////TVI QHD30/QHD25
static void TP2825B_QHDP30_25_DataSet(unsigned char chip)
{
    unsigned char tmp;

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x15, 0x23);
    tp28xx_byte_write(chip, 0x16, 0x1b);
    tp28xx_byte_write(chip, 0x18, 0x38);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x58);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x74);
    tp28xx_byte_write(chip, 0x31, 0x58);
    tp28xx_byte_write(chip, 0x32, 0x9f);
    tp28xx_byte_write(chip, 0x33, 0x60);

    tp28xx_byte_write(chip, 0x35, 0x15);
    tp28xx_byte_write(chip, 0x36, 0xdc);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);
}

/////TVI 5M20
static void TP2825B_5MP20_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x74);
    tp28xx_byte_write(chip, 0x31, 0xa7);
    tp28xx_byte_write(chip, 0x32, 0x18);
    tp28xx_byte_write(chip, 0x33, 0x50);

    //tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}

/////HDA 5M20
static void TP2825B_A5MP20_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);
    tp28xx_byte_write(chip, 0x20, 0x80);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x36);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0xA0);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x77);
    tp28xx_byte_write(chip, 0x32, 0x0e);
    tp28xx_byte_write(chip, 0x33, 0xa0);
    tp28xx_byte_write(chip, 0x39, 0x48);
}

/////TVI 8M15
static void TP2825B_8MP15_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x60);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x58);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x74);
    tp28xx_byte_write(chip, 0x31, 0x59);
    tp28xx_byte_write(chip, 0x32, 0xbd);
    tp28xx_byte_write(chip, 0x33, 0x60);

    //tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
///////HDA 5M12.5
static void TP2825B_A5MP12_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);
    tp28xx_byte_write(chip, 0x13, 0x00);

    tp28xx_byte_write(chip, 0x20, 0x38);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);

    tp28xx_byte_write(chip, 0x2d, 0x44);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x29);
    tp28xx_byte_write(chip, 0x31, 0x68);
    tp28xx_byte_write(chip, 0x32, 0x72);
    tp28xx_byte_write(chip, 0x33, 0xb0);

    tp28xx_byte_write(chip, 0x16, 0x10);
    tp28xx_byte_write(chip, 0x18, 0x1a);
    tp28xx_byte_write(chip, 0x1d, 0xb8);
    tp28xx_byte_write(chip, 0x36, 0xbc);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x40);
    tp28xx_byte_write(chip, 0x3a, 0x12);
}
///////////////////////////////////////////////////////////////////
static void TP2825B_A720P30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x0d, 0x70);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);

    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x5a);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x9d);
    tp28xx_byte_write(chip, 0x31, 0xca);
    tp28xx_byte_write(chip, 0x32, 0x01);
    tp28xx_byte_write(chip, 0x33, 0xd0);
}
static void TP2825B_A720P25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x0d, 0x71);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);

    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x5a);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x9e);
    tp28xx_byte_write(chip, 0x31, 0x20);
    tp28xx_byte_write(chip, 0x32, 0x10);
    tp28xx_byte_write(chip, 0x33, 0x90);
}
static void TP2825B_A1080P30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x15, 0x01);
    tp28xx_byte_write(chip, 0x16, 0xf0);

    tp28xx_byte_write(chip, 0x0d, 0x72);

    tp28xx_byte_write(chip, 0x20, 0x38);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x0d);

    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0xa5);
    tp28xx_byte_write(chip, 0x31, 0x95);
    tp28xx_byte_write(chip, 0x32, 0xe0);
    tp28xx_byte_write(chip, 0x33, 0x60);
}
static void TP2825B_A1080P25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x15, 0x01);
    tp28xx_byte_write(chip, 0x16, 0xf0);

    tp28xx_byte_write(chip, 0x0d, 0x73);

    tp28xx_byte_write(chip, 0x20, 0x3c);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x0d);

    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0xa5);
    tp28xx_byte_write(chip, 0x31, 0x86);
    tp28xx_byte_write(chip, 0x32, 0xfb);
    tp28xx_byte_write(chip, 0x33, 0x60);
}
static void TP2825B_1080P60_DataSet(unsigned char chip)
{
    unsigned char tmp;

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x15, 0x03);
    tp28xx_byte_write(chip, 0x16, 0xe2);
    tp28xx_byte_write(chip, 0x17, 0x80);
    tp28xx_byte_write(chip, 0x18, 0x27);
    tp28xx_byte_write(chip, 0x19, 0x38);
    tp28xx_byte_write(chip, 0x1a, 0x47);
    tp28xx_byte_write(chip, 0x1c, 0x08);
    tp28xx_byte_write(chip, 0x1d, 0x96);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x40);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x74);
    tp28xx_byte_write(chip, 0x31, 0x9b);
    tp28xx_byte_write(chip, 0x32, 0xa5);
    tp28xx_byte_write(chip, 0x33, 0xe0);

    //tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
//HDC 8M15
static void TP2825B_C8MP15_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x38);

    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);

    tp28xx_byte_write(chip, 0x2d, 0x84);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x31);

    tp28xx_byte_write(chip, 0x39, 0x48);
}

/////HDC 8M12
static void TP2825B_C8MP12_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);
    tp28xx_byte_write(chip, 0x15, 0x23);
    tp28xx_byte_write(chip, 0x16, 0xf8);
    tp28xx_byte_write(chip, 0x18, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x68);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x84);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x32);

    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);
}
/////HDA 8M15
static void TP2825B_A8MP15_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tp28xx_byte_write(chip, 0x15, 0x13);
    tp28xx_byte_write(chip, 0x16, 0x74);
    //tp28xx_byte_write(chip, 0x18, 0x32);

    tp28xx_byte_write(chip, 0x20, 0x50);
    //tp28xx_byte_write(chip, 0x21, 0x86);
    //tp28xx_byte_write(chip, 0x22, 0x36);

    //tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0x58);
    tp28xx_byte_write(chip, 0x2e, 0x48);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x68);
    tp28xx_byte_write(chip, 0x32, 0x43);
    tp28xx_byte_write(chip, 0x33, 0x00);
    //tp28xx_byte_write(chip, 0x35, 0x15);
    tp28xx_byte_write(chip, 0x39, 0x40);
}
static void TP2825B_1080P50_DataSet(unsigned char chip)
{
    unsigned char tmp;

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x15, 0x03);
    tp28xx_byte_write(chip, 0x16, 0xe2);
    tp28xx_byte_write(chip, 0x17, 0x80);
    tp28xx_byte_write(chip, 0x18, 0x27);
    tp28xx_byte_write(chip, 0x19, 0x38);
    tp28xx_byte_write(chip, 0x1a, 0x47);
    tp28xx_byte_write(chip, 0x1c, 0x0A);
    tp28xx_byte_write(chip, 0x1d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xad);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x40);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x74);
    tp28xx_byte_write(chip, 0x31, 0x9b);
    tp28xx_byte_write(chip, 0x32, 0xa5);
    tp28xx_byte_write(chip, 0x33, 0xe0);

    //tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x40);
    tp28xx_byte_write(chip, 0x39, 0x48);
    tp28xx_byte_write(chip, 0x3a, 0x12);
    tp28xx_byte_write(chip, 0x3b, 0x26);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
//HDC 5M20
static void TP2825B_C5MP20_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x15, 0x33);
    tp28xx_byte_write(chip, 0x16, 0x9c);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x38);

    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);

    tp28xx_byte_write(chip, 0x2d, 0x74);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x30);

    tp28xx_byte_write(chip, 0x39, 0x48);
}
//HDC 6M20
static void TP2825B_C6MP20_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x50);
    tp28xx_byte_write(chip, 0x21, 0x86);
    tp28xx_byte_write(chip, 0x22, 0x38);

    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0xda);

    tp28xx_byte_write(chip, 0x2d, 0x74);
    tp28xx_byte_write(chip, 0x2e, 0x50);

    tp28xx_byte_write(chip, 0x30, 0x75);
    tp28xx_byte_write(chip, 0x31, 0x39);
    tp28xx_byte_write(chip, 0x32, 0xc0);
    tp28xx_byte_write(chip, 0x33, 0x30);

    tp28xx_byte_write(chip, 0x39, 0x48);
}



static void push_table(REGPAIR *table, unsigned int count)
{
	unsigned int i;
	unsigned char iChip = 0;	
	//pthread_mutex_lock(&gSensorMutex);
	for (i = 0; i < count; i++)
	{
//        if (table[i].addr == REGFLAG_DELAY)
//        {
//            ithDelay(table[i].value * 1000);
//            continue;
//        }
//        else
		if (table[i].addr == REGFLAG_END_OF_TABLE)
		{
			break;
		}
		else
		{
			tp28xx_byte_write(iChip, table[i].addr, table[i].value);
		}
	}
    //pthread_mutex_unlock(&gSensorMutex);
}


struct TP2825_STANDAR_TO_STRING
{
	unsigned char std;
	unsigned char str[32];
} ;

struct TP2825_MODE_TO_STRING
{
	unsigned char mode;
	unsigned char str[32];
} ;

static struct TP2825_STANDAR_TO_STRING STD_STR_TABLE[] = {
	{STD_TVI,"STD_TVI"},
	{STD_HDA,"STD_HDA"},
	{STD_HDC,"STD_HDC"},
	{STD_HDA_DEFAULT,"STD_HDA_DEFAULT"},
	{STD_HDC_DEFAULT,"STD_HDC_DEFAULT"},
	{STD_NONE,"STD_NONE"}
};

static struct TP2825_MODE_TO_STRING MODE_STR_TABLE[] = {
	{TP2802_1080P25 	,	 "TP2802_1080P25	"},
	{TP2802_1080P30 	,	 "TP2802_1080P30	"},
	{TP2802_720P25		,	 "TP2802_720P25 	"},
	{TP2802_720P30		,	 "TP2802_720P30 	"},
	{TP2802_720P50		,	 "TP2802_720P50 	"},
	{TP2802_720P60		,	 "TP2802_720P60 	"},
	{TP2802_SD			,	 "TP2802_SD 		"},
	{INVALID_FORMAT 	,	 "INVALID_FORMAT	"},
	{TP2802_720P25V2	,	 "TP2802_720P25V2	"},
	{TP2802_720P30V2	,	 "TP2802_720P30V2	"},
	{TP2802_PAL 		,	 "TP2802_PAL		"},
	{TP2802_NTSC		,	 "TP2802_NTSC		"},  
	{TP2802_3M18		,	 "TP2802_3M18		"},
	{TP2802_5M12		,	 "TP2802_5M12		"},
	{TP2802_4M15		,	 "TP2802_4M15		"},
	{TP2802_3M20		,	 "TP2802_3M20		"},
	{TP2802_4M12		,	 "TP2802_4M12		"},
	{TP2802_6M10		,	 "TP2802_6M10		"},
	{TP2802_QHD30		,	 "TP2802_QHD30		"},
	{TP2802_QHD25		,	 "TP2802_QHD25		"},
	{TP2802_QHD15		,	 "TP2802_QHD15		"},
	{TP2802_QXGA18		,	 "TP2802_QXGA18 	"},
	{TP2802_QXGA30		,	 "TP2802_QXGA30 	"},
	{TP2802_QXGA25		,	 "TP2802_QXGA25 	"},
	{TP2802_4M30		,	 "TP2802_4M30		"},
	{TP2802_4M25		,	 "TP2802_4M25		"},
	{TP2802_5M20		,	 "TP2802_5M20		"},
	{TP2802_8M15		,	 "TP2802_8M15		"},
	{TP2802_8M12		,	 "TP2802_8M12		"},
	{TP2802_1080P15 	,	 "TP2802_1080P15	"},
	{TP2802_1080P60 	,	 "TP2802_1080P60	"},
	{TP2802_960P30		,	 "TP2802_960P30 	"},
	{TP2802_1080P20 	,	 "TP2802_1080P20	"},
	{TP2802_1080P50 	,	 "TP2802_1080P50	"},
	{TP2802_720P14		,	 "TP2802_720P14 	"},
	{TP2802_720P30HDR	,	 "TP2802_720P30HDR	"},
	{TP2802_6M20		,	 "TP2802_6M20		"},
	{TP2802_8M15V2		,	 "TP2802_8M15V2 	"},
	{NO_VIDEO		,	 "NO VIDEO 	"}, //add for no signal not origin from techpoint

};

static tp2825_print_mode_string(unsigned char mode)
{
	int i;
	int cout;	
	cout = sizeof(MODE_STR_TABLE) / sizeof(struct TP2825_MODE_TO_STRING) ;

	for(i=0;i< cout;i++) 
	{

		if(mode	== MODE_STR_TABLE[i].mode) 
		{
			//ithPrintf("(mode=%d=%s)",MODE_STR_TABLE[i].mode,MODE_STR_TABLE[i].str );		
			break;

		}

	}
	if(i==cout)
		ithPrintf("1(UNKNOWN)");


}
static tp2825_print_standard_string(unsigned char std)
{
	int i;
	int cout;
	cout = sizeof(STD_STR_TABLE) / sizeof(struct TP2825_STANDAR_TO_STRING) ;
	for(i=0;i< cout;i++) 
	{

		if(std	== STD_STR_TABLE[i].std) 
		{
			//ithPrintf("(std=%d=%s)\n",STD_STR_TABLE[i].std,STD_STR_TABLE[i].str );\
				break;
		}

	}
//	if(i==cout)
//		ithPrintf("2(UNKNOWN)");

}

////////////////////////////////////////////////////////////////////////////////
static int tp2802_set_video_mode(unsigned char chip, unsigned char mode, unsigned char ch, unsigned char std)
{
    int err=0;
    unsigned int tmp;


	ithPrintf("[TP2825 NEW DRIVER] %s chip %x, channel %x,  mode %x, ,  stanard %x==== \n",__FUNCTION__, chip,   ch,mode,  std);

	ithPrintf("[TP2825 NEW DRIVER] %s,\n",__FUNCTION__);
	tp2825_print_mode_string(mode);
	tp2825_print_standard_string(std);


ithPrintf("==============================tp2802_set_video_mode:%d\n",mode);
    switch(mode)
    {
		case TP2802_1080P25:    //JAVISTECH_20191128 KKM (Entry AHD_1080P25, TVI_1080P25, CVI_1080P25)

		tp2802_set_work_mode_1080p25(chip);
		TP2825B_Set_REG0X02(chip, 0xC8);
		TP2825B_V1_DataSet(chip);
		if( STD_HDA == std)
		{
			TP2825B_A1080P25_DataSet(chip);
		}
		else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		{
			TP2826_C1080P25_DataSet(chip);
			if(STD_HDC == std)
			{
				tp28xx_byte_write(chip, 0x15, 0x13);
				tp28xx_byte_write(chip, 0x16, 0x60);
				tp28xx_byte_write(chip, 0x17, 0x80);
				tp28xx_byte_write(chip, 0x18, 0x29);
				tp28xx_byte_write(chip, 0x19, 0x38);
				tp28xx_byte_write(chip, 0x1A, 0x47);
				tp28xx_byte_write(chip, 0x1C, 0x09);
				tp28xx_byte_write(chip, 0x1D, 0x60);
			}

		}
		TP2825B_SYSCLK_V1(chip);	

		break;

		case TP2802_1080P30:   //JAVISTECH_20191128 KKM  (Entry AHD_1080P30, TVI_1080P30, CVI_1080P30)

			tp2802_set_work_mode_1080p30(chip);
			TP2825B_Set_REG0X02(chip, 0xC8);
			TP2825B_V1_DataSet(chip);
			if( STD_HDA == std)
			{
				TP2825B_A1080P30_DataSet(chip);
			}
			else if(STD_HDC == std || STD_HDC_DEFAULT == std)
			{
				TP2826_C1080P30_DataSet(chip);
				if(STD_HDC == std)
				{
					tp28xx_byte_write(chip, 0x15, 0x13);
					tp28xx_byte_write(chip, 0x16, 0x60);
					tp28xx_byte_write(chip, 0x17, 0x80);
					tp28xx_byte_write(chip, 0x18, 0x29);
					tp28xx_byte_write(chip, 0x19, 0x38);
					tp28xx_byte_write(chip, 0x1A, 0x47);
					tp28xx_byte_write(chip, 0x1C, 0x09);
					tp28xx_byte_write(chip, 0x1D, 0x60);
				}
			}
			TP2825B_SYSCLK_V1(chip);

		break;

		case TP2802_720P25:   
			tp2802_set_work_mode_720p25(chip);
			TP2825B_Set_REG0X02(chip, 0xCA);
			TP2825B_V1_DataSet(chip);
			TP2825B_SYSCLK_V1(chip);
		break;

		case TP2802_720P30:
			ithPrintf("==============================TP2802_720P30,std[%d]\n",std);
			tp2802_set_work_mode_720p30(chip);
			#if 0
				TP2825B_Set_REG0X02(chip, 0xCA);
			#else
				tp28xx_byte_write(chip, 0x02, 0xCA);
				tmp = tp28xx_byte_read(chip, 0x4E);
				tmp &= 0xFB;
				tp28xx_byte_write(chip, 0x4E, tmp);
			#endif
			TP2825B_V1_DataSet(chip);
			TP2825B_SYSCLK_V1(chip);
		break;

		case TP2802_720P50:
			tp2802_set_work_mode_720p50(chip);
			TP2825B_Set_REG0X02(chip, 0xCA);
			TP2825B_V1_DataSet(chip);
			if( STD_HDA == std)
			{

			}
			else if(STD_HDC == std || STD_HDC_DEFAULT == std)
			{
				TP2826_C720P50_DataSet(chip);
				if(STD_HDC == std)
				{
					tp28xx_byte_write(chip, 0x15, 0x13);
					tp28xx_byte_write(chip, 0x16, 0x0a);
					tp28xx_byte_write(chip, 0x17, 0x00);
					tp28xx_byte_write(chip, 0x18, 0x19);
					tp28xx_byte_write(chip, 0x19, 0xd0);
					tp28xx_byte_write(chip, 0x1A, 0x25);
					tp28xx_byte_write(chip, 0x1C, 0x06);
					tp28xx_byte_write(chip, 0x1D, 0x7a);
				}

			}
			TP2825B_SYSCLK_V1(chip);
			break;

		case TP2802_720P60:
			tp2802_set_work_mode_720p60(chip);
			TP2825B_Set_REG0X02(chip, 0xCA);
			TP2825B_V1_DataSet(chip);
			if( STD_HDA == std)
			{
				//TP2826_A720P60_DataSet(chip);
			}
			else if(STD_HDC == std || STD_HDC_DEFAULT == std)
			{
				TP2826_C720P60_DataSet(chip);
				if(STD_HDC == std)
				{
					tp28xx_byte_write(chip, 0x15, 0x13);
					tp28xx_byte_write(chip, 0x16, 0x08);
					tp28xx_byte_write(chip, 0x17, 0x00);
					tp28xx_byte_write(chip, 0x18, 0x19);
					tp28xx_byte_write(chip, 0x19, 0xd0);
					tp28xx_byte_write(chip, 0x1A, 0x25);
					tp28xx_byte_write(chip, 0x1C, 0x06);
					tp28xx_byte_write(chip, 0x1D, 0x72);
				}

			}
			TP2825B_SYSCLK_V1(chip);
			break;

		case TP2802_720P30V2:    //JAVISTECH_20191128 KKM  (Entry AHD_720P30, TVI_720P30, CVI_720P30)
		
		ithPrintf("==============================TP2802_720P30V2,std[%d]\n",std);


			tp2802_set_work_mode_720p60(chip);
			#if 0//org
				TP2825B_Set_REG0X02(chip, 0xCA);
			#else
				tp28xx_byte_write(chip, 0x02, 0xCA);
				tmp = tp28xx_byte_read(chip, 0x4E);
				tmp |= 0x04;
				tp28xx_byte_write(chip, 0x4E, tmp);
			#endif
			TP2825B_V2_DataSet(chip);
			if( STD_HDA == std)
			{
				TP2825B_A720P30_DataSet(chip);
			}
			else if(STD_HDC == std || STD_HDC_DEFAULT == std)
			{
				TP2826_C720P30_DataSet(chip);
				if(STD_HDC == std)
				{
					tp28xx_byte_write(chip, 0x15, 0x13);
					tp28xx_byte_write(chip, 0x16, 0x08);
					tp28xx_byte_write(chip, 0x17, 0x00);
					tp28xx_byte_write(chip, 0x18, 0x19);
					tp28xx_byte_write(chip, 0x19, 0xd0);
					tp28xx_byte_write(chip, 0x1A, 0x25);
					tp28xx_byte_write(chip, 0x1C, 0x06);
					tp28xx_byte_write(chip, 0x1D, 0x72);
				}

			}
			TP2825B_SYSCLK_V2(chip);

		break;

		case TP2802_720P25V2:   //JAVISTECH_20191128 KKM  (Entry AHD_720P25, TVI_720P25, CVI_720P25)
			tp2802_set_work_mode_720p50(chip);
			TP2825B_Set_REG0X02(chip, 0xCA);
			TP2825B_V2_DataSet(chip);
			if( STD_HDA == std)
			{
				TP2825B_A720P25_DataSet(chip);
			}
			else if(STD_HDC == std || STD_HDC_DEFAULT == std)
			{
				TP2826_C720P25_DataSet(chip);
				if(STD_HDC == std)
				{
					tp28xx_byte_write(chip, 0x15, 0x13);
					tp28xx_byte_write(chip, 0x16, 0x0a);
					tp28xx_byte_write(chip, 0x17, 0x00);
					tp28xx_byte_write(chip, 0x18, 0x19);
					tp28xx_byte_write(chip, 0x19, 0xd0);
					tp28xx_byte_write(chip, 0x1A, 0x25);
					tp28xx_byte_write(chip, 0x1C, 0x06);
					tp28xx_byte_write(chip, 0x1D, 0x7a);
				}
			}
			TP2825B_SYSCLK_V2(chip);
		break;

		case TP2802_PAL:  //JAVISTECH_20191128 KKM  (Entry PAL)
			tp2802_set_work_mode_PAL(chip);
			TP2825B_Set_REG0X02(chip, 0xCF);
			TP2825B_PAL_DataSet(chip);
			TP2825B_SYSCLK_V2(chip);
		break;

		case TP2802_NTSC:  //JAVISTECH_20191128 KKM  (Entry NTSC)
		{
		
			ithPrintf("==============================TP2802_NTSC,std[%d]\n",std);
		    tp2802_set_work_mode_NTSC(chip);
		    #if 0	// org
		    	TP2825B_Set_REG0X02(chip, 0xCF);
		    #else
				tp28xx_byte_write(chip, 0x02, 0xCF);
				tmp = tp28xx_byte_read(chip, 0x4E);
				tmp |= 0x04;
				ithPrintf("TP2802_NTSC,Addr[0x4E]:Data[0x%02X]\n",tmp);
				tp28xx_byte_write(chip, 0x4E, tmp);
			#endif
		    TP2825B_NTSC_DataSet(chip);
		    TP2825B_SYSCLK_V2(chip);
		    }
		break;

		case TP2802_3M18:
		    tp2802_set_work_mode_3M(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x30);
		    TP2825B_SYSCLK_V1(chip);
		break;

		case TP2802_5M12:
		    tp2802_set_work_mode_5M(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x17);
		    tp28xx_byte_write(chip, 0x36, 0xd0);
		    if( STD_HDA == std)
		    {
		        TP2825B_A5MP12_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_4M15:
		    tp2802_set_work_mode_4M(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x72);
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_3M20:
		    tp2802_set_work_mode_3M20(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x72);
		    tp28xx_byte_write(chip, 0x2d, 0x26);
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_6M10:
		    tp2802_set_work_mode_6M10(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x17);
		    tp28xx_byte_write(chip, 0x36, 0xbc);
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_QHD30:
		    tp2802_set_work_mode_QHD30(chip);
		    TP2825B_Set_REG0X02(chip, 0xD8);
		    TP2825B_QHDP30_25_DataSet(chip);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQHDP30_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        TP2825B_CQHDP30_DataSet(chip);
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V3(chip);
		    break;

		case TP2802_QHD25:
		    tp2802_set_work_mode_QHD25(chip);
		    TP2825B_Set_REG0X02(chip, 0xD8);
		    TP2825B_QHDP30_25_DataSet(chip);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQHDP25_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        TP2825B_CQHDP25_DataSet(chip);
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V3(chip);
		    break;

		case TP2802_QHD15:
		    tp2802_set_work_mode_QHD15(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x15);
		    tp28xx_byte_write(chip, 0x36, 0xdc);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQHDP15_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_QXGA18:
		    tp2802_set_work_mode_3M(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    TP2825B_V1_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x72);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQXGAP18_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V1(chip);
		    break;

		case TP2802_QXGA25:
		    tp2802_set_work_mode_QXGA25(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x72);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQXGAP25_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V3(chip);
		    break;

		case TP2802_QXGA30:
		    tp2802_set_work_mode_QXGA30(chip);
		    TP2825B_Set_REG0X02(chip, 0xC8);
		    tp28xx_byte_write(chip, 0x35, 0x16);
		    tp28xx_byte_write(chip, 0x36, 0x72);
		    if( STD_HDA == std)
		    {
		        TP2825B_AQXGAP30_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V3(chip);
		    break;

		case TP2802_5M20:
		    tp2802_set_work_mode_5M20(chip);
		    TP2825B_Set_REG0X02(chip, 0xD8);
		     TP2825B_5MP20_DataSet(chip);
		    tp28xx_byte_write(chip, 0x35, 0x17);
		    tp28xx_byte_write(chip, 0x36, 0xbc);
		    if( STD_HDA == std)
		    {
		        TP2825B_A5MP20_DataSet(chip);
		    }
		    else if(STD_HDC == std || STD_HDC_DEFAULT == std)
		    {
		        TP2825B_C5MP20_DataSet(chip);
		        if(STD_HDC == std)
		        {

		        }

		    }
		    TP2825B_SYSCLK_V3(chip);
		    break;

	     case TP2802_8M15:
	        tp2802_set_work_mode_8M15(chip);
	        TP2825B_Set_REG0X02(chip, 0xD8);
	        TP2825B_8MP15_DataSet(chip);
	        tp28xx_byte_write(chip, 0x35, 0x18);
	        tp28xx_byte_write(chip, 0x36, 0xca);
	        if( STD_HDA == std)
	        {
	            TP2825B_A8MP15_DataSet(chip);
	        }
	        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
	        {
	            TP2825B_C8MP15_DataSet(chip);
	            if(STD_HDC == std)
	            {

	            }

	        }
	        TP2825B_SYSCLK_V3(chip);
	        break;

	     case TP2802_8M12:
	        tp2802_set_work_mode_8M12(chip);
	        TP2825B_Set_REG0X02(chip, 0xD8);
	        TP2825B_8MP15_DataSet(chip);
	        tp28xx_byte_write(chip, 0x35, 0x18);
	        tp28xx_byte_write(chip, 0x36, 0xca);
	        if( STD_HDA == std)
	        {

	        }
	        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
	        {
	            TP2825B_C8MP12_DataSet(chip);
	            if(STD_HDC == std)
	            {
	                        tp28xx_byte_write(chip, 0x1c, 0x13);
	                        tp28xx_byte_write(chip, 0x1d, 0x10);
	            }

	        }
	        TP2825B_SYSCLK_V3(chip);
	        break;

	    case TP2802_1080P60:
	        TP2825B_Set_REG0X02(chip, 0xC8);
	        TP2825B_1080P60_DataSet(chip);
	        TP2825B_SYSCLK_V3(chip);
	        break;

	    case TP2802_1080P50:
	        TP2825B_Set_REG0X02(chip, 0xC8);
	        TP2825B_1080P50_DataSet(chip);
	        TP2825B_SYSCLK_V3(chip);
	        break;

	    case TP2802_720P14:
	        tp2802_set_work_mode_720p30(chip);
	        TP2825B_Set_REG0X02(chip, 0xCA);
	        TP2825B_V2_DataSet(chip);
	        tp28xx_byte_write(chip, 0x35, 0x33);
	        tp28xx_byte_write(chip, 0x36, 0x20);
	        TP2825B_SYSCLK_V2(chip);
	        break;

	    case TP2802_720P30HDR:
	        tp2802_set_work_mode_720p30HDR(chip);
	        TP2825B_Set_REG0X02(chip, 0xCA);
	        TP2825B_V2_DataSet(chip);
	        tp28xx_byte_write(chip, 0x35, 0x33);
	        tp28xx_byte_write(chip, 0x36, 0x39);
	        tp28xx_byte_write(chip, 0x2d, 0x28);
	        TP2825B_SYSCLK_V2(chip);
	        break;

	     case TP2802_6M20:
	        tp2802_set_work_mode_6M20(chip);
	        TP2825B_Set_REG0X02(chip, 0xC8);
	        tp28xx_byte_write(chip, 0x35, 0x17);
	        tp28xx_byte_write(chip, 0x36, 0xd0);
	        if( STD_HDA == std)
	        {

	        }
	        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
	        {
	            TP2825B_C6MP20_DataSet(chip);
	            if(STD_HDC == std)
	            {

	            }

	        }
	        TP2825B_SYSCLK_V3(chip);
	        break;

	    default:
	        err = -1;
	        break;
    }

	if( TP2802_PAL == mode || TP2802_NTSC == mode || TP2802_1080P30 == mode || TP2802_1080P25 == mode  ) //960x480/960x576 //ut
	{    
#if (CVBS_960H)
		tmp = tp28xx_byte_read(chip, 0x35);
		tmp |= 0x40;
		tp28xx_byte_write(chip, 0x35, tmp);			
		tp28xx_byte_write(chip, 0xfa, 0x0b); // half mode clock		

#if (TP2825_DEBUG)
		ithPrintf("%s(%d) mode(%d) Change Half mode CLOCK \r\n",__FUNCTION__,__LINE__, mode);
#endif
		
#endif
	}


    return err;
}
  

static void tp2802_set_reg_page(unsigned char chip, unsigned char ch)
{
    switch(ch)
    {
    case MIPI_PAGE:
        tp28xx_byte_write(chip, 0x40, 0x08);
        break;
    default:
        tp28xx_byte_write(chip, 0x40, 0x00);
        break;
    }
}


static void TP28xx_reset_default(int chip, unsigned char ch)
{
        TP2825B_reset_default(chip, ch);
}

////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////


//[JAVISTECH_20191212 KKM(reg_dump)
#if TP2825_DUMP 
void tp2825B_Reg_dump(void)
{
	int iReg_add, val;

	tp28xx_byte_write(0, 0x40, 0x00); // MPAGE = 0;
	ithPrintf("[TP2825B_Reg_dump MPAGE=0] \n");		
	for(iReg_add=0; iReg_add<=0xFF; iReg_add++)
	{
		val = tp28xx_byte_read(0, iReg_add);
		ithPrintf("Reg_add ,data { 0x%x, 0x%x }\n", iReg_add, val);		
	}	

	tp28xx_byte_write(0, 0x40, 0x08); // MPAGE = 1;
	ithPrintf("\n[TP2825B_Reg_dump MPAGE=1] \n");		
	for(iReg_add=0x10; iReg_add<=0x15; iReg_add++)
	{
		val = tp28xx_byte_read(0, iReg_add);
		ithPrintf("Reg_add ,data { 0x%x, 0x%x }\n", iReg_add, val);		
	}	


}

#endif 
//]JAVISTECH_20191212 KKM_End(reg_dump)


static void tp2802_comm_init( int chip)
{
    unsigned int val;


    if(TP2825B == id[chip])
    {
        tp28xx_byte_write(chip, 0x40, 0x00); //default Vin1
        
        tp28xx_byte_write(chip, 0x41, 0x00);
        tp28xx_byte_write(chip, 0x4c, 0x40);
        tp28xx_byte_write(chip, 0x4e, 0x00);
        tp28xx_byte_write(chip, 0x35, 0x25);
        tp28xx_byte_write(chip, 0x3d, 0x60);
        tp28xx_byte_write(chip, 0xf5, 0x10);
        tp28xx_byte_write(chip, 0xfd, 0x80);
        tp28xx_byte_write(chip, 0xfa, 0x03);

        TP2825B_reset_default(chip, VIDEO_PAGE);


#if (WDT)

        tp28xx_byte_write(chip, 0x26, 0x04);
#endif

        //MUX output
        TP2825B_output(chip);

        TP2825B_RX_init(chip, PTZ_RX_TVI_CMD);
        //soft reset
        val = tp28xx_byte_read(chip, 0x06);
        tp28xx_byte_write(chip, 0x06, 0x80|val);
    }
    else if(TP2850 == id[chip])
    {
        tp28xx_byte_write(chip, 0x40, 0x00); //default Vin1
        tp28xx_byte_write(chip, 0x41, 0x00);
        tp28xx_byte_write(chip, 0x4c, 0x40);
        tp28xx_byte_write(chip, 0x4e, 0x00);
        tp28xx_byte_write(chip, 0x35, 0x25);
        tp28xx_byte_write(chip, 0x3d, 0x60);
        tp28xx_byte_write(chip, 0xf5, 0x10);
        tp28xx_byte_write(chip, 0xfd, 0x80);
        tp28xx_byte_write(chip, 0xfa, 0x03);

        TP2825B_reset_default(chip, VIDEO_PAGE);

        tp2802_set_video_mode(chip, mode, VIDEO_PAGE, STD_TVI);

#if (WDT)

        tp28xx_byte_write(chip, 0x26, 0x04);
#endif

        //MUX output
        TP2850_output(chip);

        TP2825B_RX_init(chip, PTZ_RX_TVI_CMD);
        //soft reset
        val = tp28xx_byte_read(chip, 0x06);
        tp28xx_byte_write(chip, 0x06, 0x80|val);
    }
}


/////////////////////////////////////////////////////////////////
unsigned char TP2825B_read_egain(unsigned char chip)
{
    unsigned char gain;

            gain = tp28xx_byte_read(chip, 0x03);
            gain >>= 4;

        return gain;
}
/////////////////////////////////////////////////////////////////
static unsigned char tp28xx_read_egain(unsigned char chip)
{
    unsigned char gain;


        tp28xx_byte_write(chip, 0x2f, 0x00);
        gain = tp28xx_byte_read(chip, 0x04);

        return gain;
}






///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/// Hear old src
///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
static void tp_2825_stauts() 
{
	unsigned char iChip = 0;
	uint8_t inputStatus;
	uint8_t detStatus;

    inputStatus = tp28xx_byte_read(iChip, 0x01);
    ithPrintf("TP2825# input status= 0x%x\n", inputStatus);
    ithPrintf("TP2825# VDLOSS : %d\n", inputStatus & (1 << 7));
    ithPrintf("TP2825# VLOCK  : %d\n", inputStatus & (1 << 6));
    ithPrintf("TP2825# HLOCK  : %d\n", inputStatus & (1 << 5));
    ithPrintf("TP2825# SLOCK  : %d\n", inputStatus & (1 << 4));
    ithPrintf("TP2825# VDET   : %d\n", inputStatus & (1 << 3));
    ithPrintf("TP2825# EQDET  : %d\n", inputStatus & (1 << 2));
    ithPrintf("TP2825# NINTL  : %d\n", inputStatus & (1 << 1));
    ithPrintf("TP2825# CDET   : %d\n", inputStatus & (1 << 0));

    detStatus = tp28xx_byte_read(iChip, 0x03);
    ithPrintf("TP2825# detection status= 0x%x\n", detStatus);
    ithPrintf("TP2825# EQGAIN : %d\n", (detStatus & 0xF0)>> 4);
    ithPrintf("TP2825# SYWD   : %d\n", detStatus & (1 << 3));
    ithPrintf("TP2825# CVSTD  : %d\n", detStatus & 0x07);



}
static void tp_2825_print_form_status_register(uint8_t inputStatus) 
{
	uint8_t detStatus;
	
	ithPrintf("=========================\n" );
	if(inputStatus & C_BIT7)
	{
		ithPrintf("VIDEO is LOSS \n" );
	}
	else
	{
		ithPrintf("VIDEO preseneted \n" );
	}
	
	if(inputStatus & C_BIT6)
	{
		ithPrintf("V PLL Locked \n" );
	}
	else
	{
		ithPrintf("V PLL not Locked \n" );
	}
	

	if(inputStatus & C_BIT5)
	{
		ithPrintf("H PLL Locked \n" );
	}
	else
	{
		ithPrintf("H PLL not Locked \n" );
	}
		
	if(inputStatus & C_BIT4)
	{
		ithPrintf("Carrier PLL Locked \n" );
	}
	else
	{
		ithPrintf("Carrier PLL not Locked \n" );
	}
	
	if(inputStatus & C_BIT3)
	{		
		ithPrintf("VIDEO  Detected \n" );
	}
	else
	{
		ithPrintf("VIDEO is LOSS \n" );

	}
	if(inputStatus & C_BIT2)
	{
		ithPrintf("EQ \n" );
	}
	else
	{
		ithPrintf("Detected or SD \n" );
	}

	
	if(inputStatus & C_BIT1)
	{
		ithPrintf(" [progressive] \n" );
	}
	else
	{
		ithPrintf(" [interlace] \n" );
	}
	
	if(inputStatus & C_BIT0)
	{
		ithPrintf("Carrier Detected \n" );
	}
	else
	{
		ithPrintf("Carrier not Detected \n" );
	}

}

static int check_video_locked()
{
	unsigned char iChip = 0;
	uint8_t inputStatus;
    inputStatus = tp28xx_byte_read(iChip, 0x01);
	if((inputStatus & 0x60) == 0x60)
	{
		return 1;
	}
	else
	{
	
		printf("video not locked \n");
		//tp_2825_print_form_status_register(inputStatus);
		return 0;
	}

}


static int check_video_loss()
{
	unsigned char iChip = 0;
	static uint8_t losscnt = 0;
	uint8_t inputStatus;
    inputStatus = tp28xx_byte_read(iChip, 0x01);
	if((inputStatus & 0x80) == 0x80)
	{
		printf("video loss:[0x%02x][%d]\n",inputStatus,losscnt);
		losscnt++;
		if (losscnt > 100) 
			losscnt = 1;
	}
	else
	{
		printf( "video detected !:[0x%02x][%d]\n",inputStatus,losscnt);
		losscnt = 0;
	}

	return (losscnt == 0)?0:1;
}

static int check_video_detcted()
{
	unsigned char iChip = 0;
	uint8_t inputStatus;
    inputStatus = tp28xx_byte_read(iChip, 0x01);
	if((inputStatus & 0x01) == 0x01)
	{
		return 1;
	}
	else
	{
		printf("detecting src \n");
		return 0;
	}
}

static void tp28xx_change_page(int page)
{
	unsigned char iChip = 0;
//pthread_mutex_lock(&gSensorMutex);
if(0 ==page )
	tp28xx_byte_write(iChip,0x40, 0x00);
else
	tp28xx_byte_write(iChip,0x40, 0x08);
//pthread_mutex_unlock(&gSensorMutex);

}

static void tp28xx_camera_switch(int channel)
{
	unsigned char iChip = 0;
	//pthread_mutex_lock(&gSensorMutex);
		tp28xx_byte_write(iChip,0x41, channel);
	//pthread_mutex_unlock(&gSensorMutex);

}



void ithTP2825LedOn(void)
{
}

void ithTP2825LedOff(void)
{
}

//=============================================================================
//                TP2825 IMPLEMENT FUNCTION START
//=============================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////////
static void tp28xx_interrupt_handler(unsigned int pin, void* arg)
{
	uint32_t val=0;

	//ithEnterCritical();

//	DBG("%s(%p)\n", __FUNCTION__, arg);
	_tp28xx_isr = 1;
	//ithExitCritical();
}

//#define XO_SN1_GPIO_RST1 	95
//#define XO_SN1_GPIO_INTR1	91


//#define CAM2_CTR

static void TP2825GpioInit(void)
{
    printf("TP2825_2# GpioInit +\n");

	ithGpioSetMode(XO_SN1_GPIO_INTR1, ITH_GPIO_MODE0);					//set the GPIO pin as GPIO mode(mode0)
	ithGpioSetIn(XO_SN1_GPIO_INTR1);									//set this GPIO pin as input

	ithGpioSetMode(CFG_SN1_GPIO_PWN, ITH_GPIO_MODE0);
	ithGpioSetOut(CFG_SN1_GPIO_PWN);
	usleep(1000);//wait
	ithGpioClear(XO_SN1_GPIO_RST1);
	ithGpioSetMode(XO_SN1_GPIO_RST1, ITH_GPIO_MODE0);
	ithGpioSetOut(XO_SN1_GPIO_RST1);
	ithGpioClear(XO_SN1_GPIO_RST1);
	#if defined(CAM2_CTR)	
		ithGpioClear(XO_SN1_GPIO_RST2);
		ithGpioSetMode(XO_SN1_GPIO_RST2, ITH_GPIO_MODE0);
		ithGpioSetOut(XO_SN1_GPIO_RST2);
		ithGpioClear(XO_SN1_GPIO_RST2);
	#endif	
		
	usleep(30*1000);//wait
	
	ithGpioSet(XO_SN1_GPIO_RST1);
	#if defined(CAM2_CTR)	
		ithGpioSet(XO_SN1_GPIO_RST2);
	#endif	
	usleep(30*1000);//wait
	
	ithGpioClear(XO_SN1_GPIO_RST1);
	#if defined(CAM2_CTR)	
		ithGpioClear(XO_SN1_GPIO_RST2);
	#endif
	usleep(50*1000);//wait ,RESET pin LOW to HIGH
	ithGpioSet(XO_SN1_GPIO_RST1);
	#if defined(CAM2_CTR)	
		ithGpioSet(XO_SN1_GPIO_RST2);
	#endif	
	usleep(50*1000);//wait ,RESET pin LOW to HIGH
    printf("TP2825_# GpioInit -\n");
}
static void TP2825HwReset(void)
{
	#if defined(XO_SN1_GPIO_RST1)	
		ithGpioSetMode(XO_SN1_GPIO_RST1, ITH_GPIO_MODE0);
		ithGpioSetOut(XO_SN1_GPIO_RST1);
		ithGpioClear(XO_SN1_GPIO_RST1);
		#if defined(CAM2_CTR)	
			ithGpioSetMode(XO_SN1_GPIO_RST2, ITH_GPIO_MODE0);
			ithGpioSetOut(XO_SN1_GPIO_RST2);
			ithGpioClear(XO_SN1_GPIO_RST2);
		#endif
		usleep(100*1000);//wait ,RESET pin LOW to HIGH
		ithGpioSet(XO_SN1_GPIO_RST1);
		#if defined(CAM2_CTR)
			ithGpioSet(XO_SN1_GPIO_RST2);
		#endif	
		usleep(100*1000);//wait ,RESET pin LOW to HIGH
	#endif
}

static void _tp2825_reset_default(void)
{

	unsigned char iChip = 0;
	uint8_t tmp = 0;

	push_table(TP2825B_SYSCLK_V1_tbl, sizeof(TP2825B_SYSCLK_V1_tbl));

    tp28xx_byte_write(iChip,0x07, 0xc0);
    tp28xx_byte_write(iChip,0x0b, 0xc0);
    tp28xx_byte_write(iChip,0x26, 0x04);

    tmp = tp28xx_byte_read(iChip, 0x06);
    tmp &= 0xfb;
    tp28xx_byte_write(iChip, 0x06, tmp);
}

static void _tp2825_soft_reset(void)
{
	unsigned char iChip = 0;
	uint8_t _reset_control = 0;
	_reset_control = tp28xx_byte_read(iChip, 0x06);
	tp28xx_byte_write(iChip, 0x06, 0x80|_reset_control);
}

static void _tp2825_switch_vin(int vin)
{
	unsigned char iChip = 0;
	uint8_t tmp = 0;

	tmp = vin & 0xff;

	tp28xx_byte_write(iChip,0x41, tmp);

	if (tmp == 0) {
		tmp = tp28xx_byte_read(iChip, 0xF0);
		tmp &= 0xcf;
		tmp |= 0x10;
		tp28xx_byte_write(iChip,0xf0, tmp);
	}
	tp28xx_byte_write(iChip,0x38, 0x40);	//single-end
	tp28xx_byte_write(iChip,0x3d, 0x60);
}


static void _tp2825_start_tx(void)
{
	unsigned char iChip = 0;
	unsigned char _tmp,i;

	tp28xx_byte_write(iChip,0xB6, 0x01); //clear flag

	_tmp = tp28xx_byte_read(iChip, 0x6F);
	_tmp |= 0x01;
	tp28xx_byte_write(iChip,0x6F, _tmp); //TX enable
	_tmp &= 0xFE;
	tp28xx_byte_write(iChip,0x6F, _tmp); //TX disable

	for (i = 0; i < 20; i++) //wait TX end
	{
		_tmp = tp28xx_byte_read(iChip, 0xB6);
		if (0x01 & _tmp) break;
		usleep(200*1000);
	}
}


void _tp2825_set_Brightness(uint8_t level)
{
	unsigned char iChip = 0;
	printf("tp2825_set_Brightness: %d\n", level);
//[JAVISTECH_20191212 KKM(reg_dump) 
// 2's complement 
// Brightness Level -64 ~ +63
// Brightness 0~100
// 50:63 , 
#if 1 
	if (!((64 <= level) && (191 >= level)) )  
	{		
printf("set_Brightness: %d\n", level);
//		pthread_mutex_lock(&gSensorMutex);
		tp28xx_change_page(0);
		tp28xx_byte_write(iChip, 0x10, level);
//		pthread_mutex_unlock(&gSensorMutex);
	}
#endif 
//]JAVISTECH_20191212 KKM_End(reg_dump)
}

void _tp2825_set_Contrast(uint8_t level)
{
	unsigned char iChip = 0;
	printf("_tp2825_set_Contrast: %d\n", level);
//[JAVISTECH_20191212 KKM(reg_dump)
#if 1
	if(level <= 127)
	{
		printf("set_Contrast: %d\n", level);
//		pthread_mutex_lock(&gSensorMutex);
		tp28xx_change_page(0);
		tp28xx_byte_write(iChip,0x11, level);
//		pthread_mutex_unlock(&gSensorMutex);
	}
#endif 
//]JAVISTECH_20191212 KKM_End(reg_dump)
}

void _tp2825_set_Uvgain(uint8_t level)
{
	unsigned char iChip = 0;
	printf("tp2825_set_Uvgain: %d\n", level);
//[JAVISTECH_20191212 KKM(reg_dump)
#if 1 
	if(level <= 127)
	{
		printf("set_Uvgain: %d\n", level);
//		pthread_mutex_lock(&gSensorMutex);
		tp28xx_change_page(0);
		tp28xx_byte_write(iChip, 0x12, level);
//		pthread_mutex_unlock(&gSensorMutex);
	}
#endif 
//]JAVISTECH_20191212 KKM_End(reg_dump)
}

//[JAVISTECH_20191218 KKM(Add_Hue)
#if 1 
void _tp2825_set_Hue(uint8_t level)  
{
	unsigned char iChip = 0;
	printf("tp2825_set_Hue: %d\n", level);
// 2's complement 
// HUE Level -90 ~ +90
	if (!((91 <= level) && (165 >= level)) )  // HUE Level -90 ~ +90
	{		
		printf("set_Hue: %d\n", level);
//		pthread_mutex_lock(&gSensorMutex);
		tp28xx_change_page(0);
		tp28xx_byte_write(iChip, 0x13, level);
//		pthread_mutex_unlock(&gSensorMutex);
	}
}
#endif 
//]JAVISTECH_20191218 KKM_End(Add_Hue)
void _tp2825_set_Sharpness(uint8_t level)
{
	unsigned char iChip = 0;
	printf("tp2825_set_Sharpness: %d\n", level);
//[JAVISTECH_20191212 KKM(reg_dump)
#if 1 
	if(level <= 31)
	{
		printf("set_Sharpness: %d\n", level);
//		pthread_mutex_lock(&gSensorMutex);
		tp28xx_change_page(0);
		tp28xx_byte_write(iChip,0x14, level);
//		pthread_mutex_unlock(&gSensorMutex);
	}
#endif 
//]JAVISTECH_20191212 KKM_End(reg_dump)
	
	
}

static uint8_t tp_2825_get_format() 
{
	unsigned char iChip = 0;
	uint8_t detStatus;
	uint8_t det_format;

    detStatus = tp28xx_byte_read(iChip, 0x03);
	ithPrintf("TP2825# SRC detected, 0x1=0x%x , 0x3=0 x%x\n",tp28xx_byte_read(iChip, 0x03),detStatus);

//    ithPrintf("TP2825# CVSTD  : %d\n", detStatus & 0x07);
	det_format= detStatus & 0x07;
    ithPrintf("TP2825# FORMAT  : %d\n",det_format);
	return det_format;

}	


//set input channel 
int set_tp2825_vin(int channel)
{
	if(channel<3){
		request_channel=channel;
		
		ithPrintf("TP2825# set_tp2825_vin to %d\n",channel);

		
		return 1;
	}
	else
	{
	
		ithPrintf("TP2825# set_tp2825_vin error\n");
		return -1;
	}
}

static uint8_t tCameraLocked = 0;

//[JAVISTECH_20191111 KKM(cam_check_status)
#if 1 
uint8_t _tp2825_get_control_state(void)
{
//	ithPrintf("_tp2825_control_state : %d\n",control_state);
	return control_state;
}
uint8_t _tp2825_get_lock_state(void)
{
//	ithPrintf("_tp2825_tCameraLocked : %d\n",tCameraLocked);
	return tCameraLocked;
}
#endif 
//]JAVISTECH_20191111 KKM_End(cam_check_status)







#define USE_STATE_PRINT	1
#if USE_STATE_PRINT
#define	PrintState(x)	printf(x)
#else
#define PrintState(x)
#endif

//i: the channel to detect
//ret : cam format 
//ret: 0xff error 
unsigned int TP2825_auto_format_detect_by_channel(int request)
{
	int iChip=0;
	int i = 0;

	tp2802wd_info* wdi;

	int interval;
	unsigned char status, cvstd, gain, agc, tmp,flag_locked;
	unsigned char rx1,rx2;
	
	wdi = &watchdog_info[iChip];
	
	tp2802_set_reg_page(iChip,i);
	status = tp28xx_byte_read(iChip, 0x01);
	TP28xx_reset_default(iChip, i);
	ithPrintf("%s(%d) [0x%X][0x%X][0x%X][0x%X]\r\n",__FUNCTION__,__LINE__, iChip, wdi-> mode[i], i, wdi-> std[i]);
	LAW_FILE_TRACE
	tp2802_set_video_mode(iChip, wdi-> mode[i], i, wdi-> std[i]);	
	
	tmp = tp28xx_byte_read(iChip, 0xa7);
	tmp |= 0x01;
	tp28xx_byte_write(iChip, 0xa7, tmp);
	#if (TP2825_DEBUG)
		tmp = tp28xx_byte_read(iChip, 0x01);
		ithPrintf("%s(%d) status%02x  ch%02x\r\n",__FUNCTION__,__LINE__, tmp, i);
		tmp = tp28xx_byte_read(iChip, 0x03);
		ithPrintf("%s(%d) CVSTD%02x  ch%02x\r\n",__FUNCTION__,__LINE__, tmp, i);
	#endif	
						input_format[request].mode =wdi->mode[i];
						input_format[request].std = wdi->std[i];

						return 1; 
	while(1)
	{
		for(iChip = 0; iChip < chips; iChip++)
		{
			

#define EQ_COUNT 7
				if( VIDEO_LOCKED == wdi->state[i]) //check signal lock
				{
					if(0 == wdi->count[i] )
					{
						tmp = tp28xx_byte_read(iChip, 0x26);
						tmp |= 0x01;
						tp28xx_byte_write(iChip, 0x26, tmp);

						if( (SCAN_AUTO == wdi->scan[i] || SCAN_TVI == wdi->scan[i] ))
						{

							if( (TP2802_720P30V2==wdi-> mode[i]) || (TP2802_720P25V2==wdi-> mode[i]) )
							{
								tmp = tp28xx_byte_read(iChip, 0x03);
#if (TP2825_DEBUG)
								ithPrintf("%s(%d) CVSTD%02x  ch%02x chip%2x\r\n",__FUNCTION__,__LINE__, tmp, i,iChip);
#endif
								if( ! (0x08 & tmp) )
								{
#if (TP2825_DEBUG)
									ithPrintf("%s(%d) 720P V1 Detected ch%02x chip%2x\r\n",__FUNCTION__,__LINE__,i,iChip);
#endif
									wdi-> mode[i] &= 0xf7;
									LAW_FILE_TRACE

									tp2802_set_video_mode(iChip, wdi-> mode[i], i, STD_TVI); //to speed the switching

								}
							}
							//these code need to keep bottom
							{
								tmp = tp28xx_byte_read(iChip, 0xa7);
								tmp &= 0xfe;
								tp28xx_byte_write(iChip, 0xa7, tmp);
								//tp28xx_byte_write(iChip, 0x2f, 0x0f);
								tp28xx_byte_write(iChip, 0x1f, 0x06);
								tp28xx_byte_write(iChip, 0x1e, 0x60);
							}
						}
					}
					else if(1 == wdi->count[i])
					{
						tmp = tp28xx_byte_read(iChip, 0xa7);
						tmp |= 0x01;
						tp28xx_byte_write(iChip, 0xa7, tmp);
#if (TP2825_DEBUG)
						tmp = tp28xx_byte_read(iChip, 0x01);
						ithPrintf("%s(%d) status%02x  ch%02x\r\n",__FUNCTION__,__LINE__, tmp, i);
						tmp = tp28xx_byte_read(iChip, 0x03);
						ithPrintf("%s(%d) CVSTD%02x  ch%02x\r\n",__FUNCTION__,__LINE__, tmp, i);
#endif
					}
					else if( wdi->count[i] < EQ_COUNT-3)
					{

						if(SCAN_AUTO == wdi->scan[i] )
						{

							if( STD_TVI == wdi-> std[i])
							{
								tmp = tp28xx_byte_read(iChip, 0x01);

								if((TP2802_PAL == wdi-> mode[i]) || (TP2802_NTSC == wdi-> mode[i]))
								{
									//nothing to do
								}
//[JAVISTECH_20191216 KKM(unotel_Delete_High_Resolution)
#if 0 
								else if(TP2802_QXGA18 == wdi-> mode[i])
								{
									if( 0x60 == (tmp&0x64) )
									{
										wdi-> std[i] = STD_HDA; //no CVI QXGA18
										LAW_FILE_TRACE
										tp2802_set_video_mode(iChip, wdi-> mode[i], i, wdi-> std[i]);
									}
								}
								else if(TP2802_QHD15 == wdi-> mode[i] || TP2802_5M12 == wdi-> mode[i])
								{
									if(0x60 == (tmp&0x64) )
									{
										wdi-> std[i] = STD_HDA; //no CVI QHD15/5M20/5M12.5
										LAW_FILE_TRACE

										tp2802_set_video_mode(iChip, wdi-> mode[i], i, wdi-> std[i]);
									}
								}
								else if( TP2802_QHD25 == wdi-> mode[i] || TP2802_QHD30 == wdi-> mode[i]
								||TP2802_8M12 == wdi-> mode[i] || TP2802_8M15 == wdi-> mode[i]
								||TP2802_5M20 == wdi-> mode[i]
								)
								{
									agc = tp28xx_byte_read(iChip, 0x10);
									tp28xx_byte_write(iChip, 0x10, 0x00);

									tp28xx_byte_write(iChip, 0x2f, 0x0f);
									rx1 = tp28xx_byte_read(iChip, 0x04);

									tp28xx_byte_write(iChip, 0x10, agc);

									if(rx1 > 0x30)
									{
										wdi-> std[i] = STD_HDA;
									}
									else if(0x60 == (tmp&0x64) )
									{
										wdi-> std[i] = STD_HDC;
									}
#if (TP2825_DEBUG)
									ithPrintf("%s(%d) RX1=%02x standard to %02x  ch%02x\r\n",__FUNCTION__,__LINE__, rx1, wdi-> std[i], i);
#endif
									if(STD_TVI != wdi->std[i]) 
									{
									LAW_FILE_TRACE
										tp2802_set_video_mode(iChip, wdi-> mode[i], i, wdi-> std[i]);
									}
									else if(TP2802_8M12 == wdi-> mode[i] || TP2802_8M15 == wdi-> mode[i]) 
									{
										tp28xx_byte_write(iChip, 0x20, 0x50); //restore TVI clamping
									}
								}
#endif 
//]JAVISTECH_20191216 KKM_End(unotel_Delete_High_Resolution)
								else if(0x60 == (tmp & 0x64) )
								{
									rx2 = tp28xx_byte_read(iChip, 0x94); //capture line7 to match 3M/4M RT
									if(HDC_enable)
									{
										if(0xff == rx2)  
										{
											wdi-> std[i] = STD_HDC;
										}
										else if(0x00 == rx2)
										{
											wdi-> std[i] = STD_HDC_DEFAULT;
										}
										else	
										{
											wdi-> std[i] = STD_HDA;
										}
									}
									else
									{
										wdi-> std[i] = STD_HDA;
									}
									
									if(STD_TVI != wdi->std[i])
									{



									LAW_FILE_TRACE
									
										tp2802_set_video_mode(iChip, wdi->mode[i], i, wdi-> std[i]);
#if (TP2825_DEBUG)
										ithPrintf("%s(%d) RX2=%02x standard to %02x  ch%02x\r\n",__FUNCTION__,__LINE__, rx2, wdi-> std[i], i);
#endif
									}
								}
							}
						}
					}
					else if( wdi->count[i] < EQ_COUNT)
					{

					}
					else if(wdi->count[i] == EQ_COUNT)
					{
						gain = tp28xx_read_egain(iChip);
						if( STD_TVI != wdi-> std[i] )
						{
							{
								tp28xx_byte_write(iChip, 0x07, 0x80|(gain>>2));  // manual mode
							}
						}
					}
					else if(wdi->count[i] == EQ_COUNT+1)
					{

						if(  SCAN_AUTO == wdi->scan[i])
						{

							if( HDC_enable )
							{
								if(STD_HDC_DEFAULT == wdi->std[i] )
								{
									tp28xx_byte_write(iChip, 0x2f,0x0c);
									tmp = tp28xx_byte_read(iChip, 0x04);
									status = tp28xx_byte_read(iChip, 0x01);

									//if(0x10 == (0x11 & status) && (tmp < 0x18 || tmp > 0xf0))
									if(0x10 == (0x11 & status))
									//if((tmp < 0x18 || tmp > 0xf0))
									{
										wdi-> std[i] = STD_HDC;
									}
									else
									{
										wdi-> std[i] = STD_HDA;
									}
									LAW_FILE_TRACE
									tp2802_set_video_mode(iChip, wdi-> mode[i], i, wdi-> std[i]);
#if (TP2825_DEBUG)
									ithPrintf("%s(%d) reg01=%02x reg04@2f=0c %02x std%02x ch%02x\r\n",__FUNCTION__,__LINE__, status, tmp, wdi-> std[i], i );
#endif
								}
							}
						}
					}
					else
					{
						if( SCAN_AUTO == wdi->scan[i])
						{

							if(wdi-> mode[i] < TP2802_3M18)
							{
								tmp = tp28xx_byte_read(iChip, 0x03); //
								tmp &= 0x07;
								if(tmp != (wdi-> mode[i]&0x07) && tmp < TP2802_SD)
								{
#if (TP2825_DEBUG)
									ithPrintf("%s(%d) correct %02x from %02x ch%02x\r\n",__FUNCTION__,__LINE__, tmp, wdi-> mode[i], i );
#endif
									wdi->force[i] = 1;
								}
							}
						}

						input_format[request].mode =wdi->mode[i];
						input_format[request].std = wdi->std[i];

						return 1; 
					}
				}
				
		}
			
		usleep(50 * 1000);
	};
		
}

static void _tp2825_pre_detection_v2()
{
int i;
	int timeout=0;
	for (i=0;i<TP2825_USED_CHANNEL ;i++)
	{
#if 0
		usleep(200*1000); // 200mSec Delay
#endif 
		ithPrintf("#DETECTION= CH%d#\n",i);
		watchdog_info[0].count[0] = 0;
		watchdog_info[0].force[0] = 0;
		//watchdog_info[i].loss[j] = 1;

		#if 0	//(DEFAULT_FORMAT==TP2802_NTSC)
			watchdog_info[0].mode[0] = TP2802_NTSC;
			watchdog_info[0].scan[0] = SCAN_TVI;
			watchdog_info[0].state[0] = VIDEO_LOCKED;
			watchdog_info[0].std[0] = STD_TVI;
		#else	// (DEFAULT_FORMAT==TP2802_720P30V2)
			watchdog_info[0].mode[0] = TP2802_720P30V2;
			watchdog_info[0].scan[0] = SCAN_HDA;
			watchdog_info[0].state[0] = VIDEO_LOCKED;
			watchdog_info[0].std[0] = STD_HDA;
		#endif

		//TP28xx_reset_default(0, i);
		//TP2825GpioInit();
		push_table(TP2825_1080p_half, sizeof(TP2825_1080p_half)); // initialize
		push_table(TP2825B_SYSCLK_V1, sizeof(TP2825B_SYSCLK_V1)); 
		tp28xx_camera_switch(i);
		//usleep(200 * 1000); //do no remove		// 2024-10-30 JPHong	usleep(100 * 1000); //do no remove
		
		usleep(200 * 1000); //do no remove		// 2024-10-30 JPHong
		#if 0
			//while( (1 ==check_video_loss()) && (timeout<50) )
			while( (1 ==check_video_loss()) && (timeout<20) )
			{
				timeout++;
				//usleep(100 * 1000);
				// usleep(33 * 1000);	// 2024-10-30 JPHong
				usleep(33 * 1000);
			}
	
			if (timeout>3)
			{
				ithPrintf("#CH%d#",i);
				ithPrintf("#no signal timeout#\n");
				input_format[i].mode = NO_VIDEO;			
				input_format[i].std = STD_NONE;
				timeout=0;
				//break;
			}
			else
		#else
			usleep(100 * 1000); //do no remove		// 2024-10-30 JPHong
		#endif
		{	
			//input_format[i].mode amd input_format[i].std will be set in TP2825_auto_format_detect_by_channel
			ithPrintf("_tp2825_pre_detection_v2_1 : %d\n",tCameraLocked);
			TP2825_auto_format_detect_by_channel(i);// ¿©±⬿N½Ňϴ °﷿ithPrintf("_tp2825_pre_detection_v2_2 : %d\n",tCameraLocked);
			ithPrintf("_tp2825_pre_detection_v2_2 : %d\n",tCameraLocked);
			print_input_format_detected(i);
			ithPrintf("_tp2825_pre_detection_v2_3 : %d\n",tCameraLocked);
			tCameraLocked = 1;
			ithPrintf("_tp2825_pre_detection_v2_4 : %d\n",tCameraLocked);
		}


	}
	//usleep(1000 * 1000);
	control_state= STATE_IDLE; 
}


void print_input_format_detected(int channel)
{
	int i;

	if(channel =0xff)
	{
		for(i=0;i<TP2825_USED_CHANNEL;i++)
		{
			//ithPrintf("1#CH%d",i);
			tp2825_print_mode_string (input_format[i].mode);
			tp2825_print_standard_string(input_format[i].std);
		}
	}
	else
	{
		//ithPrintf("2#CH%d",channel);
		tp2825_print_mode_string (input_format[channel].mode);
		tp2825_print_standard_string(input_format[channel].std);


	}
}


static void TP2802_watchdog_deamon(void)
{
	ithPrintf("TP2802_watchdog_deamon: start! : %d\n",control_state);
	#if TP2825_DUMP 
	static uint8_t pre_detect_first_time_reg_dump=0;
	#endif
	int ii=0;
	uint8_t tmp1;
	uint8_t tmp2;
	uint8_t tmp3;
	uint32_t irqR;
	#define LOCKING_COUNTDOWN 	2   //5
	int locking_countdown=LOCKING_COUNTDOWN;
	{
		uint8_t inputStatus = 0;
		uint8_t dump_flag=0; // for dump TP2825 register // test only
		//static uint8_t last_format=INPUT_UNKNOWN;
		uint8_t curr_format;
		uint8_t count_down=10;

		
		#if 0
		//added by sjjang for interrupt
		printf("tp2825 interrupt i2c set start\n");
		tp28xx_byte_write(0, 0x4f, 0x01);// IRQ pin output enable
		tp28xx_byte_write(0, 0xbe, 0x80);// IRQCLRMD
		tp28xx_byte_write(0, 0xbc, 0x01);// V1IRQMD
		printf("tp2825 interrupt i2c set end\n");		
		//added by sjjang for interrupt
		#endif
		
		for (;;)
		{
			switch(control_state)
			{

				case STATE_PRE_DETECTION: // pre detection	mode
					// in order to know the format in advance
					printf("#STATE_PRE_DETECTION#\n");
					_tp2825_pre_detection_v2();
#if TP2825_DUMP 
					if(pre_detect_first_time_reg_dump == 0){
						pre_detect_first_time_reg_dump = 1;
						tp2825B_Reg_dump();
					}
#endif
				break;

				case STATE_IDLE: // idle mode
					//PrintState("#STATE_IDLE#\n");
					print_input_format_detected(0xff);
				//	usleep(1000 * 1000);
					control_state = STATE_INITIALIZING; //JPHong 05-30
				break;

				case STATE_INITIALIZING: // un
					PrintState("#STATE_INITIALIZING#\n");
				
					locking_countdown=LOCKING_COUNTDOWN;
					//tp28xx_byte_write(iChip,0x41, request_channel);
					if(    (INVALID_FORMAT == input_format[request_channel].mode)  //detecting again
						|| (NO_VIDEO == input_format[request_channel].mode ))
					{
						PrintState("#REQUESET DETECTING#\n");	
						watchdog_info[0].count[0] = 0;
						watchdog_info[0].force[0] = 0;
						//watchdog_info[i].loss[j] = 1;
						watchdog_info[0].mode[0] = INVALID_FORMAT;
						watchdog_info[0].scan[0] = SCAN_AUTO;			
						watchdog_info[0].state[0] = VIDEO_UNPLUG;
						//TP28xx_reset_default(0, i);
						//TP2825GpioInit();
						push_table(TP2825_1080p_half, sizeof(TP2825_1080p_half)); // initialize
						push_table(TP2825B_SYSCLK_V1, sizeof(TP2825B_SYSCLK_V1));					
						tp28xx_camera_switch(request_channel);
						usleep(100 * 1000); //do no remove
						control_state = STATE_DETECTING;

					}
					else //already detected just set format and
					{
						ithPrintf("# MODE PREDETECT SET #\n",tp2825_print_mode_string (input_format[request_channel].mode),tp2825_print_standard_string( input_format[request_channel].std) );
						//TP2825GpioInit();
						push_table(TP2825_1080p_half, sizeof(TP2825_1080p_half)); // initialize
						push_table(TP2825B_SYSCLK_V1, sizeof(TP2825B_SYSCLK_V1));					
						tp28xx_camera_switch(request_channel);
						usleep(100 * 1000);
						tp2802_set_video_mode(0, input_format[request_channel].mode, 0,input_format[request_channel].std);

						control_state = STATE_LOCKING;

						usleep(200 * 1000);
					}
				break;
				
				case STATE_DETECTING: // un
					PrintState("#STATE_DETECTING#\n");
					unsigned int detec;

					if(1 == TP2825_auto_format_detect_by_channel(request_channel))
					{
					//	control_state = STATE_RUNNING;
					//	print_input_format_detected(request_channel);
					
					control_state= STATE_PRE_DETECTION;//to intial
					}
					else
					{
						usleep(100 * 1000);
					}

				break;
							
				case STATE_LOCKING: // un
					inputStatus = tp28xx_byte_read(0,0x01);
//					printf("#STATE_LOCKING#[0x%02X][%d]\n",inputStatus,locking_countdown);
					if ((inputStatus & 0x60) == 0x60)
					{
						if(0==locking_countdown)
							control_state = STATE_RUNNING;
						else 
							locking_countdown--;
					}
					else
					{
						locking_countdown=LOCKING_COUNTDOWN;

					}
					
					//usleep(100 * 1000);
					usleep(10 * 1000);
					break;
					
				case STATE_RUNNING: // camera initialed keep polling
//					PrintState("#STATE_RUNNING#\n");
					inputStatus = tp28xx_byte_read(0,0x01);
					if ((inputStatus & 0x60) != 0x60)
					{
						ithPrintf("TP2825->UNLOCK!!!\n");
						control_state = STATE_INITIALIZING;
					}

					if ((inputStatus & 0x80) == 0x80)
					{
						ithPrintf("STATE_RUNNING video loss \n");
					}					
					usleep(500 * 1000); // 500mSec Delay
				break;
			}
		}
	}
}


static void TP2802_deamon_start(void)
{

    int ret = 0, val = 0;
    printf("TP2802_deamon_start\n");

#if defined(__OPENRTOS__)
	 pthread_t daemon_tid=0;

	TP2825GpioInit();

	int i, j;
	watchdog_state = WATCHDOG_RUNNING;
	memset(&watchdog_info, 0, sizeof(watchdog_info));
//TP2825  Watchdog follow original driver
	for(i=0; i<MAX_CHIPS; i++)
	{
		watchdog_info[i].addr = tp2802_i2c_addr[i];
		for(j=0; j<CHANNELS_PER_CHIP; j++)
		{
			watchdog_info[i].count[j] = 0;
			watchdog_info[i].force[j] = 0;
			//watchdog_info[i].loss[j] = 1;
			watchdog_info[i].mode[j] = INVALID_FORMAT;
			watchdog_info[i].scan[j] = SCAN_AUTO;			
			watchdog_info[i].state[j] = VIDEO_UNPLUG;

		}
	}


	/* initize each tp2802*/
	for (i = 0; i < chips; i ++)
	{

		val = tp28xx_byte_read(i, 0xfe);
		val <<= 8;
		val += tp28xx_byte_read(i, 0xff);

		if(TP2825B == val)
			ithPrintf("Detected TP2825B \n");
		else if(TP2850 == val)
			ithPrintf("Detected TP2850/TP9950\n");
		else
			ithPrintf("Invalid chip %2x\n", val);

		id[i] = val;

		tp2802_comm_init(i);

	}

// ITE capture input format
	for(i=0;i<TP2825_USED_CHANNEL;i++)
	{
			input_format[i].mode=INVALID_FORMAT;
			input_format[i].std=STD_NONE;
			
	}

    	pthread_create(&daemon_tid, NULL, TP2802_watchdog_deamon, NULL);
#endif

	control_state= STATE_PRE_DETECTION;//to intial


	//printf("TP2825# Init END -\n");
}

void TP2825_init(void)
{
	TP2802_deamon_start();
}

void TP2825Initialize(uint16_t Mode)
{

	int ret = 0, val = 0;

	printf("\n*********************************\n");
	printf("*********************************\n");
	printf("\n*********** CAM_INIT 1 '%d''%d'*********\n",Mode,control_state);
	printf("*********************************\n");
	printf("*********************************\n\n");

//	printf("TP2825# Init +\n");
//	control_state= STATE_INITIALIZING;//to intial
//	usleep(50*1000);//

	//printf("TP2825# Init END -\n");
}






void TP2825Terminate(void)
{
    /* Please implement terminate code here */
	printf("TP2825# Terminate +\n");

//	control_state= STATE_IDLE;//to intial
}

void TP2825OutputPinTriState(uint8_t flag)
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



 struct CAP_PROERTY_FORMAT
{
	unsigned int mode;
	uint16_t width;
	uint16_t height;
	uint16_t frame_rate;
	uint16_t interlace;
	uint16_t matchRes;
	uint16_t topFieldPolarity;
} ;

static struct CAP_PROERTY_FORMAT CAP_FORMAT_TABLE[] = {
	/*
	// mode 			width  height frame_rate interlace  matchRes  topFieldPolarity
	{TP2802_1080P25   ,960,1080,25,0,1,1   },
	{TP2802_1080P30   ,960,1080,30,0,1,1   },
	{TP2802_720P25    ,1280,720,25,0,1,1   },
	{TP2802_720P30    ,1280,720,30,1,1,1   },
	{TP2802_720P50    ,1920,1080,30,1,1,1   },
	{TP2802_720P60    ,1920,1080,30,1,1,1   },
	{TP2802_SD        ,1920,1080,30,1,1,1   },
	{INVALID_FORMAT   ,1920,1080,30,1,1,1   },
	{TP2802_720P25V2  ,1920,1080,30,1,1,1   },
	{TP2802_720P30V2  ,1920,1080,30,1,1,1   },
	{TP2802_PAL	      ,1920,1080,30,1,1,1   },
	{TP2802_NTSC	  ,1920,1080,30,1,1,1   },
	{TP2802_3M18      ,1920,1080,30,1,1,1   },//2048x1536@18.75 for TVI
	{TP2802_5M12      ,1920,1080,30,1,1,1   },//2592x1944@12.5 for TVI
	{TP2802_4M15      ,1920,1080,30,1,1,1   },//2688x1520@15 for TVI
	{TP2802_3M20      ,1920,1080,30,1,1,1   },//2048x1536@20 for TVI
	{TP2802_4M12      ,1920,1080,30,1,1,1   },//2688x1520@12.5 for TVI
	{TP2802_6M10      ,1920,1080,30,1,1,1   },//3200x1800@10 for TVI
	{TP2802_QHD30     ,1920,1080,30,1,1,1   },//2560x1440@30 for TVI/HDA/HDC
	{TP2802_QHD25     ,1920,1080,30,1,1,1   },//2560x1440@25 for TVI/HDA/HDC
	{TP2802_QHD15     ,1920,1080,30,1,1,1   },//2560x1440@15 for HDA
	{TP2802_QXGA18    ,1920,1080,30,1,1,1   },//2048x1536@18 for HDA/TVI
	{TP2802_QXGA30    ,1920,1080,30,1,1,1   },//2048x1536@30 for HDA
	{TP2802_QXGA25    ,1920,1080,30,1,1,1   },//2048x1536@25 for HDA
	{TP2802_4M30      ,1920,1080,30,1,1,1   },//2688x1520@30 for TVI(for future)
	{TP2802_4M25      ,1920,1080,30,1,1,1   },//2688x1520@25 for TVI(for future)
	{TP2802_5M20      ,1920,1080,30,1,1,1   },//2592x1944@20 for TVI/HDA
	{TP2802_8M15      ,1920,1080,30,1,1,1   },//3840x2160@15 for TVI
	{TP2802_8M12      ,1920,1080,30,1,1,1   },//3840x2160@12.5 for TVI
	{TP2802_1080P15   ,1920,1080,30,1,1,1   },//1920x1080@15 for TVI
	{TP2802_1080P60   ,1920,1080,30,1,1,1   },//1920x1080@60 for TVI
	{TP2802_960P30    ,1920,1080,30,1,1,1   },//1280x960@30 for TVI
	{TP2802_1080P20   ,1920,1080,30,1,1,1   },//1920x1080@20 for TVI
	{TP2802_1080P50   ,1920,1080,30,1,1,1   },//1920x1080@50 for TVI
	{TP2802_720P14    ,1920,1080,30,1,1,1   },//1280x720@14 for TVI
	{TP2802_720P30HDR ,1920,1080,30,1,1,1   },//1280x720@30 for TVI
	{TP2802_6M20      ,1920,1080,30,1,1,1   },//2960x1920@20 for CVI
	{TP2802_8M15V2    ,1920,1080,30,1,1,1   },//3264x2448@15 for TVI
*/
	//		{TP2802_1080P25         ,960,1080,2500,0,1,1   },
			{TP2802_1080P30         ,960,1080,3000,0,1,1   },
	//		{TP2802_720P25	        ,1280,720,2500,0,1,1   },
			{TP2802_720P30	 	 ,1280,720,3000,0,1,1   },
	//		{TP2802_720P25V2	  ,1280,720,2500,0,1,1   },
			{TP2802_720P30V2	  ,1280,720,3000,0,1,1   },			
	//		  {TP2802_720P50	,1280,720,50,0,1,1	 },
	//		  {TP2802_720P60	,1280,720,60,0,1,1	 },
	//		  {TP2802_SD		,1920,1080,30,1,1,1   },
	//		  {INVALID_FORMAT	,1920,1080,30,1,1,1   },
	//		  {TP2802_720P25V2	,1920,1080,30,1,1,1   },
	//		  {TP2802_720P30V2	,1920,1080,30,1,1,1   },
			{TP2802_NTSC 	  ,960,480,2997,1,1,1   },
			{TP2802_PAL	  ,960,576,2500,1,1,1   },

};


uint16_t TP2825GetProperty(MODULE_GETPROPERTY property)
{
    /* Please implement get information from device code here */

//[JAVISTECH_20200103 KKM(hangup_button_push_die)
#if 1 
	int16_t timeout = 10;   // 30
#else
	int16_t timeout = 300;   // 30 Sec
#endif 
//]JAVISTECH_20200103 KKM_End(hangup_button_push_die)


#if 0
	for (;timeout > 0;timeout--)
	{
		 
		if(tp2825_cam_running() ) //watchdog_info[0].state[0] == VIDEO_LOCKED)
		{
			break;
		}
		else
		{
			usleep(100 * 1000);
			//usleep(50 * 1000);
			timeout--;
		}
	};

	if (timeout <= 0) {
		printf("#TP2825GetProperty# no signal detected ! (%d:timeout)\n",property);
		//sensor power was suddendly cut by MCU(FOR: KOCOM design) 
		//set back to idle mode when last fetch,(we have to feed the capture module to continue the flow)
		if(GetModuleIsInterlace==property )
				control_state = STATE_IDLE;
	}
#endif
	printf("TP2825# TP2825GetProperty (mode %d) property(%d) \n", input_format[request_channel].mode, property);


	int i;
	int cout;
	cout = sizeof(CAP_FORMAT_TABLE) / sizeof(struct CAP_PROERTY_FORMAT) ;
	for(i=0;i< cout;i++) 
	{

		if(input_format[request_channel].mode	== CAP_FORMAT_TABLE[i].mode) 
		{

			switch (property)
			{
			case GetTopFieldPolarity:				
				printf("TP2825# (GetTopFieldPolarity %d)\n",  CAP_FORMAT_TABLE[i].topFieldPolarity );
				return CAP_FORMAT_TABLE[i].topFieldPolarity;

			case GetWidth:
				printf("TP2825# (width %d)\n",  CAP_FORMAT_TABLE[i].width );
				return CAP_FORMAT_TABLE[i].width;

			case GetHeight:
				printf("TP2825# (height %d)\n",  CAP_FORMAT_TABLE[i].height );
				return CAP_FORMAT_TABLE[i].height;

			case Rate:
				printf("TP2825# (frame_rate %d)\n",  CAP_FORMAT_TABLE[i].frame_rate );
				return CAP_FORMAT_TABLE[i].frame_rate;

			case GetModuleIsInterlace:
				printf("TP2825# (interlace %d)\n",  CAP_FORMAT_TABLE[i].interlace );
				return CAP_FORMAT_TABLE[i].interlace;
			case GetIsAnalogDecoder:
				printf("TP2825# (GetIsAnalogDecoder %d)\n", property);
				return 1;
				break;
			default:
				printf("TP2825# GetPropertyError (property = %d)\n", property);
				return 0;

			}

		}


	}
	printf("TP2825# GetPropertyError (not in capture list mode= %x)\n", input_format[request_channel].mode);
	return 0;




}
uint8_t TP2825GetStatus(MODULE_GETSTATUS Status)
{
    ithPrintf("[%s:%d] %s(%d)\n", __FILE__, __LINE__, __FUNCTION__, Status);

    /* Please implement get status from device code here */
    switch (Status)
    {
    default:
        break;
    }
    return 0;
}

void TP2825SetProperty(MODULE_SETPROPERTY property, uint16_t Value)
{
    ithPrintf("[%s:%d] %s(%d, %d)\n", __FILE__, __LINE__, __FUNCTION__, property, Value);

    /* Please implement set property to device code here */
    switch (property)
    {
	case LEDON:
		ithTP2825LedOn();
		break;
	case LEDOFF:
		ithTP2825LedOff();
		break;
	case VideoInCH:
		set_tp2825_vin(Value);
		break;
	default:
		break;
    }
}

void TP2825PowerDown(uint8_t enable)
{
#ifdef CFG_SENSOR_POWERDOWNPIN_ENABLE

#endif
}

//=============================================================================
//                TP2825 IMPLEMENT FUNCTION END
//=============================================================================
SensorDriver TP2825SensorDriver_Create();
static void TP2825SensorDriver_Destory(SensorDriver base);

/* assign callback funciton */
static SensorDriverInterfaceStruct interface =
{
    TP2825Initialize,
    TP2825Terminate,
    TP2825OutputPinTriState,
    TP2825IsSignalStable,
    TP2825GetProperty,
    TP2825GetStatus,
    TP2825SetProperty,
    TP2825PowerDown,
    TP2825SensorDriver_Destory
};

SensorDriver TP2825SensorDriver_Create()
{
    TP2825SensorDriver self = calloc(1, sizeof(TP2825SensorDriverStruct));
    self->base.vtable = &interface;
    self->base.type   = "TP2825";
	
     printf("TP2825SensorDriver_Create\n");
	 
    return (SensorDriver)self;
}

static void TP2825SensorDriver_Destory(SensorDriver base)
{
	
    TP2825SensorDriver self = (TP2825SensorDriver)base;
    if (self)
        free(self);
#if 0	
	control_state= STATE_IDLE;//to intial
#endif	
	printf("TP2825SensorDriver_Destory\n");
	
}

//========================================================================
//========API of TP2825 state============================
//========================================================================
int tp2825_cam_detect_result(int ch)
{
//	if(control_state== STATE_IDLE )
	if(	 INVALID_FORMAT == input_format[ch].mode
	  || NO_VIDEO == input_format[ch].mode )
	{
		return 0;

	}
	else
	{
		return 1;
	}
	return 0;
}

int tp2825_cam_available()
{
//	if(control_state== STATE_IDLE )
	if(	 INVALID_FORMAT == input_format[request_channel].mode
	  || NO_VIDEO == input_format[request_channel].mode )
	{
		return 0;

	}
	else
	{
		return 1;
	}
}

int tp2825_cam_running()
{
	if(control_state== STATE_RUNNING )
	{
		return 1;

	}
	else
	{
		return 0;
	}
}



uint8_t TP2825IsSignalStable(uint16_t Mode)
{
    /* Please implement checking signal stable code here */
    /* stable return true else return false                       */
	if ( (control_state == STATE_RUNNING)  )
//	if(1==check_video_locked())
	{
		//CAM_DISP(" ");
		return 1;
	}
	else 
	{
		//CAM_DISP(" ");
		return 0;
	}
//return _lockStatus;
}


static int check_src_is_interlace() 
{
// HD : 0,   SD : 1
	if ( (input_format[request_channel].mode == TP2802_NTSC) || (input_format[request_channel].mode == TP2802_PAL) )
	{
		return 1;
	}

	return 0;
}

static int _tp2825_get_vin()
{
	return request_channel;
}

static void _tp2825_initial_vin()
{
	push_table(TP2825_1080p_half, sizeof(TP2825_1080p_half)); // initialize
//	printf("reg 2a #0 x%x\n",tp28xx_byte_read(0x2a));
	push_table(TP2825B_SYSCLK_V1_tbl, sizeof(TP2825B_SYSCLK_V1_tbl)); 
	tp28xx_camera_switch(request_channel);
	//usleep(1000 * 1000);
}

//end of X10LightDriver_t1.c
#endif