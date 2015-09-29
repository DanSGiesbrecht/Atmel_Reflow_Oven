/**************************************************************************/
/*
 * quad_encoder.h
 *
 *   Created: 7/28/2015 7:33:15 PM
 *    Author: dsgiesbrecht
 *   Version: v1.0 - BASIC WORKING FUNCTION
 *  
 * Description:
 *      Driver for a basic 12-step quadrature encoder with pushbutton.
 *      Requires:
 *      -   3 external interrupts for best performance.
 *      -   3 GPIO for successful operation.
 *      Promises:
 *      -   To read the input of a quadrature encoder and interpret it as
 *          directional data.
 * Important Notes:
 *      @   This library uses external ISR's, and is
 *          designed specifically for the ATMega328P. Be sure that your
 *          hardware is compatible.
 */
/**************************************************************************/
/**************************************************************************/

#ifndef     QUAD_ENCODER_H
#define     QUAD_ENCODER_H

#include <avr/interrupt.h>

#define ENCODER_PORT    PORTC
#define ENCODER_PINS    PINC
#define ENCODER_PINA    0
#define ENCODER_PINB    1
#define ENCODER_PUSH    5

/**************************************************************************/
/*  Function:   Initialize the PCInterrupt pins for the specific MCU, 
*               record inital values.
*   Requires:   -Proper assignment of pins, and review of the ISR.
*               -Must be called before the global interrupts are
*                enabled. < sei(); >
*   Promises:   To set up the interrupts, and record initial values.
*/
void encoder_pininit(void);
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

ISR (PCINT1_vect);

#endif
/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/