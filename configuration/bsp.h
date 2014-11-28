/**
 * \file bsp.h
 * \brief classic Board Support Package
 *
 * Basic configuration of the project
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author Mazeryt Freager
 * \date 2014-07-29
 */

#ifndef BSP_H_
#define BSP_H_

/*---------------------------------------------------------------------------------------------------------------------+
| LEDs INTERFACE
+---------------------------------------------------------------------------------------------------------------------*/

#define LED_GPIO							GPIOB	///< GPIO port to which the LED is connected
#define LED_pin								GPIO_PIN_6	///< pin number of the LED
#define LED_pin_1							GPIO_PIN_7	///< pin number of the LED1
#define LED_pin_2							GPIO_PIN_13	///< pin number of the LED2
#define LED_pin_3							GPIO_PIN_14	///< pin number of the LED3

/// bit-band "variable" to directly handle the pin
#define LED_bb								BITBAND(&LED_GPIO->ODR, LED_pin)
#define LED1_bb								BITBAND(&LED_GPIO->ODR, LED_pin_1)
#define LED2_bb								BITBAND(&LED_GPIO->ODR, LED_pin_2)
#define LED3_bb								BITBAND(&LED_GPIO->ODR, LED_pin_3)


/*---------------------------------------------------------------------------------------------------------------------+
| LCD INTERFACE
+---------------------------------------------------------------------------------------------------------------------*/

#define LCD_PORT							GPIOB
#define LCD_PIN								GPIO_PIN_12
#define LCD_PIN_bb							BITBAND(&LCD_PORT->ODR, LCD_PIN)

#define LCD_COM0_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_8)
#define LCD_COM1_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_9)
#define LCD_COM2_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_10)
#define LCD_COM3_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_9)

#define LCD_PA1_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_1)
#define LCD_PA2_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_2)
#define LCD_PA3_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_3)
#define LCD_PA15_PIN_bb						BITBAND(&GPIOA->ODR, GPIO_PIN_15)

#define LCD_PB3_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_3)
#define LCD_PB4_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_4)
#define LCD_PB5_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_5)
#define LCD_PB10_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_10)
#define LCD_PB11_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_11)
#define LCD_PB12_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_12)
#define LCD_PB13_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_13)
#define LCD_PB14_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_14)
#define LCD_PB15_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_15)
#define LCD_PB8_PIN_bb						BITBAND(&GPIOB->ODR, GPIO_PIN_8)

#define LCD_PC0_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_0)
#define LCD_PC1_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_1)
#define LCD_PC2_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_2)
#define LCD_PC3_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_3)
#define LCD_PC6_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_6)
#define LCD_PC7_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_7)
#define LCD_PC8_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_8)
#define LCD_PC9_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_9)
#define LCD_PC10_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_10)
#define LCD_PC11_PIN_bb						BITBAND(&GPIOC->ODR, GPIO_PIN_11)


/*---------------------------------------------------------------------------------------------------------------------+
| SD card
+---------------------------------------------------------------------------------------------------------------------*/

#define SD_CS_GPIO							GPIOC
#define SD_CS_PIN							GPIO_PIN_12
#define SD_CS_bb							BITBAND(&SD_CS_GPIO->ODR, SD_CS_PIN)

#define SD_CS_ASSERTED						0
#define SD_CS_DEASSERTED					1

/*---------------------------------------------------------------------------------------------------------------------+
| ACC
+---------------------------------------------------------------------------------------------------------------------*/


#endif //BSP_H_
