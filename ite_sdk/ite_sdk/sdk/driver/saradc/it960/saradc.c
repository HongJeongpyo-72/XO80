#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#include "ite/ith.h"
#include "saradc/saradc.h"
#include "saradc/saradc_hw.h"

//=============================================================================
//                Constant Definition
//=============================================================================

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
    SARADC_MODE_AVG             opModeAVG;
    SARADC_MODE_STORE           opModeStore;
    SARADC_AMPLIFY_GAIN         driving;
    SARADC_CLK_DIV              clkDivider;
    uint16_t                    wbSize;
    SARADC_AVG_CAL_COUNT        avgCalCount;
    uint8_t                     trigStoreCount;
    bool                        inUse;
    XAIN_OBJECT                 XAINObjects[SARADC_XAIN_MAX_COUNT];
} SARADC_OBJECT;

typedef struct PORT_IO_MAPPING_ENTRY_TAG
{
    SARADC_PORT                 hwPort;
    SARADC_IO_MAPPING_ENTRY     mappingEntry;
} PORT_IO_MAPPING_ENTRY;

//=============================================================================
//                Global Data Definition
//=============================================================================
static SARADC_OBJECT SARADCObject =
{
    0,
    PTHREAD_MUTEX_INITIALIZER,
    CFG_SARADC_VALID_XAIN,
    CFG_SARADC_VALID_XAIN,
    SARADC_MODE_AVG_ENABLE,
    SARADC_MODE_STORE_AVG_ENABLE,
    SARADC_AMPLIFY_1X,
    SARADC_CLK_DIV_9,
    0,
    SARADC_AVG_CAL_COUNT_0,
    5,
    false,
    {0}
};

static PORT_IO_MAPPING_ENTRY tSARADCMappingTable[] =
{
    {SARADC_0, 17, 0}, {SARADC_1, 18, 0}, {SARADC_2, 19, 0}, {SARADC_3, 20, 0}
};

//=============================================================================
//                Private Function Definition
//=============================================================================
static void
_resetSARADCObject(
    SARADC_OBJECT *object)
{
    int i = 0;

    object->validXAIN = CFG_SARADC_VALID_XAIN;
    object->enableXAIN = CFG_SARADC_VALID_XAIN;
    object->opModeAVG = SARADC_MODE_AVG_ENABLE;
    object->opModeStore = SARADC_MODE_STORE_AVG_ENABLE;
    object->driving = SARADC_AMPLIFY_1X;
    object->clkDivider = SARADC_CLK_DIV_9;
    object->wbSize = 0;
    object->avgCalCount = SARADC_AVG_CAL_COUNT_0;
    object->trigStoreCount = 5;
    object->inUse = false;

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
_assignSARADCINTR(SARADC_OBJECT *object)
{
    uint8_t enableXAIN = object->enableXAIN;
    int     i = 0;

    for (i = 0; i < SARADC_XAIN_MAX_COUNT; i++)
    {
        if (enableXAIN & 0x1)
        {
            if (i == SARADC_0)
                SARADC_EnableXAININTR_Reg(SARADC_0);
            else if (i == SARADC_1)
                SARADC_EnableXAININTR_Reg(SARADC_1);
            else if (i == SARADC_2)
                SARADC_EnableXAININTR_Reg(SARADC_2);
            else if (i == SARADC_3)
                SARADC_EnableXAININTR_Reg(SARADC_3);
        }
        else
        {
            if (i == SARADC_0)
                SARADC_DisableXAININTR_Reg(SARADC_0);
            else if (i == SARADC_1)
                SARADC_DisableXAININTR_Reg(SARADC_1);
            else if (i == SARADC_2)
                SARADC_DisableXAININTR_Reg(SARADC_2);
            else if (i == SARADC_3)
                SARADC_DisableXAININTR_Reg(SARADC_3);
        }

        enableXAIN >>= 1;
    }
}

static void
_setSARADCGeneralReg(
    SARADC_OBJECT *object)
{
    SARADC_SetCLKDIV_Reg(object->clkDivider);
    SARADC_SetModeAVG_Reg(object->opModeAVG);
    SARADC_SetModeStore_Reg(object->opModeStore);
    SARADC_SetGAIN_Reg(object->driving);
    SARADC_SetValidXAIN_Reg(object->validXAIN);
    _assignSARADCINTR(object);
}

static void
_resetSARADCAllXAINReg(
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
            SARADC_ResetXAINEVENT_Reg(SARADC_0);
        }
        else if (i == SARADC_1)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_1, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_1);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_1);
            SARADC_ResetXAINEVENT_Reg(SARADC_1);
        }
        else if (i == SARADC_2)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_2, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_2);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_2);
            SARADC_ResetXAINEVENT_Reg(SARADC_2);
        }
        else if (i == SARADC_3)
        {
            SARADC_SetXAINRPTR_Reg(SARADC_3, object->XAINObjects[i].xainInfo.rptr);
            SARADC_ResetXAINOVERWT_Reg(SARADC_3);
            SARADC_ResetXAINAVGDetect_Reg(SARADC_3);
            SARADC_ResetXAINEVENT_Reg(SARADC_3);
        }
    }
}

//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * SARADC initialization.
 *
 * @param modeAVG            set average notification.
 * @param modeStore          set DRAM storing type.
 * @param amplifyDriving     determine XAIN driving.
 * @param divider            set clock divider of SARADC.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARInitialize(
    SARADC_MODE_AVG     modeAVG,
    SARADC_MODE_STORE   modeStore,
    SARADC_AMPLIFY_GAIN amplifyDriving,
    SARADC_CLK_DIV      divider)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (modeAVG < SARADC_MODE_AVG_DISABLE || modeAVG > SARADC_MODE_AVG_ENABLE ||
        modeStore < SARADC_MODE_STORE_DISABLE || modeStore > SARADC_MODE_STORE_AVG_ENABLE ||
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
        SARADCObject.opModeAVG = modeAVG;
        SARADCObject.opModeStore = modeStore;
        SARADCObject.driving = amplifyDriving;
        SARADCObject.clkDivider = divider;

        if (!_setSARADCGPIO(&SARADCObject))
        {
            printf("[SARADC][%s] _setSARADCGPIO() fail\n", __func__);
            result = SARADC_ERR_INVALID_GPIO_SETTING;
            goto end;
        }

        _setSARADCGeneralReg(&SARADCObject);
        _resetSARADCAllXAINReg(&SARADCObject);
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

    if (wbSize > CFG_RAM_SIZE)
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

    if (avgCalCount < SARADC_AVG_CAL_COUNT_0 || avgCalCount > SARADC_AVG_CAL_COUNT_3)
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
 * Set store average count of SARADC trigger.
 *
 * @param trigStoreCount    set store average count for SARADC reference, 0 means always
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetTRIGStoreCount(
    uint8_t trigStoreCount)
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
        printf("[SARADC][%s] SARADC is not idle, can not reset trigStoreCount\n", __func__);
        result = SARADC_ERR_NOT_IDLE;
        goto end;
    }

    SARADCObject.trigStoreCount = trigStoreCount;
    SARADC_SetTRIGStoreCount_Reg(SARADCObject.trigStoreCount);

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
    _resetSARADCAllXAINReg(&SARADCObject);
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

    SARADCObject.XAINObjects[hwPort].xainInfo.baseAddr = baseAddr;
    SARADC_SetXAINMEMBase_Reg(hwPort, SARADCObject.XAINObjects[hwPort].xainInfo.baseAddr);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

//=============================================================================
/**
 * Set XAIN trigger rule of SARADC.
 *
 * @param hwPort           indicate which XAIN will be applied.
 * @param trigAVG          determine average trigger type of XAIN.
 * @param maxAVGTrigger    determine max trigger rule of XAIN.
 * @param minAVGTrigger    determine min trigger rule of XAIN.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARSetAVGTriggerRule(
    SARADC_PORT     hwPort,
    SARADC_TRIG_AVG trigAVG,
    uint16_t        maxAVGTrigger,
    uint16_t        minAVGTrigger)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3 ||
            trigAVG < SARADC_TRIG_AVG_ALWAYS || trigAVG > SARADC_TRIG_AVG_LEVEL ||
            maxAVGTrigger > 0xFFF || minAVGTrigger > 0xFFF)
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

    SARADCObject.XAINObjects[hwPort].xainInfo.trigAVG       = trigAVG;
    SARADCObject.XAINObjects[hwPort].xainInfo.maxAVGTrigger = maxAVGTrigger;
    SARADCObject.XAINObjects[hwPort].xainInfo.minAVGTrigger = minAVGTrigger;
    SARADC_SetXAINTriggerRule_Reg(hwPort,
            SARADCObject.XAINObjects[hwPort].xainInfo.trigAVG,
            SARADCObject.XAINObjects[hwPort].xainInfo.maxAVGTrigger,
            SARADCObject.XAINObjects[hwPort].xainInfo.minAVGTrigger);

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
 * Read SARADC average register.
 *
 * @param hwPort    indicate which XAIN will be applied.
 * @param avg       return average value that XAIN recorded.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARReadAVGREG(
    SARADC_PORT hwPort,
    uint16_t    *avg)
{
    SARADC_RESULT result = SARADC_SUCCESS;

    if (hwPort < SARADC_0 || hwPort > SARADC_3)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        result = SARADC_ERR_INVALID_INPUT_PARAM;
        return result;
    }

    SARADC_ReadXAINAVGREG_Reg(hwPort, avg);

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
 * Check Event occurrence Interrupt of SARADC.
 *
 * @param hwPort    indicate which XAIN will be checked.
 * @return true if event has been occurred, false otherwise.
 */
//=============================================================================
bool
mmpSARIsEventOccurrence(
    SARADC_PORT hwPort)
{
    if (hwPort < SARADC_0 || hwPort > SARADC_3)
    {
        printf("[SARADC][%s] parameter fail\n", __func__);
        return false;
    }

    return SARADC_IsXAINEVENTOccurrence_Reg(hwPort);
}

//=============================================================================
/**
 * Reset SARADC XAIN notification.
 *
 * @param hwPort    indicate which XAIN will be checked.
 * @return SARADC_SUCCESS if succeed, error codes of SARADC_ERR otherwise.
 */
//=============================================================================
SARADC_RESULT
mmpSARResetXAINNotification(
    SARADC_PORT hwPort)
{
    SARADC_RESULT result = SARADC_SUCCESS;
    int           i = hwPort;

    if (hwPort < SARADC_0 || hwPort > SARADC_3)
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

    SARADC_ResetXAINOVERWT_Reg(i);
    SARADC_ResetXAINAVGDetect_Reg(i);
    SARADC_ResetXAINEVENT_Reg(i);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
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
    _assignSARADCINTR(&SARADCObject);
    SARADC_SetValidXAIN_Reg(SARADCObject.enableXAIN);

end:
    pthread_mutex_unlock(&SARADCObject.mutex);

    return result;
}

