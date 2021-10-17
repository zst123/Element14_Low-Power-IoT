/***********************************************************************************************//**
 * \file mtb_light_sensor.c
 *
 * This file is the implementation for the light sensor.
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

#include "mtb_light_sensor.h"

#if defined(__cplusplus)
extern "C"
{
#endif


//--------------------------------------------------------------------------------------------------
// mtb_light_sensor_init
//
// Initializes an ADC channel to use with the light sensor. If no ADC
// channel is available (i.e. reserved by the hardware resource
// manager), then this initialization fails.
//
// NOTE: The ADC initialization function, cyhal_adc_init(), must be called
// before this function is called.
//--------------------------------------------------------------------------------------------------
cy_rslt_t mtb_light_sensor_init(mtb_light_sensor_t* light_sensor, cyhal_adc_t* adc_obj,
                                cyhal_gpio_t pin)
{
    cy_rslt_t result;

    result = cyhal_adc_channel_init(&light_sensor->channel, adc_obj, pin);

    return result;
}


//--------------------------------------------------------------------------------------------------
// mtb_light_sensor_light_level
//
// Returns the current light level as a percentage.
//--------------------------------------------------------------------------------------------------
uint8_t mtb_light_sensor_light_level(mtb_light_sensor_t* light_sensor)
{
    uint16_t adc_reading;
    uint32_t percentage;

    adc_reading = cyhal_adc_read_u16(&light_sensor->channel);

    //percentage = ((uint32_t)adc_reading * 100) / CYHAL_ADC_MAX_VALUE;
    // This is approximately the same as the line above and it doesn't
    // require a divide operation.
    percentage = (((uint32_t)adc_reading + 1) * 100) >> CYHAL_ADC_BITS;

    return (uint8_t)percentage;
}


//--------------------------------------------------------------------------------------------------
// mtb_light_sensor_free
//
// Free the resources used with the light sensor.
//--------------------------------------------------------------------------------------------------
void mtb_light_sensor_free(mtb_light_sensor_t* light_sensor)
{
    cyhal_adc_channel_free(&light_sensor->channel);
}


#if defined(__cplusplus)
}
#endif
