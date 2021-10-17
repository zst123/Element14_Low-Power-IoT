# BMI-160 Inertial Measurement Unit (Motion Sensor)

### Overview

This library provides functions for interfacing with the BMI-160 I2C/SPI 16-bit Inertial Measurement Unit with three axis accelerometer and three axis gyroscope as used on the CY8CKIT-028-EPD and CY8CKIT-028-TFT shields.

Data Sheet: https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi160.html
GitHub: https://github.com/BoschSensortec/BMI160_driver

### Quick Start
Follow the steps below to create a simple application which outputs the
accelerometer and gyroscope data from the motion sensor to the uart
1. Create an empty PSoC6 application
2. Add this library to the application
3. Add retarget-io library using the Library Manager
4. Place following code in the main.c file.
5. Define I2C SDA and SCL as appropriate for your hardware/shield kit
```cpp
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "mtb_bmi160.h"

mtb_bmi160_t motion_sensor;
cyhal_i2c_t i2c;
cyhal_i2c_cfg_t i2c_cfg = {
    .is_slave = false,
    .address = 0,
    .frequencyhal_hz = 400000
};

#define IMU_I2C_SDA (?) // Define me
#define IMU_I2C_SCL (?) // Define me

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize i2c for motion sensor */
    result = cyhal_i2c_init(&i2c, IMU_I2C_SDA, IMU_I2C_SCL, NULL);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    result = cyhal_i2c_configure(&i2c, &i2c_cfg);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize motion sensor */
    result = mtb_bmi160_init_i2c(&motion_sensor, &i2c, MTB_BMI160_DEFAULT_ADDRESS);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    for (;;)
    {
        /* Get the accel and gyro data and print the results to the UART */
        mtb_bmi160_data_t data;
        mtb_bmi160_read(&motion_sensor, &data);

        printf("Accel: X:%6d Y:%6d Z:%6d\r\n", data.accel.x, data.accel.y, data.accel.z);
        printf("Gyro : X:%6d Y:%6d Z:%6d\r\n\r\n", data.gyro.x, data.gyro.y, data.gyro.z);

        cyhal_system_delay_ms(1000);
    }
}
```
6. Build the application and program the kit.

### More information

* [API Reference Guide](https://cypresssemiconductorco.github.io/sensor-motion-bmi160/html/index.html)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)
* [PSoC 6 Code Examples using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)
* [PSoC 6 Middleware](https://github.com/cypresssemiconductorco/psoc6-middleware)
* [PSoC 6 Resources - KBA223067](https://community.cypress.com/docs/DOC-14644)

---
© Cypress Semiconductor Corporation, 2019-2021.
