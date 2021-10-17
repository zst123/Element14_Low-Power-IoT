/***************************************************************************//**
* \file cyhal_hw_resources.h
*
* \brief
* Provides struct definitions for configuration resources in the PDL.
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
* \addtogroup group_hal_impl_hw_types
* \ingroup group_hal_impl
* \{
*/

#pragma once

#include "cy_pdl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Documented in cyhal.h
#define CYHAL_DRIVER_AVAILABLE_ADC          (CY_IP_MXS40PASS_SAR || CY_IP_MXS40ADCMIC_INSTANCES)
#define CYHAL_DRIVER_AVAILABLE_CLOCK        (1)
#define CYHAL_DRIVER_AVAILABLE_COMP         (CY_IP_MXLPCOMP_INSTANCES || CY_IP_MXS40PASS_CTB_INSTANCES)
#define CYHAL_DRIVER_AVAILABLE_CRC          (CY_IP_MXCRYPTO_INSTANCES || CPUSS_CRYPTO_PRESENT)
#define CYHAL_DRIVER_AVAILABLE_DAC          (CY_IP_MXS40PASS_CTDAC)
#define CYHAL_DRIVER_AVAILABLE_DMA          (CY_IP_M4CPUSS_DMAC || CY_IP_M4CPUSS_DMA || CY_IP_MXAHBDMAC || CY_IP_MXDW)
#define CYHAL_DRIVER_AVAILABLE_EZI2C        (CY_IP_MXSCB)
#define CYHAL_DRIVER_AVAILABLE_FLASH        (1)
#define CYHAL_DRIVER_AVAILABLE_GPIO         (1)
#define CYHAL_DRIVER_AVAILABLE_HWMGR        (1)
#define CYHAL_DRIVER_AVAILABLE_I2C          (CY_IP_MXSCB)
#define CYHAL_DRIVER_AVAILABLE_I2S          (CY_IP_MXAUDIOSS || CY_IP_MXTDM)
#define CYHAL_DRIVER_AVAILABLE_INTERCONNECT (1)
#define CYHAL_DRIVER_AVAILABLE_KEYSCAN      (CY_IP_MXKEYSCAN)
#define CYHAL_DRIVER_AVAILABLE_LPTIMER      (1)
#define CYHAL_DRIVER_AVAILABLE_OPAMP        (CY_IP_MXS40PASS_CTB_INSTANCES)
#define CYHAL_DRIVER_AVAILABLE_PDMPCM       (CY_IP_MXAUDIOSS || CY_IP_MXPDM)
#define CYHAL_DRIVER_AVAILABLE_PWM          (CY_IP_MXTCPWM)
#define CYHAL_DRIVER_AVAILABLE_QSPI         (CY_IP_MXSMIF)
#define CYHAL_DRIVER_AVAILABLE_QUADDEC      (CY_IP_MXTCPWM)
#define CYHAL_DRIVER_AVAILABLE_RTC          (CY_IP_MXS40SRSS_RTC || CY_IP_MXS40SSRSS)
#define CYHAL_DRIVER_AVAILABLE_SDHC         (CY_IP_MXSDHC)
#define CYHAL_DRIVER_AVAILABLE_SDIO         (CY_IP_MXSDHC || CYHAL_UDB_SDIO)
#define CYHAL_DRIVER_AVAILABLE_SPI          (CY_IP_MXSCB)
#define CYHAL_DRIVER_AVAILABLE_SYSPM        (1)
#define CYHAL_DRIVER_AVAILABLE_SYSTEM       (1)
#define CYHAL_DRIVER_AVAILABLE_TDM          (CY_IP_MXAUDIOSS || CY_IP_MXTDM)
#define CYHAL_DRIVER_AVAILABLE_TIMER        (CY_IP_MXTCPWM)
#define CYHAL_DRIVER_AVAILABLE_TRNG         (CY_IP_MXCRYPTO_INSTANCES || CPUSS_CRYPTO_PRESENT)
#define CYHAL_DRIVER_AVAILABLE_UART         (CY_IP_MXSCB)
#define CYHAL_DRIVER_AVAILABLE_USB_DEV      (CY_IP_MXUSBFS)
#define CYHAL_DRIVER_AVAILABLE_WDT          (1)


/* NOTE: Any changes made to this enum must also be made to the hardware manager resource tracking */
/** Resource types that the hardware manager supports */
typedef enum
{
    CYHAL_RSC_ADC,       /*!< Analog to digital converter */
    CYHAL_RSC_ADCMIC,    /*!< Analog to digital converter with Analog Mic support */
    CYHAL_RSC_BLESS,     /*!< Bluetooth communications block */
    CYHAL_RSC_CAN,       /*!< CAN communication block */
    CYHAL_RSC_CLKPATH,   /*!< Clock Path. DEPRECATED. */
    CYHAL_RSC_CLOCK,     /*!< Clock */
    CYHAL_RSC_CRYPTO,    /*!< Crypto hardware accelerator */
    CYHAL_RSC_DAC,       /*!< Digital to analog converter */
    CYHAL_RSC_DMA,       /*!< DMA controller */
    CYHAL_RSC_DW,        /*!< Datawire DMA controller */
    CYHAL_RSC_ETH,       /*!< Ethernet communications block */
    CYHAL_RSC_GPIO,      /*!< General purpose I/O pin */
    CYHAL_RSC_I2S,       /*!< I2S communications block */
    CYHAL_RSC_I3C,       /*!< I3C communications block */
    CYHAL_RSC_KEYSCAN,   /*!< KeyScan block */
    CYHAL_RSC_LCD,       /*!< Segment LCD controller */
    CYHAL_RSC_LIN,       /*!< LIN communications block */
    CYHAL_RSC_LPCOMP,    /*!< Low power comparator */
    CYHAL_RSC_LPTIMER,   /*!< Low power timer */
    CYHAL_RSC_OPAMP,     /*!< Opamp */
    CYHAL_RSC_PDM,       /*!< PCM/PDM communications block */
    CYHAL_RSC_SMIF,      /*!< Quad-SPI communications block */
    CYHAL_RSC_RTC,       /*!< Real time clock */
    CYHAL_RSC_SCB,       /*!< Serial Communications Block */
    CYHAL_RSC_SDHC,      /*!< SD Host Controller */
    CYHAL_RSC_SDIODEV,   /*!< SDIO Device Block */
    CYHAL_RSC_TCPWM,     /*!< Timer/Counter/PWM block */
    CYHAL_RSC_TDM,       /*!< TDM block */
    CYHAL_RSC_UDB,       /*!< UDB Array */
    CYHAL_RSC_USB,       /*!< USB communication block */
    CYHAL_RSC_INVALID,   /*!< Placeholder for invalid type */
} cyhal_resource_t;

/** \cond INTERNAL */
    /* Extracts the divider from the Peri group block number */
    #define _CYHAL_PERIPHERAL_GROUP_GET_DIVIDER_TYPE(block)     ((cy_en_divider_types_t)(block & 0x03))
    #define _CYHAL_PERIPHERAL_GROUP_GET_GROUP(block)            (block >> 2)
#if defined(COMPONENT_CAT1B)
    /* Converts the group/div pair into a unique block number. */
    #define _CYHAL_PERIPHERAL_GROUP_ADJUST(group, div)        ((group << 2) | div)

    #define _CYHAL_CLOCK_BLOCK_PERI_GROUP(gr) \
        CYHAL_CLOCK_BLOCK_PERIPHERAL##gr##_8BIT = _CYHAL_PERIPHERAL_GROUP_ADJUST(gr, CY_SYSCLK_DIV_8_BIT),        /*!< 8bit Peripheral Divider Group 1 */ \
        CYHAL_CLOCK_BLOCK_PERIPHERAL##gr##_16BIT = _CYHAL_PERIPHERAL_GROUP_ADJUST(gr, CY_SYSCLK_DIV_16_BIT),      /*!< 16bit Peripheral Divider Group 1 */ \
        CYHAL_CLOCK_BLOCK_PERIPHERAL##gr##_16_5BIT = _CYHAL_PERIPHERAL_GROUP_ADJUST(gr, CY_SYSCLK_DIV_16_5_BIT),  /*!< 16.5bit Peripheral Divider Group 1 */ \
        CYHAL_CLOCK_BLOCK_PERIPHERAL##gr##_24_5BIT = _CYHAL_PERIPHERAL_GROUP_ADJUST(gr, CY_SYSCLK_DIV_24_5_BIT)   /*!< 24.5bit Peripheral Divider Group 1 */
#endif
/** \endcond */

/* NOTE: Any changes here must also be made in cyhal_hwmgr.c */
/** Enum for the different types of clocks that exist on the device. */
typedef enum
{
#if defined(COMPONENT_CAT1A)
    // The first four items are here for backwards compatability with old clock APIs
    CYHAL_CLOCK_BLOCK_PERIPHERAL_8BIT = CY_SYSCLK_DIV_8_BIT,        /*!< 8bit Peripheral Divider */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_16BIT = CY_SYSCLK_DIV_16_BIT,      /*!< 16bit Peripheral Divider */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_16_5BIT = CY_SYSCLK_DIV_16_5_BIT,  /*!< 16.5bit Peripheral Divider */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_24_5BIT = CY_SYSCLK_DIV_24_5_BIT,  /*!< 24.5bit Peripheral Divider */

    CYHAL_CLOCK_BLOCK_IMO,                                          /*!< Internal Main Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_ECO,                                          /*!< External Crystal Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_EXT,                                          /*!< External Input Clock */
    CYHAL_CLOCK_BLOCK_ALTHF,                                        /*!< Alternate High Frequency Input Clock */
    CYHAL_CLOCK_BLOCK_ALTLF,                                        /*!< Alternate Low Frequency Input Clock */
    CYHAL_CLOCK_BLOCK_ILO,                                          /*!< Internal Low Speed Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_PILO,                                         /*!< Precision ILO Input Clock */
    CYHAL_CLOCK_BLOCK_WCO,                                          /*!< Watch Crystal Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_MFO,                                          /*!< Medium Frequency Oscillator Clock */

    CYHAL_CLOCK_BLOCK_PATHMUX,                                      /*!< Path selection mux for input to FLL/PLLs */

    CYHAL_CLOCK_BLOCK_FLL,                                          /*!< Frequency-Locked Loop Clock */
    CYHAL_CLOCK_BLOCK_PLL,                                          /*!< Phase-Locked Loop Clock */

    CYHAL_CLOCK_BLOCK_LF,                                           /*!< Low Frequency Clock */
    CYHAL_CLOCK_BLOCK_MF,                                           /*!< Medium Frequency Clock */
    CYHAL_CLOCK_BLOCK_HF,                                           /*!< High Frequency Clock */

    CYHAL_CLOCK_BLOCK_PUMP,                                         /*!< Analog Pump Clock */
    CYHAL_CLOCK_BLOCK_BAK,                                          /*!< Backup Power Domain Clock */
    CYHAL_CLOCK_BLOCK_TIMER,                                        /*!< Timer Clock */
    CYHAL_CLOCK_BLOCK_ALT_SYS_TICK,                                 /*!< Alternative SysTick Clock */

    CYHAL_CLOCK_BLOCK_FAST,                                         /*!< Fast Clock for CM4 */
    CYHAL_CLOCK_BLOCK_PERI,                                         /*!< Peripheral Clock */
    CYHAL_CLOCK_BLOCK_SLOW,                                         /*!< Slow Clock for CM0+ */
#elif defined(COMPONENT_CAT1B)

    CYHAL_CLOCK_BLOCK_PERIPHERAL_8BIT = CY_SYSCLK_DIV_8_BIT,        /*!< Equivalent to CYHAL_CLOCK_BLOCK_PERIPHERAL0_8_BIT */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_16BIT = CY_SYSCLK_DIV_16_BIT,      /*!< Equivalent to CYHAL_CLOCK_BLOCK_PERIPHERAL0_16_BIT */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_16_5BIT = CY_SYSCLK_DIV_16_5_BIT,  /*!< Equivalent to CYHAL_CLOCK_BLOCK_PERIPHERAL0_16_5_BIT */
    CYHAL_CLOCK_BLOCK_PERIPHERAL_24_5BIT = CY_SYSCLK_DIV_24_5_BIT,  /*!< Equivalent to CYHAL_CLOCK_BLOCK_PERIPHERAL0_24_5_BIT */

    // The first four items are here for backwards compatability with old clock APIs
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 1)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(0),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 2)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(1),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 3)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(2),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 4)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(3),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 5)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(4),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 6)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(5),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 7)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(6),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 8)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(7),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 9)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(8),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 10)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(9),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 11)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(10),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 12)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(11),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 13)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(12),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 14)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(13),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 15)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(14),
    #endif
    #if (PERI_PERI_PCLK_PCLK_GROUP_NR >= 16)
    _CYHAL_CLOCK_BLOCK_PERI_GROUP(15),
    #endif

    CYHAL_CLOCK_BLOCK_IHO,                                          /*!< Internal High Speed Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_IMO,                                          /*!< Internal Main Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_ECO,                                          /*!< External Crystal Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_EXT,                                          /*!< External Input Clock */
    CYHAL_CLOCK_BLOCK_ALTHF,                                        /*!< Alternate High Frequency Input Clock */
    CYHAL_CLOCK_BLOCK_ALTLF,                                        /*!< Alternate Low Frequency Input Clock */
    CYHAL_CLOCK_BLOCK_ILO,                                          /*!< Internal Low Speed Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_PILO,                                         /*!< Precision ILO Input Clock */
    CYHAL_CLOCK_BLOCK_WCO,                                          /*!< Watch Crystal Oscillator Input Clock */
    CYHAL_CLOCK_BLOCK_MFO,                                          /*!< Medium Frequency Oscillator Clock */

    CYHAL_CLOCK_BLOCK_PATHMUX,                                      /*!< Path selection mux for input to FLL/PLLs */

    CYHAL_CLOCK_BLOCK_FLL,                                          /*!< Frequency-Locked Loop Clock */
    CYHAL_CLOCK_BLOCK_PLL200,                                       /*!< 200MHz Phase-Locked Loop Clock */
    CYHAL_CLOCK_BLOCK_PLL400,                                       /*!< 400MHz Phase-Locked Loop Clock */
    CYHAL_CLOCK_BLOCK_ECO_PRESCALER,                                /*!< ECO Prescaler Divider */

    CYHAL_CLOCK_BLOCK_LF,                                           /*!< Low Frequency Clock */
    CYHAL_CLOCK_BLOCK_MF,                                           /*!< Medium Frequency Clock */
    CYHAL_CLOCK_BLOCK_HF,                                           /*!< High Frequency Clock */

    CYHAL_CLOCK_BLOCK_PUMP,                                         /*!< Analog Pump Clock */
    CYHAL_CLOCK_BLOCK_BAK,                                          /*!< Backup Power Domain Clock */
    CYHAL_CLOCK_BLOCK_ALT_SYS_TICK,                                 /*!< Alternative SysTick Clock */
    CYHAL_CLOCK_BLOCK_PERI,                                         /*!< Peripheral Clock Group */
#endif
} cyhal_clock_block_t;

/** @brief Clock object
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases. */
typedef struct
{
    //For backwards compatibility with cyhal_clock_divider_t only. Do not use going forward.
    cy_en_divider_types_t   div_type;                /*!< Deprecated */
    uint8_t                 div_num;                 /*!< Deprecated */
    //End BWC items
    cyhal_clock_block_t     block;
    uint8_t                 channel;
    bool                    reserved;
} cyhal_clock_t;

/**
  * @brief Represents a particular instance of a resource on the chip.
  * Application code should not rely on the specific contents of this struct.
  * They are considered an implementation detail which is subject to change
  * between platforms and/or HAL releases.
  */
typedef struct
{
    cyhal_resource_t type;      //!< The resource block type
    uint8_t          block_num; //!< The resource block index
    /**
      * The channel number, if the resource type defines multiple channels
      * per block instance. Otherwise, 0 */
    uint8_t          channel_num;
} cyhal_resource_inst_t;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** \} group_hal_impl_hw_types */
