/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __STDREQ_H__
#define __STDREQ_H__

	/* Public Interface - May be used in end-application: */
		/* Macros: */
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

		/* Enums: */
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

		/* Type Defines: */
			typedef struct
			{
				unsigned int Recipient : 5;
				unsigned int Type      : 2;
				unsigned int Direction : 1;
			} RequestTypeBitfield_t;

#endif
