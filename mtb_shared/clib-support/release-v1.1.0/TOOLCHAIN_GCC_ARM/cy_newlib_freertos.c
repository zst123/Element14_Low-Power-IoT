/***********************************************************************************************//**
 * \file cy_newlib_freertos.c
 *
 * \brief
 * Newlib port for FreeRTOS
 *
 ***************************************************************************************************
 * \copyright
 * Copyright 2018-2019 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <stdatomic.h>
#include <stdint.h>
#include <sys/types.h>
#include <envlock.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <cmsis_compiler.h>
#include "cy_mutex_pool.h"

#if configUSE_MUTEXES == 0 || configUSE_RECURSIVE_MUTEXES == 0 || \
    configSUPPORT_STATIC_ALLOCATION == 0
#warning \
    configUSE_MUTEXES, configUSE_RECURSIVE_MUTEXES, and configSUPPORT_STATIC_ALLOCATION must be enabled and set to 1 to use clib-support

#else

#if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
static SemaphoreHandle_t cy_malloc_mutex = NULL, cy_env_mutex = NULL, cy_ctor_mutex = NULL;
SemaphoreHandle_t cy_timer_mutex;
#endif


//--------------------------------------------------------------------------------------------------
// cy_toolchain_init
//--------------------------------------------------------------------------------------------------
void cy_toolchain_init(void)
{
    #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
    cy_malloc_mutex = cy_mutex_pool_create();
    cy_env_mutex    = cy_mutex_pool_create();
    cy_ctor_mutex   = cy_mutex_pool_create();
    cy_timer_mutex  = cy_mutex_pool_create();
    #endif
}


// XMC Lib already defines this so, don't redefine for those devices
#if !defined(COMPONENT_CAT3)
//--------------------------------------------------------------------------------------------------
// _sbrk
//--------------------------------------------------------------------------------------------------
caddr_t _sbrk(uint32_t incr)
{
    extern uint8_t  __HeapBase, __HeapLimit;
    static uint8_t* heapBrk = &__HeapBase;
    uint8_t*        prevBrk = heapBrk;
    if (incr > (uint32_t)(&__HeapLimit - heapBrk))
    {
        errno = ENOMEM;
        return (caddr_t)-1;
    }
    heapBrk += incr;
    return (caddr_t)prevBrk;
}


#endif // if !defined(COMPONENT_CAT3)


//--------------------------------------------------------------------------------------------------
// __malloc_lock
//--------------------------------------------------------------------------------------------------
void __malloc_lock(struct _reent* reent)
{
    (void)reent;
    #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
    cy_mutex_pool_acquire(cy_malloc_mutex);
    #else
    vTaskSuspendAll();
    #endif
}


//--------------------------------------------------------------------------------------------------
// __malloc_unlock
//--------------------------------------------------------------------------------------------------
void __malloc_unlock(struct _reent* reent)
{
    (void)reent;
    #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
    cy_mutex_pool_release(cy_malloc_mutex);
    #else
    xTaskResumeAll();
    #endif
}


//--------------------------------------------------------------------------------------------------
// __env_lock
//--------------------------------------------------------------------------------------------------
void __env_lock(struct _reent* reent)
{
    (void)reent;
    #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
    cy_mutex_pool_acquire(cy_env_mutex);
    #else
    vTaskSuspendAll();
    #endif
}


//--------------------------------------------------------------------------------------------------
// __env_unlock
//--------------------------------------------------------------------------------------------------
void __env_unlock(struct _reent* reent)
{
    (void)reent;
    #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
    cy_mutex_pool_release(cy_env_mutex);
    #else
    xTaskResumeAll();
    #endif
}


// The __cxa_guard_acquire, __cxa_guard_release, and __cxa_guard_abort
// functions ensure that constructors for static local variables are
// executed exactly once. For more information, see:
// https://itanium-cxx-abi.github.io/cxx-abi/abi.html#obj-ctor

typedef struct
{
    atomic_uchar initialized;
    uint8_t      acquired;
} cy_cxa_guard_object_t;

//--------------------------------------------------------------------------------------------------
// __cxa_guard_acquire
//--------------------------------------------------------------------------------------------------
int __cxa_guard_acquire(cy_cxa_guard_object_t* guard_object)
{
    int acquired = 0;
    if (0 == atomic_load(&guard_object->initialized))
    {
        #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
        cy_mutex_pool_acquire(cy_ctor_mutex);
        #else
        vTaskSuspendAll();
        #endif
        if (0 == atomic_load(&guard_object->initialized))
        {
            acquired = 1;
            #ifndef NDEBUG
            if (guard_object->acquired)
            {
                __BKPT(0);  // acquire called again without release/abort
            }
            #endif
            guard_object->acquired = 1;
        }
        else
        {
            #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
            cy_mutex_pool_release(cy_ctor_mutex);
            #else
            xTaskResumeAll();
            #endif
        }
    }
    return acquired;
}


//--------------------------------------------------------------------------------------------------
// __cxa_guard_abort
//--------------------------------------------------------------------------------------------------
void __cxa_guard_abort(cy_cxa_guard_object_t* guard_object)
{
    if (guard_object->acquired)
    {
        guard_object->acquired = 0;
        #if (configHEAP_ALLOCATION_SCHEME != HEAP_ALLOCATION_TYPE3)
        cy_mutex_pool_release(cy_ctor_mutex);
        #else
        xTaskResumeAll();
        #endif
    }
    #ifndef NDEBUG
    else
    {
        __BKPT(0);  // __cxa_guard_abort called when not acquired
    }
    #endif
}


//--------------------------------------------------------------------------------------------------
// __cxa_guard_release
//--------------------------------------------------------------------------------------------------
void __cxa_guard_release(cy_cxa_guard_object_t* guard_object)
{
    atomic_store(&guard_object->initialized, 1);
    __cxa_guard_abort(guard_object);    // Release mutex
}


#endif // if configUSE_MUTEXES == 0 || configUSE_RECURSIVE_MUTEXES == 0 ||
// configSUPPORT_STATIC_ALLOCATION == 0
