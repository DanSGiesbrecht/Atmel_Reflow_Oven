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
#include "quad_encoder.h"

#define DDR(x) (*(&x - 1))/* address of data direction register of port x */

#define DEBOUNCE_TIME_MS    10
/**************************************************************************/
/*      PROTOTYPES (excluded from header)                                 */
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/

/**************************************************************************/
/*      State Machines  (also private)                                    */
/*      This is here and not in the header because static function
 *      prototyptes in a header file included elsewhere causes a
 *      whole mess of problems.  This allows the functions to work
 *      as intended: locally in this file.
 */
/**************************************************************************/
static void EncoderDebounceSM_Idle();

static void EncoderDebounceSM_Wait();

static void EncoderDebounceSM_CheckValid();

/**************************************************************************/
/*      Global Variables that are !exteral!                               */

extern volatile uint16_t   main_MASTER_CTRL_FLAG; 


volatile uint8_t    _encoder_pinhistory;
volatile uint8_t    Encoder_Button_Pushed = 0;

volatile uint8_t    lastEncoded;

/**************************************************************************/
/*      Global Variable definitions with scope limited to this local
 *      application.  Variables shall start with the name 'ENC'
 *      and be declared as static.
 */
/**************************************************************************/
/*      State Machine Function Pointer                                    */
static fnCode_type EncoderDebounce_pfnStateMachine;
/*------------------------------------------------------------------------*/
volatile static int    encoderValue = 0;
static int oldencoderValue = 0;

volatile static uint8_t ENC_LEFT  = 0;
volatile static uint8_t ENC_RIGHT = 0;
volatile static uint8_t ENC_PUSH  = 0;
/**************************************************************************/
/*      STRUCTS                                                           */

typedef struct
{
    InputState status;
    } EncoderInput;

/**************************************************************************/
/**************************************************************************/
/*      Function Definitions                                              */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/
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
    /*  Enable PCINT8 (PC0); PCINT9 (PC1); PCINT10 (PC5).   */
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT13);
    
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


uint8_t WasEncoderTurnedRIGHT()
{
    if (encoderValue > 4)
    {

        return 1;
    }
    return 0;
}

uint8_t WasEncoderTurnedLEFT()
{
    if (encoderValue < -4)
    {
        return 1;
    }
    return 0;
}

uint8_t WasEncoderPressed()
{
    if (Encoder_Button_Pushed)
    {
        return 1;
    }
    return 0;
}

/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/

/*      Initialize the State Machine.                                     */
void EncoderDebounce_Initialize()
{
    EncoderDebounce_pfnStateMachine = EncoderDebounceSM_Idle;
}

/*      Run the current State.                                            */
void EncoderDebounce_ActiveState()
{
    EncoderDebounce_pfnStateMachine();
}
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/


/**************************************************************************/
/*      State Machine Functions  (also private)                           */
/**************************************************************************/

static void EncoderDebounceSM_Idle()
{
    if ((encoderValue > 4) || (encoderValue < -4) || Encoder_Button_Pushed)
    {
        oldencoderValue = encoderValue;
        EncoderDebounce_pfnStateMachine = EncoderDebounceSM_Wait;
    }
}

static void EncoderDebounceSM_Wait()
{
    static uint8_t count = 0;
    if (count == DEBOUNCE_TIME_MS)
    {
        EncoderDebounce_pfnStateMachine = EncoderDebounceSM_CheckValid;
        count = 0;
    }
    else
    {
        count++;
    }
}

static void EncoderDebounceSM_CheckValid()
{
    if (oldencoderValue == encoderValue)
    {
        if (encoderValue > 0)
        {
            ENC_RIGHT = 1;
        }
        else
        {
            ENC_LEFT = 1;
        }
    }
    
    if (Encoder_Button_Pushed == 1)
    {
        ENC_PUSH = 1;
    }
    
    encoderValue = 0;
    EncoderDebounce_pfnStateMachine = EncoderDebounceSM_Idle;
}

/**************************************************************************/
/*------------------------------------------------------------------------*/
/*      INTERRUPT SERVICE ROUTINE - PCINT 1                               */
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
        
/* This switches the direction of the encoder   */
/* Increasing: Right turn
 * Decreasing: Left turn
 */        
#ifdef INVERT_ENCODER_DIRECTION
        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010
        || sum == 0b1011) encoderValue--;
        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001
        || sum == 0b1000) encoderValue++;
#else
        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010
        || sum == 0b1011) encoderValue++;
        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001
        || sum == 0b1000) encoderValue--;
#endif

        lastEncoded = encoded;
    }
    
    if (changedbits & (1 << ENCODER_PUSH) )
    {
        if (ENCODER_PINS & (1 << ENCODER_PUSH))
        {
            //Encoder_Button_Pushed = 0;
        }
        else
        {
            Encoder_Button_Pushed = 1;
        }
    }
    
}



/**************************************************************************/
/*      END OF FILE                                                       */
/**************************************************************************/