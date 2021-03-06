/**
 * \file            debug.c                    
 * \brief           Source file for debug interface. 
 *                   
 * Source file for the cm4 debug interface, that allows
 * showing debug messages on a serial port. 
 * 
 * \author Davide Marzorati
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

#include "debug.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/**
 * \brief       Display error message.
 * 
 * The current implemenation turns on all LEDs when 
 * there is an error in the application. 
 */

#include "debug.h"
#include "led.h"

void ShowError(void)
{
    cyhal_gpio_write(CONNECTION_LED, 0);
    cyhal_gpio_write(ALERT_LED, 0);

    CY_ASSERT( 0!=0 );
}
