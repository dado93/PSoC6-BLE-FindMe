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

static cy_stc_ble_conn_handle_t connection_handle;

static uint8_t alert_level;

static void BLEFindMe_EnterLowPowerMode(void);



void BLEFindMe_Init(void)
{
    /* Initialize debug interface */
    DEBUG_START();
    DEBUG_PRINTF("\f");
    DEBUG_PRINTF("PSoC MCU BLE - Find Me Example\r\n");

}

void BLEFindMe_Process(void)
{

}

void BLEFindMe_EnterLowPowerMode(void)
{
    Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
}