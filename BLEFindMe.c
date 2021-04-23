/**
* \file         BLEFindMe.c
* \brief        Source file for BLE Interface in the Find Me project.
*
* \author       Davide Marzorati
*/

/*******************************************************************************
 * Copyright (c) 2021 Marzorati Davide
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
*******************************************************************************/
#include "BLEFindMe.h"

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cycfg_ble.h"

#include "debug.h"
#include "led.h"

/**
 * \brief           BLE Connection handle.
 */ 
static cy_stc_ble_conn_handle_t connection_handle;

/**
 * \brief           Current alert level for Immediate Alert Service.
 */ 
static uint8_t alert_level;

/**
 * \brief           Timer flag updated every 1 second, for LED toggling.
 */ 
static uint8_t timer_flag = 1;

/**
 *  \brief          Variable to get 1-second timer from BLE Timer.
 */
static cy_stc_ble_timer_info_t  timer_param = { .timeout = 1u /* second */ };

static void BLEFindMe_EnterLowPowerMode(void);
void BLEFindMe_StackEventHandler(uint32_t event, void* event_parameters);
void BLEFindMe_IasEventHandler(uint32_t event, void* event_parameters);
void BLEFindMe_Process(void);

/**
 * \brief           BLE Interrupt Handler.
 * 
 *                  It is required to implement this handler
 *                  as the code is running only on the CM4.
 */ 
static void BlessInterrupt(void)
{
    /* Call interrupt processing */
    Cy_BLE_BlessIsrHandler();
}


/**
 *  \brief          Initialize BLE Find Me components.
 * 
 *                  This function initializes all the components required for proper
 *                  functionality of the BLE component:
 *                      - the BLE
 *                      - the debug interface
 *                      - the GPIOs connected to the LEDs
 *                      - the GPIOs connected to a switch to leave hibernate mode
 *                      - the BLESS isr
 */
void BLEFindMe_Init(void)
{
    //cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    /* Initialize debug interface */
    DEBUG_START();
    DEBUG_PRINTF("\f");
    DEBUG_PRINTF("PSoC MCU BLE - Find Me Example\r\n");
    
    /* Initialize the LEDs */
    cyhal_gpio_init(CONNECTION_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, LED_OFF);
    cyhal_gpio_init(ALERT_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, LED_OFF);

    /* Assign BLESS interrupt number and priority */
    const cy_stc_sysint_t blessIsrCfg =
    {
        /* The BLESS interrupt */
        .intrSrc      = bless_interrupt_IRQn,
        /* The interrupt priority number */
        .intrPriority = 1u
    };
    /* Hook interrupt service routines for BLESS */
    (void) Cy_SysInt_Init(&blessIsrCfg, &BlessInterrupt);
    /* Store pointer to blessIsrCfg in BLE configuration structure */
    cy_ble_config.hw->blessIsrConfig = &blessIsrCfg;

    cy_en_ble_api_result_t result;

    /* Register stack event callback */
    Cy_BLE_RegisterEventCallback(BLEFindMe_StackEventHandler);

    result = Cy_BLE_Init(&cy_ble_config);
    if ( result != CY_BLE_SUCCESS )
    {
        DEBUG_PRINTF("Cy_BLE_Init API Error %x\r\n", result);
        ShowError();
    }
    else
    {
        DEBUG_PRINTF("Cy_BLE_Init API Success %x\r\n", result);
    }

    cy_stc_ble_stack_lib_version_t version;
    result = Cy_BLE_GetStackLibraryVersion(&version);
    if ( result != CY_BLE_SUCCESS )
    {
        DEBUG_PRINTF("Cy_BLE_GetStackLibraryVersion API Error %x\r\n", result);
    }
    else
    {
        DEBUG_PRINTF("Cy_BLE_GetStackLibraryVersion API Success %x\r\n", result);
        DEBUG_PRINTF("Stack Library Version: %d.%d.%d.%d\r\n", version.majorVersion, 
                                                                version.minorVersion,
                                                                version.patch,
                                                                version.buildNumber);
    }

    /* Enable BLE */
    Cy_BLE_Enable();

    /* Enable Low Power Mode */
    Cy_BLE_EnableLowPowerMode();

    /* Register IAS Event handler for service specific events */
    Cy_BLE_IAS_RegisterAttrCallback(BLEFindMe_IasEventHandler);
}

/**
 * \brief           Main processing loop for BLE Find Me.
 * 
 *                  Check current status of \ref alert_level variable and update
 *                  the LED accordingly.
 */ 
void BLEFindMe_Process(void)
{
    Cy_BLE_ProcessEvents();

    switch(alert_level)
    {
        case CY_BLE_NO_ALERT:
            cyhal_gpio_write(ALERT_LED, LED_OFF);
            break;
        case CY_BLE_MILD_ALERT:
            if (timer_flag)
            {
                cyhal_gpio_toggle(ALERT_LED);
            }
            break;
        case CY_BLE_HIGH_ALERT:
            cyhal_gpio_write(ALERT_LED, LED_ON);
            break;
        default:
            break;
    }

    if (timer_flag)
    {
        timer_flag = 0;
        Cy_BLE_StartTimer(&timer_param);
    }

    BLEFindMe_EnterLowPowerMode();
}

/**
 *  \brief          Enter Low Power mode. 
 * 
 *                  This function is called as many times as possible
 *                  to save on battery life.
 */
void BLEFindMe_EnterLowPowerMode(void)
{
    Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
}

/**
 * \brief           BLE Stack event handler.
 */ 
void BLEFindMe_StackEventHandler(uint32_t event, void* event_parameters)
{
    cy_en_ble_api_result_t result;
    switch (event)
    {
        /*****************************************
         *          Generic Events               *
         ****************************************/
        case CY_BLE_EVT_STACK_ON:
            DEBUG_PRINTF("CY_BLE_STACK_ON\r\n");
            // Start Advertising
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            break;
        case CY_BLE_EVT_HARDWARE_ERROR:
            DEBUG_PRINTF("CY_BLE_HARDWARE_ERROR\r\n");
            ShowError();
            break;
        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            DEBUG_PRINTF("CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE\r\n");
            DEBUG_PRINTF("Entering Hibernate Mode\r\n");
            Cy_SysPm_SystemEnterHibernate();
            break;
        /*****************************************
         *             GAP Events                *
         ****************************************/
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DEBUG_PRINTF("CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            if ( Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING )
            {
                DEBUG_PRINTF("Started Advertising\r\n");
            }
            else
            {
                DEBUG_PRINTF("Stopped Advertising\r\n");
                // Advertising timed out before completing, enter hibernate mode
                Cy_BLE_Disable();

            }
            break;
        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            break;
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            
            if ( Cy_BLE_GetConnectionState(connection_handle) == CY_BLE_CONN_STATE_DISCONNECTED)
            {
                DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
                alert_level = CY_BLE_NO_ALERT;
                result = Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
                if (result != CY_BLE_SUCCESS)
                {
                    DEBUG_PRINTF("CY_BLE_GAPP_StartAdvertisement API Error %x\r\n", result);
                    DEBUG_PRINTF("Could not start advertising\r\n");
                    ShowError();
                }
                else
                {
                    DEBUG_PRINTF("CY_BLE_GAPP_StartAdvertisement API Success %x\r\n", result);
                }
            }
            break;
        case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_ENCHANCE_CONN_COMPLETE\r\n");
            break;
        case CY_BLE_EVT_TIMEOUT:
            // Check if generic timeout event by proper casting of parameters
            if ( (((cy_stc_ble_timeout_param_t *) event_parameters)->reasonCode == CY_BLE_GENERIC_APP_TO)
                    && (((cy_stc_ble_timeout_param_t *) event_parameters)->timerHandle == timer_param.timerHandle) )
            {
                timer_flag = timer_flag + 1;
                // Update LED state
                if (Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
                {
                    // Toogle connection LED during advertising
                    cyhal_gpio_toggle(CONNECTION_LED);
                    // Turn off alert led
                    cyhal_gpio_write(ALERT_LED, LED_OFF);
                }
                else if (Cy_BLE_GetNumOfActiveConn() == 0)
                {
                    // Turn on connection LED
                    cyhal_gpio_write(CONNECTION_LED, LED_ON);
                    // Turn off alert LED
                    cyhal_gpio_write(ALERT_LED, LED_OFF);
                }
                else
                {
                    // Connected state, turn connection LED off
                    cyhal_gpio_write(CONNECTION_LED, LED_OFF);
                }
            }
            break;
        default:
            break;
    }
}

/**
 *  \brief          BLE Immediate Alert Service Handler.
 */ 
void BLEFindMe_IasEventHandler(uint32_t event, void* event_parameters)
{
    if (event == CY_BLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        Cy_BLE_IASS_GetCharacteristicValue(CY_BLE_IAS_ALERT_LEVEL, sizeof(alert_level), &alert_level);
    }

    (void) event_parameters;
}