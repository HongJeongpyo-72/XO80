/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "core_v5.h"
#include "nds_v5_platform.h"

extern void reset_vector(void);
extern void BootInit(void);

/* This must be a leaf function, no child function */
void __platform_init (void) __attribute__((naked));
void __platform_init(void)
{
	/* Do your platform low-level initial */

	__asm("ret");
}

void c_startup(void)
{
#define MEMCPY(des, src, n)     __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n)         __builtin_memset ((s), (c), (n))
	/* Data section initialization */
	extern char  __bss_start__, __bss_end__;
	unsigned int size;

	/* Clear bss section */
	size = &__bss_end__ - &__bss_start__;
	MEMSET(&__bss_start__, 0, size);
}

void system_init(void)
{
	/*
	 * Do your system reset handling here
	 */
	/* Enable PLIC features */
	if (read_csr(NDS_MMISC_CTL) & (1 << 1)) {
		/* External PLIC interrupt is vectored */
		//__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT | NDS_PLIC_FEATURE_VECTORED);
	} else {
		/* External PLIC interrupt is NOT vectored */
		//__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT);
	}

	/* Enable Misaligned access */
	set_csr(NDS_MMISC_CTL, (1 << 6));

	BootInit();
}
