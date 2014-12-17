/**
 * \file config.h
 * \brief Basic configuration of the project
 *
 * Basic configuration of the project that aim to the uC blocks and peripherials
 * Do not define here board specific region/pins or external OEM paths
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author Mazeryt Freager
 * \date 2014-08-06
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "hdr/hdr_bitband.h"

/*---------------------------------------------------------------------------------------------------------------------+
| Device
+---------------------------------------------------------------------------------------------------------------------*/

#define DEVICE_VCORE_mV						1800	///< VCORE voltage, configured via VOS in PWR_CR, in mV

#define FREQUENCY							32000000	///< desired target frequency of the core

#define CLOCK_SOURCE						USING_HSE  ///< describe what is clock source of uC

#define USING_HSI							0

#define USING_HSE							1

#endif /* CONFIG_H_ */
