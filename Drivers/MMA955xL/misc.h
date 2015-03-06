/*
 * misc.h
 *
 *  Created on: 19 paü 2014
 *      Author: admin
 */

#ifndef FUNCT_MISC_H_
#define FUNCT_MISC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

inline int8_t sign(int32_t x) {
	return (x > 0) - (x < 0);
}

#ifdef __cplusplus
}
#endif

#endif /* FUNCT_MISC_H_ */
