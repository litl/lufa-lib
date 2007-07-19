#ifndef CHAPTER9_H
#define CHAPTER9_H

	/* Includes: */
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	#include "../../../Configuration/USB/Descriptors.h"
	#include "../../../Configuration/USB/EventHooks.h"
	#include "LowLevel.h"

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
			unsigned int Direction : 1;
			unsigned int Type      : 2;
			unsigned int Recipient : 4;
		} RequestTypeBitfield_t;

	/* Function Prototypes */
		void USB_ProcessControlPacket(void);
		void USB_CHAP9_SetAddress(void);
		void USB_CHAP9_SetConfiguration(void);
		void USB_CHAP9_GetConfiguration(void);
		void USB_CHAP9_GetDescriptor(void);
#endif
