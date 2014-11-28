/** \file vectors_STM32L1xx_md.c
 * \brief STM32L1xx medium-density vector table and __Default_Handler()
 *
 * STM32L1xx medium-density vector table and __Default_Handler()
 *
 * chip: STM32L1xx medium-density
 *
 * \author Mazeryt Freager
 * \date 2012-08-23
 */

/**
 * \brief Default interrupt handler.
 *
 * Default interrupt handler, used for interrupts that don't have their
 * own handler defined.
*/

void __Default_Handler(void) __attribute__ ((interrupt));
void __Default_Handler(void)
{
	while (1);
}

/*-----------------------------------------------------------------------------+
| assign all unhandled interrupts to the default handler
+-----------------------------------------------------------------------------*/

// Non maskable interrupt. The RCC Clock Security System (CSS) is linked to the NMI vector.
void NMI_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// All class of fault
void HardFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Memory management
void MemManage_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Pre-fetch fault, memory access fault
void BusFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Undefined instruction or illegal state
void UsageFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x1C
void __Reserved_0x1C_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x20
void __Reserved_0x20_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x24
void __Reserved_0x24_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x28
void __Reserved_0x28_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// System service call via SWI instruction
void SVC_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Debug Monitor
void DebugMon_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x34
void __Reserved_0x34_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Pendable request for system service
void PendSV_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// System tick timer
void SysTick_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Window Watchdog interrupt
void WWDG_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// PVD through EXTI Line detection interrupt
void PVD_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Tamper and TimeStamp through EXTI line interrupts
void TAMPER_STAMP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RTC Wakeup through EXTI line interrupt
void RTC_WKUP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Flash global interrupt
void FLASH_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RCC global interrupt
void RCC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line0 interrupt
void EXTI0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line1 interrupt
void EXTI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line2 interrupt
void EXTI2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line3 interrupt
void EXTI3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line4 interrupt
void EXTI4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel1 global interrupt
void DMA1_Channel1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel2 global interrupt
void DMA1_Channel2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel3 global interrupt
void DMA1_Channel3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel4 global interrupt
void DMA1_Channel4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel5 global interrupt
void DMA1_Channel5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel6 global interrupt
void DMA1_Channel6_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel7 global interrupt
void DMA1_Channel7_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// ADC1 global interrupt
void ADC1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB High priority interrupt
void USB_HP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB Low priority interrupt
void USB_LP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DAC interrupt
void DAC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Comparator wakeup through EXTI line (21 and 22) interrupt
void COMP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line[9:5] interrupts
void EXTI9_5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// LCD global interrupt
void LCD_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM9 global interrupt
void TIM9_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM10 global interrupt
void TIM10_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM11 global interrupt
void TIM11_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM2 global interrupt
void TIM2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM3 global interrupt
void TIM3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM4 global interrupt
void TIM4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C1 event interrupt
void I2C1_EV_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C1 error interrupt
void I2C1_ER_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C2 event interrupt
void I2C2_EV_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C2 error interrupt
void I2C2_ER_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI1 global interrupt
void SPI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI2 global interrupt
void SPI2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART1 global interrupt
void USART1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART2 global interrupt
void USART2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART3 global interrupt
void USART3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line[15:10] interrupts
void EXTI15_10_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RTC Alarms (A and B) through EXTI line interrupt
void RTC_Alarm_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB Device FS Wakeup through EXTI line interrupt
void USB_FS_WKUP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM6 global interrupt
void TIM6_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM7 global interrupt
void TIM7_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

/*-----------------------------------------------------------------------------+
| Vector table
+-----------------------------------------------------------------------------*/

extern const char __main_stack_end[];		// imported main stack end (from linker script)

void Reset_Handler(void);					// import the address of Reset_Handler()

void (* const vectors[])(void) __attribute__ ((section(".vectors"))) = {
		(void (*)(void))__main_stack_end,	// Main stack end address
		Reset_Handler,						// Reset
		NMI_Handler,						// Non maskable interrupt. The RCC Clock Security System (CSS) is linked to the NMI vector.
		HardFault_Handler,					// All class of fault
		MemManage_Handler,					// Memory management
		BusFault_Handler,					// Pre-fetch fault, memory access fault
		UsageFault_Handler,					// Undefined instruction or illegal state
		__Reserved_0x1C_Handler,			// Reserved 0x1C
		__Reserved_0x20_Handler,			// Reserved 0x20
		__Reserved_0x24_Handler,			// Reserved 0x24
		__Reserved_0x28_Handler,			// Reserved 0x28
		SVC_Handler,						// System service call via SWI instruction
		DebugMon_Handler,					// Debug Monitor
		__Reserved_0x34_Handler,			// Reserved 0x34
		PendSV_Handler,						// Pendable request for system service
		SysTick_Handler,					// System tick timer
		WWDG_IRQHandler,					// Window Watchdog interrupt
		PVD_IRQHandler,						// PVD through EXTI Line detection interrupt
		TAMPER_STAMP_IRQHandler,			// Tamper and TimeStamp through EXTI line interrupts
		RTC_WKUP_IRQHandler,				// RTC Wakeup through EXTI line interrupt
		FLASH_IRQHandler,					// Flash global interrupt
		RCC_IRQHandler,						// RCC global interrupt
		EXTI0_IRQHandler,					// EXTI Line0 interrupt
		EXTI1_IRQHandler,					// EXTI Line1 interrupt
		EXTI2_IRQHandler,					// EXTI Line2 interrupt
		EXTI3_IRQHandler,					// EXTI Line3 interrupt
		EXTI4_IRQHandler,					// EXTI Line4 interrupt
		DMA1_Channel1_IRQHandler,			// DMA1 Channel1 global interrupt
		DMA1_Channel2_IRQHandler,			// DMA1 Channel2 global interrupt
		DMA1_Channel3_IRQHandler,			// DMA1 Channel3 global interrupt
		DMA1_Channel4_IRQHandler,			// DMA1 Channel4 global interrupt
		DMA1_Channel5_IRQHandler,			// DMA1 Channel5 global interrupt
		DMA1_Channel6_IRQHandler,			// DMA1 Channel6 global interrupt
		DMA1_Channel7_IRQHandler,			// DMA1 Channel7 global interrupt
		ADC1_IRQHandler,					// ADC1 global interrupt
		USB_HP_IRQHandler,					// USB High priority interrupt
		USB_LP_IRQHandler,					// USB Low priority interrupt
		DAC_IRQHandler,						// DAC interrupt
		COMP_IRQHandler,					// Comparator wakeup through EXTI line (21 and 22) interrupt
		EXTI9_5_IRQHandler,					// EXTI Line[9:5] interrupts
		LCD_IRQHandler,						// LCD global interrupt
		TIM9_IRQHandler,					// TIM9 global interrupt
		TIM10_IRQHandler,					// TIM10 global interrupt
		TIM11_IRQHandler,					// TIM11 global interrupt
		TIM2_IRQHandler,					// TIM2 global interrupt
		TIM3_IRQHandler,					// TIM3 global interrupt
		TIM4_IRQHandler,					// TIM4 global interrupt
		I2C1_EV_IRQHandler,					// I2C1 event interrupt
		I2C1_ER_IRQHandler,					// I2C1 error interrupt
		I2C2_EV_IRQHandler,					// I2C2 event interrupt
		I2C2_ER_IRQHandler,					// I2C2 error interrupt
		SPI1_IRQHandler,					// SPI1 global interrupt
		SPI2_IRQHandler,					// SPI2 global interrupt
		USART1_IRQHandler,					// USART1 global interrupt
		USART2_IRQHandler,					// USART2 global interrupt
		USART3_IRQHandler,					// USART3 global interrupt
		EXTI15_10_IRQHandler,				// EXTI Line[15:10] interrupts
		RTC_Alarm_IRQHandler,				// RTC Alarms (A and B) through EXTI line interrupt
		USB_FS_WKUP_IRQHandler,				// USB Device FS Wakeup through EXTI line interrupt
		TIM6_IRQHandler,					// TIM6 global interrupt
		TIM7_IRQHandler,					// TIM7 global interrupt
};
