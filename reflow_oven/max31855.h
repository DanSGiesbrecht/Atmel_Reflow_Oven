/**************************************************************************/
/*
 * max31855.h
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

#ifndef MAX31855_H
#define MAX31855_H

#ifndef SPI_PORT
#define SPI_PORT    PORTB
#endif
#ifndef SPI_SCLK
#define SPI_SCLK    5
#endif
#ifndef SPI_CS
#define SPI_CS      2
#endif
#ifndef SPI_MISO
#define SPI_MISO    4
#endif

/**************************************************************************/
/*
*   SPI Inititalization.

*   Requires:
*   -   SPI PORT and pins are defined.
*   -   Hardware SPI bus available.
*   Promises:
*   -   To initialize the SPI hardware, compatible with the thermo amp.
*/
void max31855_spi_init_master(void);
/*------------------------------------------------------------------------*/
/*
*   Read 1 byte from SPI bus.

*   Requires:
*   -   SPI PORT and pins are defined.
*   -   Hardware SPI bus available.
*   Promises:
*   -   To transmit <data> argument to SPI slave (if applicable), and
*       return the received data.
*/
uint8_t max31855_spi_read(uint8_t data);
/*------------------------------------------------------------------------*/
/*
*   Read 32 bits from SPI bus.

*   Requires:
*   -   SPI PORT and pins are defined.
*   -   Hardware SPI bus available.
*   Promises:
*   -   To perform 4 SPI transfers.
*   -   To return 32 bits of data.
*/
uint32_t max31855_spi_read32(void);
/*------------------------------------------------------------------------*/
/*
*   Read temperature in Celsius from the SPI slave (thermocouple amp.)

*   Requires:
*   -   SPI PORT and pins are defined.
*   -   Hardware SPI bus available.
*   Promises:
*   -   To read a 32-bit size temperature value from the thermocouple amp.
*/
double max31855_readCelsius(void);
/*------------------------------------------------------------------------*/


#endif
/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/