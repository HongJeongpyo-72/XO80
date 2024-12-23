/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MT7686_H__
#define __MT7686_H__

#define   __I     volatile             /*!< Defines 'read only' permissions                 */
#define   __O     volatile             /*!< Defines 'write only' permissions                */
#define   __IO    volatile             /*!< Defines 'read / write' permissions              */


//#define __CM4_REV                 0x0001  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1       /*!< MT7686 provides an MPU                        */
#define __NVIC_PRIO_BITS          5       /*!< MT7686 uses 5 Bits for the Priority Levels    */
//#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */

#define CM4_HIF_IRQ               22

typedef enum IRQn {
    /****  CM4 internal exceptions  **********/
    Reset_IRQn                  = -15,  /* Reset */
    NonMaskableInt_IRQn         = -14,  /* NMI */
    HardFault_IRQn              = -13,  /* HarFault */
    MemoryManagement_IRQn       = -12,  /* Memory Management */
    BusFault_IRQn               = -11,  /* Bus Fault  */
    UsageFault_IRQn             = -10,  /* Usage Fault */
    SVC_IRQn                    = -5,   /* SV Call*/
    DebugMonitor_IRQn           = -4,   /* Debug Monitor */
    PendSV_IRQn                 = -2,   /* Pend SV */
    SysTick_IRQn                = -1,   /* System Tick */

    /****  MT7686 specific external/peripheral interrupt ****/
    DMA_IRQn                    =  0,
    SPI_MST_IRQn                =  1,
    SPI_SLV_IRQn                =  2,
    SDIO_SLV_IRQn               =  3,
    SDIO_MST_IRQn               =  4,
    TRNG_IRQn                   =  5,
    CRYPTO_IRQn                 =  6,
    UART0_IRQn                  =  7,
    UART1_IRQn                  =  8,
    UART2_IRQn                  =  9,
    AUDIO_IRQn                  =  10,
    ASYS_IRQn                   =  11,
    I2C0_IRQn                   =  12,
    I2C1_IRQn                   =  13,
    RTC_IRQn                    =  14,
    GPTimer_IRQn                =  15,
    SPM_IRQn                    =  16,
    RGU_IRQn                    =  17,
    PMU_DIG_IRQn                =  18,
    EINT_IRQn                   =  19,
    SFC_IRQn                    =  20,
    BTIF_IRQn                   =  21,
    CONNSYS0_IRQn               =  22,
    CONNSYS1_IRQn               =  23,
    XO_IRQn                     =  24,
    Reserved1_IRQn              =  25,
    SW_ISR0_IRQn                =  26,
    SW_ISR1_IRQn                =  27,
    SW_ISR2_IRQn                =  28,
    SW_ISR3_IRQn                =  29,
    Reserved2_IRQn              =  30,
    Reserved3_IRQn              =  31,
    IRQ_NUMBER_MAX              =  32
} IRQn_Type;

typedef IRQn_Type hal_nvic_irq_t;

//#include "core_cm4.h"                  /* Core Peripheral Access Layer */

/* === Fix me: Port from mt7687.h ...Michael ========*/
#define CM4_HIF_IRQ               22
#define DEFAULT_PRI             5
#define HIGHEST_PRI             9
#define CM4_HIF_PRI   DEFAULT_PRI
/* ================================================== */

//#include "system_mt7686.h"

/* MT7686 private config */
#define VECTBL_BASE (0x0)

/* PD APB Peripheral */
#define TRNG_BASE            0xA0010000 /*TRNG*/
#define DMA_BASE             0xA0020000 /*DMA Controller*/
#define INFRA_MBIST_BASE     0xA0030000 /*INFRA MBIST CONFIG*/
#define SFC_BASE             0xA0040000 /*Serial Flash Contoller*/
#define EMI_BASE             0xA0050000 /*External Memory Interface*/
#define CRYPTO_BASE          0xA0060000 /*Crypto_Engine*/
#define AUDIO_BASE           0xA0070000 /*ADUIO TOP*/
#define ASYS_BASE            0xA0080000 /*ASYS TOP*/
#define BTIF_BASE            0xA0090000 /*Bluetooth interface*/
#define SPI_MASTER_BASE      0xA00A0000 /*SPI_MASTER*/
#define SPI_SLAVE_BASE       0xA00B0000 /*SPI_SLAVE*/
#define UART0_BASE           0xA00C0000 /*UART 0*/
#define UART1_BASE           0xA00D0000 /*UART 1*/
#define UART2_BASE           0xA00E0000 /*UART 2*/
#define I2C_0_BASE           0xA0100000 /*I2C_0*/
#define I2C_1_BASE           0xA0110000 /*I2C_1*/
#define AUXADC_BASE          0xA0120000 /*Auxiliary ADC Unit*/

/* PD AHB Peripheral */
#define DMA_AHB_BASE         0xA1000000 /*PD DMA AHB*/
#define AUDIO_AHB_BASE       0xA1010000 /*PD ADUIO AHB*/
#define ASYS_AHB_BASE        0xA1020000 /*PD ASYS AHB*/
#define SDIO_MASTER_BASE     0xA1030000 /*SDIO MASTER*/
#define SDIO_SLAVE_BASE      0xA1040000 /*SDIO SLAVE*/

/* PD CMSYS AHB Peripheral */
#define CMSYS_CFG_BASE       0xE0100000 /*cmsys_cfg*/
#define CMSYS_CFG_EXT_BASE   0xE00FE000 /*cmsys_cfg_ext*/
#define CMSYS_MBIST_BASE     0xE0110000 /*cmsys_mbist*/
#define CM4_MCU_BASE         0xE0130000 /*Reserved for MDM*/
#define CMSYS_L1CACHE_BASE   0xE0180000 /*cmsys_l1cache*/

/* AO APB Peripheral */
#define CONFIG_BASE          0xA2010000 /*Configuration Registers(Clock, Power Down, Version and Reset)*/
#define TOP_MISC_CFG_BASE    0xA2010000 /*Configuration Registers(Clock, Power Down, Version and Reset)*/
#define CKSYS_BASE           0xA2020000 /*BBPLL control*/
#define CKSYS_XO_CLK_BASE    0xA2030000 /*XPLL control*/
#define BBPLL_CTRL_BASE      0xA2040000 /*26M, clk switch control (clk divider related setting)*/
#define XPLL_CTRL_BASE       0xA2050000 /*26M, top clk control (dcm, cg)*/
#define XTAL_CTRL_BASE       0xA2060000 /*26M, RF Xtal control register*/
#define PMU_BASE             0xA2070000 /*PMU configure*/
#define RTC_BASE             0xA2080000 /*Real Time Clock*/
#define RGU_BASE             0xA2090000 /*Reset Generation Unit*/
#define EFUSE_BASE           0xA20A0000 /*EFUSE*/
#define GPIO_BASE            0xA20B0000 /*General Purpose Inputs/Outputs*/
#define IO_CFG_0_BASE        0xA20C0000 /*io_cfg_0*/
#define IO_CFG_1_BASE        0xA20D0000 /*io_cfg_1*/
#define SPM_BASE             0xA20F0000 /*System Power Manager */
#define EINT_BASE            0xA2100300 /*External Interrupts*/
#define GPT_BASE             0xA2110000 /*General Purpose Timer */
#define PWM0_BASE            0xA2120000 /*Pulse-Width Modulation Outputs 0*/
#define PWM1_BASE            0xA2130000 /*Pulse-Width Modulation Outputs 1*/
#define PWM2_BASE            0xA2140000 /*Pulse-Width Modulation Outputs 2*/
#define PWM3_BASE            0xA2150000 /*Pulse-Width Modulation Outputs 3*/
#define PWM4_BASE            0xA2160000 /*Pulse-Width Modulation Outputs 4*/
#define PWM5_BASE            0xA2170000 /*Pulse-Width Modulation Outputs 5*/
#define CKSYS_BUS_CLK_BASE   0xA21D0000 /*96M, top clk control (dcm, cg)*/
#define CM4_CFG_PRI_BASE     0xA21E0000 /*CM4 always-on CFG*/
#define INFRA_CFG_BASE       0xA21F0000 /*bus configuration registers*/

#define CM4_CONFIG_BASE      0x83000000 /*config CM4 base address*/


/* structure type of top_misc_cfg
 */

/* structure type of CMSYS_CFG
 */

/* Total register field bit definition */
/* Bit definition for Interrupt Enable Register */
#define UART_IER_ETBEI_OFFSET   (0) /* generated if RBR has data or the RX Trigger Level is reached */
#define UART_IER_ETBEI_MASK     (0x1<<UART_IER_ETBEI_OFFSET)

/* Bit definition for Interrupt Identification Register */
#define UART_IIR_ID_OFFSET      (0) /* Interrupt Source Identification */
#define UART_IIR_ID_MASK        (0x3F<<UART_IIR_ID_OFFSET)

/* Bit definition for FIFO Control Register */
#define UART_FCR_CLRT_OFFSET    (24) /* control bit to clear tx fifo */
#define UART_FCR_CLRT_MASK      (0x1<<UART_FCR_CLRT_OFFSET)
#define UART_FCR_CLRR_OFFSET    (16) /* control bit to clear rx fifo */
#define UART_FCR_CLRR_MASK      (0x1<<UART_FCR_CLRR_OFFSET)
#define UART_FCR_RFTL_OFFSET    (10) /* RX FIFO trigger threshold */
#define UART_FCR_RFTL_MASK      (0x3<<UART_FCR_RFTL_OFFSET)
#define UART_FCR_TFTL_OFFSET    (8) /* TX FIFO trigger threshold */
#define UART_FCR_TFTL_MASK      (0x3<<UART_FCR_TFTL_OFFSET)
#define UART_FCR_FIFOE_OFFSET   (0) /* Enable Rx & Tx FIFOs */
#define UART_FCR_FIFOE_MASK     (0x1<<UART_FCR_FIFOE_OFFSET)

/* Bit definition for Line Control Register */
#define UART_LCR_SB_OFFSET      (8) /* Set Break */
#define UART_LCR_SB_MASK        (0x1<<UART_LCR_SB_OFFSET)
#define UART_LCR_PARITY_OFFSET  (3) /* Parity */
#define UART_LCR_PARITY_MASK    (0x7<<UART_LCR_PARITY_OFFSET)
#define UART_LCR_STB_OFFSET     (2) /* Number of STOP bits */
#define UART_LCR_STB_MASK       (0x1<<UART_LCR_STB_OFFSET)
#define UART_LCR_WORD_OFFSET    (0) /* Word Length */
#define UART_LCR_WORD_MASK      (0x3<<UART_LCR_WORD_OFFSET)

/* Bit definition for Modem Control Register */
#define UART_MCR_XOFF_STATUS_OFFSET (16) /* whether an XON character is received */
#define UART_MCR_XOFF_STATUS_MASK   (0x1<<UART_MCR_XOFF_STATUS_OFFSET)
#define UART_MCR_LOOP_OFFSET        (8) /* Loop-back control bit */
#define UART_MCR_LOOP_MASK          (0x1<<UART_MCR_LOOP_OFFSET)
#define UART_MCR_RTS_OFFSET         (0) /* Controls the state of the output RTS, even in loop mode */
#define UART_MCR_RTS_MASK           (0x1<<UART_MCR_RTS_OFFSET)

/* Bit definition for XON & XOFF character Register */
#define UART_XON_XOFF_XONCHAR_OFFSET (8) /* define XON character */
#define UART_XON_XOFF_XONCHAR_MASK   (0xff<<UART_XON_XOFF_XONCHAR_OFFSET)
#define UART_XON_XOFF_XOFFCHAR_OFFSET (0) /* define XOFF character */
#define UART_XON_XOFF_XOFFCHAR_MASK   (0xff<<UART_XON_XOFF_XOFFCHAR_OFFSET)

/* Bit definition for Line Status Register */
#define UART_LSR_FIFOERR_OFFSET (7) /* RX FIFO Error Indicator */
#define UART_LSR_FIFOERR_MASK   (0x1<<UART_LSR_FIFOERR_OFFSET)
#define UART_LSR_TEMT_OFFSET    (6) /* TX Holding Register (or TX FIFO) and the TX Shift Register are empty */
#define UART_LSR_TEMT_MASK      (0x1<<UART_LSR_TEMT_OFFSET)
#define UART_LSR_THRE_OFFSET    (5) /* Indicates if there is room for TX Holding Register or TX FIFO is reduced to its Trigger Level */
#define UART_LSR_THRE_MASK      (0x1<<UART_LSR_THRE_OFFSET)
#define UART_LSR_BI_OFFSET  (4) /* Break Interrupt */
#define UART_LSR_BI_MASK    (0x1<<UART_LSR_BI_OFFSET)
#define UART_LSR_FE_OFFSET  (3) /* Framing Error */
#define UART_LSR_FE_MASK    (0x1<<UART_LSR_FE_OFFSET)
#define UART_LSR_PE_OFFSET  (2) /* Parity Error */
#define UART_LSR_PE_MASK    (0x1<<UART_LSR_PE_OFFSET)
#define UART_LSR_OE_OFFSET  (1) /* Overrun Error */
#define UART_LSR_OE_MASK    (0x1<<UART_LSR_OE_OFFSET)
#define UART_LSR_DR_OFFSET  (0) /* Data Ready */
#define UART_LSR_DR_MASK    (0x1<<UART_LSR_DR_OFFSET)

/* Bit definition for Auto Baud Detect Enable Register */
#define UART_AUTOBAUD_CON_SLEEP_ACK_SEL_OFFSET   (16) /* Sleep ack select when autobaud_en */
#define UART_AUTOBAUD_CON_SLEEP_ACK_SEL_MASK     (0x1<<UART_AUTOBAUD_CON_SLEEP_ACK_SEL_OFFSET)
#define UART_AUTOBAUD_CON_AUTOBAUD_SEL_OFFSET    (8) /* Auto-baud select */
#define UART_AUTOBAUD_CON_AUTOBAUD_SEL_MASK      (0x1<<UART_AUTOBAUD_CON_AUTOBAUD_SEL_OFFSET)
#define UART_AUTOBAUD_CON_AUTOBAUD_EN_OFFSET     (0) /* Auto-baud enable signal */
#define UART_AUTOBAUD_CON_AUTOBAUD_EN_MASK       (0x1<<UART_AUTOBAUD_CON_AUTOBAUD_EN_OFFSET)

/* Bit definition for High Speed Mode Register */
#define UART_HIGHSPEED_SPEED_OFFSET (0) /* UART sample counter base */
#define UART_HIGHSPEED_SPEED_MASK   (0x3<<UART_HIGHSPEED_SPEED_OFFSET)

/* Bit definition for Sample counter & sample point register */
#define UART_SAMPLE_REG_SAMPLE_POINT_OFFSET (8) /* Sample point */
#define UART_SAMPLE_REG_SAMPLE_POINT_MASK   (0xff<<UART_SAMPLE_REG_SAMPLE_POINT_OFFSET)
#define UART_SAMPLE_REG_SAMPLE_COUNT_OFFSET (0) /* Sample counter */
#define UART_SAMPLE_REG_SAMPLE_COUNT_MASK   (0xff<<UART_SAMPLE_REG_SAMPLE_COUNT_OFFSET)

/* Bit definition for Auto Baud Monitor Register */
#define UART_AUTOBAUD_REG_BAUD_STAT_OFFSET  (8) /* frame detect result */
#define UART_AUTOBAUD_REG_BAUD_STAT_MASK    (0x0F<<UART_AUTOBAUD_REG_BAUD_STAT_OFFSET)
#define UART_AUTOBAUD_REG_BAUD_RATE_OFFSET  (0) /* baudrate detect result */
#define UART_AUTOBAUD_REG_BAUD_RATE_MASK    (0x0F<<UART_AUTOBAUD_REG_BAUD_RATE_OFFSET)

/* Bit definition for Clock Rate Fix Register */
#define UART_RATEFIX_RATEFIX_OFFSET (16) /* System clock rate for Tx/Rx */
#define UART_RATEFIX_RATEFIX_MASK   (0x1<<UART_RATEFIX_RATEFIX_OFFSET)
#define UART_RATEFIX_AUTOBAUD_RATEFIX_OFFSET (8) /* System clock rate for auto-baud detection */
#define UART_RATEFIX_AUTOBAUD_RATEFIX_MASK   (0x1<<UART_RATEFIX_AUTOBAUD_RATEFIX_OFFSET)
#define UART_RATEFIX_AUTOBAUD_SAMPLE_OFFSET (0) /* Clock division for auto-baud detection */
#define UART_RATEFIX_AUTOBAUD_SAMPLE_MASK   (0x3f<<UART_RATEFIX_AUTOBAUD_SAMPLE_OFFSET)

/* Bit definition for Guard time added register */
#define UART_GUARD_GUARD_EN_OFFSET  (4) /* Guard interval add enable signal */
#define UART_GUARD_GUARD_EN_MASK    (0x1<<UART_GUARD_GUARD_EN_OFFSET)
#define UART_GUARD_GUARD_CNT_OFFSET (0) /* Guard interval count value */
#define UART_GUARD_GUARD_CNT_MASK   (0x0F<<UART_GUARD_GUARD_CNT_OFFSET)

/* Bit definition for Escape character register */
#define UART_ESCAPE_REG_EN_OFFSET     (8) /* Add escape character when TX and remove escape character when RX */
#define UART_ESCAPE_REG_EN_MASK       (0x1<<UART_ESCAPE_REG_EN_OFFSET)
#define UART_ESCAPE_REG_CHAR_OFFSET   (0) /* define escape character */
#define UART_ESCAPE_REG_CHAR_MASK     (0xff<<UART_ESCAPE_REG_CHAR_OFFSET)

/* Bit definition for Sleep enable register */
#define UART_SLEEP_EN_OFFSET    (0) /* Sleep enable bit */
#define UART_SLEEP_EN_MASK      (0x1<<UART_SLEEP_EN_OFFSET)


/* Bit definition for Rx Trigger Address register */
#define UART_RXTRIG_OFFSET (0) /* When {rtm,rtl}=2'b11, The Rx FIFO threshold will be Rxtrig */
#define UART_RXTRIG_MASK   (0x0F<<UART_RXTRIG_OFFSET)

/* Bit definition for Fractional Divider Address register */
#define UART_FRACDIV_OFFSET   (0) /* Add sampling count when in state stop to parity */
#define UART_FRACDIV_MASK     (0x3ff<<UART_FRACDIV_OFFSET)

/* Bit definition for Rx timeout mode control register */
#define UART_RX_TO_CON_TO_CNT_AUTORST_OFFSET (8) /* Time-out counter auto reset */
#define UART_RX_TO_CON_TO_CNT_AUTORST_MASK   (0x1<<UART_RX_TO_CON_TO_CNT_AUTORST_OFFSET)
#define UART_RX_TO_CON_RX_TO_MODE_OFFSET (0) /* Rx timeout mode */
#define UART_RX_TO_CON_RX_TO_MODE_MASK   (0x1<<UART_RX_TO_CON_RX_TO_MODE_OFFSET)


/* Bit definition from BTA view */
/* Bit definition for Interrupt Enable Register */
#define UART_IER_CTSI_OFFSET    (1) /* generated when a rising edge is detected on the CTS modem control line */
#define UART_IER_CTSI_MASK      (0x1<<UART_IER_CTSI_OFFSET)
#define UART_IER_RTSI_OFFSET    (0) /* generated when a rising edge is detected on the RTS modem control line */
#define UART_IER_RTSI_MASK      (0x1<<UART_IER_RTSI_OFFSET)
#define UART_IER_XOFFI_OFFSET   (0) /* generated when an XOFF character is received */
#define UART_IER_XOFFI_MASK     (0x1<<UART_IER_XOFFI_OFFSET)
#define UART_IER_ELSI_OFFSET    (1) /* generated if BI, FE, PE or OE (LSR[4:1]) becomes set */
#define UART_IER_ELSI_MASK      (0x1<<UART_IER_ELSI_OFFSET)
#define UART_IER_ERBFI_OFFSET   (0) /* generated if the THR is empty or TX FIFO reduced to its Trigger Level */
#define UART_IER_ERBFI_MASK     (0x1<<UART_IER_ERBFI_OFFSET)

/* Bit definition for Enhanced Feature Register */
#define UART_EFR_SEND_XON_OFFSET    (0) /* Send XON */
#define UART_EFR_SEND_XON_MASK      (0x1<<UART_EFR_SEND_XON_OFFSET)
#define UART_EFR_SEND_XOFF_OFFSET   (0) /* Send XOFF */
#define UART_EFR_SEND_XOFF_MASK     (0x1<<UART_EFR_SEND_XOFF_OFFSET)
#define UART_EFR_HW_TX_FLOWCTRL_OFFSET (1) /* Hardware flow control */
#define UART_EFR_HW_TX_FLOWCTRL_MASK   (0x1<<UART_EFR_HW_TX_FLOWCTRL_OFFSET)
#define UART_EFR_HW_RX_FLOWCTRL_OFFSET (0) /* Hardware flow control */
#define UART_EFR_HW_RX_FLOWCTRL_MASK   (0x1<<UART_EFR_HW_RX_FLOWCTRL_OFFSET)
#define UART_EFR_SW_TX_FLOWCTRL_OFFSET (1) /* Software flow control */
#define UART_EFR_SW_TX_FLOWCTRL_MASK   (0x1<<UART_EFR_SW_TX_FLOWCTRL_OFFSET)
#define UART_EFR_SW_RX_FLOWCTRL_OFFSET (0) /* Software flow control */
#define UART_EFR_SW_RX_FLOWCTRL_MASK   (0x1<<UART_EFR_SW_RX_FLOWCTRL_OFFSET)

/* Bit definition for DMA mode control register */
#define UART_DMA_CON_FIFO_LSR_SEL_OFFSET (0) /* fifo lsr mode selection */
#define UART_DMA_CON_FIFO_LSR_SEL_MASK   (0x1<<UART_DMA_CON_FIFO_LSR_SEL_OFFSET)
#define UART_DMA_CON_TX_DMA_EN_OFFSET    (0) /* TX_DMA mechanism enable signal */
#define UART_DMA_CON_TX_DMA_EN_MASK      (0x1<<UART_DMA_CON_TX_DMA_EN_OFFSET)
#define UART_DMA_CON_RX_DMA_EN_OFFSET    (0) /* RX_DMA mechanism enable signal */
#define UART_DMA_CON_RX_DMA_EN_MASK      (0x1<<UART_DMA_CON_RX_DMA_EN_OFFSET)



#define UART_IIR_NONE      (0x1<<UART_IIR_ID_OFFSET) /* No interrupt pending */
#define UART_IIR_LSR       (0x6<<UART_IIR_ID_OFFSET) /* Line Status Interrupt */
#define UART_IIR_RDT       (0xC<<UART_IIR_ID_OFFSET) /* RX Data Timeout */
#define UART_IIR_RDR       (0x4<<UART_IIR_ID_OFFSET) /* RX Data Received */
#define UART_IIR_THRE      (0x2<<UART_IIR_ID_OFFSET) /* TX Holding Register Empty */
#define UART_IIR_RCVXOFF   (0x10<<UART_IIR_ID_OFFSET) /* Software Flow Control */
#define UART_IIR_CTSRTS    (0x20<<UART_IIR_ID_OFFSET) /* Hardware Flow Control */
#define UART_FCR_RXTRIG_1       (0x0<<UART_FCR_RFTL_OFFSET) /* RX FIFO trigger = 1 */
#define UART_FCR_RXTRIG_6       (0x1<<UART_FCR_RFTL_OFFSET) /* RX FIFO trigger = 6 */
#define UART_FCR_RXTRIG_12      (0x2<<UART_FCR_RFTL_OFFSET) /* RX FIFO trigger = 12 */
#define UART_FCR_RXTRIG_USER    (0x3<<UART_FCR_RFTL_OFFSET) /* RX FIFO trigger = RXTRIG */
#define UART_FCR_TXTRIG_1       (0x0<<UART_FCR_TFTL_OFFSET) /* RX FIFO trigger = 1 */
#define UART_FCR_TXTRIG_4       (0x1<<UART_FCR_TFTL_OFFSET) /* RX FIFO trigger = 4 */
#define UART_FCR_TXTRIG_8       (0x2<<UART_FCR_TFTL_OFFSET) /* RX FIFO trigger = 8 */
#define UART_FCR_TXTRIG_14      (0x3<<UART_FCR_TFTL_OFFSET) /* RX FIFO trigger = 14 */
#define UART_LCR_PAR_NONE      (0x4<<UART_LCR_PARITY_OFFSET) /* Odd Even Parity */
#define UART_LCR_PAR_ODD       (0x1<<UART_LCR_PARITY_OFFSET) /* Even Parity Select */
#define UART_LCR_PAR_EVEN      (0x0<<UART_LCR_PARITY_OFFSET) /* Even Parity Select */
#define UART_LCR_STB_1      (0x0<<UART_LCR_STB_OFFSET) /* 1 STOP bits */
#define UART_LCR_STB_2      (0x1<<UART_LCR_STB_OFFSET) /* 2 STOP bits */
#define UART_LCR_WORD_5BITS (0x0<<UART_LCR_WORD_OFFSET) /* Word Length = 5BITS */
#define UART_LCR_WORD_6BITS (0x1<<UART_LCR_WORD_OFFSET) /* Word Length = 6BITS */
#define UART_LCR_WORD_7BITS (0x2<<UART_LCR_WORD_OFFSET) /* Word Length = 7BITS */
#define UART_LCR_WORD_8BITS (0x3<<UART_LCR_WORD_OFFSET) /* Word Length = 8BITS */
#define UART_HIGHSPEED_SPEED_MODE0  (0x0<<UART_HIGHSPEED_SPEED_OFFSET)  /* baud_rate = system clock frequency/16/{DLH, DLL} */
#define UART_HIGHSPEED_SPEED_MODE1  (0x1<<UART_HIGHSPEED_SPEED_OFFSET)  /* baud_rate = system clock frequency/8/{DLH, DLL} */
#define UART_HIGHSPEED_SPEED_MODE2  (0x2<<UART_HIGHSPEED_SPEED_OFFSET)  /* baud_rate = system clock frequency/14/{DLH, DLL} */
#define UART_HIGHSPEED_SPEED_MODE3  (0x3<<UART_HIGHSPEED_SPEED_OFFSET)  /* baud_rate = system clock frequency/(sampe_count+1)/{DLM, DLL} */


/************************ EMI register definition start line  *******************************
 */



/*EMI register definition start*/

/*
__IO uint8_t  RESERVED5[16];
__IO uint32_t EMI_IOA;
__IO uint8_t  RESERVED13[4];
__IO uint32_t EMI_IOB;

*/
/************************ end register definition start line  *******************************
 */


/* *************************flash hardware definition start line**********************************
*/

/*flash register structure definition*/



/* *************************flash hardware definition end line***********************************/

/* *************************dma hardware definition start line**********************************
*/



/*general dma register definition
*/




/************************ dma end register definition end line  *******************************
 */



/************************ GPT end register definition start line  *******************************
 */
/************************ GPT end register definition end line  *******************************
 */




/* structure type to access the CACHE register
 */



/* structure type to access the ADC register
 */




/* CLK_SOURCE_SEL used offset and mask definitions */


/* SPI master register definitions begin. */

/* register list defination */


/* SPIM_CTRL0 register definitions */


/* SPI master register definitions end. */


/* SPI slaver register definitions begin. */

/* register list defination */

/* Bit definition for SPIS Control Register */



/*************************** EINT definition start line  *******************************/


/*************************** EINT definition end line  ********************************/

/************************ RTC register definition start line  *******************************
 */


/************************ RTC register definition end line  *******************************
 */

/* ************************* CRYPTO hardware definition start line**********************************
*/


/************************ CRYPTO register definition end line  *******************************
 */

/* *************************trng hardware definition start line**********************************
*/



/* *************************trng hardware definition end line**********************************
*/

/************************ WDT register definition start line  *******************************
 */

/************************ WDT register definition end line  *******************************
 */
 /* *************************pwm hardware definition start line**********************************
*/

/*pwm register structure definition*/

/*the bit value in PWM  control  register*/



/* *************************pwm hardware definition end line**********************************
*/


/************************ SDIO_SLAVE register definition start line  *******************************
 */

typedef struct {
    /****************************************************************/
    /*************This part define FW global register****************/
    /****************************************************************/
    __IO uint32_t HGFCR;          /*offset:0x00, HIF Global Firmware Configuration Register.*/
    __IO uint32_t HGFISR;         /*offset:0x04, HIF Global Firmware Interrupt Status Register.*/
    __IO uint32_t HGFIER;         /*offset:0x08, HIF Global Firmware Interrupt Enable Register.*/
    __IO uint32_t rsv0;
    __IO uint32_t HSDBDLSR;       /*offset:0x10, HIF SDIO Bus Delay Selection Register.*/
    __IO uint32_t HSDLSR;         /*offset:0x14, HIF SRAM Delay Selection Register.*/
    __IO uint32_t HCSDCR;         /*offset:0x18, HIF Clock Stop Detection register.*/
    __IO uint32_t HGH2DR;         /*offset:0x1c, HIF Global Host to Device Register.*/
    __IO uint32_t HDBGCR;         /*offset:0x20, HIF Debug Control Register.*/
    __IO uint32_t rsv1[2];
    __IO uint32_t FWDSIOCR;   /*offset:0x2c, DS Pad Macro IO Control Register.*/
    __IO uint32_t HGTMTCR;    /*offset:0x30, Test Mode Trigger Control Register.*/
    __IO uint32_t HGTMCR;     /*offset:0x34, Test Mode Control Register.*/
    __IO uint32_t HGTMDPCR0;     /*offset:0x38, Test Mode Data Pattern Control Register 0.*/
    __IO uint32_t HGTMDPCR1;     /*offset:0x3c, Test Mode Data Pattern Control Register 1.*/
    __IO uint32_t FWCLKIOCR_T28LP;     /*offset:0x40, Clock Pad Macro IO Control Register.*/
    __IO uint32_t FWCMDIOCR_T28LP;     /*offset:0x44, Command Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT0IOCR_T28LP;     /*offset:0x48, Data 0 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT1IOCR_T28LP;     /*offset:0x4c, Data 1 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT2IOCR_T28LP;     /*offset:0x50, Data 2 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT3IOCR_T28LP;     /*offset:0x54, Data 3 Pad Macro IO Control Register.*/
    __IO uint32_t FWCLKDLYCR;     /*offset:0x58, Clock Pad Macro Delay Chain Control Register.*/
    __IO uint32_t FWCMDDLYCR;     /*offset:0x5c, Command Pad Macro Delay Chain Control Register.*/
    __IO uint32_t FWODATDLYCR;     /*offset:0x60, SDIO Output Data Delay Chain Control Register.*/
    __IO uint32_t FWIDATDLYCR1;     /*offset:0x64, SDIO Input Data Delay Chain Control Register 1.*/
    __IO uint32_t FWIDATDLYCR2;     /*offset:0x68, SDIO Input Data Delay Chain Control Register 2.*/
    __IO uint32_t FWILCHCR;     /*offset:0x6c, SDIO Input Data Latch Time Control Register.*/
    __IO uint32_t rsv2[36];

    /****************************************************************/
    /*************This part define FW DMA register*******************/
    /****************************************************************/
    __IO uint32_t HWFISR;     /*offset:0x100, HIF WLAN Firmware Interrupt Status Register.*/
    __IO uint32_t HWFIER;     /*offset:0x104, HIF WLAN Firmware Interrupt Enable Register.*/
    __IO uint32_t HWFISR1;     /*offset:0x108, Reserve for HWFISR1.*/
    __IO uint32_t HWFIER1;     /*offset:0x10c, Reserve for HWFIER1.*/
    __IO uint32_t HWFTE0SR;     /*offset:0x110, HIF WLAN Firmware TX Event 0 Status Register.*/
    __IO uint32_t HWFTE1SR;     /*offset:0x114, Reserve for HWFTE1SR.*/
    __IO uint32_t HWFTE2SR;     /*offset:0x118, Reserve for HWFTE2SR.*/
    __IO uint32_t HWFTE3SR;     /*offset:0x11c, Reserve for HWFTE3SR.*/
    __IO uint32_t HWFTE0ER;     /*offset:0x120, HIF WLAN Firmware TX Event 0 Enable Register.*/
    __IO uint32_t HWFTE1ER;     /*offset:0x124, Reserve for HWFTE1ER.*/
    __IO uint32_t HWFTE2ER;     /*offset:0x128, Reserve for HWFTE2ER.*/
    __IO uint32_t HWFTE3ER;     /*offset:0x12c, Reserve for HWFTE3ER.*/
    __IO uint32_t HWFRE0SR;     /*offset:0x130, HIF WLAN Firmware RX Event 0 Status Register.*/
    __IO uint32_t HWFRE1SR;     /*offset:0x134, HIF WLAN Firmware RX Event 1 Status Register.*/
    __IO uint32_t HWFRE2SR;     /*offset:0x138, Reserve for HWFRE2SR.*/
    __IO uint32_t HWFRE3SR;     /*offset:0x13c, Reserve for HWFRE3SR.*/
    __IO uint32_t HWFRE0ER;     /*offset:0x140, HIF WLAN Firmware RX Event 0 Enable Register.*/
    __IO uint32_t HWFRE1ER;     /*offset:0x144, HIF WLAN Firmware RX Event 1 Enable Register.*/
    __IO uint32_t HWFRE2ER;     /*offset:0x148, Reserve for HWFRE2ER.*/
    __IO uint32_t HWFRE3ER;     /*offset:0x14c, Reserve for HWFRE3ER.*/
    __IO uint32_t HWFICR;       /*offset:0x150, HIF WLAN Firmware Interrupt Control Register.*/
    __IO uint32_t HWFCR;        /*offset:0x154, HIF WLAN Firmware Control Register.*/
    __IO uint32_t HWTDCR;       /*offset:0x158, HIF WLAN TX DMA Control Register.*/
    __IO uint32_t HWTPCCR;       /*offset:0x15c, HIF WLAN TX Packet Count Control Register.*/
    __IO uint32_t HWFTQ0SAR;       /*offset:0x160, HIF WLAN Firmware TX Queue 0 Start Address Register.*/
    __IO uint32_t HWFTQ1SAR;       /*offset:0x164, HIF WLAN Firmware TX Queue 1 Start Address Register.*/
    __IO uint32_t HWFTQ2SAR;       /*offset:0x168, HIF WLAN Firmware TX Queue 2 Start Address Register.*/
    __IO uint32_t HWFTQ3SAR;       /*offset:0x16c, HIF WLAN Firmware TX Queue 3 Start Address Register.*/
    __IO uint32_t HWFTQ4SAR;       /*offset:0x170, HIF WLAN Firmware TX Queue 4 Start Address Register.*/
    __IO uint32_t HWFTQ5SAR;       /*offset:0x174, Reserve for HIF WLAN Firmware TX Queue 5 Start Address Register.*/
    __IO uint32_t HWFTQ6SAR;       /*offset:0x178, Reserve for HIF WLAN Firmware TX Queue 6 Start Address Register.*/
    __IO uint32_t HWFTQ7SAR;       /*offset:0x17c, Reserve for HIF WLAN Firmware TX Queue 7 Start Address Register.*/
    __IO uint32_t HWFRQ0SAR;       /*offset:0x180, HIF WLAN Firmware RX Queue 0 Start Address Register.*/
    __IO uint32_t HWFRQ1SAR;       /*offset:0x184, HIF WLAN Firmware RX Queue 1 Start Address Register.*/
    __IO uint32_t HWFRQ2SAR;       /*offset:0x188, HIF WLAN Firmware RX Queue 2 Start Address Register.*/
    __IO uint32_t HWFRQ3SAR;       /*offset:0x18c, HIF WLAN Firmware RX Queue 3 Start Address Register.*/
    __IO uint32_t HWFRQ4SAR;       /*offset:0x190, Reserve for HIF WLAN Firmware RX Queue 4 Start Address Register.*/
    __IO uint32_t HWFRQ5SAR;       /*offset:0x194, Reserve for HIF WLAN Firmware RX Queue 5 Start Address Register.*/
    __IO uint32_t HWFRQ6SAR;       /*offset:0x198, Reserve for HIF WLAN Firmware RX Queue 6 Start Address Register.*/
    __IO uint32_t HWFRQ7SAR;       /*offset:0x19c, Reserve for HIF WLAN Firmware RX Queue 7 Start Address Register.*/
    __IO uint32_t H2DRM0R;       /*offset:0x1a0, Host to Device Receive Mailbox 0 Register.*/
    __IO  uint32_t H2DRM1R;       /*offset:0x1a4, Host to Device Receive Mailbox 1 Register.*/
    __IO  uint32_t D2HSM0R;       /*offset:0x1a8, Device to Host Send Mailbox 0 Register.*/
    __IO uint32_t D2HSM1R;       /*offset:0x1ac, Device to Host Send Mailbox 1 Register.*/
    __IO uint32_t D2HSM2R;       /*offset:0x1b0, Device to Host Send Mailbox 2 Register.*/
    __IO uint32_t rsv3[3];
    __IO uint32_t HWRQ0CR;       /*offset:0x1c0, HIF WLAN RX Queue 0 Control Register.*/
    __IO uint32_t HWRQ1CR;       /*offset:0x1c4, HIF WLAN RX Queue 1 Control Register.*/
    __IO uint32_t HWRQ2CR;       /*offset:0x1c8, HIF WLAN RX Queue 2 Control Register.*/
    __IO uint32_t HWRQ3CR;       /*offset:0x1cc, HIF WLAN RX Queue 3 Control Register.*/
    __IO uint32_t HWRQ4CR;       /*offset:0x1d0, Reserve for HWRQ4CR.*/
    __IO uint32_t HWRQ5CR;       /*offset:0x1d4, Reserve for HWRQ5CR.*/
    __IO uint32_t HWRQ6CR;       /*offset:0x1d8, Reserve for HWRQ6CR.*/
    __IO uint32_t HWRQ7CR;       /*offset:0x1dc, Reserve for HWRQ7CR.*/
    __I  uint32_t HWRLFACR;       /*offset:0x1e0, HIF WLAN RX Length FIFO Available Count Register.*/
    __I  uint32_t HWRLFACR1;       /*offset:0x1e4, Reserve for HWRLFACR1.*/
    __IO uint32_t HWDMACR;       /*offset:0x1e8, HIF WLAN DMA Control Register.*/
    __IO uint32_t HWFIOCDR;       /*offset:0x1ec, HIF WLAN Firmware GPD IOC bit Disable Register.*/
    __IO uint32_t HSDIOTOCR;       /*offset:0x1f0, HIF SDIO Time-Out Control Register.*/
    __IO uint32_t rsv4[3];
    __I  uint32_t HWFTSR0;       /*offset:0x200, HIF WLAN Firmware TX Status Register 0.*/
    __I  uint32_t HWFTSR1;       /*offset:0x204, HIF WLAN Firmware TX Status Register 1.*/
    __IO uint32_t rsv5[2];
    __IO uint32_t HWDBGCR;       /*offset:0x210, HIF WLAN Debug Control Register.*/
    __IO uint32_t HWDBGPLR;       /*offset:0x214, HIF WLAN Debug Packet Length Register.*/
    __IO uint32_t HSPICSR;       /*offset:0x218, WLAN SPI Control Status Register (SPI Only).*/
} sdio_slv_register_t;



#define SDIO_SLAVE_REG     ((sdio_slv_register_t *)SDIO_SLAVE_BASE)


/*********************SDIO FW HGFCR*****************************/
#define HGFCR_DB_HIF_SEL_OFFSET        (0)
#define HGFCR_DB_HIF_SEL_MASK          (0x07 << HGFCR_DB_HIF_SEL_OFFSET)
#define HGFCR_SPI_MODE_OFFSET          (4)
#define HGFCR_SPI_MODE_MASK            (1 << HGFCR_SPI_MODE_OFFSET)
#define HGFCR_EHPI_MODE_OFFSET         (5)
#define HGFCR_EHPI_MODE_MASK           (1 << HGFCR_EHPI_MODE_OFFSET)
#define HGFCR_SDIO_PIO_SEL_OFFSET      (6)
#define HGFCR_SDIO_PIO_SEL_MASK        (1 << HGFCR_SDIO_PIO_SEL_OFFSET)
#define HGFCR_HINT_AS_FW_OB_OFFSET     (8)
#define HGFCR_HINT_AS_FW_OB_MASK       (1 << HGFCR_HINT_AS_FW_OB_OFFSET)
#define HGFCR_CARD_IS_18V_OFFSET       (9)
#define HGFCR_CARD_IS_18V_MASK         (1 << HGFCR_CARD_IS_18V_OFFSET)
#define HGFCR_SDCTL_BUSY_OFFSET        (10)
#define HGFCR_SDCTL_BUSY_MASK          (1 << HGFCR_SDCTL_BUSY_OFFSET)
#define HGFCR_INT_TER_CYC_MASK_OFFSET  (16)
#define HGFCR_INT_TER_CYC_MASK_MASK    (1 << HGFCR_INT_TER_CYC_MASK_OFFSET)
#define HGFCR_HCLK_NO_GATED_OFFSET     (17)
#define HGFCR_HCLK_NO_GATED_MASK       (1 << HGFCR_HCLK_NO_GATED_OFFSET)
#define HGFCR_FORCE_SD_HS_OFFSET       (18)
#define HGFCR_FORCE_SD_HS_MASK         (1 << HGFCR_FORCE_SD_HS_OFFSET)
#define HGFCR_SDIO_HCLK_DIS_OFFSET     (24)
#define HGFCR_SDIO_HCLK_DIS_MASK       (1 << HGFCR_SDIO_HCLK_DIS_OFFSET)
#define HGFCR_SPI_HCLK_DIS_OFFSET      (25)
#define HGFCR_SPI_HCLK_DIS_MASK        (1 << HGFCR_SPI_HCLK_DIS_OFFSET)
#define HGFCR_EHPI_HCLK_DIS_OFFSET     (26)
#define HGFCR_EHPI_HCLK_DIS_MASK       (1 << HGFCR_EHPI_HCLK_DIS_OFFSET)
#define HGFCR_PB_HCLK_DIS_OFFSET       (27)
#define HGFCR_PB_HCLK_DIS_MASK         (1 << HGFCR_PB_HCLK_DIS_OFFSET)
#define HGFCR_PAD_CR_SET_BY_FW_OFFSET  (28)
#define HGFCR_PAD_CR_SET_BY_FW_MASK    (1 << HGFCR_PAD_CR_SET_BY_FW_OFFSET)


/*********************SDIO FW HGFISR*****************************/
#define HGFISR_DRV_CLR_DB_IOE_OFFSET        (0)
#define HGFISR_DRV_CLR_DB_IOE_MASK          (1 << HGFISR_DRV_CLR_DB_IOE_OFFSET)
#define HGFISR_DRV_CLR_PB_IOE_OFFSET        (1)
#define HGFISR_DRV_CLR_PB_IOE_MASK          (1 << HGFISR_DRV_CLR_PB_IOE_OFFSET)
#define HGFISR_DRV_SET_DB_IOE_OFFSET        (2)
#define HGFISR_DRV_SET_DB_IOE_MASK          (1 << HGFISR_DRV_SET_DB_IOE_OFFSET)
#define HGFISR_DRV_SET_PB_IOE_OFFSET        (3)
#define HGFISR_DRV_SET_PB_IOE_MASK          (1 << HGFISR_DRV_SET_PB_IOE_OFFSET)
#define HGFISR_SDIO_SET_RES_OFFSET          (4)
#define HGFISR_SDIO_SET_RES_MASK            (1 << HGFISR_SDIO_SET_RES_OFFSET)
#define HGFISR_SDIO_SET_ABT_OFFSET          (5)
#define HGFISR_SDIO_SET_ABT_MASK            (1 << HGFISR_SDIO_SET_ABT_OFFSET)
#define HGFISR_DB_INT_OFFSET                (6)
#define HGFISR_DB_INT_MASK                  (1 << HGFISR_DB_INT_OFFSET)
#define HGFISR_PB_INT_OFFSET                (7)
#define HGFISR_PB_INT_MASK                  (1 << HGFISR_PB_INT_OFFSET)
#define HGFISR_CRC_ERROR_INT_OFFSET         (8)
#define HGFISR_CRC_ERROR_INT_MASK           (1 << HGFISR_CRC_ERROR_INT_OFFSET)
#define HGFISR_CHG_TO_18V_REQ_INT_OFFSET    (9)
#define HGFISR_CHG_TO_18V_REQ_INT_MASK      (1 << HGFISR_CHG_TO_18V_REQ_INT_OFFSET)
#define HGFISR_SD1_SET_XTAL_UPD_INT_OFFSET  (10)
#define HGFISR_SD1_SET_XTAL_UPD_INT_MASK    (1 << HGFISR_SD1_SET_XTAL_UPD_INT_OFFSET)
#define HGFISR_SD1_SET_DS_INT_OFFSET        (11)
#define HGFISR_SD1_SET_DS_INT_MASK          (1 << HGFISR_SD1_SET_DS_INT_OFFSET)


/*********************SDIO FW HWFCR*****************************/
#define HWFCR_W_FUNC_RDY_OFFSET            (0)
#define HWFCR_W_FUNC_RDY_MASK              (1 << HWFCR_W_FUNC_RDY_OFFSET)
#define HWFCR_TRX_DESC_CHKSUM_EN_OFFSET    (1)
#define HWFCR_TRX_DESC_CHKSUM_EN_MASK      (1 << HWFCR_TRX_DESC_CHKSUM_EN_OFFSET)
#define HWFCR_TRX_DESC_CHKSUM_12B_OFFSET   (2)
#define HWFCR_TRX_DESC_CHKSUM_12B_MASK     (1 << HWFCR_TRX_DESC_CHKSUM_12B_OFFSET)
#define HWFCR_TX_CS_OFLD_EN_OFFSET         (3)
#define HWFCR_TX_CS_OFLD_EN_MASK           (1 << HWFCR_TX_CS_OFLD_EN_OFFSET)
#define HWFCR_TX_NO_HEADER_OFFSET          (8)
#define HWFCR_TX_NO_HEADER_MASK            (1 << HWFCR_TX_NO_HEADER_OFFSET)
#define HWFCR_RX_NO_TAIL_OFFSET            (9)
#define HWFCR_RX_NO_TAIL_MASK              (1 << HWFCR_RX_NO_TAIL_OFFSET)


/*********************SDIO FW HWFISR*****************************/
#define HWFISR_DRV_SET_FW_OWN_OFFSET     (0)
#define HWFISR_DRV_SET_FW_OWN_MASK       (1 << HWFISR_DRV_SET_FW_OWN_OFFSET)
#define HWFISR_DRV_CLR_FW_OWN_OFFSET     (1)
#define HWFISR_DRV_CLR_FW_OWN_MASK       (1 << HWFISR_DRV_CLR_FW_OWN_OFFSET)
#define HWFISR_D2HSM2R_RD_INT_OFFSET     (2)
#define HWFISR_D2HSM2R_RD_INT_MASK       (1 << HWFISR_D2HSM2R_RD_INT_OFFSET)
#define HWFISR_RD_TIMEOUT_INT_OFFSET     (3)
#define HWFISR_RD_TIMEOUT_INT_MASK       (1 << HWFISR_RD_TIMEOUT_INT_OFFSET)
#define HWFISR_WR_TIMEOUT_INT_OFFSET     (4)
#define HWFISR_WR_TIMEOUT_INT_MASK       (1 << HWFISR_WR_TIMEOUT_INT_OFFSET)
#define HWFISR_TX_EVENT_0_OFFSET         (8)
#define HWFISR_TX_EVENT_0_MASK           (1 << HWFISR_TX_EVENT_0_OFFSET)
#define HWFISR_RX_EVENT_0_OFFSET         (12)
#define HWFISR_RX_EVENT_0_MASK           (1 << HWFISR_RX_EVENT_0_OFFSET)
#define HWFISR_RX_EVENT_1_OFFSET         (13)
#define HWFISR_RX_EVENT_1_MASK           (1 << HWFISR_RX_EVENT_1_OFFSET)
#define HWFISR_H2D_SW_INT_OFFSET         (16)
#define HWFISR_H2D_SW_INT_MASK           (0xffff << HWFISR_H2D_SW_INT_OFFSET)

/*********************SDIO FW HWFIER*****************************/
#define HWFIER_DRV_SET_FW_OWN_INT_EN_OFFSET     (0)
#define HWFIER_DRV_SET_FW_OWN_INT_EN_MASK       (1 << HWFIER_DRV_SET_FW_OWN_INT_EN_OFFSET)
#define HWFIER_DRV_CLR_FW_OWN_INT_EN_OFFSET     (1)
#define HWFIER_DRV_CLR_FW_OWN_INT_EN_MASK       (1 << HWFIER_DRV_CLR_FW_OWN_INT_EN_OFFSET)
#define HWFIER_D2HSM2R_RD_INT_EN_OFFSET         (2)
#define HWFIER_D2HSM2R_RD_INT_EN_MASK           (1 << HWFIER_D2HSM2R_RD_INT_EN_OFFSET)
#define HWFIER_RD_TIMEOUT_INT_EN_OFFSET         (3)
#define HWFIER_RD_TIMEOUT_INT_EN_MASK           (1 << HWFIER_RD_TIMEOUT_INT_EN_OFFSET)
#define HWFIER_WR_TIMEOUT_INT_EN_OFFSET         (4)
#define HWFIER_WR_TIMEOUT_INT_EN_MASK           (1 << HWFIER_WR_TIMEOUT_INT_EN_OFFSET)
#define HWFIER_TX_EVENT_0_INT_EN_OFFSET         (8)
#define HWFIER_TX_EVENT_0_INT_EN_MASK           (1 << HWFIER_TX_EVENT_0_INT_EN_OFFSET)
#define HWFIER_RX_EVENT_0_INT_EN_OFFSET         (12)
#define HWFIER_RX_EVENT_0_INT_EN_MASK           (1 << HWFIER_RX_EVENT_0_INT_EN_OFFSET)
#define HWFIER_RX_EVENT_1_INT_EN_OFFSET         (13)
#define HWFIER_RX_EVENT_1_INT_EN_MASK           (1 << HWFIER_RX_EVENT_1_INT_EN_OFFSET)
#define HWFIER_H2D_SW_INT_EN_OFFSET             (16)
#define HWFIER_H2D_SW_INT_EN_MASK               (0xffff << HWFIER_H2D_SW_INT_EN_OFFSET)


/*********************SDIO FW HWFICR*****************************/
#define HWFICR_FW_OWN_BACK_INT_SET_OFFSET   (4)
#define HWFICR_FW_OWN_BACK_INT_SET_MASK     (1 << HWFICR_FW_OWN_BACK_INT_SET_OFFSET)
#define HWFICR_D2H_SW_INT_SET_OFFSET        (8)
#define HWFICR_D2H_SW_INT_SET_MASK          (0xffffff << HWFICR_D2H_SW_INT_SET_OFFSET)


/*********************SDIO FW HWDMACR*****************************/
#define HWFICR_DEST_BST_TYP_OFFSET   (1)
#define HWFICR_DEST_BST_TYP_MASK     (1 << HWFICR_DEST_BST_TYP_OFFSET)
#define HWFICR_DMA_BST_SIZE_OFFSET   (6)
#define HWFICR_DMA_BST_SIZE_MASK     (0x03 << HWFICR_DMA_BST_SIZE_OFFSET)


/*********************SDIO FW HWRQ0CR*****************************/
#define HWRQ0CR_RXQ0_PACKET_LENGTH_OFFSET   (0)
#define HWRQ0CR_RXQ0_PACKET_LENGTH_MASK     (0xffff << HWRQ0CR_RXQ0_PACKET_LENGTH_OFFSET)
#define HWRQ0CR_RXQ0_DMA_STOP_OFFSET        (16)
#define HWRQ0CR_RXQ0_DMA_STOP_MASK          (1 << HWRQ0CR_RXQ0_DMA_STOP_OFFSET)
#define HWRQ0CR_RXQ0_DMA_START_OFFSET       (17)
#define HWRQ0CR_RXQ0_DMA_START_MASK         (1 << HWRQ0CR_RXQ0_DMA_START_OFFSET)
#define HWRQ0CR_RXQ0_DMA_RUM_OFFSET         (18)
#define HWRQ0CR_RXQ0_DMA_RUM_MASK           (1 << HWRQ0CR_RXQ0_DMA_RUM_OFFSET)
#define HWRQ0CR_RXQ0_DMA_STATUS_OFFSET      (19)
#define HWRQ0CR_RXQ0_DMA_STATUS_MASK        (1 << HWRQ0CR_RXQ0_DMA_STATUS_OFFSET)


/*********************SDIO FW HWRQ1CR*****************************/
#define HWRQ1CR_RXQ1_PACKET_LENGTH_OFFSET   (0)
#define HWRQ1CR_RXQ1_PACKET_LENGTH_MASK     (0xffff << HWRQ1CR_RXQ1_PACKET_LENGTH_OFFSET)
#define HWRQ1CR_RXQ1_DMA_STOP_OFFSET        (16)
#define HWRQ1CR_RXQ1_DMA_STOP_MASK          (1 << HWRQ1CR_RXQ1_DMA_STOP_OFFSET)
#define HWRQ1CR_RXQ1_DMA_START_OFFSET       (17)
#define HWRQ1CR_RXQ1_DMA_START_MASK         (1 << HWRQ1CR_RXQ1_DMA_START_OFFSET)
#define HWRQ1CR_RXQ1_DMA_RUM_OFFSET         (18)
#define HWRQ1CR_RXQ1_DMA_RUM_MASK           (1 << HWRQ1CR_RXQ1_DMA_RUM_OFFSET)
#define HWRQ1CR_RXQ1_DMA_STATUS_OFFSET      (19)
#define HWRQ1CR_RXQ1_DMA_STATUS_MASK        (1 << HWRQ1CR_RXQ1_DMA_STATUS_OFFSET)

/*********************SDIO FW HWTPCCR*****************************/
#define HWTPCCR_INC_TQ_CNT_OFFSET     (0)
#define HWTPCCR_INC_TQ_CNT_MASK       (0xff << HWTPCCR_INC_TQ_CNT_OFFSET)
#define HWTPCCR_TQ_INDEX_OFFSET       (12)
#define HWTPCCR_TQ_INDEX_MASK         (0x0f << HWTPCCR_TQ_INDEX_OFFSET)
#define HWTPCCR_TQ_CNT_RESET_OFFSET   (16)
#define HWTPCCR_TQ_CNT_RESET_MASK     (1 << HWTPCCR_TQ_CNT_RESET_OFFSET)

/************************ SDIO_SLAVE register definition end line  *******************************
 */


/*************************** GPIO register definition start line  *******************************
 */
#endif

