/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __USB_EVENTS_H__
#define __USB_EVENTS_H__

	// Includes:
		#include <avr/io.h>
		
		#include "../../../Common/FunctionAttributes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define RAISE_EVENT(e, ...)           Event_ ## e (__VA_ARGS__)
			#define EVENT_HANDLER(e)              void Event_ ## e e ## _P
			#define HANDLES_EVENT(e)              EVENT_HANDLER(e) e ## _M
			#define RAISES_EVENT(e)               HANDLES_EVENT(e)
			#define ALIAS_STUB(e)                 EVENT_HANDLER(e) ATTR_WEAK ATTR_ALIAS(__stub)

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define NO_EVENT_ATTRIBUTES
		
			// Event Parameters:
			#define USB_VBUSChange_P                  (void)
			#define USB_VBUSConnect_P                 (void)
			#define USB_VBUSDisconnect_P              (void)
			#define USB_Connect_P                     (void)
			#define USB_Disconnect_P                  (void)
			#define USB_PowerOnFail_P                 (const uint8_t ErrorCode)

			#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
				#define USB_UIDChange_P               (void)
			#endif

			#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
				#define USB_HostError_P               (const uint8_t ErrorCode)
				#define USB_DeviceAttached_P          (void)
				#define USB_DeviceUnattached_P        (void)
			#endif
			
			#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
				#define USB_UnhandledControlPacket_P  (const uint8_t Request, const uint8_t RequestType)
				#define USB_CreateEndpoints_P         (void)
				#define USB_Suspend_P                 (void)
				#define USB_WakeUp_P                  (void)
				#define USB_Reset_P                   (void)
				#define USB_DeviceError_P             (const uint8_t ErrorCode)
			#endif

			// Event Modifiers (to deprecate old events):
			#define USB_VBUSChange_M	              NO_EVENT_ATTRIBUTES
			#define USB_VBUSConnect_M                 NO_EVENT_ATTRIBUTES
			#define USB_VBUSDisconnect_M              NO_EVENT_ATTRIBUTES
			#define USB_Connect_M                     NO_EVENT_ATTRIBUTES
			#define USB_Disconnect_M                  NO_EVENT_ATTRIBUTES
			#define USB_PowerOnFail_M                 NO_EVENT_ATTRIBUTES

			#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
				#define USB_UIDChange_M               
			#endif

			#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
				#define USB_HostError_M               NO_EVENT_ATTRIBUTES
				#define USB_DeviceAttached_M          NO_EVENT_ATTRIBUTES 
				#define USB_DeviceUnattached_M        NO_EVENT_ATTRIBUTES
			#endif
			
			#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
				#define USB_CreateEndpoints_M         NO_EVENT_ATTRIBUTES
				#define USB_UnhandledControlPacket_M  NO_EVENT_ATTRIBUTES
				#define USB_Suspend_M                 NO_EVENT_ATTRIBUTES
				#define USB_WakeUp_M                  NO_EVENT_ATTRIBUTES
				#define USB_Reset_M                   NO_EVENT_ATTRIBUTES
				#define USB_DeviceError_M             NO_EVENT_ATTRIBUTES
			#endif

		/* Function Prototypes: */
			#if defined(INCLUDE_FROM_EVENTS_C)
				static void __stub (void);

				ALIAS_STUB(USB_VBUSChange);
				ALIAS_STUB(USB_VBUSConnect);
				ALIAS_STUB(USB_VBUSDisconnect);
				ALIAS_STUB(USB_Connect);
				ALIAS_STUB(USB_Disconnect);
				ALIAS_STUB(USB_PowerOnFail);
				
				#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
					ALIAS_STUB(USB_UIDChange);
				#endif
				
				#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
					ALIAS_STUB(USB_HostError);
					ALIAS_STUB(USB_DeviceAttached);
					ALIAS_STUB(USB_DeviceUnattached);
				#endif

				#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
					ALIAS_STUB(USB_UnhandledControlPacket);
					ALIAS_STUB(USB_CreateEndpoints);
					ALIAS_STUB(USB_Suspend);
					ALIAS_STUB(USB_WakeUp);
					ALIAS_STUB(USB_Reset);
					ALIAS_STUB(USB_DeviceError);
				#endif

			#endif
			
#endif
