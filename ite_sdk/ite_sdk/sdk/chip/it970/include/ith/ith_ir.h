#ifndef ITH_IR_H
#define ITH_IR_H

/** @addtogroup ith ITE Hardware Library
 *  @{
 */
/** @addtogroup ith_ir IR
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IR port definition.
 */
typedef enum
{
    ITH_IR0 = ITH_IR_BASE,      ///< IR Port #0
} ITHIrPort;

/**
 * Initializes IR module.
 *
 * @param port the IR base address
 * @param pin the GPIO pin.
 * @param extClk the external clock. 0 to use PLL clock.
 * @param sample the sample clock in microseconds.
 * @param precision the precision.
 */
void ithIrInit(unsigned int pin, unsigned long extClk, unsigned long sample, unsigned int precision);

/**
 * Initializes IR TX module.
 *
 * @param port the IR base address
 * @param pin the GPIO pin.
 * @param extClk the external clock. 0 to use PLL clock.
 * @param sample the sample clock in microseconds.
 * @param precision the precision.
 */
void ithIrTxInit(unsigned int pin, unsigned long extClk, unsigned long sample, unsigned int precision);

/**
 * Probes IR code.
 *
 * @param port the IR base address
 * @return the code. -1 indicates no key input.
 */
int ithIrProbe(void);

/**
 * Set IR RX modulation frequency.
 *
 * @param freqDiv the modulation frequency.
 */
static inline void ithIrRxSetModFilter(ITHIrPort port, int minFreqDiv, int maxFreqDiv)
{
    ithWriteRegA(port + ITH_IR_MOD_FILTER_REG, (0x1 << 31) | (minFreqDiv & 0x7FFF) << 16 | (maxFreqDiv & 0x7FFF));
}

#include "ith/ith_ir_generic.h"

#ifdef __cplusplus
}
#endif

#endif // ITH_IR_H
/** @} */ // end of ith_ir
/** @} */ // end of ith