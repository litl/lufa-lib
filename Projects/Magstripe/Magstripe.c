/*
	Firmware for a USB AVR powered USB TTL magnetic stripe reader (using a card
	reader such as the Omron V3B-4K) by Denver Gingerich. This project is designed
	to be used with the open source Stripe Snoop project at http://stripesnoop.sourceforge.net/.
	
	See http://ossguy.com/ss_usb/ for the USB reader hardware project website,
	including construction and support details.

	To use, connect your magentic card reader device to the USB AVR as follows:
	
	 ---- Signal ---+--- AVR Port ----
	    DATA        |   PORTC, Pin 0
	    CLOCK       |   PORTC, Pin 3
	    CARD LOAD   |   PORTC, Pin 4

	This project is based on the MyUSB Keyboard demonstration application,
	written by Denver Gingerich.
*/

/*
	This demo uses a keyboard HID driver to communicate the data collected
	a TTL magnetic stripe reader to the connected computer. The raw bitstream
	obtained from the magnetic stripe reader is "typed" through the keyboard
	driver as 0's and 1's. After every card swipe, the demo will send a Return key.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Keyboard
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "Magstripe.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB Magstripe App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Keyboard_Report  , TaskStatus: TASK_STOP },
};

/* Global Variables */
USB_KeyboardReport_Data_t KeyboardReportData = {Modifier: 0, KeyCode: 0};


int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	Magstripe_Init();
	LEDs_Init();
	
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
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Report Endpoints */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start Keyboard reporting task */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_RUN);
}

HANDLES_EVENT(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Ignore report type and ID number value */
				Endpoint_Discard_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > sizeof(KeyboardReportData))
				  wLength = sizeof(KeyboardReportData);

				Endpoint_ClearSetupReceived();
	
				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, wLength);
				
				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
			}
		
			break;
	}
}

TASK(USB_Keyboard_Report)
{
	uint8_t        MagStatus_LCL       = Magstripe_GetStatus();	
	uint16_t       StripeDataLen       = 0;
	static uint8_t StripeData[DATA_LENGTH];

	/* Select the LED Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);
	
	/* Throw away any LED reports from the host */
	if (Endpoint_ReadWriteAllowed())
	  Endpoint_ClearCurrentBank();

	/* Abort task if no card inserted */
	if (!(MagStatus_LCL & MAG_CLS))
	{
		/* Indicate no key press to the host */
		Keyboard_SendKeyReport(0);

		return;
	}

	/* While card is inserted, read the data */
	while (MagStatus_LCL & MAG_CLS)
	{
		/* Read card reader pins until clock pin becomes high or the card is removed */
		do
		{
			MagStatus_LCL = Magstripe_GetStatus();
		} while ((MagStatus_LCL & MAG_CLOCK) && (MagStatus_LCL & MAG_CLS));

		/* Abort if card has been removed */
		if (!(MagStatus_LCL & MAG_CLS))
		  break;

		/* Data bit sucessfully retrieved, load the keyboard key code into the buffer */
		StripeData[StripeDataLen++] = ((!(MagStatus_LCL & MAG_DATA)) ? 39 : 30);
		
		/* If buffer full, go back to the beginning */
		StripeDataLen %= DATA_LENGTH;

		/* Read card reader pins until clock pin becomes low or the card is removed */
		do
		{
			MagStatus_LCL = Magstripe_GetStatus();
		} while (!(MagStatus_LCL & MAG_CLOCK) && (MagStatus_LCL & MAG_CLS));
	}

	/* Send the decoded card data from the buffer to the host as a stream of keyboard keycodes */
	for (uint8_t i = 0; i < StripeDataLen; i++)
	{
		Keyboard_SendKeyReport(StripeData[i]);
		Keyboard_SendKeyReport(0);
	}

	/* Terminate data with a newline (return) keycode */
	Keyboard_SendKeyReport(40); // Enter Key
	Keyboard_SendKeyReport(0);
}

void Keyboard_SendKeyReport(uint8_t KeyCode)
{
	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Check if Keyboard Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Keyboard Report Data */
			Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
						
			/* Clear the report data afterwards */
			KeyboardReportData.Modifier = 0;
			KeyboardReportData.KeyCode  = 0;
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
		}
	}
}
