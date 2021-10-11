/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "cybsp.h"
#include "cyhal.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "accel_task.h"
#include "cy_tft.h" // Library for CY8CKIT-028-TFT
#include "GUI.h" // Library for emWin
#include "mtb_bmi160.h" // Library for Motion Sensor

/*******************************************************************************
* Macro Definitions
*******************************************************************************/


/*******************************************************************************
 * Global variable
 ******************************************************************************/
extern bool user_button_press_onboot;

char task_accel_buffer[100];

/* Motion sensor */
cyhal_i2c_t i2c;
cyhal_i2c_cfg_t i2c_cfg = {
    .is_slave = false,
    .address = 0,
    .frequencyhal_hz = 400000
};

/*******************************************************************************
 * Function Name: task_accel
 *******************************************************************************/
void task_accel(void* param)
{

    /* Suppress warning for unused parameter */
    (void)param;

    cy_rslt_t result = CY_RSLT_SUCCESS;

    // Delay to wait for other task to initialise
    cyhal_system_delay_ms(100);

    /* Initialize i2c for motion sensor */
    result = cyhal_i2c_init(&i2c, CY8CKIT_028_TFT_PIN_IMU_I2C_SDA, CY8CKIT_028_TFT_PIN_IMU_I2C_SCL, NULL);

    /* Initialize motion sensor */
    mtb_bmi160_t motion_sensor;
    result = mtb_bmi160_init_i2c(&motion_sensor, &i2c, MTB_BMI160_DEFAULT_ADDRESS);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    GUI_SetFont(GUI_FONT_13B_1);
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);

    // Button press on boot - enter data forwarding mode
    long previous = xTaskGetTickCount();
    if (user_button_press_onboot) {
        GUI_DispStringAt("Data Forwarding Mode", 0, 200);

        float scaled_x = 0;
        float scaled_y = 0;
        float scaled_z = 0;

        mtb_bmi160_data_t data;
        while (1) {
            while ((xTaskGetTickCount() - previous) < 10);
            previous = xTaskGetTickCount();

            mtb_bmi160_read(&motion_sensor, &data);
            scaled_x = data.accel.x / (32768.0 * 2.0); // Signed 16 bits scaled to 2g.
            scaled_y = data.accel.y / (32768.0 * 2.0); // Signed 16 bits scaled to 2g.
            scaled_z = data.accel.z / (32768.0 * 2.0); // Signed 16 bits scaled to 2g.
            printf("%0.5f,%0.5f,%0.5f\n", scaled_x, scaled_y, scaled_z);
        }
    }

    /* Repeatedly running part of the task */
    mtb_bmi160_data_t data;
    for(;;)
    {
        /* Get the accel and gyro data and print the results to the UART */
        result = mtb_bmi160_read(&motion_sensor, &data);
        vTaskDelay(100);

        /* Display on screen */
        snprintf(task_accel_buffer, 100,
                "Accel/ X:%-6d Y:%-6d Z:%-6d\n"
                "Gyro / X:%-6d Y:%-6d Z:%-6d\n",
                data.accel.x, data.accel.y, data.accel.z,
                data.gyro.x, data.gyro.y, data.gyro.z);
        GUI_DispStringAt(task_accel_buffer, 0, 140);
    }
}

/* END OF FILE [] */
