#ifndef ITH_INTR_H
#define ITH_INTR_H

/** @addtogroup ith ITE Hardware Library
 *  @{
 */
/** @addtogroup ith_intr Interrupt
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interrupt number definition.
 */
typedef enum
{
    ITH_INTR_SW0        = 0,    ///< Software (interrupt is invoking when write the software interrupt to 1)
    ITH_INTR_MC         = 1,    ///< Memory Controller
    ITH_INTR_DMA        = 2,    ///< DMA / DMA Terminal Count /DMA Error
    ITH_INTR_TIMER      = 4,    ///< Timer (for All)
    ITH_INTR_TIMER1     = 5,    ///< Timer 1
    ITH_INTR_TIMER2     = 6,    ///< Timer 2
    ITH_INTR_TIMER3     = 7,    ///< Timer 3
    ITH_INTR_TIMER4     = 8,    ///< Timer 4
    ITH_INTR_TIMER5     = 9,    ///< Timer 5
    ITH_INTR_TIMER6     = 10,   ///< Timer 6
    ITH_INTR_TIMER7     = 11,   ///< Timer 7
    ITH_INTR_TIMER8     = 12,   ///< Timer 8
    ITH_INTR_RTC        = 13,   ///< Real time clock
    ITH_INTR_RTCSEC     = 14,   ///< Real time clock second
    ITH_INTR_RTCMIN     = 15,   ///< Real time clock minute
    ITH_INTR_RTCHOUR    = 16,   ///< Real time clock hour
    ITH_INTR_RTCDAY     = 17,   ///< Real time day
    ITH_INTR_RTCALARM   = 18,   ///< Real time alarm
    ITH_INTR_WD         = 19,   ///< Watch dog
    ITH_INTR_CPU0       = 20,   ///< CPU0
    ITH_INTR_CPU1       = 21,   ///< CPU1
    ITH_INTR_ALTCPU     = 23,   ///< ALTCPU
    ITH_INTR_CMDQ       = 24,   ///< Command Queue 0
    ITH_INTR_CMDQ1      = 25,   ///< Command Queue 1
    ITH_INTR_2D         = 26,   ///< 2D
    ITH_INTR_GPIO       = 27,   ///< GPIO
    //ITH_INTR_VIDEO      = 28, ///< Video
    //ITH_INTR_JPEG       = 29, ///< Jpeg
    ITH_INTR_I2C0       = 28,      ///< I2C0
    ITH_INTR_I2C1       = 29,      ///< I2C1
    ITH_INTR_I2C2       = 30,      ///< I2C2
    ITH_INTR_I2C3       = 31,      ///< I2C2

    ITH_INTR_SW1        = 32 + 0,  ///< Software (interrupt is invoking when write the software interrupt register to 1) // 0
    ITH_INTR_USB0       = 32 + 1,  ///< USB0
    ITH_INTR_USB1       = 32 + 2,  ///< USB1
    ITH_INTR_MAC        = 32 + 3,  ///< MAC / MAC WOL
    ITH_INTR_WIEGAND0   = 32 + 4,  ///< Wiegand0
    ITH_INTR_WIEGAND1   = 32 + 5,  ///< Wiegand1
    ITH_INTR_UART0      = 32 + 6,  ///< UART 0
    ITH_INTR_UART1      = 32 + 7,  ///< UART 1
    ITH_INTR_UART2      = 32 + 8,  ///< UART 2
    ITH_INTR_UART3      = 32 + 9,  ///< UART 3
    ITH_INTR_UART4      = 32 + 10, ///< UART 4
    ITH_INTR_UART5      = 32 + 11, ///< UART 5
    ITH_INTR_JPEG       = 32 + 12, ///< Jpeg
    ITH_INTR_LCD        = 32 + 13, ///< LCD
    //ITH_INTR_ISP        = 32 + 14,     ///< ISP
    //ITH_INTR_IQ                    = 32 + 15,  ///< IQ
    ITH_INTR_ISP_CORE0  = 32 + 14, ///< ISP Core 0
    ITH_INTR_ISP_CORE1  = 32 + 15, ///< ISP Core 1
    ITH_INTR_I2S        = 32 + 16, ///< I2S
    ITH_INTR_DPU        = 32 + 17, ///< DPU
    ITH_INTR_DECOMPRESS = 32 + 18, ///< Decompress
    ITH_INTR_CAPTURE    = 32 + 19, ///< CCIR601/656 Capture
    ITH_INTR_TSI0       = 32 + 20, ///< TSI 0 Interface
    ITH_INTR_VIDEO      = 32 + 21, ///< Video
    //ITH_INTR_TSI1     = 32 + 21,  ///< TSI 1 Interface
    ITH_INTR_TSM        = 32 + 22, ///< TSM
    ITH_INTR_SARADC     = 32 + 23, ///< SARADC
    ITH_INTR_SD0        = 32 + 24, ///< SD/MMC 0
    ITH_INTR_SD1        = 32 + 25, ///< SD/MMC 1
    ITH_INTR_IRRX       = 32 + 26, ///< Remote IR RX
    ITH_INTR_IRTX       = 32 + 27, ///< Remote IR TX
    ITH_INTR_SSP0       = 32 + 28, ///< SSP 0
    ITH_INTR_SSP1       = 32 + 29, ///< SSP 1
    ITH_INTR_AXISPI     = 32 + 30, ///< AXISPI
    ITH_INTR_MIPI       = 32 + 31  ///< MIPI
} ITHIntr;

#include "ith/ith_intr_generic.h"

#ifdef __cplusplus
}
#endif

#endif // ITH_INTR_H
/** @} */ // end of ith_intr
/** @} */ // end of ith