/** @file
 * ITE Display Control Board Modules.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2015
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup ctrlboard ITE Display Control Board Modules
 *  @{
 */
#ifndef CTRLBOARD_H
#define CTRLBOARD_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ctrlboard_audio Audio Player
 *  @{
 */
 // Test for 2 itu file rotation
typedef enum
{
    EXTERNAL_HORIZONTAL,   ///< Horizontal
    EXTERNAL_VERTICAL      ///< Vertical
} ExternalOrientation;

typedef int (*AudioPlayCallback)(int state);

/**
 * Initializes audio module.
 */
void AudioInit(void);

/**
 * Exits audio module.
 */
void AudioExit(void);

/**
 * Plays the specified wav file.
 *
 * @param filename The specified wav file to play.
 * @param func The callback function.
 * @return 0 for success, otherwise failed.
 */
int AudioPlay(char* filename, AudioPlayCallback func);

/**
 * Stops playing sound.
 */
void AudioStop(void);

int AudioPlayMusic(char* filename, AudioPlayCallback func);

/**
 * Plays keypad sound.
 */
void AudioPlayKeySound(void);
void AudioPauseKeySound(void);
void AudioResumeKeySound(void);

/**
 * Sets the volume of keypad sound.
 *
 * @param level the percentage of volume.
 */
void AudioSetKeyLevel(int level);

/**
 * Mutes all audio.
 */
void AudioMute(void);

/**
 * Un-mutes all audio.
 */
void AudioUnMute(void);

/**
 * Determines whether this audio is muted or not.
 *
 * @return true muted, false otherwise.
 */
bool AudioIsMuted(void);

bool AudioIsPlaying(void);

void AudioSetVolume(int level);
void AudioSetLevel(int level);
int AudioGetVolume(void);

/** @} */ // end of ctrlboard_audio

/** @defgroup ctrlboard_config Configuration
 *  @{
 */
/**
 * Language type definition.
 */
typedef enum
{
    LANG_ENG,   ///< English
    LANG_CHT,   ///< Traditional Chinese
    LANG_CHS    ///< Simplified Chinese
} LangType;

typedef enum
{
    WIFI_CLIENT = 0,   ///< Client mode
    WIFI_SOFTAP,   ///< SoftAP mode
} WifiType;


/**
 * Main menu type definition.
 */
typedef enum
{
    MAINMENU_COVERFLOW,             ///< Cover flow
    MAINMENU_COVERFLOW_REFLECTION,  ///< Cover flow with reflection effect
    MAINMENU_PAGEFLOW_FLIP,         ///< Flip page flow
    MAINMENU_PAGEFLOW_FLIP2,        ///< Flip2 page flow
    MAINMENU_PAGEFLOW_FOLD,         ///< Fold page flow
    MAINMENU_COVERFLOW_RIPPLE       ///< Cover flow with ripple effect
} MainMenuType;


char USER_NAME[15][40];//chpark.08.22
char USER_PC[15][10];//chpark.8.22


typedef struct
{
    // network
    int dhcp;                                                           ///< Enable DHCP or not
    char ipaddr[16];                                                    ///< IP address
    char netmask[16];                                                   ///< Netmask
    char gw[16];                                                        ///< Gateway address
    char dns[16];                                                       ///< DNS address

    // display
    int brightness;                                                     ///< Brightness, the range is 0~9
    int screensaver_time;                                               ///< Time to enter screen saver mode, unit is minute
    int screensaver_type;                                               ///< Screen saver type @see ScreensaverType
    int lang;                                                           ///< Language type @see LangType
    int mainmenu_type;                                                  ///< Main menu type @see MainMenuType

    // sound
    char      keysound[PATH_MAX];                                       // /< Key sound file path
    int       keylevel;                                                 // /< Key volume percentage, range is 0~100
    int       audiolevel;                                               // /< Audio volume percentage, range is 0~100

    // photo
    int       photo_interval;                                           // /< Photo change interval (second)

    // setting
    int touch_calibration;                                              ///< Need to do the touch calibration or not

    // wifi
    char ssid[64];
    char password[256];
    char secumode[3];
    int wifi_mode;
    int wifi_on_off;
    // wifi
    char ap_ssid[64];  // ap mode ssid
    char ap_password[256]; // ap mode password
    

	// login
    char user_id[64];
    char user_password[64];

	int order[2][6];



// ==================   XO  (Menu)===========================
//chpark.08.22
char CU_0_NAME[10];	// User Name[유저별] ; 미정 , 10byte 
char CU_1_NAME[10];
char CU_2_NAME[10];
char CU_3_NAME[10];
char CU_4_NAME[10];
char CU_5_NAME[10];
char CU_6_NAME[10];
char CU_7_NAME[10];
char CU_8_NAME[10];
char CU_9_NAME[10];
char CU_10_NAME[40];
char CU_11_NAME[40];
char CU_12_NAME[40];
char CU_13_NAME[40];
char CU_14_NAME[40];

char CU_0_PC[9];		// User Pass Code [유저별] ; 미정 , 최대 9byte (가변가능 4 ~ 9 자리)
char CU_1_PC[9];
char CU_2_PC[9];
char CU_3_PC[9];
char CU_4_PC[9];
char CU_5_PC[9];
char CU_6_PC[9];
char CU_7_PC[9];
char CU_8_PC[9];
char CU_9_PC[9];
char CU_10_PC[10];
char CU_11_PC[10];
char CU_12_PC[10];
char CU_13_PC[10];
char CU_14_PC[10];

int ConfigM7_0_Group;		// uint8_t M7_Group[유저별];	// Menu_Layer.h , 추후 유저부분 변경에 
int ConfigM7_1_Group;
int ConfigM7_2_Group;
int ConfigM7_3_Group;
int ConfigM7_4_Group;
int ConfigM7_5_Group;
int ConfigM7_6_Group;
int ConfigM7_7_Group;
int ConfigM7_8_Group;
int ConfigM7_9_Group;

int CU_0_A_V_0;		//UserPresetData M7_UserSet[유저별][Preset Group Nomber];	// Menu_Layer.h , AUX1  데이터 값
int CU_0_B_V_0;
int CU_0_P1_V_0;
int CU_0_ATT_S_0;
int CU_0_Way_O_0;
int CU_0_A_V_1;
int CU_0_B_V_1;
int CU_0_P1_V_1;
int CU_0_ATT_S_1;
int CU_0_Way_O_1;
int CU_0_A_V_2;
int CU_0_B_V_2;
int CU_0_P1_V_2;
int CU_0_ATT_S_2;
int CU_0_Way_O_2;

int CU_1_A_V_0;
int CU_1_B_V_0;
int CU_1_P1_V_0;
int CU_1_ATT_S_0;
int CU_1_Way_O_0;
int CU_1_A_V_1;
int CU_1_B_V_1;
int CU_1_P1_V_1;
int CU_1_ATT_S_1;
int CU_1_Way_O_1;
int CU_1_A_V_2;
int CU_1_B_V_2;
int CU_1_P1_V_2;
int CU_1_ATT_S_2;
int CU_1_Way_O_2;

int CU_2_A_V_0;
int CU_2_B_V_0;
int CU_2_P1_V_0;
int CU_2_ATT_S_0;
int CU_2_Way_O_0;
int CU_2_A_V_1;
int CU_2_B_V_1;
int CU_2_P1_V_1;
int CU_2_ATT_S_1;
int CU_2_Way_O_1;
int CU_2_A_V_2;
int CU_2_B_V_2;
int CU_2_P1_V_2;
int CU_2_ATT_S_2;
int CU_2_Way_O_2;

int CU_3_A_V_0;
int CU_3_B_V_0;
int CU_3_P1_V_0;
int CU_3_ATT_S_0;
int CU_3_Way_O_0;
int CU_3_A_V_1;
int CU_3_B_V_1;
int CU_3_P1_V_1;
int CU_3_ATT_S_1;
int CU_3_Way_O_1;
int CU_3_A_V_2;
int CU_3_B_V_2;
int CU_3_P1_V_2;
int CU_3_ATT_S_2;
int CU_3_Way_O_2;

int CU_4_A_V_0;
int CU_4_B_V_0;
int CU_4_P1_V_0;
int CU_4_ATT_S_0;
int CU_4_Way_O_0;
int CU_4_A_V_1;
int CU_4_B_V_1;
int CU_4_P1_V_1;
int CU_4_ATT_S_1;
int CU_4_Way_O_1;
int CU_4_A_V_2;
int CU_4_B_V_2;
int CU_4_P1_V_2;
int CU_4_ATT_S_2;
int CU_4_Way_O_2;

int CU_5_A_V_0;
int CU_5_B_V_0;
int CU_5_P1_V_0;
int CU_5_ATT_S_0;
int CU_5_Way_O_0;
int CU_5_A_V_1;
int CU_5_B_V_1;
int CU_5_P1_V_1;
int CU_5_ATT_S_1;
int CU_5_Way_O_1;
int CU_5_A_V_2;
int CU_5_B_V_2;
int CU_5_P1_V_2;
int CU_5_ATT_S_2;
int CU_5_Way_O_2;

int CU_6_A_V_0;
int CU_6_B_V_0;
int CU_6_P1_V_0;
int CU_6_ATT_S_0;
int CU_6_Way_O_0;
int CU_6_A_V_1;
int CU_6_B_V_1;
int CU_6_P1_V_1;
int CU_6_ATT_S_1;
int CU_6_Way_O_1;
int CU_6_A_V_2;
int CU_6_B_V_2;
int CU_6_P1_V_2;
int CU_6_ATT_S_2;
int CU_6_Way_O_2;

int CU_7_A_V_0;
int CU_7_B_V_0;
int CU_7_P1_V_0;
int CU_7_ATT_S_0;
int CU_7_Way_O_0;
int CU_7_A_V_1;
int CU_7_B_V_1;
int CU_7_P1_V_1;
int CU_7_ATT_S_1;
int CU_7_Way_O_1;
int CU_7_A_V_2;
int CU_7_B_V_2;
int CU_7_P1_V_2;
int CU_7_ATT_S_2;
int CU_7_Way_O_2;

int CU_8_A_V_0;
int CU_8_B_V_0;
int CU_8_P1_V_0;
int CU_8_ATT_S_0;
int CU_8_Way_O_0;
int CU_8_A_V_1;
int CU_8_B_V_1;
int CU_8_P1_V_1;
int CU_8_ATT_S_1;
int CU_8_Way_O_1;
int CU_8_A_V_2;
int CU_8_B_V_2;
int CU_8_P1_V_2;
int CU_8_ATT_S_2;
int CU_8_Way_O_2;

int CU_9_A_V_0;
int CU_9_B_V_0;
int CU_9_P1_V_0;
int CU_9_ATT_S_0;
int CU_9_Way_O_0;
int CU_9_A_V_1;
int CU_9_B_V_1;
int CU_9_P1_V_1;
int CU_9_ATT_S_1;
int CU_9_Way_O_1;
int CU_9_A_V_2;
int CU_9_B_V_2;
int CU_9_P1_V_2;
int CU_9_ATT_S_2;
int CU_9_Way_O_2;


int CU_AUX2_0C_V;		//UserAUX24Data M7_UserAUX24Set[유저별];	// Menu_Layer.h , AUX2/4 데이터 값
int CU_AUX2_0D_V;
int CU_AUX2_0P2_V;
int CU_AUX2_0G_V;
int CU_AUX2_0H_V;
int CU_AUX2_0P4_V;

int CU_AUX2_1C_V;
int CU_AUX2_1D_V;
int CU_AUX2_1P2_V;
int CU_AUX2_1G_V;
int CU_AUX2_1H_V;
int CU_AUX2_1P4_V;

int CU_AUX2_2C_V;
int CU_AUX2_2D_V;
int CU_AUX2_2P2_V;
int CU_AUX2_2G_V;
int CU_AUX2_2H_V;
int CU_AUX2_2P4_V;

int CU_AUX2_3C_V;
int CU_AUX2_3D_V;
int CU_AUX2_3P2_V;
int CU_AUX2_3G_V;
int CU_AUX2_3H_V;
int CU_AUX2_3P4_V;

int CU_AUX2_4C_V;
int CU_AUX2_4D_V;
int CU_AUX2_4P2_V;
int CU_AUX2_4G_V;
int CU_AUX2_4H_V;
int CU_AUX2_4P4_V;

int CU_AUX2_5C_V;
int CU_AUX2_5D_V;
int CU_AUX2_5P2_V;
int CU_AUX2_5G_V;
int CU_AUX2_5H_V;
int CU_AUX2_5P4_V;

int CU_AUX2_6C_V;
int CU_AUX2_6D_V;
int CU_AUX2_6P2_V;
int CU_AUX2_6G_V;
int CU_AUX2_6H_V;
int CU_AUX2_6P4_V;

int CU_AUX2_7C_V;
int CU_AUX2_7D_V;
int CU_AUX2_7P2_V;
int CU_AUX2_7G_V;
int CU_AUX2_7H_V;
int CU_AUX2_7P4_V;

int CU_AUX2_8C_V;
int CU_AUX2_8D_V;
int CU_AUX2_8P2_V;
int CU_AUX2_8G_V;
int CU_AUX2_8H_V;
int CU_AUX2_8P4_V;

int CU_AUX2_9C_V;
int CU_AUX2_9D_V;
int CU_AUX2_9P2_V;
int CU_AUX2_9G_V;
int CU_AUX2_9H_V;
int CU_AUX2_9P4_V;
	
	
int ConfigLang_Flag;		// Lang_Flag Menu_Layer.h , Menu5_Layer.c
int ConfigRd_Rg;			// Radio_Region M7S7_Layer.c 선언 , 메인 변수와 연동및 저장
int ConfigRd_PC;			// static uint8_t cRadio_PowerCondition;		// M7S7_Layer.c 선언 , 메인 변수와 연
int	ConfigM7UI;				//	uint8_t M7_UserIndex;	// Menu_Layer.h , 유저 인증화면 연결

int OF0_AS;		//Option_Function.AutoStop = (sData[6]>>4)&0x03;
int OF0_AD;		//Option_Function.AutoDecel = (sData[1])&0x03;
int OF0_AC;		//Option_Function.AutoCamera = (sData[0]>>2)&0x03;
int OF0_RF;		//Option_Function.Rotate_Flag = (sData[1]>>2)&0x03;
int OF1_AS;
int OF1_AD;
int OF1_AC;
int OF1_RF;
int OF2_AS;
int OF2_AD;
int OF2_AC;
int OF2_RF;
int OF3_AS;
int OF3_AD;
int OF3_AC;
int OF3_RF;
int OF4_AS;
int OF4_AD;
int OF4_AC;
int OF4_RF;
int OF5_AS;
int OF5_AD;
int OF5_AC;
int OF5_RF;
int OF6_AS;
int OF6_AD;
int OF6_AC;
int OF6_RF;
int OF7_AS;
int OF7_AD;
int OF7_AC;
int OF7_RF;
int OF8_AS;
int OF8_AD;
int OF8_AC;
int OF8_RF;
int OF9_AS;
int OF9_AD;
int OF9_AC;
int OF9_RF;				
						

	int	APN;	// AUX1CurrentOutMode
	
	

	int DDTM; 	// DTM_DateTime_Mode=0;	// Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
	int BGC; 	// BG_Color;			// BG_BLUE:1 , BG_BLACK:2
	int LBD; 	// LCD_BL_Day
	int LBN; 	// LCD_BL_Night
	int SUSF; 	// SIUS_SEL_Flag;		// 0:SI , 1:US
	int HHT; 	// HMV.HourMeter_Trip 0~3
	int MDN;	// Main_DayNight;		// 0:Day , 1:Night
	
	
	

	int CC1_BR;		// 		CC_Brightness
	int CC1_CTR;	// 		CC_Contrast
	int CC1_SRT;	// 		CC_Saturation
	int CC1_HUE;	// 		CC_Hue
	int CC1_SP;		// 		CC_Sharpness
	int CC2_BR;		// 		CC_Brightness
	int CC2_CTR;	// 		CC_Contrast
	int CC2_SRT;	// 		CC_Saturation
	int CC2_HUE;	// 		CC_Hue
	int CC2_SP;		// 		CC_Sharpness
		
	int S7_Rg;		// 		S7_SelectRegion			
} Config;

/**
 * Global instance variable of configuration.
 */
extern Config theConfig;

/**
 * Loads configuration file.
 */
bool ConfigInit(void);

/**
 * Exits configuration.
 */
void ConfigExit(void);

/**
 * Updates CRC files.
 *
 * @param filepath The file path to update the CRC value. NULL for ini file on public drive.
 */
void ConfigUpdateCrc(char* filepath);

/**
 * Saves the public part of configuration to file.
 */
void ConfigSave(void);

/** @} */ // end of ctrlboard_config

/** @defgroup ctrlboard_external External
 *  @{
 */
#define EXTERNAL_BUFFER_SIZE 64 ///< External buffer size

typedef enum
{
    EXTERNAL_SHOW_MSG,  ///< Show message
    EXTERNAL_TEST0,     ///< Test #0
    EXTERNAL_TEST1,     ///< Test #1
    EXTERNAL_TEST2,     ///< Test #2
    EXTERNAL_TEST3,     ///< Test #3
    EXTERNAL_TEST4,     ///< Test #4
    EXTERNAL_TEST5      ///< Test #5
} ExternalEventType;

typedef struct
{
    ExternalEventType type;
    int arg1;
    int arg2;
    int arg3;
    uint8_t buf1[EXTERNAL_BUFFER_SIZE];

} ExternalEvent;

/**
 * Initializes external module.
 */
void ExternalInit(void);

/**
 * Exits external module.
 */
void ExternalExit(void);

/**
 * Receives external module event.
 *
 * @param ev The external event.
 * @return 0 for no event yet, otherwise for event occured.
 */
int ExternalReceive(ExternalEvent* ev);

/**
 * Sends external module event.
 *
 * @param ev The external event.
 * @return 0 for success, otherwise for failure.
 */
int ExternalSend(ExternalEvent* ev);

/**
 * Initializes external process module.
 */
void ExternalProcessInit(void);

/**
 * Processes external module event.
 *
 * @param ev The external event.
 * @return 0 for no event yet, otherwise for event occured.
 */
void ExternalProcessEvent(ExternalEvent* ev);

/** @} */ // end of ctrlboard_external

/** @defgroup ctrlboard upgarde_uart
 *  @{
 */
#if defined(CFG_UPGRADE_FROM_UART)
#if defined(CFG_UPGRADE_UART0)
#define UPGRADE_UART_PORT	ITP_DEVICE_UART0
#elif defined(CFG_UPGRADE_UART1)
#define UPGRADE_UART_PORT	ITP_DEVICE_UART1
#else
#define UPGRADE_UART_PORT	ITP_DEVICE_UART0
#endif
#define UPGRADE_PATTERN				0x1A

#define ACK20						0x14
#define ACK50						0x32
#define ACK100						0x64
#define ACK150						0x96
#define ACK200						0xC8
#define ACK210						0xD2
#define ACK211						0xD3
#define ACK220						0xDC
#define ACK221						0xDD

//the total check times is CHECK_NUM or CHECK_NUM+1
#define CHECK_NUM			4		
#define RETRY_SIZE			5
#define RETRY_CHECKSUM		1
#define RETRY_DATA			1

/**
 * Initializes Upgrade Fw by Uart module.
 */
void UpgradeUartInit(void);

/**
 * Exits Upgrade Fw by Uart module.
 */
void UpgradeUartExit(void);
#endif
/** @} */ // end of ctrlboard upgarde_uart

/** @defgroup ctrlboard_network Network
 *  @{
 */
/**
 * Initializes network module.
 */
void NetworkInit(void);
void NetworkWifiModeSwitch(void);

/**
 * Network pre-setting.
 */
void NetworkPreSetting(void);
void NetworkWifiPreSetting(void);

/**
 * Network function process.
 */
void NetworkEthernetProcess(void);
void NetworkWifiProcess(void);

/**
 * Exits network module.
 */
void NetworkExit(void);
bool NetworkIsExit(void);

/**
 * Resets network module.
 */
void NetworkReset(void);

/**
 * Determines whether the network(Ethernet) is ready or not.
 *
 * @return true for ready; false for net ready yet.
 */
bool NetworkIsReady(void);

/**
 * Determines whether the network(Ethernet) and Center Server are both ready or not.
 *
 * @return true for ready; false for net ready yet.
 */
bool NetworkServerIsReady(void);

/**
 * Determines whether the network(WIFI) is ready or not.
 *
 * @return true for ready; false for net ready yet.
 */
bool NetworkWifiIsReady(void);

/** @} */ // end of ctrlboard_network

/** @defgroup ctrlboard_photo Photo Loader
 *  @{
 */

typedef void (*PhotoLoadCallback)(uint8_t* data, int size);

void PhotoInit(void);

void PhotoExit(void);

int PhotoLoad(char* filename, PhotoLoadCallback func);

/** @} */ // end of ctrlboard_photo

/** @defgroup ctrlboard_screen Screen
 *  @{
 */
/**
 * Screensaver type definition.
 */
typedef enum
{
    SCREENSAVER_NONE,   ///< No screensaver
    SCREENSAVER_CLOCK,  ///< Clock sreensaver
    SCREENSAVER_BLANK,  ///< Black screen screensaver
    SCREENSAVER_PHOTO   ///< Photo screensaver
} ScreensaverType;

/**
 * Initializes screen module.
 */
void ScreenInit(void);

/**
 * Turns off screen.
 */
void ScreenOff(void);

/**
 * Turns on screen.
 */
void ScreenOn(void);

/**
 * Is the screen off or on.
 *
 * @return true for off; false for on.
 */
bool ScreenIsOff(void);

/**
 * Sets the brightness.
 *
 * @param value The brightness value.
 */
void ScreenSetBrightness(int value);
bool ScreenSetOnOff(bool OnOff);
/**
 * Gets the maximum brightness level.
 *
 * @return the maximum brightness level.
 */
int ScreenGetMaxBrightness(void);

/**
 * Re-counts the time to enter screensaver.
 */
void ScreenSaverRefresh(void);

/**
 * Checks whether it is about time to enter screensaver mode.
 *
 * @return 0 for not yet, otherwise for entering screensaver mode currently.
 */
int ScreenSaverCheck(void);

/**
 * Is on screensaver mode or not.
 */
bool ScreenSaverIsScreenSaving(void);

/**
 * Takes a screenshot to USB drive.
 *
 * @param lcdSurf The LCD surface widget.
 */
void Screenshot(void* lcdSurf);

/**
 * Clears screen.
 *
 */
void ScreenClear(void);

/** @} */ // end of ctrlboard_screen

/** @defgroup ctrlboard_storage Storage
 *  @{
 */

typedef enum
{
    STORAGE_NONE = -1,
    STORAGE_USB,
    STORAGE_SD,
    STORAGE_INTERNAL,

    STORAGE_MAX_COUNT
} StorageType;

typedef enum
{
    STORAGE_UNKNOWN,
    STORAGE_SD_INSERTED,
    STORAGE_SD_REMOVED,
    STORAGE_USB_INSERTED,
    STORAGE_USB_REMOVED,
    STORAGE_USB_DEVICE_INSERTED,
    STORAGE_USB_DEVICE_REMOVED
} StorageAction;

/**
 * Initializes storage module.
 */
void StorageInit(void);

StorageAction StorageCheck(void);
StorageType StorageGetCurrType(void);
void StorageSetCurrType(StorageType type);
char* StorageGetDrivePath(StorageType type);
bool StorageIsInUsbDeviceMode(void);

/** @} */ // end of ctrlboard_storage

/** @defgroup ctrlboard_string String
 *  @{
 */
/**
 * Guard sensors definition.
 */
typedef enum
{
    GUARD_EMERGENCY,    ///< Emergency sensor
    GUARD_INFRARED,     ///< Infrared sensor
    GUARD_DOOR,         ///< Door sensor
    GUARD_WINDOW,       ///< Window sensor
    GUARD_SMOKE,        ///< Smoke sensor
    GUARD_GAS,          ///< Gas sensor
    GUARD_AREA,         ///< Area sensor
    GUARD_ROB,          ///< Rob sensor

    GUARD_SENSOR_COUNT  ///< Total sensor count
} GuardSensor;

/**
 * Gets the description of guard sensor.
 *
 * @param sensor The guard sensor.
 * @return the string of guard sensor.
 */
const char* StringGetGuardSensor(GuardSensor sensor);

/**
 * Gets the description of WiFi connected.
 *
 * @return the string of WiFi connected.
 */
const char* StringGetWiFiConnected(void);

/** @} */ // end of ctrlboard_string

/** @defgroup ctrlboard_upgrade Upgrade
 *  @{
 */
/**
 * Quit value definition.
 */
typedef enum
{
    QUIT_NONE,                  ///< Do not quit
    QUIT_DEFAULT,               ///< Quit for nothing
    QUIT_RESET_FACTORY,         ///< Quit to reset to factory setting
    QUIT_UPGRADE_FIRMWARE,      ///< Quit to upgrade firmware
    QUIT_UPGRADE_WEB,           ///< Quit to wait web upgrading
    QUIT_RESET_NETWORK,         ///< Quit to reset network
    QUIT_UPGRADE_UART			///< Quit to upgrade firmware by Uart
} QuitValue;

/**
 * Initializes upgrade module.
 *
 * @return 0 for initializing success, non-zero for initializing failed and the value will be the QuitValue.
 */
int UpgradeInit(void);

/**
 * Sets the CRC value of the specified file path.
 *
 * @param filepath The file path to update the CRC value.
 */
void UpgradeSetFileCrc(char* filepath);

/**
 * Sets the URL to upgrade.
 *
 * @param url The url to download and upgrade.
 */
void UpgradeSetUrl(char* url);

/**
 * Sets the stream to upgrade.
 *
 * @param stream The stream to upgrade.
 */
void UpgradeSetStream(void* stream);

/**
 * Processes the upgrade procedure by QuitValue.
 *
 * @param code The QuitValue.
 * @return 0 for process success; otherwise failed.
 */
int UpgradeProcess(int code);

/**
 * Is upgrading ready or not.
 *
 * @return true for ready; otherwise not ready yet.
 */
bool UpgradeIsReady(void);

/**
 * Is upgrading finished or not.
 *
 * @return true for finished; otherwise not finish yet.
 */
bool UpgradeIsFinished(void);

/**
 * Gets the upgrading result.
 *
 * @return 0 for success, failed otherwise.
 */
int UpgradeGetResult(void);

/** @} */ // end of ctrlboard_upgrade

/** @defgroup ctrlboard_webserver Web Server
 *  @{
 */
/**
 * Initializes web server module.
 */
void WebServerInit(void);

/**
 * Exits web server module.
 */
void WebServerExit(void);

/** @} */ // end of ctrlboard_webserver

void PlayVideo(int x, int y, int width, int height, int bgColor, int volume);
void WaitPlayVideoFinish(void);

void PlayMjpeg(int x, int y, int width, int height, int bgColor, int volume);
void WaitPlayMjpegFinish(void);

#ifdef	CFG_DYNAMIC_LOAD_TP_MODULE
void DynamicLoadTpModule(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CTRLBOARD_H */
/** @} */ // end of ctrlboard