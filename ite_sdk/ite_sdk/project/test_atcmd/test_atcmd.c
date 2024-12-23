#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "bt/bluetooth.h"

#define AT_NUM 3

void* TestFunc(void* arg)
{
	int i = 0;
    char getstr[256];
    char sendtr[256];
	int len = 0;
	int fd;
	char atc[40];
	bool readFin = true;
	const char *atc0[AT_NUM] = { //[27] = { 
#if CFG_ESP32
		//"AT\r\n",
		//"AT+GMR\r\n",
		"AT+CWMODE=3\r\n",
		"AT+CWJAP=\"dd-wrt-ITE\",\"ff75525613\"\r\n",
		"AT+CWLAP\r\n",
#else
		"AT\r\n",
		"AT+VERSION\r\n",
		//"AT+LADDR\r\n",
		"AT+NAME\r\n",
		//"AT+PIN\r\n",
		//"AT+TYPE\r\n",
		//"AT+BAUD\r\n",
		//"AT+STOP\r\n",
		//"AT+PARI\r\n",
		//"AT+ADVI\r\n",
		//"AT+NOTI\r\n",
		//"AT+NOTP\r\n",
		//"AT+IMME\r\n",
		//"AT+START\r\n",
		//"AT+UUID\r\n",
		//"AT+CHAR\r\n",
		//"AT+IBEA\r\n",
		//"AT+IBE0\r\n",
		//"AT+IBE1\r\n",
		//"AT+IBE2\r\n",
		//"AT+IBE3\r\n",
		//"AT+MARJ\r\n",
		//"AT+MINO\r\n",
		//"AT+MEA\r\n",
		//"AT+PWRM\r\n",
		//"AT+ROLE\r\n",
		//"AT+INQ\r\n"
		//"\r\n",
#endif
	};
	printf("Start uart test!\n");

    itpRegisterDevice(ITP_DEVICE_UART1, &itpDeviceUart1);
    ioctl(ITP_DEVICE_UART1, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_UART1, ITP_IOCTL_RESET, CFG_UART1_BAUDRATE);
	//usleep(5000000);
	fd = bt_dev_open(ITP_DEVICE_UART1);
	if (fd)
		printf("open device success\n");
	else
		printf("open device fail\n");
    while(1)
    {
    	if(readFin)
    	{
	    	printf("send at command\n");
	   		strcpy(atc, atc0[i++ % AT_NUM]);
	   		bt_dev_write(fd, atc, (size_t)strlen(atc));
			readFin = false;
    	}
	   	memset(getstr, 0, 256);
		len = bt_dev_read(fd, getstr, 256);
	   	if(len > 0)
			printf("uart read: %s\n",getstr);
		else
			readFin = true;
		usleep(500000*2);
	}
}