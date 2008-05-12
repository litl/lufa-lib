/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "StillImageCommands.h"

/* Globals: */
PIMA_Container_t PIMA_SendBlock;
PIMA_Container_t PIMA_RecievedBlock;
PIMA_Container_t PIMA_EventBlock;

void SImage_SendBlockHeader(void)
{
	/* Unfreeze the data OUT pipe ready for data transmission */
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	/* Write the PIMA block to the data OUT pipe */
	Pipe_Write_Stream_LE(&PIMA_SendBlock, PIMA_COMMAND_SIZE(0));
	
	/* Command blocks have associated parameters */
	if (PIMA_SendBlock.Type == CType_CommandBlock)
	{
		uint8_t ParamBytes = (PIMA_SendBlock.DataLength - PIMA_COMMAND_SIZE(0));

		if (ParamBytes)
		{
			/* Write the PIMA parameters to the data OUT pipe */
			Pipe_Write_Stream_LE(&PIMA_SendBlock.Params, ParamBytes);
		}
		
		/* Send the PIMA command block to the attached device */
		Pipe_FIFOCON_Clear();
	}
					
	/* Freeze pipe after use */
	Pipe_Freeze();
}

void SImage_RecieveEventHeader(void)
{
	Pipe_SelectPipe(SIMAGE_EVENTS_PIPE);
	Pipe_Unfreeze();
	
	Pipe_Read_Stream_LE(&PIMA_EventBlock, sizeof(PIMA_EventBlock));
	
	Pipe_FIFOCON_Clear();
	
	Pipe_Freeze();
}

uint8_t SImage_RecieveBlockHeader(void)
{
	uint16_t TimeoutMSRem = COMMAND_DATA_TIMEOUT_MS;

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
				/* Return error code */
				return CommandTimeout;
			}
		}
		
		Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Clear the stall condition on the OUT pipe */
			SImage_ClearPipeStall(SIMAGE_DATA_OUT_PIPE);

			/* Return error code and break out of the loop */
			return OutPipeStalled;
		}

		Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);

		/* Check if pipe stalled (command failed by device) */
		if (Pipe_IsStalled())
		{
			/* Clear the stall condition on the IN pipe */
			SImage_ClearPipeStall(SIMAGE_DATA_IN_PIPE);

			/* Return error code */
			return InPipeStalled;
		}
		  
		/* Check to see if the device was disconnected, if so exit function */
		if (!(USB_IsConnected))
		{
			/* Return error code */
			return DeviceDisconnected;
		}
	};
	
	/* Freeze OUT pipe after use */
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Freeze();

	/* Select the IN data pipe for data reception */
	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	
	/* Load in the response from the attached device */
	Pipe_Read_Stream_LE(&PIMA_RecievedBlock, PIMA_COMMAND_SIZE(0));
	
	/* Response blocks blocks have associated parameters */
	if (PIMA_RecievedBlock.Type == CType_ResponseBlock)
	{
		uint8_t ParamBytes = (PIMA_RecievedBlock.DataLength - PIMA_COMMAND_SIZE(0));

		/* Check if the device has returned any parameters */
		if (ParamBytes)
		{
			/* Read the PIMA parameters from the data IN pipe */
			Pipe_Read_Stream_LE(&PIMA_RecievedBlock.Params, ParamBytes);
		}
		
		/* Clear pipe banks after use */
		Pipe_FIFOCON_Clear();
		Pipe_FIFOCON_Clear();
	}
	
	/* Freeze the IN pipe after use */
	Pipe_Freeze();
	
	return NoError;
}

void SImage_SendData(void* Buffer, uint16_t Bytes)
{
	Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	Pipe_Write_Stream_LE(Buffer, Bytes);

	Pipe_FIFOCON_Clear();

	Pipe_Freeze();
}

uint8_t SImage_ReadData(void* Buffer, uint16_t Bytes)
{
	uint8_t ErrorCode = NoError;

	Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
	Pipe_Unfreeze();

	ErrorCode = Pipe_Read_Stream_LE(Buffer, Bytes);

	Pipe_FIFOCON_Clear();

	Pipe_Freeze();
	
	return ErrorCode;
}

bool SImage_IsEventReceived(void)
{
	bool IsEventReceived = false;

	Pipe_SelectPipe(SIMAGE_EVENTS_PIPE);
	Pipe_Unfreeze();
	
	if (Pipe_ReadWriteAllowed())
	  IsEventReceived = true;
	
	Pipe_Freeze();
	
	return IsEventReceived;
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
