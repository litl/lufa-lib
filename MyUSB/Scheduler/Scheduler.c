#include "Scheduler.h"

volatile SchedulerDelayCounter_t Scheduler_TickCounter;
         uint8_t                 Scheduler_TotalTasks;

bool Scheduler_HasDelayElapsed(const uint16_t Delay, SchedulerDelayCounter_t* TaskCounter)
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

void Scheduler_SetTaskMode(const uint8_t id, const bool run)
{
	for (uint8_t i = 0; i < Scheduler_TotalTasks; i++)
	{
		if (Scheduler_TaskList[i].TaskID == id)
		  Scheduler_TaskList[i].TaskStatus = run;
	}
}
