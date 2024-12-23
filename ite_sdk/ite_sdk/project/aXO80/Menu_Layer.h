// Menu Grobal Variable

#define Menu_Top					0
#define Menu_Trip					1
#define Menu_AUX					2
#define Menu_LCD					3
#define Menu_Data					4
#define Menu_Time					5
#define Menu_Preset					6
#define Menu_ErCode					7
#define Menu_UserDefine				8

extern uint8_t Menu_Index;				// menu index 0; menu top

//#define BR_None					0
//#define BR_2Sub					1
//#define BR_2Main					2
//#define BR_2Top					3
//#define BR_2UpMenu				4

//extern uint8_t Menu_BranchTriger;					// menu branch control triger
extern uint16_t Menu_JOGWheelTimer;

extern uint8_t MenuTop_Choice_Index;			// default 1 ; trip menu
extern uint8_t MenuTrip_Choice_Index;			// default 1 : trip 1 value

#define mLCD_DayBright		1
#define mLCD_NightBright	2
#define mLCD_BGMode			3
#define mLCD_UnitMode		4

extern uint8_t MenuLCD_Choice_Index;			// default 1 : brightness day control

#define mDate_ValueSet		1
#define mDate_TypeSet		2

extern uint8_t MenuDate_Choice_Index;

#define mPreset_1st				1
#define mPreset_2nd				2
#define mPreset_Lang			3
#define mPreset_Opt				4
#define mPreset_Radio			5
#define mPreset_User			6
#define mPreset_Greese			7
#define mPreset_AutoPressure	8
#define mPreset_lock	        12   //chpark.24.09.12
#define mPreset_password	    13   //chpark.24.09.12

uint8_t MenuPreset_Choice_Index;

extern uint8_t M7_Group;				// 0 ~ 2 ; 3set
extern UserPresetData M7_UserSet[3];	// JPHong Menu V13 2024-04-17
extern UserAUX24Data M7_UserAUX24Set;	//


extern uint8_t S1_AUX1_FlowRate_Max;			// CAN[AUX_U/L] 0byte
extern uint8_t S1_AUX1_FlowRate_Min;			// CAN[AUX_U/L] 1byte
extern uint8_t S1_AUX1_Pressure_Max;			// CAN[AUX_PU/L] 0byte
extern uint8_t S1_AUX1_Pressure_Min;			// CAN[AUX_PU/L] 1byte

extern uint8_t S2_AUX2_FlowRate_Max;		// CAN[AUX_U/L] 2byte
extern uint8_t S2_AUX2_FlowRate_Min;		// CAN[AUX_U/L] 3byte
extern uint8_t S2_AUX2_Pressure_Max;		// CAN[AUX_PU/L] 2byte
extern uint8_t S2_AUX2_Pressure_Min;		// CAN[AUX_PU/L] 3byte

extern uint8_t S2_AUX4_FlowRate_Max;		// CAN[AUX_U/L] 4byte
extern uint8_t S2_AUX4_FlowRate_Min;		// CAN[AUX_U/L] 5byte
extern uint8_t S2_AUX4_Pressure_Max;		// CAN[AUX_PU/L] 4byte
extern uint8_t S2_AUX4_Pressure_Min;		// CAN[AUX_PU/L] 5byte

extern uint8_t Radio_Region;
extern uint8_t S7_SelectRegion;		// jhlee 0608
extern uint8_t S7_ChoiceRegion;		// add jhlee 0630
extern uint8_t cRadio_PowerCondition;	//JPHong 0607
extern uint16_t Change_kpa2psi(uint16_t DateVal);
extern uint16_t Change_psi2kpa(uint16_t DataVal);

extern void GotoM7S1(uint8_t AUX_NO);

extern void AUX1_Init(void);
extern void AUX24_Init(void);

extern void AUX1_LimitCheck(void);
extern void AUX2_4_LimitCheck(void);

extern uint8_t Lang_Flag[MAX_SU100_USER_NUM];

extern UserFunc Option_Function;		// [user_index] //chpark.08.22

#define MaUserSelect_Mode		1
#define MaUserAdd_Mode			2
#define MaUserDelete_Mode		3
#define MaUserEdit_Mode			4
#define OpUserEdit_Mode			5

uint8_t S9_UserMode;

#define DEV_OPT			0

uint8_t M7_KeyMode_Choice_Index;    //chpark.24.09.24
uint8_t M7_LockMode_Choice_Index;   //chpark.24.09.24