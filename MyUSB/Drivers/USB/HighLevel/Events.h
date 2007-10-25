/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef EVENTS_H
#define EVENTS_H

	// Includes:
	#include <avr/io.h>

	// Event Defines:
	#define RAISE_EVENT(e, ...)           Event_ ## e (__VA_ARGS__)
	#define EVENT_HANDLER(e)              void _GETEVENT(e)
	#define RAISES_EVENT(e)               void _GETEVENT(e)
	#define HANDLES_EVENT(e)              void _GETEVENT(e) e ## _M
	#define ALIAS_STUB(e)                 void _GETEVENT(e) __attribute__((weak, alias("__stub")))

	#define _GETEVENT(e)                  Event_ ## e

	// Event Modifiers:
	#define DEPRECATED                    __attribute__((deprecated))
 	#define NO_MODIFIER

	// Event Prototypes:
	#define USB_VBUSChange	              USB_VBUSChange (void)
	#define USB_VBUSConnect               USB_VBUSConnect (void)
	#define USB_VBUSDisconnect            USB_VBUSDisconnect (void)
	#define USB_Connect                   USB_Connect (void)
	#define USB_Disconnect                USB_Disconnect (void)
	#define USB_Suspend                   USB_Suspend (void)
	#define USB_WakeUp                    USB_WakeUp (void)
	#define USB_Reset                     USB_Reset (void)
	#define USB_UIDChange                 USB_UIDChange (void)
	#define USB_PowerOnFail               USB_PowerOnFail (const unsigned char ErrorCode)
	#define USB_HostError                 USB_HostError (const unsigned char ErrorCode)
	#define USB_UnhandledControlPacket    USB_UnhandledControlPacket (const uint8_t Request, const uint8_t RequestType)
	#define USB_CreateEndpoints           USB_CreateEndpoints (void)

	// Event Modifiers:
	#define USB_VBUSChange_M	          NO_MODIFIER
	#define USB_VBUSConnect_M             NO_MODIFIER
	#define USB_VBUSDisconnect_M          NO_MODIFIER
	#define USB_Connect_M                 NO_MODIFIER
	#define USB_Disconnect_M              NO_MODIFIER
	#define USB_Suspend_M                 NO_MODIFIER
	#define USB_WakeUp_M                  NO_MODIFIER
	#define USB_Reset_M                   NO_MODIFIER
	#define USB_UIDChange_M               NO_MODIFIER
	#define USB_PowerOnFail_M             NO_MODIFIER
	#define USB_HostError_M               NO_MODIFIER
	#define USB_UnhandledControlPacket_M  NO_MODIFIER
	#define USB_CreateEndpoints_M         NO_MODIFIER

	// Events File Prototypes:
	#ifdef INCLUDE_FROM_EVENTS_H
		static void __stub (void);

		ALIAS_STUB(USB_VBUSChange);
		ALIAS_STUB(USB_VBUSConnect);
		ALIAS_STUB(USB_VBUSDisconnect);
		ALIAS_STUB(USB_Connect);
		ALIAS_STUB(USB_Disconnect);
		ALIAS_STUB(USB_Suspend);
		ALIAS_STUB(USB_WakeUp);
		ALIAS_STUB(USB_Reset);
		ALIAS_STUB(USB_UIDChange);
		ALIAS_STUB(USB_PowerOnFail);
		ALIAS_STUB(USB_HostError);
		ALIAS_STUB(USB_UnhandledControlPacket);
		ALIAS_STUB(USB_CreateEndpoints);
	#endif

#endif
