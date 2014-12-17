/**
 * \file helper.h
 * \brief Helper macros and header for helper.cpp
 * \author: Mazeryt Freager
 * \date 2014-07-20
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <stdint.h>
#include <stdio.h>

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define STRINGIZE_DETAIL(x)					#x
#define STRINGIZE(x)						STRINGIZE_DETAIL(x)

#define PLATFORM_ASSERT(x)					do { printf("Assertion \"%s\" failed at line %d in %s\n", \
											x, __LINE__, __FILE__); while (1); } while (0)

#define ASSERT(message, assertion)			do { if (!(assertion)) \
											PLATFORM_ASSERT(message); } while (0)

/*---------------------------------------------------------------------------------------------------------------------+
| global variables
+---------------------------------------------------------------------------------------------------------------------*/

extern volatile uint16_t tim6OverflowCount;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void configureTimerForRuntimestats(void);

#ifdef __cplusplus
}
#endif

#endif /* HELPER_H_ */
