/*******************************************************************************
* File Name:   ble_task.c
*
* Description: This file contains the task that handles ble events and
* notifications.
*
* Related Document: See README.md
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
 * Header file includes
 ******************************************************************************/

#include "cybsp.h"
#include "cyhal.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "cycfg_gap.h"
#include "cycfg_gatt_db.h"
#include "cycfg_bt_settings.h"
#include "app_platform_cfg.h"
#include "wiced_bt_stack.h"
#include "ble_task.h"
#include "accel_task.h"

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

static void ble_app_init(void);
static wiced_result_t ble_app_management_cb(wiced_bt_management_evt_t event,
                                    wiced_bt_management_evt_data_t *p_event_data);
static wiced_bt_gatt_status_t ble_app_gatt_event_handler( wiced_bt_gatt_evt_t event,
                                    wiced_bt_gatt_event_data_t *p_data);
static wiced_bt_gatt_status_t ble_app_gatts_conn_status_cb(wiced_bt_gatt_connection_status_t *p_conn_status);
static wiced_bt_gatt_status_t ble_app_gatts_req_cb(wiced_bt_gatt_request_type_t type,
                                    wiced_bt_gatt_request_data_t *p_data);
static wiced_bt_gatt_status_t ble_app_set_value(wiced_bt_gatt_write_t *p_write_req);
static wiced_bt_gatt_status_t ble_app_get_value(wiced_bt_gatt_read_t *p_read_req);
static wiced_bt_gatt_status_t ble_app_gatt_read_handler( wiced_bt_gatt_read_t *p_read_req);
static wiced_bt_gatt_status_t ble_app_gatt_write_handler( wiced_bt_gatt_write_t *p_wriet_req);
static void ble_print_bd_address(wiced_bt_device_address_t bdadr);
void ble_send_notification(void);
void myservice_send_notification(void);

/*******************************************************************************
 * Global variable
 ******************************************************************************/
/* Queue handle for ble app data */
QueueHandle_t ble_capsense_data_q;
QueueHandle_t ble_myservice_data_q;

/* Holds the connection ID */
volatile uint16_t ble_connection_id = 0;
/* Holds the capsense data */
ble_capsense_data_t ble_capsense_data;
myservice_data_t ble_myservice_data;
/*******************************************************************************
* Function Name: task_ble
********************************************************************************
* Summary:
*  Task that handles BLE Initilization and updates gaatt nnotification data.
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void task_ble(void* param)
{
    BaseType_t rtos_api_result = pdFAIL;
    wiced_result_t result = WICED_BT_SUCCESS;

    /* Suppress warning for unused parameter */
    (void)param;

    /* Configure platform specific settings for the BT device */
    cybt_platform_config_init(&bt_platform_cfg_settings);

    /* Register call back and configuration with stack */
    result = wiced_bt_stack_init(ble_app_management_cb, &wiced_bt_cfg_settings);

    /* Check if stack initialization was successful */
    if( CY_RSLT_SUCCESS == result)
    {
        printf("Bluetooth Stack Initialization Successful!\r\n");
    }
    else
    {
        printf("Bluetooth Stack Initialization failed!\r\n");
        CY_ASSERT(0);
    }
    /* Repeatedly running part of the task */
    for(;;)
    {
         /* Block until a command has been received over queue */
        rtos_api_result = xQueueReceive(ble_capsense_data_q, &ble_capsense_data,
                                        500);
        /* Command has been received from queue */
        if(pdPASS == rtos_api_result)
        {
            ble_send_notification();

        }

        rtos_api_result = xQueueReceive(ble_myservice_data_q, &ble_myservice_data,
                                        500);
        /* Command has been received from queue */
        if(pdPASS == rtos_api_result)
        {
            myservice_send_notification();

        }


    }
}


/*******************************************************************************
* Function Name: ble_app_management_cb
********************************************************************************
* Summary:
* This function handles the BT stack events.
*
* Parameters:
*  wiced_bt_management_evt_t event: event code
*  wiced_bt_management_evt_data_t *p_event_data : Data corresponding to the event
*
* Return:
*  wiced_result_t : status
*
*******************************************************************************/

wiced_result_t ble_app_management_cb( wiced_bt_management_evt_t event,
                                      wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_result_t status = WICED_BT_SUCCESS;
    wiced_bt_device_address_t bda = { 0 };
    printf("App management cback: 0x%x\r\n", event);
    switch (event)
    {
        case BTM_ENABLED_EVT:
            /* Bluetooth Controller and Host Stack Enabled */
            if (WICED_BT_SUCCESS == p_event_data->enabled.status)
            {
                wiced_bt_dev_read_local_addr(bda);
                printf("Local Bluetooth Address: ");
                ble_print_bd_address(bda);

                /* Perform application-specific initialization */
                ble_app_init();
            }
            break;
        case BTM_DISABLED_EVT:
            break;

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT:

            /* Advertisement State Changed */
            printf("Advertisement state change: 0x%x\r\n",
                                        p_event_data->ble_advert_state_changed);
            break;

        default:
            break;
    }

    return status;
}


/*******************************************************************************
* Function Name: ble_app_gatts_conn_status_cb
********************************************************************************
* Summary:
* This function is invoked on GATT connection event
*
* Parameters:
*  wiced_bt_gatt_connection_status_t *p_conn_status : GATT connection status
*
* Return:
*  wiced_bt_gatt_status_t status : Status
*
*******************************************************************************/

wiced_bt_gatt_status_t ble_app_gatts_conn_status_cb(
                                wiced_bt_gatt_connection_status_t *p_conn_status)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;
    wiced_result_t result = WICED_BT_ERROR;

    if ( NULL != p_conn_status )
    {

        if (p_conn_status->connected)
        {
            /* Device has connected */
            printf("Connected Bluetooth device address:" );
            ble_print_bd_address(p_conn_status->bd_addr);
            printf("Connection ID: 0x%x\r\n", p_conn_status->conn_id );
            /* Store the connection ID */
            ble_connection_id = p_conn_status->conn_id;

        }
        else
        {
            /* Device has disconnected */
            printf("Disconnected Bluetooth device address:" );
            ble_print_bd_address(p_conn_status->bd_addr);
            printf("Connection ID: 0x%x\r\n", p_conn_status->conn_id );
            /* Set the connection id to zero to indicate disconnected state */
            ble_connection_id = 0;
            /* Restart the advertisements */
            result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);

            /* Failed to start advertisement. Stop program execution */
            if (CY_RSLT_SUCCESS != result)
            {
                CY_ASSERT(0);
            }

        }

        status = WICED_BT_GATT_SUCCESS;
    }

    return status;
}


/*******************************************************************************
* Function Name: ble_app_init()
********************************************************************************
* Summary:
* This function handles application level initialization tasks and is called from
* the BT management callback once the BLE stack enabled event BTM_ENABLED_EVT is
* triggered. This function is executed in the BTM_ENABLED_EVT management callback.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/

void ble_app_init( void )
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;
    wiced_result_t result = WICED_BT_ERROR;

    printf("Discover the device with name: \"%s\"\r\n\r\n", app_gap_device_name);

    /* Register with BT stack to receive GATT callback */
    status = wiced_bt_gatt_register(ble_app_gatt_event_handler);
    printf("GATT event handler registration status: 0x%x\r\n", status);

    /* Initialize GATT Database */
    status = wiced_bt_gatt_db_init(gatt_database, gatt_database_len, NULL);
    printf("GATT database initiliazation status: 0x%x\r\n", status);

    /* Disable pairing for this application */
    wiced_bt_set_pairable_mode(WICED_FALSE, WICED_FALSE);

    /* Set Advertisement Data */
    wiced_bt_ble_set_raw_advertisement_data(CY_BT_ADV_PACKET_DATA_SIZE, cy_bt_adv_packet_data);

    /* Start Undirected LE Advertisements on device startup.
     * The corresponding parameters are contained in 'app_bt_cfg.c' */
    result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);

    /* Failed to start advertisement. Stop program execution */
    if (WICED_BT_SUCCESS != result)
    {
        printf("failed to start advertisement! \n");
        CY_ASSERT(0);
    }
}


/*******************************************************************************
* Function Name: ble_app_gatt_event_handler()
********************************************************************************
* Summary:
* This function handles GATT callback events from the BT stack.
*
* Parameters:
*  wiced_bt_gatt_evt_t event                   : BLE GATT event code
*  wiced_bt_gatt_event_data_t *p_event_data    : Pointer to BLE GATT event data
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
*  in wiced_bt_gatt.h
*
********************************************************************************/
wiced_bt_gatt_status_t ble_app_gatt_event_handler( wiced_bt_gatt_evt_t event,
                                              wiced_bt_gatt_event_data_t *p_data)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;
    wiced_bt_gatt_attribute_request_t *p_attr_req = NULL;

    printf("GATT call back event: 0x%x\r\n",event);

    /* Call the appropriate callback function based on the GATT event type, and pass the relevant event
     * parameters to the callback function */

    switch(event)
     {
     case GATT_CONNECTION_STATUS_EVT:
         status = ble_app_gatts_conn_status_cb(&p_data->connection_status);
         if(WICED_BT_GATT_SUCCESS != status)
         {
             printf("GATT Connection CB failed: 0x%x\r\n", status);
         }
         break;

     case GATT_ATTRIBUTE_REQUEST_EVT:
         p_attr_req = &p_data->attribute_request;
         status = ble_app_gatts_req_cb(p_attr_req->request_type,
                                            &p_attr_req->data);
         if(WICED_BT_GATT_SUCCESS != status)
         {
             printf("GATT Request CB failed: 0x%x\r\n", status);
         }
         break;

     default:
         break;
     }
     return status;
}


/*******************************************************************************
* Function Name: ble_app_get_value()
********************************************************************************
* Summary:
* This function handles reading of the attribute value from the GATT database
* and passing the data to the BT stack. The value read from the GATT database
* is stored in a buffer whose starting address is passed as one of the function
* parameters
*
* Parameters:
*  wiced_bt_gatt_write_t *p_read_req : Pointer that contains details of Read
*                                      Request including the attribute handle
*
* Return:
* wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
* in wiced_bt_gatt.h
*
*******************************************************************************/
static wiced_bt_gatt_status_t ble_app_get_value(wiced_bt_gatt_read_t *p_read_req)
{
    wiced_bool_t isHandleInTable = WICED_FALSE;
    wiced_bt_gatt_status_t res = WICED_BT_GATT_INVALID_HANDLE;
    uint16_t attr_handle = p_read_req->handle;
    uint8_t *p_val = p_read_req->p_val;
    uint16_t max_len = *p_read_req->p_val_len;
    uint16_t *p_len = p_read_req->p_val_len;

    /* Check for a matching handle entry */
    for (int i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
        if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
        {
            /* Detected a matching handle in external lookup table */
            isHandleInTable = WICED_TRUE;

            /* Check if the buffer has space to store the data */
            if (app_gatt_db_ext_attr_tbl[i].cur_len <= max_len)
            {
                /* Value fits within the supplied buffer; copy over the value */
                *p_len = app_gatt_db_ext_attr_tbl[i].cur_len;
                memcpy(p_val, app_gatt_db_ext_attr_tbl[i].p_data,
                                app_gatt_db_ext_attr_tbl[i].cur_len);
                res = WICED_BT_GATT_SUCCESS;

            }
            else
            {
                /* Value to read will not fit within the buffer */
                res = WICED_BT_GATT_INVALID_ATTR_LEN;
            }
            break;
        }
    }

    if (!isHandleInTable)
    {
        switch ( attr_handle )
        {
            default:
                /* The read operation was not performed for the indicated handle */
                printf("Read Request to Invalid Handle: 0x%x\n", attr_handle);
                res = WICED_BT_GATT_READ_NOT_PERMIT;
                break;
        }
    }

    return res;
}


/*******************************************************************************
* Function Name: ble_app_set_value()
********************************************************************************
* Summary:
* This function handles writing to the attribute handle in the GATT database
* using the data passed from the BT stack. The value to write is stored in a
* buffer whose starting address is passed as one of the function parameters
*
* Parameters:
*  wiced_bt_gatt_write_t *p_write_req : Pointer that contains details of Write
*                                       Request including the attribute handle
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
*  in wiced_bt_gatt.h
*
*******************************************************************************/
static wiced_bt_gatt_status_t ble_app_set_value(wiced_bt_gatt_write_t *p_write_req)
{
    wiced_bool_t isHandleInTable = WICED_FALSE;
    wiced_bool_t validLen = WICED_FALSE;
    wiced_bt_gatt_status_t res = WICED_BT_GATT_INVALID_HANDLE;
    uint16_t attr_handle = p_write_req->handle;
    uint8_t *p_val = p_write_req->p_val;
    uint16_t len = p_write_req->val_len;

    /* Check for a matching handle entry */
    for (int i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
        if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
        {
            /* Detected a matching handle in external lookup table */
            isHandleInTable = WICED_TRUE;

            /* Check if the buffer has space to store the data */
            validLen = (app_gatt_db_ext_attr_tbl[i].max_len >= len);

            if (validLen)
            {
                /* Value fits within the supplied buffer; copy over the value */
                app_gatt_db_ext_attr_tbl[i].cur_len = len;
                memcpy(app_gatt_db_ext_attr_tbl[i].p_data, p_val, len);
                res = WICED_BT_GATT_SUCCESS;

                /* Add code for any action required when this attribute is written.
                 * In this case, we initilize the characteristic value */

                switch ( attr_handle )
                {
                case HDLD_CAPSENSE_BUTTON_CLIENT_CHAR_CONFIG:
                    ble_capsense_data.buttoncount= 2u; /* No.of Capsense button */
                    ble_capsense_data.buttonstatus1= 0u; /* Button status byte1 */
                    ble_capsense_data.buttonstatus2= 0u; /* Button status byte2 */
                    break;
                case HDLD_CAPSENSE_SLIDER_CLIENT_CHAR_CONFIG:
                    ble_capsense_data.sliderdata= 0u; /* Capsense slider data */
                    break;
                }

                ble_send_notification();
            }
            else
            {
                /* Value to write does not meet size constraints */
                res = WICED_BT_GATT_INVALID_ATTR_LEN;
            }
            break;
        }
    }

    if (!isHandleInTable)
    {
        switch ( attr_handle )
        {
            default:
                /* The write operation was not performed for the indicated handle */
                printf("Write Request to Invalid Handle: 0x%x\r\n", attr_handle);
                res = WICED_BT_GATT_WRITE_NOT_PERMIT;
                break;
        }
    }

    return res;
}


/*******************************************************************************
* Function Name: ble_app_gatt_write_handler()
********************************************************************************
* Summary:
* This function handles Write Requests received from the client device
*
* Parameters:
*  wiced_bt_gatt_write_t *p_write_req : Pointer that contains details of Write
*                                       Request including the attribute handle
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
*  in wiced_bt_gatt.h
*
*******************************************************************************/
static wiced_bt_gatt_status_t ble_app_gatt_write_handler(wiced_bt_gatt_write_t *p_write_req)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_INVALID_HANDLE;

    /* Attempt to perform the Write Request */
    status = ble_app_set_value(p_write_req);

    return status;
}


/*******************************************************************************
* Function Name: ble_app_gatt_read_handler()
********************************************************************************
* Summary:
* This function handles Read Requests received from the client device
*
* Parameters:
*  wiced_bt_gatt_write_t *p_read_req : Pointer that contains details of Read
*                                      Request including the attribute handle
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
*  in wiced_bt_gatt.h
*
*******************************************************************************/
static wiced_bt_gatt_status_t ble_app_gatt_read_handler( wiced_bt_gatt_read_t *p_read_req)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_INVALID_HANDLE;

    /* Attempt to perform the Read Request */
    status = ble_app_get_value(p_read_req);

    return status;
}


/*******************************************************************************
* Function Name: ble_app_gatts_req_cb()
********************************************************************************
* Summary:
* This function handles GATT server events from the BT stack.
*
* Parameters:
*  uint16_t conn_id                        : Connection ID
*  wiced_bt_gatt_request_type_t type       : Type of GATT server event
*  wiced_bt_gatt_request_data_t *p_data    : Pointer to GATT server event data
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e
*  in wiced_bt_gatt.h
*
*******************************************************************************/

static wiced_bt_gatt_status_t ble_app_gatts_req_cb(wiced_bt_gatt_request_type_t type,
                                                wiced_bt_gatt_request_data_t *p_data)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_SUCCESS;

    switch ( type )
    {
        case GATTS_REQ_TYPE_READ:
            /* Attribute read request */
            status = ble_app_gatt_read_handler(&p_data->read_req);
            break;
        case GATTS_REQ_TYPE_WRITE:
            /* Attribute write request */
            status = ble_app_gatt_write_handler(&p_data->write_req);
            break;
        default:
            break;
    }

    return status;
}


/*******************************************************************************
* Function Name: void ble_send_notification()
********************************************************************************
* Summary:
* Sends GATT notification.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/

void ble_send_notification(void)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;

    if((GATT_CLIENT_CONFIG_NOTIFICATION == \
                                    app_capsense_slider_client_char_config[0])
                                    && (0 != ble_connection_id))
    {
        /* capsense slider data to be send*/
        app_capsense_slider[0] = ble_capsense_data.sliderdata;
        status = wiced_bt_gatt_send_notification(
                                    ble_connection_id,
                                    HDLC_CAPSENSE_SLIDER_VALUE,
                                    app_gatt_db_ext_attr_tbl[4].cur_len,
                                    app_gatt_db_ext_attr_tbl[4].p_data);

        if(WICED_BT_GATT_SUCCESS != status)
        {
           printf("Sending CapSense slider notification failed\r\n");
        }

    }

    if((GATT_CLIENT_CONFIG_NOTIFICATION == \
                                    app_capsense_button_client_char_config[0])
                                    && (0 != ble_connection_id))
    {
        /* capsense button data to be send*/
        app_capsense_button[0] = ble_capsense_data.buttoncount;
        app_capsense_button[1] = ble_capsense_data.buttonstatus1;
        app_capsense_button[2] = ble_capsense_data.buttonstatus2;

        status = wiced_bt_gatt_send_notification(
                                    ble_connection_id,
                                    HDLC_CAPSENSE_BUTTON_VALUE,
                                    app_gatt_db_ext_attr_tbl[2].cur_len,
                                    app_gatt_db_ext_attr_tbl[2].p_data);

        if(WICED_BT_GATT_SUCCESS != status)
        {
           printf("Sending CapSense button notification failed\r\n");
        }

    }
}

void myservice_send_notification(void)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;

    if((GATT_CLIENT_CONFIG_NOTIFICATION == \
                                    app_myservice_gesture_client_char_config[0])
                                    && (0 != ble_connection_id))
    {
        strncpy((char * restrict) app_myservice_gesture,
                (const char * restrict) ble_myservice_data.gesture_text,
                app_gatt_db_ext_attr_tbl[6].max_len);
        status = wiced_bt_gatt_send_notification(
                                    ble_connection_id,
                                    HDLC_MYSERVICE_GESTURE_VALUE,
                                    app_gatt_db_ext_attr_tbl[6].cur_len,
                                    app_gatt_db_ext_attr_tbl[6].p_data);

        if(WICED_BT_GATT_SUCCESS != status)
        {
           printf("Sending CapSense slider notification failed\r\n");
        }

    }
}

/*******************************************************************************
* Function Name: ble_print_bd_address()
********************************************************************************
* Summary:
* This is the utility function that prints the address of the Bluetooth device
*
* Parameters:
*  wiced_bt_device_address_t bdaddr : Bluetooth address
*
* Return:
*  None
*
*******************************************************************************/
static void ble_print_bd_address(wiced_bt_device_address_t bdadr)
{
    for(uint8_t i=0;i<BD_ADDR_LEN-1;i++)
    {
        printf("%2X:",bdadr[i]);
    }
    printf("%2X\n",bdadr[BD_ADDR_LEN-1]);
}


/* END OF FILE [] */
