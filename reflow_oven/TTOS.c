/**************************************************************************/
/*
 * reflow_statemachines.c
 *
 * Created: 10/1/2015 10:53:42 PM
 *  Author: dsgiesbrecht
 *  
 *		
 *
 *
 */ 
/**************************************************************************/
/**************************************************************************/
// TODO : Create a header file.
#include "ATMEGA328P_reflow_config.h"
#include "TTOS.h"
volatile uint32_t millis = 0;
volatile uint8_t schedule_lock = 0;

void Sleep_Mode_init()
{
    set_sleep_mode(SLEEP_MODE_IDLE);
}

void Enter_Sleep_Mode(uint8_t sleep_condition)
{
    cli();
    if (sleep_condition)
    {
        schedule_lock = 1;
        
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
    /* Disable global interrupts    */
    TCCR2A = 0;
    TCCR2B = 0;
    
    /*  Set Prescaler               */
    TCCR2B |= TIMER2_PRESCALE_BITSHIFT;
    /*  Set compareA register value */
    OCR2A = TOTAL_TICKS;
    /*  Set CTC Mode                */
    TCCR2A |= TIMER2_CTC_BITSHIFT;
    /*  Enable CompareA Interrupt   */
    TIMSK2 |= (0x1 << OCIE2A);
    
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
    schedule_lock = 0;
    millis++;
}

/********** THREAD STUFF **************************************************/

task_t *head = NULL;
#define NUM_TASKS   3
task_t Tasks[NUM_TASKS] =
{
    {
        //example task, period, offset.
    },
    
    {
        //example task, period, offset.
    },
    
    {
        //example task, period, offset.
    }
    
};

#if 0
void DispatchThreads()
{
    if (schedule_lock == 0)     // if timer ISR woke up uC.
    {
        /* Run Scheduled tasks */
        for(i = 0; i < NUM_TASKS; i++)
        {
            if (Tasks[i].task_delay == 0)
            {
                Tasks[i].task_delay = Tasks[i].task_period - 1;
                (Tasks[i].task_function)();
            }
            else
            {
                Tasks[i].task_delay--;
            }
        }
    }
    
}
#endif

void DispatchThreads_linkedlist()
{
    task_t * current = head;
    while(current != NULL)
    {
        current->task_function();
        current = current->next;
    }
}

uint8_t AddThread(task_t *ptr)
{
    if(head == NULL)
    {
        head = ptr;
        ptr->next = NULL;
    }
    else
    {
        task_t *current = head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = ptr;
        ptr->next = NULL;
    }
    return 0;
    #warning "fix this return type."
}

void DeleteThread(task_t *ptr)
{
    if (head == ptr)
    {
        head = head->next;
    }
    else
    {
        task_t *current = head;
        while(current->next != ptr)
        {
            current = current->next;
        }
        current->next = current->next->next;
    }
    ptr->next = NULL;
}



/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/
