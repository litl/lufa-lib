/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "SCSI.h"

SCSI_Inquiry_Response_t InquiryData PROGMEM = 
	{
		DeviceType:          0,
		PeripheralQualifier: 0,
			
		Removable:           true,
			
		ANSI_Version:        0,
		ECMA_Version:        0,
		ISO_IEC_Version:     0,
			
		ResponseDataFormat:  2,
		NormACA:             false,
		TrmTsk:              false,
		AERC: false,

		AdditionalLength:    0x1F,
			
		SoftReset:           false,
		CmdQue:              false,
		Linked:              false,
		Sync:                false,
		WideBus16Bit:        false,
		WideBus32Bit:        false,
		RelAddr:             false,
		
		VendorID:            {'D',' ','C','a','m','e','r','a'},
		ProductID:           {'M','y','U','S','B',' ','M','a','s','s',' ','S','t','o','r','e'},
		RevisionID:          {'0','.','0','0'},
	};

void DecodeSCSICommand(void)
{
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
	
	bool CommandSuccess = false;

	switch (CommandBlock.SCSICommandData[0])
	{
		case SCSI_CMD_INQUIRY:
			if (!(CommandBlock.SCSICommandData[1] & ((1 << 0) | (1 << 1))) &&
			    !(CommandBlock.SCSICommandData[2]))
			{
				uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
				uint8_t* InquiryDataPtr   = (uint8_t*)&InquiryData;
				uint8_t  BytesTransferred = 0;
				
				if (AllocationLength > 96)
				  AllocationLength = 96;
				
				for (uint8_t i = 0; i < sizeof(InquiryData); i++)
				{
					if (i == AllocationLength)
					  break;
					  
					USB_Device_Write_Byte(pgm_read_byte(InquiryDataPtr++));
					BytesTransferred++;
				}
			
				while (BytesTransferred < AllocationLength)
				{
					if (!(BytesTransferred % ConfigurationDescriptor.DataInEndpoint.EndpointSize))
					  Endpoint_In_Clear();
					
					BytesTransferred++;
					USB_Device_Write_Byte(0x00);
				}
			
				Endpoint_In_Clear();
				CommandSuccess = true;
			}
			else
			{
				// TODO: Fail Command
			}			
			
			break;
	}
	
	if (CommandSuccess)
	  CommandStatus.Status = Command_Pass;
	else
	  CommandStatus.Status = Command_Fail;
}
