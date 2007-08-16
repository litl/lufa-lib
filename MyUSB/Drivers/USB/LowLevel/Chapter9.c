/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Chapter9.h"

uint8_t USB_ConfigurationNumber;

void USB_ProcessControlPacket(void)
{
	uint8_t RequestType;
	uint8_t Request;
	
	bool    RequestHandled = false;
	
	RequestType = USB_Read_Byte();
	Request     = USB_Read_Byte();
	
	switch (Request)
	{
		case REQ_SetAddress:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_CHAP9_SetAddress();
				RequestHandled = true;
			}

			break;
		case REQ_SetConfiguration:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_CHAP9_SetConfiguration();
				RequestHandled = true;
			}

			break;
		case REQ_GetConfiguration:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_CHAP9_GetConfiguration();
				RequestHandled = true;
			}

			break;
		case REQ_GetDescriptor:
			USB_CHAP9_GetDescriptor();
			RequestHandled = true;
			
			break;
		case REQ_GetStatus:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_DEVICETOHOST | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_CHAP9_GetStatus(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_SetFeature:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_HOSTTODEVICE | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_CHAP9_SetFeature(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_ClearFeature:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_RECIPIENT))
			   == (REQDIR_HOSTTODEVICE | (REQREC_INTERFACE | REQREC_ENDPOINT | REQREC_OTHER)))
			{
				USB_CHAP9_ClearFeature(RequestType);
				RequestHandled = true;
			}			
			
			break;
	}

	if (RequestHandled == false)
	  USB_User_ProcessControlPacket(Request, RequestType);
	  
	if (USB_IsSetupRecieved())
	{
		USB_Stall_Transaction();
		USB_ClearSetupRecieved();		
	}
}

void USB_CHAP9_SetAddress(void)
{
	uint8_t NewAddress = USB_Read_Byte();

	UDADDR = ((UDADDR & (1 << ADDEN)) | (NewAddress & 0b00111111));

	USB_ClearSetupRecieved();

	USB_In_Clear();
	while (!(USB_In_IsReady()));
	
	UDADDR |= (1 << ADDEN);

	return;
}

void USB_CHAP9_SetConfiguration(void)
{
	uint8_t ConfigNum = USB_Read_Byte();
	
	if (ConfigNum <= CONFIGURATIONS)
	{
		USB_ConfigurationNumber = ConfigNum;
		USB_ClearSetupRecieved();
	}
	else
	{
		USB_Stall_Transaction();
		USB_ClearSetupRecieved();
		return;
	}
	
	USB_In_Clear();

	USB_User_CreateEndpoints();
}

void USB_CHAP9_GetConfiguration(void)
{
	USB_ClearSetupRecieved();
	
	USB_Write_Byte(USB_ConfigurationNumber);
	
	USB_In_Clear();

	while(!(USB_Out_IsRecieved()));
	USB_Out_Clear();
}

void USB_CHAP9_GetDescriptor(void)
{
	uint8_t  DescriptorIndex = USB_Read_Byte();
	uint8_t  DescriptorType  = USB_Read_Byte();
	uint16_t DescriptorLength;
	
	void*    DescriptorPointer;
	uint16_t DescriptorBytesRem;
	
	bool     SendZeroPacket  = false;
	
	switch (DescriptorType)
	{
		case DTYPE_Device:
			DescriptorPointer  = (void*)&DeviceDescriptor;
			DescriptorBytesRem = sizeof(DeviceDescriptor);
			break;
		case DTYPE_Configuration:
			DescriptorPointer  = (void*)&ConfigurationDescriptor;
			DescriptorBytesRem = sizeof(ConfigurationDescriptor);
			break;			
		default:
			if (USB_GetDescriptor(DescriptorType, DescriptorIndex, &DescriptorPointer, &DescriptorBytesRem) == false)
			{
				USB_Stall_Transaction();
				USB_ClearSetupRecieved();
				return;
			}
	}

	USB_Ignore_Word(); // Ignore language identifier

	DescriptorLength  = USB_Read_Word();
	
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
			USB_Write_Byte(pgm_read_byte_near(DescriptorPointer++));
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

void USB_CHAP9_GetStatus(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t StatusByte = 0;
	
	USB_Ignore_Word(); // Ignore unused Value word
	EndpointIndex = (USB_Read_Byte() & ENDPOINT_EPNUM_MASK);
	
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
			USB_Stall_Transaction();
			USB_ClearSetupRecieved();
			
			return;
	}
	
	USB_ClearSetupRecieved();
	
	USB_Write_Byte(StatusByte);
	USB_Write_Byte(0x00);

	USB_In_Clear();
	
	while(!(USB_Out_IsRecieved()));
	USB_Out_Clear();

	USB_ClearSetupRecieved();
}

void USB_CHAP9_SetFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;
	bool    SetFeatureFailed = true;

	Feature       = USB_Read_Byte();
	USB_Ignore_Byte();
	EndpointIndex = (USB_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if ((Feature == FEATURE_ENDPOINT) && (EndpointIndex != ENDPOINT_CONTROLEP))
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (!(Endpoint_IsEnabled()))
				{
					Endpoint_EnableEndpoint();
					SetFeatureFailed = false;
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);	
			}

			break;
	}

	if (SetFeatureFailed)
	{
		USB_Stall_Transaction();
		USB_ClearSetupRecieved();
	}
}

void USB_CHAP9_ClearFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;
	bool    ClearFeatureFailed = true;

	Feature       = USB_Read_Byte();
	USB_Ignore_Byte();
	EndpointIndex = (USB_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if ((Feature == FEATURE_ENDPOINT) && (EndpointIndex != ENDPOINT_CONTROLEP))
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (Endpoint_IsEnabled())
				{
					Endpoint_DisableEndpoint();
					ClearFeatureFailed = false;
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);		
			}

			break;
	}

	if (ClearFeatureFailed)
	{
		USB_Stall_Transaction();
		USB_ClearSetupRecieved();
	}
}
