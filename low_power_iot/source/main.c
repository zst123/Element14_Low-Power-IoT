/*******************************************************************************
* (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
********************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* File Name: main.c
*
* Description: This is the source code for the AnyCloud: BLE Capsense Buttons
*              Slider Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************/
/*******************************************************************************
*        Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cy_tft.h" // Library for CY8CKIT-028-TFT
#include "GUI.h" // Library for emWin
#include "mtb_bmi160.h" // Library for Motion Sensor
#include "image_splash.h"
#include "capsense_task.h"
#include "led_task.h"
#include "ble_task.h"
#include "accel_task.h"

/*******************************************************************************
*        Macro Definitions
*******************************************************************************/

/* Priorities of user tasks in this project. configMAX_PRIORITIES is defined in
* the FreeRTOSConfig.h and higher priority numbers denote high priority tasks.
*/
#define TASK_CAPSENSE_PRIORITY      (1u)
#define TASK_LED_PRIORITY           (1u)
#define TASK_BLE_PRIORITY           (1u)
#define TASK_ACCEL_PRIORITY         (1u)

/* Stack sizes of user tasks in this project */
#define TASK_CAPSENSE_STACK_SIZE    (configMINIMAL_STACK_SIZE)
#define TASK_LED_STACK_SIZE         (configMINIMAL_STACK_SIZE)
#define TASK_BLE_STACK_SIZE         (4u*configMINIMAL_STACK_SIZE)
#define TASK_ACCEL_STACK_SIZE       (4u*configMINIMAL_STACK_SIZE)

/* Queue lengths of message queues used in this project */
#define SINGLE_ELEMENT_QUEUE        (1u)


/******************************************************************************
* Global Variables
******************************************************************************/
/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;
bool user_button_press_onboot;

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/
/*
 *  Entry point to the application with BSP initilization and Tasks creation.
 */
int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* This enables RTOS aware debugging in OpenOCD. */
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the board support package */
    result = cybsp_init();
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0u);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize user button */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
    user_button_press_onboot = cyhal_gpio_read(CYBSP_USER_BTN) == false; // active low

    /* Init the display */
    cy_tft_io_init();
    GUI_Init();
    GUI_Clear();
    GUI_DrawBitmap(&Splash_Screen, 0, 0);

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init( CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                     CY_RETARGET_IO_BAUDRATE);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0u);
    }
    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("*****************AnyCloud Example******************\r\n");
    printf("*****BLE Capsense Buttons & Slider Application*****\r\n");
    printf("***************************************************\r\n\r\n");

    /* Create the queues. See the respective data-types for details of queue
    * contents
    */
    led_command_data_q  = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                     sizeof(led_command_data_t));
    if(NULL == led_command_data_q)
    {
        printf("Failed to create the Queue!\r\n");
        CY_ASSERT(0u);
    }
    capsense_command_q  = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                     sizeof(capsense_command_t));
    if(NULL == capsense_command_q)
    {
        printf("Failed to create the Queue!\r\n");
        CY_ASSERT(0u);
    }

    ble_capsense_data_q  = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                     sizeof(ble_capsense_data_t));
    if(NULL == ble_capsense_data_q)
    {
        printf("Failed to create the Queue!\r\n");
        CY_ASSERT(0u);
    }

    ble_myservice_data_q  = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                     sizeof(ble_capsense_data_t));
    if(NULL == ble_myservice_data_q)
    {
        printf("Failed to create the Queue!\r\n");
        CY_ASSERT(0u);
    }

    /* Create the user tasks. See the respective task definition for more
    * details of these tasks.
    */

    if (pdPASS != xTaskCreate(task_capsense, "CapSense Task", TASK_CAPSENSE_STACK_SIZE,
                              NULL, TASK_CAPSENSE_PRIORITY, NULL))
    {
        printf("Failed to create the CapSense task!\r\n");
        CY_ASSERT(0u);
    }

    if (pdPASS != xTaskCreate(task_led, "Led Task", TASK_LED_STACK_SIZE,
                              NULL, TASK_LED_PRIORITY, NULL))
    {
        printf("Failed to create the LED task!\r\n");
        CY_ASSERT(0u);
    }

    if (pdPASS != xTaskCreate(task_ble, "Ble Task", TASK_BLE_STACK_SIZE,
                              NULL, TASK_BLE_PRIORITY, NULL))
    {
        printf("Failed to create the BLE task!\r\n");
        CY_ASSERT(0u);
    }

    if (pdPASS != xTaskCreate(task_accel, "Accel Task", TASK_ACCEL_STACK_SIZE,
                              NULL, TASK_ACCEL_PRIORITY, NULL))
	{
		printf("Failed to create the Accel task!\r\n");
		CY_ASSERT(0u);
	}

    /* Start the RTOS scheduler. This function should never return */
    vTaskStartScheduler();

    /*~~~~~~~~~~~~~~~~~~~~~ Should never get here! ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* RTOS scheduler exited */
    /* Halt the CPU if scheduler exits */
    CY_ASSERT(0u);

}

/* [] END OF FILE */
