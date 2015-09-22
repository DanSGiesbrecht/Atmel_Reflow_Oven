/**************************************************************************/
/*
 * reflow_oven.c
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
#include "lcd.h"
#include "max31855.h"
#include "quad_encoder.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
/**************************************************************************/
/*      GLOBAL VARIABLES                                                  */    
extern volatile int encoderValue;

/**************************************************************************/
/*         TO DO:                                                         */
/*        @     
*/

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


/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/