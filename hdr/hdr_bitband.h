/**
 * \file hdr_bitband.h
 * \brief Header for bit-banding
 *
 * Header for bit-banding
 *
 * chip: ARMv7-M(E) (Cortex-M3 / Cortex-M4)
 *
 * \author grochu
 * \date 2012-07-24
 */

#ifndef HDR_BITBAND_H_
#define HDR_BITBAND_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define bitband_t							*(volatile unsigned long*)

#define BITBAND_SRAM_REF   					0x20000000	///< start of SRAM region with bit-band alias
#define BITBAND_SRAM_END   					0x200FFFFF	///< end of SRAM region with bit-band alias
#define BITBAND_SRAM_BASE  					0x22000000	///< start of bit-band alias region of SRAM

#define BITBAND_PERIPH_REF   				0x40000000	///< start of peripherals region with bit-band alias
#define BITBAND_PERIPH_END   				0x400FFFFF	///< end of peripherals region with bit-band alias
#define BITBAND_PERIPH_BASE  				0x42000000	///< start of bit-band alias region of peripherals

/// address of bitband for SRAM region
#define BITBAND_SRAM_ADDRESS(address, bit)		(BITBAND_SRAM_BASE + (((unsigned long)address) - BITBAND_SRAM_REF) * 32 + (bit) * 4)
/// address of bitband for peripherals region
#define BITBAND_PERIPH_ADDRESS(address, bit)	(BITBAND_PERIPH_BASE + (((unsigned long)address) - BITBAND_PERIPH_REF) * 32 + (bit) * 4)
/// address of bitband for any region
#define BITBAND_ADDRESS(address, bit)		( \
	(((unsigned long)address) >= BITBAND_SRAM_REF) && (((unsigned long)address) <= BITBAND_SRAM_END) ? BITBAND_SRAM_ADDRESS(address, bit) : \
	(((unsigned long)address) >= BITBAND_PERIPH_REF) && (((unsigned long)address) <= BITBAND_PERIPH_END) ? BITBAND_PERIPH_ADDRESS(address, bit) : \
	0 /* fail */ \
	)

/// bitband variable in SRAM region
#define BITBAND_SRAM(address, bit)			(bitband_t BITBAND_SRAM_ADDRESS(address, bit))
/// bitband variable in peripherals region
#define BITBAND_PERIPH(address, bit)		(bitband_t BITBAND_PERIPH_ADDRESS(address, bit))
/// bitband variable in any region
#define BITBAND(address, bit)				(bitband_t BITBAND_ADDRESS(address, bit))

#endif /* HDR_BITBAND_H_ */
