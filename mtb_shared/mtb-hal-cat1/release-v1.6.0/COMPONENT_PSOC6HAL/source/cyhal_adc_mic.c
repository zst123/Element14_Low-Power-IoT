/***************************************************************************/ /**
* \file cyhal_adc_mic.c
*
* \brief
* Provides a high level interface for interacting with the Cypress Analog/Digital
* convert. This interface abstracts out the chip specific details. If any chip
* specific functionality is necessary, or performance is critical the low level
* functions can be used directly.
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

/**
 * \addtogroup group_hal_impl_adcmic ADC (Analog Digital Converter)
 * \ingroup group_hal_impl
 * \{
 * \section cyhal_adcmic_impl_features Features
 * The CAT1B ADC supports the following features:
 * * Resolution: 16 bit
 * * Sample rate: Fixed 480 ksps
 * * Minimum acquisition time: Up to 2084 ns
 * * SW-based async transfer only (DMA is not supported)
 * * VREF: @ref CYHAL_ADC_REF_INTERNAL (1.8V) only
 * * Only @ref CYHAL_POWER_LEVEL_DEFAULT and @ref CYHAL_POWER_LEVEL_OFF are defined.
 * * Single ended vneg: @ref CYHAL_ADC_VNEG_VSSA
 *
 * The following functions are not supported:
 * * Differential channels
 * * Continuous scanning
 * * @ref CYHAL_ADC_EOS event (this is only applicable when continuously scanning)
 * * Averaging. In @ref cyhal_adc_config_t, average count must be 1 and average_mode_flags must be 0.
 *   In @ref cyhal_adc_channel_config_t, enable_averaging must be false.
 * * External vref and bypass pins
 * *
 * \} group_hal_impl_adcmic
 */

#include <string.h>
#include "cyhal_adc.h"
#include "cyhal_clock.h"
#include "cyhal_gpio.h"
#include "cyhal_hwmgr.h"
#include "cyhal_utils.h"
#include "cyhal_system.h"
#include <string.h>
#if defined(CY_IP_MXS40ADCMIC_INSTANCES)
#include "cy_adcmic.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define _CYHAL_ADCMIC_NUM_CHANNELS(obj) (sizeof(obj->channel_config) / sizeof(obj->channel_config[0]))
static const uint8_t  _CYHAL_ADCMIC_RESOLUTION          = 16u;
static const uint32_t _CYHAL_ADCMIC_TARGET_CLOCK_HZ     = 24000000u;
static const uint32_t _CYHAL_ADCMIC_SAMPLE_RATE_HZ      = 480000u; /* Fixed by the HW */
static const uint32_t _CYHAL_ADCMIC_ACQUISITION_TIME_NS = 2084u;   /* 1 / 480 ksps */

static const cy_stc_adcmic_dc_path_config_t _CYHAL_ADCMIC_DEFAULT_DC_CONFIG =
{
    .range = CY_ADCMIC_DC_RANGE_1_8V,
    .input = CY_ADCMIC_REFGND, /* Will be updated to match the actual input when a read is triggered */
    .tmrLatch = false, /* Don't wait for timer trigger to latch result */
};

static const cy_stc_adcmic_config_t _CYHAL_ADCMIC_DEFAULT_CONFIG =
{
    .source = CY_ADCMIC_DC,
    .sampleRate = CY_ADCMIC_480KSPS, /* This is the only sample rate supported for DC measurement */
    .anaConfig = NULL, /* Only applicable for the analog mic input path */
    .digConfig = NULL, /* Only applicable for the digital PDM input path */
    /* Safe to cast away const becuase this struct is itself stored in flash */
    .dcConfig = (cy_stc_adcmic_dc_path_config_t *)&_CYHAL_ADCMIC_DEFAULT_DC_CONFIG,
    .biQuadConfig = NULL, /* BiQuad isn't used for DC measurements */
    .fifoConfig = NULL, /* FIFO isn't used for DC measurements */
    .tmrTrgConfig = NULL, /* The timer and triggers aren't useful in DC measurement mode */
};

static MXS40ADCMIC_Type *const _cyhal_adcmic_base[] =
{
#if (CY_IP_MXS40ADCMIC_INSTANCES <= 1)
    MXS40ADCMIC0,
#else
    #warning Unhandled ADCMIC instance count
#endif
};

static const en_clk_dst_t _cyhal_adcmic_clock[] =
{
#if (CY_IP_MXS40ADCMIC_INSTANCES <= 1)
    PCLK_ADCMIC_CLK_HF,
#else
    #warning Unhandled ADCMIC instance count
#endif
};

static cyhal_adc_t* _cyhal_adcmic_config_structs[CY_IP_MXS40ADCMIC_INSTANCES];

static const IRQn_Type _cyhal_adcmic_irq_n[] =
{
#if (CY_IP_MXS40ADCMIC_INSTANCES <= 1)
    adcmic_interrupt_adcmic_IRQn,
#else
    #warning Unhandled ADCMIC instance count
#endif
};

static uint8_t _cyhal_adcmic_get_block_from_irqn(IRQn_Type irqn)
{
    switch (irqn)
    {
    #if (CY_IP_MXS40ADCMIC_INSTANCES <= 1)
        case adcmic_interrupt_adcmic_IRQn:
            return 0;
    #else
        #warning Unhandled ADCMIC instance count
    #endif
        default:
            CY_ASSERT(false); // Should never be called with a non-ADCMIC IRQn
            return 0;
    }
}

/* Find the next enabled channel, starting from current_idx and adjusting the buffer
 * along the way to account for disabled channels */
static void _cyhal_adcmic_find_next_channel(cyhal_adc_t* obj, uint8_t* current_idx, int32_t **buffer)
{
    uint8_t start_idx = *current_idx;
    do
    {
        if(NULL != obj->channel_config[*current_idx])
        {
            if(obj->channel_config[*current_idx]->enabled)
            {
                break;
            }
            else
            {
                ++(*buffer);
            }
            *current_idx = (*current_idx + 1) % _CYHAL_ADCMIC_NUM_CHANNELS(obj);
        }
    } while(*current_idx != start_idx); /* While we haven't wrapped completely around */
}

static void _cyhal_adcmic_irq_handler(void)
{
    /* The only interrupt we enable is DC conversion complete */
    IRQn_Type irqn = _CYHAL_UTILS_GET_CURRENT_IRQN();
    uint8_t block = _cyhal_adcmic_get_block_from_irqn(irqn);
    cyhal_adc_t* obj = _cyhal_adcmic_config_structs[block];
    Cy_ADCMic_ClearInterrupt(obj->base, CY_ADCMIC_INTR_DC);
    obj->conversion_complete = true;

    if(obj->async_scans_remaining > 0)
    {
        *(obj->async_buff_next) = Cy_ADCMic_GetDcResult(obj->base);
        uint8_t old_channel = obj->current_channel_index;
        _cyhal_adcmic_find_next_channel(obj, &(obj->current_channel_index), &(obj->async_buff_next));
        /* we know that at least one channel was enabled, so we don't have to worry about that case,
         * but we do need to check for rollover */
        if(obj->current_channel_index <= old_channel)
        {
            --(obj->async_scans_remaining);
            if(obj->async_scans_remaining == 0)
            {
                /* We're done, notify the user if they asked us to */
                obj->async_buff_next = NULL;
                Cy_ADCMic_DisableInterrupt(obj->base, CY_ADCMIC_INTR_DC);
                if(0 != (CYHAL_ADC_ASYNC_READ_COMPLETE & ((cyhal_adc_event_t)obj->user_enabled_events)))
                {
                    cyhal_adc_event_callback_t callback = (cyhal_adc_event_callback_t)obj->callback_data.callback;
                    if(NULL != callback)
                    {
                        callback(obj->callback_data.callback_arg, CYHAL_ADC_ASYNC_READ_COMPLETE);
                    }
                }
            }
        }
    }
}

static cy_en_adcmic_dc_channel_t _cyhal_adcmic_convert_channel_sel(uint8_t bit_index)
{
    static const cy_en_adcmic_dc_channel_t gpio_channel[] =
    {
        CY_ADCMIC_GPIO0,
        CY_ADCMIC_GPIO1,
        CY_ADCMIC_GPIO2,
        CY_ADCMIC_GPIO3,
        CY_ADCMIC_GPIO4,
        CY_ADCMIC_GPIO5,
        CY_ADCMIC_GPIO6,
        CY_ADCMIC_GPIO7,
        CY_ADCMIC_GPIO8,
        CY_ADCMIC_GPIO9,
        CY_ADCMIC_GPIO10,
        CY_ADCMIC_GPIO11,
        CY_ADCMIC_GPIO12,
        CY_ADCMIC_GPIO13,
        CY_ADCMIC_GPIO14,
        CY_ADCMIC_GPIO15,
        CY_ADCMIC_GPIO16,
        CY_ADCMIC_GPIO17,
        CY_ADCMIC_GPIO18,
        CY_ADCMIC_GPIO19,
        CY_ADCMIC_GPIO20,
        CY_ADCMIC_GPIO21,
        CY_ADCMIC_GPIO22,
        CY_ADCMIC_GPIO23,
        CY_ADCMIC_GPIO24,
        CY_ADCMIC_GPIO25,
        CY_ADCMIC_GPIO26,
        CY_ADCMIC_GPIO27
    };

    if (bit_index < sizeof(gpio_channel) / sizeof(gpio_channel[0]))
    {
        return gpio_channel[bit_index];
    }
    else
    {
        CY_ASSERT(false); /* We only support GPIO channels and the above defines all of them */
        return CY_ADCMIC_REFGND;
    }
}

/*******************************************************************************
*       ADC HAL Functions
*******************************************************************************/

cy_rslt_t cyhal_adc_init(cyhal_adc_t *obj, cyhal_gpio_t pin, const cyhal_clock_t *clk)
{
    CY_ASSERT(NULL != obj);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    memset(obj, 0, sizeof(cyhal_adc_t));
    obj->resource.type = CYHAL_RSC_INVALID;

    const cyhal_resource_pin_mapping_t* map = _cyhal_utils_try_alloc(pin, cyhal_pin_map_adcmic_gpio_adc_in, sizeof(cyhal_pin_map_adcmic_gpio_adc_in) / sizeof(cyhal_pin_map_adcmic_gpio_adc_in[0]));

    if (NULL == map)
        result = CYHAL_ADC_RSLT_BAD_ARGUMENT;

    cyhal_resource_inst_t adc_inst;
    if (CY_RSLT_SUCCESS == result)
    {
        adc_inst = *map->inst;
        /* No need to reserve - try_alloc did so for us already */
    }

    en_clk_dst_t pclk = (en_clk_dst_t)0;
    if (CY_RSLT_SUCCESS == result)
    {
        obj->resource = adc_inst;

        obj->base = _cyhal_adcmic_base[adc_inst.block_num];
        pclk = _cyhal_adcmic_clock[adc_inst.block_num];
        if (NULL != clk)
        {
            obj->clock = *clk;
            obj->dedicated_clock = false;
        }
        else if (CY_RSLT_SUCCESS ==
            (result = _cyhal_utils_allocate_clock(&(obj->clock), &(obj->resource), CYHAL_CLOCK_BLOCK_PERIPHERAL_16BIT, true)))
        {
            obj->dedicated_clock = true;
            result = cyhal_clock_set_enabled(&(obj->clock), true /* enabled */, false /* don't wait for lock. Not applicable to dividers */);
        }
    }

    if (CY_RSLT_SUCCESS == result && obj->clock.block != CYHAL_CLOCK_BLOCK_HF)
    {
        if (CY_SYSCLK_SUCCESS != _cyhal_utils_peri_pclk_assign_divider(pclk, &(obj->clock)))
            result = CYHAL_ADC_RSLT_FAILED_CLOCK;
    }

    if (CY_RSLT_SUCCESS == result)
    {
        if(obj->dedicated_clock)
        {
            if(CY_RSLT_SUCCESS != _cyhal_utils_set_clock_frequency(&(obj->clock), _CYHAL_ADCMIC_TARGET_CLOCK_HZ, NULL))
            {
               result = CYHAL_ADC_RSLT_FAILED_CLOCK;
            }
        }
    }

    if (result == CY_RSLT_SUCCESS)
    {
        result = (cy_rslt_t)Cy_ADCMic_Init(obj->base, &_CYHAL_ADCMIC_DEFAULT_CONFIG);
    }

    if (result == CY_RSLT_SUCCESS)
    {
        Cy_ADCMic_Enable(obj->base);
        _cyhal_adcmic_config_structs[obj->resource.block_num] = obj;
        cy_stc_sysint_t irqCfg = { _cyhal_adcmic_irq_n[obj->resource.block_num], CYHAL_ISR_PRIORITY_DEFAULT };
        Cy_SysInt_Init(&irqCfg, _cyhal_adcmic_irq_handler);
        NVIC_EnableIRQ(_cyhal_adcmic_irq_n[obj->resource.block_num]);
    }
    else
    {
        cyhal_adc_free(obj);
    }
    return result;
}

void cyhal_adc_free(cyhal_adc_t *obj)
{
    CY_ASSERT(NULL != obj);
    if (CYHAL_RSC_INVALID != obj->resource.type)
    {
        IRQn_Type irqn = _cyhal_adcmic_irq_n[obj->resource.block_num];
        NVIC_DisableIRQ(irqn);
        _cyhal_adcmic_config_structs[obj->resource.block_num] = NULL;

        if(obj->dedicated_clock)
        {
            _cyhal_utils_peri_pclk_disable_divider(_cyhal_adcmic_clock[obj->resource.block_num], &(obj->clock));
            cyhal_clock_free(&obj->clock);
        }

        if (NULL != obj->base)
        {
            Cy_ADCMic_Disable(obj->base);
            obj->base = NULL;
        }

        cyhal_hwmgr_free(&obj->resource);
    }
}

cy_rslt_t cyhal_adc_configure(cyhal_adc_t *obj, const cyhal_adc_config_t *config)
{
    /* The hardware is very limited, so all we can do is check that the config matches what we support */
    if((false != config->continuous_scanning)
        || (_CYHAL_ADCMIC_RESOLUTION != config->resolution)
        || (1u != config->average_count)
        || (0u != config->average_mode_flags)
        || (0u != config->ext_vref_mv)
        || (CYHAL_ADC_VNEG_VSSA != config->vneg)
        || (CYHAL_ADC_REF_INTERNAL != config->vref)
        || (NC != config->ext_vref)
        || (false != config->is_bypassed)
        || (NC != config->bypass_pin))
    {
        return CYHAL_ADC_RSLT_BAD_ARGUMENT;
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_adc_set_power(cyhal_adc_t *obj, cyhal_power_level_t power)
{
    // The ADC doesn't have selectable power levels in the same way that the opamps do.
    if(CYHAL_POWER_LEVEL_OFF == power)
    {
        Cy_ADCMic_Disable(obj->base);
    }
    else
    {
        Cy_ADCMic_Enable(obj->base);
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_adc_set_sample_rate(cyhal_adc_t* obj, uint32_t desired_sample_rate_hz, uint32_t* achieved_sample_rate_hz)
{
    /* Only one sample rate supported, so all we can do is validate */
    *achieved_sample_rate_hz = _CYHAL_ADCMIC_SAMPLE_RATE_HZ;
    return (_CYHAL_ADCMIC_SAMPLE_RATE_HZ == desired_sample_rate_hz) ? CY_RSLT_SUCCESS : CYHAL_ADC_RSLT_BAD_ARGUMENT;
}

cy_rslt_t cyhal_adc_channel_init_diff(cyhal_adc_channel_t *obj, cyhal_adc_t* adc, cyhal_gpio_t vplus, cyhal_gpio_t vminus, const cyhal_adc_channel_config_t* cfg)
{
    CY_ASSERT(obj != NULL);
    CY_ASSERT(adc != NULL);

    cy_rslt_t result = CY_RSLT_SUCCESS;

    memset(obj, 0, sizeof(cyhal_adc_channel_t));
    obj->vplus = NC;

    // Check for invalid pin or pin belonging to a different SAR
    const cyhal_resource_pin_mapping_t *vplus_map = _cyhal_utils_get_resource(vplus,
        cyhal_pin_map_adcmic_gpio_adc_in,
        sizeof(cyhal_pin_map_adcmic_gpio_adc_in)/sizeof(cyhal_pin_map_adcmic_gpio_adc_in[0]),
        &(adc->resource));

    if(NULL == vplus_map)
    {
        result = CYHAL_ADC_RSLT_BAD_ARGUMENT;
    }

    if(CY_RSLT_SUCCESS == result && CYHAL_ADC_VNEG != vminus)
    {
        result = CYHAL_ADC_RSLT_BAD_ARGUMENT; /* Only single-ended supported on this ADC HW */
    }

    if(CY_RSLT_SUCCESS == result)
    {
        /* For this block, we reuse the drive mode field to store the bit index on the adcmic.
         * Pull off and save that value, but provide the real required drive mode for connecting
         */
        uint8_t bit_index = vplus_map->drive_mode;
        obj->channel_sel = _cyhal_adcmic_convert_channel_sel(bit_index);
        cyhal_resource_pin_mapping_t mapping = *vplus_map;
        mapping.drive_mode = CY_GPIO_DM_ANALOG;
        result = _cyhal_utils_reserve_and_connect(vplus, &mapping);
    }

    uint8_t chosen_channel = 0;
    if (CY_RSLT_SUCCESS == result)
    {
        obj->vplus = vplus;
        // Find the first available channel
        for(chosen_channel = 0; chosen_channel < _CYHAL_ADCMIC_NUM_CHANNELS(obj->adc); ++chosen_channel)
        {
            if(NULL == adc->channel_config[chosen_channel])
            {
                break;
            }
        }
        if (chosen_channel >= _CYHAL_ADCMIC_NUM_CHANNELS(obj->adc))
            result = CYHAL_ADC_RSLT_NO_CHANNELS;
    }

    if(CY_RSLT_SUCCESS == result)
    {
        // Don't set the ADC until here so that free knows whether we have allocated
        // the channel on the parent ADC instance (and therefore doesn't try to free it if
        // something fails further up)
        obj->adc = adc;
        obj->channel_idx = chosen_channel;
        obj->adc->channel_config[chosen_channel] = obj;
    }

    if(CY_RSLT_SUCCESS == result)
    {
        result = cyhal_adc_channel_configure(obj, cfg);
    }

    if(CY_RSLT_SUCCESS != result)
    {
        cyhal_adc_channel_free(obj);
    }

    return result;
}

cy_rslt_t cyhal_adc_channel_configure(cyhal_adc_channel_t *obj, const cyhal_adc_channel_config_t *config)
{
    CY_ASSERT(NULL != obj);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    if((config->min_acquisition_ns > _CYHAL_ADCMIC_ACQUISITION_TIME_NS) || config->enable_averaging)
    {
        result = CYHAL_ADC_RSLT_BAD_ARGUMENT;
    }
    if(CY_RSLT_SUCCESS == result)
    {
        obj->enabled = config->enabled;
    }

    return result;
}

void cyhal_adc_channel_free(cyhal_adc_channel_t *obj)
{
    if(obj->adc != NULL)
    {
        // Disable the channel. No per-channel configuration was statically set, so nothing to unconfigure
        obj->adc->channel_config[obj->channel_idx] = NULL;
        obj->adc = NULL;
    }

    _cyhal_utils_release_if_used(&(obj->vplus));
}

uint16_t cyhal_adc_read_u16(const cyhal_adc_channel_t *obj)
{
    Cy_ADCMic_SelectDcChannel(obj->adc->base, obj->channel_sel);
    /* Cy_ADCMic_IsEndConversion relies on and clears the EOS interrupt status bit.
     * We don't know how this read will be used in combination with interrupts,
     * so implement our own interrupt-driven EOS flag.
     */
    Cy_ADCMic_ClearInterrupt(obj->adc->base, CY_ADCMIC_INTR_DC);
    Cy_ADCMic_EnableInterrupt(obj->adc->base, CY_ADCMIC_INTR_DC);
    obj->adc->conversion_complete = false;
    Cy_ADCMic_DisableInterrupt(obj->adc->base, CY_ADCMIC_INTR_DC);
    int retry = 1000;
    while(!obj->adc->conversion_complete && retry > 0)
    {
        cyhal_system_delay_us(1u); /* Conversion should take ~2 us */
        --retry;
    }

    uint16_t result = Cy_ADCMic_GetDcResult(obj->adc->base);
    return result;
}

int32_t cyhal_adc_read(const cyhal_adc_channel_t *obj)
{
    return (int32_t)cyhal_adc_read_u16(obj);
}

int32_t cyhal_adc_read_uv(const cyhal_adc_channel_t *obj)
{
    CY_ASSERT(NULL != obj);

    int32_t counts = cyhal_adc_read(obj);
    return Cy_ADCMic_CountsTo_uVolts(obj->adc->base, counts);
}

static void _cyhal_adcmic_start_async_read(cyhal_adc_t* obj, size_t num_scan, int32_t* result_list)
{
    CY_ASSERT(NULL == obj->async_buff_next); /* Transfer already in progress */
    uint32_t savedIntrStatus = cyhal_system_critical_section_enter();
    obj->current_channel_index = 0;
    obj->async_scans_remaining = num_scan;
    obj->async_buff_next = result_list;
    _cyhal_adcmic_find_next_channel(obj, &(obj->current_channel_index), &(obj->async_buff_next));
    if(NULL == obj->channel_config[obj->current_channel_index]
        || (false == obj->channel_config[obj->current_channel_index]->enabled))
    {
        /* No enabled channels found, we're done */
        obj->async_buff_next = NULL;
        obj->async_scans_remaining = 0u;
    }
    else
    {
        Cy_ADCMic_SelectDcChannel(obj->base, obj->channel_config[obj->current_channel_index]->channel_sel);
        Cy_ADCMic_ClearInterrupt(obj->base, CY_ADCMIC_INTR_DC);
        Cy_ADCMic_EnableInterrupt(obj->base, CY_ADCMIC_INTR_DC);
    }
    cyhal_system_critical_section_exit(savedIntrStatus);
}

cy_rslt_t cyhal_adc_read_async(cyhal_adc_t* obj, size_t num_scan, int32_t* result_list)
{
    CY_ASSERT(NULL != obj);
    obj->async_transfer_in_uv = false;
    _cyhal_adcmic_start_async_read(obj, num_scan, result_list);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_adc_read_async_uv(cyhal_adc_t* obj, size_t num_scan, int32_t* result_list)
{
    CY_ASSERT(NULL != obj);
    obj->async_transfer_in_uv = true;
    _cyhal_adcmic_start_async_read(obj, num_scan, result_list);
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cyhal_adc_set_async_mode(cyhal_adc_t *obj, cyhal_async_mode_t mode, uint8_t dma_priority)
{
    CY_ASSERT(NULL != obj);
    CY_ASSERT(NULL == obj->async_buff_next); /* Can't swap mode while a transfer is running */
    if(mode == CYHAL_ASYNC_DMA)
    {
        /* DMA not supported on this HW. CPU intervention is required after every sample anyway,
         * so triggering the DMA would involve more overhead than just CPU copying the 32 bits */
        return CYHAL_ADC_RSLT_BAD_ARGUMENT;
    }
    else
    {
        return CY_RSLT_SUCCESS;
    }
}

void cyhal_adc_register_callback(cyhal_adc_t *obj, cyhal_adc_event_callback_t callback, void *callback_arg)
{
    CY_ASSERT(NULL != obj);

    uint32_t savedIntrStatus = cyhal_system_critical_section_enter();
    obj->callback_data.callback = (cy_israddress) callback;
    obj->callback_data.callback_arg = callback_arg;
    cyhal_system_critical_section_exit(savedIntrStatus);
}

void cyhal_adc_enable_event(cyhal_adc_t *obj, cyhal_adc_event_t event, uint8_t intr_priority, bool enable)
{
    /* Continuous scanning isn't supported (the hardware is converting continuously, but it's not automatically
     * scanning across all enabled channels). We listen for EOC internally at times but that doesn't correspond to
     * the EOS event. So there's no interrupts that we need to enable/disable here. */
    if(enable)
    {
        obj->user_enabled_events |= event;
    }
    else
    {
        obj->user_enabled_events &= ~event;
    }

    IRQn_Type irqn = _cyhal_adcmic_irq_n[obj->resource.block_num];
    NVIC_SetPriority(irqn, intr_priority);
}

cy_rslt_t cyhal_adc_connect_digital(cyhal_adc_t *obj, cyhal_source_t source, cyhal_adc_input_t input)
{
    /* No trigger inputs supported on this hardware */
    return CYHAL_ADC_RSLT_BAD_ARGUMENT;
}

cy_rslt_t cyhal_adc_enable_output(cyhal_adc_t *obj, cyhal_adc_output_t output, cyhal_source_t *source)
{
    /* No trigger outputs supported on this hardware */
    return CYHAL_ADC_RSLT_BAD_ARGUMENT;
}

cy_rslt_t cyhal_adc_disconnect_digital(cyhal_adc_t *obj, cyhal_source_t source,  cyhal_adc_input_t input)
{
    /* No trigger inputs supported on this hardware */
    return CYHAL_ADC_RSLT_BAD_ARGUMENT;
}

cy_rslt_t cyhal_adc_disable_output(cyhal_adc_t *obj, cyhal_adc_output_t output)
{
    /* No trigger outputs supported on this hardware */
    return CYHAL_ADC_RSLT_BAD_ARGUMENT;
}

#if defined(__cplusplus)
}
#endif

#endif /* defined(CY_IP_MXS40PASS_SAR_INSTANCES) */
