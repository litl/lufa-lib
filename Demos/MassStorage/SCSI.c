/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#define INCLUDE_FROM_SCSI_C
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

SCSI_Read_Write_Error_Recovery_Sense_Page_t RecoveryPage PROGMEM =
	{
		AWRE:                true,

		ReadRetryCount:      0x03,
		WriteRetryCount:     0x03,
	};

SCSI_Informational_Exceptions_Sense_Page_t InformationalExceptionsPage PROGMEM =
	{
		MRIE:                true,
		
		ReportCount:         1,
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
		case SCSI_CMD_VERIFY_10:
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
		case SCSI_CMD_MODE_SENSE_6:
			CommandSuccess = SCSI_Command_Mode_Sense_6();
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
		
		SCSI_SET_SENSE(SCSI_SENSE_KEY_GOOD,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);					   
	}
	else
	{
		CommandStatus.Status = Command_Fail;
	}
}

static bool SCSI_Command_Inquiry(void)
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

static bool SCSI_Command_Request_Sense(void)
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

static bool SCSI_Command_Read_Capacity_10(void)
{
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCKS);
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCK_SIZE);

	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= 8;
	return true;
}

static bool SCSI_Command_Send_Diagnostic(void)
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

static bool SCSI_Command_PreventAllowMediumRemoval(void)
{
	if (CommandBlock.SCSICommandData[4] & (1 << 0))
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_RED);
	else
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_GREEN);
	
	CommandBlock.Header.DataTransferLength = 0;
	return true;
}

static bool SCSI_Command_ReadWrite_10(const bool IsDataRead)
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

	if (IsDataRead == DATA_READ)
	  VirtualMemory_ReadBlocks(BlockAddress, TotalBlocks);	
	else
	  VirtualMemory_WriteBlocks(BlockAddress, TotalBlocks);

	CommandBlock.Header.DataTransferLength -= (VIRTUAL_MEMORY_BLOCK_SIZE * TotalBlocks);
	return true;
}

static bool SCSI_Command_Mode_Sense_6(void)
{
	uint8_t  AllocationLength =  CommandBlock.SCSICommandData[4];							   
	uint8_t  PageCode         = (CommandBlock.SCSICommandData[2] & 0x3F);
										   
	switch (PageCode)
	{
		case SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY:
			Endpoint_Write_Byte(sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t) + 5);	
			Endpoint_Write_Word_BE(0x0000);
			Endpoint_Write_Byte(0x00);

			SCSI_WriteSensePage(SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY,
			                    sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t),
			                    (uint8_t*)&RecoveryPage,
			                    (AllocationLength - 4));
			break;
		case SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS:
			Endpoint_Write_Byte(sizeof(SCSI_Informational_Exceptions_Sense_Page_t) + 5);	
			Endpoint_Write_Word_BE(0x0000);
			Endpoint_Write_Byte(0x00);

			SCSI_WriteSensePage(SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS,
			                    sizeof(SCSI_Informational_Exceptions_Sense_Page_t),
			                    (uint8_t*)&InformationalExceptionsPage,
			                    (AllocationLength - 4));
			break;
		case SCSI_SENSE_PAGE_ALL:
			Endpoint_Write_Byte(sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t)
			                  + sizeof(SCSI_Informational_Exceptions_Sense_Page_t)
			                  + 7);
			Endpoint_Write_Word_BE(0x0000);
			Endpoint_Write_Byte(0x00);
			
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY,
			                    sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t),
			                    (uint8_t*)&RecoveryPage,
			                    (AllocationLength - 4));
			
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS,
			                    sizeof(SCSI_Informational_Exceptions_Sense_Page_t),
			                    (uint8_t*)&InformationalExceptionsPage,
			                    (AllocationLength - sizeof(SCSI_Informational_Exceptions_Sense_Page_t)
                                                  - 6));
			break;
		default:
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
						   SCSI_ASENSE_INVALID_FIELD_IN_CDB,
						   SCSI_ASENSEQ_NO_QUALIFIER);
			return false;
	}
	
	Endpoint_In_Clear();
	CommandBlock.Header.DataTransferLength -= AllocationLength;

	return true;
}

static void SCSI_WriteSensePage(const uint8_t PageCode, const uint8_t PageSize,
                         const uint8_t* PageDataPtr, const int16_t AllocationLength)
{
	uint8_t BytesTransferred;

	if (AllocationLength <= 0)
	  return;

	Endpoint_Write_Byte(PageCode);
	Endpoint_Write_Byte(PageSize);
	
	BytesTransferred = 2;
	
	for (uint8_t i = 0; i < PageSize; i++)
	{
		if (BytesTransferred == AllocationLength)
		  break;
		
		Endpoint_Write_Byte(pgm_read_byte(*(PageDataPtr++)));
		BytesTransferred++;
	}	
}
