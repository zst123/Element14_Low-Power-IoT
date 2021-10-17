/***************************************************************************//**
* \file cyhal_wdt.c
*
* \brief
* Provides a high level interface for interacting with the Cypress Watchdog Timer.
* This interface abstracts out the chip specific details. If any chip specific
* functionality is necessary, or performance is critical the low level functions
* can be used directly.
*
*
********************************************************************************
* \copyright
* Copyright 2019-2021 Cypress Semiconductor Corporation
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
*******************************************************************************/

#include <stdbool.h>
#include "cyhal_wdt.h"
#include "cyhal_wdt_impl.h"
#include "cy_wdt.h"
#include "cy_utils.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(COMPONENT_CAT1A) || defined(COMPONENT_CAT1B)
#define _cyhal_wdt_lock()   Cy_WDT_Lock()
#define _cyhal_wdt_unlock() Cy_WDT_Unlock()

// ILO Frequency = 32768 Hz
// ILO Period = 1 / 32768 Hz = .030518 ms
// WDT Reset Period (timeout_ms) = .030518 ms * (2 * 2^(16 - ignore_bits) + match)
// ignore_bits range: 0 - 12
// match range: 0 - 2^(16 - ignore_bits)
static const _cyhal_wdt_ignore_bits_data_t _cyhal_wdt_ignore_data[] = {
    {4000, 3001}, //  0 bit(s): min period: 4000ms, max period: 6000ms, round up from 3001+ms
    {2000, 1501}, //  1 bit(s): min period: 2000ms, max period: 3000ms, round up from 1501+ms
    {1000,  751}, //  2 bit(s): min period: 1000ms, max period: 1500ms, round up from 751+ms
    { 500,  376}, //  3 bit(s): min period:  500ms, max period:  750ms, round up from 376+ms
    { 250,  188}, //  4 bit(s): min period:  250ms, max period:  375ms, round up from 188+ms
    { 125,   94}, //  5 bit(s): min period:  125ms, max period:  187ms, round up from 94+ms
    {  63,   47}, //  6 bit(s): min period:   63ms, max period:   93ms, round up from 47+ms
    {  32,   24}, //  7 bit(s): min period:   32ms, max period:   46ms, round up from 24+ms
    {  16,   12}, //  8 bit(s): min period:   16ms, max period:   23ms, round up from 12+ms
    {   8,    6}, //  9 bit(s): min period:    8ms, max period:   11ms, round up from 6+ms
    {   4,    3}, // 10 bit(s): min period:    4ms, max period:    5ms, round up from 3+ms
    {   2,    2}, // 11 bit(s): min period:    2ms, max period:    2ms, round up from 2+ms
    {   1,    1}, // 12 bit(s): min period:    1ms, max period:    1ms, round up from 1+ms
};
#elif defined(COMPONENT_CAT2)
#define _cyhal_wdt_lock()
#define _cyhal_wdt_unlock()

// ILO Frequency = 40000 Hz
// ILO Period = 1 / 40000 Hz = .025 ms
// WDT Reset Period (timeout_ms) = .025 ms * (2 * 2^(16 - ignore_bits) + match)
// ignore_bits range: 0 - 16 (only need up to 12)
// match range: 0 - 2^(16 - ignore_bits)
static const _cyhal_wdt_ignore_bits_data_t _cyhal_wdt_ignore_data[] = {
    {3277, 2458}, //  0 bit(s): min period: 3277ms, max period: 4915ms, round up from 2458+ms
    {1639, 1229}, //  1 bit(s): min period: 1639ms, max period: 2457ms, round up from 1229+ms
    { 820,  615}, //  2 bit(s): min period:  820ms, max period: 1228ms, round up from 615+ms
    { 410,  308}, //  3 bit(s): min period:  410ms, max period:  614ms, round up from 308+ms
    { 205,  154}, //  4 bit(s): min period:  205ms, max period:  307ms, round up from 154+ms
    { 103,   77}, //  5 bit(s): min period:  103ms, max period:  153ms, round up from 77+ms
    {  52,   39}, //  6 bit(s): min period:   52ms, max period:   76ms, round up from 39+ms
    {  26,   20}, //  7 bit(s): min period:   26ms, max period:   38ms, round up from 20+ms
    {  13,   10}, //  8 bit(s): min period:   13ms, max period:   19ms, round up from 10+ms
    {   7,    5}, //  9 bit(s): min period:    7ms, max period:    9ms, round up from 5+ms
    {   4,    3}, // 10 bit(s): min period:    4ms, max period:    4ms, round up from 3+ms
    {   2,    2}, // 11 bit(s): min period:    2ms, max period:    2ms, round up from 2+ms
    {   1,    1}, // 12 bit(s): min period:    1ms, max period:    1ms, round up from 1+ms
};
#endif

static bool _cyhal_wdt_initialized = false;
static uint16_t _cyhal_wdt_initial_timeout_ms = 0;
static uint16_t _cyhal_wdt_rounded_timeout_ms = 0;
static uint32_t _cyhal_wdt_ignore_bits = 0;

// Rounds up *timeout_ms if it's outside of the valid timeout range (_cyhal_wdt_ignore_data)
__STATIC_INLINE uint32_t _cyhal_wdt_timeout_to_ignore_bits(uint32_t *timeout_ms)
{
    for (uint32_t i = 0; i <= _CYHAL_WDT_MAX_IGNORE_BITS; i++)
    {
        if (*timeout_ms >= _cyhal_wdt_ignore_data[i].round_threshold_ms)
        {
            if (*timeout_ms < _cyhal_wdt_ignore_data[i].min_period_ms)
            {
                *timeout_ms = _cyhal_wdt_ignore_data[i].min_period_ms;
            }
            return i;
        }
    }
    return _CYHAL_WDT_MAX_IGNORE_BITS; // Ideally should never reach this
}

__STATIC_INLINE uint16_t _cyhal_wdt_timeout_to_match(uint16_t timeout_ms, uint16_t ignore_bits)
{
    uint32_t timeout = ((uint32_t)timeout_ms * CY_SYSCLK_ILO_FREQ) / 1000;
    return (uint16_t)(timeout - (1UL << (17 - ignore_bits)) + Cy_WDT_GetCount());
}

cy_rslt_t cyhal_wdt_init(cyhal_wdt_t *obj, uint32_t timeout_ms)
{
    if ((timeout_ms == 0) || (timeout_ms > _CYHAL_WDT_MAX_TIMEOUT_MS))
        return CY_RSLT_WDT_INVALID_TIMEOUT;
    if (_cyhal_wdt_initialized)
        return CY_RSLT_WDT_ALREADY_INITIALIZED;

    _cyhal_wdt_initial_timeout_ms = timeout_ms;
    _cyhal_wdt_ignore_bits = _cyhal_wdt_timeout_to_ignore_bits(&timeout_ms);
    _cyhal_wdt_rounded_timeout_ms = timeout_ms;

    cyhal_wdt_stop(obj); // Stop and unlock before doing other work
    Cy_WDT_ClearInterrupt();
    Cy_WDT_MaskInterrupt();
    Cy_WDT_SetIgnoreBits(_cyhal_wdt_ignore_bits);
    Cy_WDT_SetMatch(_cyhal_wdt_timeout_to_match(_cyhal_wdt_rounded_timeout_ms, _cyhal_wdt_ignore_bits));
    cyhal_wdt_start(obj);
    _cyhal_wdt_initialized = true;
    
    return CY_RSLT_SUCCESS;
}

void cyhal_wdt_free(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    cyhal_wdt_stop(obj);
    _cyhal_wdt_initialized = false;
}

void cyhal_wdt_kick(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    Cy_WDT_ClearWatchdog(); /* Clear to prevent reset from WDT */
    _cyhal_wdt_unlock();
    Cy_WDT_SetMatch(_cyhal_wdt_timeout_to_match(_cyhal_wdt_rounded_timeout_ms, _cyhal_wdt_ignore_bits));
    _cyhal_wdt_lock();
}

void cyhal_wdt_start(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    _cyhal_wdt_unlock();
    Cy_WDT_Enable();
    _cyhal_wdt_lock();
}

void cyhal_wdt_stop(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    _cyhal_wdt_unlock();
    Cy_WDT_Disable();
}

uint32_t cyhal_wdt_get_timeout_ms(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    return _cyhal_wdt_initial_timeout_ms;
}

uint32_t cyhal_wdt_get_max_timeout_ms(void)
{
    return _CYHAL_WDT_MAX_TIMEOUT_MS;
}

bool cyhal_wdt_is_enabled(cyhal_wdt_t *obj)
{
    CY_UNUSED_PARAMETER(obj);
    return Cy_WDT_IsEnabled();
}

#if defined(__cplusplus)
}
#endif