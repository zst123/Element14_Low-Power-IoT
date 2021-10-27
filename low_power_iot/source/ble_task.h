/*******************************************************************************
* File Name: ble_task.h
*
* Description: This file is the public interface of led_task.c source file
*
* Related Document: README.md
*
********************************************************************************
* Copyright (2020), Cypress Semiconductor Corporation.
********************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
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
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*****************************************​**************************************/

/*******************************************************************************
 * Include guard
 ******************************************************************************/
#ifndef BLE_TASK_H
#define BLE_TASK_H

/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*******************************************************************************
* Macro Definitions
*******************************************************************************/
#define NOTIFY_TIME_MS                  (50u)

/* Connection parameters related macro */
#define CONN_INTERVAL                   (80u)
#define CONN_LATENCY                    (0u)
#define SUP_TIMEOUT                     (512u)

/*******************************************************************************
 * Data structure and enumeration
 ******************************************************************************/
/* Structure used for storing APP data */
typedef struct
{
    uint8_t sliderdata; /* contains capsense slider data */
    uint8_t buttoncount; /* Contains capsense button count */
    uint8_t buttonstatus1; /* Contains capsense button status  */
    uint8_t buttonstatus2; /* Contains capsense button status  */
} ble_capsense_data_t;


/*******************************************************************************
 * Global variable
 ******************************************************************************/
extern QueueHandle_t ble_capsense_data_q;
extern QueueHandle_t ble_myservice_data_q;


/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void task_ble(void* param);


#endif /* BLE_TASK_H */


/* [] END OF FILE  */
