/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#include "FreeRTOSConfig.h"

#if __riscv_xlen == 64
# define STORE    sd
# define LOAD     ld
# define REGBYTES 8
#else
# define STORE    sw
# define LOAD     lw
# define REGBYTES 4
#endif

#if __riscv_flen == 64
# define FPSTORE  fsd
# define FPLOAD   fld
# define FPREGBYTES 8
#else
# define FPSTORE  fsw
# define FPLOAD   flw
# define FPREGBYTES 4
#endif

/* Enable interrupts when returning from the handler */
#ifdef __riscv_flen
#define MSTATUS_PRV1 0x3880
#else
#define MSTATUS_PRV1 0x1880
#endif

/*When the kernel update, TCB structure may be changed. So the offset need to be modified*/
#if (configHSP_ENABLE==1)
/*Set the bytes of stack's offset in TCB. Unit:byte*/
#define StackOffset_TCB			48 /*The offset of pxCurrentTCB->pxStack in TCB structure*/

/*Set the offset of top address of stack in TCB. Unit:byte*/
#define EndStackOffset_TCB		52 + configMAX_TASK_NAME_LEN /*The offset of pxCurrentTCB->pxEndOfStack in TCB structure*/
#endif

.global portSAVE_CONTEXT
.global portRESTORE_CONTEXT
.global FreeRTOS_MTIMER_Handler
.global xPortStartScheduler
.global vPortYield
.global vTaskIncrementTick
.global vPortEndScheduler
.global xExitStack
.global vPortSAVE_CONTEXT
.global vPortRESTORE_CONTEXT


/* Macro for saving task context */
.macro portSAVE_CONTEXT
	.global	pxCurrentTCB
	/* make room in stack */
#ifdef __riscv_flen
	addi	sp, sp, -(REGBYTES + FPREGBYTES) * 32
#else
	addi	sp, sp, -REGBYTES * 32
#endif

	/* Save Context */
	STORE	x1, 0x0(sp)
	STORE	x2, 1 * REGBYTES(sp)
	STORE	x3, 2 * REGBYTES(sp)
	STORE	x4, 3 * REGBYTES(sp)
	STORE	x5, 4 * REGBYTES(sp)
	STORE	x6, 5 * REGBYTES(sp)
	STORE	x7, 6 * REGBYTES(sp)
	STORE	x8, 7 * REGBYTES(sp)
	STORE	x9, 8 * REGBYTES(sp)
	STORE	x10, 9 * REGBYTES(sp)
	STORE	x11, 10 * REGBYTES(sp)
	STORE	x12, 11 * REGBYTES(sp)
	STORE	x13, 12 * REGBYTES(sp)
	STORE	x14, 13 * REGBYTES(sp)
	STORE	x15, 14 * REGBYTES(sp)
#ifndef __riscv_32e
	STORE	x16, 15 * REGBYTES(sp)
	STORE	x17, 16 * REGBYTES(sp)
	STORE	x18, 17 * REGBYTES(sp)
	STORE	x19, 18 * REGBYTES(sp)
	STORE	x20, 19 * REGBYTES(sp)
	STORE	x21, 20 * REGBYTES(sp)
	STORE	x22, 21 * REGBYTES(sp)
	STORE	x23, 22 * REGBYTES(sp)
	STORE	x24, 23 * REGBYTES(sp)
	STORE	x25, 24 * REGBYTES(sp)
	STORE	x26, 25 * REGBYTES(sp)
	STORE	x27, 26 * REGBYTES(sp)
	STORE	x28, 27 * REGBYTES(sp)
	STORE	x29, 28 * REGBYTES(sp)
	STORE	x30, 29 * REGBYTES(sp)
	STORE	x31, 30 * REGBYTES(sp)
#endif

#ifdef __riscv_flen
	/* Restore fp registers */
	FPSTORE	f0, (32 * REGBYTES + 0 * FPREGBYTES)(sp)
	FPSTORE	f1, (32 * REGBYTES + 1 * FPREGBYTES)(sp)
	FPSTORE	f2, (32 * REGBYTES + 2 * FPREGBYTES)(sp)
	FPSTORE	f3, (32 * REGBYTES + 3 * FPREGBYTES)(sp)
	FPSTORE	f4, (32 * REGBYTES + 4 * FPREGBYTES)(sp)
	FPSTORE	f5, (32 * REGBYTES + 5 * FPREGBYTES)(sp)
	FPSTORE	f6, (32 * REGBYTES + 6 * FPREGBYTES)(sp)
	FPSTORE	f7, (32 * REGBYTES + 7 * FPREGBYTES)(sp)
	FPSTORE	f8, (32 * REGBYTES + 8 * FPREGBYTES)(sp)
	FPSTORE	f9, (32 * REGBYTES + 9 * FPREGBYTES)(sp)
	FPSTORE	f10, (32 * REGBYTES + 10 * FPREGBYTES)(sp)
	FPSTORE	f11, (32 * REGBYTES + 11 * FPREGBYTES)(sp)
	FPSTORE	f12, (32 * REGBYTES + 12 * FPREGBYTES)(sp)
	FPSTORE	f13, (32 * REGBYTES + 13 * FPREGBYTES)(sp)
	FPSTORE	f14, (32 * REGBYTES + 14 * FPREGBYTES)(sp)
	FPSTORE	f15, (32 * REGBYTES + 15 * FPREGBYTES)(sp)
	FPSTORE	f16, (32 * REGBYTES + 16 * FPREGBYTES)(sp)
	FPSTORE	f17, (32 * REGBYTES + 17 * FPREGBYTES)(sp)
	FPSTORE	f18, (32 * REGBYTES + 18 * FPREGBYTES)(sp)
	FPSTORE	f19, (32 * REGBYTES + 19 * FPREGBYTES)(sp)
	FPSTORE	f20, (32 * REGBYTES + 20 * FPREGBYTES)(sp)
	FPSTORE	f21, (32 * REGBYTES + 21 * FPREGBYTES)(sp)
	FPSTORE	f22, (32 * REGBYTES + 22 * FPREGBYTES)(sp)
	FPSTORE	f23, (32 * REGBYTES + 23 * FPREGBYTES)(sp)
	FPSTORE	f24, (32 * REGBYTES + 24 * FPREGBYTES)(sp)
	FPSTORE	f25, (32 * REGBYTES + 25 * FPREGBYTES)(sp)
	FPSTORE	f26, (32 * REGBYTES + 26 * FPREGBYTES)(sp)
	FPSTORE	f27, (32 * REGBYTES + 27 * FPREGBYTES)(sp)
	FPSTORE	f28, (32 * REGBYTES + 28 * FPREGBYTES)(sp)
	FPSTORE	f29, (32 * REGBYTES + 29 * FPREGBYTES)(sp)
	FPSTORE	f30, (32 * REGBYTES + 30 * FPREGBYTES)(sp)
	FPSTORE	f31, (32 * REGBYTES + 31 * FPREGBYTES)(sp)
#endif

/*
   If HSP mechanism is on. At the begining of context switch, the mhsp_ctl should be turn off.
   Because the sp will change to another task's stack.
   Also, before setting the sp_bound & sp_base, mhsp_ctl need to be turn off
 */
#if( configHSP_ENABLE == 1 )
	li	t0, 0x0
	csrw	mhsp_ctl, t0
#endif

	/* Store current stack pointer in task control block (TCB) */
	LOAD	t0, pxCurrentTCB	//pointer
	STORE	sp, 0x0(t0)
	.endm

/* Saves current error program counter (EPC) as task program counter */
.macro portSAVE_EPC
  	csrr	t0, mepc
	STORE	t0, 31 * REGBYTES(sp)
	.endm

/* Saves current return adress (RA) as task program counter */
.macro portSAVE_RA
	STORE	ra, 31 * REGBYTES(sp)
	.endm

/* Macro for restoring task context */
.macro portRESTORE_CONTEXT

	.global	pxCurrentTCB
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)

	/* Load task program counter */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrw	mepc, t0

	/* Run in machine mode */
  	li 	t0, MSTATUS_PRV1
  	csrw	mstatus, t0

/*
   Set the msp_bound & msp_base to the new task's stack state
   and turn on the mhsp_ctl.
 */
#if( configHSP_ENABLE == 1 )
	/* Load current hardware stack protection and recording CSR*/
	LOAD	t0, pxCurrentTCB
	LOAD	t0, StackOffset_TCB(t0)
	csrw	msp_bound, t0

#if( configRECORD_STACK_HIGH_ADDRESS == 1 )
	LOAD    t0, pxCurrentTCB
	LOAD    t0, EndStackOffset_TCB(t0)
	csrw    msp_base, t0
	li      t0, 0x23
#else
	li	t0, 0x21
#endif
	csrw	mhsp_ctl, t0
#endif

	/* Restore registers,
	   Skip global pointer because that does not change */
	LOAD	x1, 0x0(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
#ifndef __riscv_32e
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)
#endif

#ifdef __riscv_flen
	/* Restore fp registers */
	FPLOAD	f0, (32 * REGBYTES + 0 * FPREGBYTES)(sp)
	FPLOAD	f1, (32 * REGBYTES + 1 * FPREGBYTES)(sp)
	FPLOAD	f2, (32 * REGBYTES + 2 * FPREGBYTES)(sp)
	FPLOAD	f3, (32 * REGBYTES + 3 * FPREGBYTES)(sp)
	FPLOAD	f4, (32 * REGBYTES + 4 * FPREGBYTES)(sp)
	FPLOAD	f5, (32 * REGBYTES + 5 * FPREGBYTES)(sp)
	FPLOAD	f6, (32 * REGBYTES + 6 * FPREGBYTES)(sp)
	FPLOAD	f7, (32 * REGBYTES + 7 * FPREGBYTES)(sp)
	FPLOAD	f8, (32 * REGBYTES + 8 * FPREGBYTES)(sp)
	FPLOAD	f9, (32 * REGBYTES + 9 * FPREGBYTES)(sp)
	FPLOAD	f10, (32 * REGBYTES + 10 * FPREGBYTES)(sp)
	FPLOAD	f11, (32 * REGBYTES + 11 * FPREGBYTES)(sp)
	FPLOAD	f12, (32 * REGBYTES + 12 * FPREGBYTES)(sp)
	FPLOAD	f13, (32 * REGBYTES + 13 * FPREGBYTES)(sp)
	FPLOAD	f14, (32 * REGBYTES + 14 * FPREGBYTES)(sp)
	FPLOAD	f15, (32 * REGBYTES + 15 * FPREGBYTES)(sp)
	FPLOAD	f16, (32 * REGBYTES + 16 * FPREGBYTES)(sp)
	FPLOAD	f17, (32 * REGBYTES + 17 * FPREGBYTES)(sp)
	FPLOAD	f18, (32 * REGBYTES + 18 * FPREGBYTES)(sp)
	FPLOAD	f19, (32 * REGBYTES + 19 * FPREGBYTES)(sp)
	FPLOAD	f20, (32 * REGBYTES + 20 * FPREGBYTES)(sp)
	FPLOAD	f21, (32 * REGBYTES + 21 * FPREGBYTES)(sp)
	FPLOAD	f22, (32 * REGBYTES + 22 * FPREGBYTES)(sp)
	FPLOAD	f23, (32 * REGBYTES + 23 * FPREGBYTES)(sp)
	FPLOAD	f24, (32 * REGBYTES + 24 * FPREGBYTES)(sp)
	FPLOAD	f25, (32 * REGBYTES + 25 * FPREGBYTES)(sp)
	FPLOAD	f26, (32 * REGBYTES + 26 * FPREGBYTES)(sp)
	FPLOAD	f27, (32 * REGBYTES + 27 * FPREGBYTES)(sp)
	FPLOAD	f28, (32 * REGBYTES + 28 * FPREGBYTES)(sp)
	FPLOAD	f29, (32 * REGBYTES + 29 * FPREGBYTES)(sp)
	FPLOAD	f30, (32 * REGBYTES + 30 * FPREGBYTES)(sp)
	FPLOAD	f31, (32 * REGBYTES + 31 * FPREGBYTES)(sp)

	addi	sp, sp, (REGBYTES + FPREGBYTES) * 32
#else
	addi	sp, sp, REGBYTES * 32
#endif
	mret
	.endm

/* Macro for restoring task context */
FreeRTOS_MTIMER_Handler:
	portSAVE_CONTEXT
	portSAVE_EPC
	jal	vPortSysTickHandler
	portRESTORE_CONTEXT

xPortStartScheduler:
#ifdef __gracefulExit
	/* Stores context when starting the scheduler in xStartContext.
	   This is used for when you want to gracefully exit the scheduler.
	   For example if you want to test multiple instances after each other in one test suite.
     */
	la		t0, xStartContext
	STORE	x1, 0x0(t0)
	STORE	x2, 1 * REGBYTES(t0)
	STORE	x3, 2 * REGBYTES(t0)
	STORE	x4, 3 * REGBYTES(t0)
	STORE	x5, 4 * REGBYTES(t0)
	STORE	x6, 5 * REGBYTES(t0)
	STORE	x7, 6 * REGBYTES(t0)
	STORE	x8, 7 * REGBYTES(t0)
	STORE	x9, 8 * REGBYTES(t0)
	STORE	x10, 9 * REGBYTES(t0)
	STORE	x11, 10 * REGBYTES(t0)
	STORE	x12, 11 * REGBYTES(t0)
	STORE	x13, 12 * REGBYTES(t0)
	STORE	x14, 13 * REGBYTES(t0)
	STORE	x15, 14 * REGBYTES(t0)
#ifndef __riscv_32e
	STORE	x16, 15 * REGBYTES(t0)
	STORE	x17, 16 * REGBYTES(t0)
	STORE	x18, 17 * REGBYTES(t0)
	STORE	x19, 18 * REGBYTES(t0)
	STORE	x20, 19 * REGBYTES(t0)
	STORE	x21, 20 * REGBYTES(t0)
	STORE	x22, 21 * REGBYTES(t0)
	STORE	x23, 22 * REGBYTES(t0)
	STORE	x24, 23 * REGBYTES(t0)
	STORE	x25, 24 * REGBYTES(t0)
	STORE	x26, 25 * REGBYTES(t0)
	STORE	x27, 26 * REGBYTES(t0)
	STORE	x28, 27 * REGBYTES(t0)
	STORE	x29, 28 * REGBYTES(t0)
	STORE	x30, 29 * REGBYTES(t0)
	STORE	x31, 30 * REGBYTES(t0)
#endif
#endif
	jal	vPortSetupTimer
	portRESTORE_CONTEXT

vPortEndScheduler:
#ifdef __gracefulExit
	/* Load current context from xStartContext */
	la	t0, xStartContext
	LOAD	x1, 0x0(t0)
	LOAD	x2, 1 * REGBYTES(t0)
	LOAD	x3, 2 * REGBYTES(t0)
	LOAD	x4, 3 * REGBYTES(t0)
	LOAD	x5, 4 * REGBYTES(t0)
	LOAD	x6, 5 * REGBYTES(t0)
	LOAD	x7, 6 * REGBYTES(t0)
	LOAD	x8, 7 * REGBYTES(t0)
	LOAD	x9, 8 * REGBYTES(t0)
	LOAD	x10, 9 * REGBYTES(t0)
	LOAD	x11, 10 * REGBYTES(t0)
	LOAD	x12, 11 * REGBYTES(t0)
	LOAD	x13, 12 * REGBYTES(t0)
	LOAD	x14, 13 * REGBYTES(t0)
	LOAD	x15, 14 * REGBYTES(t0)
#ifndef __riscv_32e
	LOAD	x16, 15 * REGBYTES(t0)
	LOAD	x17, 16 * REGBYTES(t0)
	LOAD	x18, 17 * REGBYTES(t0)
	LOAD	x19, 18 * REGBYTES(t0)
	LOAD	x20, 19 * REGBYTES(t0)
	LOAD	x21, 20 * REGBYTES(t0)
	LOAD	x22, 21 * REGBYTES(t0)
	LOAD	x23, 22 * REGBYTES(t0)
	LOAD	x24, 23 * REGBYTES(t0)
	LOAD	x25, 24 * REGBYTES(t0)
	LOAD	x26, 25 * REGBYTES(t0)
	LOAD	x27, 26 * REGBYTES(t0)
	LOAD	x28, 27 * REGBYTES(t0)
	LOAD	x29, 28 * REGBYTES(t0)
	LOAD	x30, 39 * REGBYTES(t0)
	LOAD	x31, 30 * REGBYTES(t0)
#endif
#endif
	ret

vPortYield:
	/*
	 *  This routine can be called from outside of interrupt handler. This means
	 *  interrupts may be enabled at this point. This is probably okay for registers and
	 *  stack. However, "mepc" will be overwritten by the interrupt handler if a timer
	 *  interrupt happens during the yield. To avoid this, prevent interrupts before starting.
	 *  The write to mstatus in the restore context routine will enable interrupts after the
	 *  mret. A more fine-grain lock may be possible.
	*/
	csrc	mstatus,8

	portSAVE_CONTEXT
	portSAVE_RA
	jal	vTaskSwitchContext
	portRESTORE_CONTEXT


vPortSAVE_CONTEXT:
  portSAVE_CONTEXT


vPortRESTORE_CONTEXT:
  portRESTORE_CONTEXT
