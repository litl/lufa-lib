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

	printf_P(PSTR("SC: %X"), CommandBlock.SCSICommandData[0]); // DEBUG

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
			CommandSuccess = SCSI_Command_Write_10();
			break;
		case SCSI_CMD_READ_10:
			CommandSuccess = SCSI_Command_Read_10();
			break;	
		default:
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		                   SCSI_ASENSE_INVALID_COMMAND,
		                   SCSI_ASENSEQ_NO_QUALIFIER);
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
					
		Endpoint_Write_Byte(0x00);
		BytesTransferred++;
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
	
	for (uint8_t i = 0; i < sizeof(SenseData); i++)
	{
		if (i == AllocationLength)
		  break;
					  
		Endpoint_Write_Byte(*(SenseDataPtr++));
		BytesTransferred++;
	}
	
	Endpoint_In_Clear();

	CommandBlock.Header.DataTransferLength -= BytesTransferred;
	return true;
}

bool SCSI_Command_Read_Capacity_10(void)
{
	uint32_t BlockAddress = *(uint32_t*)&CommandBlock.SCSICommandData[2];

	Endpoint_Write_DWord(BlockAddress);
	Endpoint_Write_DWord(DATAFLASH_PAGE_SIZE);

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
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED2_RED);
	else
	  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED2_GREEN);
	
	CommandBlock.Header.DataTransferLength = 0;
	return true;
}

bool SCSI_Command_Write_10(void)
{
	uint32_t BlockAddress   = *(uint32_t*)&CommandBlock.SCSICommandData[2];
	uint16_t TotalBlocks    = *(uint16_t*)&CommandBlock.SCSICommandData[7];
	uint16_t BlocksRem      = TotalBlocks;
	uint16_t AddressInBlock = 0;
	
	if (BlockAddress >= (DATAFLASH_PAGE_SIZE * 2))
	{
		// TODO - set sense data
		return false;
	}

	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	Dataflash_SelectChipFromPage(BlockAddress);
	
	Dataflash_SendByte(DF_CMD_BUFF1WRITE);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);

	while (BlocksRem)
	{
		if (AddressInBlock == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_ToggleSelectedChipCS();
			
			Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
			Dataflash_SendByte(BlockAddress >> 8);
			Dataflash_SendByte((BlockAddress & 0xFF) << 5);
			Dataflash_SendByte(0);

			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_GETSTATUS);
			
			while (!(Dataflash_SendByte(0) & DF_STATUS_READY));

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);			
			
			AddressInBlock = 0;
			BlocksRem--;
			BlockAddress++;
			
			Dataflash_SelectChipFromPage(BlockAddress);

			Dataflash_SendByte(DF_CMD_BUFF1WRITE);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
		}

		Dataflash_SendByte(Endpoint_Read_Byte());

		AddressInBlock++;
	}

	Dataflash_SelectChip(DATAFLASH_NO_CHIP);

	Endpoint_Out_Clear();

	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
	
	CommandBlock.Header.DataTransferLength -= (DATAFLASH_PAGE_SIZE * TotalBlocks);

	return true;
}

bool SCSI_Command_Read_10(void)
{
	uint32_t BlockAddress   = *(uint32_t*)&CommandBlock.SCSICommandData[2];
	uint16_t TotalBlocks    = *(uint16_t*)&CommandBlock.SCSICommandData[7];
	uint16_t BlocksRem      = TotalBlocks;
	uint16_t AddressInBlock = 0;

	if (BlockAddress >= (DATAFLASH_PAGE_SIZE * 2))
	{
		// TODO - set sense data
		return false;
	}

	Dataflash_SelectChipFromPage(BlockAddress);

	Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
	Dataflash_SendByte(BlockAddress >> 8);
	Dataflash_SendByte((BlockAddress & 0xFF) << 5);
	Dataflash_SendByte(0);

	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);

	while (BlocksRem)
	{
		if (AddressInBlock == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_SelectChip(DATAFLASH_NO_CHIP);			
			Dataflash_SelectChipFromPage(BlockAddress);

			Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
			Dataflash_SendByte(BlockAddress >> 8);
			Dataflash_SendByte((BlockAddress & 0xFF) << 5);
			Dataflash_SendByte(0);

			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);			

			AddressInBlock = 0;
			BlocksRem--;
			BlockAddress++;
		}
		
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		AddressInBlock++;
	}
	
	CommandBlock.Header.DataTransferLength -= (DATAFLASH_PAGE_SIZE * TotalBlocks);

	Endpoint_In_Clear();

	return true;
}
