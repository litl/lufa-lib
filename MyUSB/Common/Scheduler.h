#ifndef SCHEDULER_H
#define SCHEDULER_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "FunctionAttributes.h"

	/* Public Macros */
		#define TASK(name)                        void name (void)
		#define TASK_LIST                         extern TaskEntry_t TaskList[]; TaskEntry_t TaskList[] = 
		
		#define TASK_MAX_DELAY                    0xFE

		#define TASK_RUN                          true
		#define TASK_STOP                         false

		#define Scheduler_Start()                 Scheduler_GoSchedule(TOTAL_TASKS);
		#define Scheduler_IncrementElapsedDelay() SchedulerDelayCounter++
		
	/* Private Macros */
		#define TOTAL_TASKS                       4//(sizeof(TaskList) / sizeof(TaskList[0]))
		#define MAX_DELAYCTR_COUNT                0xFF
		
	/* Private Type Defines */
		typedef void (*TaskPtr_t)(void);

	/* Public Type Defines */
		typedef unsigned char SchedulerDelayCounter_t;
		typedef struct
		{
			uint8_t   TaskID;
			TaskPtr_t TaskPointer;
			bool      TaskStatus;
		} TaskEntry_t;

	/* External Variables */
		extern TaskEntry_t TaskList[];
		extern volatile SchedulerDelayCounter_t SchedulerDelayCounter;
	
	/* Inline Functions */
		static inline void Scheduler_GoSchedule(uint8_t TotalTasks)
		{
			while (1)
			{
				for (uint8_t CurrTask = 0; CurrTask < TotalTasks; CurrTask++)
				{
					if (TaskList[CurrTask].TaskStatus == TASK_RUN)
					  TaskList[CurrTask].TaskPointer();
				}
			}
		}

	/* Function Prototypes */
		bool Scheduler_HasDelayElapsed(uint8_t Delay, SchedulerDelayCounter_t Count) ATTR_WARN_UNUSED_RESULT;
		void Scheduler_SetTaskMode(uint8_t id, bool run);

#endif
