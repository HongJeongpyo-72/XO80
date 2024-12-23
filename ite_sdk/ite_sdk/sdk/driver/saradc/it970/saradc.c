#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#include "ite/ith.h"
#include "ite/itp.h"
#include "saradc/saradc.h"
#include "saradc/saradc_hw.h"

//=============================================================================
//                Constant Definition
//=============================================================================
#define SARADC_output_set_byte       8
#define SARADC_output_set_valid_byte 6
#define SARADC_output_unit2_byte     3

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
typedef struct XAIN_OBJECT_TAG
{
    SARADC_PORT                 hwPort;
    int                         cfgGPIO;
    SARADC_IO_MAPPING_ENTRY     mappingEntry;
    XAIN_INFO                   xainInfo;
} XAIN_OBJECT;

typedef struct SARADC_OBJECT_TAG
{
    int                         refCount;
    pthread_mutex_t             mutex;
    uint8_t                     validXAIN;
    uint8_t                     enableXAIN;
    SARADC_MODE                 opMode;
    SARADC_AMPLIFY_GAIN         driving;
    SARADC_CLK_DIV              clkDivider;
    uint16_t                    wbSize;
    SARADC_AVG_CAL_COUNT        avgCalCount;
    bool                        inUse;
    XAIN_OBJECT                 XAINObjects[SARADC_XAIN_MAX_COUNT];
#ifdef CFG_SARADC_CALIBRATION_ENABLE
    uint8_t                     caliXAIN;
    float                       caliVoltage[SARADC_XAIN_MAX_COUNT];
    bool                        caliUse;
#endif
} SARADC_OBJECT;

typedef struct PORT_IO_MAPPING_ENTRY_TAG
{
    SARADC_PORT                 hwPort;
    SARADC_IO_MAPPING_ENTRY     mappingEntry;
} PORT_IO_MAPPING_ENTRY;

typedef struct AD_MAPPING_ENTRY_TAG
{
    float                       analogInput;
    uint16_t                    digitalOutput;
} AD_MAPPING_ENTRY;

//=============================================================================
//                Global Data Definition
//=============================================================================
static SARADC_OBJECT SARADCObject =
{
    0,
    PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
    CFG_SARADC_VALID_XAIN,
    CFG_SARADC_VALID_XAIN,
    SARADC_MODE_SRAM_INTR,
    SARADC_AMPLIFY_1X,
    SARADC_CLK_DIV_9,
    0,
    SARADC_AVG_CAL_COUNT_5,
    false,
#ifdef CFG_SARADC_CALIBRATION_ENABLE
    {0},
    CFG_SARADC_CALIBRATION_XAIN,
    {CFG_SARADC_CALIBRATION_VOLTAGE},
    true
#else
    {0}
#endif
};

static PORT_IO_MAPPING_ENTRY tSARADCMappingTable[] =
{
    {SARADC_0, 17, 0}, {SARADC_1, 18, 0}, {SARADC_2, 19, 0}, {SARADC_3, 20, 0}
};

static AD_MAPPING_ENTRY tREALMappingTable[] =
{
    {0.0,    0}, {0.1,   52}, {0.2,  189}, {0.3,  322},
    {0.4,  452}, {0.5,  584}, {0.6,  723}, {0.7,  847},
    {0.8,  988}, {0.9, 1117}, {1.0, 1253}, {1.1, 1369},
    {1.2, 1503}, {1.3, 1644}, {1.4, 1769}, {1.5, 1900},
    {1.6, 2038}, {1.7, 2183}, {1.8, 2303}, {1.9, 2440},
    {2.0, 2559}, {2.1, 2712}, {2.2, 2845}, {2.3, 2968},
    {2.4, 3071}, {2.5, 3237}, {2.6, 3368}, {2.7, 3493},
    {2.8, 3627}, {2.9, 3765}, {3.0, 3894}, {3.1, 4026},
    {3.2, 4095}, {3.3, 4095}
};

static AD_MAPPING_ENTRY tINTERPOLATIONMappingTable[] =
{
    {0.0,    0}, {0.1,  124}, {0.2,  248}, {0.3,  372},
    {0.4,  496}, {0.5,  620}, {0.6,  745}, {0.7,  869},
    {0.8,  993}, {0.9, 1117}, {1.0, 1241}, {1.1, 1365},
    {1.2, 1489}, {1.3, 1613}, {1.4, 1737}, {1.5, 1861},
    {1.6, 1985}, {1.7, 2110}, {1.8, 2234}, {1.9, 2358},
    {2.0, 2482}, {2.1, 2606}, {2.2, 2730}, {2.3, 2854},
    {2.4, 2978}, {2.5, 3102}, {2.6, 3226}, {2.7, 3350},
    {2.8, 3475}, {2.9, 3599}, {3.0, 3723}, {3.1, 3847},
    {3.2, 3971}, {3.3, 4095}
};

#ifdef CFG_SARADC_CALIBRATION_ENABLE
static AD_MAPPING_ENTRY tCALIBRATIONMappingTable[SARADC_XAIN_MAX_COUNT] = {0};
#endif

//=============================================================================
//                Private Function Definition
//=============================================================================
static void
_resetSARADCObject(
    SARADC_OBJECT *object)
{
    int   i = 0;
#ifdef CFG_SARADC_CALIBRATION_ENABLE
    float caliVoltage[SARADC_XAIN_MAX_COUNT] = {CFG_SARADC_CALIBRATION_VOLTAGE};
#endif

    object->validXAIN = CFG_SARADC_VALID_XAIN;
    object->enableXAIN = CFG_SARADC_VALID_XAIN;
    object->opMode = SARADC_MODE_SRAM_INTR;
    object->driving = SARADC_AMPLIFY_1X;
    object->clkDivider = SARADC_CLK_DIV_9;
    object->wbSize = 0;
    object->avgCalCount = SARADC_AVG_CAL_COUNT_5;
    object->inUse = false;
#ifdef CFG_SARADC_CALIBRATION_ENABLE
    object->caliXAIN = CFG_SARADC_CALIBRATION_XAIN;
    memcpy(object->caliVoltage, caliVoltage, sizeof(caliVoltage));
    object->caliUse = true;
#endif

    memset(object->XAINObjects, 0, sizeof(object->XAINObjects));
    for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
    {
        if (i == SARADC_0)
        {
            object->XAINObjects[i].hwPort = SARADC_0;
            object->XAINObjects[i].cfgGPIO = CFG_GPIO_XAIN_0_RX;
        }
        else if (i == SARADC_1)
        {
            object->XAINObjects[i].hwPort = SARADC_1;
            object->XAINObjects[i].cfgGPIO = CFG_GPIO_XAIN_1_RX;
        }
        else if (i == SARADC_2)
        {
            object->XAINObjects[i].hwPort = SARADC_2;
            object->XAINObjects[i].cfgGPIO = CFG_GPIO_XAIN_2_RX;
        }
        else if (i == SARADC_3)
        {
            object->XAINObjects[i].hwPort = SARADC_3;
            object->XAINObjects[i].cfgGPIO = CFG_GPIO_XAIN_3_RX;
        }
    }
}

static bool
_assignSARADCGpio(SARADC_OBJECT *object)
{
    int                     i = 0;
    int                     j = 0;
    int                     XAINGpio = 0;
    int                     entryCount = 0;
    SARADC_IO_MAPPING_ENTRY *pMappingEntry = NULL;

    for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
    {
        pMappingEntry = &object->XAINObjects[i].mappingEntry;

        entryCount = sizeof(tSARADCMappingTable) / sizeof(PORT_IO_MAPPING_ENTRY);
        for (j = 0; j < entryCount; j++)
        {
            if (object->XAINObjects[i].hwPort == tSARADCMappingTable[j].hwPort &&
                    object->XAINObjects[i].cfgGPIO == tSARADCMappingTable[j].mappingEntry.gpioPin)
                break;
        }

        if (j >= entryCount)
            return false;

        pMappingEntry->gpioPin = tSARADCMappingTable[j].mappingEntry.gpioPin;
        pMappingEntry->gpioMode = tSARADCMappingTable[j].mappingEntry.gpioMode;
    }

    return true;
}

static bool
_setSARADCGPIO(SARADC_OBJECT *object)
{
    int                     setGpio = 0;
    int                     setMode = 0;
    uint8_t                 validXAIN = object->validXAIN;
    int                     i = 0;
    SARADC_IO_MAPPING_ENTRY *pMappingEntry = NULL;

    if (validXAIN < 0 || validXAIN > (int)pow(2, SARADC_XAIN_MAX_COUNT) - 1)
    {
        printf("[SARADC][%s] validXAIN is out of range: %d\n", __func__, validXAIN);
        return false;
    }

    if (!_assignSARADCGpio(object))
    {
        printf("[SARADC][%s] _assignSARADCGpio() fail\n", __func__);
        return false;
    }

    while (validXAIN)
    {
        if (validXAIN & 0x1)
        {
            pMappingEntry  = &object->XAINObjects[i].mappingEntry;

            setGpio = pMappingEntry->gpioPin;
            setMode = pMappingEntry->gpioMode;

            ithGpioSetDriving(setGpio, ITH_GPIO_DRIVING_3);
            //ithGpioCtrlEnable(setGpio, ITH_GPIO_PULL_ENABLE);
            //ithGpioCtrlDisable(setGpio, ITH_GPIO_PULL_UP);
            ithGpioSetIn(setGpio);
            ithGpioSetMode(setGpio, setMode);
        }
        validXAIN >>= 1;
        i++;
    }

    return true;
}

static void
_setSARADCGeneralReg(
    SARADC_OBJECT *object)
{
    SARADC_SetCLKDIV_Reg(object->clkDivider);
    SARADC_SetMode_Reg(object->opMode);
    SARADC_SetGAIN_Reg(object->driving);
    SARADC_SetValidXAIN_Reg(object->validXAIN);
    SARADC_EnableXAININTR_Reg(object->enableXAIN);
}

static void
_resetSARADCXAINReg(
    SARADC_OBJECT *object)
{
    int i = 0;

    for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
    {
        if (i == SARADC_0)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_0, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_0);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_0);
        }
        else if (i == SARADC_1)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_1, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_1);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_1);
        }
        else if (i == SARADC_2)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_2, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_2);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_2);
        }
        else if (i == SARADC_3)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_3, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_3);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_3);
        }
    }
}

#ifdef CFG_SARADC_CALIBRATION_ENABLE
static SARADC_RESULT
mmpSARDynamicallyCalibrate(
    uint16_t input,
    uint16_t *output)
{
    SARADC_RESULT    result = SARADC_SUCCESS;
    uint8_t          caliXAIN = 0x0;
    int              h = 0;
    int              i = 0;
    int              j = 0;
    int              k = 0;
    int              z = 0;
    bool             check = false;
    AD_MAPPING_ENTRY start = {0};
    AD_MAPPING_ENTRY end = {0};
    float            ratio_temp = 0;
    float            voltage_temp = 0;

    if (input > 0xFFF || !output)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        goto end;
    }

    caliXAIN = SARADCObject.caliXAIN;
    while (caliXAIN)
    {
        if (caliXAIN & 0x1)
        {
            if (tCALIBRATIONMappingTable[j].digitalOutput <= input)
                h++;
            i++;
        }
        j++;
        caliXAIN >>= 1;
    }

    k = 0;
    z = 0;
    caliXAIN = SARADCObject.caliXAIN;
    if (h == 0 || h == i)
    {
        while (caliXAIN)
        {
            if (caliXAIN & 0x1)
            {
                if (h == 0 && k == 0)
                {
                    memcpy(&start, &tCALIBRATIONMappingTable[z], sizeof(start));
                }
                else if (h == 0 && k == 1)
                {
                    memcpy(&end, &tCALIBRATIONMappingTable[z], sizeof(end));
                    break;
                }
                else if (h == i && k == h - 2)
                {
                    memcpy(&start, &tCALIBRATIONMappingTable[z], sizeof(start));
                }
                else if (h == i && k == h - 1)
                {
                    memcpy(&end, &tCALIBRATIONMappingTable[z], sizeof(end));
                    break;
                }
                k++;
            }
            z++;
            caliXAIN >>= 1;
        }

        ratio_temp = ((float)end.digitalOutput - start.digitalOutput) / (end.analogInput - start.analogInput);

        if (h == 0)
        {
            voltage_temp = start.analogInput - (((float)start.digitalOutput - input) / ratio_temp);
            if (voltage_temp < tINTERPOLATIONMappingTable[0].analogInput) voltage_temp = 0;
        }
        else if (h == i)
        {
            voltage_temp = end.analogInput + (((float)input - end.digitalOutput) / ratio_temp);
            if (voltage_temp > tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].analogInput)
                voltage_temp = tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].analogInput;
        }
    }
    else
    {
        while (caliXAIN)
        {
            if (caliXAIN & 0x1)
            {
                if (k == h - 1)
                {
                    memcpy(&start, &tCALIBRATIONMappingTable[z], sizeof(start));
                }
                else if (k == h)
                {
                    memcpy(&end, &tCALIBRATIONMappingTable[z], sizeof(end));
                    break;
                }
                k++;
            }
            z++;
            caliXAIN >>= 1;
        }

        ratio_temp = ((float)end.digitalOutput - start.digitalOutput) / (end.analogInput - start.analogInput);

        voltage_temp = start.analogInput + (((float)input - start.digitalOutput) / ratio_temp);
        if (voltage_temp > tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].analogInput)
            voltage_temp = tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].analogInput;
    }

    *output = (uint16_t)(voltage_temp / tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].analogInput *
            tINTERPOLATIONMappingTable[sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]) - 1].digitalOutput);

end:
    return result;
}
#endif

//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * SARADC initialization.
 *
 * @param mode               indicate which message notification will run.
 * @param amplifyDriving     determine XAIN driving.
 * @param divider            set clock divider of SARADC.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARInitialize(
    SARADC_MODE         mode,
    SARADC_AMPLIFY_GAIN amplifyDriving,
    SARADC_CLK_DIV      divider)
{
    SARADC_RESULT result = SARADC_SUCCESS;
#ifdef CFG_SARADC_CALIBRATION_ENABLE
    uint8_t       validXAIN = 0x0;
    uint8_t       caliXAIN = 0x0;
    int           i = 0;
#endif

    if (mode < SARADC_MODE_SRAM_INTR || mode > SARADC_MODE_INTR ||
        amplifyDriving < SARADC_AMPLIFY_1X || amplifyDriving > SARADC_AMPLIFY_8X ||
        divider < SARADC_CLK_DIV_0 || divider > SARADC_CLK_DIV_31)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount == 0)
    {
        ithSARADCEnableClock();
        ithSARADCResetEngine();

        _resetSARADCObject(&SARADCObject);
        SARADCObject.refCount = 1;
        SARADCObject.opMode = mode;
        SARADCObject.driving = amplifyDriving;
        SARADCObject.clkDivider = divider;

        if (!_setSARADCGPIO(&SARADCObject))
        {
            printf("[SARADC][%s] _setSARADCGPIO() fail\n", __func__);
            result = SARADC_ERR_INVALID_GPIO_SETTING;
            goto end;
        }

#ifdef CFG_SARADC_CALIBRATION_ENABLE
        validXAIN = SARADCObject.validXAIN;
        caliXAIN = SARADCObject.caliXAIN;

        if (caliXAIN < 0 || caliXAIN > 0xf)
        {
            SARADCObject.caliUse = false;
            printf("[SARADC][%s] caliXAIN is out of range: %d\n", __func__, caliXAIN);
            result = SARADC_ERR_INVALID_XAIN;
            goto end;
        }

        while (caliXAIN)
        {
            if (caliXAIN & 0x1) i++;
            caliXAIN >>= 1;
        }

        if (i < 2)
        {
            SARADCObject.caliUse = false;
            printf("[SARADC][%s] caliXAIN is 0x%x, but it must be greater than one reference\n", __func__, caliXAIN);
            result = SARADC_ERR_INVALID_XAIN;
            goto end;
        }

        caliXAIN = SARADCObject.caliXAIN;
        for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
        {
            if (!(validXAIN & 0x1) && (caliXAIN & 0x1))
            {
                SARADCObject.caliUse = false;
                printf("[SARADC][%s] validXAIN is 0x%x, but caliXAIN is 0x%x\n",
                        __func__, SARADCObject.validXAIN, SARADCObject.caliXAIN);
                result = SARADC_ERR_INVALID_XAIN;
                goto end;
            }
            validXAIN >>= 1;
            caliXAIN >>= 1;
        }
#endif

        _setSARADCGeneralReg(&SARADCObject);
        _resetSARADCXAINReg(&SARADCObject);
    }
    else
    {
        SARADCObject.refCount++;
        printf("[SARADC][%s] already initialed. refCount = %d\n", __func__, SARADCObject.refCount);
    }

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set Write buffer size of SARADC.
 *
 * @param    wbSize set Write buffer range for SARADC accessing.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetWriteBufferSize(
    uint16_t wbSize)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (wbSize < 0x40 || wbSize > CFG_RAM_SIZE)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not reset wbSize\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.wbSize = wbSize;
    SARADC_SetWBSize_Reg(SARADCObject.wbSize - 1);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set average calculation count of SARADC.
 *
 * @param avgCalCount    set average calculation count for SARADC reference
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetAVGCalCount(
    SARADC_AVG_CAL_COUNT avgCalCount)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (avgCalCount < SARADC_AVG_CAL_COUNT_0 || avgCalCount > SARADC_AVG_CAL_COUNT_31)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not reset avgCalCount\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.avgCalCount = avgCalCount;
    SARADC_SetAVGCALCount_Reg(SARADCObject.avgCalCount);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
* SARADC termination.
*
* @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
*/
//=============================================================================
SARADC_RESULT
mmpSARTerminate(
    void)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    pthread_mutex_lock(&SARADCObject.mutex);
    SARADCObject.refCount--;
    if (SARADCObject.refCount == 0)
    {
        ithSARADCResetEngine();
        ithSARADCDisableClock();
    }
    else if (SARADCObject.refCount == -1)
    {
        SARADCObject.refCount = 0;
        printf("[SARADC][%s] SARADC has not been initialized\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
    }
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Fire SARADC Engine.
 *
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARFire(
    void)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not fire SARADC\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.inUse = true;
    _resetSARADCXAINReg(&SARADCObject);
    SARADC_DriverFire_Reg();

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Stop SARADC Engine.
 *
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARStop(
    void)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (!SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is idle, can not stop SARADC\n", __func__);
        result = SARADC_ERR_IS_IDLE;
        goto end;
    }

    SARADCObject.inUse = false;
    SARADC_DriverStop_Reg();

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set XAIN memory base of SARADC.
 *
 * @param hwPort      indicate which XAIN will be applied.
 * @param baseAddr    determine memory base of XAIN.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetMEMBase(
    SARADC_PORT hwPort,
    uint8_t     *baseAddr)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 || (uint32_t)baseAddr >= CFG_RAM_SIZE)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not set XAIN information\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.XAINObjects[hwPort].xainInfo.baseAddr = baseAddr;
    SARADC_SetXAINMEMBase_Reg(hwPort, SARADCObject.XAINObjects[hwPort].xainInfo.baseAddr);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set XAIN detection rule of SARADC.
 *
 * @param hwPort          indicate which XAIN will be applied.
 * @param maxAVGDetect    determine max detection rule of XAIN.
 * @param minAVGDetect    determine min detection rule of XAIN.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetAVGDetectRule(
    SARADC_PORT hwPort,
    uint16_t    maxAVGDetect,
    uint16_t    minAVGDetect)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 || maxAVGDetect > 0xFFF || minAVGDetect > 0xFFF)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not set XAIN information\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.XAINObjects[hwPort].xainInfo.maxAVGDetect = maxAVGDetect;
    SARADCObject.XAINObjects[hwPort].xainInfo.minAVGDetect = minAVGDetect;
    SARADC_SetXAINDetectRule_Reg(hwPort,
            SARADCObject.XAINObjects[hwPort].xainInfo.maxAVGDetect,
            SARADCObject.XAINObjects[hwPort].xainInfo.minAVGDetect);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set SARADC Read pointer.
 *
 * @param hwPort    indicate which XAIN will be applied.
 * @param rptr      determine Read pointer offset of XAIN.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetReadPointer(
    SARADC_PORT hwPort,
    uint16_t    rptr)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 || (uint32_t)rptr >= CFG_RAM_SIZE)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not set XAIN information\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.XAINObjects[hwPort].xainInfo.rptr = rptr;
    SARADC_SetXAINRPTR_Reg(hwPort, SARADCObject.XAINObjects[hwPort].xainInfo.rptr);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Read SARADC Write pointer.
 *
 * @param hwPort    indicate which XAIN will be applied.
 * @param wptr      return Write pointer offset that XAIN recorded.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARReadWritePointer(
    SARADC_PORT hwPort,
    uint16_t    *wptr)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 || !wptr)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    SARADC_ReadXAINWPTR_Reg(hwPort, wptr);

    return result;
}

//=============================================================================
/**
 * Read SARADC Write pointer.
 *
 * @param hwPort    indicate which XAIN will be checked.
 * @return true if memory base has been overwritten, false otherwise.
 */
//=============================================================================
bool
mmpSARIsOverwritingMEM(
    SARADC_PORT hwPort)
{
    if (hwPort < SARADC_0 || hwPort > SARADC_3)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        return false;
    }

    return SARADC_IsXAINOVERWT_Reg(hwPort);
}

//=============================================================================
/**
 * Check average detection Interrupt of SARADC.
 *
 * @param hwPort    indicate which XAIN will be checked.
 * @return SARADC_INTR_AVG_VALID if digtal value was within valid range,
 * SARADC_INTR_AVG_ABOMAX and SARADC_INTR_AVG_UNDMIN otherwise.
 */
//=============================================================================
SARADC_INTR_AVG
mmpSARIsOutOfRange(
    SARADC_PORT hwPort)
{
    if (hwPort < SARADC_0 || hwPort > SARADC_3)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        return SARADC_INTR_AVG_VALID;
    }

    return SARADC_IsXAINOutOfRange_Reg(hwPort);
}

//=============================================================================
/**
 * Reset SARADC engine.
 *
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARReset(
    void)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    ithSARADCResetEngine();

    return result;
}

//=============================================================================
/**
 * Enable SARADC XAIN set.
 *
 * @param enXAIN    the bits stand for XAIN numbers from low bit to high bit.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSAREnableXAIN(
    uint8_t enXAIN)
{
    SARADC_RESULT result = SARADC_SUCCESS;
    uint8_t       validXAIN = 0x0;
    uint8_t       enXAINCheck = enXAIN;
    int           i = 0;

    if (enXAIN < 0 || enXAIN > 0xf)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not re-enable XAIN set\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    validXAIN = SARADCObject.validXAIN;

    for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
    {
        if (!(validXAIN & 0x1) && (enXAINCheck & 0x1))
        {
            printf("[SARADC][%s] SARADC validXAIN is 0x%x, can not re-enable 0x%x set\n",
                    __func__, SARADCObject.validXAIN, enXAIN);
            result = SARADC_ERR_INVALID_XAIN;
            goto end;
        }
        validXAIN >>= 1;
        enXAINCheck >>= 1;
    }

    SARADCObject.enableXAIN = enXAIN;
    SARADC_SetValidXAIN_Reg(SARADCObject.enableXAIN);
    SARADC_EnableXAININTR_Reg(SARADCObject.enableXAIN);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * SARADC conversion.
 *
 * @param hwPort    indicate which XAIN will be applied.
 * @param wbSize    set Write buffer range for SARADC accessing.
 * @param avg       return average value that XAIN recorded.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARConvert(
    SARADC_PORT hwPort,
    uint16_t    wbSize,
    uint16_t    *avg)
{
#ifdef CFG_SARADC_CALIBRATION_ENABLE

    SARADC_RESULT result = SARADC_SUCCESS;
    uint8_t       *writeBuffer = NULL;
    uint16_t      read_ptr = 0;
    uint16_t      write_ptr = 0;
    uint32_t      printf_count = 0;
    uint8_t       caliXAIN = 0x0;
    int           i = 0;
    int           k = 0;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 ||
            wbSize < 0x40 || wbSize > 0xFFFF || !avg)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (!SARADCObject.caliUse)
    {
        printf("[SARADC][%s] validXAIN is 0x%x, but caliXAIN is 0x%x\n",
                __func__, SARADCObject.validXAIN, SARADCObject.caliXAIN);
        result = SARADC_ERR_INVALID_XAIN;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not set XAIN information\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    caliXAIN = SARADCObject.caliXAIN;

    while (caliXAIN)
    {
        if (caliXAIN & 0x1)
        {
            if (result = mmpSAREnableXAIN(0x1 << i)) goto end;

            wbSize = wbSize >> 3 << 3;
            if (result = mmpSARSetWriteBufferSize(wbSize)) goto end;

            writeBuffer = (uint8_t *)itpVmemAlignedAlloc(32, wbSize);
            if (!writeBuffer)
            {
                printf("[SARADC][%s] out of memory\n", __func__);
                result = SARADC_ERR_ALLOCATION_FAIL;
                goto end;
            }
            else
            {
                memset(writeBuffer, 0, wbSize);
                ithFlushDCacheRange(writeBuffer, wbSize);
            }

            if (result = mmpSARSetMEMBase(i, writeBuffer)) goto end;

            if (result = mmpSARReadWritePointer(i, &read_ptr)) goto end;

            if (result = mmpSARSetReadPointer(i, read_ptr)) goto end;

            if (result = mmpSARSetAVGDetectRule(i, 0, 0)) goto end;

            if (result = mmpSARFire()) goto end;

            //printf("[SARADC] SARADC is writing memory ");
            printf_count = 0;
            while (mmpSARIsOverwritingMEM(i) != true)
            {
                if (printf_count % (1 * 100 * 1000) == 0)
                {
                    //printf(".");
                    //fflush(stdout);
                }
                printf_count++;
                usleep(1);
            }
            //printf("\n");
            //printf("[SARADC] finish!\n");

            if (result = mmpSARStop()) goto end;

            if (result = mmpSARReadWritePointer(i, &write_ptr)) goto end;

            ithInvalidateDCacheRange((void *)writeBuffer, wbSize);

            if (result = mmpSARCollectOutput(writeBuffer, wbSize, write_ptr, avg)) goto end;

            itpVmemFree((uint32_t)writeBuffer);
            writeBuffer = NULL;

            tCALIBRATIONMappingTable[i].analogInput = SARADCObject.caliVoltage[k];
            tCALIBRATIONMappingTable[i].digitalOutput = *avg;

            k++;
        }
        caliXAIN >>= 1;
        i++;
    }

    if (result = mmpSAREnableXAIN(0x1 << hwPort)) goto end;

    wbSize = wbSize >> 3 << 3;
    if (result = mmpSARSetWriteBufferSize(wbSize)) goto end;

    writeBuffer = (uint8_t *)itpVmemAlignedAlloc(32, wbSize);
    if (!writeBuffer)
    {
        printf("[SARADC][%s] out of memory\n", __func__);
        result = SARADC_ERR_ALLOCATION_FAIL;
        goto end;
    }
    else
    {
        memset(writeBuffer, 0, wbSize);
        ithFlushDCacheRange(writeBuffer, wbSize);
    }

    if (result = mmpSARSetMEMBase(hwPort, writeBuffer)) goto end;

    if (result = mmpSARReadWritePointer(hwPort, &read_ptr)) goto end;

    if (result = mmpSARSetReadPointer(hwPort, read_ptr)) goto end;

    if (result = mmpSARSetAVGDetectRule(hwPort, 0, 0)) goto end;

    if (result = mmpSARFire()) goto end;

    //printf("[SARADC] SARADC is writing memory ");
    printf_count = 0;
    while (mmpSARIsOverwritingMEM(hwPort) != true)
    {
        if (printf_count % (1 * 100 * 1000) == 0)
        {
            //printf(".");
            //fflush(stdout);
        }
        printf_count++;
        usleep(1);
    }
    //printf("\n");
    //printf("[SARADC] finish!\n");

    if (result = mmpSARStop()) goto end;

    if (result = mmpSARReadWritePointer(hwPort, &write_ptr)) goto end;

    ithInvalidateDCacheRange((void *)writeBuffer, wbSize);

    if (result = mmpSARCollectOutput(writeBuffer, wbSize, write_ptr, avg)) goto end;

    if (result = mmpSARDynamicallyCalibrate(*avg, avg)) goto end;

end:
    if (writeBuffer) itpVmemFree((uint32_t)writeBuffer);

    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;

#else

    SARADC_RESULT result = SARADC_SUCCESS;
    uint8_t       *writeBuffer = NULL;
    uint16_t      read_ptr = 0;
    uint16_t      write_ptr = 0;
    uint32_t      printf_count = 0;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 ||
            wbSize < 0x40 || wbSize > 0xFFFF || !avg)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    pthread_mutex_lock(&SARADCObject.mutex);

    if (SARADCObject.refCount <= 0)
    {
        printf("[SARADC][%s] SARADC has not been initialized, can not use API\n", __func__);
        result = SARADC_ERR_NOT_INITIALIZE;
        goto end;
    }

    if (SARADCObject.inUse)
    {
        printf("[SARADC][%s] SARADC is not idle, can not set XAIN information\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    if (result = mmpSAREnableXAIN(0x1 << hwPort)) goto end;

    wbSize = wbSize >> 3 << 3;
    if (result = mmpSARSetWriteBufferSize(wbSize)) goto end;

    writeBuffer = (uint8_t *)itpVmemAlignedAlloc(32, wbSize);
    if (!writeBuffer)
    {
        printf("[SARADC][%s] out of memory\n", __func__);
        result = SARADC_ERR_ALLOCATION_FAIL;
        goto end;
    }
    else
    {
        memset(writeBuffer, 0, wbSize);
        ithFlushDCacheRange(writeBuffer, wbSize);
    }

    if (result = mmpSARSetMEMBase(hwPort, writeBuffer)) goto end;

    if (result = mmpSARReadWritePointer(hwPort, &read_ptr)) goto end;

    if (result = mmpSARSetReadPointer(hwPort, read_ptr)) goto end;

    if (result = mmpSARSetAVGDetectRule(hwPort, 0, 0)) goto end;

    if (result = mmpSARFire()) goto end;

    //printf("[SARADC] SARADC is writing memory ");
    printf_count = 0;
    while (mmpSARIsOverwritingMEM(hwPort) != true)
    {
        if (printf_count % (1 * 100 * 1000) == 0)
        {
            //printf(".");
            //fflush(stdout);
        }
        printf_count++;
        usleep(1);
    }
    //printf("\n");
    //printf("[SARADC] finish!\n");

    if (result = mmpSARStop()) goto end;

    if (result = mmpSARReadWritePointer(hwPort, &write_ptr)) goto end;

    ithInvalidateDCacheRange((void *)writeBuffer, wbSize);

    if (result = mmpSARCollectOutput(writeBuffer, wbSize, write_ptr, avg)) goto end;

    if (result = mmpSARTableCalibrate(*avg, avg)) goto end;

end:
    if (writeBuffer) itpVmemFree((uint32_t)writeBuffer);

    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;

#endif
}

//=============================================================================
/**
 * Collect SARADC output.
 *
 * @param baseAddr    indicate memory base of XAIN.
 * @param wbSize      indicate Write buffer range for SARADC accessing.
 * @param wptr        indicate Write pointer offset that XAIN recorded.
 * @param avg         return average value that XAIN recorded.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARCollectOutput(
    uint8_t  *baseAddr,
    uint16_t wbSize,
    uint16_t wptr,
    uint16_t *avg)
{
    SARADC_RESULT result = SARADC_SUCCESS;
    uint16_t      wptr_offset = 0;
    uint16_t      unit_start = 0;
    uint8_t       temp = 0;
    uint16_t      buffer = 0;
    uint16_t      tail_check = 0;
    uint16_t      break_check = 0;
    uint16_t      minimum = 0x0;
    uint16_t      maximum = 0xfff;
    uint16_t      index = 0;
    uint32_t      average = 0x0;

    if (!baseAddr || wbSize < 0x40 || wbSize > 0xFFFF || !avg)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        goto end;
    }

    wbSize = wbSize >> 3 << 3;
    wptr_offset = wptr + 8;
    if (wptr_offset == wbSize)
    {
        wptr_offset = 0;
        tail_check = 1;
    }
    else if (wptr_offset > wbSize)
    {
        printf("[SARADC][%s][%d] WPTR offset is out of range\n", __func__, __LINE__);
        result = SARADC_ERR_WPTR_OUT_OF_RANGE;
        goto end;
    }

    while (break_check == 0)
    {
        if ((wptr_offset % SARADC_output_set_byte == SARADC_output_set_valid_byte) ||
                (wptr_offset % SARADC_output_set_byte == SARADC_output_set_valid_byte + 1))
            goto break_check_end;
        else if (wptr_offset % SARADC_output_set_byte == 0)
            unit_start = wptr_offset % SARADC_output_unit2_byte;

        temp = *(baseAddr + wptr_offset);
        if ((wptr_offset - unit_start) % SARADC_output_unit2_byte == 0)
        {
            buffer = temp;
        }
        else if ((wptr_offset - unit_start) % SARADC_output_unit2_byte == 1)
        {
            buffer |= (temp & 0xf) << 8;
            index++;
            //printf("[%d,%d]:%x\n", index++, wptr_offset, buffer);

            if (index == 1)
            {
                minimum = buffer;
                maximum = buffer;
                average += buffer;
            }
            else
            {
                if (buffer < minimum) minimum = buffer;
                else if (buffer > maximum) maximum = buffer;
                average += buffer;
            }

            buffer = (temp & 0xf0) >> 4;
        }
        else
        {
            buffer |= temp << 4;
            index++;
            //printf("[%d,%d]:%x\n", index++, wptr_offset, buffer);

            if (buffer < minimum) minimum = buffer;
            else if (buffer > maximum) maximum = buffer;

            average += buffer;
        }

break_check_end:
        wptr_offset++;
        if (tail_check)
        {
            if (wptr_offset == wbSize)
            {
                break_check = 1;
            }
            else if (wptr_offset > wbSize)
            {
                printf("[SARADC][%s][%d] WPTR offset is out of range\n", __func__, __LINE__);
                result = SARADC_ERR_WPTR_OUT_OF_RANGE;
                goto end;
            }
        }
        else
        {
            if (wptr_offset == wbSize)
            {
                wptr_offset = 0;
            }
            else if (wptr_offset == wptr + 8)
            {
                break_check = 1;
            }
            else if (wptr_offset > wbSize)
            {
                printf("[SARADC][%s][%d] WPTR offset is out of range\n", __func__, __LINE__);
                result = SARADC_ERR_WPTR_OUT_OF_RANGE;
                goto end;
            }
        }
    }

    *avg = (uint16_t)(average / index);
    //printf("[SARADC] %d outputs\n[SARADC] min:%x\n[SARADC] max:%x\n[SARADC] average:%x\n", index, minimum, maximum, *avg);

end:
    return result;
}

//=============================================================================
/**
 * SARADC table calibration.
 *
 * @param input     indicate real SARADC output.
 * @param output    return table calibration output which is calculated by input.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARTableCalibrate(
    uint16_t input,
    uint16_t *output)
{
    SARADC_RESULT result = SARADC_SUCCESS;
    float         ratio_temp = 0;
    uint16_t      real_index = 0;
    uint16_t      trans_index = 0;

    if (input > 0xFFF || !output)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        goto end;
    }

    for (real_index = 0; real_index < (sizeof(tREALMappingTable) / sizeof(tREALMappingTable[0])); real_index++)
    {
        if (tREALMappingTable[real_index].digitalOutput >= input)
            break;
    }

    if (real_index)
    {
        if (real_index == sizeof(tREALMappingTable) / sizeof(tREALMappingTable[0]))
        {
            printf("[SARADC][%s][%d] calibration fail\n", __func__, __LINE__);
            result = SARADC_ERR_INVALID_CALIBRATION;
            goto end;
        }

        ratio_temp = ((float)input - tREALMappingTable[real_index - 1].digitalOutput) /
                (tREALMappingTable[real_index].digitalOutput - tREALMappingTable[real_index - 1].digitalOutput);
    }

    for (trans_index = 0; trans_index < (sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0])); trans_index++)
    {
        if (tINTERPOLATIONMappingTable[trans_index].analogInput == tREALMappingTable[real_index].analogInput)
        {
            if (real_index)
            {
                *output = tINTERPOLATIONMappingTable[trans_index - 1].digitalOutput +
                        (uint16_t)(ratio_temp * (tINTERPOLATIONMappingTable[trans_index].digitalOutput - tINTERPOLATIONMappingTable[trans_index - 1].digitalOutput));
            }
            else
            {
                *output = tINTERPOLATIONMappingTable[trans_index].digitalOutput;
            }

            break;
        }
    }

    if (trans_index == sizeof(tINTERPOLATIONMappingTable) / sizeof(tINTERPOLATIONMappingTable[0]))
    {
        printf("[SARADC][%s][%d] calibration fail\n", __func__, __LINE__);
        result = SARADC_ERR_INVALID_CALIBRATION;
        goto end;
    }

end:
    return result;
}

