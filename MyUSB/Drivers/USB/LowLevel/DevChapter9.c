/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "DevChapter9.h"

uint8_t USB_ConfigurationNumber;

void USB_Device_ProcessControlPacket(void)
{
	uint8_t RequestType;
	uint8_t Request;
	
	bool    RequestHandled = false;
	
	RequestType = USB_Device_Read_Byte();
	Request     = USB_Device_Read_Byte();
	
	switch (Request)
	{
		case REQ_SetAddress:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_Device_SetAddress();
				RequestHandled = true;
			}

			break;
		case REQ_SetConfiguration:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_Device_SetConfiguration();
				RequestHandled = true;
			}

			break;
		case REQ_GetConfiguration:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_Device_GetConfiguration();
				RequestHandled = true;
			}

			break;
		case REQ_GetDescriptor:
			USB_Device_GetDescriptor();
			RequestHandled = true;
			
			break;
		case REQ_GetStatus:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_DEVICETOHOST | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_Device_GetStatus(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_SetFeature:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_HOSTTODEVICE | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_Device_SetFeature(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_ClearFeature:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_HOSTTODEVICE | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_Device_ClearFeature(RequestType);
				RequestHandled = true;
			}			
			
			break;
	}

	if (RequestHandled == false)
	  RAISE_EVENT(USB_UnhandledControlPacket, Request, RequestType);
	  
	if (USB_IsSetupRecieved())
	{
		USB_Stall_Transaction();
		USB_ClearSetupRecieved();		
	}
}

void USB_Device_SetAddress(void)
{
	uint8_t NewAddress = USB_Device_Read_Byte();

	UDADDR = ((UDADDR & (1 << ADDEN)) | (NewAddress & 0b00111111));

	USB_ClearSetupRecieved();

	USB_In_Clear();
	while (!(USB_In_IsReady()));
	
	UDADDR |= (1 << ADDEN);

	return;
}

void USB_Device_SetConfiguration(void)
{
	uint8_t ConfigNum = USB_Device_Read_Byte();
	
	if (ConfigNum > CONFIGURATIONS)
		return;
	
	USB_ConfigurationNumber = ConfigNum;

	USB_ClearSetupRecieved();
	USB_In_Clear();

	RAISE_EVENT(USB_CreateEndpoints);
}

void USB_Device_GetConfiguration(void)
{
	USB_ClearSetupRecieved();
	
	USB_Device_Write_Byte(USB_ConfigurationNumber);
	
	USB_In_Clear();

	while(!(USB_Out_IsRecieved()));
	USB_Out_Clear();
}

void USB_Device_GetDescriptor(void)
{
	uint8_t  DescriptorIndex = USB_Device_Read_Byte();
	uint8_t  DescriptorType  = USB_Device_Read_Byte();
	uint16_t DescriptorLength;
	
	void*    DescriptorPointer;
	uint16_t DescriptorBytesRem;
	
	bool     SendZeroPacket  = false;
	
	if (USB_GetDescriptor(DescriptorType, DescriptorIndex, &DescriptorPointer, &DescriptorBytesRem) == false)
		return;

	USB_Device_Ignore_Word(); // Ignore language identifier

	DescriptorLength  = USB_Device_Read_Word();
	
	USB_ClearSetupRecieved();
	
	if (DescriptorLength > DescriptorBytesRem)
	{
		if ((DescriptorBytesRem % ENDPOINT_CONTROLEP_SIZE) == 0)
		  SendZeroPacket = true;
	}
	else
	{
		DescriptorBytesRem = DescriptorLength;
	}
	
	while (DescriptorBytesRem && (!(USB_Out_IsRecieved())))
	{
		while (!(USB_In_IsReady()));
		
		uint8_t BytesInPacket = 0;
		
		while (DescriptorBytesRem && (BytesInPacket++ < ENDPOINT_CONTROLEP_SIZE))
		{
			USB_Device_Write_Byte(pgm_read_byte_near(DescriptorPointer++));
			DescriptorBytesRem--;
		}
		
		USB_In_Clear();
	}
	
	if (USB_Out_IsRecieved())
	{
		USB_Out_Clear();
		return;
	}
	
	if(SendZeroPacket == true)
	{
		while (!(USB_In_IsReady()));
		USB_In_Clear();
	}

   while(!(USB_Out_IsRecieved()));
   USB_Out_Clear();
}

void USB_Device_GetStatus(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t StatusByte = 0;
	
	USB_Device_Ignore_Word(); // Ignore unused Value word
	EndpointIndex = (USB_Device_Read_Byte() & ENDPOINT_EPNUM_MASK);
	
	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_DEVICE:
			if (CONFIG_ATTRIBUTES & USB_CONFIG_ATTR_SELFPOWERED)
			  StatusByte |= FEATURE_SELFPOWERED;
			
			if (CONFIG_ATTRIBUTES & USB_CONFIG_ATTR_REMOTEWAKEUP)
			  StatusByte |= FEATURE_REMOTE_WAKEUP;
			
			break;
		case REQREC_INTERFACE:
			// No bits set, all bits currently reserved
				
			break;
		case REQREC_ENDPOINT:
			Endpoint_SelectEndpoint(EndpointIndex);

			if (!(Endpoint_IsEnabled()))
			  StatusByte = FEATURE_ENDPOINT_ENABLED;

			Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);			  
			break;
		default:
			return;
	}
	
	USB_ClearSetupRecieved();
	
	USB_Device_Write_Byte(StatusByte);
	USB_Device_Write_Byte(0x00);

	USB_In_Clear();
	
	while(!(USB_Out_IsRecieved()));
	USB_Out_Clear();

	USB_ClearSetupRecieved();
}

void USB_Device_SetFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;

	Feature       = USB_Device_Read_Byte();
	USB_Device_Ignore_Byte();
	EndpointIndex = (USB_Device_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if ((Feature == FEATURE_ENDPOINT) && (EndpointIndex != ENDPOINT_CONTROLEP))
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (!(Endpoint_IsEnabled()))
				{
					Endpoint_EnableEndpoint();
					USB_ClearSetupRecieved();
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);	
			}

			break;
	}
}

void USB_Device_ClearFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;

	Feature       = USB_Device_Read_Byte();
	USB_Device_Ignore_Byte();
	EndpointIndex = (USB_Device_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if ((Feature == FEATURE_ENDPOINT) && (EndpointIndex != ENDPOINT_CONTROLEP))
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (Endpoint_IsEnabled())
				{
					Endpoint_DisableEndpoint();
					USB_ClearSetupRecieved();
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);		
			}

			break;
	}
}
