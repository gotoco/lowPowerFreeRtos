/**
 * \file rcc.cpp
 * \brief RCC driver.
 *
 * RCC default configuration and function for configuring a pin
 *
 * chip: STM32L1xx; prefix: rcc
 *
 * \author: Mazeryt Freager
 * \date 2012-08-30
 */

#include <stdint.h>

#include "stm32l152xb.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_bitband.h"

#include "config.h"

#include "rcc.h"

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static void _flashLatency(uint32_t frequency);

/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

static uint32_t _coreFrequency = 2097000;	///< frequency of the core

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Returns current core frequency in Hz.
 *
 * Returns current core frequency in Hz.
 *
 * \return current core frequency in Hz
 */

uint32_t rccGetCoreFrequency(void)
{
	return _coreFrequency;
}

/**
 * \brief Starts the PLL
 *
 * Configures and enables PLL to achieve some frequency with some input frequency. The clock source must be enabled and
 * running! Before the speed change Flash latency is configured via _flashLatency(). PLL parameters are based on
 * function parameters. The PLL is set up, started and connected. Clock ratios are set to 1:1 for APB1, APB2 and AHB.
 *
 * \param [in] pll_input selects the source of PLL input clock, allowed values {RCC_PLL_INPUT_HSI, RCC_PLL_INPUT_HSE}
 * \param [in] input_frequency is the input frequency for the PLL in Hz
 * \param [in] otput_frequency is the desired target frequency after enabling the PLL
 *
 * \return real frequency that was set
 */

uint32_t rccStartPll(enum rccPllInput pll_input, uint32_t input_frequency, uint32_t output_frequency)
{
#if DEVICE_VCORE_mV == 1800
	static const uint32_t pllvco_max = 96000000;
#elif DEVICE_VCORE_mV == 1500
	static const uint32_t pllvco_max = 48000000;
#elif DEVICE_VCORE_mV == 1200
	static const uint32_t pllvco_max = 24000000;
#else
	#error "Invalid value of DEVICE_VCORE_mV!"
#endif

	static const uint8_t muls[] = {3, 4, 6, 8, 12, 16, 24, 32, 48};	// allowed values of PLL multiplier
	uint32_t mul_i;
	uint32_t best_frequency = 0, best_mul_i = 0, best_div = 0;

	for (mul_i = 0; mul_i < sizeof (muls) / sizeof (muls[0]); mul_i++)	// loop through all multipliers
	{
		uint32_t pllvco = input_frequency * muls[mul_i];

		if (pllvco > pllvco_max)			// internal PLL frequency out of valid range?
			continue;

		uint32_t div;

		for (div = 2; div <= 4; div++)		// loop through all dividers
		{
			uint32_t frequency = pllvco / div;

			if (frequency > output_frequency)	// resulting frequency too high?
				continue;

			if (frequency >= best_frequency)	// is this configuration better than previously found?
			{
				best_frequency = frequency;	// yes - store values
				best_mul_i = mul_i;
				best_div = div;

				if (best_frequency == output_frequency && pllvco/2 == 48000000)	// is this a perfect match and deliver 48MHz for USB?
					break;
			}
		}

	}

	_flashLatency(best_frequency);			// configure flash latency using found frequency

	RCC->CFGR = ((best_div - 1) << RCC_CFGR_PLLDIV_bit) | (best_mul_i << RCC_CFGR_PLLMUL_bit) |
			(pll_input << RCC_CFGR_PLLSRC_bit);	// set PLL

	RCC_CR_PLLON_bb = 1;					// enable PLL and wait for stabilization
	while (RCC_CR_PLLRDY_bb == 0);

	RCC->CFGR |= RCC_CFGR_SW_PLL;			// change SYSCLK to PLL
	while (((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	// wait for switch

	_coreFrequency = best_frequency;

	RCC->APB2ENR |= (RCC_APB2ENR_SYSCFGEN);

	return best_frequency;
}

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Configures Flash latency.
 *
 * Configures Flash latency (wait-states) which allows the chip to run at higher speeds.
 *
 * \param [in] frequency defines the target frequency of the core
 */

static void _flashLatency(uint32_t frequency)
{
#if DEVICE_VCORE_mV == 1800

	uint32_t wait_states;

	if (frequency > 16000000)
		wait_states = 1;
	else
		wait_states = 0;

    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR = (FLASH->ACR & (~FLASH_ACR_LATENCY)) | FLASH_ACR_PRFTEN | wait_states;

#else
	#error "Invalid flash latency configuration for device's VCORE!"
#endif
}
