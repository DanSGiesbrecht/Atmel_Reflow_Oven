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

#include "ATMEGA328P_reflow_config.h"
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
/* for testing*/
//#include <stdio.h>
#define FINAL_MAIN_
//#define MAIN_TEST_1_
//#define MAIN_TEST_2_
//#define MAIN_TEST_3_



/**************************************************************************/
/*      GLOBAL VARIABLES                                                  */

/***************************************/
/*       MASTER CONTROL REGISTER       */
volatile uint16_t    main_MASTER_CTRL_FLAG = 0;
/************************************/

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
/*------------------------------------------------------------------------*/

/* FINAL MAIN */
#ifdef FINAL_MAIN_
int main()
{
/*------------------------------------------------------------------------*/
/*      Initialize System Hardware.                                       */
/*------------------------------------------------------------------------*/
    ATMEGA328_init();
    /* Allow peripherals to stabilize. */
    _delay_ms(200);

/*------------------------------------------------------------------------*/
/*      Initialize State Machines.                                        */
/*------------------------------------------------------------------------*/
    MeasureTemp_Initialize();
    HeaterPWM_Initialize();
    reflow_Initialize();
/*------------------------------------------------------------------------*/
/*      Start the System Tick.                                            */
/*------------------------------------------------------------------------*/
    AT328_SysTick_Start();
    
    while(1)
    {
        /* Run state Machines */
        MeasureTemp_ActiveState();
        HeaterPWM_ActiveState();
        reflow_ActiveState();
        
        /********** GO TO SLEEP **************/
        /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
        /* The fan pin and connected LED are being used as the heartbeat,
        because there is no fan connected yet, and there is no other unused onboard LED
        */
        // Turn off LED before sleeping.
        FanSet(FAN_OFF);
        // sleep for remainder of 1ms.
        Enter_Sleep_Mode(1);
        // Turn on LED after sleeping.
        FanSet(FAN_ON);
        /************************************/
        
    }// end while(1);
    
    return 0;
}// end main();
#endif

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
// main.c tests.
#ifdef MAIN_TEST_1_
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
    /**************************************************************************/
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

#ifdef MAIN_TEST_2_

extern uint8_t *profile1_ptr[];

int main()
{
    Profile test_Profile;
    //char tempstring[16];
    //uint8_t tempint;
    //char tempchar[2];
    
    lcd_init(LCD_DISP_ON);
    _LCD_backlight_init(0);
    /* Flash Backlight */
    _delay_ms(500);
    _LCD_backWrite(1);
    _delay_ms(500);
    _LCD_backWrite(0);
    _delay_ms(500);
    _LCD_backWrite(1);
    _delay_ms(600);
    
    lcd_clrscr();
    
    update_Profile(&test_Profile, profile1_ptr, _Name);
    
    lcd_puts("hello world");
    _delay_ms(1000);
    lcd_gotoxy(0,1);
  
    lcd_puts(test_Profile.name);    // this works well.
    
    return 0;
}
#endif

#ifdef MAIN_TEST_3_
int main()
{
/*------------------------------------------------------------------------*/
/*      Initialize System Hardware.                                       */
/*------------------------------------------------------------------------*/
    ATMEGA328_init();
    /* Allow peripherals to stabilize. */
    _delay_ms(200);

/*------------------------------------------------------------------------*/
/*      Initialize State Machines.                                        */
/*------------------------------------------------------------------------*/
    MeasureTemp_Initialize();
    //EncoderDebounce_Initialize();
    HeaterPWM_Initialize();

/*------------------------------------------------------------------------*/
/*      Start the System Tick.                                            */
/*------------------------------------------------------------------------*/
    AT328_SysTick_Start();
    
    double currentTemp = 0;
    char tempstring[6];
    lcd_clrscr();
    while(1)
    {
        /* Run state Machines */
        MeasureTemp_ActiveState();
        //EncoderDebounce_ActiveState();
        HeaterPWM_ActiveState();
        
        /* Update LCD */
        main_MASTER_CTRL_FLAG |= TEMP_REQUEST;
        
        if (main_MASTER_CTRL_FLAG & TEMP_IS_VALID)
        {
            lcd_gotoxy(0,0);
            currentTemp = MeasureTemp_ReadAverage();
            dtostrf(currentTemp, -5, 1, tempstring);
            lcd_puts(tempstring);
        }
        
        lcd_gotoxy(0,1);
        if (WasEncoderPressed())
        {
            lcd_putc('P');
            HeaterPercent(50); // test temperature at 50%. flags must also be set. (below).
        }
        else
        {
            lcd_putc(' ');
        }
        lcd_gotoxy(1, 1);
        if (WasEncoderTurnedLEFT())
        {
            lcd_putc('L');
        }
        else
        {
            lcd_putc(' ');
        }
        //lcd_gotoxy(0, 1);
        if (WasEncoderTurnedRIGHT())
        {
            lcd_putc('R');
        }
        else
        {
            lcd_putc(' ');
        }
        
        /* TEST THE TEMPERATURE OPERATING AT 50%        */
        main_MASTER_CTRL_FLAG |= (HEATER_POWERED | REFLOW_IN_PROGRESS);
        
        /* Test timing */
        static uint16_t milSecTick = 0;
        if (milSecTick == 500)
        {
            static char c = '0';
            lcd_gotoxy(10,1);
            lcd_putc(c);
            c++;
            if (c > '9')
                c = '0';
            milSecTick = 0;
        }
        milSecTick++;
        
        /********** GO TO SLEEP **************/
        /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
        /* The fan pin and connected LED are being used as the heartbeat,
        because there is no fan connected yet, and there is no other unused onboard LED
        */
        // Turn off LED before sleeping.
        FanSet(FAN_OFF);
        // sleep for remainder of 1ms.
        Enter_Sleep_Mode(1);
        // Turn on LED after sleeping.
        FanSet(FAN_ON);
        /************************************/
        
    }// end while(1);
    
    return 0;
}// end main();
#endif




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

void ATMEGA328_init()
{
    cli();
    /*  Initialize the peripherals      */
    
    /*  LCD         */
    lcd_init(LCD_DISP_ON);
    _LCD_backlight_init(BACKLIGHT_ON);
    
    /*  MAX31855    */
    max31855_spi_init_master();
    /*  ENCODER     */
    encoder_pininit();
    /* HEATER */
    Heater_Init();
    /* FAN    */
    Fan_Init();
    /* system timer */
    AT328_SysTick_Init();
    
    Sleep_Mode_init();
    
    /* Interrupts   */
    // Or do this inside the specific source files?
    
    sei();
}


void Sleep_Mode_init()
{
    set_sleep_mode(SLEEP_MODE_IDLE);
}

void Enter_Sleep_Mode(uint8_t sleep_condition)
{
     cli();
     if (sleep_condition)
     {
         sleep_enable();
         sei();
         sleep_cpu();
         sleep_disable();
     }
     sei();
}

void AT328_SysTick_Init()
{
    /* Systick = 0.001s */
    TCCR2A = 0;
    TCCR2B = 0;
    
    /*  Set Prescaler               */
    TCCR2B |= TIMER2_PRESCALE_BITSHIFT;    
    /*  Set compareA register value */
    OCR2A = TOTAL_TICKS;
    /*  Set CTC Mode                */
    TCCR2A |= TIMER2_CTC_BITSHIFT;
    
    /*  Enable CompareA Interrupt (this is done in a separate funciton.  */
    //TIMSK2 |= (0x1 << OCIE2A);
 
}

void AT328_SysTick_Start()
{
    /*  Enable CompareA Interrupt   */
    TIMSK2 |= (0x1 << OCIE2A);
}

void AT328_SysTick_Stop()
{
    /*  Disable CompareA Interrupt */
    TIMSK2 &= ~(0x1 << OCIE2A);
    /* For use with Deep Sleep Mode */
}

ISR(TIMER2_COMPA_vect)
{
    //millis++; TODO: clean up later.
}
/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/