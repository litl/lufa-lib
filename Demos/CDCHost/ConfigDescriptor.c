/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "ConfigDescriptor.h"

uint8_t ProcessConfigurationDescriptor(void)
{
	uint8_t* ConfigDescriptorData;
	uint16_t ConfigDescriptorSize;
	uint8_t  ErrorCode;
	uint8_t  FoundEndpoints = 0;
	
	/* Get Configuration Descriptor size from the device */
	if (USB_Host_GetDeviceConfigDescriptor(&ConfigDescriptorSize, NULL) != HOST_SENDCONTROL_Successful)
	  return ControlError;
	
	/* Ensure that the Configuration Descriptor isn't too large */
	if (ConfigDescriptorSize > MAX_CONFIG_DESCRIPTOR_SIZE)
	  return DescriptorTooLarge;
	  
	/* Allocate enough memory for the entire config descriptor */
	ConfigDescriptorData = alloca(ConfigDescriptorSize);

	/* Retrieve the entire configuration descriptor into the allocated buffer */
	USB_Host_GetDeviceConfigDescriptor(&ConfigDescriptorSize, ConfigDescriptorData);
	
	/* Validate returned data - ensure first entry is a configuration header descriptor */
	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return ControlError;
	
	/* Get the CDC interface from the configuration descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData, NextCDCInterface)))
	{
		/* Descriptor not found, error out */
		return NoCDCInterfaceFound;
	}

	/* Get the IN and OUT data endpoints for the CDC interface */
	while (FoundEndpoints != ((1 << CDC_NOTIFICATIONPIPE) | (1 << CDC_DATAPIPE_IN) | (1 << CDC_DATAPIPE_OUT)))
	{
		/* Fetch the next bulk endpoint from the current mass storage interface */
		if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                                                NextInterfaceCDCDataEndpoint)))
		{
			/* Descriptor not found, error out */
			return NoEndpointFound;
		}
		
		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		/* Check if the found endpoint is a interrupt or bulk type descriptor */
		if ((EndpointData->Attributes & EP_TYPE_MASK) == EP_TYPE_INTERRUPT)
		{
			/* If the endpoint is a IN type interrupt endpoint */
			if (EndpointData->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{							   
				/* Configure the notification pipe */
				Pipe_ConfigurePipe(CDC_NOTIFICATIONPIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
								   EndpointData->EndpointAddress, EndpointData->EndpointSize, PIPE_BANK_SINGLE);							   

				Pipe_SetInfiniteINRequests();
				Pipe_SetInterruptPeriod(EndpointData->PollingIntervalMS);
				
				/* Set the flag indicating that the notification pipe has been found */
				FoundEndpoints |= (1 << CDC_NOTIFICATIONPIPE);
			}
		}
		else
		{
			/* Check if the endpoint is a bulk IN or bulk OUT endpoint */
			if (EndpointData->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{
				/* Configure the data IN pipe */
				Pipe_ConfigurePipe(CDC_DATAPIPE_IN, EP_TYPE_BULK, PIPE_TOKEN_IN,
								   EndpointData->EndpointAddress, EndpointData->EndpointSize, PIPE_BANK_SINGLE);

				Pipe_SetInfiniteINRequests();
				Pipe_Unfreeze();
				
				/* Set the flag indicating that the data IN pipe has been found */
				FoundEndpoints |= (1 << CDC_DATAPIPE_IN);
			}
			else
			{
				/* Configure the data OUT pipe */
				Pipe_ConfigurePipe(CDC_DATAPIPE_OUT, EP_TYPE_BULK, PIPE_TOKEN_OUT,
								   EndpointData->EndpointAddress, EndpointData->EndpointSize, PIPE_BANK_SINGLE);
				
				Pipe_Unfreeze();
				
				/* Set the flag indicating that the data OUT pipe has been found */
				FoundEndpoints |= (1 << CDC_DATAPIPE_OUT);
			}
		}
	}

	/* Valid data found, return success */
	return SuccessfulConfigRead;
}

DESCRIPTOR_COMPARATOR(NextCDCInterface)
{
	/* Descriptor Search Comparitor Function - find next keyboard class interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		/* Check the HID descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Class    == CDC_CLASS)    &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).SubClass == CDC_SUBCLASS) &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Protocol == CDC_PROTOCOL))
		{
			return Descriptor_Search_Found;
		}
	}
	
	return Descriptor_Search_NotFound;
}

DESCRIPTOR_COMPARATOR(NextInterfaceCDCDataEndpoint)
{
	/* Descriptor Search Comparitor Function - find next interface bulk or interrupt endpoint descriptor before
	                                           next interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Endpoint)
	{
		uint8_t EndpointType = (DESCRIPTOR_CAST(CurrentDescriptor,
		                                        USB_Descriptor_Endpoint_t).Attributes & EP_TYPE_MASK);
	
		if ((EndpointType == EP_TYPE_BULK) || (EndpointType == EP_TYPE_INTERRUPT))
		  return Descriptor_Search_Found;
	}
	else if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		return Descriptor_Search_Fail;
	}

	return Descriptor_Search_NotFound;
}
