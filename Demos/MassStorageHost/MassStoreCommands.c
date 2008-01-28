/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "MassStoreCommands.h"

/* Globals: */
CommandBlockWrapper_t  SCSICommandBlock;
CommandStatusWrapper_t SCSICommandStatus;
uint32_t               MassStore_Tag = 1;

void MassStore_SendCommand(void)
{
	uint8_t* CommandByte = (uint8_t*)&SCSICommandBlock;

	/* Each transmission should have a unique tag value */
	MassStore_Tag++;

	/* Wait 5 frames before issuing a command to ensure device is ready */
	USB_Host_WaitMS(5);

	/* Select the OUT data pipe for CBW transmission */
	Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	/* Write the CBW command to the OUT pipe */
	for (uint8_t Byte = 0; Byte < sizeof(CommandBlockWrapper_t); Byte++)
	  Pipe_Write_Byte(*(CommandByte++));

	/* Send the data in the OUT pipe to the attached device */
	Pipe_FIFOCON_Clear();
	
	/* Freeze pipe after use */
	Pipe_Freeze();
}

uint8_t MassStore_WaitForDataRecieved(void)
{
	/* Unfreeze the OUT pipe so that it can be checked */
	Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	/* Select the IN data pipe for data reception */
	Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);
	Pipe_Unfreeze();

	/* Wait until data recieved in the IN pipe */
	while (!(Pipe_ReadWriteAllowed()))
	{
		Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Freeze IN and OUT pipes after use */
			Pipe_Freeze();
			Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);
			Pipe_Freeze();
			
			/* Clear the stall condition on the IN pipe */
			MassStore_ClearPipeStall(MASS_STORE_DATA_OUT_PIPE);

			/* Return error code */
			return OutPipeStalled;
		}

		Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Freeze IN and OUT pipes after use */
			Pipe_Freeze();
			Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
			Pipe_Freeze();

			/* Clear the stall condition on the OUT pipe */
			MassStore_ClearPipeStall(MASS_STORE_DATA_IN_PIPE);

			/* Return error code */
			return InPipeStalled;
		}
		  
		/* Check to see if the device was disconnected, if so exit function */
		if (!(USB_IsConnected))
		{
			/* Freeze IN and OUT pipes after use */
			Pipe_Freeze();
			Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
			Pipe_Freeze();

			/* Return error code */
			return DeviceDisconnected;
		}
	};
	
	/* Freeze IN and OUT pipes after use */
	Pipe_Freeze();
	Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
	Pipe_Freeze();
	
	return NoError;
}

uint8_t MassStore_SendRecieveData(uint8_t* BufferPtr)
{
	uint16_t BytesRem = SCSICommandBlock.Header.DataTransferLength;

	/* Check the direction of the SCSI command data stage */
	if (SCSICommandBlock.Header.Flags & COMMAND_DIRECTION_DATA_IN)
	{
		/* Select the IN data pipe for data reception */
		Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);
		Pipe_Unfreeze();
		
		/* Loop until all bytes read */
		while (BytesRem)
		{
			/* Load each byte into the buffer */
			*(BufferPtr++) = Pipe_Read_Byte();
			
			/* Decrement the bytes remaining counter */
			BytesRem--;
			
			/* Check to see if the device was disconnected, if so exit function */
			if (!(USB_IsConnected))
			{
				Pipe_Freeze();

				return DeviceDisconnected;
			}

			/* When pipe is empty, clear it and wait for the next packet */
			if (!(Pipe_BytesInPipe()))
			{
				Pipe_FIFOCON_Clear();
				
				while (!(Pipe_ReadWriteAllowed()));
			}
		}
	}
	else
	{
		uint16_t BytesInEndpoint = 0;

		/* Select the OUT data pipe for data transmission */
		Pipe_SelectPipe(MASS_STORE_DATA_OUT_PIPE);
		Pipe_Unfreeze();

		/* Write the block data to the pipe */
		while (BytesRem)
		{
			Pipe_Write_Byte(*(BufferPtr++));
			
			BytesRem--;
			BytesInEndpoint++;
			
			/* Check if the pipe is full */
			if (BytesInEndpoint == MassStoreEndpointSize_OUT)
			{
				/* Send the pipe data, clear the counter */
				Pipe_FIFOCON_Clear();
				BytesInEndpoint = 0;
			}
		}

		/* Check to see if any data is still in the pipe - if so, send it */
		if (BytesInEndpoint)
		  Pipe_FIFOCON_Clear();
	}
	
	/* Freeze used pipe after use */
	Pipe_Freeze();

	return NoError;
}

void MassStore_GetReturnedStatus(void)
{
	uint8_t* StatusByte = (uint8_t*)&SCSICommandStatus;

	/* Select the IN data pipe for data reception */
	Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);
	Pipe_Unfreeze();

	/* Wait until pipe is ready to be read from (contains data) */
	while (!(Pipe_ReadWriteAllowed()));
	
	/* Load in the CSW from the attached device */
	for (uint8_t CSWLen = 0; CSWLen < sizeof(CommandStatusWrapper_t); CSWLen++)
	  (*(StatusByte++)) = Pipe_Read_Byte();
	
	/* Clear the data ready for next reception */
	Pipe_FIFOCON_Clear();

	/* Freeze the IN pipe after use */
	Pipe_Freeze();
}

uint8_t MassStore_RequestSense(const uint8_t LUNIndex, SCSI_Request_Sense_Response_t* SensePtr)
{
	uint8_t ReturnCode = NoError;

	/* Create a CBW with a SCSI command to issue REQUEST SENSE command */
	SCSICommandBlock = (CommandBlockWrapper_t)
		{
			Header:
				{
					Signature:          CBW_SIGNATURE,
					Tag:                MassStore_Tag,
					DataTransferLength: sizeof(SCSI_Request_Sense_Response_t),
					Flags:              COMMAND_DIRECTION_DATA_IN,
					LUN:                LUNIndex,
					SCSICommandLength:  10
				},
					
			SCSICommandData:
				{
					SCSI_CMD_REQUEST_SENSE,
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Reserved
					sizeof(SCSI_Request_Sense_Response_t), // Allocation Length
					0x00                    // Unused (control)
				}
		};
	
	/* Send SCSI command to the attached device */
	MassStore_SendCommand();

	/* Wait until data recieved from the device */
	if ((ReturnCode = MassStore_WaitForDataRecieved()) != NoError)
	  return ReturnCode;

	/* Read the returned sense data into the buffer */
	if ((ReturnCode = MassStore_SendRecieveData((uint8_t*)SensePtr)) != NoError)
	  return ReturnCode;	
	
	/* Read in the returned CSW from the device */
	MassStore_GetReturnedStatus();
	
	return ReturnCode;
}

uint8_t MassStore_ReadDeviceBlock(const uint8_t LUNIndex, const uint32_t BlockAddress,
                                  const uint8_t Blocks, uint8_t* BufferPtr)
{
	uint8_t ReturnCode = NoError;

	/* Create a CBW with a SCSI command to read in the given blocks from the device */
	SCSICommandBlock = (CommandBlockWrapper_t)
		{
			Header:
				{
					Signature:          CBW_SIGNATURE,
					Tag:                MassStore_Tag,
					DataTransferLength: (Blocks * DEVICE_BLOCK_SIZE),
					Flags:              COMMAND_DIRECTION_DATA_IN,
					LUN:                LUNIndex,
					SCSICommandLength:  10
				},
					
			SCSICommandData:
				{
					SCSI_CMD_READ_10,
					0x00,                   // Unused (control bits, all off)
					(BlockAddress >> 24),   // MSB of Block Address
					(BlockAddress >> 16),
					(BlockAddress >> 8),
					(BlockAddress & 0xFF),  // LSB of Block Address
					0x00,                   // Unused (reserved)
					0x00,                   // MSB of Total Blocks to Read
					Blocks,                 // LSB of Total Blocks to Read
					0x00                    // Unused (control)
				}
		};
	
	/* Send SCSI command to the attached device */
	MassStore_SendCommand();

	/* Wait until data recieved from the device */
	if ((ReturnCode = MassStore_WaitForDataRecieved()) != NoError)
	  return ReturnCode;

	/* Read the returned block data into the buffer */
	if ((ReturnCode = MassStore_SendRecieveData(BufferPtr)) != NoError)
	  return ReturnCode;	
	
	/* Read in the returned CSW from the device */
	MassStore_GetReturnedStatus();
	
	return ReturnCode;
}

uint8_t MassStore_ClearPipeStall(const uint8_t PipeEndpointNum)
{
	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			RequestType: (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT),
			RequestData: REQ_ClearFeature,
			Value:       FEATURE_ENDPOINT,
			Index:       PipeEndpointNum,
			DataLength:  0,
		};
	
	return USB_Host_SendControlRequest(NULL);
}

void MassStore_TestUnitReady(const uint8_t LUNIndex)
{
	/* Create a CBW with a SCSI command to issue TEST UNIT READY command */
	SCSICommandBlock = (CommandBlockWrapper_t)
		{
			Header:
				{
					Signature:          CBW_SIGNATURE,
					Tag:                MassStore_Tag,
					DataTransferLength: 0,
					Flags:              COMMAND_DIRECTION_DATA_IN,
					LUN:                LUNIndex,
					SCSICommandLength:  6
				},
					
			SCSICommandData:
				{
					SCSI_CMD_TEST_UNIT_READY,
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00                    // Unused (control)
				}
		};
	
	/* Send SCSI command to the attached device */
	MassStore_SendCommand();	

	/* Read in the returned CSW from the device */
	MassStore_GetReturnedStatus();
}

uint8_t MassStore_ReadCapacity(const uint8_t LUNIndex, SCSI_Capacity_t* CapacityPtr)
{
	uint8_t ReturnCode = NoError;

	/* Create a CBW with a SCSI command to issue READ CAPACITY command */
	SCSICommandBlock = (CommandBlockWrapper_t)
		{
			Header:
				{
					Signature:          CBW_SIGNATURE,
					Tag:                MassStore_Tag,
					DataTransferLength: 8,
					Flags:              COMMAND_DIRECTION_DATA_IN,
					LUN:                LUNIndex,
					SCSICommandLength:  10
				},
					
			SCSICommandData:
				{
					SCSI_CMD_READ_CAPACITY_10,
					0x00,                   // Reserved
					0x00,                   // MSB of Logical block address
					0x00,
					0x00,
					0x00,                   // LSB of Logical block address
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Partial Medium Indicator
					0x00                    // Unused (control)
				}
		};
	
	/* Send SCSI command to the attached device */
	MassStore_SendCommand();

	/* Wait until data recieved from the device */
	if ((ReturnCode = MassStore_WaitForDataRecieved()) != NoError)
	  return ReturnCode;

	/* Read the returned capacity data into the buffer */
	if ((ReturnCode = MassStore_SendRecieveData((uint8_t*)CapacityPtr)) != NoError)
	  return ReturnCode;	
	
	/* Read in the returned CSW from the device */
	MassStore_GetReturnedStatus();
	
	return ReturnCode;
}

void MassStore_PreventAllowMediumRemoval(const uint8_t LUNIndex, const bool PreventRemoval)
{
	/* Create a CBW with a SCSI command to issue PREVENT ALLOW MEDIUM REMOVAL command */
	SCSICommandBlock = (CommandBlockWrapper_t)
		{
			Header:
				{
					Signature:          CBW_SIGNATURE,
					Tag:                MassStore_Tag,
					DataTransferLength: 0,
					Flags:              COMMAND_DIRECTION_DATA_OUT,
					LUN:                LUNIndex,
					SCSICommandLength:  6
				},
					
			SCSICommandData:
				{
					SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL,
					0x00,                   // Reserved
					0x00,                   // Reserved
					PreventRemoval,         // Prevent flag
					0x00,                   // Reserved
					0x00                    // Unused (control)
				}
		};
	
	/* Send SCSI command to the attached device */
	MassStore_SendCommand();

	/* Read in the returned CSW from the device */
	MassStore_GetReturnedStatus();
}
