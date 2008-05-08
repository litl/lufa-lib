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

	/* Wait until pipe is ready to be read from (contains data) */
	while (!(Pipe_ReadWriteAllowed()))
	{
		/* If USB device is disconnected during transfer, exit function */
		if (!(USB_IsConnected))
		{
			Pipe_Freeze();
			return;
		}
	}
	
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
	uint8_t ReturnCode = NoError;

	PIMA_Command = (PIMA_Container_t)
	{
		DataLength:    (sizeof(PIMA_Container_t) + 4),
		Type:          CType_CommandBlock,
		Code:          PIMA_GETDEVICEINFO,
		TransactionID: PIMA_TransactionID,
	};
	
	uint32_t SessionID = 0;
	
	SImage_SendCommand((uint8_t*)&SessionID);
	
	if (++PIMA_TransactionID == 0xFFFFFFFF)
	  PIMA_TransactionID = 1;
	
	if ((ReturnCode = SImage_WaitForDataReceived()) != NoError)
	  return ReturnCode;

	SImage_GetResponse();

	if (PIMA_Response.Type != CType_DataBlock)
	  return BadResponse;

	uint8_t Temp[PIMA_Response.DataLength];
	SImage_GetData(Temp);
	
	if ((ReturnCode = SImage_WaitForDataReceived()) != NoError)
	  return ReturnCode;

	SImage_GetResponse();

	if (PIMA_Response.Type != CType_ResponseBlock)
	  return BadResponse;
	  
	Pipe_FIFOCON_Clear();

	return ReturnCode;
}
