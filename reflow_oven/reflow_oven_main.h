/**************************************************************************/
/*
 * reflow_oven_main.h
 *
 * Created: 10/03/2015 3:48:26 PM
 *  Author: dsgiesbrecht
 *  
 *	Important Notes:
 *		- Refer to <reflow_config.h> for hardware specifics.
 *		- Details on overhead process flags can be found in <reflow_config.h>
 *
 *
 */ 
/**************************************************************************/
/**************************************************************************/
#ifndef REFLOW_OVEN_MAIN_H_
#define REFLOW_OVEN_MAIN_H_


#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "max31855.h"
#include "quad_encoder.h"
#include "ProfileFunctions.h"
#include "reflow_EEPROM.h"
#include "HeaterControl.h"
#include "TemperatureMonitor.h"


#define BACKLIGHT_ON    1
#define BACKLIGHT_OFF   0


/**************************** PROTOTYPES ***********************************/

/* Initialize the LCD Backlight */
/* Write a 1 to turn backlight on, 0 to turn backlight off. */
void _LCD_backlight_init(uint8_t);

/* Write to LCD Backlight */
/* Write a 1 to turn backlight on, 0 to turn backlight off. */
void _LCD_backWrite(uint8_t LED_status);

/* Initialize the ATMega328P chip. This calls all initialize functions. */
/* Enable interrupts.                                                   */
void ATMEGA328_init(void);


void Sleep_Mode_init();

/* Put the AVR uC into selected sleep mode. */
void Enter_Sleep_Mode(uint8_t sleep_condition);

/* Initialize the system tick (timer2)      */
void AT328_SysTick_Init();

/* Enable CompareA Interrupt.               */
void AT328_SysTick_Start();

/* Disable CompareA Interrupt.              */
void AT328_SysTick_Stop();


ISR(TIMER2_COMPA_vect);

/****************************** STRUCTS ************************************/



#endif