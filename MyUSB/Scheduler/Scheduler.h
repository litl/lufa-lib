/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define TASK(name)                        void name (void)
			#define TASK_LIST                         extern TaskEntry_t Scheduler_TaskList[]; TaskEntry_t Scheduler_TaskList[] = 
			
			#define TASK_MAX_DELAY                    (MAX_DELAYCTR_COUNT - 1)

			#define TASK_RUN                          true
			#define TASK_STOP                         false
			
			#define Scheduler_Start()                 Scheduler_GoSchedule(TOTAL_TASKS);
			#define Scheduler_Init()                  Scheduler_InitScheduler(TOTAL_TASKS);

		/* Type Defines: */
			typedef void (*TaskPtr_t)(void);
			typedef uint16_t SchedulerDelayCounter_t;
			typedef struct
			{
				TaskPtr_t Task;
				bool      TaskStatus;
				uint8_t   GroupID;
			} TaskEntry_t;			

		/* Global Variables: */
			extern          TaskEntry_t               Scheduler_TaskList[];
			extern volatile uint8_t                   Scheduler_TotalTasks;
			extern volatile SchedulerDelayCounter_t   Scheduler_TickCounter;

		/* Inline Functions: */
			static inline void Scheduler_InitScheduler(const uint8_t TotalTasks)
			{
				Scheduler_TotalTasks = TotalTasks;
			}
		
			static inline void Scheduler_GoSchedule(const uint8_t TotalTasks) ATTR_NO_RETURN;
			static inline void Scheduler_GoSchedule(const uint8_t TotalTasks)
			{
				Scheduler_InitScheduler(TotalTasks);

				for (;;)
				{
					TaskEntry_t* CurrTask = &Scheduler_TaskList[0];
					
					while (CurrTask != &Scheduler_TaskList[TotalTasks])
					{
						if (CurrTask->TaskStatus == TASK_RUN)
						  CurrTask->Task();

						CurrTask++;
					}
				}
			}

			static inline void Scheduler_ResetDelay(SchedulerDelayCounter_t* const TaskCounter)
			                                        ATTR_NON_NULL_PTR_ARG(1);
			static inline void Scheduler_ResetDelay(SchedulerDelayCounter_t* const TaskCounter)
			{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					*TaskCounter = Scheduler_TickCounter;
				}
			}
		
		/* Function Prototypes: */
			bool Scheduler_HasDelayElapsed(const uint16_t Delay,
			                               SchedulerDelayCounter_t* const TaskCounter)
										   ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(2);
			void Scheduler_SetTaskMode(const TaskPtr_t Task, const bool Run);
			void Scheduler_SetGroupTaskMode(const uint8_t GroupID, const bool Run);

	/* Private Interface - For use in library only: */		
		/* Macros: */
			#define TOTAL_TASKS                       (sizeof(Scheduler_TaskList) / sizeof(TaskEntry_t))
			#define MAX_DELAYCTR_COUNT                0xFFFF
		
	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
