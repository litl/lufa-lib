#include "Scheduler.h"

volatile SchedulerDelayCounterNS_t SchedulerDelayCounter;
         uint8_t                   TotalSchedulerTasks;

bool Scheduler_HasDelayElapsed(const uint8_t Delay, SchedulerDelayCounterNS_t Count)
{
	SchedulerDelayCounterNS_t DelayCounter_LCL = SchedulerDelayCounter;

	if (DelayCounter_LCL > Count)
	{
		if ((DelayCounter_LCL - Count) >= Delay)
		{
			Count = DelayCounter_LCL;
			return true;
		}
	}
	else
	{
		if (((MAX_DELAYCTR_COUNT - Count) + DelayCounter_LCL) >= Delay)
		{
			Count = DelayCounter_LCL;
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
