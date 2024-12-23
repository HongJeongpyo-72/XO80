// Menu Grobal Variable

#define Menu_Top			0
#define Menu_Trip			1
#define Menu_AUX			2
#define Menu_LCD			3
#define Menu_Data			4
#define Menu_Time			5
#define Menu_Preset			6
#define Menu_ErCode			7
#define Menu_UserDefine		8

uint8_t Menu_Index = Menu_Top;				// menu index 0; menu top

//#define BR_None				0
//#define BR_2Sub				1
//#define BR_2Main				2
//#define BR_2Top				3
//#define BR_2UpMenu			4

//uint8_t Menu_BranchTriger = 0;					// menu branch control triger
uint16_t Menu_JOGWheelTimer = 0;

uint8_t MenuTop_Choice_Index = 1;			// default 1 : trip menu
uint8_t MenuTrip_Choice_Index = 1;		// default 1 : trip 1 value

#define mLCD_DayBright		1
#define mLCD_NightBright	2
#define mLCD_BGMode			3
#define mLCD_UnitMode		4

uint8_t MenuLCD_Choice_Index = mLCD_DayBright;			// default 1 : brightness day control

#define mDate_ValueSet		1
#define mDate_TypeSet		2

uint8_t MenuDate_Choice_Index = 1;

#define mPreset_1st				1
#define mPreset_2nd				2
#define mPreset_Lang			3
#define mPreset_Opt				4
#define mPreset_Radio			5
#define mPreset_User			6
#define mPreset_Greese			7
#define mPreset_AutoPressure	8

#define mPreset_lock	        12   //chpark.24.09.12
#define mPreset_password	    13  //chpark.24.09.12

//uint8_t MenuPreset_Choice_Index = mPreset_1st; //chpark.24.09.24

