/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
	Mass Storage host demonstration application. This gives a simple reference
	application for implementing a USB Mass Storage host, for USB storage devices
	using the standard Mass Storage USB profile.
	
	The first 512 bytes (boot sector) of an attached disk's memory will be dumped
	out of the serial port when it is attached to the AT90USB1287 AVR.
	
	Requires header files from the Mass Storage Device demonstation application.
*/

/*
	USB Mode:           Host
	USB Class:          Mass Storage Device
	USB Subclass:       Bulk Only
	Relevant Standards: USBIF Mass Storage Standard
	                    USB Bulk-Only Transport Standard
	                    SCSI Primary Commands Specification
	                    SCSI Block Commands Specification
	Usable Speeds:      Full Speed Mode
*/

#include "MassStorageHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MS Host App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_MassStore_Host   , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t MassStore_NumberOfLUNs;

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	SerialStream_Init(9600);
	LEDs_Init();
	
	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "MassStore Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();
	
	/* Scheduling routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	UpdateStatus(Status_USBEnumerating);
	
	/* Start USB management task to enumerate the device */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop USB management and Mass Storage tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_MassStore_Host, TASK_STOP);

	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
	UpdateStatus(Status_USBNotReady);
}

EVENT_HANDLER(USB_DeviceEnumerationComplete)
{
	/* Once device is fully enumerated, start the Mass Storage Host task */
	Scheduler_SetTaskMode(USB_MassStore_Host, TASK_RUN);
	
	/* Indicate device enumeration complete */
	UpdateStatus(Status_USBReady);
}

EVENT_HANDLER(USB_HostError)
{
	USB_ShutDown();

	puts_P(PSTR(ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	UpdateStatus(Status_HardwareError);
	for(;;);
}

EVENT_HANDLER(USB_DeviceEnumerationFailed)
{
	puts_P(PSTR(ESC_BG_RED "Dev Enum Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
	printf_P(PSTR(" -- In State %d\r\n"), USB_HostState);
	
	UpdateStatus(Status_EnumerationError);
}

/** Task to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the StatusCodes_t enum
 */
void UpdateStatus(uint8_t CurrentStatus)
{
	uint8_t LEDMask = LEDS_NO_LEDS;
	
	/* Set the LED mask to the appropriate LED mask based on the given status code */
	switch (CurrentStatus)
	{
		case Status_USBNotReady:
			LEDMask = (LEDS_LED1);
			break;
		case Status_USBEnumerating:
			LEDMask = (LEDS_LED1 | LEDS_LED2);
			break;
		case Status_USBReady:
			LEDMask = (LEDS_LED2);
			break;
		case Status_EnumerationError:
		case Status_HardwareError:
		case Status_SCSICommandError:
			LEDMask = (LEDS_LED1 | LEDS_LED3);
			break;
		case Status_Busy:
			LEDMask = (LEDS_LED1 | LEDS_LED3 | LEDS_LED4);
			break;
	}
	
	/* Set the board LEDs to the new LED mask */
	LEDs_SetAllLEDs(LEDMask);
}

TASK(USB_MassStore_Host)
{
	uint8_t ErrorCode;

	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			/* Standard request to set the device configuration to configuration 1 */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					bmRequestType: (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE),
					bRequest:      REQ_SetConfiguration,
					wValue:        1,
					wIndex:        0,
					wLength:       0,
				};
				
			/* Send the request, display error and wait for device detatch if request fails */
			if ((ErrorCode = USB_Host_SendControlRequest(NULL)) != HOST_SENDCONTROL_Successful)
			{
				puts_P(PSTR("Control Error (Set Configuration).\r\n"));
				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);

				/* Indicate error via status LEDs */
				UpdateStatus(Status_EnumerationError);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
				
			USB_HostState = HOST_STATE_Configured;
			break;
		case HOST_STATE_Configured:
			puts_P(PSTR("Getting Config Data.\r\n"));
		
			/* Get and process the configuration descriptor data */
			if ((ErrorCode = ProcessConfigurationDescriptor()) != SuccessfulConfigRead)
			{
				if (ErrorCode == ControlError)
				  puts_P(PSTR("Control Error (Get Configuration).\r\n"));
				else
				  puts_P(PSTR("Invalid Device.\r\n"));

				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);
				
				/* Indicate error via status LEDs */
				UpdateStatus(Status_EnumerationError);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}

			puts_P(PSTR("Mass Storage Disk Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			UpdateStatus(Status_Busy);
			
			/* Request to prepare the disk for use */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					bmRequestType: (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
					bRequest:      MASS_STORAGE_RESET,
					wValue:        0,
					wIndex:        0,
					wLength:       0,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if ((ErrorCode = USB_Host_SendControlRequest(NULL)) != HOST_SENDCONTROL_Successful)
			{
				puts_P(PSTR("Control error (Mass Storage Reset)."));
				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);

				/* Indicate error via status LEDs */
				UpdateStatus(Status_EnumerationError);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
			
			/* Request to retrieve the maximum LUN index from the device */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					bmRequestType: (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
					bRequest:      GET_MAX_LUN,
					wValue:        0,
					wIndex:        0,
					wLength:       1,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if ((ErrorCode = USB_Host_SendControlRequest(&MassStore_NumberOfLUNs)) != HOST_SENDCONTROL_Successful)
			{
				puts_P(PSTR("Control error (Get Max LUN)."));
				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);

				/* Indicate error via status LEDs */
				UpdateStatus(Status_EnumerationError);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
			
			/* Device indicates the maximum LUN index, the 1-indexed number of LUNs is one more that this */
			MassStore_NumberOfLUNs++;
			
			/* Print number of LUNs detected in the attached device */
			printf_P(PSTR("Total LUNs: %d.\r\n"), MassStore_NumberOfLUNs);

			/* Set the prevent removal flag for the device, allowing it to be accessed */
			MassStore_PreventAllowMediumRemoval(0, true);
			
			/* Get sense data from the device - many devices will not accept any other commands until the sense data
			 * is read - both on startup and after a failed command */
			SCSI_Request_Sense_Response_t SenseData;
			if ((ErrorCode = MassStore_RequestSense(0, &SenseData)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}

			puts_P(PSTR("Waiting until ready"));
			
			/* Wait until disk ready */
			do
			{
				Serial_TxByte('.');
				MassStore_TestUnitReady(0);
			}
			while ((SCSICommandStatus.Status != Command_Pass) && USB_IsConnected);
			
			/* Abort if device removed */
			if (!(USB_IsConnected))
			  break;

			puts_P(PSTR("\r\nRetrieving Capacity.\r\n"));

			/* Create new structure for the disk's capacity in blocks and block size */
			SCSI_Capacity_t DiskCapacity;

			/* Retrieve disk capacity */
			if ((ErrorCode = MassStore_ReadCapacity(0, &DiskCapacity)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}
			
			/* Display the disk capacity in blocks * block size bytes */
			printf_P(PSTR("Capacity: %lu*%lu bytes.\r\n"), DiskCapacity.Blocks, DiskCapacity.BlockSize);
			
			/* Create a new buffer capabable of holding a single block from the device */
			uint8_t BlockBuffer[DEVICE_BLOCK_SIZE];

			/* Read in the first 512 byte block from the device */
			if ((ErrorCode = MassStore_ReadDeviceBlock(0, 0, 1, BlockBuffer)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}
			
			puts_P(PSTR("Contents of first block:\r\n"));
			
			/* Print the block bytes out through the serial USART */
			for (uint16_t Byte = 0; Byte < DEVICE_BLOCK_SIZE; Byte++)
			  printf_P(PSTR("0x%.2X "), BlockBuffer[Byte]);
			
			puts_P(PSTR("\r\n\r\nASCII Data:\r\n"));
			
			/* Do the same in ASCII characters */
			for (uint16_t Byte = 0; Byte < DEVICE_BLOCK_SIZE; Byte++)
			{
				if ((BlockBuffer[Byte] >= '0' && BlockBuffer[Byte] <= '9') ||
				    (BlockBuffer[Byte] >= 'a' && BlockBuffer[Byte] <= 'z') ||
				    (BlockBuffer[Byte] >= 'A' && BlockBuffer[Byte] <= 'Z'))
				{
					printf_P(PSTR("%c"), BlockBuffer[Byte]);
				}
				else
				{
					printf_P(PSTR("%c"), '.');				
				}
			}

			/* Indicate device no longer busy */
			UpdateStatus(Status_USBReady);
			
			/* Wait until USB device disconnected */
			while (USB_IsConnected);
			
			break;
	}
}

void ShowDiskReadError(uint8_t ErrorCode)
{
	/* Display the error code */
	puts_P(PSTR(ESC_BG_RED "Command error.\r\n"));
	printf_P(PSTR("  -- Error Code: %d"), ErrorCode);

	/* Indicate device error via the status LEDs */
	UpdateStatus(Status_SCSICommandError);

	/* Wait until USB device disconnected */
	while (USB_IsConnected);
}
