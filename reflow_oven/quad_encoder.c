/**************************************************************************/
/*
 * quad_encoder.c
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

//#include <avr/interrupt.h>
#include "reflow_config.h"
#include "quad_encoder.h"

#define DDR(x) (*(&x - 1))/* address of data direction register of port x */

/**************************************************************************/
/*  Volatile Global variables, created in this document.                  */
//volatile uint8_t    _encoder_ms = 0;    // don't think this is needed.
volatile uint8_t    _encoder_pinhistory;
volatile uint8_t    _Button_Pushed = 0;

volatile uint8_t    lastEncoded;
volatile int16_t    encoderValue = 0;


void encoder_pininit(void)
{
    uint8_t MSB;
    uint8_t LSB;
    /*  Set encoder pins as inputs */
    DDR(ENCODER_PORT) &= ~((1 << ENCODER_PINA) | (1 << ENCODER_PINB) |
    (1 << ENCODER_PUSH));
    
    /*  Enable Pullup resistors */
    //ENCODER_PORT |= (1 << ENCODER_PINA) | (1 << ENCODER_PINB) |
    ENCODER_PORT|= (1 << ENCODER_PUSH);
    
    /*  Enable PinChange Interrupt pins 14:8    */
    PCICR |= (1 << PCIE1);
    /*  Enable PCINT8 (PC0); PCINT9 (PC1); PCINT10 (PC2).   */
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);
    
    /*  Record historical values of PCINT   */
    _encoder_pinhistory = ENCODER_PINS;
    /*  Use _encoder_pinhistory to set MSB and LSB,             */
    /*  just in case the values change during this operation.   */
    if ( _encoder_pinhistory & (1 << ENCODER_PINA))
    {
        MSB = 1;
    }
    else
    {
        MSB = 0;
    }
    if ( _encoder_pinhistory & (1 << ENCODER_PINB))
    {
        LSB = 1;
    }
    else
    {
        LSB = 0;
    }
    
    lastEncoded = (MSB << 1) | LSB;
}

/*---------------------------------- --------------------------------------*/
/*------------------------------------------------------------------------*/

ISR (PCINT1_vect)
{
    uint8_t changedbits;
    
    changedbits = ENCODER_PINS ^ _encoder_pinhistory;
    _encoder_pinhistory = ENCODER_PINS;
    
    if (changedbits & ((1 << ENCODER_PINA) | (1 << ENCODER_PINB) ) )
    {
        uint8_t MSB;
        uint8_t LSB;
        uint8_t encoded;
        uint8_t sum;
        
        if ( ENCODER_PINS & (1 << ENCODER_PINA))
        {
            MSB = 1;
        }
        else
        {
            MSB = 0;
        }
        if ( ENCODER_PINS & (1 << ENCODER_PINB))
        {
            LSB = 1;
        }
        else
        {
            LSB = 0;
        }
        
        encoded = (MSB << 1) | LSB;
        sum = (lastEncoded << 2) | encoded;
        
        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010
        || sum == 0b1011) encoderValue ++;
        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001
        || sum == 0b1000) encoderValue --;
        lastEncoded = encoded;
    }
    
    if (changedbits & (1 << ENCODER_PUSH) )
    {
        if (ENCODER_PINS & (1 << ENCODER_PUSH))
        {
            _Button_Pushed = 0;
        }
        else
        {
            _Button_Pushed = 1;
        }
    }
    
}





/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/