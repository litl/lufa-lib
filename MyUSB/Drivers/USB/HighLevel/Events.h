/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef EVENTS_H
#define EVENTS_H

	// Event Defines:
	#define RAISE_EVENT(e, ...)    Event_ ## e (__VA_ARGS__)
	#define EVENT_HANDLER(e)       void e
	#define RAISES_EVENT(e)        void e
	#define HANDLES_EVENT(e)       void e e ## _M
	#define ALIAS_STUB(e)          void e __attribute__((weak, alias("__stub")))

	// Event Modifiers:
	#define DEPRECATED             __attribute__((deprecated))
 	#define NO_MODIFIER

	// Event Prototypes:
	#define OnVBUSChange	       Event_OnVBUSChange (void)
	#define OnVBUSConnect          Event_OnVBUSConnect (void)
	#define OnVBUSDisconnect       Event_OnVBUSDisconnect (void)
	#define OnUSBConnect           Event_OnUSBConnect (void)
	#define OnUSBDisconnect        Event_OnUSBDisconnect (void)
	#define OnSuspend              Event_OnSuspend (void)
	#define OnWakeUp               Event_OnWakeUp (void)
	#define OnReset                Event_OnReset (void)
	#define OnUIDChange            Event_OnUIDChange (void)
	#define PowerOnFail            Event_PowerOnFail (unsigned char ErrorCode)
	#define HostError              Event_HostError (unsigned char ErrorCode)

	// Event Modifiers:
	#define OnVBUSChange_M	       NO_MODIFIER
	#define OnVBUSConnect_M        NO_MODIFIER
	#define OnVBUSDisconnect_M     NO_MODIFIER
	#define OnUSBConnect_M         NO_MODIFIER
	#define OnUSBDisconnect_M      NO_MODIFIER
	#define OnSuspend_M            NO_MODIFIER
	#define OnWakeUp_M             NO_MODIFIER
	#define OnReset_M              NO_MODIFIER
	#define OnUIDChange_M          NO_MODIFIER
	#define PowerOnFail_M          NO_MODIFIER
	#define HostError_M            NO_MODIFIER

	// Events File Prototypes:
	#ifdef INCLUDE_FROM_EVENTS_H
		static void __stub (void);

		ALIAS_STUB(OnVBUSChange);
		ALIAS_STUB(OnVBUSConnect);
		ALIAS_STUB(OnVBUSDisconnect);
		ALIAS_STUB(OnUSBConnect);
		ALIAS_STUB(OnUSBDisconnect);
		ALIAS_STUB(OnSuspend);
		ALIAS_STUB(OnWakeUp);
		ALIAS_STUB(OnReset);
		ALIAS_STUB(OnUIDChange);
		ALIAS_STUB(PowerOnFail);
		ALIAS_STUB(HostError);
	#endif

#endif
