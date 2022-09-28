/*
 * TrafficLights_priv.h
 *
 *  Created on: Sep 28, 2022
 *      Author: basse
 */

#ifndef TRAFFICLIGHTS_PRIV_H_
#define TRAFFICLIGHTS_PRIV_H_

#define NORMAL				5
#define SETUP				9

#define TRAFFIC_STOP		12
#define TRAFFIC_GO			16

#define COUNTER_HOLD		25
#define COUNTER_STANDBY		28

#define COUNTER_STATUS		( (Global_u8CounterValue > 3 )? COUNTER_HOLD : COUNTER_STANDBY )

#define RED_LAMP			0
#define YLW_LAMP			1
#define GRN_LAMP			2

#define BOUNCE_COUNTS		5
#define SETUP_COUNTS		50
//#define BLINK_COUNTS		1


void DisplayCounter(void*p);
void CheckTrafficStatus(void*p);
void RedLampStatus(void*p);
void YellowLampStatus(void*p);
void GreenLampStatus(void*p);
void CheckIncrementSwitch(void*p);
void CheckDecrementSwitch(void*p);

#endif /* TRAFFICLIGHTS_PRIV_H_ */
