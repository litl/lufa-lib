#ifndef SCHEDULER_H
#define SCHEDULER_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "FunctionAttributes.h"

	/* Public Macros */
		#define TASK(name)                        void name (void)
		#define TASK_LIST                         extern TaskEntry_t TaskList[]; TaskEntry_t TaskList[] = 
		#define TASK_ID_LIST                      enum TaskIDs
		
		#define TASK_MAX_DELAY                    0xFE

		#define TASK_RUN                          true
		#define TASK_STOP                         false

		#define SchedulerDelayCounter_t           static SchedulerDelayCounterNS_t
		
		#define Scheduler_Start()                 Scheduler_GoSchedule(TOTAL_TASKS);
		#define Scheduler_IncrementElapsedDelay() SchedulerDelayCounter++
		
	/* Private Macros */
		#define TOTAL_TASKS                       4//(sizeof(TaskList) / sizeof(TaskList[0]))
		#define MAX_DELAYCTR_COUNT                0xFF
		
	/* Private Type Defines */
		typedef void (*TaskPtr_t)(void);

	/* Public Type Defines */
		typedef struct
		{
			uint8_t   TaskID;
			TaskPtr_t TaskName;
			bool      TaskStatus;
		} TaskEntry_t;

	/* Private Type Defines */
		typedef unsigned char SchedulerDelayCounterNS_t;

	/* External Variables */
		extern TaskEntry_t TaskList[];
		extern volatile SchedulerDelayCounterNS_t SchedulerDelayCounter;
	
	/* Inline Functions */
		static inline void Scheduler_GoSchedule(uint8_t TotalTasks)
		{
			while (1)
			{
				for (uint8_t CurrTask = 0; CurrTask < TotalTasks; CurrTask++)
				{
					if (TaskList[CurrTask].TaskStatus == TASK_RUN)
					  TaskList[CurrTask].TaskName();
				}
			}
		}

	/* Function Prototypes */
		bool Scheduler_HasDelayElapsed(uint8_t Delay, SchedulerDelayCounterNS_t Count) ATTR_WARN_UNUSED_RESULT;
		void Scheduler_SetTaskMode(uint8_t id, bool run);

#endif
