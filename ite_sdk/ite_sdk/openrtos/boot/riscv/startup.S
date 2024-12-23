/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "core_v5.h"

	.section .text

	.global reset_vector

reset_vector:
	/* Decide whether this is an NMI or cold reset */
	csrr t0, mcause
	bnez t0, nmi_handler

	.global _start
	.type _start,@function

_start:
	/* Initialize global pointer */
	.option push
	.option norelax
	la gp, _gp
	.option pop

	/* Initialize stack pointer */
	la t0, __c_stack_top__
	mv sp, t0

#ifdef __nds_execit
	/* Initialize EXECIT table */
	la t0, _ITB_BASE_
	csrw uitb, t0
#endif

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, MSTATUS_FS
	csrrs t0, mstatus, t0

	/* Initialize FCSR */
	fscsr zero
#endif

	/* Initial machine trap-vector Base */
	la t0, trap_entry
	csrw mtvec, t0

#ifdef CFG_CACHE_ENABLE
	/* Enable I/D cache */
	csrrs t0, mcache_ctl, 3
#endif

	/* Do system low level setup. It must be a leaf function */
	call __platform_init

	/* System reset handler */
	call reset_handler

	/* Infinite loop, if returned accidently */
1:	j 1b

// *****************************************************************************
// Version Information
// *****************************************************************************
    .org    0x54
    .global __version_major             // uint32_t __version_major = CFG_VERSION_MAJOR;
__version_major:
    .word   CFG_VERSION_MAJOR
    .global __version_minor             // uint32_t __version_minor = CFG_VERSION_MINOR;
__version_minor:
    .word   CFG_VERSION_MINOR
    .global __version_patch             // uint32_t __version_patch = CFG_VERSION_PATCH;
__version_patch:
    .word   CFG_VERSION_PATCH
    .global __version_custom            // uint32_t __version_custom = CFG_VERSION_CUSTOM;
__version_custom:
    .word   CFG_VERSION_CUSTOM
    .global __version_tweak             // uint32_t __version_tweak = CFG_VERSION_TWEAK;
__version_tweak:
    .word   CFG_VERSION_TWEAK

// *****************************************************************************
// Pointers of Print Buffer
// *****************************************************************************
    .org    0x1000 - 12
    .global __printbuf_addr
__printbuf_addr:
    .word   __printbuf_base
    .global __printbuf_size
__printbuf_size:
    .word   CFG_DBG_PRINTBUF_SIZE
    .global __printbuf_ptr
__printbuf_ptr:
    .word   0

    .org    0x1000
#ifdef CFG_XCPU_MSGQ
// *****************************************************************************
// _xcpu_msgq
// *****************************************************************************
_xcpu_msgq:
    .global _xcpu_msgq
    .fill CFG_XCPU_MSGQ_SIZE

#endif // CFG_XCPU_MSGQ


	.weak __platform_init
__platform_init:
	ret

	.section .text

	.global trap_entry
	.align 2

trap_entry:
	/* Check for Machine timer interrupt */
	STORE t0, -1*REGBYTES(sp)
	csrr t0, mcause
	bgez t0, 1f
	andi t0, t0, 255
	addi t0, t0, -7
	bnez t0, 1f
	LOAD t0, -1*REGBYTES(sp)
	tail ithIntrDoIrq

1:
	LOAD t0, -1*REGBYTES(sp)
#ifdef __riscv_flen
	addi sp, sp, -32*REGBYTES - 16*FPREGBYTES
#else
	addi sp, sp, -32*REGBYTES
#endif

	/* Save caller registers */
	STORE x1, 0*REGBYTES(sp)
	STORE x4, 1*REGBYTES(sp)
	STORE x5, 2*REGBYTES(sp)
	STORE x6, 3*REGBYTES(sp)
	STORE x7, 4*REGBYTES(sp)
	STORE x10, 5*REGBYTES(sp)
	STORE x11, 6*REGBYTES(sp)
	STORE x12, 7*REGBYTES(sp)
	STORE x13, 8*REGBYTES(sp)
	STORE x14, 9*REGBYTES(sp)
	STORE x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
	STORE x16, 11*REGBYTES(sp)
	STORE x17, 12*REGBYTES(sp)
	STORE x28, 13*REGBYTES(sp)
	STORE x29, 14*REGBYTES(sp)
	STORE x30, 15*REGBYTES(sp)
	STORE x31, 16*REGBYTES(sp)
#endif

	/* Save mstatus and mepc */
	csrr t0, mepc
	csrr t1, mstatus

	STORE t0, 17*REGBYTES(sp)
	STORE t1, 18*REGBYTES(sp)

#ifdef __riscv_flen
	/* Save fp caller registers */
	FPSTORE f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
	FPSTORE f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
	FPSTORE f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
	FPSTORE f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
	FPSTORE f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
	FPSTORE f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
	FPSTORE f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
	FPSTORE f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
	FPSTORE f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
	FPSTORE f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
	FPSTORE f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
	FPSTORE f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
	FPSTORE f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
	FPSTORE f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
	FPSTORE f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
	FPSTORE f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
	FPSTORE f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
	FPSTORE f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
	FPSTORE f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
	FPSTORE f31, (20*REGBYTES + 19*FPREGBYTES)(sp)
#endif

	/* Call trap handler */
	csrr a0, mcause
	mv a1, sp
	call trap_handler

#ifdef __riscv_flen
	/* Restore fp caller registers */
	FPLOAD f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
	FPLOAD f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
	FPLOAD f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
	FPLOAD f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
	FPLOAD f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
	FPLOAD f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
	FPLOAD f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
	FPLOAD f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
	FPLOAD f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
	FPLOAD f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
	FPLOAD f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
	FPLOAD f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
	FPLOAD f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
	FPLOAD f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
	FPLOAD f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
	FPLOAD f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
	FPLOAD f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
	FPLOAD f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
	FPLOAD f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
	FPLOAD f31, (20*REGBYTES + 19*FPREGBYTES)(sp)
#endif

	/* Disable interrupt */
	csrrci a5, mstatus, MSTATUS_MIE

	/* Restore mstatus and mepc */
	LOAD t0, 17*REGBYTES(sp)
	LOAD t1, 18*REGBYTES(sp)

	csrw mepc, t0
	csrw mstatus, t1

	/* Restore caller registers */
	LOAD x1, 0*REGBYTES(sp)
	LOAD x4, 1*REGBYTES(sp)
	LOAD x5, 2*REGBYTES(sp)
	LOAD x6, 3*REGBYTES(sp)
	LOAD x7, 4*REGBYTES(sp)
	LOAD x10, 5*REGBYTES(sp)
	LOAD x11, 6*REGBYTES(sp)
	LOAD x12, 7*REGBYTES(sp)
	LOAD x13, 8*REGBYTES(sp)
	LOAD x14, 9*REGBYTES(sp)
	LOAD x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
	LOAD x16, 11*REGBYTES(sp)
	LOAD x17, 12*REGBYTES(sp)
	LOAD x28, 13*REGBYTES(sp)
	LOAD x29, 14*REGBYTES(sp)
	LOAD x30, 15*REGBYTES(sp)
	LOAD x31, 16*REGBYTES(sp)
#endif

#ifdef __riscv_flen
	addi sp, sp, 32*REGBYTES + 16*FPREGBYTES
#else
	addi sp, sp, 32*REGBYTES
#endif

	mret

	.weak trap_handler
trap_handler:
1:	j 1b

	.weak nmi_handler
nmi_handler:
1:	j 1b
