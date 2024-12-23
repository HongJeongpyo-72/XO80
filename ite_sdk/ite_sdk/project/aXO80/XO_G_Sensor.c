
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>	

#include <sys/ioctl.h>
#include "ite/itp.h"
#include "SDL/SDL.h"
#include "iic/mmp_iic.h"

#include "HT_DispDdefine.h"


#define G_Sensor_DBG	0

#define G_Sensor_Addr	0x19	//(0x33>>1)

//pthread_mutex_t G_Sensor_Mutex  = PTHREAD_MUTEX_INITIALIZER;

short G_Sensor_X = 0;
short G_Sensor_Y = 0;
short G_Sensor_Z = 0;

void* G_Sensor_Thread(void* arg);
//void G_SensorISR(void* data);
uint8_t SC7A20_Byte_Write(uint8_t ADDR,uint8_t DATA);
uint8_t SC7A20_Byte_Read(uint8_t ADDR,uint8_t *DATA);

uint8_t G_Sensor_Boot(void);
uint8_t G_Sensor_ID(void);
uint8_t G_Sensor_FS(void);
uint8_t G_Sensor_ODR_CFG(void);
uint8_t G_Sensor_DRDY1_CFG(void);

uint8_t G_Sensor_XYZ(void);
void Cal_XYZ1(void);
bool Cal_XYZ2(uint8_t OpMode);

void G_Sensor_Init(void);				// extern 
uint8_t Get_PL_Flag(void);			// extern


uint8_t PL_Flag_New = 0;
uint8_t PL_Flag_Cnt = 0;


void G_Sensor_Init(void)
{
	// G sensor(SC7A20) initial code interrupt,config register etc...
	G_Sensor_Boot();
	G_Sensor_ID();
	G_Sensor_FS();
	G_Sensor_ODR_CFG();
	//G_Sensor_DRDY1_CFG();

	G_Sensor_XYZ();
	Cal_XYZ2(0);
}

void G_Sensor_Thread_Init(void)
{
	pthread_t G_Sensor_tid=0;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 1024);
	
	// G_Sensor Thread Create
	pthread_create(&G_Sensor_tid, &attr, G_Sensor_Thread, NULL);
	PL_Flag_Cnt = 0;
}

void* G_Sensor_Thread(void* arg)
{
	uint32_t tick=0, lasttick=0;
	
	for(;;)
	{
		tick = SDL_GetTicks();

		//if(tick - lasttick >= 100)
		if(tick - lasttick >= 200)
		{
			if(G_Sensor_XYZ() == 2)
			{
				//Cal_XYZ1();
				Cal_XYZ2(1);
			}
			lasttick    = tick;
		}
	}
}

uint8_t G_Sensor_Boot(void)
{
	uint8_t ret = 0;
	
	uint8_t CMD_Addr = 0x24;		// CTRL_REG5
	uint8_t BOOT_Status = 0;

	if(SC7A20_Byte_Read(CMD_Addr,&BOOT_Status) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 1 Ok :: CTRL_REG5 Read [%02X:%02X]\r\n",CMD_Addr,BOOT_Status);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 1 Fail :: CTRL_REG5 Read [%02X:%02X]\r\n",CMD_Addr,BOOT_Status);
#endif
		return 0;
	}
	
	BOOT_Status = 0X80 | BOOT_Status;
	
	if(SC7A20_Byte_Write(CMD_Addr,BOOT_Status) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 2 Ok :: CTRL_REG5 Write [%02X:%02X]\r\n",CMD_Addr,BOOT_Status);
#endif
		ret = 1;
	}
	else
	{
		// fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 2 Fail :: CTRL_REG5 Write [%02X:%02X]\r\n",CMD_Addr,BOOT_Status);
#endif
		return 0;
	}

	return ret;
}

uint8_t G_Sensor_ID(void)
{
	uint8_t ret = 0;
	
	uint8_t CMD_Addr = 0x0F;		// WHO_AM_I
	uint8_t _ID = 0;

	if(SC7A20_Byte_Read(CMD_Addr,&_ID) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 3 Ok :: WHO_AM_I Read [%02X:%02X]\r\n",CMD_Addr,_ID);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 3 Fail :: WHO_AM_I Read [%02X:%02X]\r\n",CMD_Addr,_ID);
#endif
		return 0;
	}

	if(_ID == 0x11)
	{
#if	1//G_Sensor_DBG
		printf("G Sensor ID ok , [%02X:%02X]\r\n",CMD_Addr,_ID);
#endif
		ret = 1;
	}
	else
	{
#if	1//G_Sensor_DBG
		printf("G Sensor ID Error , [%02X:%02X]\r\n",CMD_Addr,_ID);
#endif
		return 0;
	}

	return ret;
}

uint8_t G_Sensor_FS(void)
{
	uint8_t ret = 0;

	uint8_t CMD_Addr = 0x23;		// CTRL_REG4
	uint8_t FS_Data_WR = 0,FS_Data_RD = 0;

	
	FS_Data_WR = 0x80|0x00|0x08	;		// 0x80:Block data update disable , 0x00:Full_Scale_2G , 0x08:high resolrution enable

	if(SC7A20_Byte_Write(CMD_Addr,FS_Data_WR) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 4 Ok :: CTRL_REG4 Write [%02X:%02X]\r\n",CMD_Addr,FS_Data_WR);
#endif
		ret = 1;
	}
	else
	{
		// fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 4 Fail :: CTRL_REG4 Write [%02X:%02X]\r\n",CMD_Addr,FS_Data_WR);
#endif
		return 0;
	}

	if(SC7A20_Byte_Read(CMD_Addr,&FS_Data_RD) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 5 Ok :: CTRL_REG4 Read [%02X:%02X]\r\n",CMD_Addr,FS_Data_RD);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 5 Fail :: CTRL_REG4 Read [%02X:%02X]\r\n",CMD_Addr,FS_Data_RD);
#endif
		return 0;
	}
	
	if(FS_Data_WR == FS_Data_RD)
	{
#if	1//G_Sensor_DBG
		printf("G Sensor Full Scale Set ok , [%02X:%02X]\r\n",CMD_Addr,FS_Data_RD);
#endif
		ret = 1;
	}
	else
	{
#if	1//G_Sensor_DBG
		printf("G Sensor Full Scale Set Error , [%02X:%02X]\r\n",CMD_Addr,FS_Data_RD);
#endif
		return 0;
	}

	return ret;
}

uint8_t G_Sensor_ODR_CFG(void)
{
	uint8_t ret = 0;

	uint8_t CMD_Addr = 0x20;		// CTRL_REG1
	uint8_t ODR_Data_RD = 0,ODR_Data_WR = 0;


	ODR_Data_WR = 0x27;					// 0x27:ODR 10Hz / XYZ enable / Normal mode

	if(SC7A20_Byte_Write(CMD_Addr,ODR_Data_WR) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 6 Ok :: CTRL_REG1 Write [%02X:%02X]\r\n",CMD_Addr,ODR_Data_WR);
#endif
		ret = 1;
	}
	else
	{
		// fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 6 Fail :: CTRL_REG1 Write [%02X:%02X]\r\n",CMD_Addr,ODR_Data_WR);
#endif
		return 0;
	}

	if(SC7A20_Byte_Read(CMD_Addr,&ODR_Data_RD) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 7 Ok :: CTRL_REG1 Read [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 7 Fail :: CTRL_REG1 Read [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		return 0;
	}

	if(ODR_Data_RD == ODR_Data_WR)
	{
#if	1//G_Sensor_DBG
		printf("G Sensor ODR/Power Mode Set ok , [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		ret = 1;
	}
	else
	{
#if	1//G_Sensor_DBG
		printf("G Sensor ODR/Power Mode Set Error , [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		return 0;
	}

	return ret;
}

uint8_t G_Sensor_DRDY1_CFG(void)
{
	uint8_t ret = 0;

	uint8_t CMD_Addr = 0x22;		// CTRL_REG3
	uint8_t ODR_Data_RD = 0,ODR_Data_WR = 0;


	ODR_Data_WR = 0x10;					// DRDY1 interrupt on INT1
	//ODR_Data_WR = 0x08;					// DRDY2 interrupt on INT1

	if(SC7A20_Byte_Write(CMD_Addr,ODR_Data_WR) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 8 Ok :: CTRL_REG3 Write [%02X:%02X]\r\n",CMD_Addr,ODR_Data_WR);
#endif
		ret = 1;
	}
	else
	{
		// fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 8 Fail :: CTRL_REG3 Write [%02X:%02X]\r\n",CMD_Addr,ODR_Data_WR);
#endif
		return 0;
	}

	if(SC7A20_Byte_Read(CMD_Addr,&ODR_Data_RD) == 1)
	{
		// ok
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 9 Ok :: CTRL_REG1 Read [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Boot Step 9 Fail :: CTRL_REG1 Read [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		return 0;
	}

	if(ODR_Data_RD == ODR_Data_WR)
	{
#if	1//G_Sensor_DBG
		printf("G Sensor DRDY1_INT1 Set ok , [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		ret = 1;
	}
	else
	{
#if	1//G_Sensor_DBG
		printf("G Sensor DRDY1_INT1 Set Error , [%02X:%02X]\r\n",CMD_Addr,ODR_Data_RD);
#endif
		return 0;
	}

	return ret;
}

uint8_t G_Sensor_XYZ(void)
{
	int G_Sensor_Handle = 0;
	
	uint8_t CMD_Addr = 0x27;		// STATUS_REG
	uint8_t Status_Reg = 0;
	uint8_t XYZ_Data[10]={0};

	uint8_t ret = 0;
	int result = 0;

	ITPI2cInfo *evt;
	evt = alloca(sizeof(ITPI2cInfo));
	if(SC7A20_Byte_Read(CMD_Addr,&Status_Reg) == 1)
	{
		// ok
#if	G_Sensor_DBG
		printf(" G Sensor Status Reg Read Ok :: STATUS_REg Read [%02X:%02X]\r\n",CMD_Addr,Status_Reg);
#endif
		ret = 1;
	}
	else
	{
		//fail
#if	1//G_Sensor_DBG
		printf(" G Sensor Status Reg Read Fail :: STATUS_REG Read [%02X:%02X]\r\n",CMD_Addr,Status_Reg);
#endif
		return 0;
	}

	if((Status_Reg & 0x0F) == 0x0F)
	{
#if	G_Sensor_DBG
		printf("XYZ New data update\r\n");
#endif
		ret = 1;
	}
	else
	{
#if	0//G_Sensor_DBG
		printf("XYZ New data not update CMD[%02X]=%02X\r\n",CMD_Addr,Status_Reg);
#endif
		return 0;
	}

	G_Sensor_Handle = open(":i2c2", 0);				// i2c2 device open

	memset(XYZ_Data,0,6);
	CMD_Addr = 0x28|0x80;		// OUT_X_L register

	evt->slaveAddress = G_Sensor_Addr;
	evt->cmdBuffer = &CMD_Addr;
	evt->cmdBufferSize = 1;
	evt->dataBuffer = XYZ_Data;
	evt->dataBufferSize = 6;

//	pthread_mutex_lock(&G_Sensor_Mutex);
	result = read(G_Sensor_Handle, evt, 1);
//	pthread_mutex_unlock(&G_Sensor_Mutex);
	
	if(result < 0)
	{
#if	1//G_Sensor_DBG
		printf("XYZ2 Read fail\r\n");
#endif
		return 0;
	}
	else
	{
#if	G_Sensor_DBG
		printf("XYZ2 Read ok , X:%02X %02X Y:%02X %02X Z:%02X %02X\r\n",XYZ_Data[1],XYZ_Data[0],XYZ_Data[3],XYZ_Data[2],XYZ_Data[5],XYZ_Data[4]);
#endif
		ret = 2;
	}

	G_Sensor_X = (short)((XYZ_Data[1] << 8) + XYZ_Data[0]);
	G_Sensor_Y = (short)((XYZ_Data[3] << 8) + XYZ_Data[2]);
	G_Sensor_Z = (short)((XYZ_Data[5] << 8) + XYZ_Data[4]);
	
#if	G_Sensor_DBG
	printf("X:%04x,%dmg, Y:%04x,%dmg, Z:%04x,%dmg\r\n",(uint16_t)G_Sensor_X,G_Sensor_X,(uint16_t)G_Sensor_Y,G_Sensor_Y,(uint16_t)G_Sensor_Z,G_Sensor_Z);
#endif

	return ret;
}

uint8_t SC7A20_Byte_Read(uint8_t ADDR,uint8_t *DATA)
{

	int G_Sensor_Handle = 0;
	
	uint8_t CMD_WR_Buf[10]={0};

	int result = 0;
	uint8_t ret = 0;
	ITPI2cInfo *evt;
	evt = alloca(sizeof(ITPI2cInfo));
	G_Sensor_Handle = open(":i2c2", 0);				// i2c2 device open

	CMD_WR_Buf[0] = ADDR;

	evt->slaveAddress = G_Sensor_Addr;//G_Sensor_Read_Addr;
	evt->cmdBuffer = CMD_WR_Buf;
	evt->cmdBufferSize = 1;
	evt->dataBuffer = (uint8_t *)(CMD_WR_Buf+1);
	evt->dataBufferSize = 1;

	result = read(G_Sensor_Handle, evt, 1);
	
	if(result != 0)
	{
		printf("I2C_2 Read fail CMD[%02X]\r\n",CMD_WR_Buf[0]);
		return 0;
	}
	else
	{
#if	G_Sensor_DBG
		printf("I2C_2 Read ok CMD[%02X]=%02X\r\n",CMD_WR_Buf[0],CMD_WR_Buf[1]);
#endif
		
		*DATA = CMD_WR_Buf[1];
		ret = 1;
	}

	return ret;	
}

uint8_t SC7A20_Byte_Write(uint8_t ADDR,uint8_t DATA)
{

	int G_Sensor_Handle = 0;

	uint8_t CMD_WD_Buf[2];		// Status register
	uint8_t CMD_RD_Buf[2];

	int result = 0;
	uint8_t ret = 0;

	ITPI2cInfo *evt;	
	evt = alloca(sizeof(ITPI2cInfo));
    
	CMD_WD_Buf[0] = ADDR;
	CMD_WD_Buf[1] = DATA;

	G_Sensor_Handle = open(":i2c2", 0);				// i2c2 device open

	evt->slaveAddress = G_Sensor_Addr;//G_Sensor_Read_Addr;
	evt->cmdBuffer = CMD_WD_Buf;
	evt->cmdBufferSize = 2;
	evt->dataBuffer = 0;//CMD_RD_Buf;
	evt->dataBufferSize = 0;//2;

	result = write(G_Sensor_Handle, evt, 1);

	if(result != 0)
	{
		printf("I2C_2 Write fail CMD[%02X]\r\n",CMD_WD_Buf[0]);
		return 0;
	}
	else
	{
#if	G_Sensor_DBG
		printf("I2C_2 Write ok CMD[%02X]=%02X\r\n",CMD_WD_Buf[0],CMD_WD_Buf[1]);
#endif
		ret = 1;
	}

	return ret;	
}

//short G_Sensor_X = 0;
//short G_Sensor_Y = 0;
//short G_Sensor_Z = 0;
#define PI_XO		3.14159265358979323846l



void Cal_XYZ1(void)
{
	double alpha=0l,beta=0l,gamma=0l; 
	double cal_X=0l,cal_Y=0l,cal_Z=0l;

	double Xp=0l,Yp=0l,Zp=0l;
	double Pxyz = 0l;
	double CosP = 0l;

	alpha = 0l;
	cal_X = (double)G_Sensor_X/9806l;
	
	if((cal_X >= -1l) && (cal_X <= 1l))
	{
		alpha = asin(cal_X)*180l/PI_XO;	// degree
#if	G_Sensor_DBG
		printf("Alpha1 = %f , asin_%f\r\n",alpha,cal_X);
#endif
	}
	else if((cal_X < -1l)&&(cal_X >= -2l))
	{
		alpha = asin(cal_X+2l)*180l/PI_XO-90l;	// degree
#if	G_Sensor_DBG
		printf("Alpha2 = %f , asin_%f\r\n",alpha,cal_X);
#endif
	}
	else if((cal_X > 1l)&&(cal_X <= 2l))
	{
		alpha = asin(cal_X-1l)*180l/PI_XO+90l;	// degree
#if	G_Sensor_DBG
	printf("Alpha3 = %f , asin_%f\r\n",alpha,cal_X);
#endif
	}

	beta = 0l;
	cal_Y = (double)G_Sensor_Y/9806l;
	
	if((cal_Y >= -1l) && (cal_Y <= 1l))
	{
		beta = asin(cal_Y)*180l/PI_XO;	// degree
#if	G_Sensor_DBG
		printf("Beta1 = %f , asin_%f\r\n",beta,cal_Y);
#endif
	}
	else if((cal_Y < -1l) && (cal_Y >= -2l))
	{
		beta = asin(cal_Y+1l)*180l/PI_XO-90l;	// degree
#if	G_Sensor_DBG
		printf("Beta2 = %f , asin_%f\r\n",beta,cal_Y);
#endif
	}
	else if((cal_Y > 1l) && (cal_Y <= 2l))
	{
		beta = asin(cal_Y-1l)*180l/PI_XO+90l;	// degree
#if	G_Sensor_DBG
		printf("Beta3 = %f , asin_%f\r\n",beta,cal_Y);
#endif
	}

	gamma = 0l;
	cal_Z = (double)G_Sensor_Z/9806l;
	
	if((cal_Z >= -1l) && (cal_Z <= 1l))
	{
		gamma = acos(cal_Z)*180l/PI_XO;	// degree
#if	G_Sensor_DBG
		printf("Gamma1 = %f , acos_%f\r\n",gamma,cal_Z);
#endif
	}
	else if((cal_Z < -1l) && (cal_Z >= -2l))
	{
		gamma = acos(cal_Z+1l)*180l/PI_XO-90l;	// degree
#if	G_Sensor_DBG
		printf("Gamma2 = %f , acos_%f\r\n",gamma,cal_Z);
#endif
	}
	else if((cal_Z > 1l) && (cal_Z <= 2l))
	{
		gamma = acos(cal_Z-1l)*180l/PI_XO+90l;	// degree
#if	G_Sensor_DBG
		printf("Gamma3 = %f , acos_%f\r\n",gamma,cal_Z);
#endif
	}

	printf("Xg=%f, Yg=%f, Zg=%f, ",alpha,beta,gamma);

}

// #define G_DBG_MSG


uint8_t TempPL_Flag_New = PORTRAIT_2;
bool Cal_XYZ2(uint8_t OpMode)
{
	double Xp=0l,Yp=0l,Zp=0l;
	double Pxyz = 0l;
	double CosP = 0l;

	Xp = (double)G_Sensor_X/2l/9806l;
	Yp = (double)G_Sensor_Y/2l/9806l;
	Zp = (double)G_Sensor_Z/2l/9806l;
	
	#ifdef G_DBG_MSG
		printf("Zp=%f , Xp=%f, Yp=%f, mode:%d\n ",Zp,Xp,Yp,OpMode);
	#endif
//Pxyz = sqrt(pow(Xp,2) + pow(Yp,2) + pow(Zp,2));
//	CosP = Zp/Pxyz;

	#ifdef G_DBG_MSG
//		printf("CosP=%f,DegP=%f\r\n",CosP,CosP*180l/PI_XO);
	#endif
	if (OpMode==1)
	{  //뒤는 40도 앞으20도 , 좌는 15도 
		if((fabs(Zp)<0.6)&&(Xp>0.5)&&(fabs(Yp)<0.25))
		{
			#ifdef G_DBG_MSG
				printf("bL1\r\n");
			#endif
			TempPL_Flag_New = LANDSCAPE_1;
		}
		else if((fabs(Zp)<0.6)&&(Xp<-0.51)&&(fabs(Yp)<0.25))
		{
			#ifdef G_DBG_MSG
				printf("bL2\r\n");
			#endif
			TempPL_Flag_New = LANDSCAPE_2;
		}
		//else if((fabs(Zp)<0.35)&&(Yp>0.74)&&(fabs(Xp)<0.29))
		else if((fabs(Zp)<0.6)&&(Yp>0.35)&&(fabs(Xp)<0.29))
		{
			#ifdef G_DBG_MSG
				printf("bP1\r\n");
			#endif
			TempPL_Flag_New = PORTRAIT_1;
		}
		//else if((fabs(Zp)<0.35)&&(Yp<-0.74)&&(fabs(Xp)<0.29))  // 뒤 30도 
		else if((fabs(Zp)<0.6)&&(Yp<-0.54)&&(fabs(Xp)<0.29))  // 뒤 30도 
		{
			#ifdef G_DBG_MSG
				printf("bP2\r\n");
			#endif
			TempPL_Flag_New = PORTRAIT_2;
		}
		else
		{
			#ifdef G_DBG_MSG
				printf("bNo Check\r\n");
			#endif
		}
	}
	else
	{
		if((fabs(Zp)<0.7)&&(Xp>0.3)&&(fabs(Yp)<0.63))
		{
			printf("LANDSCAPE_1[%d]\r\n",OPI2_1Byte_23bit);
			TempPL_Flag_New = LANDSCAPE_1;
		}
		else if((fabs(Zp)<0.7)&&(Xp<-0.3)&&(fabs(Yp)<0.63))
		{
			printf("LANDSCAPE_2[%d]\r\n",OPI2_1Byte_23bit);
			TempPL_Flag_New = LANDSCAPE_2;
		}
		else if((fabs(Zp)<0.7)&&(Yp>=0.30)&&(fabs(Xp)<=0.63))
		{
			printf("PORTRAIT_1[%d]\r\n",OPI2_1Byte_23bit);
			TempPL_Flag_New = PORTRAIT_1;
		}
		else if((fabs(Zp)<0.7)&&(Yp<=-0.3)&&(fabs(Xp)<=0.63))
		{
			printf("PORTRAIT_2[%d]\r\n",OPI2_1Byte_23bit);
			TempPL_Flag_New = PORTRAIT_2;
		}
		else
		{
//			printf("No Check\r\n");
			TempPL_Flag_New = PORTRAIT_2;
		}
	}
#if 0	// 2024-12-16 V26
	if ((OPI2_1Byte_23bit==0)||(OPI2_1Byte_23bit==3))
		return 0;
#endif
	
	if (OpMode==0)
	{
		if(TempPL_Flag_New != PL_Flag_New)
		{
			PL_Flag_New = TempPL_Flag_New;
			switch(PL_Flag_New)
			{
				case NO_CHECK:
//						printf("aNo Check\r\n");
				break;
				
				case PORTRAIT_1:
					printf("aPORTRATE_1\r\n");
				break;
				
				case PORTRAIT_2:
					printf("aPORTRATE_2\r\n");
				break;
				
				case LANDSCAPE_1:
					printf("aLANDSCAPE_1\r\n");
				break;
				
				case LANDSCAPE_2:
					printf("aLANDSCAPE_2\r\n");
				break;
				
				default:
				break;
			}
		}
	}
	else
	{
		if(TempPL_Flag_New != PL_Flag_New)
		{
//				printf("==>Zp=%f , Xp=%f, Yp=%f, mode:%d, PL_Flag_Cnt:%d\n ",Zp,Xp,Yp,TempPL_Flag_New,PL_Flag_Cnt);
			PL_Flag_Cnt++;
			if (PL_Flag_Cnt>5)
			{
				PL_Flag_New = TempPL_Flag_New;
				PL_Flag_Cnt = 0;
				switch(PL_Flag_New)
				{
					case NO_CHECK:
//							printf("bNo Check\r\n");
					break;
					
					case PORTRAIT_1:
						printf("bPORTRATE_1\r\n");
					break;
					
					case PORTRAIT_2:
						printf("bPORTRATE_2\r\n");
					break;
					
					case LANDSCAPE_1:
						printf("bLANDSCAPE_1\r\n");
					break;
					
					case LANDSCAPE_2:
						printf("bLANDSCAPE_2\r\n");
					break;
					
					default:
					break;
				}
			}
		}
		else
			PL_Flag_Cnt = 0;
	}
	return 1;
}		

uint8_t Get_PL_Flag(void)
{
	return PL_Flag_New;
}

/*
// G Sensor INT1 ISR
void G_SensorISR(void* data)
{

	printf(">>>>>>>>>>>>> G Sensor DRDY1_INT1 >>>>>>>>>>>>>>\r\n");	
	ithGpioClearIntr(ITH_GPIO_PIN29);
}
*/
