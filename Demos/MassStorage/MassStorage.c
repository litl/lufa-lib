/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Mass Storage demonstration application. This gives a simple reference
	application for implementing a USB Mass Storage device using the basic
	USB UFI drivers in all modern OSes (i.e. no special drivers required).
	
	On startup the system will automatically enumerate and function as an
	external mass storage device which may be formatted and used in the
	same manner as commercial USB Mass Storage devices.
	
	Only one Logical Unit (LUN) is currently supported, allowing for one
	external storage device.
*/

/*
		---=== UNFINISHED AND NON-OPERATIONAL ===---
*/

#include "MassStorage.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB MassStore App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_MassStorage_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_MassStorage_ID      , TaskName: USB_MassStorage      , TaskStatus: TASK_RUN  },
};

/* Global Variables */
CommandBlockWrapper_t  CommandBlock;
CommandStatusWrapper_t CommandStatus = { Signature: CSW_SIGNATURE };

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Mass Storage In and Out Endpoints */
	Endpoint_ConfigureEndpoint(MASS_STORAGE_IN_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	Endpoint_ConfigureEndpoint(MASS_STORAGE_OUT_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	USB_Device_Ignore_Word();

	/* Process UFI specific control requests */
	switch (Request)
	{
		case MASS_STORAGE_RESET:
			Endpoint_ClearSetupRecieved();
			
			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));

			break;
	}
}
	
TASK(USB_MassStorage)
{
	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Data Out Endpoint */
		Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);
		
		/* Check to see if a command from the host has been issued */
		if (Endpoint_Out_IsRecieved())
		{	
			/* Bicolour LEDs to green/orange - busy */
			Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE);

			/* Process sent command block from the host */
			ProcessCommandBlock();

			/* Return command status block to the host */
			ReturnCommandStatus();
		}
	}
}

void ProcessCommandBlock(void)
{
	uint8_t* CommandBlockPtr = (uint8_t*)&CommandBlock;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	/* Read in command block header */
	for (uint8_t i = 0; i < sizeof(CommandBlock.Header); i++)
	  *(CommandBlockPtr++) = USB_Device_Read_Byte();

	/* Verify the command block - abort if invalid */
	if ((CommandBlock.Header.Signature != CBW_SIGNATURE) ||
	    (CommandBlock.Header.LUN != 0x00) ||
		(CommandBlock.Header.CommandLength > 16))
	{
		/* Bicolour LEDs to green/red - error */
		Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);

		/* Stall both data pipes until reset by host */
		Endpoint_Stall_Transaction();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
		Endpoint_Stall_Transaction();
		
		return;
	}

	/* Read in command block command data */
	for (uint8_t b = 0; b < CommandBlock.Header.CommandLength; b++)
	  *(CommandBlockPtr++) = USB_Device_Read_Byte();
	  
	/* Clear the endpoint */
	Endpoint_Out_Clear();

	/* Decode the recieved SCSI command */
	DecodeSCSICommand();
}

void ReturnCommandStatus(void)
{
	uint8_t* CommandStatusPtr = (uint8_t*)&CommandStatus;

	/* Select the Data In endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);

	/* Load in the CBW tag into the CSW */
	CommandStatus.Tag = CommandBlock.Header.Tag;
	
	/* Write the CSW to the endpoint */
	for (uint8_t i = 0; i < sizeof(CommandStatus); i++)
	  USB_Device_Write_Byte(*(CommandStatusPtr++));
	
	/* Send the CSW */
	Endpoint_In_Clear();

	/* Bicolour LEDs to green/green - ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

