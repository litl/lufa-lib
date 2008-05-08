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

bool Scheduler_HasDelayElapsed(const uint16_t Delay, SchedulerDelayCounter_t* const TaskCounter)
{
	SchedulerDelayCounter_t DelayCounter_LCL;
	SchedulerDelayCounter_t TaskCounter_LCL;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		DelayCounter_LCL = Scheduler_TickCounter;
	}
	
	TaskCounter_LCL = *TaskCounter;
	
	if (DelayCounter_LCL >= TaskCounter_LCL)
	{
		if ((DelayCounter_LCL - TaskCounter_LCL) >= Delay)
		{
			*TaskCounter = DelayCounter_LCL;
			return true;
		}
	}
	else
	{
		if (((MAX_DELAYCTR_COUNT - TaskCounter_LCL) + DelayCounter_LCL) >= Delay)
		{
			*TaskCounter = DelayCounter_LCL;
			return true;
		}	
	}
	
	return false;
}

void Scheduler_SetTaskMode(const TaskPtr_t Task, const bool Run)
{
	TaskEntry_t* CurrTask = &Scheduler_TaskList[0];
					
	while (CurrTask != &Scheduler_TaskList[Scheduler_TotalTasks])
	{
		if (CurrTask->Task == Task)
		{
			CurrTask->TaskStatus = Run;
			break;
		}
		
		CurrTask++;
	}
}

void Scheduler_SetGroupTaskMode(const uint8_t GroupID, const bool Run)
{
	TaskEntry_t* CurrTask = &Scheduler_TaskList[0];
					
	while (CurrTask != &Scheduler_TaskList[Scheduler_TotalTasks])
	{
		if (CurrTask->GroupID == GroupID)
		  CurrTask->TaskStatus = Run;
		
		CurrTask++;
	}
}
