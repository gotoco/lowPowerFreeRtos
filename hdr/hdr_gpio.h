/**
 * \file hdr_gpio.h
 * \brief Header with values of bitfields in GPIO control registers
 *
 * Header with values of bitfields in GPIO control registers
 *
 * chip: STM32L1xx
 *
 * \author grochu
 * \date 2012-08-24
 */

#ifndef HDR_GPIO_H_
#define HDR_GPIO_H_

/*---------------------------------------------------------------------------------------------------------------------+
| GPIOx_MODER - GPIO port mode register
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_MODER_mask						3
#define GPIO_MODER_IN_value					0
#define GPIO_MODER_OUT_value				1
#define GPIO_MODER_ALT_value				2
#define GPIO_MODER_ANALOG_value				3

/*---------------------------------------------------------------------------------------------------------------------+
| GPIOx_OTYPER - GPIO port output type register
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_OTYPER_mask					1
#define GPIO_OTYPER_PP_value				0
#define GPIO_OTYPER_OD_value				1

/*---------------------------------------------------------------------------------------------------------------------+
| GPIOx_OSPEEDR - GPIO port output speed register
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_OSPEEDR_mask					3
#define GPIO_OSPEEDR_400kHz_value			0
#define GPIO_OSPEEDR_2MHz_value				1
#define GPIO_OSPEEDR_10MHz_value			2
#define GPIO_OSPEEDR_40MHz_value			3

/*---------------------------------------------------------------------------------------------------------------------+
| GPIOx_PUPDR - GPIO port pull-up/pull-down register
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_PUPDR_mask						3
#define GPIO_PUPDR_FLOATING_value			0
#define GPIO_PUPDR_PULL_UP_value			1
#define GPIO_PUPDR_PULL_DOWN_value			2

/*---------------------------------------------------------------------------------------------------------------------+
| GPIOx_AFRx - GPIO alternate function registers
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_AFRx_mask						15
#define GPIO_AFRx_AF0_value					0
#define GPIO_AFRx_AF1_value					1
#define GPIO_AFRx_AF2_value					2
#define GPIO_AFRx_AF3_value					3
#define GPIO_AFRx_AF4_value					4
#define GPIO_AFRx_AF5_value					5
#define GPIO_AFRx_AF6_value					6
#define GPIO_AFRx_AF7_value					7
#define GPIO_AFRx_AF8_value					8
#define GPIO_AFRx_AF9_value					9
#define GPIO_AFRx_AF10_value				10
#define GPIO_AFRx_AF11_value				11
#define GPIO_AFRx_AF12_value				12
#define GPIO_AFRx_AF13_value				13
#define GPIO_AFRx_AF14_value				14
#define GPIO_AFRx_AF15_value				15

#endif /* HDR_GPIO_H_ */
