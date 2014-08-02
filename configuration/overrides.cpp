/**
 * \file overrides.cpp
 * \brief Overrides of C++ functions.
 *
 * Overrides of C++ functions.
 *
 * \author Mazeryt Freager
 * \date 2012-08-28
 */

#include <stddef.h>

#include "FreeRTOS.h"

#include "helper.h"

/**
 * \brief Operator new without exceptions.
 *
 * Operator new without exceptions. This uses FreeRTOS memory allocation.
 *
 * \param [in] size is the requested size of memory
 *
 * \return pointer to allocated memory or NULL if allocation failed
 */

void* operator new(size_t size) throw()
{
	return pvPortMalloc(size);
}

/**
 * \brief Operator delete without exceptions.
 *
 * Operator delete without exceptions. This uses FreeRTOS memory allocation.
 *
 * \param [in] ptr is the pointer to memory segment that should be freed
 */

void operator delete(void* ptr) throw ()
{
	if (ptr)
		vPortFree(ptr);
}

/**
 * \brief Pure virtual function called handler.
 *
 * Pure virtual function called handler.
 */

extern "C" void __cxa_pure_virtual(void)
{
	PLATFORM_ASSERT("Pure virtual function was called...");
}
