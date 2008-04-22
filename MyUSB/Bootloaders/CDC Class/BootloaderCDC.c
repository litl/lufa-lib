/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	MyUSB USB CDC Bootloader. This bootloader enumerates to the host
	as a CDC Class device, allowing for AVR109 compatible programming
	software to load firmware onto the AVR.
	
	This bootloader is compatible with the open source applications
	AVRDUDE and AVR-OSP.
*/

#include "BootloaderCDC.h"

/* Globals: */
CDC_Line_Coding_t LineCoding = { BaudRateBPS: 9600,
                                 CharFormat:  OneStopBit,
                                 ParityType:  Parity_None,
                                 DataBits:    8            };

bool RunBootloader = true;

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);
	
	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1);

	/* Initialize USB Subsystem */
	USB_Init();

	while (RunBootloader);
	{
		USB_USBTask();
		CDC_Task();
	}
	
	/* Shut down the USB subsystem */
	USB_ShutDown();
	
	/* Relocate the interrupt vector table back to the application section */
	MCUCR = (1 << IVCE);
	MCUCR = 0;

	/* Reset any used hardware ports back to their defaults */
	PORTD = 0;
	DDRD  = 0;
	
	#if defined(PORTE)
	PORTE = 0;
	DDRE  = 0;
	#endif
	
	/* Start the user application */
	AppPtr_t AppStartPtr = 0;
	AppStartPtr();	
}

EVENT_HANDLER(USB_Connect)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup CDC Notification, Rx and Tx Endpoints */
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	uint8_t* LineCodingData = (uint8_t*)&LineCoding;

	Endpoint_Ignore_Word();

	/* Process CDC specific control requests */
	switch (Request)
	{
		case GET_LINE_CODING:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

				for (uint8_t i = 0; i < sizeof(LineCoding); i++)
				  Endpoint_Write_Byte(*(LineCodingData++));	
				
				Endpoint_Setup_In_Clear();
				while (!(Endpoint_Setup_In_IsReady()));
				
				while (!(Endpoint_Setup_Out_IsReceived()));
				Endpoint_Setup_Out_Clear();
			}
			
			break;
		case SET_LINE_CODING:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

				while (!(Endpoint_Setup_Out_IsReceived()));

				for (uint8_t i = 0; i < sizeof(LineCoding); i++)
				  *(LineCodingData++) = Endpoint_Read_Byte();

				Endpoint_Setup_Out_Clear();

				Endpoint_Setup_In_Clear();
				while (!(Endpoint_Setup_In_IsReady()));
			}
	
			break;
		case SET_CONTROL_LINE_STATE:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				Endpoint_Setup_In_Clear();
				while (!(Endpoint_Setup_In_IsReady()));
			}
	
			break;
	}
}

TASK(CDC_Task)
{
	/* Select the OUT endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	/* Check if endpoint has a command in it sent from the host */
	if (Endpoint_ReadWriteAllowed())
	{
		uint8_t Response[2] = {COMMAND_RESPONSE_OK};
		uint8_t ResponseLen = 1;
	
		/* Read in the bootloader command (first byte sent from host) */
		uint8_t Command = Endpoint_Read_Byte();

		switch (Command)
		{
			case 'E': // Exit Bootloader
				RunBootloader = false;
				break;
			case 'L': // Leave Programming Mode
			case 'P': // Enter Programming Mode
				/* No processing required - just return the standard OK response */
				break;
			case 'x': // Set LED
				LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3);
				break;
			case 'y': // Clear LED
				LEDs_SetAllLEDs(LEDS_LED2);
				break;
			case 'e': // Chip Erase
				/* Clear the application section of flash */
				for (uint32_t CurrFlashAddress = 0; CurrFlashAddress < BOOT_START_ADDR; CurrFlashAddress++)
				{
					boot_page_erase(CurrFlashAddress);
					boot_spm_busy_wait();
					boot_page_write(CurrFlashAddress);
					boot_spm_busy_wait();

					CurrFlashAddress += SPM_PAGESIZE;
				}

				/* Re-enable the RWW section of flash as writing to the flash locks it out */
				boot_rww_enable();

				break;
			default:
				Response[0] = COMMAND_RESPONSE_UNKNOWN;
		}

		/* Select the IN endpoint */
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);

		/* Wait until host ready for more data */
		while (!(Endpoint_ReadWriteAllowed()));

		/* Write the response to the endpoint */
		for (uint8_t ResponseByte = 0; ResponseByte < ResponseLen; ResponseByte++)
		  Endpoint_Write_Byte(ResponseByte);

		/* Send the endpoint data to the host */
		Endpoint_FIFOCON_Clear();
	}
}
