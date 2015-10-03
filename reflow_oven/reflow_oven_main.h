/**************************************************************************/
/*
 * reflow_oven_main.h
 *
 * Created: 10/03/2015 3:48:26 PM
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

#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "max31855.h"
#include "quad_encoder.h"