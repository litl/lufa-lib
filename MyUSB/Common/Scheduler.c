#include "Scheduler.h"

volatile SchedulerDelayCounter_t SchedulerDelayCounter;

bool Scheduler_HasDelayElapsed(uint8_t Delay, SchedulerDelayCounter_t Count)
{
	SchedulerDelayCounter_t DelayCounter_LCL = SchedulerDelayCounter;

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

void Scheduler_SetTaskMode(uint8_t id, bool run)
{
	for (uint8_t i = 0; i < TOTAL_TASKS; i++)
	{
		if (TaskList[i].TaskID == id)
		{
			TaskList[i].TaskStatus = run;
			return;
		}
	}
}
