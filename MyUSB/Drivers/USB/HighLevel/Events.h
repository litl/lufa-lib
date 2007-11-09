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
			// Event Parameters:
			#define USB_VBUSChange_P              (void)
			#define USB_VBUSConnect_P             (void)
			#define USB_VBUSDisconnect_P          (void)
			#define USB_Connect_P                 (void)
			#define USB_Disconnect_P              (void)
			#define USB_Suspend_P                 (void)
			#define USB_WakeUp_P                  (void)
			#define USB_Reset_P                   (void)
			#define USB_UIDChange_P               (void)
			#define USB_PowerOnFail_P             (const unsigned char ErrorCode)
			#define USB_HostError_P               (const unsigned char ErrorCode)
			#define USB_DeviceError_P             (const unsigned char ErrorCode)
			#define USB_UnhandledControlPacket_P  (const uint8_t Request, const uint8_t RequestType)
			#define USB_CreateEndpoints_P         (void)
			#define USB_DeviceAttached_P          (void)
			#define USB_DeviceUnattached_P        (void)

			// Event Modifiers (to deprecate old events):
			#define USB_VBUSChange_M	          
			#define USB_VBUSConnect_M             
			#define USB_VBUSDisconnect_M          
			#define USB_Connect_M                 
			#define USB_Disconnect_M              
			#define USB_Suspend_M                 
			#define USB_WakeUp_M                  
			#define USB_Reset_M                   
			#define USB_UIDChange_M               
			#define USB_PowerOnFail_M             
			#define USB_HostError_M               
			#define USB_DeviceError_M             
			#define USB_UnhandledControlPacket_M  
			#define USB_CreateEndpoints_M         
			#define USB_DeviceAttached_M          
			#define USB_DeviceUnattached_M        

		/* Function Prototypes: */
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
				ALIAS_STUB(USB_DeviceError);
				ALIAS_STUB(USB_UnhandledControlPacket);
				ALIAS_STUB(USB_CreateEndpoints);
				ALIAS_STUB(USB_DeviceAttached);
				ALIAS_STUB(USB_DeviceUnattached);
			#endif
			
#endif
