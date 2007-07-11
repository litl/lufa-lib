#ifndef SCHEDULER_H
#define SCHEDULER_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "Atomic.h"
		#include "FunctionAttributes.h"

	/* Public Macros */
		#define TASK(name)                        void name (void)
		#define TASK_LIST                         extern TaskEntry_t TaskList[]; TaskEntry_t TaskList[] = 
		#define TASK_ID_LIST                      enum TaskIDs
		
		#define TASK_MAX_DELAY                    (MAX_DELAYCTR_COUNT - 1)

		#define TASK_RUN                          true
		#define TASK_STOP                         false

		#define SchedulerDelayCounter_t           static SchedulerDelayCounterNS_t
		
		#define Scheduler_Start()                 Scheduler_GoSchedule(TOTAL_TASKS);
		#define Scheduler_IncrementElapsedDelay() SchedulerDelayCounter++
		
	/* Private Macros */
		#define TOTAL_TASKS                       (sizeof(TaskList) / sizeof(TaskEntry_t))
		#define MAX_DELAYCTR_COUNT                0xFFFF
		
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
		typedef uint16_t SchedulerDelayCounterNS_t;

	/* External Variables */
		extern          TaskEntry_t               TaskList[];
		extern volatile SchedulerDelayCounterNS_t SchedulerDelayCounter;
		extern          uint8_t                   TotalSchedulerTasks;
	
	/* Inline Functions */
		static inline void Scheduler_GoSchedule(const uint8_t TotalTasks)
		{
			TotalSchedulerTasks = TotalTasks;
		
			while (1)
			{
				for (uint8_t CurrTask = 0; CurrTask < TotalTasks; CurrTask++)
				{
					if (TaskList[CurrTask].TaskStatus == TASK_RUN)
					  TaskList[CurrTask].TaskName();
				}
			}
		}

		static inline void Scheduler_ResetDelay(SchedulerDelayCounterNS_t* TaskCounter)
		{
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				*TaskCounter = SchedulerDelayCounter;
			}
		}
		
	/* Function Prototypes */
		bool Scheduler_HasDelayElapsed(const uint16_t Delay, SchedulerDelayCounterNS_t* TaskCounter) ATTR_WARN_UNUSED_RESULT;
		void Scheduler_SetTaskMode(const uint8_t id, const bool run);

#endif
