/*
 * reflow_HeaterControl.h
 *
 * Created: 12/6/2015 2:20:24 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef REFLOW_HEATERCONTROL_H_
#define REFLOW_HEATERCONTROL_H_


/* Set up the heater port/pin. */
void Heater_Init();

/* Turn the heater on or off. */
typedef enum { OFF = 0, ON } HeaterSetting;
void HeaterSet(HeaterSetting status);

void Fan_Init();

typedef enum { OFF = 0, ON } FanSetting;
void FanSet(FanSetting status);

#endif /* REFLOW_HEATERCONTROL_H_ */