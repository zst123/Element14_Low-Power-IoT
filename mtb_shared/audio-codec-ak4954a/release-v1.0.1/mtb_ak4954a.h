/**************************************************************************//***
 *
 *
 * \file mtb_ak4954a.h
 *
 * Description: This file contains the function prototypes and constants used
 * in mtb_ak4954a.c. This driver is intended for the AK4954A audio codec.
 *
 * \note The AK4954A supports these I2C speed grades:
 *       * standard mode (100 kbit/s)
 *       * full speed (400 kbit/s)
 *
 *******************************************************************************
 * \copyright
 * Copyright 2018-2020 Cypress Semiconductor Corporation
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

#pragma once

/**
* \addtogroup group_board_libs Audio Codec
* \{
* Basic set of APIs for interacting with the AK4954A audio codec display.
* This provides basic initialization and access to to the audio codec.
*/

#include <stdint.h>

#include "cy_result.h"
#include "cyhal_i2c.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * Enumeration for the AK4954A I2C Interface Register Addresses
 *
 * The entries below marked as a "two-byte value" are intended for use with these functions:
 * * mtb_ak4954a_read_word()
 * * mtb_ak4954a_write_word()
 *
 */
typedef enum
{
    AK4954A_REG_PWR_MGMT1,                            /**< 0x00: Power Management 1 */
    AK4954A_REG_PWR_MGMT2,                            /**< 0x01: Power Management 2 */
    AK4954A_REG_SIG_SEL1,                             /**< 0x02: Signal Select 1 */
    AK4954A_REG_SIG_SEL2,                             /**< 0x03: Signal Select 2 */
    AK4954A_REG_SIG_SEL3,                             /**< 0x04: Signal Select 3 */
    AK4954A_REG_MODE_CTRL1,                           /**< 0x05: Mode Control 1 */
    AK4954A_REG_MODE_CTRL2,                           /**< 0x06: Mode Control 2 */
    AK4954A_REG_MODE_CTRL3,                           /**< 0x07: Mode Control 3 */
    AK4954A_REG_DIG_MIC,                              /**< 0x08: Digital MIC  0 */
    AK4954A_REG_TMR_SEL,                              /**< 0x09: Timer Select */
    AK4954A_REG_ALC_TMR_SEL,                          /**< 0x0A: ALC Timer Select */
    AK4954A_REG_ALC_MODE_CTRL1,                       /**< 0x0B: ALC Mode Control 1 */
    AK4954A_REG_ALC_MODE_CTRL2,                       /**< 0x0C: ALC Mode Control 2 */
    AK4954A_REG_LCH_INPUT_VOL_CTRL,                   /**< 0x0D: Lch Input Volume Control */
    AK4954A_REG_RCH_INPUT_VOL_CTRL,                   /**< 0x0E: Rch Input Volume Control */
    AK4954A_REG_RESERVED_0x0F,                        /*   0x0F: Reserved */
    AK4954A_REG_RESERVED_0x10,                        /*   0x10: Reserved */
    AK4954A_REG_RESERVED_0x11,                        /*   0x11: Reserved */
    AK4954A_REG_HP_OUTPUT_CTRL,                       /**< 0x12: HP Output Control */
    AK4954A_REG_LCH_DIG_VOL_CTRL,                     /**< 0x13: Lch Digital Volume Control */
    AK4954A_REG_RCH_DIG_VOL_CTRL,                     /**< 0x14: Rch Digital Volume Control */
    AK4954A_REG_BEEP_FREQ,                            /**< 0x15: BEEP Frequency */
    AK4954A_REG_BEEP_ON_TIME,                         /**< 0x16: BEEP ON Time */
    AK4954A_REG_BEEP_OFF_TIME,                        /**< 0x17: BEEP OFF Time */
    AK4954A_REG_BEEP_REPEAT_CNT,                      /**< 0x18: BEEP Repeat Count */
    AK4954A_REG_BEEP_VOL_CTRL,                        /**< 0x19: BEEP Volume Control */
    AK4954A_REG_RESERVED_0x1A,                        /*   0x1A: Reserved */
    AK4954A_REG_DIG_FLTR_SEL1,                        /**< 0x1B: Digital Filter Select 1 */
    AK4954A_REG_DIG_FLTR_SEL2,                        /**< 0x1C: Digital Filter Select 2 */
    AK4954A_REG_DIG_FLTR_MODE,                        /**< 0x1D: Digital Filter Mode */
    AK4954A_REG_HPF2_COEF0,                           /**< 0x1E: HPF2 Co-efficient 0 */
    AK4954A_REG_F1A = AK4954A_REG_HPF2_COEF0,         /**< 0x1E: F1A, two-byte value */
    AK4954A_REG_HPF2_COEF1,                           /**< 0x1F: HPF2 Co-efficient 1 */
    AK4954A_REG_HPF2_COEF2,                           /**< 0x20: HPF2 Co-efficient 2 */
    AK4954A_REG_F1B = AK4954A_REG_HPF2_COEF2,         /**< 0x20: F1B, two-byte value */
    AK4954A_REG_HPF2_COEF3,                           /**< 0x21: HPF2 Co-efficient 3 */
    AK4954A_REG_LPF_COEF0,                            /**< 0x22: LPF Co-efficient 0 */
    AK4954A_REG_F2A = AK4954A_REG_LPF_COEF0,          /**< 0x22: F2A, two-byte value */
    AK4954A_REG_LPF_COEF1,                            /**< 0x23: LPF Co-efficient 1 */
    AK4954A_REG_LPF_COEF2,                            /**< 0x24: LPF Co-efficient 2 */
    AK4954A_REG_F2B = AK4954A_REG_LPF_COEF2,          /**< 0x24: F2B, two-byte value */
    AK4954A_REG_LPF_COEF3,                            /**< 0x25: LPF Co-efficient 3 */
    AK4954A_REG_FIL3_COEF0,                           /**< 0x26: FIL3 Co-efficient 0 */
    AK4954A_REG_F3A = AK4954A_REG_FIL3_COEF0,         /**< 0x26: F3A, two-byte value */
    AK4954A_REG_FIL3_COEF1,                           /**< 0x27: FIL3 Co-efficient 1 */
    AK4954A_REG_FIL3_COEF2,                           /**< 0x28: FIL3 Co-efficient 2 */
    AK4954A_REG_F3B = AK4954A_REG_FIL3_COEF2,         /**< 0x28: F3B, two-byte value */
    AK4954A_REG_FIL3_COEF3,                           /**< 0x29: FIL3 Co-efficient 3 */
    AK4954A_REG_EQ_COEF0,                             /**< 0x2A: EQ Co-efficient 0 */
    AK4954A_REG_E0A = AK4954A_REG_EQ_COEF0,           /**< 0x2A: E0A, two-byte value */
    AK4954A_REG_EQ_COEF1,                             /**< 0x2B: EQ Co-efficient 1 */
    AK4954A_REG_EQ_COEF2,                             /**< 0x2C: EQ Co-efficient 2 */
    AK4954A_REG_E0B = AK4954A_REG_EQ_COEF2,           /**< 0x2C: E0B, two-byte value */
    AK4954A_REG_EQ_COEF3,                             /**< 0x2D: EQ Co-efficient 3 */
    AK4954A_REG_EQ_COEF4,                             /**< 0x2E: EQ Co-efficient 4 */
    AK4954A_REG_E0C = AK4954A_REG_EQ_COEF4,           /**< 0x2E: E0C, two-byte value */
    AK4954A_REG_EQ_COEF5,                             /**< 0x2F: EQ Co-efficient 5 */
    AK4954A_REG_DIG_FLTR_SEL3,                        /**< 0x30: Digital Filter Select 3 */
    AK4954A_REG_RESERVED_0x31,                        /*   0x31: Reserved */
    AK4954A_REG_E1_COEF0,                             /**< 0x32: E1 Co-efficient 0 */
    AK4954A_REG_E1A = AK4954A_REG_E1_COEF0,           /**< 0x32: E1A, two-byte value */
    AK4954A_REG_E1_COEF1,                             /**< 0x33: E1 Co-efficient 1 */
    AK4954A_REG_E1_COEF2,                             /**< 0x34: E1 Co-efficient 2 */
    AK4954A_REG_E1B = AK4954A_REG_E1_COEF2,           /**< 0x34: E1B, two-byte value */
    AK4954A_REG_E1_COEF3,                             /**< 0x35: E1 Co-efficient 3 */
    AK4954A_REG_E1_COEF4,                             /**< 0x36: E1 Co-efficient 4 */
    AK4954A_REG_E1C = AK4954A_REG_E1_COEF4,           /**< 0x36: E1C, two-byte value */
    AK4954A_REG_E1_COEF5,                             /**< 0x37: E1 Co-efficient 5 */
    AK4954A_REG_E2_COEF0,                             /**< 0x38: E2 Co-efficient 0 */
    AK4954A_REG_E2A = AK4954A_REG_E2_COEF0,           /**< 0x38: E2A, two-byte value */
    AK4954A_REG_E2_COEF1,                             /**< 0x39: E2 Co-efficient 1 */
    AK4954A_REG_E2_COEF2,                             /**< 0x3A: E2 Co-efficient 2 */
    AK4954A_REG_E2B = AK4954A_REG_E2_COEF2,           /**< 0x3A: E2B, two-byte value */
    AK4954A_REG_E2_COEF3,                             /**< 0x3B: E2 Co-efficient 3 */
    AK4954A_REG_E2_COEF4,                             /**< 0x3C: E2 Co-efficient 4 */
    AK4954A_REG_E2C = AK4954A_REG_E2_COEF4,           /**< 0x3C: E2C, two-byte value */
    AK4954A_REG_E2_COEF5,                             /**< 0x3D: E2 Co-efficient 5 */
    AK4954A_REG_E3_COEF0,                             /**< 0x3E: E3 Co-efficient 0 */
    AK4954A_REG_E3A = AK4954A_REG_E3_COEF0,           /**< 0x3E: E3A, two-byte value */
    AK4954A_REG_E3_COEF1,                             /**< 0x3F: E3 Co-efficient 1 */
    AK4954A_REG_E3_COEF2,                             /**< 0x40: E3 Co-efficient 2 */
    AK4954A_REG_E3B = AK4954A_REG_E3_COEF2,           /**< 0x40: E3B, two-byte value */
    AK4954A_REG_E3_COEF3,                             /**< 0x41: E3 Co-efficient 3 */
    AK4954A_REG_E3_COEF4,                             /**< 0x42: E3 Co-efficient 4 */
    AK4954A_REG_E3C = AK4954A_REG_E3_COEF4,           /**< 0x42: E3C, two-byte value */
    AK4954A_REG_E3_COEF5,                             /**< 0x43: E3 Co-efficient 5 */
    AK4954A_REG_E4_COEF0,                             /**< 0x44: E4 Co-efficient 0 */
    AK4954A_REG_E4A = AK4954A_REG_E4_COEF0,           /**< 0x44: E4A, two-byte value */
    AK4954A_REG_E4_COEF1,                             /**< 0x45: E4 Co-efficient 1 */
    AK4954A_REG_E4_COEF2,                             /**< 0x46: E4 Co-efficient 2 */
    AK4954A_REG_E4B = AK4954A_REG_E4_COEF2,           /**< 0x46: E4B, two-byte value */
    AK4954A_REG_E4_COEF3,                             /**< 0x47: E4 Co-efficient 3 */
    AK4954A_REG_E4_COEF4,                             /**< 0x48: E4 Co-efficient 4 */
    AK4954A_REG_E4C = AK4954A_REG_E4_COEF4,           /**< 0x48: E4C, two-byte value */
    AK4954A_REG_E4_COEF5,                             /**< 0x49: E4 Co-efficient 5 */
    AK4954A_REG_E5_COEF0,                             /**< 0x4A: E5 Co-efficient 0 */
    AK4954A_REG_E5A = AK4954A_REG_E5_COEF0,           /**< 0x4A: E5A, two-byte value */
    AK4954A_REG_E5_COEF1,                             /**< 0x4B: E5 Co-efficient 1 */
    AK4954A_REG_E5_COEF2,                             /**< 0x4C: E5 Co-efficient 2 */
    AK4954A_REG_E5B = AK4954A_REG_E5_COEF2,           /**< 0x4C: E5B, two-byte value */
    AK4954A_REG_E5_COEF3,                             /**< 0x4D: E5 Co-efficient 3 */
    AK4954A_REG_E5_COEF4,                             /**< 0x4E: E5 Co-efficient 4 */
    AK4954A_REG_E5C = AK4954A_REG_E5_COEF4,           /**< 0x4E: E5C, two-byte value */
    AK4954A_REG_E5_COEF5,                             /**< 0x4F: E5 Co-efficient 5 */
    AK4954A_REG_DRC_MODE_CTRL,                        /**< 0x50: DRC Mode Control */
    AK4954A_REG_NS_CTRL,                              /**< 0x51: NS Control */
    AK4954A_REG_NS_GAIN_AND_ATT_CTRL,                 /**< 0x52: NS Gain & ATT Control */
    AK4954A_REG_NS_ON_LEVEL,                          /**< 0x53: NS On Level */
    AK4954A_REG_NS_OFF_LEVEL,                         /**< 0x54: NS Off Level */
    AK4954A_REG_NS_REFERENCE_SEL,                     /**< 0x55: NS Reference Select */
    AK4954A_REG_NSREF = AK4954A_REG_NS_REFERENCE_SEL, /**< 0x55: NSREF */
    AK4954A_REG_NS_LPF_COEF0,                         /**< 0x56: NS LPF Co-efficient 0 */
    AK4954A_REG_NSLA = AK4954A_REG_NS_LPF_COEF0,      /**< 0x56: NSLA, two-byte value */
    AK4954A_REG_NS_LPF_COEF1,                         /**< 0x57: NS LPF Co-efficient 1 */
    AK4954A_REG_NS_LPF_COEF2,                         /**< 0x58: NS LPF Co-efficient 2 */
    AK4954A_REG_NSLB = AK4954A_REG_NS_LPF_COEF2,      /**< 0x58: NSLB, two-byte value */
    AK4954A_REG_NS_LPF_COEF3,                         /**< 0x59: NS LPF Co-efficient 3 */
    AK4954A_REG_NS_HPF_COEF0,                         /**< 0x5A: NS HPF Co-efficient 0 */
    AK4954A_REG_NSHA = AK4954A_REG_NS_HPF_COEF0,      /**< 0x5A: NSHA, two-byte value */
    AK4954A_REG_NS_HPF_COEF1,                         /**< 0x5B: NS HPF Co-efficient 1 */
    AK4954A_REG_NS_HPF_COEF2,                         /**< 0x5C: NS HPF Co-efficient 2 */
    AK4954A_REG_NSHB = AK4954A_REG_NS_HPF_COEF2,      /**< 0x5C: NSHB, two-byte value */
    AK4954A_REG_NS_HPF_COEF3,                         /**< 0x5D: NS HPF Co-efficient 3 */
    AK4954A_REG_RESERVED_0x5E,                        /*   0x5E: Reserved */
    AK4954A_REG_RESERVED_0x5F,                        /*   0x5F: Reserved */
    AK4954A_REG_DVLC_FLTR_SEL,                        /**< 0x60: DVLC Filter Select */
    AK4954A_REG_DVLC_MODE_CTRL,                       /**< 0x61: DVLC Mode Control */
    AK4954A_REG_DVLCL_CURVE_X1,                       /**< 0x62: DVLCL Curve X1 */
    AK4954A_REG_VL1X = AK4954A_REG_DVLCL_CURVE_X1,    /**< 0x62: VL1X */
    AK4954A_REG_DVLCL_CURVE_Y1,                       /**< 0x63: DVLCL Curve Y1 */
    AK4954A_REG_VL1Y = AK4954A_REG_DVLCL_CURVE_Y1,    /**< 0x63: VL1Y */
    AK4954A_REG_DVLCL_CURVE_X2,                       /**< 0x64: DVLCL Curve X2 */
    AK4954A_REG_VL2X = AK4954A_REG_DVLCL_CURVE_X2,    /**< 0x64: VL2X */
    AK4954A_REG_DVLCL_CURVE_Y2,                       /**< 0x65: DVLCL Curve Y2 */
    AK4954A_REG_VL2Y = AK4954A_REG_DVLCL_CURVE_Y2,    /**< 0x65: VL2Y */
    AK4954A_REG_DVLCL_CURVE_X3,                       /**< 0x66: DVLCL Curve X3 */
    AK4954A_REG_VL3X = AK4954A_REG_DVLCL_CURVE_X3,    /**< 0x66: VL3X */
    AK4954A_REG_DVLCL_CURVE_Y3,                       /**< 0x67: DVLCL Curve Y3 */
    AK4954A_REG_VL3Y = AK4954A_REG_DVLCL_CURVE_Y3,    /**< 0x67: VL3Y */
    AK4954A_REG_DVLCL_SLOPE1,                         /**< 0x68: DVLCL Slope 1 */
    AK4954A_REG_L1G = AK4954A_REG_DVLCL_SLOPE1,       /**< 0x68: L1G */
    AK4954A_REG_DVLCL_SLOPE2,                         /**< 0x69: DVLCL Slope 2 */
    AK4954A_REG_L2G = AK4954A_REG_DVLCL_SLOPE2,       /**< 0x69: L2G */
    AK4954A_REG_DVLCL_SLOPE3,                         /**< 0x6A: DVLCL Slope 3 */
    AK4954A_REG_L3G = AK4954A_REG_DVLCL_SLOPE3,       /**< 0x6A: L3G */
    AK4954A_REG_DVLCL_SLOPE4,                         /**< 0x6B: DVLCL Slope 4 */
    AK4954A_REG_L4G = AK4954A_REG_DVLCL_SLOPE4,       /**< 0x6B: L4G */
    AK4954A_REG_DVLCM_CURVE_X1,                       /**< 0x6C: DVLCM Curve X1 */
    AK4954A_REG_VM1X = AK4954A_REG_DVLCM_CURVE_X1,    /**< 0x6C: VM1X */
    AK4954A_REG_DVLCM_CURVE_Y1,                       /**< 0x6D: DVLCM Curve Y1 */
    AK4954A_REG_VM1Y = AK4954A_REG_DVLCM_CURVE_Y1,    /**< 0x6D: VM1Y */
    AK4954A_REG_DVLCM_CURVE_X2,                       /**< 0x6E: DVLCM Curve X2 */
    AK4954A_REG_VM2X = AK4954A_REG_DVLCM_CURVE_X2,    /**< 0x6E: VM2X */
    AK4954A_REG_DVLCM_CURVE_Y2,                       /**< 0x6F: DVLCM Curve Y2 */
    AK4954A_REG_VM2Y = AK4954A_REG_DVLCM_CURVE_Y2,    /**< 0x6F: VM2Y */
    AK4954A_REG_DVLCM_CURVE_X3,                       /**< 0x70: DVLCM Curve X3 */
    AK4954A_REG_VM3X = AK4954A_REG_DVLCM_CURVE_X3,    /**< 0x70: VM3X */
    AK4954A_REG_DVLCM_CURVE_Y3,                       /**< 0x71: DVLCM Curve Y3 */
    AK4954A_REG_VM3Y = AK4954A_REG_DVLCM_CURVE_Y3,    /**< 0x71: VM3Y */
    AK4954A_REG_DVLCM_SLOPE1,                         /**< 0x72: DVLCM Slope 1 */
    AK4954A_REG_M1G = AK4954A_REG_DVLCM_SLOPE1,       /**< 0x72: M1G */
    AK4954A_REG_DVLCM_SLOPE2,                         /**< 0x73: DVLCM Slope 2 */
    AK4954A_REG_M2G = AK4954A_REG_DVLCM_SLOPE2,       /**< 0x73: M2G */
    AK4954A_REG_DVLCM_SLOPE3,                         /**< 0x74: DVLCM Slope 3 */
    AK4954A_REG_M3G = AK4954A_REG_DVLCM_SLOPE3,       /**< 0x74: M3G */
    AK4954A_REG_DVLCM_SLOPE4,                         /**< 0x75: DVLCM Slope 4 */
    AK4954A_REG_M4G = AK4954A_REG_DVLCM_SLOPE4,       /**< 0x75: M4G */
    AK4954A_REG_DVLCH_CURVE_X1,                       /**< 0x76: DVLCH Curve X1 */
    AK4954A_REG_VH1X = AK4954A_REG_DVLCH_CURVE_X1,    /**< 0x76: VH1X */
    AK4954A_REG_DVLCH_CURVE_Y1,                       /**< 0x77: DVLCH Curve Y1 */
    AK4954A_REG_VH1Y = AK4954A_REG_DVLCH_CURVE_Y1,    /**< 0x77: VH1Y */
    AK4954A_REG_DVLCH_CURVE_X2,                       /**< 0x78: DVLCH Curve X2 */
    AK4954A_REG_VH2X = AK4954A_REG_DVLCH_CURVE_X2,    /**< 0x78: VH2X */
    AK4954A_REG_DVLCH_CURVE_Y2,                       /**< 0x79: DVLCH Curve Y2 */
    AK4954A_REG_VH2Y = AK4954A_REG_DVLCH_CURVE_Y2,    /**< 0x79: VH2Y */
    AK4954A_REG_DVLCH_CURVE_X3,                       /**< 0x7A: DVLCH Curve X3 */
    AK4954A_REG_VH3X = AK4954A_REG_DVLCH_CURVE_X3,    /**< 0x7A: VH3X */
    AK4954A_REG_DVLCH_CURVE_Y3,                       /**< 0x7B: DVLCH Curve Y3 */
    AK4954A_REG_VH3Y = AK4954A_REG_DVLCH_CURVE_Y3,    /**< 0x7B: VH3Y */
    AK4954A_REG_DVLCH_SLOPE1,                         /**< 0x7C: DVLCH Slope 1 */
    AK4954A_REG_H1G = AK4954A_REG_DVLCH_SLOPE1,       /**< 0x7C: H1G */
    AK4954A_REG_DVLCH_SLOPE2,                         /**< 0x7D: DVLCH Slope 2 */
    AK4954A_REG_H2G = AK4954A_REG_DVLCH_SLOPE2,       /**< 0x7D: H2G */
    AK4954A_REG_DVLCH_SLOPE3,                         /**< 0x7E: DVLCH Slope 3 */
    AK4954A_REG_H3G = AK4954A_REG_DVLCH_SLOPE3,       /**< 0x7E: H3G */
    AK4954A_REG_DVLCH_SLOPE4,                         /**< 0x7F: DVLCH Slope 4 */
    AK4954A_REG_H4G = AK4954A_REG_DVLCH_SLOPE4,       /**< 0x7F: H4G */
    AK4954A_REG_DVLCL_LPF_COEF0,                      /**< 0x80: DVLCL LPF Co-efficient 0 */
    AK4954A_REG_DLLA = AK4954A_REG_DVLCL_LPF_COEF0,   /**< 0x80: DLLA, two-byte value */
    AK4954A_REG_DVLCL_LPF_COEF1,                      /**< 0x81: DVLCL LPF Co-efficient 1 */
    AK4954A_REG_DVLCL_LPF_COEF2,                      /**< 0x82: DVLCL LPF Co-efficient 2 */
    AK4954A_REG_DLLB = AK4954A_REG_DVLCL_LPF_COEF2,   /**< 0x82: DLLB, two-byte value */
    AK4954A_REG_DVLCL_LPF_COEF3,                      /**< 0x83: DVLCL LPF Co-efficient 3 */
    AK4954A_REG_DVLCM_HPF_COEF0,                      /**< 0x84: DVLCM HPF Co-efficient 0 */
    AK4954A_REG_DMHA = AK4954A_REG_DVLCM_HPF_COEF0,   /**< 0x84: DMHA, two-byte value */
    AK4954A_REG_DVLCM_HPF_COEF1,                      /**< 0x85: DVLCM HPF Co-efficient 1 */
    AK4954A_REG_DVLCM_HPF_COEF2,                      /**< 0x86: DVLCM HPF Co-efficient 2 */
    AK4954A_REG_DMHB = AK4954A_REG_DVLCM_HPF_COEF2,   /**< 0x86: DMHB, two-byte value */
    AK4954A_REG_DVLCM_HPF_COEF3,                      /**< 0x87: DVLCM HPF Co-efficient 3 */
    AK4954A_REG_DVLCM_LPF_COEF0,                      /**< 0x88: DVLCM LPF Co-efficient 0 */
    AK4954A_REG_DMLA = AK4954A_REG_DVLCM_LPF_COEF0,   /**< 0x88: DMLA, two-byte value */
    AK4954A_REG_DVLCM_LPF_COEF1,                      /**< 0x89: DVLCM LPF Co-efficient 1 */
    AK4954A_REG_DVLCM_LPF_COEF2,                      /**< 0x8A: DVLCM LPF Co-efficient 2 */
    AK4954A_REG_DMLB = AK4954A_REG_DVLCM_LPF_COEF2,   /**< 0x8A: DMLB, two-byte value */
    AK4954A_REG_DVLCM_LPF_COEF3,                      /**< 0x8B: DVLCM LPF Co-efficient 3 */
    AK4954A_REG_DVLCH_HPF_COEF0,                      /**< 0x8C: DVLCH HPF Co-efficient 0 */
    AK4954A_REG_DHHA = AK4954A_REG_DVLCH_HPF_COEF0,   /**< 0x8C: DHHA, two-byte value */
    AK4954A_REG_DVLCH_HPF_COEF1,                      /**< 0x8D: DVLCH HPF Co-efficient 1 */
    AK4954A_REG_DVLCH_HPF_COEF2,                      /**< 0x8E: DVLCH HPF Co-efficient 2 */
    AK4954A_REG_DHHB = AK4954A_REG_DVLCH_HPF_COEF2,   /**< 0x8E: DHHB, two-byte value */
    AK4954A_REG_DVLCH_HPF_COEF3,                      /**< 0x8F: DVLCH HPF Co-efficient 3 */
    AK4954A_REG_MAX_ADDRESS = AK4954A_REG_DVLCH_HPF_COEF3
} mtb_ak4954a_reg_t;

/**
* \{
* @name AK4954A_REG_PWR_MGMT1 
* Bit definitions for the AK4954A_REG_PWR_MGMT1 register
*/
#define AK4954A_PWR_MGMT1_PMADL              (0x01) /**< Microphone Amplifier Lch and ADC Lch
                                                         Power Management */
#define AK4954A_PWR_MGMT1_PMADR              (0x02) /**< Microphone Amplifier Rch and ADC Rch
                                                         Power Management */
#define AK4954A_PWR_MGMT1_PMDAC              (0x04) /**< DAC Power Management */
#define AK4954A_PWR_MGMT1_LSV                (0x08) /**< Low Voltage Operation Mode of the
                                                         Speaker Amplifier */
#define AK4954A_PWR_MGMT1_PMBP               (0x20) /**< BEEP Generating Circuit Power Management */
#define AK4954A_PWR_MGMT1_PMVCM              (0x40) /**< VCOM Power Management */
#define AK4954A_PWR_MGMT1_PMPFIL             (0x80) /**< Programmable Filter Block Power
                                                         Management */
/** \} AK4954A_REG_PWR_MGMT1 */

/**
* \{
* @name AK4954A_REG_PWR_MGMT2 
* Bit definitions for the AK4954A_REG_PWR_MGMT2 register
*/
#define AK4954A_PWR_MGMT2_LOSEL              (0x01) /**< Stereo Line Output Select */
#define AK4954A_PWR_MGMT2_PMSL               (0x02) /**< Speaker Amp or Stereo Line Output
                                                         Power Management */
#define AK4954A_PWR_MGMT2_PMPLL              (0x04) /**< PLL Power Management */
#define AK4954A_PWR_MGMT2_MS                 (0x08) /**< Master/Slave Mode Select */
#define AK4954A_PWR_MGMT2_PMHPL              (0x10) /**< Lch Headphone Amplifier & Charge Pump
                                                         Power Management */
#define AK4954A_PWR_MGMT2_PMHPR              (0x20) /**< Rch Headphone Amplifier & Charge Pump
                                                         Power Management */
/** \} AK4954A_REG_PWR_MGMT2 */

/**
* \{
* @name AK4954A_REG_SIG_SEL1 
* Bit definitions for the AK4954A_REG_SIG_SEL1 register
*/
#define AK4954A_SIG_SEL1_PMMP                (0x08) /**< MPWR pin Power Management */
#define AK4954A_SIG_SEL1_MPSEL               (0x10) /**< MPWR Output Select */
#define AK4954A_SIG_SEL1_DACSL               (0x20) /**< Signal Switch Control from DAC to Speaker
                                                         or Stereo Line Amplifier */
#define AK4954A_SIG_SEL1_SLPSN               (0x80) /**< Speaker or Stereo Line Amplifier Power-Save
                                                         Mode */
/** \} AK4954A_REG_SIG_SEL1 */
/**
* \{
* @name AK4954A_REG_SIG_SEL1 (MGAIN)
* Bit field in the AK4954A_REG_SIG_SEL1 register\n
* Microphone Amplifier Gain Control
*/
#define AK4954A_SIG_SEL1_MGAIN_0dB           (0x04)
#define AK4954A_SIG_SEL1_MGAIN_6dB           (0x00)
#define AK4954A_SIG_SEL1_MGAIN_13dB          (0x01)
#define AK4954A_SIG_SEL1_MGAIN_20dB          (0x02)
#define AK4954A_SIG_SEL1_MGAIN_26dB          (0x03)
/** \} AK4954A_REG_SIG_SEL1 (MGAIN) */

/**
* \{
* @name AK4954A_REG_SIG_SEL2 (RIN)
* Bit field in the AK4954A_REG_SIG_SEL2 register\n
* ADC Rch Input Source Select
*/
#define AK4954A_SIG_SEL2_RIN1                (0x00)
#define AK4954A_SIG_SEL2_RIN2                (0x01)
#define AK4954A_SIG_SEL2_RIN3                (0x02)
/** \} AK4954A_REG_SIG_SEL2 (RIN) */
/**
* \{
* @name AK4954A_REG_SIG_SEL2 (LIN)
* Bit field in the AK4954A_REG_SIG_SEL2 register\n
* ADC Lch Input Source Select
*/
#define AK4954A_SIG_SEL2_LIN1                (0x00)
#define AK4954A_SIG_SEL2_LIN2                (0x04)
#define AK4954A_SIG_SEL2_LIN3                (0x08)
/** \} AK4954A_REG_SIG_SEL2 (LIN) */
/**
* \{
* @name AK4954A_REG_SIG_SEL2 (SLG)
* Bit field in the AK4954A_REG_SIG_SEL2 register\n
* Stereo Line Amplifier Output Gain
*/
#define AK4954A_SIG_SEL2_SLG_0db             (0x00)
#define AK4954A_SIG_SEL2_SLG_2db             (0x40)
#define AK4954A_SIG_SEL2_SLG_4db             (0x80)
#define AK4954A_SIG_SEL2_SLG_6db             (0xC0)
/** \} AK4954A_REG_SIG_SEL2 (SLG) */

/**
* \{
* @name AK4954A_REG_SIG_SEL3 
* Bit definitions for the AK4954A_REG_SIG_SEL3 register
*/
#define AK4954A_SIG_SEL3_MONO                (0x01) /**< Monaural mixing setting of the DAC
                                                         output */
#define AK4954A_SIG_SEL3_MOFF                (0x02) /**< Soft Transition Control of
                                                         "BEEP->Headphone" Connection ON/OFF */
/** \} AK4954A_REG_SIG_SEL3 */
/**
* \{
* @name AK4954A_REG_SIG_SEL3 (PTS)
* Bit field in the AK4954A_REG_SIG_SEL3 register\n
* Soft Transition Time of "BEEP->Headphone" Connection ON/OFF
*/
#define AK4954A_SIG_SEL3_PTS_1X              (0x00)
#define AK4954A_SIG_SEL3_PTS_2X              (0x04)
#define AK4954A_SIG_SEL3_PTS_4X              (0x08)
#define AK4954A_SIG_SEL3_PTS_8X              (0x0C)
/** \} AK4954A_REG_SIG_SEL3 (PTS) */

/**
* \{
* @name AK4954A_REG_MODE_CTRL1 (DIF)
* Bit field in the AK4954A_REG_MODE_CTRL1 register\n
* Audio Interface Format
*/
#define AK4954A_MODE_CTRL1_DIF_24M_24L       (0x00) /* 24-bit MSB / 24-bit LSB */
#define AK4954A_MODE_CTRL1_DIF_24M_16L       (0x01) /* 24-bit MSB / 16-bit LSB */
#define AK4954A_MODE_CTRL1_DIF_24M_24M       (0x02) /* 24-bit MSB / 24-bit MSB */
#define AK4954A_MODE_CTRL1_DIF_24_16_I2S     (0x03) /* 24/16-bit I2S Compatible */
#define AK4954A_MODE_CTRL1_DIF_32M_32M       (0x06) /* 32-bit MSB / 32-bit MSB */
#define AK4954A_MODE_CTRL1_DIF_32_I2S        (0x07) /* 32-bit I2S Compatible */
/** \} AK4954A_REG_MODE_CTRL1 (DIF) */
/**
* \{
* @name AK4954A_REG_MODE_CTRL1 (BCKO)
* Bit field in the AK4954A_REG_MODE_CTRL1 register\n
* BICK Output Frequency
*/
#define AK4954A_MODE_CTRL1_BCK0_32fs         (0x00)
#define AK4954A_MODE_CTRL1_BCK0_64fs         (0x08)
/** \} AK4954A_REG_MODE_CTRL1 (BCKO) */
/**
* \{
* @name AK4954A_REG_MODE_CTRL1 (PLL)
* Bit field in the AK4954A_REG_MODE_CTRL1 register\n
* PLL Reference Clock Select
*/
#define AK4954A_MODE_CTRL1_PLL_32fs          (0x00) /* BICK pin, 32 fs */
#define AK4954A_MODE_CTRL1_PLL_64fs          (0x10) /* BICK pin, 64 fs */
#define AK4954A_MODE_CTRL1_PLL_11p2986MHz    (0x20) /* MCKI pin, 11.2896 MHz */
#define AK4954A_MODE_CTRL1_PLL_12p288MHz     (0x30) /* MCKI pin,  12.288 MHz */
#define AK4954A_MODE_CTRL1_PLL_12MHz         (0x40) /* MCKI pin,      12 MHz */
#define AK4954A_MODE_CTRL1_PLL_24MHz         (0x50) /* MCKI pin,      24 MHz */
#define AK4954A_MODE_CTRL1_PLL_13p5MHz       (0x60) /* MCKI pin,    13.5 MHz */
#define AK4954A_MODE_CTRL1_PLL_27MHz         (0x70) /* MCKI pin,      27 MHz */
/** \} AK4954A_REG_MODE_CTRL1 (PLL) */

/**
* \{
* @name AK4954A_REG_MODE_CTRL2 (FS)
* Bit field in the AK4954A_REG_MODE_CTRL2 register\n
* Sampling frequency
*/
#define AK4954A_MODE_CTRL2_FS_8kHz           (0x00) /*      8 kHz */
#define AK4954A_MODE_CTRL2_FS_11p025kHz      (0x01) /* 11.025 kHz */
#define AK4954A_MODE_CTRL2_FS_12kHz          (0x02) /*     12 kHz */
#define AK4954A_MODE_CTRL2_FS_16kHz          (0x04) /*     16 kHz */
#define AK4954A_MODE_CTRL2_FS_22p05kHz       (0x05) /*  22.05 kHz */
#define AK4954A_MODE_CTRL2_FS_24kHz          (0x06) /*     24 kHz */
#define AK4954A_MODE_CTRL2_FS_32kHz          (0x08) /*     32 kHz */
#define AK4954A_MODE_CTRL2_FS_44p1kHz        (0x09) /*   44.1 kHz */
#define AK4954A_MODE_CTRL2_FS_48kHz          (0x0A) /*     48 kHz */
#define AK4954A_MODE_CTRL2_FS_64kHz          (0x0C) /*     64 kHz */
#define AK4954A_MODE_CTRL2_FS_88p2kHz        (0x0D) /*   88.2 kHz */
#define AK4954A_MODE_CTRL2_FS_96kHz          (0x0E) /*     96 kHz */
/** \} AK4954A_REG_MODE_CTRL2 (FS) */
/**
* \{
* @name AK4954A_REG_MODE_CTRL2 (CM)
* Bit field in the AK4954A_REG_MODE_CTRL2 register\n
* MCKI Input Frequency Setting in EXT mode
*/
#define AK4954A_MODE_CTRL2_CM_256fs          (0x00)
#define AK4954A_MODE_CTRL2_CM_384fs          (0x40)
#define AK4954A_MODE_CTRL2_CM_512fs          (0x80)
#define AK4954A_MODE_CTRL2_CM_1024fs         (0xC0)
/** \} AK4954A_REG_MODE_CTRL2 (CM) */

/**
* \{
* @name AK4954A_REG_MODE_CTRL3 
* Bit definitions for the AK4954A_REG_MODE_CTRL3 register
*/
#define AK4954A_MODE_CTRL3_LPDA              (0x01) /**< Low-Power Consumption Mode of DAC + HP */
#define AK4954A_MODE_CTRL3_LPMIC             (0x02) /**< Low-Power Consumption Mode of Microphone
                                                         Amplifier */
#define AK4954A_MODE_CTRL3_IVOLC             (0x04) /**< Input Digital Volume Control Mode Select */
#define AK4954A_MODE_CTRL3_DVOLC             (0x10) /**< Output Digital Volume 2 Control Mode
                                                         Select */
#define AK4954A_MODE_CTRL3_SMUTE             (0x20) /**< Soft Mute Control */
#define AK4954A_MODE_CTRL3_THDET             (0x40) /**< Thermal Shutdown Detection Result */
#define AK4954A_MODE_CTRL3_OVFL              (0x80) /**< ADC Overflow Output Enable (OVF Pin) */
/** \} AK4954A_REG_MODE_CTRL3 */

/**
* \{
* @name AK4954A_REG_DIG_MIC 
* Bit definitions for the AK4954A_REG_DIG_MIC register
*/
#define AK4954A_DIG_MIC_DMIC                 (0x01) /**< Digital Microphone Connection Select */
#define AK4954A_DIG_MIC_DCLKP                (0x02) /**< Data Latching Edge Select */
#define AK4954A_DIG_MIC_DCLKE                (0x08) /**< DMCLK pin Output Clock Control */
#define AK4954A_DIG_MIC_TEST                 (0x40) /**< Device TEST mode Enable. */
/** \} AK4954A_REG_DIG_MIC */
/**
* \{
* @name AK4954A_REG_DIG_MIC (PMDM)
* Bit field in the AK4954A_REG_DIG_MIC register\n
* Digital Microphone Input Signal Select
*/
#define AK4954A_DIG_MIC_PMDM_OFF             (0x00)
#define AK4954A_DIG_MIC_PMDM_RIGHT           (0x10)
#define AK4954A_DIG_MIC_PMDM_LEFT            (0x20)
#define AK4954A_DIG_MIC_PMDM_STEREO          (0x30)
/** \} AK4954A_REG_DIG_MIC (PMDM) */

/**
* \{
* @name AK4954A_REG_TMR_SEL (DVTM)
* Bit field in the AK4954A_REG_TMR_SEL register\n
* Output Digital Volume Soft Transition Time
*/
#define AK4954A_TMR_SEL_DVTM_144fs           (0x00)
#define AK4954A_TMR_SEL_DVTM_288fs           (0x01)
#define AK4954A_TMR_SEL_DVTM_576fs           (0x02)
/** \} AK4954A_REG_TMR_SEL (DVTM) */
/**
* \{
* @name AK4954A_REG_TMR_SEL (OVTM)
* Bit field in the AK4954A_REG_TMR_SEL register\n
* ADC Overflow Output Hold Time
*/
#define AK4954A_TMR_SEL_OVTM_16fs            (0x00)
#define AK4954A_TMR_SEL_OVTM_64fs            (0x04)
#define AK4954A_TMR_SEL_OVTM_128fs           (0x08)
#define AK4954A_TMR_SEL_OVTM_256fs           (0x0C)
/** \} AK4954A_REG_TMR_SEL (OVTM) */
/**
* \{
* @name AK4954A_REG_TMR_SEL (ADRST)
* Bit field in the AK4954A_REG_TMR_SEL register\n
* ADC Initialization Cycle Setting
*/
#define AK4954A_TMR_SEL_ADRST_2115fs         (0x00)
#define AK4954A_TMR_SEL_ADRST_4227fs         (0x40)
#define AK4954A_TMR_SEL_ADRST_267fs          (0x80)
#define AK4954A_TMR_SEL_ADRST_1059fs         (0xC0)
/** \} AK4954A_REG_TMR_SEL (ADRST) */

/**
* \{
* @name AK4954A_REG_ALC_TMR_SEL (RFST)
* Bit field in the AK4954A_REG_ALC_TMR_SEL register\n
* ALC First recovery Speed
*/
#define AK4954A_ALC_TMR_SEL_RFST_0p0032db    (0x00)
#define AK4954A_ALC_TMR_SEL_RFST_0p0042db    (0x01)
#define AK4954A_ALC_TMR_SEL_RFST_0p0064db    (0x02)
#define AK4954A_ALC_TMR_SEL_RFST_0p0127db    (0x03)
/** \} AK4954A_REG_ALC_TMR_SEL (RFST) */
/**
* \{
* @name AK4954A_REG_ALC_TMR_SEL (WTM)
* Bit field in the AK4954A_REG_ALC_TMR_SEL register\n
* ALC Recovery Waiting Period
*/
#define AK4954A_ALC_TMR_SEL_WTM_128fs        (0x00)
#define AK4954A_ALC_TMR_SEL_WTM_256fs        (0x04)
#define AK4954A_ALC_TMR_SEL_WTM_512fs        (0x08)
#define AK4954A_ALC_TMR_SEL_WTM_1024fs       (0x0C)
/** \} AK4954A_REG_ALC_TMR_SEL (WTM) */
/**
* \{
* @name AK4954A_REG_ALC_TMR_SEL (IVTM)
* Bit field in the AK4954A_REG_ALC_TMR_SEL register\n
* Input Digital Volume Soft Transition Time
*/
#define AK4954A_ALC_TMR_SEL_IVTM_236fs       (0x00)
#define AK4954A_ALC_TMR_SEL_IVTM_944fs       (0x40)
#define AK4954A_ALC_TMR_SEL_IVTM_1888fs      (0x80)
#define AK4954A_ALC_TMR_SEL_IVTM_3776fs      (0xC0)
/** \} AK4954A_REG_ALC_TMR_SEL (IVTM) */

/**
* \{
* @name AK4954A_REG_ALC_MODE_CTRL1 
* Bit definitions for the AK4954A_REG_ALC_MODE_CTRL1 register
*/
#define AK4954A_ALC_MODE_CTRL1_ALC           (0x20) /**< ALC Enable */
#define AK4954A_ALC_MODE_CTRL1_ALCEQN        (0x80) /**< ALC EQ Enable, NOT */
/** \} AK4954A_REG_ALC_MODE_CTRL1 */
/**
* \{
* @name AK4954A_REG_ALC_MODE_CTRL1 (LMTH)
* Bit field in the AK4954A_REG_ALC_MODE_CTRL1 register\n
* ALC Limiter Detection Level (LDL) / Recovery Counter Reset Level (RCR)
*/
#define AK4954A_ALC_MODE_CTRL1_LMTH_LDL_m2p5dBFS (0x00) /* -2.5 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_LDL_m4p1dBFS (0x01) /* -4.1 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_LDL_m6p0dBFS (0x02) /* -6.0 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_LDL_m8p5dBFS (0x03) /* -8.5 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_RCR_m4p1dBFS (0x00) /* -4.1 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_RCR_m6p0dBFS (0x01) /* -6.0 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_RCR_m8p5dBFS (0x02) /* -8.5 dBFS */
#define AK4954A_ALC_MODE_CTRL1_LMTH_RCR_m12dBFS  (0x03) /*  -12 dBFS */
/** \} AK4954A_REG_ALC_MODE_CTRL1 (LMTH) */
/**
* \{
* @name AK4954A_REG_ALC_MODE_CTRL1 (RGAIN)
* Bit field in the AK4954A_REG_ALC_MODE_CTRL1 register\n
* ALC Recovery Gain Step
*/
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00424db_1fs  (0x00) /* 0.00424dB  1/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00212db_1fs  (0x04) /* 0.00212dB  1/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_1fs  (0x08) /* 0.00106dB  1/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_2fs  (0x0C) /* 0.00106dB  2/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_4fs  (0x10) /* 0.00106dB  4/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_8fs  (0x14) /* 0.00106dB  8/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_16fs (0x18) /* 0.00106dB 16/fs */
#define AK4954A_ALC_MODE_CTRL1_RGAIN_0p00106db_32fs (0x1C) /* 0.00106dB 32/fs */
/** \} AK4954A_REG_ALC_MODE_CTRL1 (RGAIN) */

/**
* \{
* @name AK4954A_REG_HP_OUTPUT_CTRL 
* Bit definitions for the AK4954A_REG_HP_OUTPUT_CTRL register
*/
#define AK4954A_HP_OUTPUT_CTRL_HPZ           (0x80) /**< Pull-down Setting of HP Amplifier */
/** \} AK4954A_REG_HP_OUTPUT_CTRL */

/**
* \{
* @name AK4954A_REG_BEEP_FREQ 
* Bit definitions for the AK4954A_REG_BEEP_FREQ register
*/
#define AK4954A_BEEP_FREQ_BPCNT              (0x80) /**< BEEP Signal Output Mode Setting */
/** \} AK4954A_REG_BEEP_FREQ */

/**
* \{
* @name AK4954A_REG_BEEP_FREQ (BPFR)
* Bit field in the AK4954A_REG_BEEP_FREQ register\n
* BEEP Signal Output Frequency
*/
#define AK4954A_BEEP_FREQ_BPFR_4000Hz        (0x01)
#define AK4954A_BEEP_FREQ_BPFR_2000Hz        (0x02)
#define AK4954A_BEEP_FREQ_BPFR_1300Hz        (0x03)
#define AK4954A_BEEP_FREQ_BPFR_800Hz         (0x04)
/** \} AK4954A_REG_BEEP_FREQ (BPFR) */

/**
* \{
* @name AK4954A_REG_BEEP_VOL_CTRL 
* Bit definitions for the AK4954A_REG_BEEP_VOL_CTRL register
*/
#define AK4954A_BEEP_VOL_CTRL_BPOUT          (0x80) /**< BEEP Signal Control */
/** \} AK4954A_REG_BEEP_VOL_CTRL */
/**
* \{
* @name AK4954A_REG_BEEP_VOL_CTRL (BPLVL)
* Bit field in the AK4954A_REG_BEEP_VOL_CTRL register
*/
                                             /* BEEP Output level Setting */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_0db      (0x00) /*   0 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m3db     (0x01) /*  -3 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m6db     (0x02) /*  -6 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m9db     (0x03) /*  -9 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m12db    (0x04) /* -12 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m15db    (0x05) /* -15 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m18db    (0x06) /* -18 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m21db    (0x07) /* -21 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m24db    (0x08) /* -24 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m27db    (0x09) /* -27 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m30db    (0x0A) /* -30 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m33db    (0x0B) /* -33 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m36db    (0x0C) /* -36 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m39db    (0x0D) /* -39 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m42db    (0x0E) /* -42 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m45db    (0x0F) /* -45 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m48db    (0x10) /* -48 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m51db    (0x11) /* -51 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m54db    (0x12) /* -54 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m57db    (0x13) /* -57 dB */
#define AK4954A_BEEP_VOL_CTRL_BPLVL_m60db    (0x14) /* -60 dB */
/** \} AK4954A_REG_BEEP_VOL_CTRL (BPLVL) */

/**
* \{
* @name AK4954A_REG_DIG_FLTR_SEL1 
* Bit definitions for the AK4954A_REG_DIG_FLTR_SEL1 register
*/
#define AK4954A_DIG_FLTR_SEL1_HPFAD          (0x01) /**< HPF1 Control after ADC */
#define AK4954A_DIG_FLTR_SEL1_SDAD           (0x08) /**< ADC Digital Filter Select */
/** \} AK4954A_REG_DIG_FLTR_SEL1 */
/**
* \{
* @name AK4954A_REG_DIG_FLTR_SEL1 (HPFC)
* Bit field in the AK4954A_REG_DIG_FLTR_SEL1 register\n
* Cut-off Frequency Setting of HPF1
*/
#define AK4954A_DIG_FLTR_SEL1_HPFC_96kHz_0p62Hz     (0x00) /* fc fs=96kHz,  0.62Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_96kHz_2p49Hz     (0x02) /* fc fs=96kHz,  2.49Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_96kHz_19p9Hz     (0x04) /* fc fs=96kHz, 19.9Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_96kHz_39p8Hz     (0x06) /* fc fs=96kHz, 39.8Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_44p1kHz_1p7Hz    (0x00) /* fc fs=44.1kHz,   1.7Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_44p1kHz_6p9Hz    (0x02) /* fc fs=44.1kHz,   6.9Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_44p1kHz_54p8Hz   (0x04) /* fc fs=44.1kHz,  54.8Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_44p1kHz_109p7Hz  (0x06) /* fc fs=44.1kHz, 109.7Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_22p05kHz_3p4Hz   (0x00) /* fc fs=22.05kHz,   3.4Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_22p05kHz_13p7Hz  (0x02) /* fc fs=22.05kHz,  13.7Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_22p05kHz_109p7Hz (0x04) /* fc fs=22.05kHz, 109.7Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_22p05kHz_219p3Hz (0x06) /* fc fs=22.05kHz, 219.3Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_8kHz_7p5Hz       (0x00) /* fc fs=8kHz,   7.5Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_8kHz_29p8Hz      (0x02) /* fc fs=8kHz,  29.8Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_8kHz_238p7Hz     (0x04) /* fc fs=8kHz, 238.7Hz */
#define AK4954A_DIG_FLTR_SEL1_HPFC_8kHz_477p5Hz     (0x06) /* fc fs=8kHz, 477.5Hz */
/** \} AK4954A_REG_DIG_FLTR_SEL1 (HPFC) */

/**
* \{
* @name AK4954A_REG_DIG_FLTR_SEL2 
* Bit definitions for the AK4954A_REG_DIG_FLTR_SEL2 register
*/
#define AK4954A_DIG_FLTR_SEL2_HPF            (0x01) /**< HPF2 Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL2_LPF            (0x02) /**< LPF Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL2_FIL3           (0x10) /**< FIL3 (Stereo Emphasis Filter) Coefficient
                                                         Setting Enable*/
#define AK4954A_DIG_FLTR_SEL2_EQ0            (0x20) /**< EQ0 (Gain Compensation Filter) Coefficient
                                                         Setting Enable */
/** \} AK4954A_REG_DIG_FLTR_SEL2 */
/**
* \{
* @name AK4954A_REG_DIG_FLTR_SEL2 (GN)
* Bit field in the AK4954A_REG_DIG_FLTR_SEL2 register\n
* Gain Setting of the Gain Block
*/
#define AK4954A_DIG_FLTR_SEL2_GN_0db         (0x00)
#define AK4954A_DIG_FLTR_SEL2_GN_12db        (0x40)
#define AK4954A_DIG_FLTR_SEL2_GN_24db        (0x80)
/** \} AK4954A_REG_DIG_FLTR_SEL2 (GN) */

/**
* \{
* @name AK4954A_REG_DIG_FLTR_MODE 
* Bit definitions for the AK4954A_REG_DIG_FLTR_MODE register
*/
#define AK4954A_DIG_FLTR_MODE_PFSDO          (0x01) /**< SDTO Output Signal Select */
#define AK4954A_DIG_FLTR_MODE_ADCPF          (0x02) /**< Programmable Filter / ALC Input Signal
                                                         Select */
#define AK4954A_DIG_FLTR_MODE_PFDAC          (0x04) /**< DAC Input Signal Select */
#define AK4954A_DIG_FLTR_MODE_PMDRC          (0x80) /**< Dynamic Range Control Circuit Power
                                                         Management */
/** \} AK4954A_REG_DIG_FLTR_MODE */

/**
* \{
* @name AK4954A_REG_DIG_FLTR_SEL3 
* Bit definitions for the AK4954A_REG_DIG_FLTR_SEL3 register
*/
#define AK4954A_DIG_FLTR_SEL3_EQ1            (0x01) /**< Equalizer 1 Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL3_EQ2            (0x02) /**< Equalizer 2 Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL3_EQ3            (0x04) /**< Equalizer 3 Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL3_EQ4            (0x08) /**< Equalizer 4 Coefficient Setting Enable */
#define AK4954A_DIG_FLTR_SEL3_EQ5            (0x10) /**< Equalizer 5 Coefficient Setting Enable */
/** \} AK4954A_REG_DIG_FLTR_SEL3 */

/**
* \{
* @name AK4954A_REG_DRC_MODE_CTRL (DRCC)
* Bit field in the AK4954A_REG_DRC_MODE_CTRL register\n
* DRC Setting Enable
*/
#define AK4954A_DRC_MODE_CTRL_DRCC_DISABLE   (0x00)
#define AK4954A_DRC_MODE_CTRL_DRCC_LOW       (0x01)
#define AK4954A_DRC_MODE_CTRL_DRCC_MIDDLE    (0x02)
#define AK4954A_DRC_MODE_CTRL_DRCC_HIGH      (0x03)
/** \} AK4954A_REG_DRC_MODE_CTRL (DRCC) */
/**
* \{
* @name AK4954A_REG_DRC_MODE_CTRL (DRGAIN)
* Bit field in the AK4954A_REG_DRC_MODE_CTRL register\n
* DRC Recovery Speed Setting
*/
#define AK4954A_DRC_MODE_CTRL_DRGAIN_8kHz_1p1dBps     (0x00) /* 8kHz, 1.1dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_8kHz_2p1dBps     (0x04) /* 8kHz, 2.1dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_8kHz_4p2dBps     (0x08) /* 8kHz, 4.2dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_8kHz_8p5dBps     (0x0C) /* 8kHz, 8.5dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_16kHz_2p1dBps    (0x00) /* 16kHz,  2.1dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_16kHz_4p2dBps    (0x04) /* 16kHz,  4.2dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_16kHz_8p5dBps    (0x08) /* 16kHz,  8.5dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_16kHz_17p0dBps   (0x0C) /* 16kHz, 17.0dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_44p1kHz_5p9dBps  (0x00) /* 44.1kHz,  5.9dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_44p1kHz_11p7dBps (0x04) /* 44.1kHz, 11.7dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_44p1kHz_23p4dBps (0x08) /* 44.1kHz, 23.4dB/s */
#define AK4954A_DRC_MODE_CTRL_DRGAIN_44p1kHz_46p7dBps (0x0C) /* 44.1kHz, 46.7dB/s */
/** \} AK4954A_REG_DRC_MODE_CTRL (DRGAIN) */
/**
* \{
* @name AK4954A_REG_DRC_MODE_CTRL (DLMAT)
* Bit field in the AK4954A_REG_DRC_MODE_CTRL register\n
* DRC Attenuation Speed Setting
*/
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_0p1dBpms     (0x00) /* 8kHz, 0.1dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_0p3dBpms     (0x10) /* 8kHz, 0.3dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_0p5dBpms     (0x20) /* 8kHz, 0.5dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_1p1dBpms     (0x30) /* 8kHz, 1.1dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_2p2dBpms     (0x40) /* 8kHz, 2.2dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_8kHz_4p5dBpms     (0x50) /* 8kHz, 4.5dB/ms     */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_0p3dBpms    (0x00) /* 16kHz, 0.3dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_0p5dBpms    (0x10) /* 16kHz, 0.5dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_1p1dBpms    (0x20) /* 16kHz, 1.1dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_2p2dBpms    (0x30) /* 16kHz, 2.2dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_4p4dBpms    (0x40) /* 16kHz, 4.4dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_16kHz_9p0dBpms    (0x50) /* 16kHz, 9.0dB/ms    */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_0p7dBpms  (0x00) /* 44.1kHz, 0.7dB/ms  */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_1p5dBpms  (0x10) /* 44.1kHz, 1.5dB/ms  */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_3p0dBpms  (0x20) /* 44.1kHz, 3.0dB/ms  */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_6p0dBpms  (0x30) /* 44.1kHz, 6.0dB/ms  */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_12p2dBpms (0x40) /* 44.1kHz, 12.2dB/ms */
#define AK4954A_DRC_MODE_CTRL_DLMAT_44p1kHz_24p7dBpms (0x50) /* 44.1kHz, 24.7dB/ms */
/** \} AK4954A_REG_DRC_MODE_CTRL (DLMAT) */

/**
* \{
* @name AK4954A_REG_NS_CTRL 
* Bit definitions for the AK4954A_REG_NS_CTRL register
*/
#define AK4954A_NS_CTRL_NSCE                 (0x01) /**< Noise Suppression Setting Enable */
#define AK4954A_NS_CTRL_NSHPF                (0x02) /**< HPF for Noise Suppression Coefficient
                                                         Setting Enable */
#define AK4954A_NS_CTRL_NSLPF                (0x04) /**< Noise Suppression LPF Coefficient
                                                         Setting Enable */
/** \} AK4954A_REG_NS_CTRL */
/**
* \{
* @name AK4954A_REG_NS_CTRL (DRCM)
* Bit field in the AK4954A_REG_NS_CTRL register \n
* DRC Input Signal Setting
*/
#define AK4954A_NS_CTRL_DRCM_STEREO          (0x00)
#define AK4954A_NS_CTRL_DRCM_LEFT            (0x10)
#define AK4954A_NS_CTRL_DRCM_RIGHT           (0x20)
/** \} AK4954A_REG_NS_CTRL (DRCM) */

/**
* \{
* @name AK4954A_REG_NS_GAIN_AND_ATT_CTRL (NSATT)
* Bit field in the AK4954A_REG_NS_GAIN_AND_ATT_CTRL register\n
* Noise Suppression Attenuation Speed
*/
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_1p1dBps      (0x00) /* 8kHz,  1.1dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_2p1dBps      (0x01) /* 8kHz,  2.1dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_4p2dBps      (0x02) /* 8kHz,  4.2dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_8p5dBps      (0x03) /* 8kHz,  8.5dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_17p0dBps     (0x04) /* 8kHz, 17.0dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_8kHz_33p9dBps     (0x05) /* 8kHz, 33.9dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_2p1dBps     (0x00) /* 16kHz,  2.1dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_4p2dBps     (0x01) /* 16kHz,  4.2dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_8p5dBps     (0x02) /* 16kHz,  8.5dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_17p0dBps    (0x03) /* 16kHz, 17.0dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_33p9dBps    (0x04) /* 16kHz, 33.9dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_16kHz_67p9dBps    (0x05) /* 16kHz, 67.9dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_5p8dBps   (0x00) /* 44.1kHz,   5.8dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_11p7dBps  (0x01) /* 44.1kHz,  11.7dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_23p4dBps  (0x02) /* 44.1kHz,  23.4dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_46p8dBps  (0x03) /* 44.1kHz,  46.8dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_93p5dBps  (0x04) /* 44.1kHz,  93.5dB/s */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSATT_44p1kHz_187p1dBps (0x05) /* 44.1kHz, 187.1dB/s */
/** \} AK4954A_REG_NS_GAIN_AND_ATT_CTRL (NSATT) */
/**
* \{
* @name AK4954A_REG_NS_GAIN_AND_ATT_CTRL (NSGAIN)
* Bit field in the AK4954A_REG_NS_GAIN_AND_ATT_CTRL register\n
* Noise Suppression Recovery Speed
*/
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_8kHz_0p3dBpms     (0x00) /* 8kHz, 0.3dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_8kHz_0p5dBpms     (0x10) /* 8kHz, 0.5dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_8kHz_1p1dBpms     (0x20) /* 8kHz, 1.1dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_8kHz_2p2dBpms     (0x30) /* 8kHz, 2.2dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_8kHz_4p5dBpms     (0x40) /* 8kHz, 4.5dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_16kHz_0p5dBpms    (0x00) /* 16kHz, 0.5dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_16kHz_1p1dBpms    (0x10) /* 16kHz, 1.1dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_16kHz_2p2dBpms    (0x20) /* 16kHz, 2.2dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_16kHz_4p4dBpms    (0x30) /* 16kHz, 4.4dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_16kHz_9p0dBpms    (0x40) /* 16kHz, 9.0dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_44p1kHz_1p5dBpms  (0x00) /* 44.1kHz,  1.5dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_44p1kHz_3p0dBpms  (0x10) /* 44.1kHz,  3.0dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_44p1kHz_6p0dBpms  (0x20) /* 44.1kHz,  6.0dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_44p1kHz_12p2dBpms (0x30) /* 44.1kHz, 12.2dB/ms */
#define AK4954A_NS_GAIN_AND_ATT_CTRL_NSGAIN_44p1kHz_24p7dBpms (0x40) /* 44.1kHz, 24.7dB/ms */
/** \} AK4954A_REG_NS_GAIN_AND_ATT_CTRL (NSGAIN) */

/**
* \{
* @name AK4954A_REG_NS_ON_LEVEL 
* Bit field in the AK4954A_REG_NS_ON_LEVEL register\n
* Noise Suppression Threshold Low Level Setting
*/
#define AK4954A_NS_ON_LEVEL_NSTHL_m36p0db    (0x00) /* -36.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m37p5db    (0x01) /* -37.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m39p0db    (0x02) /* -39.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m40p5db    (0x03) /* -40.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m42p0db    (0x04) /* -42.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m43p5db    (0x05) /* -43.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m45p0db    (0x06) /* -45.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m46p5db    (0x07) /* -46.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m48p0db    (0x08) /* -48.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m49p5db    (0x09) /* -49.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m51p0db    (0x0A) /* -51.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m52p5db    (0x0B) /* -52.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m54p0db    (0x0C) /* -54.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m55p5db    (0x0D) /* -55.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m57p0db    (0x0E) /* -57.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m58p5db    (0x0F) /* -58.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m60p0db    (0x10) /* -60.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m61p5db    (0x11) /* -61.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m63p0db    (0x12) /* -63.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m64p5db    (0x13) /* -64.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m66p0db    (0x14) /* -66.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m67p5db    (0x15) /* -67.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m69p0db    (0x16) /* -69.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m70p5db    (0x17) /* -70.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m72p0db    (0x18) /* -72.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m73p5db    (0x19) /* -73.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m75p0db    (0x1A) /* -75.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m76p5db    (0x1B) /* -76.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m78p0db    (0x1C) /* -78.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m79p5db    (0x1D) /* -79.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m81p0db    (0x1E) /* -81.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHL_m82p5db    (0x1F) /* -82.5 dB */
/** \} AK4954A_REG_NS_ON_LEVEL */
/**
* \{
* @name AK4954A_REG_NS_ON_LEVEL (NSIAF)
* Bit field in the AK4954A_REG_NS_ON_LEVEL register\n
* Moving Avarage Parameter Setting at Noise Suppression Off
*/
#define AK4954A_NS_ON_LEVEL_NSIAF_256fs      (0x00)
#define AK4954A_NS_ON_LEVEL_NSIAF_512fs      (0x40)
#define AK4954A_NS_ON_LEVEL_NSIAF_1024fs     (0x80)
#define AK4954A_NS_ON_LEVEL_NSIAF_2048fs     (0xC0)
/** \} AK4954A_REG_NS_ON_LEVEL (NSIAF) */

/**
* \{
* @name AK4954A_REG_NS_OFF_LEVEL 
* Bit field in the AK4954A_REG_NS_OFF_LEVEL register\n
* Noise Suppression Threshold High Level Setting
*/
#define AK4954A_NS_ON_LEVEL_NSTHH_m36p0db    (0x00) /* -36.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m37p5db    (0x01) /* -37.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m39p0db    (0x02) /* -39.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m40p5db    (0x03) /* -40.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m42p0db    (0x04) /* -42.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m43p5db    (0x05) /* -43.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m45p0db    (0x06) /* -45.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m46p5db    (0x07) /* -46.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m48p0db    (0x08) /* -48.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m49p5db    (0x09) /* -49.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m51p0db    (0x0A) /* -51.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m52p5db    (0x0B) /* -52.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m54p0db    (0x0C) /* -54.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m55p5db    (0x0D) /* -55.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m57p0db    (0x0E) /* -57.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m58p5db    (0x0F) /* -58.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m60p0db    (0x10) /* -60.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m61p5db    (0x11) /* -61.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m63p0db    (0x12) /* -63.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m64p5db    (0x13) /* -64.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m66p0db    (0x14) /* -66.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m67p5db    (0x15) /* -67.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m69p0db    (0x16) /* -69.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m70p5db    (0x17) /* -70.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m72p0db    (0x18) /* -72.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m73p5db    (0x19) /* -73.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m75p0db    (0x1A) /* -75.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m76p5db    (0x1B) /* -76.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m78p0db    (0x1C) /* -78.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m79p5db    (0x1D) /* -79.5 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m81p0db    (0x1E) /* -81.0 dB */
#define AK4954A_NS_ON_LEVEL_NSTHH_m82p5db    (0x1F) /* -82.5 dB */
/**
* \{
* @name AK4954A_REG_NS_OFF_LEVEL (NSOAF)
* Bit field in the AK4954A_REG_NS_OFF_LEVEL register\n
* Moving Avarage Parameter Setting at Noise Suppression On
*/
#define AK4954A_NS_OFF_LEVEL_NSOAF_4fs       (0x00)
#define AK4954A_NS_OFF_LEVEL_NSOAF_8fs       (0x40)
#define AK4954A_NS_OFF_LEVEL_NSOAF_16fs      (0x80)
#define AK4954A_NS_OFF_LEVEL_NSOAF_32fs      (0xC0)
/** \} AK4954A_REG_NS_OFF_LEVEL (NSOAF) */

/**
* \{
* @name AK4954A_REG_NSREF 
* Bit field in the AK4954A_REG_NSREF register\n
* Reference Value at Noise Suppression
*/
#define AK4954A_NSREF_m9db  (0x00) /*  -9 dB */
#define AK4954A_NSREF_m12db (0x01) /* -12 dB */
#define AK4954A_NSREF_m15db (0x02) /* -15 dB */
#define AK4954A_NSREF_m18db (0x03) /* -18 dB */
#define AK4954A_NSREF_m21db (0x04) /* -21 dB */
#define AK4954A_NSREF_m24db (0x05) /* -24 dB */
#define AK4954A_NSREF_m27db (0x06) /* -27 dB */
#define AK4954A_NSREF_m30db (0x07) /* -30 dB */
#define AK4954A_NSREF_m33db (0x08) /* -33 dB */
#define AK4954A_NSREF_m36db (0x09) /* -36 dB */
#define AK4954A_NSREF_m39db (0x0A) /* -39 dB */
#define AK4954A_NSREF_m42db (0x0B) /* -42 dB */
#define AK4954A_NSREF_m45db (0x0C) /* -45 dB */
#define AK4954A_NSREF_m48db (0x0D) /* -48 dB */
#define AK4954A_NSREF_m51db (0x0E) /* -51 dB */
#define AK4954A_NSREF_m54db (0x0F) /* -54 dB */
/** \} AK4954A_REG_NSREF */

/**
* \{
* @name AK4954A_REG_DVLC_FLTR_SEL (DHHPF)
* Bit field in the AK4954A_REG_DVLC_FLTR_SEL register\n
* DVLC High Frequency Range HPF Setting
*/
#define AK4954A_DVLC_FLTR_SEL_DHHPF_DISABLE  (0x00)
#define AK4954A_DVLC_FLTR_SEL_DHHPF_1ST      (0x01)
#define AK4954A_DVLC_FLTR_SEL_DHHPF_2ND      (0x02)
/** \} AK4954A_REG_DVLC_FLTR_SEL (DHHPF) */
/**
* \{
* @name AK4954A_REG_DVLC_FLTR_SEL (DMLPF)
* Bit field in the AK4954A_REG_DVLC_FLTR_SEL register\n
* DVLC Middle Frequency Range LPF Coefficient Setting Enable
*/
#define AK4954A_DVLC_FLTR_SEL_DMLPF_DISABLE  (0x00)
#define AK4954A_DVLC_FLTR_SEL_DMLPF_1ST      (0x04)
#define AK4954A_DVLC_FLTR_SEL_DMLPF_2ND      (0x08)
/** \} AK4954A_REG_DVLC_FLTR_SEL (DMLPF) */
/**
* \{
* @name AK4954A_REG_DVLC_FLTR_SEL (DMHPF)
* Bit field in the AK4954A_REG_DVLC_FLTR_SEL register\n
* DVLC Middle Frequency Range HPF Coefficient Setting Enable
*/
#define AK4954A_DVLC_FLTR_SEL_DMHPF_DISABLE  (0x00)
#define AK4954A_DVLC_FLTR_SEL_DMHPF_1ST      (0x10)
#define AK4954A_DVLC_FLTR_SEL_DMHPF_2ND      (0x20)
/** \} AK4954A_REG_DVLC_FLTR_SEL (DMHPF) */
/**
* \{
* @name AK4954A_REG_DVLC_FLTR_SEL (DLLPF)
* Bit field in the AK4954A_REG_DVLC_FLTR_SEL register\n
* DVLC Low Frequency Range LPF Coefficient * Setting Enable
*/
#define AK4954A_DVLC_FLTR_SEL_DLLPF_DISABLE  (0x00)
#define AK4954A_DVLC_FLTR_SEL_DLLPF_1ST      (0x40)
#define AK4954A_DVLC_FLTR_SEL_DLLPF_2ND      (0x80)
/** \} AK4954A_REG_DVLC_FLTR_SEL (DLLPF) */

/**
* \{
* @name AK4954A_REG_DVLC_MODE_CTRL (DAF)
* Bit field in the AK4954A_REG_DVLC_MODE_CTRL register\n
* Moving Avarage Parameter Setting for DVLC
*/
#define AK4954A_DVLC_MODE_CTRL_DAF_256fs     (0x00)
#define AK4954A_DVLC_MODE_CTRL_DAF_384fs     (0x01)
#define AK4954A_DVLC_MODE_CTRL_DAF_512fs     (0x02)
#define AK4954A_DVLC_MODE_CTRL_DAF_1024fs    (0x03)
/** \} AK4954A_REG_DVLC_MODE_CTRL (DAF) */
/**
* \{
* @name AK4954A_REG_DVLC_MODE_CTRL (DVLMAT)
* Bit field in the AK4954A_REG_DVLC_MODE_CTRL register\n
* DVLC Attenuation Speed Setting
*/
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_1p1dBps      (0x00) /* 8kHz,  1.1dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_2p1dBps      (0x04) /* 8kHz,  2.1dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_4p2dBps      (0x08) /* 8kHz,  4.2dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_8p5dBps      (0x0C) /* 8kHz,  8.5dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_17p0dBps     (0x10) /* 8kHz, 17.0dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_33p9dBps     (0x14) /* 8kHz, 33.9dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_8kHz_67p9dBps     (0x18) /* 8kHz, 67.9dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_2p1dBps     (0x00) /* 16kHz,  2.1dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_4p2dBps     (0x04) /* 16kHz,  4.2dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_8p5dBps     (0x08) /* 16kHz,  8.5dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_17p0dBps    (0x0C) /* 16kHz, 17.0dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_33p9dBps    (0x10) /* 16kHz, 33.9dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_67p9dBps    (0x14) /* 16kHz, 67.9dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_16kHz_135p8dBps   (0x18) /* 16kHz, 135.8dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_5p8dBps   (0x00) /* 44.1kHz,   5.8dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_11p7dBps  (0x04) /* 44.1kHz,  11.7dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_23p4dBps  (0x08) /* 44.1kHz,  23.4dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_46p8dBps  (0x0C) /* 44.1kHz,  46.8dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_93p5dBps  (0x10) /* 44.1kHz,  93.5dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_187p1dBps (0x14) /* 44.1kHz, 187.1dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVLMAT_44p1kHz_374p3dBps (0x18) /* 44.1kHz, 374.3dB/s */
/** \} AK4954A_REG_DVLC_MODE_CTRL (DVLMAT) */
/**
* \{
* @name AK4954A_REG_DVLC_MODE_CTRL (DVRGAIN)
* Bit field in the AK4954A_REG_DVLC_MODE_CTRL register\n
* DVLC Recovery Speed Setting
*/
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_0p07dBps    (0x00) /* 8kHz, 0.07dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_0p13dBps    (0x20) /* 8kHz, 0.13dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_0p27dBps    (0x40) /* 8kHz, 0.27dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_0p53dBps    (0x50) /* 8kHz, 0.53dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_1p06dBps    (0x80) /* 8kHz, 1.06dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_2p12dBps    (0xA0) /* 8kHz, 2.12dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_8kHz_4p24dBps    (0xC0) /* 8kHz, 4.24dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_0p13dBps   (0x00) /* 16kHz, 0.13dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_0p27dBps   (0x20) /* 16kHz, 0.27dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_0p53dBps   (0x40) /* 16kHz, 0.53dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_1p06dBps   (0x50) /* 16kHz, 1.06dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_2p12dBps   (0x80) /* 16kHz, 2.12dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_4p24dBps   (0xA0) /* 16kHz, 4.24dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_16kHz_8p48dBps   (0xC0) /* 16kHz, 8.48dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_0p37dBps (0x00) /* 44.1kHz,  0.37dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_0p73dBps (0x20) /* 44.1kHz,  0.73dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_1p46dBps (0x40) /* 44.1kHz,  1.46dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_2p92dBps (0x50) /* 44.1kHz,  2.92dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_5p84dBps (0x80) /* 44.1kHz,  5.84dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_11p7dBps (0xA0) /* 44.1kHz, 11.7dB/s */
#define AK4954A_DVLC_MODE_CTRL_DVRGAIN_44p1kHz_23p4dBps (0xC0) /* 44.1kHz, 23.4dB/s */
/** \} AK4954A_REG_DVLC_MODE_CTRL (DVRGAIN) */

#define AK4954A_RESET_WAIT_DELAY   (10)    /**< Wait Delay, in milli seconds */

/**
* \{
* @name Volume
* Volume Control Constants
*/
#define AK4954A_HP_VOLUME_DEFAULT  (0x0C)  /**< Default Value (0.0dB) */
#define AK4954A_HP_VOLUME_MAX      (0x00)  /**< Maximum Value (+6.0dB) */
#define AK4954A_HP_VOLUME_MIN      (0x8F)  /**< Minimum Value (-65.5dB) */
#define AK4954A_HP_VOLUME_MUTE     (0x90)  /**< Writing >= 0x90 mutes the headphone output */
/** \} Volume */

/**
* \{
* @name Default Configuration
* Default Configuration Values
*/
#define AK4954A_DEF_SAMPLING_RATE   AK4954A_MODE_CTRL2_CM_256fs
#define AK4954A_DEF_DATA_ALIGNMENT  AK4954A_MODE_CTRL1_DIF_32_I2S
/** \} Default Configuration */

/** Initialization failure error */
#define CY_RSLT_AK4954A_INIT_FAIL \
    (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_HARDWARE_AK4954A, 0))

/**
 * Initialize the I2C communication with the audio codec and do basic configuration of
 * the codec.
 * @param[in] i2c_inst I2C instance to use for communicating with the AK4954A audio codec.
 * @return CY_RSLT_SUCCESS if properly initialized, else an error indicating what went wrong.
 */
cy_rslt_t mtb_ak4954a_init(cyhal_i2c_t *i2c_inst);

/**
 * Frees up any resources allocated by the display as part of \ref mtb_ak4954a_init().
 */
void mtb_ak4954a_free();

/**
 * This function updates the volume of both the left and right channels of the
 * headphone output.
 *
 * @param[in] volume - Steps of 0.5dB, where:
 *            Minimum volume: -65.5dB (0x8F)
 *            Maximum volume:  +6.0dB (0x00)
 *            Mute: (0x90~0xFF)
 *
 */
void mtb_ak4954a_adjust_volume(uint8_t volume);

/**
 * Activates the codec - This function is called in conjunction with 
 * ak4954A_deactivate API after successful configuration update of the codec.
 */
void mtb_ak4954a_activate(void);

/**
 * Deactivates the codec - the configuration is retained, just the codec 
 * input/outputs are disabled. The function should be called before changing 
 * any setting in the codec over I2C
 */
void mtb_ak4954a_deactivate(void);

/**
 * This function writes a data byte to an audio codec register
 *
 * @param[in] reg   The audio codec register to update
 * @param[in] data  The byte to be written to the audio codec register
 */
void mtb_ak4954a_write_byte(mtb_ak4954a_reg_t reg, uint8_t data);

/**
 * This function sets bits in a register.  This function can be used instead
 * of mtb_ak4954a_write_byte() if you want to change a single bit or select bits in
 * the register and preserve the value of other bits in the register. Only the bits
 * set to 1 in the mask are effected.
 *
 * @param[in] reg   The audio codec register to update
 * @param[in] mask  The mask used to set bits in the register
 */
void mtb_ak4954a_set(mtb_ak4954a_reg_t reg, uint8_t mask);

/**
 * This function clears bits in a register.  This function can be used instead
 * of mtb_ak4954a_write_byte() if you want to change a single bit or select bits in
 * the register and preserve the value of other bits in the register. Only the bits
 * set to 1 in the mask are effected.
 *
 * @param[in] reg   The audio codec register to update
 * @param[in] mask  The mask used to clear bits in the register
 */
void mtb_ak4954a_clear(mtb_ak4954a_reg_t reg, uint8_t mask);

/**
 * This function writes a data value that spans two register addresses
 *
 * @param[in] reg   The first of two audio codec registers to update
 * @param[in] data  The word (two-byte value) to be written to the audio codec register
 */
void mtb_ak4954a_write_word(mtb_ak4954a_reg_t reg, uint16_t data);

/**
 * This function writes multiple data bytes to the audio codec registers
 *
 * @param[in] reg       The first audio codec register to update
 * @param[in] pData     Pointer to the buffer that has data
 * @param[in] numBytes  Number of bytes to be written to the display controller
 */
void mtb_ak4954a_write_data_stream(mtb_ak4954a_reg_t reg, uint8_t *pData, uint8_t numBytes);

/**
 * This function reads a data byte from an audio codec register
 *
 * @param[in] reg    The audio codec register read
 * @return data The byte read from the audio codec register
 */
uint8_t mtb_ak4954a_read_byte(mtb_ak4954a_reg_t reg);

/**
 * This function reads a data value that spans two register addresses
 *
 * @param[in] reg    The first of two audio codec registers to read
 * @return data The word (two-byte value) read from the audio codec register
 */
uint16_t mtb_ak4954a_read_word(mtb_ak4954a_reg_t reg);

/**
 * This function reads multiple data bytes from the audio codec registers
 *
 * @param[in] reg       The first audio codec register to read
 * @param[in] pData     Pointer to the location to store the data read
 * @param[in] numBytes  Number of bytes to read from the display controller
 */
void mtb_ak4954a_read_data_stream(mtb_ak4954a_reg_t reg, uint8_t *pData, uint8_t numBytes);

#if defined(__cplusplus)
}
#endif

/** \} group_board_libs */
