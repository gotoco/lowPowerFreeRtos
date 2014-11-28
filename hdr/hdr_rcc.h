/**
 * \file hdr_rcc.h
 * \brief Header with definition of bits in RCC control registers
 *
 * Header with definition of bits in RCC control registers
 *
 * chip: STM32L1xx
 *
 * \author Mazeryt Freager
 * \date 2012-08-24
 */

#ifndef HDR_RCC_H_
#define HDR_RCC_H_

#include "hdr/hdr_bitband.h"

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_CR - Clock Control Register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_CR_RTCPRE_bit					29
#define RCC_CR_RTCPRE_0_bit					29
#define RCC_CR_RTCPRE_1_bit					30
#define RCC_CR_CSSON_bit					28
#define RCC_CR_PLLRDY_bit					25
#define RCC_CR_PLLON_bit					24
#define RCC_CR_HSEBYP_bit					18
#define RCC_CR_HSERDY_bit					17
#define RCC_CR_HSEON_bit					16
#define RCC_CR_MSIRDY_bit					9
#define RCC_CR_MSION_bit					8
#define RCC_CR_HSIRDY_bit					1
#define RCC_CR_HSION_bit					0

#define RCC_CR_RTCPRE_DIV2_value			0
#define RCC_CR_RTCPRE_DIV4_value			1
#define RCC_CR_RTCPRE_DIV8_value			2
#define RCC_CR_RTCPRE_DIV16_value			3
#define RCC_CR_RTCPRE_mask					3

#define RCC_CR_RTCPRE_DIV2					(RCC_CR_RTCPRE_DIV2_value << RCC_CR_RTCPRE_bit)
#define RCC_CR_RTCPRE_DIV4					(RCC_CR_RTCPRE_DIV4_value << RCC_CR_RTCPRE_bit)
#define RCC_CR_RTCPRE_DIV8					(RCC_CR_RTCPRE_DIV8_value << RCC_CR_RTCPRE_bit)
#define RCC_CR_RTCPRE_DIV16					(RCC_CR_RTCPRE_DIV16_value << RCC_CR_RTCPRE_bit)

#define RCC_CR_RTCPRE_0_bb					BITBAND(&RCC->CR, RCC_CR_RTCPRE_0_bit)
#define RCC_CR_RTCPRE_1_bb					BITBAND(&RCC->CR, RCC_CR_RTCPRE_1_bit)
#define RCC_CR_CSSON_bb						BITBAND(&RCC->CR, RCC_CR_CSSON_bit)
#define RCC_CR_PLLRDY_bb					BITBAND(&RCC->CR, RCC_CR_PLLRDY_bit)
#define RCC_CR_PLLON_bb						BITBAND(&RCC->CR, RCC_CR_PLLON_bit)
#define RCC_CR_HSEBYP_bb					BITBAND(&RCC->CR, RCC_CR_HSEBYP_bit)
#define RCC_CR_HSERDY_bb					BITBAND(&RCC->CR, RCC_CR_HSERDY_bit)
#define RCC_CR_HSEON_bb						BITBAND(&RCC->CR, RCC_CR_HSEON_bit)
#define RCC_CR_MSIRDY_bb					BITBAND(&RCC->CR, RCC_CR_MSIRDY_bit)
#define RCC_CR_MSION_bb						BITBAND(&RCC->CR, RCC_CR_MSION_bit)
#define RCC_CR_HSIRDY_bb					BITBAND(&RCC->CR, RCC_CR_HSIRDY_bit)
#define RCC_CR_HSION_bb						BITBAND(&RCC->CR, RCC_CR_HSION_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_ICSCR - Internal clock sources calibration register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_ICSCR_MSITRIM_bit				24
#define RCC_ICSCR_MSITRIM_0_bit				24
#define RCC_ICSCR_MSITRIM_1_bit				25
#define RCC_ICSCR_MSITRIM_2_bit				26
#define RCC_ICSCR_MSITRIM_3_bit				27
#define RCC_ICSCR_MSITRIM_4_bit				28
#define RCC_ICSCR_MSITRIM_5_bit				29
#define RCC_ICSCR_MSITRIM_6_bit				30
#define RCC_ICSCR_MSITRIM_7_bit				31

#define RCC_ICSCR_MSICAL_bit				16
#define RCC_ICSCR_MSICAL_0_bit				16
#define RCC_ICSCR_MSICAL_1_bit				17
#define RCC_ICSCR_MSICAL_2_bit				18
#define RCC_ICSCR_MSICAL_3_bit				19
#define RCC_ICSCR_MSICAL_4_bit				20
#define RCC_ICSCR_MSICAL_5_bit				21
#define RCC_ICSCR_MSICAL_6_bit				22
#define RCC_ICSCR_MSICAL_7_bit				23

#define RCC_ICSCR_MSIRANGE_bit				13
#define RCC_ICSCR_MSIRANGE_0_bit			13
#define RCC_ICSCR_MSIRANGE_1_bit			14
#define RCC_ICSCR_MSIRANGE_2_bit			15

#define RCC_ICSCR_HSITRIM_bit				8
#define RCC_ICSCR_HSITRIM_0_bit				8
#define RCC_ICSCR_HSITRIM_1_bit				9
#define RCC_ICSCR_HSITRIM_2_bit				10
#define RCC_ICSCR_HSITRIM_3_bit				11
#define RCC_ICSCR_HSITRIM_4_bit				12

#define RCC_ICSCR_HSICAL_bit				0
#define RCC_ICSCR_HSICAL_0_bit				0
#define RCC_ICSCR_HSICAL_1_bit				1
#define RCC_ICSCR_HSICAL_2_bit				2
#define RCC_ICSCR_HSICAL_3_bit				3
#define RCC_ICSCR_HSICAL_4_bit				4
#define RCC_ICSCR_HSICAL_5_bit				5
#define RCC_ICSCR_HSICAL_6_bit				6
#define RCC_ICSCR_HSICAL_7_bit				7

#define RCC_ICSCR_MSITRIM_0					(1 << RCC_ICSCR_MSITRIM_0_bit)
#define RCC_ICSCR_MSITRIM_1					(1 << RCC_ICSCR_MSITRIM_1_bit)
#define RCC_ICSCR_MSITRIM_2					(1 << RCC_ICSCR_MSITRIM_2_bit)
#define RCC_ICSCR_MSITRIM_3					(1 << RCC_ICSCR_MSITRIM_3_bit)
#define RCC_ICSCR_MSITRIM_4					(1 << RCC_ICSCR_MSITRIM_4_bit)
#define RCC_ICSCR_MSITRIM_5					(1 << RCC_ICSCR_MSITRIM_5_bit)
#define RCC_ICSCR_MSITRIM_6					(1 << RCC_ICSCR_MSITRIM_6_bit)
#define RCC_ICSCR_MSITRIM_7					(1 << RCC_ICSCR_MSITRIM_7_bit)

#define RCC_ICSCR_MSICAL_0					(1 << RCC_ICSCR_MSICAL_0_bit)
#define RCC_ICSCR_MSICAL_1					(1 << RCC_ICSCR_MSICAL_1_bit)
#define RCC_ICSCR_MSICAL_2					(1 << RCC_ICSCR_MSICAL_2_bit)
#define RCC_ICSCR_MSICAL_3					(1 << RCC_ICSCR_MSICAL_3_bit)
#define RCC_ICSCR_MSICAL_4					(1 << RCC_ICSCR_MSICAL_4_bit)
#define RCC_ICSCR_MSICAL_5					(1 << RCC_ICSCR_MSICAL_5_bit)
#define RCC_ICSCR_MSICAL_6					(1 << RCC_ICSCR_MSICAL_6_bit)
#define RCC_ICSCR_MSICAL_7					(1 << RCC_ICSCR_MSICAL_7_bit)

/*#define RCC_ICSCR_MSIRANGE_0				(1 << RCC_ICSCR_MSIRANGE_0_bit)
#define RCC_ICSCR_MSIRANGE_1				(1 << RCC_ICSCR_MSIRANGE_1_bit)
#define RCC_ICSCR_MSIRANGE_2				(1 << RCC_ICSCR_MSIRANGE_2_bit)*/	// disabled due to name clash with current stm32l1xx.h

#define RCC_ICSCR_HSITRIM_0					(1 << RCC_ICSCR_HSITRIM_0_bit)
#define RCC_ICSCR_HSITRIM_1					(1 << RCC_ICSCR_HSITRIM_1_bit)
#define RCC_ICSCR_HSITRIM_2					(1 << RCC_ICSCR_HSITRIM_2_bit)
#define RCC_ICSCR_HSITRIM_3					(1 << RCC_ICSCR_HSITRIM_3_bit)
#define RCC_ICSCR_HSITRIM_4					(1 << RCC_ICSCR_HSITRIM_4_bit)

#define RCC_ICSCR_HSICAL_0					(1 << RCC_ICSCR_HSICAL_0_bit)
#define RCC_ICSCR_HSICAL_1					(1 << RCC_ICSCR_HSICAL_1_bit)
#define RCC_ICSCR_HSICAL_2					(1 << RCC_ICSCR_HSICAL_2_bit)
#define RCC_ICSCR_HSICAL_3					(1 << RCC_ICSCR_HSICAL_3_bit)
#define RCC_ICSCR_HSICAL_4					(1 << RCC_ICSCR_HSICAL_4_bit)
#define RCC_ICSCR_HSICAL_5					(1 << RCC_ICSCR_HSICAL_5_bit)
#define RCC_ICSCR_HSICAL_6					(1 << RCC_ICSCR_HSICAL_6_bit)
#define RCC_ICSCR_HSICAL_7					(1 << RCC_ICSCR_HSICAL_7_bit)

#define RCC_ICSCR_MSIRANGE_65_536kHz_value	0
#define RCC_ICSCR_MSIRANGE_131_072kHz_value	1
#define RCC_ICSCR_MSIRANGE_262_144kHz_value	2
#define RCC_ICSCR_MSIRANGE_524_288kHz_value	3
#define RCC_ICSCR_MSIRANGE_1_048MHz_value	4
#define RCC_ICSCR_MSIRANGE_2_097MHz_value	5
#define RCC_ICSCR_MSIRANGE_4_194MHz_value	6
#define RCC_ICSCR_MSIRANGE_mask				7

#define RCC_ICSCR_MSIRANGE_65_536kHz		(RCC_ICSCR_MSIRANGE_65_536kHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_131_072kHz		(RCC_ICSCR_MSIRANGE_131_072kHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_262_144kHz		(RCC_ICSCR_MSIRANGE_262_144kHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_524_288kHz		(RCC_ICSCR_MSIRANGE_524_288kHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_1_048MHz			(RCC_ICSCR_MSIRANGE_1_048MHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_2_097MHz			(RCC_ICSCR_MSIRANGE_2_097MHz_value << RCC_ICSCR_MSIRANGE_bit)
#define RCC_ICSCR_MSIRANGE_4_194MHz			(RCC_ICSCR_MSIRANGE_4_194MHz_value << RCC_ICSCR_MSIRANGE_bit)

#define RCC_ICSCR_MSITRIM_0_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_0_bit)
#define RCC_ICSCR_MSITRIM_1_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_1_bit)
#define RCC_ICSCR_MSITRIM_2_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_2_bit)
#define RCC_ICSCR_MSITRIM_3_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_3_bit)
#define RCC_ICSCR_MSITRIM_4_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_4_bit)
#define RCC_ICSCR_MSITRIM_5_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_5_bit)
#define RCC_ICSCR_MSITRIM_6_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_6_bit)
#define RCC_ICSCR_MSITRIM_7_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSITRIM_7_bit)

#define RCC_ICSCR_MSICAL_0_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_0_bit)
#define RCC_ICSCR_MSICAL_1_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_1_bit)
#define RCC_ICSCR_MSICAL_2_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_2_bit)
#define RCC_ICSCR_MSICAL_3_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_3_bit)
#define RCC_ICSCR_MSICAL_4_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_4_bit)
#define RCC_ICSCR_MSICAL_5_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_5_bit)
#define RCC_ICSCR_MSICAL_6_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_6_bit)
#define RCC_ICSCR_MSICAL_7_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSICAL_7_bit)

#define RCC_ICSCR_MSIRANGE_0_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSIRANGE_0_bit)
#define RCC_ICSCR_MSIRANGE_1_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSIRANGE_1_bit)
#define RCC_ICSCR_MSIRANGE_2_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_MSIRANGE_2_bit)

#define RCC_ICSCR_HSITRIM_0_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSITRIM_0_bit)
#define RCC_ICSCR_HSITRIM_1_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSITRIM_1_bit)
#define RCC_ICSCR_HSITRIM_2_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSITRIM_2_bit)
#define RCC_ICSCR_HSITRIM_3_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSITRIM_3_bit)
#define RCC_ICSCR_HSITRIM_4_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSITRIM_4_bit)

#define RCC_ICSCR_HSICAL_0_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_0_bit)
#define RCC_ICSCR_HSICAL_1_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_1_bit)
#define RCC_ICSCR_HSICAL_2_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_2_bit)
#define RCC_ICSCR_HSICAL_3_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_3_bit)
#define RCC_ICSCR_HSICAL_4_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_4_bit)
#define RCC_ICSCR_HSICAL_5_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_5_bit)
#define RCC_ICSCR_HSICAL_6_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_6_bit)
#define RCC_ICSCR_HSICAL_7_bb				BITBAND(&RCC->ICSCR, RCC_ICSCR_HSICAL_7_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_CFGR - Clock Configuration Register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_CFGR_MCOPRE_bit					28
#define RCC_CFGR_MCOPRE_0_bit				28
#define RCC_CFGR_MCOPRE_1_bit				29
#define RCC_CFGR_MCOPRE_2_bit				30

#define RCC_CFGR_MCOSEL_bit					24
#define RCC_CFGR_MCOSEL_0_bit				24
#define RCC_CFGR_MCOSEL_1_bit				25
#define RCC_CFGR_MCOSEL_2_bit				26

#define RCC_CFGR_PLLDIV_bit					22
#define RCC_CFGR_PLLDIV_0_bit				22
#define RCC_CFGR_PLLDIV_1_bit				23

#define RCC_CFGR_PLLMUL_bit					18
#define RCC_CFGR_PLLMUL_0_bit				18
#define RCC_CFGR_PLLMUL_1_bit				19
#define RCC_CFGR_PLLMUL_2_bit				20
#define RCC_CFGR_PLLMUL_3_bit				21

#define RCC_CFGR_PLLSRC_bit					16

#define RCC_CFGR_PPRE2_bit					11
#define RCC_CFGR_PPRE2_0_bit				11
#define RCC_CFGR_PPRE2_1_bit				12
#define RCC_CFGR_PPRE2_2_bit				13

#define RCC_CFGR_PPRE1_bit					8
#define RCC_CFGR_PPRE1_0_bit				8
#define RCC_CFGR_PPRE1_1_bit				9
#define RCC_CFGR_PPRE1_2_bit				10

#define RCC_CFGR_HPRE_bit					4
#define RCC_CFGR_HPRE_0_bit					4
#define RCC_CFGR_HPRE_1_bit					5
#define RCC_CFGR_HPRE_2_bit					6
#define RCC_CFGR_HPRE_3_bit					7

#define RCC_CFGR_SWS_bit					2
#define RCC_CFGR_SWS_0_bit					2
#define RCC_CFGR_SWS_1_bit					3

#define RCC_CFGR_SW_bit						0
#define RCC_CFGR_SW_0_bit					0
#define RCC_CFGR_SW_1_bit					1

#define RCC_CFGR_MCOPRE_DIV1_value			0
#define RCC_CFGR_MCOPRE_DIV2_value			1
#define RCC_CFGR_MCOPRE_DIV4_value			2
#define RCC_CFGR_MCOPRE_DIV8_value			3
#define RCC_CFGR_MCOPRE_DIV16_value			4
#define RCC_CFGR_MCOPRE_mask				7

#define RCC_CFGR_MCOSEL_NOCLOCK_value		0
#define RCC_CFGR_MCOSEL_SYSCLK_value		1
#define RCC_CFGR_MCOSEL_HSI_value			2
#define RCC_CFGR_MCOSEL_MSI_value			3
#define RCC_CFGR_MCOSEL_HSE_value			4
#define RCC_CFGR_MCOSEL_PLL_value			5
#define RCC_CFGR_MCOSEL_LSI_value			6
#define RCC_CFGR_MCOSEL_LSE_value			7
#define RCC_CFGR_MCOSEL_mask				7

#define RCC_CFGR_PLLDIV2_value				1
#define RCC_CFGR_PLLDIV3_value				2
#define RCC_CFGR_PLLDIV4_value				3
#define RCC_CFGR_PLLDIV_mask				3

#define RCC_CFGR_PLLMUL3_value				0
#define RCC_CFGR_PLLMUL4_value				1
#define RCC_CFGR_PLLMUL6_value				2
#define RCC_CFGR_PLLMUL8_value				3
#define RCC_CFGR_PLLMUL12_value				4
#define RCC_CFGR_PLLMUL16_value				5
#define RCC_CFGR_PLLMUL24_value				6
#define RCC_CFGR_PLLMUL32_value				7
#define RCC_CFGR_PLLMUL48_value				8
#define RCC_CFGR_PLLMUL_mask				15

#define RCC_CFGR_PLLSRC_HSI_value			0
#define RCC_CFGR_PLLSRC_HSE_value			1
#define RCC_CFGR_PLLSRC_mask				1

#define RCC_CFGR_PPRE2_DIV1_value			0
#define RCC_CFGR_PPRE2_DIV2_value			4
#define RCC_CFGR_PPRE2_DIV4_value			5
#define RCC_CFGR_PPRE2_DIV8_value			6
#define RCC_CFGR_PPRE2_DIV16_value			7
#define RCC_CFGR_PPRE2_mask					7

#define RCC_CFGR_PPRE1_DIV1_value			0
#define RCC_CFGR_PPRE1_DIV2_value			4
#define RCC_CFGR_PPRE1_DIV4_value			5
#define RCC_CFGR_PPRE1_DIV8_value			6
#define RCC_CFGR_PPRE1_DIV16_value			7
#define RCC_CFGR_PPRE1_mask					7

#define RCC_CFGR_HPRE_DIV1_value			0
#define RCC_CFGR_HPRE_DIV2_value			8
#define RCC_CFGR_HPRE_DIV4_value			9
#define RCC_CFGR_HPRE_DIV8_value			10
#define RCC_CFGR_HPRE_DIV16_value			11
#define RCC_CFGR_HPRE_DIV64_value			12
#define RCC_CFGR_HPRE_DIV128_value			13
#define RCC_CFGR_HPRE_DIV256_value			14
#define RCC_CFGR_HPRE_DIV512_value			15
#define RCC_CFGR_HPRE_mask					15

#define RCC_CFGR_SWS_MSI_value				0
#define RCC_CFGR_SWS_HSI_value				1
#define RCC_CFGR_SWS_HSE_value				2
#define RCC_CFGR_SWS_PLL_value				3
#define RCC_CFGR_SWS_mask					3

#define RCC_CFGR_SW_MSI_value				0
#define RCC_CFGR_SW_HSI_value				1
#define RCC_CFGR_SW_HSE_value				2
#define RCC_CFGR_SW_PLL_value				3
#define RCC_CFGR_SW_mask					3

#define RCC_CFGR_MCOPRE_DIV1				(RCC_CFGR_MCOPRE_DIV1_value << RCC_CFGR_MCOPRE_bit)
#define RCC_CFGR_MCOPRE_DIV2				(RCC_CFGR_MCOPRE_DIV2_value << RCC_CFGR_MCOPRE_bit)
#define RCC_CFGR_MCOPRE_DIV4				(RCC_CFGR_MCOPRE_DIV4_value << RCC_CFGR_MCOPRE_bit)
#define RCC_CFGR_MCOPRE_DIV8				(RCC_CFGR_MCOPRE_DIV8_value << RCC_CFGR_MCOPRE_bit)
#define RCC_CFGR_MCOPRE_DIV16				(RCC_CFGR_MCOPRE_DIV16_value << RCC_CFGR_MCOPRE_bit)

#define RCC_CFGR_MCOSEL_NOCLOCK				(RCC_CFGR_MCOSEL_NOCLOCK_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_SYSCLK				(RCC_CFGR_MCOSEL_SYSCLK_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_HSI					(RCC_CFGR_MCOSEL_HSI_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_MSI					(RCC_CFGR_MCOSEL_MSI_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_HSE					(RCC_CFGR_MCOSEL_HSE_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_PLL					(RCC_CFGR_MCOSEL_PLL_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_LSI					(RCC_CFGR_MCOSEL_LSI_value << RCC_CFGR_MCOSEL_bit)
#define RCC_CFGR_MCOSEL_LSE					(RCC_CFGR_MCOSEL_LSE_value << RCC_CFGR_MCOSEL_bit)

#define RCC_CFGR_MCOPRE_0_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOPRE_0_bit)
#define RCC_CFGR_MCOPRE_1_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOPRE_1_bit)
#define RCC_CFGR_MCOPRE_2_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOPRE_2_bit)

#define RCC_CFGR_MCOSEL_0_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOSEL_0_bit)
#define RCC_CFGR_MCOSEL_1_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOSEL_1_bit)
#define RCC_CFGR_MCOSEL_2_bb				BITBAND(&RCC->CFGR, RCC_CFGR_MCOSEL_2_bit)

#define RCC_CFGR_PLLDIV_0_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLDIV_0_bit)
#define RCC_CFGR_PLLDIV_1_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLDIV_1_bit)

#define RCC_CFGR_PLLMUL_0_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLMUL_0_bit)
#define RCC_CFGR_PLLMUL_1_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLMUL_1_bit)
#define RCC_CFGR_PLLMUL_2_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLMUL_2_bit)
#define RCC_CFGR_PLLMUL_3_bb				BITBAND(&RCC->CFGR, RCC_CFGR_PLLMUL_3_bit)

#define RCC_CFGR_PLLSRC_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PLLSRC_bit)

#define RCC_CFGR_PPRE2_0_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE2_0_bit)
#define RCC_CFGR_PPRE2_1_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE2_1_bit)
#define RCC_CFGR_PPRE2_2_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE2_2_bit)

#define RCC_CFGR_PPRE1_0_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE1_0_bit)
#define RCC_CFGR_PPRE1_1_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE1_1_bit)
#define RCC_CFGR_PPRE1_2_bb					BITBAND(&RCC->CFGR, RCC_CFGR_PPRE1_2_bit)

#define RCC_CFGR_HPRE_0_bb					BITBAND(&RCC->CFGR, RCC_CFGR_HPRE_0_bit)
#define RCC_CFGR_HPRE_1_bb					BITBAND(&RCC->CFGR, RCC_CFGR_HPRE_1_bit)
#define RCC_CFGR_HPRE_2_bb					BITBAND(&RCC->CFGR, RCC_CFGR_HPRE_2_bit)
#define RCC_CFGR_HPRE_3_bb					BITBAND(&RCC->CFGR, RCC_CFGR_HPRE_3_bit)

#define RCC_CFGR_SWS_0_bb					BITBAND(&RCC->CFGR, RCC_CFGR_SWS_0_bit)
#define RCC_CFGR_SWS_1_bb					BITBAND(&RCC->CFGR, RCC_CFGR_SWS_1_bit)

#define RCC_CFGR_SW_0_bb					BITBAND(&RCC->CFGR, RCC_CFGR_SW_0_bit)
#define RCC_CFGR_SW_1_bb					BITBAND(&RCC->CFGR, RCC_CFGR_SW_1_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_CIR - Clock interrupt register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_CIR_CSSC_bit					23
#define RCC_CIR_LSECSSC_bit					22
#define RCC_CIR_MSIRDYC_bit					21
#define RCC_CIR_PLLRDYC_bit					20
#define RCC_CIR_HSERDYC_bit					19
#define RCC_CIR_HSIRDYC_bit					18
#define RCC_CIR_LSERDYC_bit					17
#define RCC_CIR_LSIRDYC_bit					16
#define RCC_CIR_LSECSSIE_bit				14
#define RCC_CIR_MSIRDYIE_bit				13
#define RCC_CIR_PLLRDYIE_bit				12
#define RCC_CIR_HSERDYIE_bit				11
#define RCC_CIR_HSIRDYIE_bit				10
#define RCC_CIR_LSERDYIE_bit				9
#define RCC_CIR_LSIRDYIE_bit				8
#define RCC_CIR_CSSF_bit					7
#define RCC_CIR_LSECSSF_bit					6
#define RCC_CIR_MSIRDYF_bit					5
#define RCC_CIR_PLLRDYF_bit					4
#define RCC_CIR_HSERDYF_bit					3
#define RCC_CIR_HSIRDYF_bit					2
#define RCC_CIR_LSERDYF_bit					1
#define RCC_CIR_LSIRDYF_bit					0

#define RCC_CIR_CSSC_bb						BITBAND(&RCC->CIR, RCC_CIR_CSSC_bit)
#define RCC_CIR_LSECSSC_bb					BITBAND(&RCC->CIR, RCC_CIR_LSECSSC_bit)
#define RCC_CIR_MSIRDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_MSIRDYC_bit)
#define RCC_CIR_PLLRDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_PLLRDYC_bit)
#define RCC_CIR_HSERDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_HSERDYC_bit)
#define RCC_CIR_HSIRDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_HSIRDYC_bit)
#define RCC_CIR_LSERDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_LSERDYC_bit)
#define RCC_CIR_LSIRDYC_bb					BITBAND(&RCC->CIR, RCC_CIR_LSIRDYC_bit)
#define RCC_CIR_LSECSSIE_bb					BITBAND(&RCC->CIR, RCC_CIR_LSECSSIE_bit)
#define RCC_CIR_MSIRDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_MSIRDYIE_bit)
#define RCC_CIR_PLLRDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_PLLRDYIE_bit)
#define RCC_CIR_HSERDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_HSERDYIE_bit)
#define RCC_CIR_HSIRDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_HSIRDYIE_bit)
#define RCC_CIR_LSERDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_LSERDYIE_bit)
#define RCC_CIR_LSIRDYIE_bb					BITBAND(&RCC->CIR, RCC_CIR_LSIRDYIE_bit)
#define RCC_CIR_CSSF_bb						BITBAND(&RCC->CIR, RCC_CIR_CSSF_bit)
#define RCC_CIR_LSECSSF_bb					BITBAND(&RCC->CIR, RCC_CIR_LSECSSF_bit)
#define RCC_CIR_MSIRDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_MSIRDYF_bit)
#define RCC_CIR_PLLRDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_PLLRDYF_bit)
#define RCC_CIR_HSERDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_HSERDYF_bit)
#define RCC_CIR_HSIRDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_HSIRDYF_bit)
#define RCC_CIR_LSERDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_LSERDYF_bit)
#define RCC_CIR_LSIRDYF_bb					BITBAND(&RCC->CIR, RCC_CIR_LSIRDYF_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_AHBRSTR - AHB peripheral reset register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_AHBRSTR_FSMCRST_bit				30
#define RCC_AHBRSTR_AESRST_bit				27
#define RCC_AHBRSTR_DMA2RST_bit				25
#define RCC_AHBRSTR_DMA1RST_bit				24
#define RCC_AHBRSTR_FLITFRST_bit			15
#define RCC_AHBRSTR_CRCRST_bit				12
#define RCC_AHBRSTR_GPIOGRST_bit			7
#define RCC_AHBRSTR_GPIOFRST_bit			6
#define RCC_AHBRSTR_GPIOHRST_bit			5
#define RCC_AHBRSTR_GPIOERST_bit			4
#define RCC_AHBRSTR_GPIODRST_bit			3
#define RCC_AHBRSTR_GPIOCRST_bit			2
#define RCC_AHBRSTR_GPIOBRST_bit			1
#define RCC_AHBRSTR_GPIOARST_bit			0

#define RCC_AHBRSTR_FSMCRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_FSMCRST_bit)
#define RCC_AHBRSTR_AESRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_AESRST_bit)
#define RCC_AHBRSTR_DMA2RST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_DMA2RST_bit)
#define RCC_AHBRSTR_DMA1RST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_DMA1RST_bit)
#define RCC_AHBRSTR_FLITFRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_FLITFRST_bit)
#define RCC_AHBRSTR_CRCRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_CRCRST_bit)
#define RCC_AHBRSTR_GPIOGRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOGRST_bit)
#define RCC_AHBRSTR_GPIOFRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOFRST_bit)
#define RCC_AHBRSTR_GPIOHRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOHRST_bit)
#define RCC_AHBRSTR_GPIOERST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOERST_bit)
#define RCC_AHBRSTR_GPIODRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIODRST_bit)
#define RCC_AHBRSTR_GPIOCRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOCRST_bit)
#define RCC_AHBRSTR_GPIOBRST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOBRST_bit)
#define RCC_AHBRSTR_GPIOARST_bb				BITBAND(&RCC->AHBRSTR, RCC_AHBRSTR_GPIOARST_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB2RSTR - APB2 peripheral reset register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB2RSTR_USART1RST_bit			14
#define RCC_APB2RSTR_SPI1RST_bit			12
#define RCC_APB2RSTR_SDIORST_bit			11
#define RCC_APB2RSTR_ADC1RST_bit			9
#define RCC_APB2RSTR_TIM11RST_bit			4
#define RCC_APB2RSTR_TIM10RST_bit			3
#define RCC_APB2RSTR_TIM9RST_bit			2
#define RCC_APB2RSTR_SYSCFGRST_bit			0

#define RCC_APB2RSTR_USART1RST_bb			BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_USART1RST_bit)
#define RCC_APB2RSTR_SPI1RST_bb				BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_SPI1RST_bit)
#define RCC_APB2RSTR_SDIORST_bb				BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_SDIORST_bit)
#define RCC_APB2RSTR_ADC1RST_bb				BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_ADC1RST_bit)
#define RCC_APB2RSTR_TIM11RST_bb			BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_TIM11RST_bit)
#define RCC_APB2RSTR_TIM10RST_bb			BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_TIM10RST_bit)
#define RCC_APB2RSTR_TIM9RST_bb				BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_TIM9RST_bit)
#define RCC_APB2RSTR_SYSCFGRST_bb			BITBAND(&RCC->APB2RSTR, RCC_APB2RSTR_SYSCFGRST_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB1RSTR - APB1 peripheral reset register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB1RSTR_COMPRST_bit			31
#define RCC_APB1RSTR_DACRST_bit				29
#define RCC_APB1RSTR_PWRRST_bit				28
#define RCC_APB1RSTR_USBRST_bit				23
#define RCC_APB1RSTR_I2C2RST_bit			22
#define RCC_APB1RSTR_I2C1RST_bit			21
#define RCC_APB1RSTR_USAR5RST_bit			20
#define RCC_APB1RSTR_UART4RST_bit			19
#define RCC_APB1RSTR_USART3RST_bit			18
#define RCC_APB1RSTR_USART2RST_bit			17
#define RCC_APB1RSTR_SPI3RST_bit			15
#define RCC_APB1RSTR_SPI2RST_bit			14
#define RCC_APB1RSTR_WWDGRST_bit			11
#define RCC_APB1RSTR_LCDRST_bit				9
#define RCC_APB1RSTR_TIM7RST_bit			5
#define RCC_APB1RSTR_TIM6RST_bit			4
#define RCC_APB1RSTR_TIM5RST_bit			3
#define RCC_APB1RSTR_TIM4RST_bit			2
#define RCC_APB1RSTR_TIM3RST_bit			1
#define RCC_APB1RSTR_TIM2RST_bit			0

#define RCC_APB1RSTR_COMPRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_COMPRST_bit)
#define RCC_APB1RSTR_DACRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_DACRST_bit)
#define RCC_APB1RSTR_PWRRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_PWRRST_bit)
#define RCC_APB1RSTR_USBRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_USBRST_bit)
#define RCC_APB1RSTR_I2C2RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_I2C2RST_bit)
#define RCC_APB1RSTR_I2C1RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_I2C1RST_bit)
#define RCC_APB1RSTR_USAR5RST_bb			BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_USAR5RST_bit)
#define RCC_APB1RSTR_UART4RST_bb			BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_UART4RST_bit)
#define RCC_APB1RSTR_USART3RST_bb			BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_USART3RST_bit)
#define RCC_APB1RSTR_USART2RST_bb			BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_USART2RST_bit)
#define RCC_APB1RSTR_SPI3RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_SPI3RST_bit)
#define RCC_APB1RSTR_SPI2RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_SPI2RST_bit)
#define RCC_APB1RSTR_WWDGRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_WWDGRST_bit)
#define RCC_APB1RSTR_LCDRST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_LCDRST_bit)
#define RCC_APB1RSTR_TIM7RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST_bit)
#define RCC_APB1RSTR_TIM6RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM6RST_bit)
#define RCC_APB1RSTR_TIM5RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM5RST_bit)
#define RCC_APB1RSTR_TIM4RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM4RST_bit)
#define RCC_APB1RSTR_TIM3RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM3RST_bit)
#define RCC_APB1RSTR_TIM2RST_bb				BITBAND(&RCC->APB1RSTR, RCC_APB1RSTR_TIM2RST_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_AHBENR - AHB peripheral clock enable register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_AHBENR_FSMCEN_bit				30
#define RCC_AHBENR_AESEN_bit				27
#define RCC_AHBENR_DMA2EN_bit				25
#define RCC_AHBENR_DMA1EN_bit				24
#define RCC_AHBENR_FLITFEN_bit				15
#define RCC_AHBENR_CRCEN_bit				12
#define RCC_AHBENR_GPIOGEN_bit				7
#define RCC_AHBENR_GPIOFEN_bit				6
#define RCC_AHBENR_GPIOHEN_bit				5
#define RCC_AHBENR_GPIOEEN_bit				4
#define RCC_AHBENR_GPIODEN_bit				3
#define RCC_AHBENR_GPIOCEN_bit				2
#define RCC_AHBENR_GPIOBEN_bit				1
#define RCC_AHBENR_GPIOAEN_bit				0

#define RCC_AHBENR_FSMCEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_FSMCEN_bit)
#define RCC_AHBENR_AESEN_bb					BITBAND(&RCC->AHBENR, RCC_AHBENR_AESEN_bit)
#define RCC_AHBENR_DMA2EN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_DMA2EN_bit)
#define RCC_AHBENR_DMA1EN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_DMA1EN_bit)
#define RCC_AHBENR_FLITFEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_FLITFEN_bit)
#define RCC_AHBENR_CRCEN_bb					BITBAND(&RCC->AHBENR, RCC_AHBENR_CRCEN_bit)
#define RCC_AHBENR_GPIOGEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOGEN_bit)
#define RCC_AHBENR_GPIOFEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOFEN_bit)
#define RCC_AHBENR_GPIOHEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOHEN_bit)
#define RCC_AHBENR_GPIOEEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOEEN_bit)
#define RCC_AHBENR_GPIODEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIODEN_bit)
#define RCC_AHBENR_GPIOCEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOCEN_bit)
#define RCC_AHBENR_GPIOBEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOBEN_bit)
#define RCC_AHBENR_GPIOAEN_bb				BITBAND(&RCC->AHBENR, RCC_AHBENR_GPIOAEN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB2ENR - APB2 peripheral clock enable register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB2ENR_USART1EN_bit			14
#define RCC_APB2ENR_SPI1EN_bit				12
#define RCC_APB2ENR_SDIOEN_bit				11
#define RCC_APB2ENR_ADC1EN_bit				9
#define RCC_APB2ENR_TIM11EN_bit				4
#define RCC_APB2ENR_TIM10EN_bit				3
#define RCC_APB2ENR_TIM9EN_bit				2
#define RCC_APB2ENR_SYSCFGEN_bit			0

#define RCC_APB2ENR_USART1EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_USART1EN_bit)
#define RCC_APB2ENR_SPI1EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_SPI1EN_bit)
#define RCC_APB2ENR_SDIOEN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_SDIOEN_bit)
#define RCC_APB2ENR_ADC1EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_ADC1EN_bit)
#define RCC_APB2ENR_TIM11EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_TIM11EN_bit)
#define RCC_APB2ENR_TIM10EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_TIM10EN_bit)
#define RCC_APB2ENR_TIM9EN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_TIM9EN_bit)
#define RCC_APB2ENR_SYSCFGEN_bb				BITBAND(&RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB1ENR - APB1 peripheral clock enable register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB1ENR_COMPEN_bit				31
#define RCC_APB1ENR_DACEN_bit				29
#define RCC_APB1ENR_PWREN_bit				28
#define RCC_APB1ENR_USBEN_bit				23
#define RCC_APB1ENR_I2C2EN_bit				22
#define RCC_APB1ENR_I2C1EN_bit				21
#define RCC_APB1ENR_USAR5EN_bit				20
#define RCC_APB1ENR_UART4EN_bit				19
#define RCC_APB1ENR_USART3EN_bit			18
#define RCC_APB1ENR_USART2EN_bit			17
#define RCC_APB1ENR_SPI3EN_bit				15
#define RCC_APB1ENR_SPI2EN_bit				14
#define RCC_APB1ENR_WWDGEN_bit				11
#define RCC_APB1ENR_LCDEN_bit				9
#define RCC_APB1ENR_TIM7EN_bit				5
#define RCC_APB1ENR_TIM6EN_bit				4
#define RCC_APB1ENR_TIM5EN_bit				3
#define RCC_APB1ENR_TIM4EN_bit				2
#define RCC_APB1ENR_TIM3EN_bit				1
#define RCC_APB1ENR_TIM2EN_bit				0

#define RCC_APB1ENR_COMPEN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_COMPEN_bit)
#define RCC_APB1ENR_DACEN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_DACEN_bit)
#define RCC_APB1ENR_PWREN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_PWREN_bit)
#define RCC_APB1ENR_USBEN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_USBEN_bit)
#define RCC_APB1ENR_I2C2EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_I2C2EN_bit)
#define RCC_APB1ENR_I2C1EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_I2C1EN_bit)
#define RCC_APB1ENR_USAR5EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_USAR5EN_bit)
#define RCC_APB1ENR_UART4EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_UART4EN_bit)
#define RCC_APB1ENR_USART3EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_USART3EN_bit)
#define RCC_APB1ENR_USART2EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_USART2EN_bit)
#define RCC_APB1ENR_SPI3EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_SPI3EN_bit)
#define RCC_APB1ENR_SPI2EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_SPI2EN_bit)
#define RCC_APB1ENR_WWDGEN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_WWDGEN_bit)
#define RCC_APB1ENR_LCDEN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_LCDEN_bit)
#define RCC_APB1ENR_TIM7EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM7EN_bit)
#define RCC_APB1ENR_TIM6EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM6EN_bit)
#define RCC_APB1ENR_TIM5EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM5EN_bit)
#define RCC_APB1ENR_TIM4EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM4EN_bit)
#define RCC_APB1ENR_TIM3EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM3EN_bit)
#define RCC_APB1ENR_TIM2EN_bb				BITBAND(&RCC->APB1ENR, RCC_APB1ENR_TIM2EN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_AHBLPENR - AHB peripheral clock enable in low power mode register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_AHBLPENR_FSMCLPEN_bit			30
#define RCC_AHBLPENR_AESLPEN_bit			27
#define RCC_AHBLPENR_DMA2LPEN_bit			25
#define RCC_AHBLPENR_DMA1LPEN_bit			24
#define RCC_AHBLPENR_SRAMLPEN_bit			16
#define RCC_AHBLPENR_FLITFLPEN_bit			15
#define RCC_AHBLPENR_CRCLPEN_bit			12
#define RCC_AHBLPENR_GPIOGLPEN_bit			7
#define RCC_AHBLPENR_GPIOFLPEN_bit			6
#define RCC_AHBLPENR_GPIOHLPEN_bit			5
#define RCC_AHBLPENR_GPIOELPEN_bit			4
#define RCC_AHBLPENR_GPIODLPEN_bit			3
#define RCC_AHBLPENR_GPIOCLPEN_bit			2
#define RCC_AHBLPENR_GPIOBLPEN_bit			1
#define RCC_AHBLPENR_GPIOALPEN_bit			0

#define RCC_AHBLPENR_FSMCLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_FSMCLPEN_bit)
#define RCC_AHBLPENR_AESLPEN_bb				BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_AESLPEN_bit)
#define RCC_AHBLPENR_DMA2LPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_DMA2LPEN_bit)
#define RCC_AHBLPENR_DMA1LPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_DMA1LPEN_bit)
#define RCC_AHBLPENR_SRAMLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_SRAMLPEN_bit)
#define RCC_AHBLPENR_FLITFLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_FLITFLPEN_bit)
#define RCC_AHBLPENR_CRCLPEN_bb				BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_CRCLPEN_bit)
#define RCC_AHBLPENR_GPIOGLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOGLPEN_bit)
#define RCC_AHBLPENR_GPIOFLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOFLPEN_bit)
#define RCC_AHBLPENR_GPIOHLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOHLPEN_bit)
#define RCC_AHBLPENR_GPIOELPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOELPEN_bit)
#define RCC_AHBLPENR_GPIODLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIODLPEN_bit)
#define RCC_AHBLPENR_GPIOCLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOCLPEN_bit)
#define RCC_AHBLPENR_GPIOBLPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOBLPEN_bit)
#define RCC_AHBLPENR_GPIOALPEN_bb			BITBAND(&RCC->AHBLPENR, RCC_AHBLPENR_GPIOALPEN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB2LPENR - APB2 peripheral clock enable in low power mode register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB2LPENR_USART1LPEN_bit		14
#define RCC_APB2LPENR_SPI1LPEN_bit			12
#define RCC_APB2LPENR_SDIOLPEN_bit			11
#define RCC_APB2LPENR_ADC1LPEN_bit			9
#define RCC_APB2LPENR_TIM11LPEN_bit			4
#define RCC_APB2LPENR_TIM10LPEN_bit			3
#define RCC_APB2LPENR_TIM9LPEN_bit			2
#define RCC_APB2LPENR_SYSCFGLPEN_bit		0

#define RCC_APB2LPENR_USART1LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_USART1LPEN_bit)
#define RCC_APB2LPENR_SPI1LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_SPI1LPEN_bit)
#define RCC_APB2LPENR_SDIOLPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_SDIOLPEN_bit)
#define RCC_APB2LPENR_ADC1LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_ADC1LPEN_bit)
#define RCC_APB2LPENR_TIM11LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_TIM11LPEN_bit)
#define RCC_APB2LPENR_TIM10LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_TIM10LPEN_bit)
#define RCC_APB2LPENR_TIM9LPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_TIM9LPEN_bit)
#define RCC_APB2LPENR_SYSCFGLPEN_bb			BITBAND(&RCC->APB2LPENR, RCC_APB2LPENR_SYSCFGLPEN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_APB1LPENR - APB1 peripheral clock enable in low power mode register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_APB1LPENR_COMPLPEN_bit			31
#define RCC_APB1LPENR_DACLPEN_bit			29
#define RCC_APB1LPENR_PWRLPEN_bit			28
#define RCC_APB1LPENR_USBLPEN_bit			23
#define RCC_APB1LPENR_I2C2LPEN_bit			22
#define RCC_APB1LPENR_I2C1LPEN_bit			21
#define RCC_APB1LPENR_USAR5LPEN_bit			20
#define RCC_APB1LPENR_UART4LPEN_bit			19
#define RCC_APB1LPENR_USART3LPEN_bit		18
#define RCC_APB1LPENR_USART2LPEN_bit		17
#define RCC_APB1LPENR_SPI3LPEN_bit			15
#define RCC_APB1LPENR_SPI2LPEN_bit			14
#define RCC_APB1LPENR_WWDGLPEN_bit			11
#define RCC_APB1LPENR_LCDLPEN_bit			9
#define RCC_APB1LPENR_TIM7LPEN_bit			5
#define RCC_APB1LPENR_TIM6LPEN_bit			4
#define RCC_APB1LPENR_TIM5LPEN_bit			3
#define RCC_APB1LPENR_TIM4LPEN_bit			2
#define RCC_APB1LPENR_TIM3LPEN_bit			1
#define RCC_APB1LPENR_TIM2LPEN_bit			0

#define RCC_APB1LPENR_COMPLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_COMPLPEN_bit)
#define RCC_APB1LPENR_DACLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_DACLPEN_bit)
#define RCC_APB1LPENR_PWRLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_PWRLPEN_bit)
#define RCC_APB1LPENR_USBLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_USBLPEN_bit)
#define RCC_APB1LPENR_I2C2LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_I2C2LPEN_bit)
#define RCC_APB1LPENR_I2C1LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_I2C1LPEN_bit)
#define RCC_APB1LPENR_USAR5LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_USAR5LPEN_bit)
#define RCC_APB1LPENR_UART4LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_UART4LPEN_bit)
#define RCC_APB1LPENR_USART3LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_USART3LPEN_bit)
#define RCC_APB1LPENR_USART2LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_USART2LPEN_bit)
#define RCC_APB1LPENR_SPI3LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_SPI3LPEN_bit)
#define RCC_APB1LPENR_SPI2LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_SPI2LPEN_bit)
#define RCC_APB1LPENR_WWDGLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_WWDGLPEN_bit)
#define RCC_APB1LPENR_LCDLPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_LCDLPEN_bit)
#define RCC_APB1LPENR_TIM7LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM7LPEN_bit)
#define RCC_APB1LPENR_TIM6LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM6LPEN_bit)
#define RCC_APB1LPENR_TIM5LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM5LPEN_bit)
#define RCC_APB1LPENR_TIM4LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM4LPEN_bit)
#define RCC_APB1LPENR_TIM3LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM3LPEN_bit)
#define RCC_APB1LPENR_TIM2LPEN_bb			BITBAND(&RCC->APB1LPENR, RCC_APB1LPENR_TIM2LPEN_bit)

/*---------------------------------------------------------------------------------------------------------------------+
| RCC_CSR - Control/status register
+---------------------------------------------------------------------------------------------------------------------*/

#define RCC_CSR_LPWRRSTF_bit				31
#define RCC_CSR_WWDGRSTF_bit				30
#define RCC_CSR_IWDGRSTF_bit				29
#define RCC_CSR_SFTRSTF_bit					28
#define RCC_CSR_PORRSTF_bit					27
#define RCC_CSR_PINRSTF_bit					26
#define RCC_CSR_OBLRSTF_bit					25
#define RCC_CSR_RMVF_bit					24
#define RCC_CSR_RTCRST_bit					23
#define RCC_CSR_RTCEN_bit					22
#define RCC_CSR_RTCSEL_bit					16
#define RCC_CSR_RTCSEL_0_bit				16
#define RCC_CSR_RTCSEL_1_bit				17
#define RCC_CSR_LSECSSD_bit					12
#define RCC_CSR_LSECSSON_bit				11
#define RCC_CSR_LSEBYP_bit					10
#define RCC_CSR_LSERDY_bit					9
#define RCC_CSR_LSEON_bit					8
#define RCC_CSR_LSIRDY_bit					1
#define RCC_CSR_LSION_bit					0

#define RCC_CSR_RTCSEL_NOCLOCK_value		0
#define RCC_CSR_RTCSEL_LSE_value			1
#define RCC_CSR_RTCSEL_LSI_value			2
#define RCC_CSR_RTCSEL_HSE_value			3
#define RCC_CSR_RTCSEL_mask					3

#define RCC_CSR_LPWRRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_LPWRRSTF_bit)
#define RCC_CSR_WWDGRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_WWDGRSTF_bit)
#define RCC_CSR_IWDGRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_IWDGRSTF_bit)
#define RCC_CSR_SFTRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_SFTRSTF_bit)
#define RCC_CSR_PORRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_PORRSTF_bit)
#define RCC_CSR_PINRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_PINRSTF_bit)
#define RCC_CSR_OBLRSTF_bb					BITBAND(&RCC->CSR, RCC_CSR_OBLRSTF_bit)
#define RCC_CSR_RMVF_bb						BITBAND(&RCC->CSR, RCC_CSR_RMVF_bit)
#define RCC_CSR_RTCRST_bb					BITBAND(&RCC->CSR, RCC_CSR_RTCRST_bit)
#define RCC_CSR_RTCEN_bb					BITBAND(&RCC->CSR, RCC_CSR_RTCEN_bit)
#define RCC_CSR_RTCSEL_0_bb					BITBAND(&RCC->CSR, RCC_CSR_RTCSEL_0_bit)
#define RCC_CSR_RTCSEL_1_bb					BITBAND(&RCC->CSR, RCC_CSR_RTCSEL_1_bit)
#define RCC_CSR_LSECSSD_bb					BITBAND(&RCC->CSR, RCC_CSR_LSECSSD_bit)
#define RCC_CSR_LSECSSON_bb					BITBAND(&RCC->CSR, RCC_CSR_LSECSSON_bit)
#define RCC_CSR_LSEBYP_bb					BITBAND(&RCC->CSR, RCC_CSR_LSEBYP_bit)
#define RCC_CSR_LSERDY_bb					BITBAND(&RCC->CSR, RCC_CSR_LSERDY_bit)
#define RCC_CSR_LSEON_bb					BITBAND(&RCC->CSR, RCC_CSR_LSEON_bit)
#define RCC_CSR_LSIRDY_bb					BITBAND(&RCC->CSR, RCC_CSR_LSIRDY_bit)
#define RCC_CSR_LSION_bb					BITBAND(&RCC->CSR, RCC_CSR_LSION_bit)

#endif /* HDR_RCC_H_ */
