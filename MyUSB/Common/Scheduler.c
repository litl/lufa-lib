#include "Scheduler.h"

volatile SchedulerDelayCounterNS_t SchedulerDelayCounter;
         uint8_t                   TotalSchedulerTasks;

bool Scheduler_HasDelayElapsed(const uint16_t Delay, SchedulerDelayCounterNS_t* TaskCounter)
{
	SchedulerDelayCounterNS_t DelayCounter_LCL;
	SchedulerDelayCounterNS_t TaskCounter_LCL;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		DelayCounter_LCL = SchedulerDelayCounter;
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
	for (uint8_t i = 0; i < TotalSchedulerTasks; i++)
	{
		if (TaskList[i].TaskID == id)
		  TaskList[i].TaskStatus = run;
	}
}
