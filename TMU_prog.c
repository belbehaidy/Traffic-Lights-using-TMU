/*
 * TMU_prog.c
 *
 *  Created on: Sep 24, 2022
 *      Author: Bassem El-Behaidy
 */
#include "stdTypes.h"


#include "TMU_config.h"
#include "TMU_priv.h"

#if MAX_TASKS >0 && MAX_TASKS<=10
#else
#error num of tasks is invalid
#endif

#if TIMER_CHANNEL == TIMER0 || TIMER_CHANNEL == TIMER2
static u8 TMU_u8ISRNum;
static u8 TMU_u8ISRCount;
static u8 TMU_u8Preload;
#endif

static TCB_t All_Tasks[MAX_TASKS];
static u32 TMU_u32OsTicks;

void TMU_vidInit(void)
{
#if TIMER_CHANNEL == TIMER0
	TCCR0 = 0x0D;
	#if OS_TICK >0 && OS_TICK <= 16
		OCR0 = (u8)((OS_TICK * CPU_FREQ)/1024ul);

		TMU_u8ISRNum = 1;
		TMU_u8ISRCount = 1;
		TIMSK |= (1<<1);
	#elif OS_TICK <=150 && OS_TICK > 0
		u8 GCF = 16;
		for (;GCF>0;GCF--)
			if (OS_TICK%GCF == 0)
				break;
		if (GCF > 4)
		{
			OCR0 = (u8)((GCF * CPU_FREQ )/ 1024ul);
			TMU_u8ISRNum = (OS_TICK/GCF);
			TMU_u8ISRCount = TMU_u8ISRNum;
			TIMSK |= (1<<1);
		}
		else
		{
			TCCR0 = 0x05;
			u32 Counts = (OS_TICK * CPU_FREQ)/1024ul;
			TMU_u8ISRNum = (Counts + 255)/256;
			TMU_u8ISRCount = TMU_u8ISRNum;
			TMU_u8Preload = 256 - (Counts%256);
			TCNT0 = TMU_u8Preload;
			TIMSK |= (1<<0);
		}
	#else
	#error os tick value is invalid
	#endif
#elif TIMER_CHANNEL == TIMER1

	TCCR1A = 0;
	TCCR1B = 0x0D;
	#if OS_TICK >0 && OS_TICK<= 150
		u16 Counts  = (u16)((OS_TICK * CPU_FREQ)/1024ul);

		OCR1AH = (u8)(Counts>>8);
		OCR1AL = (u8)(Counts);

		TIMSK |= (1<<4);
	#else
	#error os tick value is invalid
	#endif
#elif TIMER_CHANNEL == TIMER2
	TCCR2 = 0x0F;
	#if OS_TICK >0 && OS_TICK <= 16
		OCR2 = (u8)((OS_TICK * CPU_FREQ)/1024ul);

		TMU_u8ISRNum = 1;
		TMU_u8ISRCount = 1;
		TIMSK |= (1<<7);
	#elif OS_TICK <=150 && OS_TICK > 0
		u8 GCF = 16;
		for (;GCF>0;GCF--)
			if (OS_TICK%GCF == 0)
				break;
		if (GCF > 4)
		{
			OCR2 = (u8)((GCF * CPU_FREQ )/ 1024ul);
			TMU_u8ISRNum = (OS_TICK/GCF);
			TMU_u8ISRCount = TMU_u8ISRNum;
			TIMSK |= (1<<7);
		}
		else
		{
			TCCR2 = 0x07;
			u32 Counts = (OS_TICK * CPU_FREQ)/1024ul;
			TMU_u8ISRNum = (Counts + 255)/256;
			TMU_u8ISRCount = TMU_u8ISRNum;
			TMU_u8Preload = 256 - (Counts%256);
			TCNT2 = TMU_u8Preload;
			TIMSK |= (1<<6);
		}
	#else
	#error os tick value is invalid
	#endif
#else
#error Timer channel configuration is invalid
#endif
}


void TMU_vidCreateTask( void( *Copy_pFunAppFun )( void* ) , void *Copy_pvidParameter, u16 Copy_u16Periodicity , u8 Copy_u8Priority , u8 Copy_u8Offset)
{
	if (Copy_pFunAppFun != NULL && Copy_u8Priority < MAX_TASKS)
	{
		All_Tasks[ Copy_u8Priority ].pFun = Copy_pFunAppFun;
		All_Tasks[ Copy_u8Priority ].parameter = Copy_pvidParameter;
		All_Tasks[ Copy_u8Priority ].Periodicity = Copy_u16Periodicity;
		All_Tasks[ Copy_u8Priority ].state = READY;
		All_Tasks[ Copy_u8Priority ].offset = Copy_u8Offset;
	}
}

void TMU_vidStartScheduler(void)
{
	u32 Temp = 0;
	asm ("SEI");
	while(1)
	{
		if (TMU_u32OsTicks > Temp)
		{
			Temp = TMU_u32OsTicks;
			//for (u8 i= 0 ; i<MAX_TASKS ; i++)
			for(s8 i = MAX_TASKS-1 ; i>=0 ; i--)
			{
				if (All_Tasks[i] . pFun != NULL &&
					All_Tasks[i].state == READY &&
					TMU_u32OsTicks % All_Tasks[i].Periodicity == All_Tasks[i].offset )
				{
					All_Tasks[i].pFun (All_Tasks[i].parameter);
				}
			}
		}
	}
}


void TMU_vidDeleteTask(u8 Copy_u8Priority)
{
	if (Copy_u8Priority <MAX_TASKS)
	{
		All_Tasks[Copy_u8Priority].pFun = NULL;
		All_Tasks[ Copy_u8Priority].state = KILLED;
	}
}

void TMU_vidPauseTask(u8 Copy_u8Priority)
{
	if (Copy_u8Priority <MAX_TASKS)
	{
		All_Tasks[Copy_u8Priority].state = PAUSED;
	}
}

void TMU_vidResumeTask(u8 Copy_u8Priority)
{
	if (Copy_u8Priority <MAX_TASKS)
	{
		All_Tasks[Copy_u8Priority].state = READY;
	}
}





#if TIMER_CHANNEL == TIMER0

void __vector_10 (void)__attribute__((signal));
void __vector_10 (void)
{
	TMU_u8ISRCount--;
	if ( ! TMU_u8ISRCount)
	{
		TMU_u32OsTicks++;

		TMU_u8ISRCount = TMU_u8ISRNum;
	}
}

void __vector_11 (void)__attribute__((signal));
void __vector_11 (void)
{
	TMU_u8ISRCount--;
	if ( ! TMU_u8ISRCount)
	{
		TCNT0 = TMU_u8Preload;

		TMU_u32OsTicks++;

		TMU_u8ISRCount = TMU_u8ISRNum;
	}
}

#elif TIMER_CHANNEL == TIMER1

void __vector_7 (void)__attribute__((signal));
void __vector_7 (void)
{
	TMU_u32OsTicks++;
}

#elif TIMER_CHANNEL == TIMER2
void __vector_4 (void)__attribute__((signal));
void __vector_4 (void)
{
	TMU_u8ISRCount--;
	if ( ! TMU_u8ISRCount)
	{
		TMU_u32OsTicks++;
		TMU_u8ISRCount = TMU_u8ISRNum;
	}
}

void __vector_5 (void)__attribute__((signal));
void __vector_5 (void)
{
	TMU_u8ISRCount--;
	if ( ! TMU_u8ISRCount)
	{
		TCNT2 = TMU_u8Preload;

		TMU_u32OsTicks++;

		TMU_u8ISRCount = TMU_u8ISRNum;
	}
}

#endif
