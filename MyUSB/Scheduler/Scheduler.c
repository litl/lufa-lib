/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Scheduler.h"

volatile SchedulerDelayCounter_t Scheduler_TickCounter;
volatile uint8_t                 Scheduler_TotalTasks;

bool Scheduler_HasDelayElapsed(const uint16_t Delay, SchedulerDelayCounter_t* const DelayCounter)
{
	SchedulerDelayCounter_t CurrentTickValue_LCL;
	SchedulerDelayCounter_t DelayCounter_LCL;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		CurrentTickValue_LCL = Scheduler_TickCounter;
	}
	
	DelayCounter_LCL = *DelayCounter;
	
	if (CurrentTickValue_LCL >= DelayCounter_LCL)
	{
		if ((CurrentTickValue_LCL - DelayCounter_LCL) >= Delay)
		{
			*DelayCounter = CurrentTickValue_LCL;
			return true;
		}
	}
	else
	{
		if (((MAX_DELAYCTR_COUNT - DelayCounter_LCL) + CurrentTickValue_LCL) >= Delay)
		{
			*DelayCounter = CurrentTickValue_LCL;
			return true;
		}	
	}
	
	return false;
}

void Scheduler_SetTaskMode(const TaskPtr_t Task, const bool TaskStatus)
{
	TaskEntry_t* CurrTask = &Scheduler_TaskList[0];
					
	while (CurrTask != &Scheduler_TaskList[Scheduler_TotalTasks])
	{
		if (CurrTask->Task == Task)
		{
			CurrTask->TaskStatus = TaskStatus;
			break;
		}
		
		CurrTask++;
	}
}

void Scheduler_SetGroupTaskMode(const uint8_t GroupID, const bool TaskStatus)
{
	TaskEntry_t* CurrTask = &Scheduler_TaskList[0];
					
	while (CurrTask != &Scheduler_TaskList[Scheduler_TotalTasks])
	{
		if (CurrTask->GroupID == GroupID)
		  CurrTask->TaskStatus = TaskStatus;
		
		CurrTask++;
	}
}
