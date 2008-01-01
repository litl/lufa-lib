/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __OTG_H__
#define __OTG_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_OTG_DEV_Initiate_HNP()         MACROS{ OTGCON |=  (1 << HNPREQ); }MACROE
			#define USB_OTG_DEV_Cancel_HNP()           MACROS{ OTGCON &= ~(1 << HNPREQ); }MACROE
			
			#define USB_OTG_HOST_Accept_HNP()          USB_OTG_DEV_InitiateHNP()
			#define USB_OTG_HOST_Reject_HNP()          USB_OTG_DEV_CancelHNP()
			
			#define USB_OTG_DEV_Initiate_SRP()         MACROS{ OTGCON |=  (1 << SRPREQ); }MACROE
			#define USB_OTG_DEV_Set_VBUS_SRP()         MACROS{ OTGCON |=  (1 << SRPSEL); }MACROE
			#define USB_OTG_DEV_Set_Data_SRP()         MACROS{ OTGCON &= ~(1 << SRPSEL); }MACROE

#endif