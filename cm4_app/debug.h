/**
* \file         debug.h
* \brief        Header file for the debug interface in the Find Me project.
* 
* The debug interface can be enabled/disable by properly setting the 
* \ref DEBUG_ENABLED macro to either \ref ENABLED or \ref DISBALED.
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

#ifndef __DEBUG_H__
    #define __DEBUG_H__
    
    #include "cybsp.h"
    #include "cy_retarget_io.h"

    #ifdef __cplusplus
    extern "C" {
    #endif /* __cplusplus */

    /**
     *  \brief Enable value. 
     */
    #define ENABLED 1

    /**
     *  \brief Disable value
     */
    #define DISABLED 0

    /**
     *  \brief Flag to enable/disable debug. 
     * 
     * You can set this macro to \ref ENABLED or \ref DISABLED
     * to turn ON or OFF the debug interface, respectively.
     */
    #define DEBUG_ENABLED ENABLED

    #if (DEBUG_ENABLED == ENABLED)
        #define DEBUG_START()       cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE)
        #define DEBUG_PRINTF(...)   printf(__VA_ARGS__)
    #else
        #define DEBUG_START()
        #define DEBUG_PRINTF(...)
    #endif

    void ShowError(void);

    #ifdef __cplusplus
    }
    #endif /* __cplusplus */
#endif
