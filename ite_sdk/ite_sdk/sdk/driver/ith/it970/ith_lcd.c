/*
 * Copyright (c) 2016 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * HAL LCD functions.
 *
 * @author Irene Wang
 * @version 1.0
 */
#include <unistd.h>
#include "../ith_cfg.h"
#include "ith_lcd.h"

#ifdef CFG_LCD_PQ_TUNING
#include "isp/mmp_isp.h"
#endif

#define CMD_DELAY   0xFFFFFFFF
#define CLAMP(x, low, high)      ( ((x) > (high)) ? (high) : ((x) < (low)) ? (low) : (x) )

static const uint32_t* lcdScript;
static unsigned int lcdScriptCount, lcdScriptIndex;
static uint32_t mipiHSTable[16];
static uint32_t mipiHSPLLFrange[8];
static uint32_t pinShareTable[8];
static uint32_t mipiDPHYValue = 0;

static int gHue = 0;
static int gContrast = 0;
static int gBrightness = 0;

#ifdef CFG_LCD_PQ_TUNING
static ISP_DEVICE gIspDev;
static MMP_ISP_CORE_INFO coreInfo;
static MMP_ISP_SHARE inputInfo;
static MMP_ISP_OUTPUT_INFO outInfo;
#endif

///////////////////////////////////////////////////////////
/// Color Conversion Algorithm
///////////////////////////////////////////////////////////

/** (Local Function) This is matrix multiplication of size (3x3)
*
*  @param M_in1 (3x3 matrix, input):  This is input matrix for left-hand side in matrix multiplication.
*  @param M_in2 (3x3 matrix, input):  This is input matrix for right-hand side in matrix multiplication.
*  @param M_out (3x3 matrix, output): This stores result after matrix multiplication.
*/
static void
_matrix_multiply_33(
    float M_in1[3][3],
    float M_in2[3][3],
    float M_out[3][3])
{
    int16_t i, j, k;

    for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
    {
        M_out[i][j] = 0;
        for (k = 0; k < 3; k++)
            M_out[i][j] += M_in1[i][k] * M_in2[k][j];
    }
}

/** This is to achieve Hue rotation by using single 3x3 matrix multiplication.
*
* NOTE: There is no color model conversion, only hue rotation.
*
* @param angle [unit: degree]: Hue rotation angle.
*                              If positive, red is turning into green. Vise-versa.
*
* @return 3x4 Matrix coefficients to put into hardware, where the last column is delta/constant.
*          In `EV_MATRIX`, `M_color_correction` is 3x3 matrix at the left of 3x4 matrix;
*          `V_color_correction` is the last column in 3x4 matrix.
*/
static EV_MATRIX _color_conversion(signed short angle)
{
    float a, b;
    float M_hsv_angle[3][3] = { { 1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
    float M_ycrb2rgb[3][3];
    float M_rgb2ycrb[3][3];

    float M_temp1[3][3];
    EV_MATRIX CC_MatrixCurrect;

    /// Get hue rotation matrix (in YCrCb domain)
    a = (float)cos(angle / 180.0 * 3.1516159);
    b = (float)-sin(angle / 180.0 * 3.1516159);

    M_hsv_angle[1][1] = a;
    M_hsv_angle[1][2] = -b;
    M_hsv_angle[2][1] = b;
    M_hsv_angle[2][2] = a;

    /// Color conversion from/to YCrCb to/from RGB
    {
        M_ycrb2rgb[0][0] = 1.0f;
        M_ycrb2rgb[0][1] = 1.402f;
        M_ycrb2rgb[0][2] = 0.0f;
        M_ycrb2rgb[1][0] = 1.0f;
        M_ycrb2rgb[1][1] = -0.714136f;
        M_ycrb2rgb[1][2] = -0.344136f;
        M_ycrb2rgb[2][0] = 1.0f;
        M_ycrb2rgb[2][1] = 0.0f;
        M_ycrb2rgb[2][2] = 1.772f;

        M_rgb2ycrb[0][0] = 0.299f;
        M_rgb2ycrb[0][1] = 0.587f;
        M_rgb2ycrb[0][2] = 0.114f;
        M_rgb2ycrb[1][0] = 0.5f;
        M_rgb2ycrb[1][1] = -0.418688f;
        M_rgb2ycrb[1][2] = -0.081312f;
        M_rgb2ycrb[2][0] = -0.168736f;
        M_rgb2ycrb[2][1] = -0.331264f;
        M_rgb2ycrb[2][2] = 0.5f;
    }

    /// Get final R/G/B color conversion matrix
    _matrix_multiply_33(M_ycrb2rgb, M_hsv_angle, M_temp1);
    _matrix_multiply_33(M_temp1, M_rgb2ycrb, CC_MatrixCurrect.M_color_correction);

    /// Ensure that delta is 0
    for (int i = 0; i < 3; i++)
    {
        CC_MatrixCurrect.V_color_correction[i][0] = 0;
    }

    return CC_MatrixCurrect;
}

#define POW2_INT(i)       ((int) pow(2,i))
/** This is to convert floating point number (double) into fix point representation using 2's complement notation.
*      Output format is `SIGN_BIT`:`INT_D`:`FRAC_D`
*
*  @param value:    This value is to be converted to fix-point representation.
*  @param SIGN_BIT [valid value - 0/1]: How many sign bit does the value contains. Now only 1 sign bit is supported.
*  @param INT_D:    Number of integer bits.
*  @param FRAC_D:   Number of fractional bits.
*/
static inline uint16_t _double2fix_2s_comp(
    double value,
    int SIGN_BIT,
    int INT_D,
    int FRAC_D)
{
    int value_uint = (int)(fabs(value) * pow(2.0f, FRAC_D) + 0.5f);            ///Turn to fix point (with rounding)
    uint16_t value_uint16 = CLAMP(value_uint, 0, POW2_INT(FRAC_D + INT_D) - 1);   ///Clamp to allowed value range
    uint16_t value_masked;

    /// Start Sign bit process (sign-magnitude)
    if (SIGN_BIT > 0)
    {
        if (value < 0)
            value_uint16 = (~value_uint16) + 1;
    }
    else if (value < 0)
        value_uint16 = 0;
    /// End Sign bit process (sign-magnitude)

    value_masked = value_uint16 & (POW2_INT(FRAC_D + INT_D + SIGN_BIT) - 1);

    return value_masked;
}

/** This is to check if parameter `value` is within valid range.
*
*  @param value: Input value to check if it is valid or not.
*  @param row: Row inside 3x4 matrix.
*  @param column: Row inside 3x4 matrix.
*  @param SIGN_BIT [valid value - 0/1]: How many sign bit does the value contains. Now only 1 sign bit is supported.
*  @param INT_D:    Number of integer bits.
*  @param FRAC_D:   Number of fractional bits.
*/
static inline void _check_range(
    double value, int row, int column,
    int SIGN_BIT,
    int INT_D,
    int FRAC_D)
{
    if (fabs(value) >= POW2_INT(INT_D))
    {
        printf("[ERROR] Coefficient at [%d,%d] is larger than allowed range, with value %f\n", row, column, value);
        //exit(130);
    }
}
#undef POW2_INT

static uint16_t _double_to_s1_8(double value_in)
{
    return _double2fix_2s_comp(value_in, 1, 1, 8);
}

static void _check_range_s1_8(double value, int row, int column)
{
    _check_range(value, row, column, 1, 1, 8);
}

static uint16_t _double_to_s8_0(double value_in)
{
    return _double2fix_2s_comp(value_in, 1, 8, 0);
}

static void _check_range_s8_0(double value, int row, int column)
{
    _check_range(value, row, column, 1, 8, 0);
}

/** This function is to write 3x4 color conversion coefficients into LCD's color conversion register.
*
*  ------------------------
*  Input/Output Arguments
*  ------------------------
*
*  @param out_cfg_path [const char*]: File to write register-write commands to.
*  @param color_conv_mat [`EV_MATRIX`]: This is the 3x4 color conversion matrix.
*/
static void _soc_write__color_conversion(EV_MATRIX color_conv_mat)
{
    //FILE* fp = fopen(out_cfg_path, "w");

    /// Write coefficients into registers
    for (int i = 0; i < 3; i++)
    {
        uint32_t reg_addr_base = ITH_LCD_BASE + ITH_LCD_RGB2YUV11_REG + i * 8; //0xD0000054
        uint32_t reg_value;

        _check_range_s1_8(color_conv_mat.M_color_correction[i][0], i, 0);
        _check_range_s1_8(color_conv_mat.M_color_correction[i][1], i, 1);
        _check_range_s1_8(color_conv_mat.M_color_correction[i][2], i, 2);
        _check_range_s8_0(color_conv_mat.V_color_correction[i][0], i, 3);

        reg_value = _double_to_s1_8(color_conv_mat.M_color_correction[i][0]) + (_double_to_s1_8(color_conv_mat.M_color_correction[i][1]) << 16);
        ithWriteRegA(reg_addr_base + 0, reg_value);
        //fprintf(fp, "WRITE(0x%08X, 0x%08X);\n", reg_addr_base + 0, reg_value);
        reg_value = _double_to_s1_8(color_conv_mat.M_color_correction[i][2]) + (_double_to_s8_0(color_conv_mat.V_color_correction[i][0]) << 16);
        ithWriteRegA(reg_addr_base + 4, reg_value);
        //fprintf(fp, "WRITE(0x%08X, 0x%08X);\n", reg_addr_base + 4, reg_value);
    }

    ///// Enable color conversion
    //fprintf(fp, "WRITE(0xD0000050, 0x00000001);\n");
    //fprintf(fp, "WRITE(0xD000001C, 0x80000003);\n");    /// This is due to the script is after LCD activating sequence.
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_RGB2YUV_REG, ITH_LCD_RGB2YUV_EN_BIT);
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG, ITH_LCD_LAYER1UPDATE_BIT);
    while (ithReadRegA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG) & (0x1 << ITH_LCD_LAYER1UPDATE_BIT))
    {
        ithDelay(1000);
    }
    //fclose(fp);
}

static int _color_conversion_main(int hue)
{
    EV_MATRIX color_conv_mat = _color_conversion(hue);

    _soc_write__color_conversion(color_conv_mat);

    return 0;
}

/** This function is to generate LUT for gamma function, to achieve
*  - Contrast (by setting variable `contrast` and `midpoint`)
*  - Brightness (by setting variable `brightness`)
*
* [Math equation: out_value = in_value + (in_value - `midpoint`) * `contrast` / 128.0f + `brigtness`]
*
* ------------------------
* Input/Output Arguments
* ------------------------
*
* @param contrast [-64 ~ 63]   : This is to set how much contrast is enhance or reduced.
* @param midpoint [0 ~ 255]    : This is to set mid-point for contrast.
* @param brightness [-64 ~ 63] : This is to set brightness (curve offset, but not exactly for now).
*/
static GAMMA_FUNC_CURVE _gamma_curve__contrast_brightness(int contrast, int midpoint, int brightness)
{
    float contrast_f = (float)contrast / 128.0f;
    float brightness_f = (float)brightness;

    GAMMA_FUNC_CURVE curve;           ///This is Y axis in LUT

    for (float idx = 0; idx < GAMMA_CURVE_LUT_SIZE; idx++)
    {
        float x = 256.0f / (GAMMA_CURVE_LUT_SIZE - 1) * idx;
        float y = x + (x - midpoint) * contrast_f + brightness_f;

        curve.x[(int)idx] = x;

        /// Since y is currently always > 0, directly add 0.5 to perform rounding.
        curve.y[(int)idx] = CLAMP(y, 0, 256 - 1);
    }

    return curve;
}

/** This function is to write gamma curve into LCD's gamma function register, where
*      the curve is used to tune gray-level intensity, such that only one curve is given.
*
*  ------------------------
*  Input/Output Arguments
*  ------------------------
*
* @param out_cfg_path [const char*]: File to write register-write commands to.
* @param curve_gray [struct gamma_func_curve]: Gamma Curve to write (one curve for all R/G/B tables).
* @param enable [bool]: If set False, Gamma Function will be disabled.
*/
static void _soc_write__LCD_gamma_intensity(GAMMA_FUNC_CURVE curve_gray, bool enable)
{
    //FILE *fp = fopen(out_cfg_path, "w");
    for (int channel = 0; channel < 3; channel++) ///Channel: R/G/B color
    {
        uint32_t reg_addr;
        uint32_t reg_value;

        /// R/G/B LUT write locations are starting from `0xD0000040` to `0xD0000048`
        reg_addr = ITH_LCD_BASE + ITH_LCD_GAMMA_R_PTR_REG + 4 * channel; //0xD0000040

        for (int idx = 0; idx < GAMMA_CURVE_LUT_SIZE; idx++)
        {
            /// `curve_x_int` is LUT's address in hardware
            int curve_x_int = idx;

            /// `curve_y_int` is LUT's value in given address, with format U8.2
            int curve_y_int = (int)(curve_gray.y[idx] * pow(2, 2) + 0.5f);

            if (!enable)
                reg_value = 0;                                /// Disable Gamma Function
            else
                reg_value = 1 << ITH_LCD_GAMMA_FUN_EN_BIT;    /// Enabel Gamma Function

            reg_value += (curve_y_int << 16) + curve_x_int;
            ithWriteRegA(reg_addr, reg_value);
            //fprintf(fp, "WRITE(0x%X, 0x%X);\n", reg_addr, reg_value);
        }
        ithWriteRegA(reg_addr, reg_value);
        //fprintf(fp, "WRITE(0x%X, 0x%X);\n", reg_addr, reg_value);
    }

    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG, ITH_LCD_LAYER1UPDATE_BIT);
    while (ithReadRegA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG) & (0x1 << ITH_LCD_LAYER1UPDATE_BIT))
    {
        ithDelay(1000);
    }
    //fclose(fp);
}

static int _gamma_function_api(int contrast, int brightness)
{
    GAMMA_FUNC_CURVE curve = _gamma_curve__contrast_brightness(contrast, 128, brightness);

    /// Write Register-Write command file
    {
        bool enable_gamma = true;

        /// Disable Gamma Function if contrast==brightness==0
        if (contrast == 0 && brightness == 0)
            enable_gamma = false;
        else
            enable_gamma = true;

        _soc_write__LCD_gamma_intensity(curve, enable_gamma);
    }

    return 0;
}


void ithLcdReset(void)
{

printf("\n\n\n\n\n\n");
printf("================== ithLcdReset\n");
printf("================== ithLcdReset\n");
printf("================== ithLcdReset\n");
printf("================== ithLcdReset\n");
printf("================== ithLcdReset\n");
printf("\n\n\n\n\n\n");


    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0xFFFFFFFF, (0x1 << ITH_LCD_REG_RST_BIT) | (0x1 << ITH_LCD_RST_BIT));
    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0x0, (0x1 << ITH_LCD_REG_RST_BIT) | (0x1 << ITH_LCD_RST_BIT));
}

void ithLcdEnable(void)
{

printf("\n\n\n\n\n\n");
printf("================== ithLcdEnable\n");
printf("================== ithLcdEnable\n");
printf("================== ithLcdEnable\n");
printf("================== ithLcdEnable\n");
printf("================== ithLcdEnable\n");
printf("\n\n\n\n\n\n");
    // enable clock
    //if (ithLcdGetPanelType() != ITH_LCD_RGB)
    //    ithSetRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_DCLK_BIT);
    ithSetRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_M3CLK_BIT);
    ithSetRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_W12CLK_BIT);

    if (ithLcdGetPanelType() == ITH_LCD_MIPI)
    {
        ithWriteRegA(0xD800004C, 0x0002c001);  //MIPI controller enable
        usleep(1);
        ithWriteRegA(0xD8000048, 0x8002c003);  //DPHY Enable
        usleep(100);                           //100us
        ithWriteRegA(0xD8000044, 0x400ac081);  //DPHY PORn rst normal
        usleep(75);                            //75us
        ithWriteRegA(0xD8000044, (mipiDPHYValue & 0x1F000000) | 0x000ac081);  //DPHY ResetN rst normal
        usleep(200);                           //200us

        ithLcdResetMipiHSInfo();

        //LCD
        ithWriteRegA(0xD0000070, 0x00000307);    // ctg_reset_off
        usleep(1);
    }

    // disable LCD pin input mode
    if (ithLcdGetPanelType() == ITH_LCD_RGB)
    {
        ithLcdResetPinShareInfo();
    }

    //set reg 0x0020 as 0x81000000 for test mode
    // wait for 0x81000000 become 0x01000000
    ithWriteRegA(ITH_LCD_BASE + ITH_LCD_TEST_COLOR_SET_REG, 0x80000000);
    while (ithReadRegA(ITH_LCD_BASE + ITH_LCD_TEST_COLOR_SET_REG) & 0x80000000)
    {
        ithDelay(1000);
    }
}

void ithLcdDisable(void)
{
    //set reg 0x0020 as 0x81000000 for test mode
    // wait for 0x81000000 become 0x01000000
    if ((ithReadRegA(ITH_LCD_BASE + ITH_LCD_TEST_COLOR_SET_REG) & 0x01000000) == 0x0)
	{
        ithWriteRegA(ITH_LCD_BASE + ITH_LCD_TEST_COLOR_SET_REG, 0x81000000);
        while (ithReadRegA(ITH_LCD_BASE + ITH_LCD_TEST_COLOR_SET_REG) & 0x80000000)
        {
            ithDelay(1000);
        }
    }
	
    if (ithLcdGetPanelType() == ITH_LCD_MIPI)
    {
        ithLcdSaveMipiHSInfo();
        mipiDPHYValue = ithReadRegA(0xD8000044);

        //disable MIPI CLK
        ithWriteRegA(0xD8000048, 0x0002c003);    //[31]=0 DPHY disable, KESCCLK
        ithWriteRegA(0xD8000044, 0x800ac081);    //[31]=1 DPHY PORn, KCLK
        usleep(1);                          // > 500ns
        ithWriteRegA(0xD8000044, 0xC00ac081);    //[30]=1 DPHY ResetN, KCLK
        usleep(1);                          // > 500ns
        ithWriteRegA(0xD800004C, 0xC002c001);    //[31]:Reset MIPI power controller,[30]:Reset MIPI system controller,  KDSICLK

        //LCD
        ithWriteRegA(0xD0000070, 0x00010307);    // ctg_reset_on
        usleep(1);
    }


    // set LCD pin input mode
    if (ithLcdGetPanelType() == ITH_LCD_RGB)
    {
        ithLcdSavePinShareInfo();
        ithWriteRegA(0xD10000E4, 0);
        ithWriteRegA(0xD10000E8, 0);
        ithWriteRegA(0xD10000EC, 0);
        ithWriteRegA(0xD1000160, 0);
        ithWriteRegA(0xD1000164, 0);
        ithWriteRegA(0xD1000168, 0);
        ithWriteRegA(0xD100016C, 0);
    }

    // disable clock
    ithClearRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_M3CLK_BIT);
    ithClearRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_W12CLK_BIT);
    //if (ithLcdGetPanelType() != ITH_LCD_RGB)
    //    ithClearRegBitA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, ITH_EN_DCLK_BIT);
}

void ithLcdSetBaseAddrA(uint32_t addr)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_BASEA_REG, addr, ITH_LCD_BASEA_MASK);
}

void ithLcdSetBaseAddrB(uint32_t addr)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_BASEB_REG, addr, ITH_LCD_BASEB_MASK);
}

uint32_t ithLcdGetBaseAddrB(void)
{
    return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_BASEB_REG) & ITH_LCD_BASEB_MASK);
}

void ithLcdSetBaseAddrC(uint32_t addr)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_BASEC_REG, addr, ITH_LCD_BASEC_MASK);
}

uint32_t ithLcdGetBaseAddrC(void)
{
    return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_BASEC_REG) & ITH_LCD_BASEC_MASK);
}

void ithLcdEnableHwFlip(void)
{
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_SET1_REG, ITH_LCD_HW_FLIP_BIT);
}

void ithLcdDisableHwFlip(void)
{
    ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_SET1_REG, ITH_LCD_HW_FLIP_BIT);
}

void ithLcdEnableVideoFlip(void)
{
    ithLcdDisableHwFlip();
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_SET1_REG, ITH_LCD_VIDEO_FLIP_EN_BIT);
}

void ithLcdDisableVideoFlip(void)
{
    ithLcdDisableHwFlip();
    ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_SET1_REG, ITH_LCD_VIDEO_FLIP_EN_BIT);
}

// 5-6
#if 0	// OK
	#define LCD_SCRIPT_DELAY1	100
	#define LCD_SCRIPT_DELAY2	1000
	#define LCD_SCRIPT_DELAY_1	100
#elif 0	// NG
	#define LCD_SCRIPT_DELAY1	100
	#define LCD_SCRIPT_DELAY2	0
	#define LCD_SCRIPT_DELAY_1	100 안켜짐
#elif 0	// NG
	#define LCD_SCRIPT_DELAY1	100	// 안켜짐 
	#define LCD_SCRIPT_DELAY2	0
	#define LCD_SCRIPT_DELAY_1	0	//
#elif 0 // 3-1-10 ITS NG
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	0
	#define LCD_SCRIPT_DELAY_1	100
#elif 1 // 3-1-10 ITS NG
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	0
	#define LCD_SCRIPT_DELAY_1	0
#elif 1 // 3-1-9 ITS ok
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	100
	#define LCD_SCRIPT_DELAY_1	0
#elif 0 // 3-1-9 ITS .ok
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	500
	#define LCD_SCRIPT_DELAY_1	0
#elif 1 // 3-1-8 ITS  ok
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	1000
	#define LCD_SCRIPT_DELAY_1	0
#elif 0	// NG
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	10
	#define LCD_SCRIPT_DELAY_1	0
#elif 0	// NG
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	100
	#define LCD_SCRIPT_DELAY_1	0
#elif 0	//
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	100
	#define LCD_SCRIPT_DELAY_1	0
#elif 1	// 3-1-8 ITS NG 
	#define LCD_SCRIPT_DELAY1	0
	#define LCD_SCRIPT_DELAY2	0
	#define LCD_SCRIPT_DELAY_1	0
#endif


#if (LCD_MODULE_SEL==ITS_LCD)


void ithLcdLoadScriptFirst(const uint32_t* script, unsigned int count)
{
    unsigned int i;

    lcdScript = script;
    lcdScriptCount = count;

	printf("\n\n\n\n\n\n");
	printf(" ITS_LCD : ================== ITS_LCD ithLcdLoadScriptFirst : %d\n",count);
	
	
    // Run script until fire
    for (i = 0; i < count; i += 2)
    {
        unsigned int reg    = script[i];
        unsigned int val    = script[i + 1];

        if (reg == CMD_DELAY)
            ithDelay(val);
        else
		{
			if ((reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG)) && (val == 0xFF))
			{
				printf("reg[0x%08X],val[0x%08X]\n",reg,val);
				break;
			}
			else
			{
	            ithWriteRegA(reg, val);
				if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG) && (val & ITH_LCD_SYNCFIRE_MASK))
					break;
			}
		}
    }
    printf(" ITS_LCD : ================== ithLcdLoadScriptFirst : reg[0x%08X],val[0x%08X]\n", lcdScript[i],lcdScript[i + 1]);
    lcdScriptIndex = i;
    printf(" ITS_LCD : ================== ithLcdLoadScriptFirst : %d,%d\n", lcdScriptIndex,i);
    printf(" ITS_LCD : ================== ithLcdLoadScriptFirst : reg[0x%08X],val[0x%08X]\n", lcdScript[lcdScriptIndex],lcdScript[lcdScriptIndex + 1]);
    printf("\n\n\n\n\n\n");
}

void ithLcdLoadScriptNext(void)
{
    unsigned int i, width;
	printf("\n\n\n\n\n\n");
	printf("================== ITS_LCD : ithLcdLoadScriptNext :[%d] reg[0x%08X],val[0x%08X]\n",lcdScriptIndex, lcdScript[lcdScriptIndex],lcdScript[lcdScriptIndex + 1]);
    for (i = lcdScriptIndex; i < lcdScriptCount; i += 2)
    {
        unsigned int reg    = lcdScript[i];
        unsigned int val    = lcdScript[i + 1];
		if ((reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG)) && (val == 0xFF))
		{
			printf("Out reg[0x%08X],val[0x%08X]\n",reg,val);
		}
		else if (reg == CMD_DELAY)
		{
            ithDelay(val);
            printf("ithDelay[%d]\n",val);
        }
        else
        {
            ithWriteRegA(reg, val);
            printf("reg[0x%08X],val[0x%08X]\n",reg,val);
        }
    }

    //check LCD width alignment
    width = ithLcdGetWidth();
    if (width % 4)
    {
        width = ((width >> 2) << 2) + 4;
        ithLcdSetPitch(width * 2);
        printf("check alignment ithLcdGetPitch():%d\n", ithLcdGetPitch());
    }
    printf("\n\n\n\n\n\n");
}



#else

#if 1 // ORG

void ithLcdLoadScriptFirst(const uint32_t* script, unsigned int count)
{
    unsigned int i;

    lcdScript = script;
    lcdScriptCount = count;

	printf("\n\n\n\n\n\n");
	printf("================== ORG : ithLcdLoadScriptFirst : %d\n",count);
	
	
    // Run script until fire
    for (i = 0; i < count; i += 2)
    {
        unsigned int reg    = script[i];
        unsigned int val    = script[i + 1];

        if (reg == CMD_DELAY)
            ithDelay(val);
        else
		{
			if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG) && (val == 0xFF))
			{
				printf("1reg[0x%08X],val[0x%08X]\n",reg,val);
			}
			else
			{
	            ithWriteRegA(reg, val);
				if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG) && (val & ITH_LCD_SYNCFIRE_MASK))
					break;
			}
		}
    }
    lcdScriptIndex = i;
     printf("================== ORG : ithLcdLoadScriptNext : reg[0x%08X],val[0x%08X]\\n", lcdScript[lcdScriptIndex],lcdScript[lcdScriptIndex + 1]);
    printf("\n\n\n\n\n\n");
}

void ithLcdLoadScriptNext(void)
{
    unsigned int i, width;
    
    printf("================== ORG : ithLcdLoadScriptNext : reg[0x%08X],val[0x%08X]\\n", lcdScript[i],lcdScript[i + 1]);
    
    for (i = lcdScriptIndex; i < lcdScriptCount; i += 2)
    {
        unsigned int reg    = lcdScript[i];
        unsigned int val    = lcdScript[i + 1];

        if (reg != CMD_DELAY)
            ithWriteRegA(reg, val);
    }

    //check LCD width alignment
    width = ithLcdGetWidth();
    if (width % 4)
    {
        width = ((width >> 2) << 2) + 4;
        ithLcdSetPitch(width * 2);
        printf("check alignment ithLcdGetPitch():%d\n", ithLcdGetPitch());
    }
}

#else

void ithLcdLoadScriptFirst(const uint32_t* script, unsigned int count)
{
    unsigned int i;

	printf("\n\n\n\n\n\n");
	printf("================== ithLcdLoadScriptFirst [%d]\n",LCD_SCRIPT_DELAY1);
	printf("================== ithLcdLoadScriptFirst [%d]\n",LCD_SCRIPT_DELAY2);
	printf("================== ithLcdLoadScriptFirst [%d]\n",LCD_SCRIPT_DELAY_1);
	printf("================== ithLcdLoadScriptFirst\n");
	printf("================== ithLcdLoadScriptFirst\n");
	
	
	
	lcdScript = script;
	lcdScriptCount = count;
	printf("lcdScript[%d],lcdScriptCount[%d]\n\n",lcdScript,lcdScriptCount);
	
	// Run script until fire
	for (i = 0; i < count; i += 2)
	{
		unsigned int reg    = script[i];
		unsigned int val    = script[i + 1];
		if (reg == CMD_DELAY)
		{
			if (val == 1)
				ithDelay(LCD_SCRIPT_DELAY_1);
			else
				ithDelay(LCD_SCRIPT_DELAY1);
			ithDelay(val);
		}     
		else
		{
			ithDelay(LCD_SCRIPT_DELAY2);
			if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG) && (val == 0xFF))
			{
				ithWriteRegA(reg, 0x00000000);
				printf("1reg[0x%08X],val[0x%08X]\n",reg,val);
			}
			else
			{		
				ithWriteRegA(reg, val);
				if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG) && (val & ITH_LCD_SYNCFIRE_MASK))
				{
					printf("2reg[0x%08X],val[0x%08X]\n",reg,val);
					break;
				}
			}
		}
	}
	lcdScriptIndex = i;
	
	printf("\n\n\n\n\n\n");
}

void ithLcdLoadScriptNext(void)
{
	unsigned int i, width;
	printf("\n\n\n\n\n\n");
	printf("================== ithLcdLoadScriptNext\n");
	printf("================== ithLcdLoadScriptNext\n");
	printf("================== ithLcdLoadScriptNext\n");
	printf("================== ithLcdLoadScriptNext\n");
	printf("================== ithLcdLoadScriptNext\n");
	
	printf("lcdScriptIndex[%d],lcdScriptCount[%d]\n\n",lcdScriptIndex,lcdScriptCount);
	
	#if 1
		for (i = lcdScriptIndex; i < lcdScriptCount; i += 2)
		{
			unsigned int reg    = lcdScript[i];
			unsigned int val    = lcdScript[i + 1];
			
			printf("eg[0x%08X],al[0x%08X]\n",reg,val);
			if (reg == CMD_DELAY)
			{
				if (val == 1)
					ithDelay(LCD_SCRIPT_DELAY_1);
				else
					ithDelay(LCD_SCRIPT_DELAY1);
				ithDelay(val);
			}
			else
			{
				ithDelay(LCD_SCRIPT_DELAY2);
				ithWriteRegA(reg, val);
			}
			if (reg == (ITH_LCD_BASE + ITH_LCD_UPDATE_REG))
			{
				printf("2reg[0x%08X],val[0x%08X]\n",reg,val);
			}
		}
	#endif
	//check LCD width alignment
	width = ithLcdGetWidth();
	if (width % 4)
	{
		width = ((width >> 2) << 2) + 4;
		ithLcdSetPitch(width * 2);
		printf("check alignment ithLcdGetPitch():%d\n", ithLcdGetPitch());
	}
	
	
	printf("\n\n\n\n\n\n");  
}
#endif

#endif
void ithLcdCursorSetBaseAddr(uint32_t addr)
{
    ithWriteRegA(ITH_LCD_BASE + ITH_LCD_HWC_BASE_REG, addr);
}

void ithLcdCursorSetColorWeight(ITHLcdCursorColor color, uint8_t value)
{
    switch (color)
    {
    case ITH_LCD_CURSOR_DEF_COLOR:
        ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_INVCOLORWEI_REG, value << ITH_LCD_HWC_INVCOLORWEI_BIT, ITH_LCD_HWC_INVCOLORWEI_MASK);
        break;

    case ITH_LCD_CURSOR_FG_COLOR:
        ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_FORECOLORWEI_REG, value << ITH_LCD_HWC_FORECOLORWEI_BIT, ITH_LCD_HWC_FORECOLORWEI_MASK);
        break;

    case ITH_LCD_CURSOR_BG_COLOR:
        ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_BACKCOLORWEI_REG, value << ITH_LCD_HWC_BACKCOLORWEI_BIT, ITH_LCD_HWC_BACKCOLORWEI_MASK);
        break;
    }
}

ITHLcdFormat ithLcdGetFormat(void)
{
    return (ITHLcdFormat)((ithReadRegA(ITH_LCD_BASE + ITH_LCD_SRCFMT_REG) & ITH_LCD_SRCFMT_MASK) >> ITH_LCD_SRCFMT_BIT);
}

void ithLcdSetWidth(uint32_t width)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_WIDTH_REG, width << ITH_LCD_WIDTH_BIT, ITH_LCD_WIDTH_MASK);
}


void ithLcdSetHeight(uint32_t height)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HEIGHT_REG, height << ITH_LCD_HEIGHT_BIT, ITH_LCD_HEIGHT_MASK);
}

void ithLcdSetPitch(uint32_t pitch)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_PITCH_REG, pitch << ITH_LCD_PITCH_BIT, ITH_LCD_PITCH_MASK);
}

unsigned int ithLcdGetXCounter(void)
{
    return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_CTGH_CNT_REG) & ITH_LCD_CTGH_CNT_MASK) >> ITH_LCD_CTGH_CNT_BIT;
}

unsigned int ithLcdGetYCounter(void)
{
    return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_READ_STATUS1_REG) & ITH_LCD_CTGV_CNT_MASK) >> ITH_LCD_CTGV_CNT_BIT;
}

void ithLcdSyncFire(void)
{
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG, ITH_LCD_SYNCFIRE_BIT);
}

 bool ithLcdIsSyncFired(void)
{
     return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG) & ITH_LCD_SYNCFIRE_MASK) ? true : false;
}

bool ithLcdIsEnabled(void)
{
    return ((ithReadRegA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG) & (ITH_LCD_DISPEN_MASK | ITH_LCD_SYNCFIRE_MASK)) == (ITH_LCD_DISPEN_MASK | ITH_LCD_SYNCFIRE_MASK)) ? true : false;
}

unsigned int ithLcdGetFlip(void)
{
    return (ithReadRegA(ITH_LCD_BASE + ITH_LCD_READ_STATUS1_REG) & ITH_LCD_FLIP_NUM_MASK) >> ITH_LCD_FLIP_NUM_BIT;
}

unsigned int ithLcdGetMaxLcdBufCount(void)
{
#if CFG_VIDEO_ENABLE
    return 3;
#else
    return 2;
#endif
}

void ithLcdSwFlip(unsigned int index)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_SWFLIPNUM_REG, index << ITH_LCD_SWFLIPNUM_BIT, ITH_LCD_SWFLIPNUM_MASK);
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG, 1 << ITH_LCD_LAYER1UPDATE_BIT, ITH_LCD_LAYER1UPDATE_MASK);
}

void ithLcdCursorEnable(void)
{
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_EN_REG, ITH_LCD_HWC_EN_BIT);
}

void ithLcdCursorDisable(void)
{
    ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_EN_REG, ITH_LCD_HWC_EN_BIT);
}

void ithLcdCursorCtrlEnable(ITHLcdCursorCtrl ctrl)
{
    switch(ctrl)
    {
    case ITH_LCD_CURSOR_ALPHABLEND_ENABLE:
        ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_ABLDEN_BIT);
        break;
    case ITH_LCD_CURSOR_DEFDEST_ENABLE:
        ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_DEFDSTEN_BIT);
        break;
    case ITH_LCD_CURSOR_INVDEST_ENABLE:
        ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_DEFINVDST_BIT);
        break;
    }
}

void ithLcdCursorCtrlDisable(ITHLcdCursorCtrl ctrl)
{
    switch(ctrl)
    {
    case ITH_LCD_CURSOR_ALPHABLEND_ENABLE:
        ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_ABLDEN_BIT);
        break;
    case ITH_LCD_CURSOR_DEFDEST_ENABLE:
        ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_DEFDSTEN_BIT);
        break;
    case ITH_LCD_CURSOR_INVDEST_ENABLE:
        ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_CR_REG, ITH_LCD_HWC_DEFINVDST_BIT);
        break;
    }
}

void ithLcdCursorSetWidth(unsigned int width)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_WIDTH_REG, width << ITH_LCD_HWC_WIDTH_BIT, ITH_LCD_HWC_WIDTH_MASK);
}

void ithLcdCursorSetHeight(unsigned int height)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_HEIGHT_REG, height << ITH_LCD_HWC_HEIGHT_BIT, ITH_LCD_HWC_HEIGHT_MASK);
}

void ithLcdCursorSetPitch(unsigned int pitch)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_PITCH_REG, pitch << ITH_LCD_HWC_PITCH_BIT, ITH_LCD_HWC_PITCH_MASK);
}

void ithLcdCursorSetX(unsigned int x)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_POSX_REG, x << ITH_LCD_HWC_POSX_BIT, ITH_LCD_HWC_POSX_MASK);
}

void ithLcdCursorSetY(unsigned int y)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_POSY_REG, y << ITH_LCD_HWC_POSY_BIT, ITH_LCD_HWC_POSY_MASK);
}

void ithLcdCursorSetColor(ITHLcdCursorColor color, uint16_t value)
{
    switch(color)
    {
        case ITH_LCD_CURSOR_DEF_COLOR:
            ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_DEFCOLOR_REG, value << ITH_LCD_HWC_DEFCOLOR_BIT, ITH_LCD_HWC_DEFCOLOR_MASK);
            break;
        case ITH_LCD_CURSOR_FG_COLOR:
            ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_FORECOLOR_REG, value << ITH_LCD_HWC_FORECOLOR_BIT, ITH_LCD_HWC_FORECOLOR_MASK);
            break;
        case ITH_LCD_CURSOR_BG_COLOR:
            ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_HWC_BACKCOLOR_REG, value << ITH_LCD_HWC_BACKCOLOR_BIT, ITH_LCD_HWC_BACKCOLOR_MASK);
            break;
    }
}

void ithLcdCursorUpdate(void)
{
    ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_HWC_UPDATE_REG, ITH_LCD_HWC_UPDATE_BIT);
}

 bool ithLcdCursorIsUpdateDone(void)
{
     return ithReadRegA(ITH_LCD_BASE + ITH_LCD_HWC_UPDATE_REG) & (0x1 << ITH_LCD_HWC_UPDATE_BIT);
}

void ithLcdIntrCtrlEnable(ITHLcdIntrCtrl ctrl)
{
    switch (ctrl)
    {
    case ITH_LCD_INTR_ENABLE:
        ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_INT_CTRL_REG, ITH_LCD_INT_EN_BIT);
        break;
    case ITH_LCD_INTR_FIELDMODE1:
        ithSetRegBitA(ITH_LCD_BASE + ITH_LCD_INT_CTRL_REG, ITH_LCD_INT_FIELDMODE1_BIT);
        break;
    case ITH_LCD_INTR_OUTPUT2:
    case ITH_LCD_INTR_FIELDMODE2:
    case ITH_LCD_INTR_OUTPUT1:
        break;
    }
}

void ithLcdIntrCtrlDisable(ITHLcdIntrCtrl ctrl)
{
    switch (ctrl)
    {
    case ITH_LCD_INTR_ENABLE:
        ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_INT_CTRL_REG, ITH_LCD_INT_EN_BIT);
        break;

    case ITH_LCD_INTR_FIELDMODE1:
        ithClearRegBitA(ITH_LCD_BASE + ITH_LCD_INT_CTRL_REG, ITH_LCD_INT_FIELDMODE1_BIT);
        break;
    case ITH_LCD_INTR_OUTPUT2:
    case ITH_LCD_INTR_FIELDMODE2:
    case ITH_LCD_INTR_OUTPUT1:
        break;

    }
}

void ithLcdIntrEnable(void)
{
    ithLcdIntrCtrlEnable(ITH_LCD_INTR_ENABLE);
}

void ithLcdIntrDisable(void)
{
    ithLcdIntrCtrlDisable(ITH_LCD_INTR_ENABLE);
}

void ithLcdIntrClear(void)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_INT_CLR_REG, 0x1 << ITH_LCD_INT_CLR_BIT, ITH_LCD_INT_CLR_MASK);
}

void ithLcdIntrSetScanLine1(unsigned int line)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_INT_LINE1_REG, line << ITH_LCD_INT_LINE1_BIT, ITH_LCD_INT_LINE1_MASK);
}

void ithLcdIntrSetScanLine2(unsigned int line)
{

}

void ithLcdSetRotMode(ITHLcdScanType type, ITHLcdRotMode mode)
{
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_SET1_REG, type << ITH_LCD_SCAN_TYPE_BIT, 0x1 << ITH_LCD_SCAN_TYPE_BIT);
    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_SET1_REG, mode << ITH_LCD_ROT_MODE_BIT, ITH_LCD_ROT_MODE_MASK);

    ithWriteRegMaskA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG, 1 << ITH_LCD_LAYER1UPDATE_BIT, ITH_LCD_LAYER1UPDATE_MASK);

    while (ithReadRegA(ITH_LCD_BASE + ITH_LCD_UPDATE_REG) & (0x1 << ITH_LCD_LAYER1UPDATE_BIT))
    {
        ithDelay(1000);
    }
}

ITHLcdPanelType ithLcdGetPanelType(void)
{
    ITHLcdPanelType panelType = ITH_LCD_RGB;

    if (ithReadRegA(ITH_LCD_BASE + ITH_LCD_MIPI_SET1_REG) & 0x1)
        panelType = ITH_LCD_MIPI;
    else if (ithReadRegA(ITH_LCD_BASE + ITH_LCD_LVDS_SET1_REG) & 0x1)
        panelType = ITH_LCD_LVDS;

    return panelType;

}

void ithLcdSaveMipiHSInfo(void)
{
    //save MIPI HS registers
    mipiHSTable[0] = ithReadRegA(0xd0c00004);
    mipiHSTable[1] = ithReadRegA(0xd0c00008);
    mipiHSTable[2] = ithReadRegA(0xd0c00010);
    mipiHSTable[3] = ithReadRegA(0xd0c00014);
    mipiHSTable[4] = ithReadRegA(0xd0c00018);
    mipiHSTable[5] = ithReadRegA(0xd0c0001c);
    mipiHSTable[6] = ithReadRegA(0xd0c00020);
    mipiHSTable[7] = ithReadRegA(0xd0c00028);
    mipiHSTable[8] = ithReadRegA(0xd0c00048);
    mipiHSTable[9] = ithReadRegA(0xd0c00054);
    mipiHSTable[10] = ithReadRegA(0xd0c00058);
    mipiHSTable[11] = ithReadRegA(0xd0c00080);
    mipiHSTable[12] = ithReadRegA(0xd0c00084);
    mipiHSTable[13] = ithReadRegA(0xd0c00088);
    mipiHSTable[14] = ithReadRegA(0xd0c0008c);
    mipiHSTable[15] = ithReadRegA(0xd0c00050);

    mipiHSPLLFrange[0] = ithReadRegA(0xDC10001C);
    mipiHSPLLFrange[1] = ithReadRegA(0xDC100020);
    mipiHSPLLFrange[2] = ithReadRegA(0xDC10002C);
    mipiHSPLLFrange[3] = ithReadRegA(0xDC100034);
    //non-continuous settings
    mipiHSPLLFrange[4] = ithReadRegA(0xDC100024);
    mipiHSPLLFrange[5] = ithReadRegA(0xDC100028);
}

void ithLcdResetMipiHSInfo(void)
{
    ithWriteRegA(0xDC10001C, mipiHSPLLFrange[0]);
    ithWriteRegA(0xDC100020, mipiHSPLLFrange[1]);
    ithWriteRegA(0xDC10002C, mipiHSPLLFrange[2]);
    ithWriteRegA(0xDC100034, mipiHSPLLFrange[3]);
    //non-continuous settings
    ithWriteRegA(0xDC100024, mipiHSPLLFrange[4]);
    ithWriteRegA(0xDC100028, mipiHSPLLFrange[5]);
    usleep(150);

    //resetting MIPI HS registers
    ithWriteRegA(0xd0c00004, mipiHSTable[0]); //+0x6[7]=BLLP, +0x04[0]=EOTPGE
    ithWriteRegA(0xd0c00008, mipiHSTable[1]);
    ithWriteRegA(0xd0c00010, mipiHSTable[2]);
    ithWriteRegA(0xd0c00014, mipiHSTable[3]);  //RGB666(0x1E),RGB888(0x3E)
    ithWriteRegA(0xd0c00018, mipiHSTable[4]);  //+0x18[5:0]=CLWR
    ithWriteRegA(0xd0c0001c, mipiHSTable[5]);  //24-bit pixel
    ithWriteRegA(0xd0c00020, mipiHSTable[6]);  //HACT=1024(0x0400)
    ithWriteRegA(0xd0c00028, mipiHSTable[7]);  //1024*3(0x0c00)
    //WRITE d0c0003c 000000ff f
    ithWriteRegA(0xd0c00048, mipiHSTable[8]);  //+0x48[6]=HSE Pkt
    //WRITE d0c00050 000006a0 f
    ithWriteRegA(0xd0c00054, mipiHSTable[9]);  // HSS(4)+HSA*3+HSE(4)+HBP*3+4+HACT*3+2+HFP*3
    ithWriteRegA(0xd0c00058, mipiHSTable[10]);
    ithWriteRegA(0xd0c00080, mipiHSTable[11]);  // VBP=23(0x17),VSA=2(0x02)
    ithWriteRegA(0xd0c00084, mipiHSTable[12]);  // VACT=600(0x0258),VFP=12(0x0c)
    ithWriteRegA(0xd0c00088, mipiHSTable[13]);  // HBP=140*3(0x01a4),HSA=20*3(0x3c)
    ithWriteRegA(0xd0c0008c, mipiHSTable[14]);  // HFP=160*3(0x01e0)

    ithWriteRegA(0xd0c00050, mipiHSTable[15]);  // pixel fifo threshold
    usleep(150);
}

void ithLcdSavePinShareInfo(void)
{
    //save LCD pin share registers
    pinShareTable[0] = ithReadRegA(0xD10000E4);
    pinShareTable[1] = ithReadRegA(0xD10000E8);
    pinShareTable[2] = ithReadRegA(0xD10000EC);
    pinShareTable[3] = ithReadRegA(0xD1000160);
    pinShareTable[4] = ithReadRegA(0xD1000164);
    pinShareTable[5] = ithReadRegA(0xD1000168);
    pinShareTable[6] = ithReadRegA(0xD100016C);
}

void ithLcdResetPinShareInfo(void)
{
    //resetting LCD pin share registers
    ithWriteRegA(0xD10000E4, pinShareTable[0]);
    ithWriteRegA(0xD10000E8, pinShareTable[1]);
    ithWriteRegA(0xD10000EC, pinShareTable[2]);
    ithWriteRegA(0xD1000160, pinShareTable[3]);
    ithWriteRegA(0xD1000164, pinShareTable[4]);
    ithWriteRegA(0xD1000168, pinShareTable[5]);
    ithWriteRegA(0xD100016C, pinShareTable[6]);
}

/**
* Setting hue API in LCD color conversion registers.
*
* @param hue (Hue)'s range is 0~359.
*/
int ithLcdSetHue(int hue)
{
    int result;

    if (hue < 0 || hue > 359)
    {
        printf("[ERROR] Angle (Hue)'s range is 0~359, but %d is given.\n'", hue);
        return 1;
    }

    gHue = hue;
    result = _color_conversion_main(gHue);

    return result;
}

/**
* Setting contrast API in LCD gamma function registers.
*
* @param contrast Contrast's range is -64~63.
*/
int ithLcdSetContrast(int contrast)
{
    int result;

    if (contrast < -64 || contrast > 63)
    {
        printf("[ERROR] (Contrast)'s range is -64~63, but (%d) is given.\n'", contrast);
        return 1;
    }

    gContrast = contrast;
    result = _gamma_function_api(gContrast, gBrightness);

    return result;
}

/**
* Setting brightness API in LCD gamma function registers.
*
* @param brightness Brightness's range is -64~63.
*/
int ithLcdSetBrightness(int brightness)
{
    int result;

    if (brightness < -64 || brightness > 63)
    {
        printf("[ERROR] (Brightness)'s range is -64~63, but (%d) is given.\n'", brightness);
        return 1;
    }

    gBrightness = brightness;
    result = _gamma_function_api(gContrast, gBrightness);

    return result;
}

#ifdef CFG_LCD_PQ_TUNING
void ithLcdVPOnFlyEnable()
{
    outInfo.width = ithLcdGetWidth();
    outInfo.height = ithLcdGetHeight();
    outInfo.pitchRGB = ithLcdGetPitch();
    outInfo.format = MMP_ISP_OUT_DITHER565A;
    mmpIspSetOutputWindow(gIspDev, &outInfo);
    mmpIspSetVideoWindow(gIspDev, 0, 0, ithLcdGetWidth(), ithLcdGetHeight());
    mmpIspDisableCMDQ(gIspDev);
    mmpIspEnableLCDOnFlyMode(gIspDev);
    mmpIspPlayImageProcess(gIspDev, &inputInfo);
    mmpIspEnableCMDQ(gIspDev);
}

void ithLcdOnFlyReset()
{
    //switch LCD to test mode for suspending memory access
    ithWriteRegA(ITH_LCD_BASE + 0x0020, 0x81000000);
    while (ithReadRegA(ITH_LCD_BASE + 0x0020) & 0x80000000)
    {
        ithDelay(1000);
    }

    //reset LCD controller
    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0x1 << ITH_LCD_RST_BIT, 0x1 << ITH_LCD_RST_BIT);
    usleep(2);
    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0x0, 0x1 << ITH_LCD_RST_BIT);

    //switch LCD to general mode and enable LCD to do OnFly
    ithWriteRegA(ITH_LCD_BASE + 0x0020, 0x00000000);
    ithWriteRegMaskA(ITH_LCD_BASE + 0x0000, 0x1 << 7, 0x1 << 7);

    //fire LCD
    ithWriteRegMaskA(ITH_LCD_BASE + 0x001C, 0x1 << 0, 0x1 << 0);
    usleep(1);
    ithWriteRegMaskA(ITH_LCD_BASE + 0x001C, 0x3 << 0, 0x3 << 0);
}

void ithLcdOnFlyPowerSleepReset()
{
    //reset LCD controller
    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0x1 << ITH_LCD_RST_BIT, 0x1 << ITH_LCD_RST_BIT);
    usleep(2);
    ithWriteRegMaskA(ITH_HOST_BASE + ITH_LCD_CLK1_REG, 0x0, 0x1 << ITH_LCD_RST_BIT);

    //disable VP clock and reset VP by ithClockWakeup function
    usleep(20000);
    ithIspDisableClock();
}

void ithLcdOnFlyPowerSleepEnable()
{
    //enable VP to generate clock
    ithIspEnableClock();
    usleep(2);

    //enable LCD to do OnFly
    ithWriteRegMaskA(ITH_LCD_BASE + 0x0000, 0x1 << 7, 0x1 << 7);

    //fire LCD
    ithWriteRegMaskA(ITH_LCD_BASE + 0x001C, 0x1 << 0, 0x1 << 0);
    usleep(1);
    ithWriteRegMaskA(ITH_LCD_BASE + 0x001C, 0x3 << 0, 0x3 << 0);

    //fire OnFly VP
    ithLcdVPOnFlyEnable();
}

void ithLcdOnFlyEnable()
{
    ithLcdOnFlyReset();
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_0);
    coreInfo.EnPreview = false;
    coreInfo.PreScaleSel = MMP_ISP_PRESCALE_NORMAL;
    mmpIspSetCore(gIspDev, &coreInfo);
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_TRANSFORM);
    inputInfo.addrY = ithLcdGetBaseAddrA();
    inputInfo.width = ithLcdGetWidth();
    inputInfo.height = ithLcdGetHeight();
    inputInfo.pitchY = ithLcdGetPitch();
    inputInfo.format = MMP_ISP_IN_RGB565;
    ithLcdVPOnFlyEnable();
}

void ithLcdOnFlyDisable()
{
    mmpIspTerminate(&gIspDev);
    ithIspResetAllReg();
    ithIspResetReg(ITH_ISP_CORE0);
}

void ithLcdVPOnFlyUpdate(uint32_t inputInfo_addrY)
{
    inputInfo.addrY = inputInfo_addrY;
    mmpIspPlayImageProcess(gIspDev, &inputInfo);
}

ISP_RESULT ithLcdVPOnFlyCCUpdate(MMP_ISP_COLOR_CTRL colorCtrl, int value)
{
    return mmpIspSetColorCtrl(gIspDev, colorCtrl, value);
}
#endif
