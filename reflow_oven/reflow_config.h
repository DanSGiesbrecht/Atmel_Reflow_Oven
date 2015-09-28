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
#ifndef REFLOW_CONFIG_H
#define REFLOW_CONFIG_H

/*------------------------------------------------------------------------*/
/*-----------------------   HARDWARE DEFINES   ---------------------------*/
/*** PROCESSOR PACKAGE ***/
//#define _ATMEGA328_DIP_
#define _ATMEGA328_32TQFP_    // Either DIP or TQFP, Cannot be both.

/*** CLOCK FREQUENCY ****/
#define F_CPU 16000000UL

/*** VOLTAGE SOURCE ****/
#define __5V0__
//#define __3V3__      // Either __5V0__ or __3V3__ selected at a time.

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
/*
    LCD Pin Assignment

 *  Used by:
 *    @ lcd.h
 *    @ lcd.c
*/
/*------------------------------------------------------------------------*/
#ifdef _ATMEGA328_DIP_

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

#endif

#ifdef _ATMEGA328_32TQFP_

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

#endif
/**************************************************************************/

#endif

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/