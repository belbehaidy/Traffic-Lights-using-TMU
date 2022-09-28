/*
 * M=main.c
 *
 *  Created on: Sep 24, 2022
 *      Author: Bassem El-Behaidy
 */
#include "stdTypes.h"
#include "errorState.h"

#include "TMU_int.h"
#include "DIO/DIO_int.h"
#include "LD/LD_int.h"
#include "Switch/Switch_int.h"
#include "SevSeg/SevSeg_int.h"

#include "TrafficLights_priv.h"
#include "TrafficLights_config.h"

u8 Global_u8OperationMode = NORMAL ;
u8 Global_u8TrafficStatus = TRAFFIC_STOP ;
u8 Global_u8CounterMaxValue = 30 ;
u8 Global_u8CounterValue = 30 ;

int
main(void)
{

	u8 SSeg_u8ActiveModule = COUNTER_UNITS ;
	u8 LampStatus[3]={ LD_u8OFF , LD_u8OFF , LD_u8OFF };

	LD_enuInit();
	Switch_enuInit();
	SevSeg_enuInit();
	TMU_vidInit();

	TMU_vidCreateTask(DisplayCounter		, &SSeg_u8ActiveModule	, 4   , 6 , 0 );
	TMU_vidCreateTask(CheckTrafficStatus 	, LampStatus			, 100 , 5 , 0 );
	TMU_vidCreateTask(RedLampStatus			, LampStatus+RED_LAMP	, 100 , 4 , 1 );
	TMU_vidCreateTask(YellowLampStatus		, LampStatus+YLW_LAMP	, 100 , 3 , 2 );
	TMU_vidCreateTask(GreenLampStatus		, LampStatus+GRN_LAMP	, 100 , 2 , 3 );
	TMU_vidCreateTask(CheckIncrementSwitch	, NULL					, 13  , 1 , 0 );
	TMU_vidCreateTask(CheckDecrementSwitch	, NULL					, 13  , 0 , 6 );

	TMU_vidStartScheduler();
}

void CheckTrafficStatus(void*p)
{
	u8 *ptr = (u8*)p;

	if( Global_u8OperationMode == NORMAL )
	{
		Global_u8CounterValue--;

		if( !Global_u8CounterValue )
		{
			Global_u8TrafficStatus = ( (Global_u8TrafficStatus == TRAFFIC_STOP)? TRAFFIC_GO : TRAFFIC_STOP ) ;
			Global_u8CounterValue = Global_u8CounterMaxValue;
		}


		if( Global_u8TrafficStatus == TRAFFIC_STOP && COUNTER_STATUS == COUNTER_HOLD )
		{
			*(ptr+RED_LAMP) = LD_u8ON ;
			*(ptr+YLW_LAMP) = LD_u8OFF ;
			*(ptr+GRN_LAMP) = LD_u8OFF ;
		}
		else if( Global_u8TrafficStatus == TRAFFIC_STOP && COUNTER_STATUS == COUNTER_STANDBY )
		{
			*(ptr+RED_LAMP) = LD_u8ON ;
			*(ptr+YLW_LAMP) = LD_u8ON ;
			*(ptr+GRN_LAMP) = LD_u8OFF ;
		}
		else if( Global_u8TrafficStatus == TRAFFIC_GO && COUNTER_STATUS == COUNTER_HOLD )
		{
			*(ptr+RED_LAMP) = LD_u8OFF ;
			*(ptr+YLW_LAMP) = LD_u8OFF ;
			*(ptr+GRN_LAMP) = LD_u8ON ;
		}
		else if( Global_u8TrafficStatus == TRAFFIC_GO && COUNTER_STATUS == COUNTER_STANDBY )
		{
			*(ptr+RED_LAMP) = LD_u8OFF ;
			*(ptr+YLW_LAMP) = LD_u8ON ;
			*(ptr+GRN_LAMP) = LD_u8OFF ;
		}
	}
	else
	{
		*(ptr+RED_LAMP) = LD_u8OFF;
		*(ptr+YLW_LAMP) = LD_u8OFF;
		*(ptr+GRN_LAMP) = LD_u8OFF;
	}
}

void CheckIncrementSwitch(void*p)
{
	u8 Local_u8SwitchValue;


		if( ES_OK == Switch_enuGetPressed( INC_SW , &Local_u8SwitchValue ) )
		{
			static u8 press = 0, hold = 0 , BounceDelay = BOUNCE_COUNTS ;
			static u16 SetupDelay = SETUP_COUNTS ;
			if ( (Local_u8SwitchValue == DIO_u8HIGH ) && press == 0 && hold == 0 )  //First press
			{
				if( Global_u8OperationMode == NORMAL )
				{
					Global_u8OperationMode = SETUP ;
				}
				else
				{
					if( Global_u8CounterMaxValue < COUNTER_MAX_LIMIT )
					{
						Global_u8CounterMaxValue++;
						press = 1 ;
					}
				}
			}
			else if ( (Local_u8SwitchValue == DIO_u8HIGH ) && press == 1 && hold == 0 ) //continued first press
			{
				hold = 1;
			}
			else if ( (Local_u8SwitchValue == DIO_u8LOW ) && press == 1 && hold == 1 ) // removed first press
			{
				BounceDelay--;
				if( !BounceDelay )
				{
					press = 0 ;
					hold = 0 ;
					BounceDelay = BOUNCE_COUNTS ;
				}
			}
			else if (	( Global_u8OperationMode == SETUP ) &&
						(Local_u8SwitchValue == DIO_u8LOW ) &&
						press == 0 		&&		hold == 0 ) // Unpress delay in Seup mode
			{
				SetupDelay--;
				if( !SetupDelay )
				{
					Global_u8TrafficStatus = TRAFFIC_STOP ;
					Global_u8OperationMode = NORMAL ;
					Global_u8CounterValue = Global_u8CounterMaxValue ;
					SetupDelay = SETUP_COUNTS ;
				}
			}

		}
}
void CheckDecrementSwitch(void*p)
{
	u8 Local_u8SwitchValue;

	if( ES_OK == Switch_enuGetPressed( DEC_SW , &Local_u8SwitchValue ) )
	{
		static u8 press = 0, hold = 0 , BounceDelay = BOUNCE_COUNTS ;
		static u16 SetupDelay = SETUP_COUNTS ;
		if ( (Local_u8SwitchValue == DIO_u8HIGH ) && press == 0 && hold == 0 )  //First press
		{
			if( Global_u8OperationMode == NORMAL )
			{
				Global_u8OperationMode = SETUP ;
			}
			else
			{
				if( Global_u8CounterMaxValue > COUNTER_MIN_LIMIT )
				{
					Global_u8CounterMaxValue--;
					press = 1 ;
				}
			}
		}
		else if ( (Local_u8SwitchValue == DIO_u8HIGH ) && press == 1 && hold == 0 ) //continued first press
		{
			hold = 1;
		}
		else if ( (Local_u8SwitchValue== DIO_u8LOW ) && press == 1 && hold == 1 ) // removed first press
		{
			BounceDelay--;
			if( !BounceDelay )
			{
				press = 0 ;
				hold = 0 ;
				BounceDelay = BOUNCE_COUNTS ;
			}
		}
		else if (	( Global_u8OperationMode == SETUP ) &&
					(Local_u8SwitchValue == DIO_u8LOW ) &&
					press == 0 		&&		hold == 0 ) // Unpress delay in Setup mode
		{
			SetupDelay--;
			if( !SetupDelay )
			{
				Global_u8TrafficStatus = TRAFFIC_STOP ;
				Global_u8OperationMode = NORMAL ;
				Global_u8CounterValue = Global_u8CounterMaxValue ;
				SetupDelay = SETUP_COUNTS ;
			}
		}

	}
}
void DisplayCounter(void *p)
{
	u8 *Local_u8ActiveModule = (u8*)p ;
	u8 Local_u8DisplayValue;

	if( Global_u8OperationMode == NORMAL )
	{
		Local_u8DisplayValue = Global_u8CounterValue ;
	}
	else
	{
		Local_u8DisplayValue = Global_u8CounterMaxValue ;
	}

	if( *Local_u8ActiveModule == COUNTER_UNITS )
	{
		SevSeg_enuModuleControl( COUNTER_TENS , MODULE_DISABLE);
		SevSeg_enuSetDigitValue( COUNTER_UNITS , ( Local_u8DisplayValue % 10 ) );
		SevSeg_enuModuleControl( COUNTER_UNITS , MODULE_ENABLE);
		*Local_u8ActiveModule = COUNTER_TENS ;
	}
	else
	{
		SevSeg_enuModuleControl( COUNTER_UNITS , MODULE_DISABLE);
		SevSeg_enuSetDigitValue( COUNTER_TENS , ( Local_u8DisplayValue / 10 ) );
		SevSeg_enuModuleControl( COUNTER_TENS , MODULE_ENABLE);
		*Local_u8ActiveModule = COUNTER_UNITS ;
	}
}
void RedLampStatus(void*p)
{
	static u8 Local_u8PrevStatus = LD_u8OFF ;

	u8 *LampStatus = p ;

	if( *LampStatus != Local_u8PrevStatus)
	{
		LD_enuSetState( RED_LD , *LampStatus );
		Local_u8PrevStatus = *LampStatus ;
	}
}
void YellowLampStatus(void*p)
{
	static u8 Local_u8PrevStatus = LD_u8OFF ;

	u8 *LampStatus = p ;

	if( *LampStatus != Local_u8PrevStatus)
	{
		LD_enuSetState( YLW_LD , *LampStatus );
		Local_u8PrevStatus = *LampStatus ;
	}
}
void GreenLampStatus(void*p)
{
	static u8 Local_u8PrevStatus = LD_u8OFF ;

	u8 *LampStatus = p ;

	if( *LampStatus != Local_u8PrevStatus)
	{
		LD_enuSetState( GRN_LD , *LampStatus );
		Local_u8PrevStatus = *LampStatus ;
	}
}
