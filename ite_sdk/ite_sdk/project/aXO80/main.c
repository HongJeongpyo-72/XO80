#include <sys/ioctl.h>
#include <unistd.h>
#include "libxml/parser.h"
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"
#include "network_config.h"

#include "HT_DispDdefine.h"


#ifdef _WIN32
    #include <crtdbg.h>
#else
    #include "openrtos/FreeRTOS.h"
    #include "openrtos/task.h"
#endif

extern void BackupInit(void);
extern void BackupRestore(void);
extern void BackupSyncFile(void);
extern void BackupDestroy(void);

extern void G_Sensor_Init(void);				// extern 
extern uint8_t G_Sensor_XYZ(void);
bool Cal_XYZ2(uint8_t OpMode);
uint32_t TempMainTimerCnt_10m = 0;
void boot_buzzer_check()
{
	if (gMainTimerCnt_10m>(TempMainTimerCnt_10m+50))
	{
		ithGpioClear(HT_GPIO_BUZZER);
		printf("HT_GPIO_BUZZER : OFF\n");
	}
}


extern void _tpDoPowerOnSeq_vendor1(char ResetPin,char IntPin);
extern void _tpDoPowerOnSeq_vendor2(char ResetPin,char IntPin);
extern void _tpDoPowerOnSeq_vendor3(char ResetPin,char IntPin);



int SDL_main(int argc, char *argv[])
{
    int ret = 0;
    int restryCount = 0;
    
    printf("Start SDL_main\n");
    
    
	#ifdef CFG_LCD_MULTIPLE
		ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESET, (void*)0);
	#endif
	
	#if !defined(CFG_LCD_INIT_ON_BOOTING) && !defined(CFG_BL_SHOW_LOGO)
		ScreenClear();
	#endif
	_tpDoPowerOnSeq_vendor1(CFG_GPIO_TOUCH_RESET,CFG_GPIO_TOUCH_INT);
	G_Sensor_Init();
	G_Sensor_XYZ();
	#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
		BackupInit();
		retry_backup:
		ret = UpgradeInit();
		if (ret)
		{
			if (++restryCount > 2)
			{
				printf("cannot recover from backup....\n");
				goto end;
			}
			BackupRestore();
			goto retry_backup;
		}
		BackupSyncFile();
	#else
		ret = UpgradeInit();
		if (ret)
		goto end;
	#endif
	_tpDoPowerOnSeq_vendor2(CFG_GPIO_TOUCH_RESET,CFG_GPIO_TOUCH_INT);
	G_Sensor_XYZ();
	CanbusTxDataManagement_mutex();
	G_Sensor_XYZ();
	gMain_timer();
	G_Sensor_XYZ();
	HT_Init_GPIO();
	
	G_Sensor_XYZ();
	
	XO_MICOM_INT();

//	XO_SVM_INT();   // 일체형일때만 사용

		#if defined(CFG_SENSOR_ENABLE)
			TP2825_init();
		#endif
		
	// ithGpioSet(HT_GPIO_BUZZER); JPHong 2023-06-08
	TempMainTimerCnt_10m = gMainTimerCnt_10m;
	
	#ifdef CFG_LCD_MULTIPLE
		ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
		usleep(100000);
		ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
	#endif // CFG_LCD_MULTIPLE
	
	#ifdef	CFG_DYNAMIC_LOAD_TP_MODULE
		//This function must be in front of SDL_Init().
sdfsdf		DynamicLoadTpModule();
	#endif
	
	boot_buzzer_check();
	G_Sensor_XYZ();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
	gInitVariableData();
	G_Sensor_XYZ();
	ConfigInit();
	printf("XO_MICOM_SOFT_REQ:On Main\n");
	CanbusTxDataManagement( gMainTimerCnt_10m,XO_MICOM_SOFT_REQ,NULL);
	boot_buzzer_check();
	
	
	boot_buzzer_check();

	#ifdef CFG_NET_ENABLE
		NetworkInit();
		#ifdef CFG_NET_WIFI
			if (Ethernet_Wifi_DualMAC == 1)
			WebServerInit();
		#else 
			WebServerInit();
		#endif
	#endif // CFG_NET_ENABLE
	G_Sensor_XYZ();
	boot_buzzer_check();
	G_Sensor_XYZ();
	ScreenInit();
	G_Sensor_XYZ();
	boot_buzzer_check();
	
//	ExternalInit();
	#if defined(CFG_UPGRADE_FROM_UART_RUN_TIME)
		UpgradeUartInit();
	#endif
	G_Sensor_XYZ();
	boot_buzzer_check();
	
	//StorageInit();
	
	boot_buzzer_check();
	
	//    AudioInit();
	G_Sensor_XYZ();
	boot_buzzer_check();
	
	//    PhotoInit();
	
	boot_buzzer_check();
	
	G_Sensor_XYZ();
	SceneInit();
	G_Sensor_XYZ();
	boot_buzzer_check();
    G_Sensor_XYZ();
    #ifdef TEST_ITU_USB   
	{
		int	periodPerFrame;
		periodPerFrame = SDL_GetTicks();
		int delay;
		do
		{
			sched_yield(); 
			delay = SDL_GetTicks()-periodPerFrame;
			
//			printf("periodPerFrame:%d ,%d ,%d\n",periodPerFrame,SDL_GetTicks(),delay);
			SDL_Delay(1);
			
		}while (delay < 2500);					//#define TIME_CNT_33ms_5000MS	152
	
	}
	#endif
	G_Sensor_XYZ();

	SceneLoad();
	//   printf("SDL_main1\n");
	//   InitCanbus();
	//   printf("SDL_main2\n"); 
	_tpDoPowerOnSeq_vendor3(CFG_GPIO_TOUCH_RESET,CFG_GPIO_TOUCH_INT);

	boot_buzzer_check();
	
	boot_buzzer_check();
	ret = SceneRun();
	printf("SDL_main3:%d\n",ret);
	SceneExit();
	printf("SDL_main4:%d\n",ret);
	ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_EXIT, NULL);
	printf("SDL_main5:%d\n",ret);
	//    PhotoExit();
	//   AudioExit();
	#if defined(CFG_UPGRADE_FROM_UART_RUN_TIME)
		UpgradeUartExit();
	#endif
//	ExternalExit();
	printf("SDL_main6:%d\n",ret);
	#ifdef CFG_NET_ENABLE
		if (ret != QUIT_UPGRADE_WEB)
			WebServerExit();
	
		xmlCleanupParser();
	#endif // CFG_NET_ENABLE
	printf("SDL_main7:%d\n",ret);
	ConfigExit();
	SDL_Quit();
	printf("SDL_main8:%d\n",ret);
	#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
		BackupDestroy();
	#endif
	printf("SDL_main9:%d\n",ret);
	#ifdef _WIN32
		_CrtDumpMemoryLeaks();
	#else
		if (0)
		{
			#if (configUSE_TRACE_FACILITY == 1)
				static signed char buf[2048];
				vTaskList(buf);
				puts(buf);
			#endif
			malloc_stats();
			
			#ifdef CFG_DBG_RMALLOC
				Rmalloc_stat(__FILE__);
			#endif
		}
	#endif // _WIN32
	printf("SDL_main10:%d\n",ret);
end:
	printf("SDL_main11:%d\n",ret);
	ret = UpgradeProcess(ret);
	itp_codec_standby();
	exit(ret);
	return ret;
}
