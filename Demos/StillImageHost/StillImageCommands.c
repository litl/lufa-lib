/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "StillImageCommands.h"

/* Globals: */
PIMA_Container_t PIMA_Command;
PIMA_Container_t PIMA_Response;
uint32_t         PIMA_TransactionID = 1;

void SImage_SendCommand(uint8_t* Buffer)
{
	/* Unfreeze the data OUT pipe ready for data transmission */
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	/* Write the PIMA command block to the data OUT pipe */
	Pipe_Write_Stream_LE(&PIMA_Command, sizeof(PIMA_Container_t));
	
	/* Write any command parameters to the data OUT pipe */
	Pipe_Write_Stream_LE(Buffer, (PIMA_Command.DataLength - sizeof(PIMA_Container_t)));

	/* Send the PIMA command block to the attached device */
	Pipe_FIFOCON_Clear();

	/* Increment and wrap the Transaction ID value */
	if (++PIMA_TransactionID == 0xFFFFFFFF)
	  PIMA_TransactionID = 1;
				
	/* Freeze pipe after use */
	Pipe_Freeze();
}

uint8_t SImage_WaitForDataReceived(void)
{
	uint8_t  ErrorCode    = NoError;
	uint16_t TimeoutMSRem = COMMAND_DATA_TIMEOUT_MS;

	/* Unfreeze the data OUT pipe */
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	/* Unfreeze the data IN pipe */
	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	Pipe_Unfreeze();

	/* Wait until data recieved on the IN pipe */
	while (!(Pipe_ReadWriteAllowed()))
	{
		/* Check to see if a new frame has been issued (1ms elapsed) */
		if (USB_INT_HasOccurred(USB_INT_HSOFI))
		{
			/* Clear the flag and decrement the timout period counter */
			USB_INT_Clear(USB_INT_HSOFI);
			TimeoutMSRem--;

			/* Check to see if the timeout period for the command has elapsed */
			if (!(TimeoutMSRem))
			{
				/* Set error code and break out of the loop */
				ErrorCode = CommandTimeout;
				break;
			}
		}
		
		Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Clear the stall condition on the OUT pipe */
			SImage_ClearPipeStall(SIMAGE_DATA_OUT_PIPE);

			/* Set error code and break out of the loop */
			ErrorCode = OutPipeStalled;
			break;
		}

		Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Clear the stall condition on the IN pipe */
			SImage_ClearPipeStall(SIMAGE_DATA_IN_PIPE);

			/* Set error code and break out of the loop */
			ErrorCode = InPipeStalled;
			break;
		}
		  
		/* Check to see if the device was disconnected, if so exit function */
		if (!(USB_IsConnected))
		{
			/* Set error code and break out of the loop */
			ErrorCode = DeviceDisconnected;
			break;
		}
	};
	
	/* Freeze IN and OUT pipes after use */
	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	Pipe_Freeze();
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Freeze();
	
	return ErrorCode;
}

void SImage_GetResponse(void)
{
	/* Select the IN data pipe for data reception */
	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	Pipe_Unfreeze();
	
	/* Load in the response from the attached device */
	Pipe_Read_Stream_LE(&PIMA_Response, sizeof(PIMA_Container_t));

	/* Freeze the IN pipe after use */
	Pipe_Freeze();	
}

void SImage_GetData(uint8_t* Buffer)
{
	/* Select the IN data pipe for data reception */
	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	Pipe_Unfreeze();

	/* Read in the data from the device */
	Pipe_Read_Stream_LE(Buffer, (PIMA_Response.DataLength - sizeof(PIMA_Container_t)));

	/* Clear the data ready for next reception */
	Pipe_FIFOCON_Clear();

	/* Freeze the IN pipe after use */
	Pipe_Freeze();		
}

uint8_t SImage_ClearPipeStall(const uint8_t PipeEndpointNum)
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

uint8_t SImage_GetInfo(void)
{
	uint8_t ReturnCode;

	PIMA_Command = (PIMA_Container_t)
	{
		DataLength:    (sizeof(PIMA_Container_t) + 4),
		Type:          CType_CommandBlock,
		Code:          PIMA_OPERATION_GETDEVICEINFO,
		TransactionID: PIMA_TransactionID,
	};
	
	/* Use a null Session ID for GETDEVICEINFO requests only */
	uint32_t SessionID = 0;
	
	/* Send the PIMA command to the attached device */
	SImage_SendCommand((uint8_t*)&SessionID);
	
	/* Wait until a response is received, return error code if and error occurs */
	if ((ReturnCode = SImage_WaitForDataReceived()) != NoError)
	  return ReturnCode;

	/* Get the response header from the device */
	SImage_GetResponse();

	/* First response should be a data block containing device info */
	if (PIMA_Response.Type != CType_DataBlock)
	  return BadResponse;

	/* Create buffer big enough to hold the returned data minus the header */
	uint8_t DeviceInfo[PIMA_Response.DataLength - sizeof(PIMA_Container_t)];

	/* Get the data from the attached device */
	SImage_GetData(DeviceInfo);

	/* Print out raw data from the device (note, strings are in Unicode, so nulls are skipped) */
	printf_P(PSTR("Device Info Data (Size: %lu):\r\n"), PIMA_Response.DataLength);
	for (uint16_t CByte = 0; CByte < (PIMA_Response.DataLength - sizeof(PIMA_Container_t)); CByte++)
	{
		if (DeviceInfo[CByte])
		  printf("%c", DeviceInfo[CByte]);
	}
	puts_P(PSTR("\r\n"));

	/* Wait for the next response from the device */
	if ((ReturnCode = SImage_WaitForDataReceived()) != NoError)
	  return ReturnCode;

	/* Get the next response header from the device */
	SImage_GetResponse();
		
	/* Finished with the device, clear the pipe to discard unused data */
	Pipe_FIFOCON_Clear();

	/* Second response from the device should be a response block indicating that the command suceeded */
	if ((PIMA_Response.Type != CType_ResponseBlock) || (PIMA_Response.Code != PIMA_RESPONSE_OK))
	  return BadResponse;
	
	/* If this point reached, the command completed sucessfully, return with success code */
	return NoError;
}

uint8_t SImage_OpenCloseSession(uint32_t SessionID, bool Open)
{
	uint8_t ReturnCode;

	PIMA_Command = (PIMA_Container_t)
	{
		DataLength:    (sizeof(PIMA_Container_t) + 4),
		Type:          CType_CommandBlock,
		Code:          ((Open == SESSION_OPEN) ? PIMA_OPERATION_OPENSESSION : PIMA_OPERATION_CLOSESESSION),
		TransactionID: PIMA_TransactionID,
	};
	
	/* Send the PIMA command to the attached device */
	SImage_SendCommand((uint8_t*)&SessionID);
	
	/* Wait until a response is received, return error code if and error occurs */
	if ((ReturnCode = SImage_WaitForDataReceived()) != NoError)
	  return ReturnCode;

	/* Get the response header from the device */
	SImage_GetResponse();
		
	/* Finished with the device, clear the pipe to discard unused data */
	Pipe_FIFOCON_Clear();

	/* First response from the device should be a response block indicating that the command suceeded */
	if ((PIMA_Response.Type != CType_ResponseBlock) || (PIMA_Response.Code != PIMA_RESPONSE_OK))
	  return BadResponse;
	
	/* If this point reached, the command completed sucessfully, return with success code */
	return NoError;
}

