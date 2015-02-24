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
 * \brief power save task to run when there is no other work to do by system
 */
void system_idle_task(void *parameters);

#endif  // POWER_MANAGEMENT_H_
