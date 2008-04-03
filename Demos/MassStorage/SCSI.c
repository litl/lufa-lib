/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_SCSI_C
#include "SCSI.h"

SCSI_Inquiry_Response_t InquiryData = 
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

SCSI_Read_Write_Error_Recovery_Sense_Page_t RecoveryPage =
	{
		AWRE:                true,

		ReadRetryCount:      0x03,
		WriteRetryCount:     0x03,
	};

SCSI_Informational_Exceptions_Sense_Page_t InformationalExceptionsPage =
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

	/* Run the apropriate SCSI command hander function based on the passed command */
	switch (CommandBlock.SCSICommandData[0])
	{
		case SCSI_CMD_INQUIRY:
			CommandSuccess = SCSI_Command_Inquiry();			
			break;
		case SCSI_CMD_REQUEST_SENSE:
			CommandSuccess = SCSI_Command_Request_Sense();
			break;
		case SCSI_CMD_READ_CAPACITY_10:
			CommandSuccess = SCSI_Command_Read_Capacity_10();			
			break;
		case SCSI_CMD_SEND_DIAGNOSTIC:
			CommandSuccess = SCSI_Command_Send_Diagnostic();
			break;
		case SCSI_CMD_WRITE_10:
			CommandSuccess = SCSI_Command_ReadWrite_6_10(DATA_WRITE, MODE_10);
			break;
		case SCSI_CMD_READ_10:
			CommandSuccess = SCSI_Command_ReadWrite_6_10(DATA_READ, MODE_10);
			break;
		case SCSI_CMD_WRITE_6:
			CommandSuccess = SCSI_Command_ReadWrite_6_10(DATA_WRITE, MODE_6);
			break;
		case SCSI_CMD_READ_6:
			CommandSuccess = SCSI_Command_ReadWrite_6_10(DATA_READ, MODE_6);
			break;
		case SCSI_CMD_MODE_SENSE_6:
			CommandSuccess = SCSI_Command_Mode_Sense_6_10(MODE_6);
			break;
		case SCSI_CMD_MODE_SENSE_10:
			CommandSuccess = SCSI_Command_Mode_Sense_6_10(MODE_10);
			break;
		case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
		case SCSI_CMD_TEST_UNIT_READY:
		case SCSI_CMD_VERIFY_10:
			/* These commands should just succeed, no handling required */
			CommandSuccess = true;
			CommandBlock.Header.DataTransferLength = 0;
			break;
		default:
			/* Update the SENSE key to reflect the invalid command */
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		                   SCSI_ASENSE_INVALID_COMMAND,
		                   SCSI_ASENSEQ_NO_QUALIFIER);
			break;
	}
	
	/* Check if command was sucessfully processed */
	if (CommandSuccess)
	{
		/* Command succeeded - set the CSW status and update the SENSE key */
		CommandStatus.Header.Status = Command_Pass;
		
		SCSI_SET_SENSE(SCSI_SENSE_KEY_GOOD,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);					   
	}
	else
	{
		/* Command failed - set the CSW status - failed command function updates the SENSE key */
		CommandStatus.Header.Status = Command_Fail;
	}
}

static bool SCSI_Command_Inquiry(void)
{
	uint16_t  AllocationLength = (((uint16_t)CommandBlock.SCSICommandData[3] << 8) |
	                                         CommandBlock.SCSICommandData[4]);
	uint16_t BytesTransferred  = (AllocationLength < sizeof(InquiryData))? AllocationLength :
	                                                                       sizeof(InquiryData);

	/* Only the standard INQUIRY data is supported, check if any optional INQUIRY bits set */
	if ((CommandBlock.SCSICommandData[1] & ((1 << 0) | (1 << 1))) ||
	     CommandBlock.SCSICommandData[2])
	{
		/* Optional but unsupported bits set - update the SENSE key and fail the request */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	/* Write the INQUIRY data to the endpoint */
	Endpoint_Write_Stream_LE(&InquiryData, BytesTransferred);

	/* Pad out remaining bytes with 0x00 */
	while (BytesTransferred < AllocationLength)
	{
		/* When endpoint filled, send the data and wait until it is ready to be written to again */
		if (Endpoint_BytesInEndpoint() == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_FIFOCON_Clear();
			while (!(Endpoint_ReadWriteAllowed()));
		}
					
		Endpoint_Write_Byte(0x00);
		
		BytesTransferred++;
	}
	
	/* Send the final endpoint data packet to the host */
	Endpoint_FIFOCON_Clear();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.Header.DataTransferLength -= BytesTransferred;	
	return true;
}

static bool SCSI_Command_Request_Sense(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t  BytesTransferred = (AllocationLength < sizeof(SenseData))? AllocationLength : sizeof(SenseData);
	
	/* Send the SENSE data - this indicates to the host the status of the last command */
	Endpoint_Write_Stream_LE(&SenseData, BytesTransferred);
	
	/* Pad out remaining bytes with 0x00 */
	while (BytesTransferred < AllocationLength)
	{
		/* When endpoint filled, send the data and wait until it is ready to be written to again */
		if (Endpoint_BytesInEndpoint() == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_FIFOCON_Clear();
			while (!(Endpoint_ReadWriteAllowed()));
		}
					
		Endpoint_Write_Byte(0x00);
		
		BytesTransferred++;
	}

	/* Send the final endpoint data packet to the host */
	Endpoint_FIFOCON_Clear();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.Header.DataTransferLength -= BytesTransferred;	
	return true;
}

static bool SCSI_Command_Read_Capacity_10(void)
{
	/* Send the total number of logical blocks in the device */
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCKS);

	/* Send the logical block size of the device (must be 512 bytes) */
	Endpoint_Write_DWord_BE(VIRTUAL_MEMORY_BLOCK_SIZE);

	/* Send the endpoint data packet to the host */
	Endpoint_FIFOCON_Clear();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.Header.DataTransferLength -= 8;
	return true;
}

static bool SCSI_Command_Send_Diagnostic(void)
{
	uint8_t ReturnByte;

	/* Check to see if the SELF TEST bit is not set */
	if (!(CommandBlock.SCSICommandData[1] & (1 << 2)))
	{
		/* Only self-test supported - update SENSE key and fail the command */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}
	
	/* Test first Dataflash IC is present and responding to commands */
	Dataflash_SelectChip(DATAFLASH_CHIP1);
	Dataflash_SendByte(DF_CMD_READMANUFACTURERDEVICEINFO);
	ReturnByte = Dataflash_SendByte(0x00);
	Dataflash_DeselectChip();

	/* If returned data is invalid, fail the command */
	if (ReturnByte != DF_MANUFACTURER_ATMEL)
	{
		/* Update SENSE key with a hardware error condition and return command fail */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_HARDWARE_ERROR,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);	
	
		return false;
	}

	#if (DATAFLASH_TOTALCHIPS == 2)
	/* Test second Dataflash IC is present and responding to commands */
	Dataflash_SelectChip(DATAFLASH_CHIP2);
	Dataflash_SendByte(DF_CMD_READMANUFACTURERDEVICEINFO);
	ReturnByte = Dataflash_SendByte(0x00);
	Dataflash_DeselectChip();

	/* If returned data is invalid, fail the command */
	if (ReturnByte != DF_MANUFACTURER_ATMEL)
	{
		/* Update SENSE key with a hardware error condition and return command fail */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_HARDWARE_ERROR,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);	
	
		return false;
	}
	#endif
	
	/* All Dataflash ICs are working correctly, succeed the command */
	return true;
}

static bool SCSI_Command_ReadWrite_6_10(const bool IsDataRead, const bool IsMode10)
{
	uint32_t BlockAddress;
	uint16_t TotalBlocks;
	
	if (IsMode10)
	{
		/* Load in the 32-bit block address (SCSI uses big-endian, so have to do it byte-by-byte) */
		((uint8_t*)&BlockAddress)[3] = CommandBlock.SCSICommandData[2];
		((uint8_t*)&BlockAddress)[2] = CommandBlock.SCSICommandData[3];
		((uint8_t*)&BlockAddress)[1] = CommandBlock.SCSICommandData[4];
		((uint8_t*)&BlockAddress)[0] = CommandBlock.SCSICommandData[5];

		/* Load in the 16-bit total blocks (SCSI uses big-endian, so have to do it byte-by-byte) */
		((uint8_t*)&TotalBlocks)[1] = CommandBlock.SCSICommandData[7];
		((uint8_t*)&TotalBlocks)[0] = CommandBlock.SCSICommandData[8];	
	}
	else
	{
		/* Load in the 24-bit block address (SCSI uses big-endian, so have to do it byte-by-byte) */
		((uint8_t*)&BlockAddress)[3] = 0x00;
		((uint8_t*)&BlockAddress)[2] = CommandBlock.SCSICommandData[1];
		((uint8_t*)&BlockAddress)[1] = CommandBlock.SCSICommandData[2];
		((uint8_t*)&BlockAddress)[0] = CommandBlock.SCSICommandData[3];

		/* Load in the 8-bit total blocks */
		((uint8_t*)&BlockAddress)[0] = 0x00;	
		((uint8_t*)&BlockAddress)[1] = CommandBlock.SCSICommandData[4];	
	}
	
	/* Check if the block address is outside the maximum allowable value */
	if (BlockAddress > VIRTUAL_MEMORY_BLOCKS)
	{
		/* Block address is invalid, update SENSE key and return command fail */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	/* Indicate dataflash in use */
	LEDs_TurnOnLEDs(LEDS_LED1);

	/* Determine if the packet is a READ (10) or WRITE (10) command, call appropriate function */
	if (IsDataRead == DATA_READ)
	  VirtualMemory_ReadBlocks(BlockAddress, TotalBlocks);	
	else
	  VirtualMemory_WriteBlocks(BlockAddress, TotalBlocks);

	/* Indicate dataflash no longer in use */
	LEDs_TurnOffLEDs(LEDS_LED1);

	/* Update the bytes transferred counter and succeed the command */
	CommandBlock.Header.DataTransferLength -= (VIRTUAL_MEMORY_BLOCK_SIZE * TotalBlocks);
	return true;
}

static bool SCSI_Command_Mode_Sense_6_10(const bool IsMode10)
{
	uint8_t  AllocationLength = (IsMode10 ? CommandBlock.SCSICommandData[8] : CommandBlock.SCSICommandData[4]);							   
	uint8_t  PageCode         = (CommandBlock.SCSICommandData[2] & 0x3F);

	/* Determine which SENSE page has been requested by the host */
	switch (PageCode)
	{
		case SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY:
			/* Send SENSE page header */
			SCSI_WriteSensePageHeader(sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t)
			                        + INFORMATIONAL_PAGE_OVERHEAD(1), IsMode10);

			/* Send requested SENSE page */
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY,
			                    sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t),
			                    &RecoveryPage,
			                    (AllocationLength - 4),
			                    IsMode10);
			break;
		case SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS:
			/* Send SENSE page header */
			SCSI_WriteSensePageHeader(sizeof(SCSI_Informational_Exceptions_Sense_Page_t)
			                        + INFORMATIONAL_PAGE_OVERHEAD(1), IsMode10);

			/* Send requested SENSE page */
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS,
			                    sizeof(SCSI_Informational_Exceptions_Sense_Page_t),
			                    &InformationalExceptionsPage,
			                    (AllocationLength - 4),
			                    IsMode10);
			break;
		case SCSI_SENSE_PAGE_ALL:
			/* Send SENSE page header */
			SCSI_WriteSensePageHeader(sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t)
			                        + sizeof(SCSI_Informational_Exceptions_Sense_Page_t)
			                        + INFORMATIONAL_PAGE_OVERHEAD(2), IsMode10);
			
			/* Send first page to the host (pages must be sent in ascending order) */
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_READ_WRITE_ERR_RECOVERY,
			                    sizeof(SCSI_Read_Write_Error_Recovery_Sense_Page_t),
			                    &RecoveryPage,
			                    (AllocationLength - 4),
			                    IsMode10);
			
			/* Send second page to the host (pages must be sent in ascending order) */
			SCSI_WriteSensePage(SCSI_SENSE_PAGE_INFORMATIONAL_EXCEPTIONS,
			                    sizeof(SCSI_Informational_Exceptions_Sense_Page_t),
			                    &InformationalExceptionsPage,
			                    (AllocationLength - sizeof(SCSI_Informational_Exceptions_Sense_Page_t)
                                                  - 6),
			                    IsMode10);
			break;
		default:
			/* Page not supported - update the SENSE key and fail the command */
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
						   SCSI_ASENSE_INVALID_FIELD_IN_CDB,
						   SCSI_ASENSEQ_NO_QUALIFIER);
			return false;
	}

	/* Send the endpoint data, update the bytes transferred counter and succeed the command */
	Endpoint_FIFOCON_Clear();
	CommandBlock.Header.DataTransferLength -= AllocationLength;
	return true;
}

static void SCSI_WriteSensePageHeader(const uint8_t DataLength, const bool IsMode10)
{
	if (IsMode10)
	  Endpoint_Write_Byte(0x00);

	Endpoint_Write_Byte(DataLength);	

	if (IsMode10)
	  Endpoint_Write_Word_BE(0x0000);
	  
	Endpoint_Write_Word_BE(0x0000);
	Endpoint_Write_Byte(0x00);
}
								
static void SCSI_WriteSensePage(const uint8_t PageCode, const uint8_t PageSize,
                                void* PageDataPtr, const int16_t AllocationLength,
                                const bool IsMode10)
{
	int16_t BytesToTransfer = (AllocationLength < PageSize)? AllocationLength : PageSize;

	/* Negative or zero allocation length prevents the page from being sent */
	if (AllocationLength <= 0)
	  return;

	/* Write page information to the endpoint */
	Endpoint_Write_Byte(PageCode);
	Endpoint_Write_Byte(PageSize);
	
	BytesToTransfer -= 2;
	
	Endpoint_Write_Stream_LE(PageDataPtr, BytesToTransfer);
}
