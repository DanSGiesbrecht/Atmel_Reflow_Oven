/**************************************************************************/
/*
 * reflow_oven_main.c
 *
 * Created: 7/13/2015 12:51:26 PM
 *  Author: dsgiesbrecht
 *  
 *	Important Notes:
 *		- Refer to <reflow_config.h> for hardware specifics.
 *		
 *
 *
 */ 
/**************************************************************************/
/**************************************************************************/

#include "reflow_config.h"
#include "reflow_oven_main.h"

/* INCLUDED ELSEWHERE */
//#include "lcd.h"
//#include "max31855.h"
//#include "quad_encoder.h"

//#include <stdlib.h>
//#include <avr/io.h>
//#include <avr/pgmspace.h>
//#include <util/delay.h>
//#include <avr/power.h>
//#include <avr/sleep.h>

/**************************************************************************/
/*      GLOBAL VARIABLES                                                  */    
extern volatile int encoderValue;

/**************************************************************************/
/* MACROS                                                                 */
#ifndef __DDR_MACROS__
#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
/* on ATmega64/128 PINF is on port 0x00 and not 0x60 */
#define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
#define PIN(x) (*(&x - 2))    /* address of input register of port x          */
#endif

#endif


/**************************************************************************/

/**************************************************************************/
/*         TODO                                                           */
/*        @     Move prototypes to reflow_oven_main.h
*/

/* Initialize the LCD Backlight */
/* Write a 1 to turn backlight on, 0 to turn backlight off. */
void _LCD_backlight_init(uint8_t);

/* Write to LCD Backlight */
/* Write a 1 to turn backlight on, 0 to turn backlight off. */
void _LCD_backWrite(uint8_t LED_status);

int main()
{
    
    /* !!! INITIALIZE !!! */
    /* spi bus */
    max31855_spi_init_master();
    /* 16x2 LCD */
    //lcd_init(LCD_DISP_ON);
    /* Backlight */
    _LCD_backlight_init(0);
    /* rotary encoder*/
    encoder_pininit();
    /* system timer */
    // TODO write timer functions
    
    sei();
    /* Allow peripherals to stabilize. */
    _delay_ms(200);
    
    while(1)
    {
        // TODO check stuff.
        
        // TODO sleep for 1ms.
        
    }
    return 0;
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#if 0
int main(void)
{
    double current_tempval = 0.0;
    double old_tempval = 0.0;
    
    int old_encoderVal = encoderValue;
    
    char tempstring[6];
    
    /*  Initialize the SPI bus.                 */
    max31855_spi_init_master();
    /*  Initialize the display, cursor off.     */
    lcd_init(LCD_DISP_ON);
    /*  Initialize the encoder pins.            */
    encoder_pininit();
    
    DDRB |= (1 << LCD_BACKLIGHT);
    PORTB|= (1 << LCD_BACKLIGHT);  
    
    /*  Enable global interrupts.               */
    sei();
    /*  Allow time for hardware peripherals to 
        stabilize.                              */
    _delay_ms(500);
    
    /*  Get inital temperature reading          */
    old_tempval = max31855_readCelsius();
    
    lcd_clrscr();
    lcd_puts("Temp:");
    dtostrf(old_tempval, -5, 2, tempstring);
    lcd_gotoxy(0,1);
    lcd_puts(tempstring);
    
    itoa(encoderValue, tempstring, 10);
    lcd_gotoxy(8,1);
    lcd_puts(tempstring);
    
    while(1)
    {
        current_tempval = max31855_readCelsius();
        
        if (current_tempval != old_tempval)
        {
            old_tempval = current_tempval;
            dtostrf(current_tempval, -5, 2, tempstring);
            lcd_clrscr();
            lcd_puts("Temp:");
            lcd_gotoxy(0,1);
            lcd_puts(tempstring);
        }
        
        if (old_encoderVal != encoderValue)
        {
            int temp_encoderVal = encoderValue/4;
            old_encoderVal = encoderValue;
            itoa(temp_encoderVal, tempstring, 10);
            lcd_gotoxy(8,1);
            lcd_puts(tempstring);
        }
        _delay_ms(100);
    }
}
#endif
// main.c tests.

/**************************************************************************/
/**************************************************************************/

/* Function Definitions */
void _LCD_backlight_init(uint8_t LED_status)
{
    /* Set Backlight Pin as output */
    DDR(BACKLIGHT_PORT) |= (1 << LCD_BACKLIGHT);
    _LCD_backWrite(LED_status);
}

void _LCD_backWrite(uint8_t LED_status)
{
    uint8_t Protect = BACKLIGHT_PORT & ~(1 << LCD_BACKLIGHT);
    BACKLIGHT_PORT = Protect | (LED_status << LCD_BACKLIGHT);
}

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/