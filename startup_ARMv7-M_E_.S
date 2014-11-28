/* file: startup_ARMv7-M_E_.S
 *
 * ARMv7-M(E) (Cortex-M3 / Cortex-M4) assembly startup code
 *
 * chip: ARMv7-M(E) (Cortex-M3 / Cortex-M4)
 *
 * author: Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * date: 2012-07-12
 */

/*-----------------------------------------------------------------------------+
| includes
+-----------------------------------------------------------------------------*/

#include "hdr/hdr_special_registers.h"

/*-----------------------------------------------------------------------------+
| ARMv7-M(E) (Cortex-M3 / Cortex-M4) startup code
+-----------------------------------------------------------------------------*/

.text
.balign 2
.syntax unified
.thumb
.thumb_func
.global Reset_Handler

Reset_Handler:

// Initialize the process stack pointer

	ldr		r0, =__process_stack_end
	msr		PSP, r0

// Thread mode uses process stack (PSP) and is privileged

	movs	r0, #CONTROL_ALTERNATE_STACK
	msr		CONTROL, r0
	isb

// Branch to low_level_init_0() function (.data and .bss are not initialized!)

	ldr		r0, =low_level_init_0
	blx		r0

// Initialize sections from data_array (including .data)

	ldr		r4, =__data_array_start
	ldr		r5, =__data_array_end

1:	cmp		r4, r5							// outer loop - addresses from data_array
	ittte	lo
	ldrlo	r1, [r4], #4					// start of source address
	ldrlo	r2, [r4], #4					// start of destination address
	ldrlo	r3, [r4], #4					// end of destination address
	bhs		3f

2:	cmp		r2, r3							// inner loop - section initialization
	ittt	lo
	ldrlo	r0, [r1], #4
	strlo	r0, [r2], #4
	blo		2b

	b		1b								// go back to start

3:

// Zero-init sections from bss_array (including .bss)

	movs	r0, #0
	ldr		r3, =__bss_array_start
	ldr		r4, =__bss_array_end

1:	cmp		r3, r4							// outer loop - addresses from data_array
	itte	lo
	ldrlo	r1, [r3], #4					// start of destination address
	ldrlo	r2, [r3], #4					// end of destination address
	bhs		3f

2:	cmp		r1, r2							// inner loop - section initialization
	itt		lo
	strlo	r0, [r1], #4
	blo		2b

	b		1b								// go back to start

3:

// Call C++ constructors for global and static objects

#ifdef __USES_CXX
	ldr		r0, =__libc_init_array
	blx		r0
#endif

// Branch to low_level_init_1() function

	ldr		r0, =low_level_init_1
	blx		r0

// Branch to main() with link

	ldr		r0, =main
	blx		r0

// Call C++ destructors for global and static objects

#ifdef __USES_CXX
	ldr		r0, =__libc_fini_array
	blx		r0
#endif

// On return - loop till the end of the world

	b		.

/*-----------------------------------------------------------------------------+
| __default_low_level_init() - replacement for undefined low_level_init_0()
| and/or low_level_init_1(). This function just returns.
+-----------------------------------------------------------------------------*/

.text
.balign 2
.syntax unified
.thumb
.thumb_func
.global __default_low_level_init

__default_low_level_init:
	bx		lr

/*-----------------------------------------------------------------------------+
| assign undefined low_level_init_0() and/or low_level_init_1() to
| __default_low_level_init()
+-----------------------------------------------------------------------------*/

.weak	low_level_init_0
.global	low_level_init_0
.set	low_level_init_0, __default_low_level_init

.weak	low_level_init_1
.global	low_level_init_1
.set	low_level_init_1, __default_low_level_init
