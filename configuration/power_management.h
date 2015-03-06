/**
 * \file power_management.h
 * \brief Simple power management interface
 *
 * This is simple power management interface for example system
 *
 * \author Mazeryt Freager
 * \date 2012-08-30
 */

#ifndef POWER_MANAGEMENT_H_
#define POWER_MANAGEMENT_H_

/**
 * \brief Definitions for wake-up time base (wtb)
 *
 *  To calculate wtb it is needed known __SYS_WAKE_UP_COUNTER (cnt) and __RTC_WAKEUP_CLK_DIV (div)
 *  Also accurate RTC clock source frequency is necessary (rtc_f)
 *
 *  			to calculate wtb = (div * cnt)/rtc_f
 *
 *  example:
 *  div = RTCDiv_16, __SYS_WAKE_UP_COUNTER = 0x1FFF Then Wakeup Time Base: 4s
 *  div = RTCDiv_4,  __SYS_WAKE_UP_COUNTER = 0x320  Then WTB: 1s
 */
//Current wtb = 0.2s
#define __SYS_WAKE_UP_COUNTER		0x640
#define __RTC_WAKEUP_CLK_DIV		RTC_WakeUpClock_RTCCLK_Div4


/**
 * \brief Prepare system to work in Low Power Mode
 */
void configure_sys_low_power(void);

/**
 * \brief Power save task to run when there is no other work to do by system
 */
void system_idle_task(void *parameters);

#endif  // POWER_MANAGEMENT_H_
