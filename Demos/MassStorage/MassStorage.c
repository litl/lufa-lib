/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Mass Storage demonstration application. This gives a simple reference
	application for implementing a USB Mass Storage device using the basic
	USB UFI drivers in all modern OSes (i.e. no special drivers required).
	
	On startup the system will automatically enumerate and function as an
	external mass storage device which may be formatted and used in the
	same manner as commercial USB Mass Storage devices.
	
	Only one Logical Unit (LUN) is currently supported by this example,
	allowing for one external storage device to be enumerated by the host.
	
	The two USB status LEDs indicate the status of the device. The first
	LED is lit in green when the device may be removed from the host, and
	red when the host is busy writing to or reading from the device. The
	second LED is lit in green when idling, orange when executing a command
	from the host and red when the host send an invalid USB command.
	
	You will need to format the mass storage device upon first run of this
	demonstration.
*/

#define INCLUDE_FROM_MASSSTORAGE_C
#include "MassStorage.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB MassStore App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_MassStorage      , TaskStatus: TASK_STOP },
};

/* Global Variables */
CommandBlockWrapper_t  CommandBlock;
CommandStatusWrapper_t CommandStatus = { Signature: CSW_SIGNATURE };

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	wdt_disable();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	Dataflash_Init(DATAFLASH_SPEED_FCPU_DIV_2);

	/* Clear Dataflash Section Protectioms, if enabled */
	VirtualMemory_ResetDataflashProtections();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Red/green to indicate USB enumerating */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running mass storage and USB management tasks */
	Scheduler_SetTaskMode(USB_MassStorage, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Mass Storage In and Out Endpoints */
	Endpoint_ConfigureEndpoint(MASS_STORAGE_IN_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	Endpoint_ConfigureEndpoint(MASS_STORAGE_OUT_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
	
	/* Start mass storage reporting task */
	Scheduler_SetTaskMode(USB_MassStorage, TASK_RUN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process UFI specific control requests */
	switch (Request)
	{
		case MASS_STORAGE_RESET:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				Endpoint_Setup_In_Clear();
			}

			break;
		case GET_MAX_LUN:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();			
				Endpoint_Write_Byte(0x00);
				Endpoint_Setup_In_Clear();
			}
			
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
		if (Endpoint_ReadWriteAllowed())
		{	
			/* Set LED2 orange - busy */
			Bicolour_SetLed(BICOLOUR_LED2, BICOLOUR_LED2_ORANGE);

			/* Process sent command block from the host */
			ProcessCommandBlock();

			/* Load in the CBW tag into the CSW to link them together */
			CommandStatus.Tag = CommandBlock.Header.Tag;

			/* Return command status block to the host */
			ReturnCommandStatus();
		}
	}
}

static void ProcessCommandBlock(void)
{
	uint8_t* CommandBlockPtr = (uint8_t*)&CommandBlock;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	/* Read in command block header */
	for (uint8_t i = 0; i < sizeof(CommandBlock.Header); i++)
	  *(CommandBlockPtr++) = Endpoint_Read_Byte();

	/* Verify the command block - abort if invalid */
	if ((CommandBlock.Header.Signature != CBW_SIGNATURE) ||
	    (CommandBlock.Header.LUN != 0x00) ||
		(CommandBlock.Header.SCSICommandLength > MAX_SCSI_COMMAND_LENGTH))
	{
		/* Bicolour LED2 to red - error */
		Bicolour_SetLed(BICOLOUR_LED2, BICOLOUR_LED2_RED);

		/* Stall both data pipes until reset by host */
		Endpoint_StallTransaction();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
		Endpoint_StallTransaction();
		
		return;
	}

	/* Read in command block command data */
	for (uint8_t b = 0; b < CommandBlock.Header.SCSICommandLength; b++)
	  *(CommandBlockPtr++) = Endpoint_Read_Byte();
	  
	/* Clear the endpoint */
	Endpoint_FIFOCON_Clear();

	/* Check direction of command, select Data IN endpoint if data is from the device */
	if (CommandBlock.Header.Flags & COMMAND_DIRECTION_DATA_IN)
	  Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);

	/* Decode the recieved SCSI command */
	SCSI_DecodeSCSICommand();

	/* Load in the Command Data residue into the CSW */
	CommandStatus.SCSICommandResidue = CommandBlock.Header.DataTransferLength;

	/* Stall data pipe if command failed */
	if ((CommandStatus.Status == Command_Fail) &&
	    (CommandStatus.SCSICommandResidue))
	{
		Endpoint_StallTransaction();
	}
}

static void ReturnCommandStatus(void)
{
	uint8_t* CommandStatusPtr = (uint8_t*)&CommandStatus;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	/* While data pipe is stalled, process control requests */
	while (Endpoint_IsStalled())
	{
		USB_USBTask();
		Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);
	}

	/* Select the Data In endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);

	/* While data pipe is stalled, process control requests */
	while (Endpoint_IsStalled())
	{
		USB_USBTask();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
	}
	
	/* Wait until read/write to IN data endpoint allowed */
	while (!(Endpoint_ReadWriteAllowed()));

	/* Write the CSW to the endpoint */
	for (uint8_t i = 0; i < sizeof(CommandStatus); i++)
	  Endpoint_Write_Byte(*(CommandStatusPtr++));
	
	/* Send the CSW */
	Endpoint_FIFOCON_Clear();

	/* Bicolour LED2 to green - ready */
	Bicolour_SetLed(BICOLOUR_LED2, BICOLOUR_LED2_GREEN);
}
