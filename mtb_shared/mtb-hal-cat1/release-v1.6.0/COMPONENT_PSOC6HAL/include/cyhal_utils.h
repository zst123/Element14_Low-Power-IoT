/***************************************************************************//**
* \file cyhal_utils.h
*
* \brief
* Provides utility functions.
*
********************************************************************************
* \copyright
* Copyright 2018-2021 Cypress Semiconductor Corporation
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

/** \cond INTERNAL */
/**
* \addtogroup group_hal_impl
* \{
* Common utility macros & functions used by multiple HAL drivers.
*/

#pragma once

#include "cy_result.h"
#include "cyhal_hw_types.h"
#include "cy_utils.h"
#if defined(COMPONENT_CAT1A) || defined(COMPONENT_CAT1B) || defined(COMPONENT_CAT2)
#include "cyhal_utils_psoc.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define _CYHAL_UTILS_NS_PER_SECOND (1000000000)

#define _CYHAL_UTILS_IRQN_OFFSET           (16U) /**< Offset for implementation-defined ISR type numbers (IRQ0 = 16) */
#define _CYHAL_UTILS_GET_CURRENT_IRQN()    ((IRQn_Type) (__get_IPSR() - _CYHAL_UTILS_IRQN_OFFSET)) /**< Macro to get the IRQn of the current ISR */

/**
* \addtogroup group_hal_impl_pin_package
* \{
*/

/** Attempts to reserve a resource block that connects to the specified pin from the provided resource pin mapping table.
  * If the first matching block cannot be reserved, this will continue searching until either a match is successfully
  * reserved or the end of the table is reached.
 * This is a convenience utility for _cyhal_utils_get_resource() if the mappings is an array of known size.
  *
  * @param[in] pin      The pin to which the reserved hardware block must connect
  * @param[in] mappings The mapping of pin to hardware block
  * @return The entry for the specified pin corresponding to the reserved block, if it exists and was successfully reserved.
            Otherwise, NULL.
  */
#define _CYHAL_UTILS_TRY_ALLOC(pin, mappings) \
    _cyhal_utils_try_alloc(pin, mappings, sizeof(mappings)/sizeof(cyhal_resource_pin_mapping_t))

/** Looks up the resource block that connects to the specified pins from the provided resource pin mapping table.
 * This is a convenience utility for _cyhal_utils_get_resource() if the mappings is an array of known size.
 *
 * @param[in] pin        The pin to lookup the hardware block for
 * @param[in] mappings   The mappings of pin to hardware block
 * @return The entry for the specified pin if it exists, or null if it doesn't.
 */
#define _CYHAL_UTILS_GET_RESOURCE(pin, mappings) _cyhal_utils_get_resource(pin, mappings, sizeof(mappings)/sizeof(cyhal_resource_pin_mapping_t), NULL)

/** Looks up the resource block that connects to the specified pins from the provided resource pin mapping table.
 *
 * @param[in] pin        The pin to lookup the hardware block for
 * @param[in] mappings   The mappings of pin to hardware block
 * @param[in] count      The number of items in the mappings table
 * @param[in] block_res  If not NULL, find pin mapping, that belongs to specified block
 * @return The entry for the specified pin if it exists, or null if it doesn't.
 */
const cyhal_resource_pin_mapping_t *_cyhal_utils_get_resource(cyhal_gpio_t pin, const cyhal_resource_pin_mapping_t* mappings, size_t count, const cyhal_resource_inst_t* block_res);

/** Attempts to reserve a resource block that connects to the specified pin from the provided resource pin mapping table.
  * If the first matching block cannot be reserved, this will continue searching until either a match is successfully
  * reserved or the end of the table is reached.
  *
  * @param[in] pin      The pin to which the reserved hardware block must connect
  * @param[in] mappings The mapping of pin to hardware block
  * @param[in] size     The number of items in the mappings table
  * @return The entry for the specified pin corresponding to the reserved block, if it exists and was successfully reserved.
            Otherwise, NULL.
  */
const cyhal_resource_pin_mapping_t* _cyhal_utils_try_alloc(cyhal_gpio_t pin, const cyhal_resource_pin_mapping_t *mappings, size_t count);

/** Checks to see if the provided pin is a no-connect (CYHAL_NC_PIN_VALUE). If not, calls
 * _cyhal_utils_disconnect_and_free().
 *
 * @param[in] pin       The pin to disconnect and free
 */
void _cyhal_utils_release_if_used(cyhal_gpio_t *pin);

/** Determine if two resources are the same. If more than two instances need to be
 * compared, it is better to call \ref _cyhal_utils_resources_equal_all().
 *
 * @param[in] resource1 First resource to compare
 * @param[in] resource2 Second resource to compare
 * @return Boolean indicating whether two resources are the same
 */
bool _cyhal_utils_resources_equal(const cyhal_resource_inst_t *resource1, const cyhal_resource_inst_t *resource2);

/** Determine if the set of resources are the same. This expects at least two resource
 * instances to be provided. NOTE: If only two instances are provided it is better
 * to use \ref _cyhal_utils_resources_equal().
 *
 * @param[in] count The number of resources being provided to compare
 * @param[in] ...   Variable List of const cyhal_resource_inst_t* items to compare
 * @return Boolean indicating whether resource instances are the same
 */
bool _cyhal_utils_resources_equal_all(uint32_t count, ...);

/** Converts a set of flags from one representation to a equivalent set of flags
 * in another representation.
 * For example usage see cyhal_syspm.c or cyhal_sdhc.c
 *
 * @param[in] map           New flag values. Each index corresponds to a bit position
 *                          from the source_flags. Index 0 is a default value if nothing
 *                          matches.
 * @param[in] count         Count of the total flags in the list.
 * @param[in] source_flags  Source flags to be converted.
 * @return Converted flags.
 */
uint32_t _cyhal_utils_convert_flags(const uint32_t map[], uint32_t count, uint32_t source_flags);

#if defined(__cplusplus)
}
#endif

/** \} group_hal_impl_utils */
/** \} group_hal_impl */
/** \endcond */
