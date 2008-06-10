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
	
	You will need to format the mass storage device upon first run of this
	demonstration.
*/

/*
	USB Mode:           Device
	USB Class:          Mass Storage Device
	USB Subclass:       Bulk Only
	Relevant Standards: USBIF Mass Storage Standard
	                    USB Bulk-Only Transport Standard
	                    SCSI Primary Commands Specification
	                    SCSI Block Commands Specification
	Usable Speeds:      Full Speed Mode
*/

#define INCLUDE_FROM_MASSSTORAGE_C
#include "MassStorage.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MassStore App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_MassStorage      , TaskStatus: TASK_STOP },
};

/* Global Variables */
CommandBlockWrapper_t  CommandBlock;
CommandStatusWrapper_t CommandStatus = { Header: {Signature: CSW_SIGNATURE } };

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	LEDs_Init();
	Dataflash_Init(SPI_SPEED_FCPU_DIV_2);

	/* Clear Dataflash sector protections, if enabled */
	VirtualMemory_ResetDataflashProtections();
	
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
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

	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running mass storage and USB management tasks */
	Scheduler_SetTaskMode(USB_MassStorage, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Mass Storage In and Out Endpoints */
	Endpoint_ConfigureEndpoint(MASS_STORAGE_IN_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	Endpoint_ConfigureEndpoint(MASS_STORAGE_OUT_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
	
	/* Start mass storage task */
	Scheduler_SetTaskMode(USB_MassStorage, TASK_RUN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process UFI specific control requests */
	switch (bRequest)
	{
		case MASS_STORAGE_RESET:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				Endpoint_ClearSetupIN();
			}

			break;
		case GET_MAX_LUN:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();			
				Endpoint_Write_Byte(0x00);
				Endpoint_ClearSetupIN();
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
			/* Indicate busy */
			LEDs_TurnOnLEDs(LEDS_LED3 | LEDS_LED4);

			/* Process sent command block from the host */
			if (ReadInCommandBlock())
			{
				/* Check direction of command, select Data IN endpoint if data is from the device */
				if (CommandBlock.Header.Flags & COMMAND_DIRECTION_DATA_IN)
				  Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);

				/* Decode the recieved SCSI command */
				SCSI_DecodeSCSICommand();

				/* Load in the CBW tag into the CSW to link them together */
				CommandStatus.Header.Tag = CommandBlock.Header.Tag;

				/* Load in the Command Data residue into the CSW */
				CommandStatus.Header.SCSICommandResidue = CommandBlock.Header.DataTransferLength;

				/* Stall the selected data pipe if command failed (if data is still to be transferred) */
				if ((CommandStatus.Header.Status == Command_Fail) && (CommandStatus.Header.SCSICommandResidue))
				  Endpoint_StallTransaction();

				/* Return command status block to the host */
				ReturnCommandStatus();

				/* Indicate ready */
				LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
			}
			else
			{
				/* Indicate error reading in the command block from the host */
				LEDs_SetAllLEDs(LEDS_LED1);
			}
		}
	}
}

static bool ReadInCommandBlock(void)
{
	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	/* Read in command block header */
	Endpoint_Read_Stream_LE(&CommandBlock.Header, sizeof(CommandBlock.Header));

	/* Verify the command block - abort if invalid */
	if ((CommandBlock.Header.Signature != CBW_SIGNATURE) ||
	    (CommandBlock.Header.LUN != 0x00) ||
		(CommandBlock.Header.SCSICommandLength > MAX_SCSI_COMMAND_LENGTH))
	{
		/* Stall both data pipes until reset by host */
		Endpoint_StallTransaction();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);
		Endpoint_StallTransaction();
		
		return false;
	}

	/* Read in command block command data */
	Endpoint_Read_Stream_LE(&CommandBlock.SCSICommandData, CommandBlock.Header.SCSICommandLength);
	  
	/* Clear the endpoint */
	Endpoint_ClearCurrentBank();
	
	return true;
}

static void ReturnCommandStatus(void)
{
	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);

	/* While data pipe is stalled, process control requests */
	while (Endpoint_IsStalled())
	{
		/* Run the USB task manually to process any received control requests */
		USB_USBTask();
	}

	/* Select the Data In endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPNUM);

	/* While data pipe is stalled, process control requests */
	while (Endpoint_IsStalled())
	{
		/* Run the USB task manually to process any received control requests */
		USB_USBTask();
	}
	
	/* Wait until read/write to IN data endpoint allowed */
	while (!(Endpoint_ReadWriteAllowed()));

	/* Write the CSW to the endpoint */
	Endpoint_Write_Stream_LE(&CommandStatus, sizeof(CommandStatus));
	
	/* Send the CSW */
	Endpoint_ClearCurrentBank();
}
