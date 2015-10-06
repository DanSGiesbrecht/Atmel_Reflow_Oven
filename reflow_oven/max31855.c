/**************************************************************************/
/*
 * max31855.c
 *
 *   Created: 7/26/2015 9:22:00 PM
 *    Author: dsgiesbrecht
 *   Version: v1.0 - driver is functional!
 *  
 * Description:
 *      Driver for the Adafruit MAX31855K K-type thermocouple amplifier.
 *      Requires:
 *      -   Hardware SPI interface.
 *      Promises:
 *      -   Functions to read 32-bit data from the thermo amplifier over
 *          the SPI bus.
 * Important Notes:
 *      @   This library is based on the Adafruit MAX31855 thermocouple
 *          library: https://www.adafruit.com/products/269
 *                   https://github.com/adafruit/Adafruit-MAX31855-library
 *          It is not nearly as complete or functional as the Adafruit
 *          library.  Be aware that not all functions are supported.
 *      @   MAX31855K needs 500ms to stabilize.
 *      @   MAX31855K is READ-ONLY (No MOSI pin required).
 *      @   This library is originally intended for use with the ATMega328P.
 *          Be sure that all registers are configured for the hardware used.
 *
 */
/**************************************************************************/
/**************************************************************************/

//#include <inttypes.h>
//#include <avr/io.h>
//#include <avr/pgmspace.h>
//#include <util/delay.h>

#include "ATMEGA328P_reflow_config.h"
#include "max31855.h"

/* MACROS */
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

void max31855_spi_init_master(void)
{
    /* Set MISO, SCLK and CS as outputs */
    DDR(SPI_PORT) |= (1 << SPI_MISO) | (1 << SPI_SCLK) | (1 << SPI_CS);
    /* Set MISO, SCLK LOW; CS HIGH      */
    SPI_PORT &= ~((1 << SPI_MISO) | (1 << SPI_SCLK));
    SPI_PORT |=  (1 << SPI_CS);
    /* Set SPCR control register bits  */
    SPCR |= (1 << SPE) | (1 << MSTR);
    /* DORD,CPOL,CPHA=0                */
    /* MSB FIRST, SPI_MODE:0           */
    SPCR &= ~((1 << DORD) | (1 << CPOL) | (1 << CPHA));
    /* SPI CLK DIV4: SPR1=0, SPR0=0      */
    SPCR &= ~((1 << SPR1) | (1 << SPR0));
}

/*------------------------------------------------------------------------*/

uint8_t max31855_spi_read(uint8_t data)
{
    /* Load 1 byte of data into SPDR reg. to initiate transfer */
    SPDR = data;
    /* Wait until transmission is complete.                    */
    while(!(SPSR & (1 << SPIF) ));
    return (SPDR);
}

/*------------------------------------------------------------------------*/

uint32_t max31855_spi_read32(void)
{
    int i;
    // easy conversion of four uint8_ts to uint32_t
    union bytes_to_uint32
    {
        uint8_t bytes[4];
        uint32_t integer;
    } buffer;
    
    /*  SPI_CS set to LOW       */
    SPI_PORT &= ~(1 << SPI_CS);
    _delay_ms(1);
    
    /*  Perform 4 SPI reads, store in buffer. */
    for (i=3; i>=0; i--)
    {
        buffer.bytes[i] = max31855_spi_read(0x00);
    }
    
    /*  SPI_CS set to HIGH     */
    SPI_PORT |= (1 << SPI_CS);
    
    return buffer.integer;
}

/*------------------------------------------------------------------------*/

double max31855_readCelsius(void)
{
    int32_t v;
    double centigrade;
    
    v = max31855_spi_read32();
    
    if (v & 0x7)
    {
        return NAN;
    }
    
    if (v & 0x80000000)
    {
        /*  Negative value, drop the lower 18 bits and explicitly
            extend sign bits.                                   */
        v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
    }
    else
    {
        /*  Positive value, just drop the lower 18 bits.        */
        v >>= 18;
    }
    
    centigrade = v;
    
    /*  LSB = 0.25 degrees C    */
    centigrade *= 0.25;
    return centigrade;
}

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/