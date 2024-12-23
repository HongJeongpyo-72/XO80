/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __HAL_SDIO_H__
#define __HAL_SDIO_H__
#include "hal_platform.h"
#if defined(CFG_MMC_ENABLE)
#include "ite/ite_sdio.h"
#include <linux/errno.h>
#else
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio.h>
#endif

#ifdef HAL_SDIO_MODULE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup ITE Modifies Struct
  * @{
  */
/* --------------------------- ITE --------------------------- */
void set_hal_ops(struct sdio_func *func);

struct hal_ops {
    signed int  (*init_xmit_priv)(struct sdio_func *func);
    signed int  (*init_recv_priv)(struct sdio_func *func);
};

/* --------------------------------------------------------- */


/** @defgroup hal_sdio_enum Enum
  * @{
  */

/** @brief  This enum defines the SDIO API return type. */
typedef enum {
    HAL_SDIO_STATUS_BUSY                       = -2,                        /**<  An error occurred, the SDIO bus is busy. */
    HAL_SDIO_STATUS_ERROR                      = -1,                        /**<  An error occurred and the operation failed. */
    HAL_SDIO_STATUS_OK                         =  0,                        /**<  No error occurred during the function call. */
} hal_sdio_status_t;


/** @brief  This enum define the SDIO bus width types*/
typedef enum {
    HAL_SDIO_BUS_WIDTH_1 = 1,                                        /**< The SDIO bus width is 1 bit. */
    HAL_SDIO_BUS_WIDTH_4 = 2                                         /**< The SDIO bus width is 4 bits. */
} hal_sdio_bus_width_t;


/** @brief   This enum defines the SDIO command direction types*/
typedef enum {
    HAL_SDIO_DIRECTION_READ  = 0,                                               /**< The SDIO read command. */
    HAL_SDIO_DIRECTION_WRITE = 1                                                /**< The SDIO write command. */
} hal_sdio_direction_t;


/** @brief   This enum defines the SDIO command operation types. */
typedef enum {
    HAL_SDIO_FIXED_ADDRESS        = 0,                                           /**< SDIO multi-byte read/write at a fixed address. */
    HAL_SDIO_INCREMENTING_ADDRESS = 1                                            /**< SDIO multi-byte read/write at an incrementing address. */
} hal_sdio_command53_operation_t;


/** @brief   This enum defines the SDIO command function number. */
typedef enum {
    HAL_SDIO_FUNCTION_0 = 0,                                              /**< SDIO function 0, this function selects the common I/O area register to read or write. */
    HAL_SDIO_FUNCTION_1 = 1                                               /**< SDIO function 1, this function selects the I/O function 1 register to read or write.*/
} hal_sdio_function_id_t;

/** @brief   This enum defines the SDIO callback event number. */
typedef enum {
    HAL_SDIO_EVENT_SUCCESS = 0,                  /**<  The transaction completed without any error. */
    HAL_SDIO_EVENT_TRANSFER_ERROR = -1,          /**<  An error occurred during transaction. */
    HAL_SDIO_EVENT_CRC_ERROR = -2,               /**<  CRC error occurred during transaction. */
    HAL_SDIO_EVENT_DATA_TIMEOUT = -3,            /**<  Data timeout occurred during transaction. */
}hal_sdio_callback_event_t;

/** @brief   This enum defines the SDIO interrupt callback event number. */
typedef enum {
    HAL_SDIO_INT_RW_SUCCESS     = 3,             /**<  Can  transmitte and receive data from SDIO. */
    HAL_SDIO_INT_RX_SUCCESS     = 2,             /**<  Can receive data from SDIO. */
    HAL_SDIO_INT_TX_SUCCESS     = 1,             /**<  Can transmitte data to SDIO. */
    HAL_SDIO_INT_NORMAL         = 0,             /**<  The transaction completed without any error. */
    HAL_SDIO_INT_TRANSFER_ERROR = -1,            /**<  An error occurred during transaction. */
    HAL_SDIO_INT_CRC_ERROR      = -2,            /**<  CRC error occurred during transaction. */
    HAL_SDIO_INT_DATA_TIMEOUT   = -3,            /**<  Data timeout occurred during transaction. */
}hal_sdio_int_callback_event_t;


/**
  * @}
  */

/** @defgroup hal_sd_struct Struct
  * @{
  */

struct mt7682_sdio_st {
    struct sdio_func *func;

    //TODO
    uint16_t buffer_size;
    ///< All data that do not fully fill a buffer is still counted as one buffer. E.g. 10 bytes data costs 2 buffers if the size is 8 bytes per buffer.
    ///< Buffer size of the slave pre-defined between host and slave before communication.
    uint16_t block_size;
    uint32_t tx_sent_buffers;    ///< Counter hold the amount of buffers already sent to ESP32 slave. Should be set to 0 when initialization.
    uint32_t rx_got_bytes;       ///< Counter hold the amount of bytes already received from ESP32 slave. Should be set to 0 when initialization.
};


/** @brief   This structure defines the SDIO initialization parameters. */
typedef struct {
    hal_sdio_bus_width_t bus_width;                                    /**< Bus width for the SDIO read/write operation. */
    uint32_t clock;                                                    /**< The SDIO bus clock. */
} hal_sdio_config_t;


/** @brief   This structure defines the SDIO COMMAND53 configuration parameter. For more details about the COMMAND53, please
 *   refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.3.*/
typedef struct {
    hal_sdio_direction_t direction;                                    /**< Read/write direction for the SDIO COMMAND53. */
    hal_sdio_function_id_t function;                                   /**< Function type for the SDIO COMMAND53. */
    bool block;                                                        /**< Indicates whether read/write is in a block mode or not. */
    hal_sdio_command53_operation_t operation;                          /**< Operation mode for the SDIO COMMAND53. */
    uint16_t count;                                                    /**< Byte or block count. */
    uint32_t address;                                                  /**< Read/write address of the SDIO COMMAND53. */
    uint32_t buffer;                                                   /**< Buffer address for data transfer. Must be a non-cacheable and 4 bytes aligned address for the DMA mode. */
} hal_sdio_command53_config_t;


/** @brief   This structure defines the SDIO COMMAND52 configuration parameter. For more details about the COMMAND52, please
 *   refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.1. */
typedef struct {
    hal_sdio_direction_t direction;                                    /**< Read/write direction for the SDIO COMMAND52. */
    hal_sdio_function_id_t function;                                   /**< Function type for the SDIO COMMAND52.*/
    bool read_after_write;                                             /**< Indicates whether read is after write or not. */
    bool stop;                                                         /**< Indicates if the data transfer stopped or not. */
    uint8_t data;                                                      /**< Write or read back data.*/
    uint32_t address;                                                  /**< Read/write address of the SDIO COMMAND52. */
} hal_sdio_command52_config_t;


typedef struct {
    //void (*msdc_sdio_dma_interrupt_callback)(hal_sdio_callback_event_t event,void *user_data);
    void (*msdc_sdio_interrupt_callback)(hal_sdio_int_callback_event_t event,void *user_data);
} msdc_config_t;


/**
  * @}
  */

/** @defgroup hal_sdio_typedef Typedef
  * @{
  */
/** @brief    This defines the callback function prototype.
 *         Register a callback function to handle a DMA interrupt transfer process.
 *         This function is called after the SDIO data transfer is done.
 *           For more details about the callback, please refer to #hal_sdio_register_callback().
 *  @param [in] event is the event of the DMA interrupt transfer. This parameter specifies whether the transfer is complete successfully.
 *              For more details about the event type, please refer to #hal_sdio_callback_event_t.
 *  @param [in] user_data is the user defined parameter obtained from the #hal_sdio_register_callback() function.
 */
typedef void (*hal_sdio_callback_t)(hal_sdio_int_callback_event_t event, void *user_data);
/**
  * @}
  */

/**
 * @brief This function registers a callback function to inform the user the transfer is complete.
 * @param[in] sdio_port is the MSDC port to transfer data.
 * @param[in] sdio_callback is the function pointer of the callback.
 *            The callback function is called once the SDIO data transfer is complete.
 * @param[in] user_data is the callback parameter.
 * @return    #HAL_SDIO_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_status_t hal_sdio_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback,void *user_data);

/**
 * @brief  This function initializes the MSDC hardware and SDIO slave. It can also be used to set the MSDC pin output clock and bus width.
 * @param[in] sdio_port is the initialization configuration port. For more details about this parameter, please refer to #hal_sdio_port_t.
 * @param[in] sdio_config is the initialization configuration parameter. For more details about this parameter, please refer to #hal_sdio_config_t.
 * @return    Indicates whether this function call is successful or not.
 *            If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an initialization error occurred.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 * @sa  #hal_sdio_deinit()
 */
hal_sdio_status_t hal_sdio_init(hal_sdio_port_t sdio_port, hal_sdio_config_t *sdio_config);


/**
 * @brief     This function deinitializes the MSDC and the SDIO slave settings.
 * @param[in] sdio_port is the MSDC deinitialization port.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 * @sa  #hal_sdio_init()
 */
hal_sdio_status_t hal_sdio_deinit(hal_sdio_port_t sdio_port);


/**
 * @brief     This function sets the output clock of the MSDC.
 * @param[in] sdio_port is the MSDC port to set clock.
 * @param[in] clock is the expected output clock of the MSDC.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_clock(hal_sdio_port_t sdio_port, uint32_t clock);


/**
 * @brief     This function gets the output clock of the MSDC.
 * @param[in] sdio_port is the MSDC port to get clock.
 * @param[out] clock is the current output clock of the MSDC.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_status_t hal_sdio_get_clock(hal_sdio_port_t sdio_port, uint32_t *clock);


/**
 * @brief     This function sets the bus widths for the MSDC and SDIO slave.
 * @param[in] sdio_port is the MSDC port to set.
 * @param[in] bus_width is the SDIO bus width.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_bus_width(hal_sdio_port_t sdio_port, hal_sdio_bus_width_t bus_width);


/**
 * @brief     This function sets the transaction block size of the MSDC.
 * @param[in] sdio_port is the MSDC port to set.
 * @param[in] function is the SDIO function to set block size.
 * @param[in] block_size is the SDIO transaction block size.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t block_size);


/**
 * @brief     This function gets the transaction block size of the MSDC.
 * @param[in] sdio_port is the MSDC port to get block size.
 * @param[in] function is the SDIO function to get block size.
 * @param[out] block_size is the SDIO transaction block size.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_status_t hal_sdio_get_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t *block_size);


/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND52.
 * @param[in] sdio_port is the MSDC port to read or write.
 * @param[in] command52_config is the configuration parameter pointer of the COMMAND52.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command52(hal_sdio_port_t sdio_port, hal_sdio_command52_config_t *command52_config);



/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND53 MCU mode.
 * @param[in] sdio_port is the MSDC port to read.
 * @param[in] command53_config is the configuration parameter pointer of the COMMAND53.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command53(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config);

/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND53 DMA interrupt mode. This API would block the application task.
 * @param[in] sdio_port is the MSDC port to read.
 * @param[in] command53_config is the configuration parameter pointer of the COMMAND53.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command53_dma_blocking(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config);

/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND53 DMA interrupt mode. This API would not block the application task.
 * @param[in] sdio_port is the MSDC port to read.
 * @param[in] command53_config is the configuration parameter pointer of the COMMAND53.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command53_dma(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config);

#ifdef HAL_SDIO_FEATURE_DATA1_IRQ
/**
 * @brief     This function registers a callback function to inform the user for SDIO DAT1 interrupt from slave.
 * @param[in] sdio_port is the MSDC port to transfer data.
 * @param[in] sdio_callback is the function pointer of the callback.
 *            The callback function is called once the SDIO data transfer is complete.
 * @param[in] user_data is the callback parameter.
 * @return    #HAL_SDIO_STATUS_OK, if the operation completed successfully.
 */
hal_sdio_status_t hal_sdio_interrupt_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback,void *user_data);
#endif

#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_SDIO_MODULE_ENABLED*/
#endif /* __HAL_SDIO_H__ */

