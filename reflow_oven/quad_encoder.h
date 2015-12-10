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

#include "ATMEGA328P_reflow_config.h"
#include <avr/interrupt.h>


#define ENCODER_PORT    PORTC
#define ENCODER_PINS    PINC
#define ENCODER_PINA    0
#define ENCODER_PINB    1
#define ENCODER_PUSH    5

/**************************************************************************/
/*      TYPEDEFS                                                          */
typedef void(*fnCode_type)(void);

#ifdef  INVERT_ENCODER_DIRECTION
/*
typedef enum {ENCODER_LEFT = 0, ENCODER_RIGHT, ENCODER_BTN} Enc_WhichInput;
#else
typedef enum {ENCODER_RIGHT = 0, ENCODER_LEFT, ENCODER_BTN} Enc_WhichInput;
*/
#endif

typedef enum {inactive = 0, ACTIVE} InputState;

/**************************************************************************/
/**************************************************************************/
/*      Function Prototypes                                               */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/

/*  encoder_pininit();
*   Function:   Initialize the PCInterrupt pins for the specific MCU,
*               record inital values.
*   Requires:   -Proper assignment of pins, and review of the ISR.
*               -Must be called before the global interrupts are
*                enabled. < sei(); >
*   Promises:   To set up the interrupts, and record initial values.
*/
void encoder_pininit(void);

uint8_t WasEncoderTurnedRIGHT();

uint8_t WasEncoderTurnedLEFT();

uint8_t WasEncoderPressed();

/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void EncoderDebounce_Initialize();

/*      Run the current State.                                            */
void EncoderDebounce_ActiveState();

/**************************************************************************/
/*------------------------------------------------------------------------*/
/*      INTERRUPT SERVICE ROUTINE - PCINT 1                               */

ISR (PCINT1_vect);

#endif
/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/