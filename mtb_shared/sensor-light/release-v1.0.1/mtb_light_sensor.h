/***********************************************************************************************//**
 * \file mtb_light_sensor.h
 *
 * This file is the public interface of the light sensor.
 *
 ***************************************************************************************************
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
 **************************************************************************************************/

#pragma once

/**
 * \addtogroup group_board_libs Light Sensor
 * \{
 * Basic set of APIs for interacting with the light sensor.
 */

#include "cy_result.h"
#include "cyhal_adc.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** Structure defining the pin and ADC channel used to interact with the
 * light sensor.
 */
typedef struct mtb_light_sensor
{
    cyhal_adc_channel_t channel;  /**< ADC channel obj */
} mtb_light_sensor_t;

/**
 * Initialize the ADC communication with the light sensor.
 * @param[out] light_sensor  Pointer to a Light Sensor object. The caller must allocate the memory
 *  for this object but the init function will initialize its contents.
 * @param[in] adc_obj       ADC instance to use for communicating with the light sensor.
 * @param[in] pin           The pin connected to the light sensor.
 * @return CY_RSLT_SUCCESS if properly initialized, else an error indicating what went wrong.
 *
 * \note The ADC initialization function, cyhal_adc_init(), must be called
 * before this function is called.
 */
cy_rslt_t mtb_light_sensor_init(mtb_light_sensor_t* light_sensor, cyhal_adc_t* adc_obj,
                                cyhal_gpio_t pin);

/**
 * Returns the current light level.
 * @param[in] light_sensor  Light sensor instance.
 * @return A percentage - a value between 0 (no light) and 100 (maximum measurable light).
 */
uint8_t mtb_light_sensor_light_level(mtb_light_sensor_t* light_sensor);

/**
 * Frees up any resources allocated by the light sensor as part of \ref mtb_light_sensor_init().
 * @param[in] light_sensor  Light sensor instance.
 */
void mtb_light_sensor_free(mtb_light_sensor_t* light_sensor);

#if defined(__cplusplus)
}
#endif

/** \} group_board_libs */
