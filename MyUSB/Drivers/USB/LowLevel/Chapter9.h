/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef CHAPTER9_H
#define CHAPTER9_H

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "../../../Configuration/USB/Device/Descriptors.h"
		#include "../../../Configuration/USB/Device/UserDeviceRoutines.h"
		#include "../../../Configuration/USB/EventHooks.h"
		#include "LowLevel.h"

	/* Private Macros */
		#define FEATURE_ENDPOINT           0b00000000
		#define FEATURE_ENDPOINT_ENABLED   0b00000001
		#define FEATURE_REMOTE_WAKEUP      0b00000001		
		#define FEATURE_SELFPOWERED        0b00000010
		
		#define CONTROL_REQTYPE_DIRECTION  0b10000000
		#define CONTROL_REQTYPE_TYPE       0b01100000
		#define CONTROL_REQTYPE_RECIPIENT  0b00011111

		#define REQDIR_HOSTTODEVICE        (0 << 7)
		#define REQDIR_DEVICETOHOST        (1 << 7)

		#define REQTYPE_STANDARD           (0 << 5)
		#define REQTYPE_CLASS              (1 << 5)
		#define REQTYPE_VENDOR             (2 << 5)

		#define REQREC_DEVICE              (0 << 0)
		#define REQREC_INTERFACE           (1 << 0)
		#define REQREC_ENDPOINT            (2 << 0)
		#define REQREC_OTHER               (3 << 0)

	/* Private Enums */
		enum Control_ReqType_Direction
		{
			REQDIR_HostToDevice     = 0,
			REQDIR_DeviceToHost     = 1
		};
	
		enum Control_ReqType_Type
		{
			REQTYPE_Standard        = 0,
			REQTYPE_Class           = 1,
			REQTYPE_Vendor          = 2
		};
	
		enum Control_ReqType_Recipient
		{
			REQREC_Device           = 0,
			REQREC_Interface        = 1,
			REQREC_Endpoint         = 2,
			REQREC_Other            = 3
		};

		enum Control_Request
		{
			REQ_GetStatus           = 0,
			REQ_ClearFeature        = 1,
			REQ_SetFeature          = 3,
			REQ_SetAddress          = 5,
			REQ_GetDescriptor       = 6,
			REQ_SetDescriptor       = 7,
			REQ_GetConfiguration    = 8,
			REQ_SetConfiguration    = 9,
			REQ_GetInterface        = 10,
			REQ_SetInterface        = 11,
			REQ_SynchFrame          = 12
		};
		
		enum DescriptorTypes
		{
			DTYPE_Device            = 1,
			DTYPE_Configuration     = 2,
			DTYPE_String            = 3,
			DTYPE_Interface         = 4,
			DTYPE_Endpoint          = 5,
			DTYPE_DeviceQualifier   = 6,
			DTYPE_Other             = 7,
			DTYPE_InterfacePower    = 8
		};

	/* Type Defines: */
		typedef struct
		{
			unsigned int Recipient : 5;
			unsigned int Type      : 2;
			unsigned int Direction : 1;
		} RequestTypeBitfield_t;

	/* External Variables */
		extern uint8_t USB_ConfigurationNumber;

	/* Function Prototypes */
		void USB_ProcessControlPacket(void);
		void USB_CHAP9_SetAddress(void);
		void USB_CHAP9_SetConfiguration(void);
		void USB_CHAP9_GetConfiguration(void);
		void USB_CHAP9_GetDescriptor(void);
		void USB_CHAP9_GetStatus(const uint8_t RequestType);
		void USB_CHAP9_SetFeature(const uint8_t RequestType);
		void USB_CHAP9_ClearFeature(const uint8_t RequestType);

#endif
