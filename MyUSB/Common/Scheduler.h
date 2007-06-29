#ifndef SCHEDULER_H
#define SCHEDULER_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "FunctionAttributes.h"

	/* Public Macros */
		#define TASK(name)                        extern const TaskPtr_t TaskList[]; void name (void)
		#define TASK_LIST                         const TaskPtr_t TaskList[] = 

		#define Scheduler_Start()                 Scheduler_GoSchedule(TOTAL_TASKS);
		#define Scheduler_IncrementElapsedDelay() SchedulerDelayCounter++

	/* Private Macros */
		#define TOTAL_TASKS                       (sizeof(TaskList) / sizeof(TaskList[0]))

	/* Public Type Defines */
		typedef unsigned char SchedulerDelayCounter_t;

	/* Private Type Defines */
		typedef void (*TaskPtr_t)(void);

	/* External Variables */
		extern const TaskPtr_t TaskList[];
		extern volatile SchedulerDelayCounter_t SchedulerDelayCounter;
	
	/* Inline Functions */
		static inline void Scheduler_GoSchedule(uint8_t TotalTasks)
		{
			while (1)
			{
				for (uint8_t CurrTask = 0; CurrTask < TotalTasks; CurrTask++)
				  TaskList[CurrTask]();
			}
		}
	
	/* Function Prototypes */
		bool Scheduler_HasDelayElapsed(uint8_t Delay, SchedulerDelayCounter_t Count) ATTR_WARN_UNUSED_RESULT;

#endif
