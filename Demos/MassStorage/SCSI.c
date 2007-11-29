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
		AERC:                false,

		AdditionalLength:    0x1F,
			
		SoftReset:           false,
		CmdQue:              false,
		Linked:              false,
		Sync:                false,
		WideBus16Bit:        false,
		WideBus32Bit:        false,
		RelAddr:             false,
		
		VendorID:            {'M','y','U','S','B',' ',' ',' '},
		ProductID:           {'D','a','t','a','f','l','a','s','h',' ','D','i','s','k',' ',' '},
		RevisionID:          {'0','.','0','0'},
	};
	
SCSI_Request_Sense_Response_t SenseData =
	{
		Valid:               true,
		ReponseCode:         0x48,
		AdditionalLength:    0x0A,
	};

void SCSI_DecodeSCSICommand(void)
{
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
	
	bool CommandSuccess = false;

	printf_P(PSTR("SC: %d\r\n"), CommandBlock.SCSICommandData[0]); // DEBUG

	switch (CommandBlock.SCSICommandData[0])
	{
		case SCSI_CMD_INQUIRY:
			CommandSuccess = SCSI_Command_Inquiry();			
			break;
		case SCSI_CMD_REQUEST_SENSE:
			CommandSuccess = SCSI_Command_Request_Sense();	
			break;
		case SCSI_CMD_TEST_UNIT_READY:
			CommandSuccess = true;
			CommandBlock.Header.DataTransferLength = 0;
			break;
		case SCSI_CMD_READ_CAPACITY:
			CommandSuccess = SCSI_Command_Read_Capacity();			
			break;
		default:
			printf_P(PSTR("UC: %d\r\n"), CommandBlock.SCSICommandData[0]); // DEBUG
	}
	
	if (CommandSuccess)
	{
		CommandStatus.Status = Command_Pass;
		
		SCSI_SET_SENSE(SCSI_SENSE_KEY_NO_SENSE,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);
	}
	else
	{
		CommandStatus.Status = Command_Fail;
	}
}

bool SCSI_Command_Inquiry(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t* InquiryDataPtr   = (uint8_t*)&InquiryData;
	uint8_t  BytesTransferred = 0;

	if ((CommandBlock.SCSICommandData[1] & ((1 << 0) | (1 << 1))) ||
	     CommandBlock.SCSICommandData[2])
	{
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}
		
	if (AllocationLength > 96)
	  AllocationLength = 96;
				
	for (uint8_t i = 0; i < sizeof(InquiryData); i++)
	{
		if (i == AllocationLength)
		  break;
					  
		Endpoint_Write_Byte(pgm_read_byte(InquiryDataPtr++));
		BytesTransferred++;
	}
			
	while (BytesTransferred < AllocationLength)
	{
		if (!(BytesTransferred % ConfigurationDescriptor.DataInEndpoint.EndpointSize))
		  Endpoint_In_Clear();
					
		BytesTransferred++;
		Endpoint_Write_Byte(0x00);
	}
	
	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= BytesTransferred;	
	return true;
}

bool SCSI_Command_Request_Sense(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t* SenseDataPtr     = (uint8_t*)&SenseData;
	
	for (uint8_t i = 0; i < sizeof(SenseData); i++)
	{
		if (i == AllocationLength)
		  break;
					  
		Endpoint_Write_Byte(*(SenseDataPtr++));
	}
	
	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= sizeof(SenseData);
	return true;
}

bool SCSI_Command_Read_Capacity(void)
{
	uint32_t Blocks    = 0x000000FF; // TEMP
	uint32_t BlockSize = 512;

	Endpoint_Write_DWord(Blocks);
	Endpoint_Write_DWord(BlockSize);

	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= 8;
	return true;
}
