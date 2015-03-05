/**
 * \file config.h
 * \brief Basic configuration of the project
 *
 * Basic configuration of the project
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author grochu
 * \date 2012-09-06
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "hdr/hdr_bitband.h"
#include "hdr/hdr_rcc.h"

/*---------------------------------------------------------------------------------------------------------------------+
| Device
+---------------------------------------------------------------------------------------------------------------------*/

#define DEVICE_VCORE_mV						1800	///< VCORE voltage, configured via VOS in PWR_CR, in mV

#define FREQUENCY							32000000	///< desired target frequency of the core

/*---------------------------------------------------------------------------------------------------------------------+
| LEDs INTERFACE
+---------------------------------------------------------------------------------------------------------------------*/

#define LED_GPIO							GPIOB	///< GPIO port to which the LED is connected
#define LED_pin								GPIO_PIN_6	///< pin number of the LED
#define LED_pin_1							GPIO_PIN_7	///< pin number of the LED1
//#define LED_pin_2							GPIO_PIN_13	///< pin number of the LED2
//#define LED_pin_3							GPIO_PIN_14	///< pin number of the LED3

#define LED2_GPIO							GPIOA
#define LED_pin_2							GPIO_PIN_5


/// bit-band "variable" to directly handle the pin
#define LED_bb								BITBAND(&LED_GPIO->ODR, LED_pin)
#define LED1_bb								BITBAND(&LED_GPIO->ODR, LED_pin_1)
//#define LED2_bb								BITBAND(&LED_GPIO->ODR, LED_pin_2)
//#define LED3_bb								BITBAND(&LED_GPIO->ODR, LED_pin_3)

#define LED2_bb								BITBAND(&LED2_GPIO->ODR, LED_pin_2)

/*---------------------------------------------------------------------------------------------------------------------+
| BUTTON INTERFACE
+---------------------------------------------------------------------------------------------------------------------*/

#define BUTTON_GPIO							GPIOA
#define BUTTON_PIN							GPIO_PIN_0
#define BUTTON_CONFIGURATION				GPIO_IN_FLOATING

/*---------------------------------------------------------------------------------------------------------------------+
| USART
+---------------------------------------------------------------------------------------------------------------------*/

#define USARTx								USART1

#define USARTx_TX_GPIO						GPIOA
#define USARTx_TX_PIN						GPIO_PIN_9
#define USARTx_TX_CONFIGURATION				GPIO_AF7_PP_40MHz_PULL_UP
#define USARTx_RX_GPIO						GPIOA
#define USARTx_RX_PIN						GPIO_PIN_10
#define USARTx_RX_CONFIGURATION				GPIO_AF7_PP_40MHz_PULL_UP

#define RCC_APBxENR_USARTxEN_bb				RCC_APB2ENR_USART1EN_bb

#define USARTx_BAUDRATE						115200

#define RCC_AHBENR_DMAxEN_bb				RCC_AHBENR_DMA1EN_bb
#define USARTx_DMAx_TX_CH					DMA1_Channel4
#define USARTx_DMAx_TX_CH_IRQn				DMA1_Channel4_IRQn
#define USARTx_DMAx_TX_CH_IRQHandler		DMA1_Channel4_IRQHandler
#define USARTx_DMAx_TX_IFCR_CTCIFx_bb		DMA1_IFCR_CTCIF4_bb

#define USARTx_RX_QUEUE_LENGTH				16
#define USARTx_RX_QUEUE_BUFFER_LENGTH		16
#define USARTx_TX_QUEUE_LENGTH				16

#define USARTx_IRQn							USART1_IRQn
#define USARTx_IRQHandler					USART1_IRQHandler

/*---------------------------------------------------------------------------------------------------------------------+
| SERIAL-UART
+---------------------------------------------------------------------------------------------------------------------*/

#define SERIALx								USART2

#define SERIALx_TX_GPIO						GPIOA
#define SERIALx_TX_PIN						GPIO_PIN_2
#define SERIALx_TX_CONFIGURATION			GPIO_AF7_PP_40MHz_PULL_UP
#define SERIALx_RX_GPIO						GPIOA
#define SERIALx_RX_PIN						GPIO_PIN_3
#define SERIALx_RX_CONFIGURATION			GPIO_AF7_PP_40MHz_PULL_UP

#define SERIALx_RX_QUEUE_LENGTH				16
#define SERIALx_RX_QUEUE_BUFFER_LENGTH		16
#define SERIALx_TX_QUEUE_LENGTH				16

#define RCC_APBxENR_SERIALxEN_bb			RCC_APB1ENR_USART2EN_bb

#define SERIALx_BAUDRATE					115200

/*---------------------------------------------------------------------------------------------------------------------+
| SPI
+---------------------------------------------------------------------------------------------------------------------*/

#define SPIx								SPI1

#define RCC_APBxENR_SPIxEN_bb				RCC_APB2ENR_SPI1EN_bb

#define SPIx_MISO_GPIO						GPIOA
#define SPIx_MISO_PIN						GPIO_PIN_11
#define SPIx_MISO_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP
#define SPIx_MOSI_GPIO						GPIOA
#define SPIx_MOSI_PIN						GPIO_PIN_12
#define SPIx_MOSI_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP
#define SPIx_SCK_GPIO						GPIOA
#define SPIx_SCK_PIN						GPIO_PIN_5
#define SPIx_SCK_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP
#define SPIx_CS_GPIO						GPIOA
#define SPIx_CS_PIN							GPIO_PIN_4
#define SPIx_CS_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_DOWN

#define SPIx_CS_bb							BITBAND(&SPIx_CS_GPIO->ODR, SPIx_CS_PIN)


//
//	DMA
//
#define RCC_AHBENR_SPIx_DMAxEN_bb			RCC_AHBENR_DMA1EN_bb

#define SPIx_DMAx_TX_CH						DMA1_Channel3
#define SPIx_DMAx_TX_CH_IRQn				DMA1_Channel3_IRQn
#define SPIx_DMAx_TX_CH_IRQHandler			DMA1_Channel3_IRQHandler
#define SPIx_DMAx_TX_IFCR_CTCIFx_bb			DMA1_IFCR_CTCIF3_bb
#define SPIx_DMAx_RX_CH						DMA1_Channel2
#define SPIx_DMAx_RX_CH_IRQn				DMA1_Channel2_IRQn
#define SPIx_DMAx_RX_CH_IRQHandler			DMA1_Channel2_IRQHandler
#define SPIx_DMAx_RX_IFCR_CTCIFx_bb			DMA1_IFCR_CTCIF2_bb

#define SPIx_RX_QUEUE_LENGTH				16
#define SPIx_RX_QUEUE_BUFFER_LENGTH			16
#define SPIx_TX_QUEUE_LENGTH				16

#define SPIx_IRQHandler						SPI1_IRQHandler

#define SPIx_BAUDRATE						1000

#define SPIx_DMA_ENABLE						1

#define SPI_RTOS							1

/*---------------------------------------------------------------------------------------------------------------------+
| SD card
+---------------------------------------------------------------------------------------------------------------------*/

#define SD_SPI								SPI2

#define RCC_APBxENR_SD_SPIxEN_bb			RCC_APB1ENR_SPI2EN_bb

#define SD_CS_GPIO							GPIOB
#define SD_CS_PIN							GPIO_PIN_12
#define SD_CS_CONFIGURATION					GPIO_OUT_PP_40MHz
#define SD_SCK_GPIO							GPIOB
#define SD_SCK_PIN							GPIO_PIN_13
#define SD_SCK_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP
#define SD_MISO_GPIO						GPIOB
#define SD_MISO_PIN							GPIO_PIN_14
#define SD_MISO_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP
#define SD_MOSI_GPIO						GPIOB
#define SD_MOSI_PIN							GPIO_PIN_15
#define SD_MOSI_CONFIGURATION				GPIO_AF5_PP_40MHz_PULL_UP

#define SD_CS_bb							BITBAND(&SD_CS_GPIO->ODR, SD_CS_PIN)

#define	SD_SPI_SR_RXNE_bb					SPI2_SR_RXNE_bb

#define SD_CS_ASSERTED						0
#define SD_CS_DEASSERTED					1

/*---------------------------------------------------------------------------------------------------------------------+
| I2C
+---------------------------------------------------------------------------------------------------------------------*/

#define I2Cx								I2C1

#define I2Cx_SCL_GPIO						GPIOB
#define I2Cx_SCL_PIN						GPIO_PIN_8
#define I2Cx_SCL_CONFIGURATION				GPIO_AF4_OD_40MHz
#define I2Cx_SDA_GPIO						GPIOB
#define I2Cx_SDA_PIN						GPIO_PIN_9
#define I2Cx_SDA_CONFIGURATION				GPIO_AF4_OD_40MHz

#define RCC_APBxENR_I2CxEN_bb				RCC_APB1ENR_I2C1EN_bb

#define I2C_FREQUENCY						100000

/*---------------------------------------------------------------------------------------------------------------------+
| commands
+---------------------------------------------------------------------------------------------------------------------*/

#define COMMAND_ARGUMENT_LENGTH				32

/*---------------------------------------------------------------------------------------------------------------------+
| interript priorities
+---------------------------------------------------------------------------------------------------------------------*/

#define USARTx_DMAx_TX_CH_IRQ_PRIORITY		10
#define USARTx_IRQ_PRIORITY					10
#define TIM6_IRQ_PRIORITY					10
#define SPIx_DMAx_TX_CH_IRQ_PRIORITY		10
#define SPIx_DMAx_RX_CH_IRQ_PRIORITY		10

#endif /* CONFIG_H_ */
