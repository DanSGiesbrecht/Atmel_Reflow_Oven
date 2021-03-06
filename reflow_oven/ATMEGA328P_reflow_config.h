/**************************************************************************/
/*
 * reflow_config.h
 *
 *   Created: 7/20/2015 6:20:05 PM
 *    Author: dsgiesbrecht
 *   Version: v1.0
 *
 *  Hardware: @ ATmega328P (DIP Package)
 *            @ 16MHz crystal
 *
 *  History:  v1.0 - breadboard layout
 */
/**************************************************************************/
/**************************************************************************/
#ifndef ATMEGA328P_REFLOW_CONFIG_H
#define ATMEGA328P_REFLOW_CONFIG_H

#include "avr/io.h"
/*------------------------------------------------------------------------*/
/*-----------------------       MACROS         ---------------------------*/
#ifndef __DDR_MACROS__
#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
/* on ATmega64/128 PINF is on port 0x00 and not 0x60 */
#define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
#define PIN(x) (*(&x - 2))    /* address of input register of port x          */
#endif

#endif
/*------------------------------------------------------------------------*/
/*-----------------------   HARDWARE DEFINES   ---------------------------*/
/*** PROCESSOR PACKAGE ***/
//#define _ATMEGA328_DIP_
#define _ATMEGA328_32TQFP_    // Either DIP or TQFP, Cannot be both.

/*** CLOCK FREQUENCY ****/
#define F_CPU 16000000UL
//#define F_CPU 8000000UL

/*** VOLTAGE SOURCE ****/
#define __5V0__
//#define __3V3__      // Either __5V0__ or __3V3__ selected at a time.

/* ------------------------ TIMER2 PRESCALER ---------------------------- */
/* 1ms ticks    */
#ifndef TIMER2_PRESCALE_BITSHIFT
//#define TIMER2_PRESCALE_BITSHIFT    (0x1 <<CS22)    // clk/64 for 1ms period
#define TIMER2_PRESCALE_BITSHIFT      (0x1 <<CS22 | 0x1 <<CS20) // clk/128
#endif                                  // for 2ms period.
#ifndef TIMER2_CTC_BITSHIFT
#define TIMER2_CTC_BITSHIFT         (0x1 << WGM21)  // CTC MODE
#endif
#ifndef TOTAL_TICKS
#define TOTAL_TICKS     250         // for 16MHz: 250. for 8MHz: 125.
#endif

/**************************************************************************/
/*  Definition of a second                                                */
#define TICKS_PER_SEC       500
/**************************************************************************/
/*--------------------------  WARNINGS  ----------------------------------*/
#ifdef __3V3__
#if F_CPU > 8000000UL
# warning "You are at risk of overclocking the ATMega328P."
#endif
#endif

#ifdef __5V0__ 
#if F_CPU > 16000000UL
# warning "You are at risk of overclocking the ATMega328P."
#endif
#endif


/**************************************************************************/
/*      SOFTWARE FLAGS                                                    */
/**************************** Master Control Flags ************************/
/* 16-bit variable for global settings.     */

#define TEMP_REQUEST            0x0001      //  request the temp be taken.
#define TEMP_IS_VALID           0x0002      //  validate temperature.
#define HEATER_POWERED          0x0004      //  enable/disable heating.
#define UPDATE_ENCODER          0x0008
#define GOALTEMP_REACHED        0x0010
//#define                         0x0020
//#define                         0x0040
//#define                         0x0080
//#define                         0x0100
//                              0x0200
//                              ......
#define ELEMENT_PREP_FINISHED   0x4000
#define REFLOW_IN_PROGRESS      0x8000      //  enable/cancel a reflow proc.

/*
 *  TEMP_REQUEST        used by TemperatureMonitor
 *  TEMP_IS_VALID       used by TemperatureMonitor
 *  HEATER_POWERED      used by HeaterControl
 *  
 *  ...
 *  REFLOW_IN_PROGRESS  used by HeaterControl
****************************************************************************/
/***************************************************************************/
/*      ENCODER CONFIGURATION:                                             */

/*      uncomment this define if the direction of the rotary encoder
 *      is opposite (right turn moves cursor left)
 */

//#define     INVERT_ENCODER_DIRECTION



/**************************************************************************/
/**************************************************************************/

/*
    LCD Pin Assignment

 *  Used by:
 *    @ lcd.h
 *    @ lcd.c
*/
/*------------------------------------------------------------------------*/
#ifdef _ATMEGA328_DIP_

/* LCD PINS */
#define LCD_PORT         PORTD        /**< port for the LCD lines   */
#define LCD_DATA0_PORT   LCD_PORT     /**< port for 4bit data bit 0 */
#define LCD_DATA1_PORT   LCD_PORT     /**< port for 4bit data bit 1 */
#define LCD_DATA2_PORT   LCD_PORT     /**< port for 4bit data bit 2 */
#define LCD_DATA3_PORT   PORTB        /**< port for 4bit data bit 3 */
#define LCD_DATA0_PIN    5            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    6            /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    7            /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    0            /**< pin for 4bit data bit 3  */
#define LCD_RS_PORT      LCD_PORT     /**< port for RS line         */
#define LCD_RS_PIN       2            /**< pin  for RS line         */
#define LCD_RW_PORT      LCD_PORT     /**< port for RW line         */
#define LCD_RW_PIN       3            /**< pin  for RW line         */
#define LCD_E_PORT       LCD_PORT     /**< port for Enable line     */
#define LCD_E_PIN        4            /**< pin  for Enable line     */
#define BACKLIGHT_PORT   PORTB
#define LCD_BACKLIGHT    1

/* !!! incomplete !!! */

#endif

#ifdef _ATMEGA328_32TQFP_

/* LCD PINS */
#define LCD_PORT         PORTD        /**< port for the LCD lines   */
#define LCD_DATA0_PORT   PORTB        /**< port for 4bit data bit 0 */
#define LCD_DATA1_PORT   LCD_PORT     /**< port for 4bit data bit 1 */
#define LCD_DATA2_PORT   LCD_PORT     /**< port for 4bit data bit 2 */
#define LCD_DATA3_PORT   LCD_PORT     /**< port for 4bit data bit 3 */
#define LCD_DATA0_PIN    0            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    7            /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    6            /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    5            /**< pin for 4bit data bit 3  */
#define LCD_RS_PORT      LCD_PORT     /**< port for RS line         */
#define LCD_RS_PIN       2            /**< pin  for RS line         */
#define LCD_RW_PORT      LCD_PORT     /**< port for RW line         */
#define LCD_RW_PIN       3            /**< pin  for RW line         */
#define LCD_E_PORT       LCD_PORT     /**< port for Enable line     */
#define LCD_E_PIN        4            /**< pin  for Enable line     */
#define BACKLIGHT_PORT   PORTB
#define LCD_BACKLIGHT    1

/* HEATER/FAN PINS */
#define HEATER_PORT     PORTC        /**< port for the relay control*/
#define HEATER_PIN      4            /**< pin for the relay         */
#define FAN_PORT        PORTC        /**< port for the fan          */
#define FAN_PIN         3            /**< pin for the fan           */

#endif


/* ATMEGA328P_REFLOW_CONFIG_H */
#endif

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/