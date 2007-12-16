/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
#define  INCLUDE_FROM_DECCHAPTER9_C
#include "DevChapter9.h"

uint8_t USB_ConfigurationNumber;

void USB_Device_ProcessControlPacket(void)
{
	uint8_t RequestType;
	uint8_t Request;
	bool    RequestHandled = false;
	
	RequestType = Endpoint_Read_Byte();
	Request     = Endpoint_Read_Byte();
	
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
			if (((RequestType & CONTROL_REQTYPE_RECIPIENT) != REQREC_OTHER) &&
			     (RequestType & CONTROL_REQTYPE_TYPE) != REQTYPE_VENDOR)
			{
				USB_Device_GetStatus(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_SetFeature:
			if (((RequestType & CONTROL_REQTYPE_RECIPIENT) != REQREC_OTHER) &&
			     (RequestType & CONTROL_REQTYPE_TYPE) != REQTYPE_VENDOR)
			{
				USB_Device_SetFeature(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_ClearFeature:
			if (((RequestType & CONTROL_REQTYPE_RECIPIENT) != REQREC_OTHER) &&
			     (RequestType & CONTROL_REQTYPE_TYPE) != REQTYPE_VENDOR)
			{
				USB_Device_ClearFeature(RequestType);
				RequestHandled = true;
			}

			break;
	}

	if (RequestHandled == false)
	  RAISE_EVENT(USB_UnhandledControlPacket, Request, RequestType);
	  
	if (Endpoint_IsSetupRecieved())
	{
		Endpoint_Stall_Transaction();
		Endpoint_ClearSetupReceived();		
	}
}

static void USB_Device_SetAddress(void)
{
	uint8_t NewAddress = Endpoint_Read_Byte();

	UDADDR = ((UDADDR & (1 << ADDEN)) | (NewAddress & 0b00111111));

	Endpoint_ClearSetupReceived();

	Endpoint_In_Clear();
	while (!(Endpoint_In_IsReady()));
	
	UDADDR |= (1 << ADDEN);

	return;
}

static void USB_Device_SetConfiguration(void)
{
	uint8_t                  ConfigNum = Endpoint_Read_Byte();
	USB_Descriptor_Device_t* DevDescriptorPtr;
	uint16_t                 DevDescriptorSize;

	if (USB_GetDescriptor(DTYPE_Device, 0, (void*)&DevDescriptorPtr, &DevDescriptorSize) == false)
	  return;
	
	if (ConfigNum > pgm_read_byte(&DevDescriptorPtr->NumberOfConfigurations))
	  return;
	
	USB_ConfigurationNumber = ConfigNum;

	Endpoint_ClearSetupReceived();
	Endpoint_In_Clear();

	RAISE_EVENT(USB_CreateEndpoints);
}

void USB_Device_GetConfiguration(void)
{
	Endpoint_ClearSetupReceived();
	
	Endpoint_Write_Byte(USB_ConfigurationNumber);
	
	Endpoint_In_Clear();

	while (!(Endpoint_Out_IsReceived()));
	Endpoint_Out_Clear();
}

static void USB_Device_GetDescriptor(void)
{
	uint8_t  DescriptorIndex = Endpoint_Read_Byte();
	uint8_t  DescriptorType  = Endpoint_Read_Byte();
	uint16_t DescriptorLength;
	
	void*    DescriptorPointer;
	uint16_t DescriptorBytesRem;
	
	bool     SendZeroPacket = false;
	
	if (USB_GetDescriptor(DescriptorType, DescriptorIndex,
	                      &DescriptorPointer, &DescriptorBytesRem) == false)
	{
		return;
	}
	
	Endpoint_Ignore_Word(); // Ignore language identifier

	DescriptorLength = Endpoint_Read_Word_LE();
	
	Endpoint_ClearSetupReceived();
	
	if (DescriptorLength > DescriptorBytesRem)
	{
		if ((DescriptorBytesRem % ENDPOINT_CONTROLEP_SIZE) == 0)
		  SendZeroPacket = true;
	}
	else
	{
		DescriptorBytesRem = DescriptorLength;
	}
	
	while (DescriptorBytesRem && (!(Endpoint_Out_IsReceived())))
	{
		while (!(Endpoint_In_IsReady()));
		
		uint8_t BytesInPacket = 0;
		
		while (DescriptorBytesRem && (BytesInPacket++ < ENDPOINT_CONTROLEP_SIZE))
		{
			Endpoint_Write_Byte(pgm_read_byte(DescriptorPointer++));
			DescriptorBytesRem--;
		}
		
		Endpoint_In_Clear();
	}
	
	if (Endpoint_Out_IsReceived())
	{
		Endpoint_Out_Clear();
		return;
	}
	
	if (SendZeroPacket == true)
	{
		while (!(Endpoint_In_IsReady()));
		Endpoint_In_Clear();
	}

   while (!(Endpoint_Out_IsReceived()));
   Endpoint_Out_Clear();
}

static void USB_Device_GetStatus(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t StatusByte = 0;
	
	USB_Descriptor_Configuration_Header_t* ConfigDescriptorPtr;
	uint16_t                               ConfigDescriptorSize;
	uint8_t                                ConfigAttributes;

	Endpoint_Ignore_Word(); // Ignore unused Value word
	EndpointIndex = (Endpoint_Read_Byte() & ENDPOINT_EPNUM_MASK);
	
	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_DEVICE:
			if (USB_GetDescriptor(DTYPE_Configuration, USB_ConfigurationNumber, (void*)&ConfigDescriptorPtr, &ConfigDescriptorSize) == false)
			  return;
			
			ConfigAttributes = pgm_read_byte(&ConfigDescriptorPtr->ConfigAttributes);

			if (ConfigAttributes & USB_CONFIG_ATTR_SELFPOWERED)
			  StatusByte  = FEATURE_SELFPOWERED;
			
			if (ConfigAttributes & USB_CONFIG_ATTR_REMOTEWAKEUP)
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
	
	Endpoint_ClearSetupReceived();
	
	Endpoint_Write_Byte(StatusByte);
	Endpoint_Write_Byte(0x00);

	Endpoint_In_Clear();
	
	while (!(Endpoint_Out_IsReceived()));
	Endpoint_Out_Clear();

	Endpoint_ClearSetupReceived();
}

static void USB_Device_SetFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;

	Feature       = Endpoint_Read_Byte();
	Endpoint_Ignore_Byte();
	EndpointIndex = (Endpoint_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if (Feature == FEATURE_ENDPOINT)
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (Endpoint_IsEnabled())
				{				
					if (EndpointIndex != ENDPOINT_CONTROLEP)
					  Endpoint_Stall_Transaction();
					
					Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
					Endpoint_ClearSetupReceived();
					Endpoint_In_Clear();
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);			
			}
	}
}

static void USB_Device_ClearFeature(const uint8_t RequestType)
{
	uint8_t EndpointIndex;
	uint8_t Feature;

	Feature       = Endpoint_Read_Byte();
	Endpoint_Ignore_Byte();
	EndpointIndex = (Endpoint_Read_Byte() & ENDPOINT_EPNUM_MASK);

	switch (RequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_ENDPOINT:
			if (Feature == FEATURE_ENDPOINT)
			{
				Endpoint_SelectEndpoint(EndpointIndex);

				if (Endpoint_IsEnabled())
				{				
					if (EndpointIndex != ENDPOINT_CONTROLEP)
					{
						Endpoint_ClearStall();
						Endpoint_ResetFIFO(EndpointIndex);
						Endpoint_ResetDataToggle();
					}
					
					Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
					Endpoint_ClearSetupReceived();
					Endpoint_In_Clear();
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);			
			}

			break;
	}
}
#endif
