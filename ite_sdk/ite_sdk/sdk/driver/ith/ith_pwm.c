/*
 * Copyright (c) 2011 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * HAL PWM functions.
 *
 * @author Jim Tan
 * @version 1.0
 */
#include "ith_cfg.h"
#include <pthread.h>

static uint32_t blCounts[ITH_PWM_COUNT], blMatchs[ITH_PWM_COUNT];

void ithPwmInit(ITHPwm pwm, unsigned int freq, unsigned int duty)
{
	if (duty != 100)
	{
		blCounts[pwm] = (duty) ? (ithGetBusClock() / freq) - 1 : duty;
		blMatchs[pwm] = (duty) ? (uint64_t)blCounts[pwm] * (100 - duty % 100) / 100 : -1;
	}
	else
	{
		blCounts[pwm] = 0;
		blMatchs[pwm] = 0;
	}
}

static void ithPwmSetGpioMode(ITHPwm pwm, unsigned int pin)
{
	switch (pwm) {
		case ITH_PWM1:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, pin << ITH_GPIO_PWM_LOW_BIT, ITH_GPIO_PWM_LOW_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM2:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, pin << ITH_GPIO_PWM_HIGH_BIT, ITH_GPIO_PWM_HIGH_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
		case ITH_PWM3:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, pin << ITH_GPIO_PWM_LOW_BIT, ITH_GPIO_PWM_LOW_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM4:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, pin << ITH_GPIO_PWM_HIGH_BIT, ITH_GPIO_PWM_HIGH_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
		case ITH_PWM5:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, pin << ITH_GPIO_PWM_LOW_BIT, ITH_GPIO_PWM_LOW_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM6:
			ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, pin << ITH_GPIO_PWM_HIGH_BIT, ITH_GPIO_PWM_HIGH_MASK);
			ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
	}
}

static void ithPwmClearGpioMode(ITHPwm pwm)
{
	switch(pwm) {
		case ITH_PWM1:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM2:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL01_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
		case ITH_PWM3:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM4:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL23_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
		case ITH_PWM5:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, ITH_GPIO_PWM_LOW_EN_BIT);
			break;
		case ITH_PWM6:
			ithClearRegBitA(ITH_GPIO_BASE + ITH_GPIO_PWMSEL45_REG, ITH_GPIO_PWM_HIGH_EN_BIT);
			break;
	}
}

void ithPwmReset(ITHPwm pwm, unsigned int pin, unsigned int gpio_mode)
{
#if (CFG_CHIP_FAMILY == 960)
    ithPwmSetGpioMode(pwm, pin);
#endif
	ithGpioSetMode(pin, gpio_mode);
    ithTimerReset(pwm);
    ithTimerSetCounter(pwm, 0);
    ithTimerSetPwmMatch(pwm, blMatchs[pwm], blCounts[pwm]);

    ithTimerCtrlEnable(pwm, ITH_TIMER_UPCOUNT);
    ithTimerCtrlEnable(pwm, ITH_TIMER_PERIODIC);
    ithTimerCtrlEnable(pwm, ITH_TIMER_PWM);
    ithTimerEnable(pwm);
}

void ithPwmSetDutyCycle(ITHPwm pwm, unsigned int duty)
{
	uint32_t newmatch1;

	ithTimerCtrlDisable(pwm, ITH_TIMER_EN);
	ithTimerSetCounter(pwm, 0);

	if (duty != 100)
		newmatch1 = (duty) ? ((uint64_t)blCounts[pwm] * (100 - duty % 100) / 100) : -1;
	else
		newmatch1 = 0;
	
	ithTimerSetPwmMatch(pwm, newmatch1, blCounts[pwm]);
	ithTimerCtrlEnable(pwm, ITH_TIMER_EN);
}

void ithPwmEnable(ITHPwm pwm, unsigned int pin, unsigned int gpio_mode)
{
#if (CFG_CHIP_FAMILY == 960)
	ithPwmSetGpioMode(pwm, pin);
#endif
    ithGpioSetMode(pin, gpio_mode);    
    ithTimerCtrlEnable(pwm, ITH_TIMER_PWM);
    ithTimerCtrlEnable(pwm, ITH_TIMER_EN);
}

void ithPwmDisable(ITHPwm pwm, unsigned int pin)
{
#if (CFG_CHIP_FAMILY == 960)
	ithPwmClearGpioMode(pwm);
#endif
    ithGpioClear(pin);
    ithGpioEnable(pin);
    ithGpioSetOut(pin);
    ithTimerCtrlDisable(pwm, ITH_TIMER_EN);
    ithTimerCtrlDisable(pwm, ITH_TIMER_PWM);
}
