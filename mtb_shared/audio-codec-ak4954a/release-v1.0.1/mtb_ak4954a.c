/**************************************************************************//***
 * \file ak4954a.c
 *
 * Description: This file contains the AK4954A codec control APIs.
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

#include <stdbool.h>

#include "mtb_ak4954a.h"

#define AK4954A_I2C_ADDR  (0x12u)

static cyhal_i2c_t *i2c_ptr;


/*******************************************************************************
 * Initialize the Audio codec.
 *******************************************************************************/
cy_rslt_t mtb_ak4954a_init(cyhal_i2c_t *i2c_inst)
{
    if (i2c_inst == NULL)
        return CY_RSLT_AK4954A_INIT_FAIL;

    i2c_ptr = i2c_inst;

    /* From the AK4954A spec:
     * Upon power-up, the AK4954A must be reset by bringing the PDN pin low. This reset is
     * released when a dummy command is input after the PDN pin is high. This ensures that all
     * internal registers reset to their initial value. This reset is released when the dummy
     * command (Actually, the rising edge of 16th SCL) is input after PDN pin is high. Dummy
     * command is executed by writing all 0's to the register address 00H. It is recommended
     * to set the PDN pin low before power up the AK4954A.
     *
     * We don't want to call mtb_ak4954a_write_byte() here because we will hang on the
     * CY_ASSERT() check.  The second call to cyhal_i2c_master_write() is needed to clean up
     * the error that occurs with the first one.
     * */
    {
        uint8_t buf[] = {AK4954A_REG_PWR_MGMT1, 0};
        cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, buf, sizeof(buf), 0, true);
        cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, buf, sizeof(buf), 0, true);
    }

    /* Clear Power Managament 1 register */
    mtb_ak4954a_write_byte(AK4954A_REG_PWR_MGMT1, 0x00);

    /* Set the data alignment */
    mtb_ak4954a_write_byte(AK4954A_REG_MODE_CTRL1, AK4954A_DEF_DATA_ALIGNMENT);

    /* Set sample rate */
    mtb_ak4954a_write_byte(AK4954A_REG_MODE_CTRL2, AK4954A_DEF_SAMPLING_RATE |
                                                   AK4954A_MODE_CTRL2_FS_48kHz);

    /* Clear Digital Filter Mode register */
    mtb_ak4954a_write_byte(AK4954A_REG_DIG_FLTR_MODE, 0x00);

    return CY_RSLT_SUCCESS;
}


/*******************************************************************************
 * Free the resources used with the Audio codec.
 *******************************************************************************/
void mtb_ak4954a_free(void)
{
    i2c_ptr = NULL;
}


/*******************************************************************************
 * This function writes a data byte to an audio codec register
 *******************************************************************************/
void mtb_ak4954a_write_byte(mtb_ak4954a_reg_t reg, uint8_t data)
{
    cy_rslt_t rslt;
    uint8_t buf[] = {reg, data};

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, buf, sizeof(buf), 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
}


/*******************************************************************************
 * This function sets bits in a register.
 *******************************************************************************/
void mtb_ak4954a_set(mtb_ak4954a_reg_t reg, uint8_t mask)
{
    uint8_t data = mtb_ak4954a_read_byte(reg) | mask;
    mtb_ak4954a_write_byte(reg, data);
}


/*******************************************************************************
 * This function clears bits in a register.
 *******************************************************************************/
void mtb_ak4954a_clear(mtb_ak4954a_reg_t reg, uint8_t mask)
{
    uint8_t data = mtb_ak4954a_read_byte(reg) & ~mask;
    mtb_ak4954a_write_byte(reg, data);
}


/*******************************************************************************
 * This function writes a data value that spans two register addresses
 *******************************************************************************/
void mtb_ak4954a_write_word(mtb_ak4954a_reg_t reg, uint16_t data)
{
    cy_rslt_t rslt;
    uint8_t buf[] = {reg, ((uint8_t*)&data)[0], ((uint8_t*)&data)[1]};

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, buf, sizeof(buf), 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
}


/*******************************************************************************
 * This function writes multiple data bytes to the audio codec
 *******************************************************************************/
void mtb_ak4954a_write_data_stream(mtb_ak4954a_reg_t reg, uint8_t *pData, uint8_t numBytes)
{
    CY_ASSERT(numBytes <= AK4954A_REG_MAX_ADDRESS);

    cy_rslt_t rslt;
    uint8_t buf[AK4954A_REG_MAX_ADDRESS + 1];

    buf[0] = reg;
    memcpy(&buf[1], pData, numBytes);

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, buf, numBytes + 1, 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
}


/*******************************************************************************
 * This function reads a data byte from an audio codec register
 *******************************************************************************/
uint8_t mtb_ak4954a_read_byte(mtb_ak4954a_reg_t reg)
{
    cy_rslt_t rslt;
    uint8_t data;

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, (uint8_t*)(&reg), 1, 0, false);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    rslt = cyhal_i2c_master_read(i2c_ptr, AK4954A_I2C_ADDR, &data, 1, 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    return data;
}


/*******************************************************************************
 * This function reads a data value that spans two register addresses
 *******************************************************************************/
uint16_t mtb_ak4954a_read_word(mtb_ak4954a_reg_t reg)
{
    cy_rslt_t rslt;
    uint16_t data;

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, (uint8_t*)(&reg), 1, 0, false);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    rslt = cyhal_i2c_master_read(i2c_ptr, AK4954A_I2C_ADDR, (uint8_t*)&data, 2, 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    return data;
}


/*******************************************************************************
 * This function reads multiple data bytes from the audio codec
 *******************************************************************************/
void mtb_ak4954a_read_data_stream(mtb_ak4954a_reg_t reg, uint8_t *pData, uint8_t numBytes)
{
    CY_ASSERT(numBytes <= AK4954A_REG_MAX_ADDRESS);

    cy_rslt_t rslt;

    rslt = cyhal_i2c_master_write(i2c_ptr, AK4954A_I2C_ADDR, (uint8_t*)(&reg), 1, 0, false);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    rslt = cyhal_i2c_master_read(i2c_ptr, AK4954A_I2C_ADDR, pData, numBytes, 0, true);
    CY_UNUSED_PARAMETER(rslt); /* CY_ASSERT only processes in DEBUG, ignores for others */
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
}


/*******************************************************************************
 * This function updates the volume of both the left and right channels of the
 * headphone output.
 *******************************************************************************/
void mtb_ak4954a_adjust_volume(uint8_t volume)
{
    mtb_ak4954a_write_byte(AK4954A_REG_LCH_DIG_VOL_CTRL, volume);
    mtb_ak4954a_write_byte(AK4954A_REG_RCH_DIG_VOL_CTRL, volume);
}

/*******************************************************************************
 * Activates the codec - This function is called in conjunction with
 * ak4954A_deactivate API after successful configuration update of the codec.
 *******************************************************************************/
void mtb_ak4954a_activate(void)
{
    /* Enable Power Management DAC */
    mtb_ak4954a_write_byte(AK4954A_REG_PWR_MGMT1,
                           AK4954A_PWR_MGMT1_PMDAC | AK4954A_PWR_MGMT1_PMVCM);

    /* Enable Left/Right Channels */
    mtb_ak4954a_write_byte(AK4954A_REG_PWR_MGMT2,
                           AK4954A_PWR_MGMT2_PMHPL | AK4954A_PWR_MGMT2_PMHPR);
}

/*******************************************************************************
 * Deactivates the codec - the configuration is retained, just the codec
 * input/outputs are disabled. The function should be called before changing
 * any setting in the codec over I2C
 *******************************************************************************/
void mtb_ak4954a_deactivate(void)
{
    /* Disable Left/Right Channels */
    mtb_ak4954a_write_byte(AK4954A_REG_PWR_MGMT2, 0x00);

    /* Disable Power Management DAC */
    mtb_ak4954a_write_byte(AK4954A_REG_PWR_MGMT1, AK4954A_PWR_MGMT1_PMVCM);
}

/* [] END OF FILE */
