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
	bool CommandSuccess = false;

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
		case SCSI_CMD_READ_CAPACITY_10:
			CommandSuccess = SCSI_Command_Read_Capacity_10();			
			break;
		case SCSI_CMD_SEND_DIAGNOSTIC:
			CommandSuccess = SCSI_Command_Send_Diagnostic();
			break;
		case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
			CommandSuccess = SCSI_Command_PreventAllowMediumRemoval();
			break;
		case SCSI_CMD_WRITE_10:
			CommandSuccess = SCSI_Command_ReadWrite_10(DATA_WRITE);
			break;
		case SCSI_CMD_READ_10:
			CommandSuccess = SCSI_Command_ReadWrite_10(DATA_READ);
			break;	
		default:
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		                   SCSI_ASENSE_INVALID_COMMAND,
		                   SCSI_ASENSEQ_NO_QUALIFIER);
			break;
	}
	
	if (CommandSuccess)
	{
		CommandStatus.Status = Command_Pass;
		
		printf("(PASS: %x) ", CommandBlock.SCSICommandData[0]);

		SCSI_SET_SENSE(SCSI_SENSE_KEY_GOOD,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);					   
	}
	else
	{
		printf("(FAIL: %x) ", CommandBlock.SCSICommandData[0]);

		CommandStatus.Status = Command_Fail;
	}
}

bool SCSI_Command_Inquiry(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t* InquiryDataPtr   = (uint8_t*)&InquiryData;
	uint8_t  BytesTransferred = 0;
	uint8_t  BytesInEndpoint  = 0;

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
		if (BytesInEndpoint == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_In_Clear();
			while (!(Endpoint_ReadWriteAllowed()));

			BytesInEndpoint = 0;
		}
					
		Endpoint_Write_Byte(0x00);
		
		BytesTransferred++;
		BytesInEndpoint++;
	}
	
	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= BytesTransferred;	
	return true;
}

bool SCSI_Command_Request_Sense(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t* SenseDataPtr     = (uint8_t*)&SenseData;
	uint8_t  BytesTransferred = 0;
	uint8_t  BytesInEndpoint  = 0;	
	
	for (uint8_t i = 0; i < sizeof(SenseData); i++)
	{
		if (i == AllocationLength)
		  break;
					  
		Endpoint_Write_Byte(*(SenseDataPtr++));
		BytesTransferred++;
		BytesInEndpoint++;
	}
	
	while (BytesTransferred < AllocationLength)
	{
		if (BytesInEndpoint == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_In_Clear();
			while (!(Endpoint_ReadWriteAllowed()));

			BytesInEndpoint = 0;
		}
					
		Endpoint_Write_Byte(0x00);
		
		BytesTransferred++;
		BytesInEndpoint++;
	}

	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= BytesTransferred;
	return true;
}

bool SCSI_Command_Read_Capacity_10(void)
{
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCKS - 1);
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCK_SIZE);

	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= 8;
	return true;
}

bool SCSI_Command_Send_Diagnostic(void)
{
	if (!(CommandBlock.SCSICommandData[1] & (1 << 2))) // Only self-test supported
	{
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	return true;
}

bool SCSI_Command_PreventAllowMediumRemoval(void)
{
	if (CommandBlock.SCSICommandData[4] & (1 << 0))
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_RED);
	else
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_GREEN);
	
	CommandBlock.Header.DataTransferLength = 0;
	return true;
}

bool SCSI_Command_ReadWrite_10(bool IsDataRead)
{
	uint32_t BlockAddress;
	uint16_t TotalBlocks;

	((uint8_t*)&BlockAddress)[3] = CommandBlock.SCSICommandData[2];
	((uint8_t*)&BlockAddress)[2] = CommandBlock.SCSICommandData[3];
	((uint8_t*)&BlockAddress)[1] = CommandBlock.SCSICommandData[4];
	((uint8_t*)&BlockAddress)[0] = CommandBlock.SCSICommandData[5];

	((uint8_t*)&TotalBlocks)[1] = CommandBlock.SCSICommandData[7];
	((uint8_t*)&TotalBlocks)[0] = CommandBlock.SCSICommandData[8];
	
	if (BlockAddress >= VIRTUAL_MEMORY_BLOCKS)
	{
		SCSI_SET_SENSE(SCSI_SENSE_KEY_HARDWARE_ERROR,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	if (IsDataRead == true)
	  VirtualMemory_ReadBlocks(BlockAddress, TotalBlocks);	
	else
	  VirtualMemory_WriteBlocks(BlockAddress, TotalBlocks);

	CommandBlock.Header.DataTransferLength -= (VIRTUAL_MEMORY_BLOCK_SIZE * TotalBlocks);
	return true;
}
