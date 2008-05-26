/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "USBMode.h"
#if defined(USB_CAN_BE_DEVICE)

#define  INCLUDE_FROM_DEVCHAPTER9_C
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
		case REQ_GetStatus:
			if (((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_TYPE)) ==
			                          (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD)) &&
			     ((RequestType & CONTROL_REQTYPE_RECIPIENT) != REQREC_OTHER))
			{
				USB_Device_GetStatus(RequestType);
				RequestHandled = true;
			}			

			break;
		case REQ_ClearFeature:
		case REQ_SetFeature:
			if (((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_TYPE)) ==
			                          (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD)) &&
			     ((RequestType & CONTROL_REQTYPE_RECIPIENT) != REQREC_OTHER))
			{
				USB_Device_ClearSetFeature(Request, RequestType);
				RequestHandled = true;
			}

			break;
		case REQ_SetAddress:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_Device_SetAddress();
				RequestHandled = true;
			}

			break;
		case REQ_GetDescriptor:
			USB_Device_GetDescriptor();
			RequestHandled = true;
			
			break;
		case REQ_GetConfiguration:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
			{
				USB_Device_GetConfiguration();
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
	}

	if (!(RequestHandled))
	  RAISE_EVENT(USB_UnhandledControlPacket, Request, RequestType);
	  
	if (Endpoint_IsSetupReceived())
	{
		Endpoint_StallTransaction();
		Endpoint_ClearSetupReceived();		
	}
}

static void USB_Device_SetAddress(void)
{
	uint8_t NewAddress = Endpoint_Read_Byte();

	UDADDR = ((UDADDR & (1 << ADDEN)) | (NewAddress & 0x3F));

	Endpoint_ClearSetupReceived();

	Endpoint_ClearSetupIN();
	while (!(Endpoint_IsSetupINReady()));
	
	UDADDR |= (1 << ADDEN);

	return;
}

static void USB_Device_SetConfiguration(void)
{
	uint8_t                  ConfigNum     = Endpoint_Read_Byte();
	uint8_t                  PrevConfigNum = USB_ConfigurationNumber;
	USB_Descriptor_Device_t* DevDescriptorPtr;
	uint16_t                 DevDescriptorSize;

	if ((USB_GetDescriptor(DTYPE_Device, 0, 0, (void*)&DevDescriptorPtr, &DevDescriptorSize) == false) ||
#if defined(USE_RAM_DESCRIPTORS)
	    (ConfigNum > DevDescriptorPtr->NumberOfConfigurations))
#elif defined (USE_EEPROM_DESCRIPTORS)
	    (ConfigNum > eeprom_read_byte(&DevDescriptorPtr->NumberOfConfigurations)))
#else
	    (ConfigNum > pgm_read_byte(&DevDescriptorPtr->NumberOfConfigurations)))
#endif
	{
		return;
	}
	
	USB_ConfigurationNumber = ConfigNum;

	Endpoint_ClearSetupReceived();
	Endpoint_ClearSetupIN();

	if (!(PrevConfigNum))
	  RAISE_EVENT(USB_DeviceEnumerationComplete);

	RAISE_EVENT(USB_ConfigurationChanged);
}

void USB_Device_GetConfiguration(void)
{
	Endpoint_ClearSetupReceived();	

	Endpoint_Write_Byte(USB_ConfigurationNumber);
	
	Endpoint_ClearSetupIN();

	while (!(Endpoint_IsSetupOUTReceived()));
	Endpoint_ClearSetupOUT();
}

static void USB_Device_GetDescriptor(void)
{
	uint8_t  DescriptorIndex = Endpoint_Read_Byte();
	uint8_t  DescriptorType  = Endpoint_Read_Byte();
	uint16_t DescriptorBytesToSend;
	
	void*    DescriptorPointer;
	uint16_t DescriptorBytesRem;

	uint16_t DescriptorLanguageID = Endpoint_Read_Word_LE();
	
	bool     SendZeroPacket = false;
	
	if (!(USB_GetDescriptor(DescriptorType, DescriptorIndex, DescriptorLanguageID,
	                        &DescriptorPointer, &DescriptorBytesRem)))
	{
		return;
	}
	
	DescriptorBytesToSend = Endpoint_Read_Word_LE();
	
	Endpoint_ClearSetupReceived();
	
	if (DescriptorBytesToSend > DescriptorBytesRem)
	{
		if (!(DescriptorBytesRem % USB_ControlEndpointSize))
		  SendZeroPacket = true;
	}
	else
	{
		DescriptorBytesRem = DescriptorBytesToSend;
	}
	
	while (DescriptorBytesRem && (!(Endpoint_IsSetupOUTReceived())))
	{
		while (!(Endpoint_IsSetupINReady()));
		
		while (DescriptorBytesRem && (Endpoint_BytesInEndpoint() < USB_ControlEndpointSize))
		{
			#if defined(USE_RAM_DESCRIPTORS)
			Endpoint_Write_Byte(*((uint8_t*)DescriptorPointer++));
			#elif defined (USE_EEPROM_DESCRIPTORS)
			Endpoint_Write_Byte(eeprom_read_byte(DescriptorPointer++));			
			#else
			Endpoint_Write_Byte(pgm_read_byte(DescriptorPointer++));
			#endif
			
			DescriptorBytesRem--;
		}
		
		Endpoint_ClearSetupIN();
	}
	
	if (Endpoint_IsSetupOUTReceived())
	{
		Endpoint_ClearSetupOUT();
		return;
	}
	
	if (SendZeroPacket == true)
	{
		while (!(Endpoint_IsSetupINReady()));
		Endpoint_ClearSetupIN();
	}

   while (!(Endpoint_IsSetupOUTReceived()));
   Endpoint_ClearSetupOUT();
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
			if (USB_GetDescriptor(DTYPE_Configuration, USB_ConfigurationNumber, 0, (void*)&ConfigDescriptorPtr, &ConfigDescriptorSize) == false)
			  return;
			
#if defined(USE_RAM_DESCRIPTORS)
			ConfigAttributes = ConfigDescriptorPtr->ConfigAttributes;
#elif defined (USE_EEPROM_DESCRIPTORS)
			ConfigAttributes = eeprom_read_byte(&ConfigDescriptorPtr->ConfigAttributes);
#else
			ConfigAttributes = pgm_read_byte(&ConfigDescriptorPtr->ConfigAttributes);
#endif

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
			  StatusByte = 0x01;

			Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);			  
			break;
		default:
			return;
	}
	
	Endpoint_ClearSetupReceived();
	
	Endpoint_Write_Byte(StatusByte);
	Endpoint_Write_Byte(0x00);

	Endpoint_ClearSetupIN();
	
	while (!(Endpoint_IsSetupOUTReceived()));
	Endpoint_ClearSetupOUT();
}

static void USB_Device_ClearSetFeature(const uint8_t Request, const uint8_t RequestType)
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
						if (Request == REQ_ClearFeature)
						{
							Endpoint_ClearStall();
							Endpoint_ResetFIFO(EndpointIndex);
							Endpoint_ResetDataToggle();
						}
						else
						{
							Endpoint_StallTransaction();						
						}
					}

					Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
					Endpoint_ClearSetupReceived();
					Endpoint_ClearSetupIN();
				}
				
				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);			
			}
			
			break;
	}
}

#endif
