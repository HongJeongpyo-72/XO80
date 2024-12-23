#include <locale.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"

#include "../aXO80/HT_Define.H"

#define MAIN_STACK_SIZE 100000

extern uint32_t __lcd_base_a;
extern uint32_t __lcd_base_b;
extern uint32_t __lcd_base_c;

extern void* BootloaderMain(void* arg);

static void* MainTask(void* arg)
{
#if 0 //def CFG_RTC_INTERNAL
    {
        // check RTC clk is live
        uint32_t wd_counter = 0;
        uint32_t timeout = 100000000;
        
        ithSetRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 4);
        ithSetRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 0);

        wd_counter = ithWatchDogGetCounter();

        while(wd_counter == ithWatchDogGetCounter()) {
            if(timeout <= 0) {
                ithPrintf("RTC's power is dead\n");
                timeout = 100000000;
            }
            timeout--;
        }

        ithClearRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 0);
        ithClearRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 4);
    }
#else
    {
        // check RTC clk is live
        uint32_t wd_counter = 0;
        uint32_t timeout = 100000000;
        wd_counter = ithWatchDogGetCounter();
        ithClearRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 0);
        ithClearRegBitA(ITH_WD_BASE + ITH_WD_CR_REG, 4);
    }
#endif
    // init watch dog
#ifdef CFG_WATCHDOG_ENABLE
    itpRegisterDevice(ITP_DEVICE_WATCHDOG, &itpDeviceWatchDog);
    ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_ENABLE, NULL);
#endif // CFG_WATCHDOG_ENABLE

#ifdef CFG_DASHBOARD
    //FOR IT976 CARHMI  DEMO BOARD 
    ithGpioSetMode(ITH_GPIO_PIN60, ITH_GPIO_MODE0);//LCD POWER
    ithGpioSetOut(ITH_GPIO_PIN60);
    ithGpioClear(ITH_GPIO_PIN60);
    ithGpioSetMode(ITH_GPIO_PIN62, ITH_GPIO_MODE0);//LCD STBY
    ithGpioSetOut(ITH_GPIO_PIN62);
    ithGpioClear(ITH_GPIO_PIN62);
    ithGpioSetMode(ITH_GPIO_PIN61, ITH_GPIO_MODE0);//LCD RST
    ithGpioSetOut(ITH_GPIO_PIN61);
    ithGpioSet(ITH_GPIO_PIN61);
    usleep(20);
    ithGpioClear(ITH_GPIO_PIN61); 
    usleep(2000);
    ithGpioSet(ITH_GPIO_PIN61);
    usleep(1000);
    ithGpioSet(ITH_GPIO_PIN62);//LCD STBY
    usleep(10);
#endif

#ifdef CFG_GPIO_LCD_PWR_EN
#if (CFG_GPIO_LCD_PWR_EN > 0)
    ithGpioSetMode(CFG_GPIO_LCD_PWR_EN, ITH_GPIO_MODE0);  //LCD_PWR_EN
    ithGpioSetOut(CFG_GPIO_LCD_PWR_EN);
    #ifdef CFG_GPIO_LCD_PWR_EN_ACTIVE_LOW
    ithGpioClear(CFG_GPIO_LCD_PWR_EN);
    #else
    ithGpioSet(CFG_GPIO_LCD_PWR_EN);
    #endif
#endif
#endif // CFG_GPIO_LCD_PWR_EN

#define CFG_GPIO_LCD_RESET	38
#define HT_GPIO_BUZZER		62
#define CFG_GPIO_USB_POWER	19



    ithGpioClear(HT_GPIO_BUZZER);
    ithGpioSetMode(HT_GPIO_BUZZER, ITH_GPIO_MODE0); 
    ithGpioSetOut(HT_GPIO_BUZZER);
    ithGpioClear(HT_GPIO_BUZZER);
    
    
printf("================== CFG_GPIO_LCD_RESET S\n");
#if 0
	ithGpioClear(CFG_GPIO_LCD_RESET);
	ithGpioSetMode(CFG_GPIO_LCD_RESET, ITH_GPIO_MODE0);
	ithGpioSetOut(CFG_GPIO_LCD_RESET);

	
	ithGpioSet(CFG_GPIO_LCD_RESET);
	usleep(1000);
	ithGpioClear(CFG_GPIO_LCD_RESET);
	usleep(50000);
	ithGpioSet(CFG_GPIO_LCD_RESET);
//	usleep(130000);   
#else

#endif
printf("================== CFG_GPIO_LCD_RESET E\n");
    ithGpioClear(CFG_GPIO_USB_POWER);
	ithGpioSetMode(CFG_GPIO_USB_POWER, ITH_GPIO_MODE0);	//USB POWER
    ithGpioSetOut(CFG_GPIO_USB_POWER);
    ithGpioClear(CFG_GPIO_USB_POWER);
    usleep(100);
    ithGpioSet(CFG_GPIO_USB_POWER);
    // init lcd
#ifdef CFG_LCD_ENABLE


printf("================== MainTask\n");
printf("================== MainTask\n");
printf("================== MainTask\n");

#if 1	   // JPHong ORG
    itpRegisterDevice(ITP_DEVICE_SCREEN, &itpDeviceScreen);

	#ifdef CFG_BL_SHOW_LOGO
	    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESET, NULL);
	
	    ithLcdSetBaseAddrA((uint32_t) &__lcd_base_a);
	    ithLcdSetBaseAddrB((uint32_t) &__lcd_base_b);
		#if defined(CFG_VIDEO_ENABLE) || defined(CFG_LCD_TRIPLE_BUFFER)
		    ithLcdSetBaseAddrC((uint32_t) &__lcd_base_c);
		#endif
		
		
	#endif

#endif

#endif // CFG_LCD_ENABLE

#ifdef CFG_BACKLIGHT_ENABLE
//    itpRegisterDevice(ITP_DEVICE_BACKLIGHT, &itpDeviceBacklight);
 //   ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_INIT, NULL);
#endif // CFG_BACKLIGHT_ENABLE

    // init i2c0 device
#ifdef CFG_I2C0_ENABLE
	IIC_MODE iic_port0_mode = MASTER_MODE;
    itpRegisterDevice(ITP_DEVICE_I2C0, &itpDeviceI2c0);
    ioctl(ITP_DEVICE_I2C0, ITP_IOCTL_INIT, (void*)iic_port0_mode);
#endif

    // init i2c1 device
#ifdef CFG_I2C1_ENABLE
	IIC_MODE iic_port1_mode = MASTER_MODE;
    itpRegisterDevice(ITP_DEVICE_I2C1, &itpDeviceI2c1);
    ioctl(ITP_DEVICE_I2C1, ITP_IOCTL_INIT, (void*)iic_port1_mode);
#endif

    // init i2c2 device
#ifdef CFG_I2C2_ENABLE
    IIC_MODE iic_port2_mode = MASTER_MODE;
    itpRegisterDevice(ITP_DEVICE_I2C2, &itpDeviceI2c2);
    ioctl(ITP_DEVICE_I2C2, ITP_IOCTL_INIT, (void*)iic_port2_mode);
#endif

    // init i2c3 device
#ifdef CFG_I2C3_ENABLE
    IIC_MODE iic_port3_mode = MASTER_MODE;
    itpRegisterDevice(ITP_DEVICE_I2C3, &itpDeviceI2c3);
    ioctl(ITP_DEVICE_I2C3, ITP_IOCTL_INIT, (void*)iic_port3_mode);
#endif


#ifdef CFG_RTC_ENABLE
    itpRegisterDevice(ITP_DEVICE_RTC, &itpDeviceRtc);
    ioctl(ITP_DEVICE_RTC, ITP_IOCTL_INIT, NULL);
#endif // CFG_RTC_ENABLE

    // init GPIO expander device
#ifdef CFG_GPIO_EXPANDER_ENABLE
    //itpRegisterDevice(ITP_DEVICE_GPIO_EXPANDER, &itpDeviceGpioExpander);
    //ioctl(ITP_DEVICE_GPIO_EXPANDER, ITP_IOCTL_INIT, NULL);
    itpIOExpanderInit();
#endif

    // init keypad device
#ifdef CFG_KEYPAD_ENABLE
    itpRegisterDevice(ITP_DEVICE_KEYPAD, &itpDeviceKeypad);
    ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_INIT, NULL);
#endif

    // init power device
#if defined(CFG_BATTERY_ENABLE) || defined(CFG_POWER_ON) || defined(CFG_POWER_STANDBY) || defined(CFG_POWER_SLEEP) || defined(CFG_POWER_SUSPEND) || defined(CFG_POWER_OFF)
    itpRegisterDevice(ITP_DEVICE_POWER, &itpDevicePower);
    ioctl(ITP_DEVICE_POWER, ITP_IOCTL_INIT, NULL);
#endif

#ifdef CFG_SPI_ENABLE
    itpRegisterDevice(ITP_DEVICE_SPI, &itpDeviceSpi0);
    ioctl(ITP_DEVICE_SPI, ITP_IOCTL_INIT, NULL);
#endif

#ifdef CFG_SPI1_ENABLE
    itpRegisterDevice(ITP_DEVICE_SPI1, &itpDeviceSpi1);
    ioctl(ITP_DEVICE_SPI1, ITP_IOCTL_INIT, NULL);
#endif

    // init nand device
#ifdef CFG_NAND_ENABLE
    itpRegisterDevice(ITP_DEVICE_NAND, &itpDeviceNand);
    ioctl(ITP_DEVICE_NAND, ITP_IOCTL_INIT, NULL);
#endif

    // init nor device
#ifdef CFG_NOR_ENABLE
    itpRegisterDevice(ITP_DEVICE_NOR, &itpDeviceNor);
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_INIT, NULL);
#endif

    // init sd0 device
#ifdef CFG_SD0_STATIC
    itpRegisterDevice(ITP_DEVICE_SD0, &itpDeviceSd0);
    ioctl(ITP_DEVICE_SD0, ITP_IOCTL_INIT, NULL);
#endif

    // init sd1 device
#ifdef CFG_SD1_STATIC
    itpRegisterDevice(ITP_DEVICE_SD1, &itpDeviceSd1);
    ioctl(ITP_DEVICE_SD1, ITP_IOCTL_INIT, NULL);
#endif

    // init mac
#ifdef CFG_NET_MAC_INIT_ON_BOOTLOADER
    extern int MacInit(bool initPhy);
    MacInit(true);
#endif



    // init decompress
#ifdef CFG_DCPS_ENABLE
    itpRegisterDevice(ITP_DEVICE_DECOMPRESS, &itpDeviceDecompress);
#endif

    // enable gpio interrupt
    ithIntrEnableIrq(ITH_INTR_GPIO);

    // set locale
    setlocale(LC_CTYPE, "C-UTF-8");

    // print debug information
    ithClockStats();

    // invoke main function
    BootloaderMain(arg);
}

int main(void)
{
    pthread_t task;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, MAIN_STACK_SIZE);
    pthread_create(&task, &attr, MainTask, NULL);

    /* Now all the tasks have been started - start the scheduler. */
    vTaskStartScheduler();

    /* Should never reach here! */
    return 0;
}
