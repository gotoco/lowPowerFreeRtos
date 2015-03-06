#include "FreeRTOS.h"
#include "portmacro.h"
#include "semphr.h"
#include "task.h"

#include "mma955x_adapter.h"
#include "bsp.h"
#include "gpio.h"
#include "spi.h"

static xSemaphoreHandle accDataSemaphore;
static xSemaphoreHandle _mma955x_rw_mutex;

static void _board_specific_configuration(void)
{
	//Logic translator configuration from bsp.h: OE configuration so High on pin enable translator
	gpioConfigurePin(ACC_OE_RESET_MOSI_GPIO, ACC_OE_RESET_MOSI_PIN, ACC_OE_RESET_MOSI_CONFIGURATION	);
	gpioConfigurePin(ACC_OE_SCK_NSS_GPIO, ACC_OE_SCK_NSS_PIN, ACC_OE_SCK_NSS_CONFIGURATION);
	gpioConfigurePin(ACC_OE_INT_MISO_GPIO, ACC_OE_INT_MISO_PIN, ACC_OE_INT_MISO_CONFIGURATION);
	ACC_OE_RESET_MOSI_bb = 1;
	ACC_OE_SCK_NSS_bb 	 = 1;
	ACC_OE_INT_MISO_bb	 = 1;
}

static void _configure_reset_pin(void)
{
	gpioConfigurePin(ACC_RESET_GPIO, ACC_RESET_PIN, ACC_RESET_CONFIGURATION);
}

void mma955x_low_level_init(void) {
#ifndef ACC_RESET_GPIO
	#error Please define ACC_RESET_GPIO in example inside the bsp.h file
#endif
#ifndef ACC_RESET_PIN
	#error Please define ACC_RESET_PIN in example inside the bsp.h file
#endif
#ifndef ACC_RESET_CONFIGURATION
	#error Please define ACC_RESET_CONFIGURATION in example inside the bsp.h file
#endif
#ifndef ACC_INT_GPIO
	#error Please define ACC_INT_GPIO in example inside the bsp.h file
#endif
#ifndef ACC_INT_PIN
	#error Please define ACC_INT_PIN in example inside the bsp.h file
#endif
#ifndef ACC_INT_CONFIGURATION
	#error Please define ACC_INT_CONFIGURATION in example inside the bsp.h file
#endif
	vSemaphoreCreateBinary(accDataSemaphore);

	_mma955x_configure_interrupt();

	_configure_reset_pin();

	_board_specific_configuration();

}

void mma955x_delay(portTickType delay_in_ms) {
	portTickType lastTickCnt = xTaskGetTickCount();
	vTaskDelayUntil(&lastTickCnt, delay_in_ms / portTICK_RATE_MS);
}


void mma955x_mutex_lock(void)
{
	xSemaphoreTake(accDataSemaphore, portMAX_DELAY);
}

void mma955x_mutex_unlock(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGive(_mma955x_rw_mutex);
}

/**
 * \brief Wait for COmmand COmplete
 */
void mma955x_wait_for_coco(void)
{

}

int mma955x_read_data(uint8_t address, uint8_t *data, uint8_t size)
{
	int result = -1;
	if(__MMA955x_COMM_USES == __MMA955x_VIA_SPI){
		mma955x_mutex_lock();
			spiStart();
			spiTransfer(&address, NULL, 1);
			spiTransfer(NULL, data, size);
			spiStop();
		mma955x_mutex_unlock();
	} else if(__MMA955x_COMM_USES == __MMA955x_VIA_I2C){

	}

	return result;
}

int mma955x_write_data(uint8_t address, uint8_t *data, uint8_t size)
{
	int result = -1;
	if(__MMA955x_COMM_USES == __MMA955x_VIA_SPI){
		mma955x_mutex_lock();
			spiStart();
			spiTransfer(&address, NULL, 1);
			spiTransfer(data, NULL, size);
			spiStop();
		mma955x_mutex_unlock();
	} else if(__MMA955x_COMM_USES == __MMA955x_VIA_I2C){

	}

	mma955x_delay(1);

	return result;
}

signed portBASE_TYPE mma955x_waitForData(portTickType timeout_in_ms) {
	return xSemaphoreTake(accDataSemaphore, timeout_in_ms / portTICK_RATE_MS);
}

void _mma955x_configure_interrupt(void) {
	EXTI->IMR|=EXTI_EMR_MR0;
	EXTI->RTSR|=EXTI_RTSR_TR0;
	EXTI->FTSR&= ~(EXTI_FTSR_TR0);

	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI0_PA;
	NVIC_EnableIRQ(EXTI0_IRQn);

	//ACC_INT PIN configuration
	gpioConfigurePin(ACC_INT_GPIO, ACC_INT_PIN, ACC_INT_CONFIGURATION);
}

extern "C" void EXTI0_IRQHandler(void) __attribute((interrupt));
void EXTI0_IRQHandler(void) {
	static portBASE_TYPE xHigherPriorityTaskWoken;

	EXTI->PR=EXTI_PR_PR0;
	xSemaphoreGiveFromISR(accDataSemaphore, &xHigherPriorityTaskWoken);
}
