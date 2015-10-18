/*
 * TTOS.h
 *
 * Created: 10/17/2015 1:31:48 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef TTOS_H_
#define TTOS_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


/* PROTOTYPES */
/*********************************************/
void Sleep_Mode_init();

/* Put the AVR uC into selected sleep mode. */
void Enter_Sleep_Mode(uint8_t sleep_condition);

/* Initialize the system tick (timer2)      */
void AT328_SysTick_Init();

/* Enable CompareA Interrupt.               */
void AT328_SysTick_Start();

/* Disable CompareA Interrupt.              */
void AT328_SysTick_Stop();

/* Unlock scheduler, increment millis,      */
/* and wake up the processor.               */
ISR(TIMER2_COMPA_vect);

/*********************************************/
/* Task Struct */
typedef struct task_t
{
    volatile void (*task_function)(void);                           /* function pointer         */
    uint32_t        task_period;                                   /* period in ticks          */
    uint32_t        task_delay;                                    /* init offset in ticks     */
    struct task_t *next;
} task_t;



/******* THREAD STUFF ***********************/
void DispatchThreads();
void DispatchThreads_linkedlist();
uint8_t AddThread(task_t *ptr);
/*********************************************/

#endif /* TTOS_H_ */

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/
