/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "cybsp.h"
#include "cyhal.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "accel_task.h"
#include "ble_task.h"
#include "cy_tft.h" // Library for CY8CKIT-028-TFT
#include "GUI.h" // Library for emWin
#include "mtb_bmi160.h" // Library for Motion Sensor
#include "ei_c_wrapper.h" // Edge Impulse Model

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


/* Edge Impulse */

float features[300] = { 0 };

// Example test features
float flick_test_features[300] = {
    -0.0012, 0.0018, 0.2559, -0.0013, 0.0001, 0.2677, 0.0002, -0.0050, 0.2787, -0.0022, -0.0043, 0.2811, -0.0025, 0.0020, 0.2770, -0.0019, 0.0036, 0.2690, -0.0008, 0.0081, 0.2632, -0.0021, 0.0040, 0.2665, 0.0012, 0.0001, 0.2650, 0.0013, -0.0008, 0.2675, 0.0001, 0.0008, 0.2695, 0.0006, -0.0030, 0.2750, -0.0008, -0.0079, 0.2759, 0.0030, -0.0134, 0.2761, 0.0032, -0.0135, 0.2756, 0.0005, -0.0104, 0.2719, 0.0001, -0.0091, 0.2666, 0.0018, -0.0088, 0.2579, 0.0000, -0.0108, 0.2556, 0.0013, -0.0101, 0.2577, -0.0023, -0.0079, 0.2662, -0.0027, -0.0050, 0.2725, -0.0013, -0.0050, 0.2712, 0.0015, -0.0056, 0.2673, 0.0017, -0.0066, 0.2615, 0.0014, -0.0081, 0.2609, 0.0010, -0.0054, 0.2572, 0.0013, -0.0026, 0.2599, 0.0008, -0.0027, 0.2647, 0.0027, -0.0061, 0.2780, 0.0168, -0.0234, 0.3177, 0.0215, -0.0543, 0.3958, 0.0217, -0.1028, 0.4767, 0.0427, -0.1607, 0.5000, 0.0243, -0.2238, 0.5000, -0.0246, -0.2872, 0.5000, -0.0738, -0.3390, 0.5000, -0.1049, -0.3277, 0.4725, -0.0272, -0.1154, 0.1424, -0.1361, 0.5000, -0.0646, -0.0737, 0.3787, 0.1631, -0.0194, -0.1022, 0.1937, -0.0687, -0.0986, -0.1071, 0.0194, -0.0825, -0.3551, 0.0573, -0.0682, -0.5000, -0.0164, 0.0321, -0.5000, 0.0692, 0.1144, -0.5000, 0.0752, 0.0353, -0.4355, 0.0569, -0.0310, -0.1805, 0.0272, 0.0400, 0.0867, -0.0474, 0.0692, 0.3643, -0.0319, 0.1151, 0.2115, 0.0169, 0.1776, -0.0457, 0.0091, 0.2443, -0.2266, 0.0287, 0.2654, -0.3042, 0.0356, 0.2248, -0.3088, 0.0204, 0.2280, -0.3235, 0.0044, 0.2685, -0.2924, -0.0095, 0.2429, -0.2127, -0.0060, 0.1542, -0.1289, 0.0101, 0.0582, -0.0186, 0.0192, -0.0185, 0.1429, 0.0076, -0.0058, 0.3127, -0.0489, 0.0575, 0.4305, -0.0043, 0.0894, 0.4264, 0.0010, 0.0765, 0.3806, 0.0149, 0.0345, 0.3411, 0.0287, 0.0031, 0.3025, 0.0418, -0.0230, 0.2767, 0.0556, 0.0322, 0.2443, 0.0998, -0.0398, 0.1574, 0.0073, 0.0532, 0.2225, 0.0144, 0.0083, 0.2305, -0.0531, 0.1018, 0.3565, 0.0176, -0.0142, 0.3753, -0.0047, 0.0032, 0.4043, 0.0149, -0.0087, 0.3634, 0.0089, 0.0013, 0.3269, 0.0093, 0.0213, 0.2880, 0.0079, 0.0469, 0.2626, 0.0122, 0.0519, 0.2604, 0.0159, 0.0495, 0.2552, 0.0160, 0.0497, 0.2560, 0.0144, 0.0466, 0.2698, 0.0121, 0.0415, 0.2853, 0.0115, 0.0310, 0.2940, 0.0103, 0.0243, 0.2973, 0.0094, 0.0131, 0.2981, 0.0127, 0.0049, 0.2936, 0.0043, 0.0102, 0.2800, 0.0057, 0.0205, 0.2571, 0.0089, 0.0252, 0.2399, 0.0092, 0.0298, 0.2364, 0.0075, 0.0311, 0.2346, 0.0093, 0.0291, 0.2427, 0.0111, 0.0233, 0.2556, 0.0070, 0.0203, 0.2658, 0.0098, 0.0094, 0.2771, 0.0115, 0.0072, 0.2832, 0.0039, 0.0102, 0.2790
};

int get_feature_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}


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

    int total_length = sizeof(features) / sizeof(features[0]);
    ei_impulse_result_t ei_result = { 0 };

    // BLE service data
    static myservice_data_t ble_myservice_data = {0};
    static char gesture_text[20];

    // Edge Impulse Inference
    while (1) {

        for (int i = 0; i < 150; i += 3) {
            // Delay of 10ms (100 Hz frequency)
            while ((xTaskGetTickCount() - previous) < 10);
            previous = xTaskGetTickCount();

            // Collect data into the array
            mtb_bmi160_read(&motion_sensor, &data);
            features[i + 0] = data.accel.x / (32768.0 * 2.0);
            features[i + 1] = data.accel.y / (32768.0 * 2.0);
            features[i + 2] = data.accel.z / (32768.0 * 2.0);

            features[150 + i + 0] = data.accel.x / (32768.0 * 2.0);
            features[150 + i + 1] = data.accel.y / (32768.0 * 2.0);
            features[150 + i + 2] = data.accel.z / (32768.0 * 2.0);
        }

        // Do classification
        ei_c_wrapper_run_classifier(
            total_length,
            &get_feature_data,
            &ei_result,
            true);

        // Find the largest value
        float max_value = 0;
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            float current = ei_result.classification[ix].value;
            char * label = (char *) ei_result.classification[ix].label;
            if (current > max_value) {
                max_value = current;
                strncpy(gesture_text, label, 20);
            }
        }

        // Update gestures if it crosses threshold
        if (max_value > 0.6) {
            ble_myservice_data.gesture_text = &gesture_text[0];
            xQueueSendToBack(ble_myservice_data_q, &ble_myservice_data, 0u);
        }

        // Print to screen
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            snprintf(task_accel_buffer, 100, "%s:%.5f",
                    ei_result.classification[ix].label,
                    ei_result.classification[ix].value
            );
            GUI_DispStringAt(task_accel_buffer, 0, 160 + ix*13);
        }
    }

    // Debugging
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
