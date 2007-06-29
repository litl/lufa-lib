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
		if ((Count - DelayCounter_LCL) >= Delay)
		{
			Count = DelayCounter_LCL;
			return true;
		}	
	}
	
	return false;
}
