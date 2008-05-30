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
	
	This bootloader is compatible with the open source application
	AVRDUDE.
*/

#define  INCLUDE_FROM_BOOTLOADERCDC_C
#include "BootloaderCDC.h"

/* Globals: */
CDC_Line_Coding_t LineCoding = { BaudRateBPS: 9600,
                                 CharFormat:  OneStopBit,
                                 ParityType:  Parity_None,
                                 DataBits:    8            };
								 
uint16_t          CurrAddress;

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

	while (RunBootloader)
	{
		USB_USBTask();
		CDC_Task();
	}
	
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);

	/* Wait until any pending transmissions have completed before shutting down */
	while (!(Endpoint_ReadWriteAllowed()));
	
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
	
	/* Re-enable RWW section */
	boot_rww_enable();

	/* Start the user application */
	AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
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

EVENT_HANDLER(USB_ConfigurationChanged)
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
				
				Endpoint_ClearSetupIN();
				while (!(Endpoint_IsSetupINReady()));
				
				while (!(Endpoint_IsSetupOUTReceived()));
				Endpoint_ClearSetupOUT();
			}
			
			break;
		case SET_LINE_CODING:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();

				while (!(Endpoint_IsSetupOUTReceived()));

				for (uint8_t i = 0; i < sizeof(LineCoding); i++)
				  *(LineCodingData++) = Endpoint_Read_Byte();

				Endpoint_ClearSetupOUT();

				Endpoint_ClearSetupIN();
				while (!(Endpoint_IsSetupINReady()));
			}
	
			break;
		case SET_CONTROL_LINE_STATE:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				Endpoint_ClearSetupIN();
				while (!(Endpoint_IsSetupINReady()));
			}
	
			break;
	}
}

static void ProgramReadMemoryBlock(const uint8_t Command)
{
	uint16_t BlockSize;
	char     MemoryType;
	
	bool     HighByte = false;
	uint8_t  LowByte  = 0;
	
	BlockSize  = (FetchNextCommandByte() << 8);
	BlockSize |=  FetchNextCommandByte();
	
	MemoryType =  FetchNextCommandByte();

	if ((MemoryType == 'E') || (MemoryType == 'F'))
	{
		/* Check if command is to read memory */
		if (Command == 'g')
		{
			/* Re-enable RWW section */
			boot_rww_enable();

			while (BlockSize--)
			{
				if (MemoryType == 'E')
				{
					/* Read the next EEPROM byte into the endpoint */
					WriteNextResponseByte(eeprom_read_byte((uint8_t*)CurrAddress));

					/* Increment the address counter after use */
					CurrAddress++;
				}
				else
				{
					/* Read the next FLASH byte from the current FLASH page */
					WriteNextResponseByte(pgm_read_byte_far(((uint32_t)CurrAddress << 1) + HighByte));
					
					/* If both bytes in current word have been read, increment the address counter */
					if (HighByte)
					  CurrAddress++;
					
					HighByte ^= 1;
				}
			}
		}
		else
		{
			uint32_t PageStartAddress = ((uint32_t)CurrAddress << 1);
	
			if (MemoryType == 'F')
			{
				boot_page_erase(PageStartAddress);
				boot_spm_busy_wait();
			}
			
			while (BlockSize--)
			{
				if (MemoryType == 'E')
				{
					/* Write the next EEPROM byte from the endpoint */
					eeprom_write_byte((uint8_t*)CurrAddress, FetchNextCommandByte());					

					/* Increment the address counter after use */
					CurrAddress++;
				}
				else
				{	
					/* If both bytes in current word have been written, increment the address counter */
					if (HighByte)
					{
						/* Write the next FLASH word to the current FLASH page */
						boot_page_fill(((uint32_t)CurrAddress << 1), ((FetchNextCommandByte() << 8) | LowByte));

						HighByte = false;
						
						/* Increment the address counter after use */
						CurrAddress++;
					}
					else
					{
						LowByte = FetchNextCommandByte();
					
						HighByte = true;
					}
				}
			}

			/* If in FLASH programming mode, commit the page after writing */
			if (MemoryType == 'F')
			{
				/* Commit the flash page to memory */
				boot_page_write(PageStartAddress);
				
				/* Wait until write operation has completed */
				boot_spm_busy_wait();
			}
		
			/* Send response byte back to the host */
			WriteNextResponseByte('\r');		
		}
	}
	else
	{
		/* Send error byte back to the host */
		WriteNextResponseByte('?');
	}
}

static uint8_t FetchNextCommandByte(void)
{
	/* Select the OUT endpoint so that the next data byte can be read */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	/* If OUT endpoint empty, clear it and wait for the next packet from the host */
	if (!(Endpoint_BytesInEndpoint()))
	{
		Endpoint_ClearCurrentBank();
		while (!(Endpoint_ReadWriteAllowed()));
	}
	
	/* Fetch the next byte from the OUT endpoint */
	return Endpoint_Read_Byte();
}

static void WriteNextResponseByte(const uint8_t Response)
{
	/* Select the IN endpoint so that the next data byte can be written */
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	
	/* If OUT endpoint empty, clear it and wait for the next packet from the host */
	if (Endpoint_BytesInEndpoint() == CDC_TXRX_EPSIZE)
	{
		Endpoint_ClearCurrentBank();
		while (!(Endpoint_ReadWriteAllowed()));
	}
	
	/* Write the next byte to the OUT endpoint */
	Endpoint_Write_Byte(Response);
}

TASK(CDC_Task)
{
	/* Select the OUT endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	/* Check if endpoint has a command in it sent from the host */
	if (Endpoint_ReadWriteAllowed())
	{
		/* Read in the bootloader command (first byte sent from host) */
		uint8_t Command = FetchNextCommandByte();

		if ((Command == 'L') || (Command == 'P') || (Command == 'T') || (Command == 'E'))
		{
			if (Command == 'E')
			  RunBootloader = false;
			if (Command == 'T')
			  FetchNextCommandByte();

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');			
		}
		else if (Command == 'x')
		{
			/* Turn on the secondary LED */
			LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3);

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');
		}
		else if (Command == 'y')
		{
			/* Turn off the secondary LED */
			LEDs_SetAllLEDs(LEDS_LED2);

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');		
		}
		else if (Command == 't')
		{
			/* Return ATMEGA128 part code - this is only to allow AVRProg to use the bootloader */
			WriteNextResponseByte(0x44);

			WriteNextResponseByte(0x00);
		}
		else if (Command == 'a')
		{
			/* Indicate auto-address increment is supported */
			WriteNextResponseByte('Y');
		}
		else if (Command == 'A')
		{
			/* Set the current address to that given by the host */
			CurrAddress  = (FetchNextCommandByte() << 8);
			CurrAddress |=  FetchNextCommandByte();

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');
		}
		else if (Command == 'p')
		{
			/* Indicate serial programmer back to the host */
			WriteNextResponseByte('S');		 
		}
		else if (Command == 'S')
		{
			/* Write the 7-byte software identifier to the endpoint */
			for (uint8_t CurrByte = 0; CurrByte < 7; CurrByte++)
			  WriteNextResponseByte(SOFTWARE_IDENTIFIER[CurrByte]);		
		}
		else if (Command == 'V')
		{
			WriteNextResponseByte('0' + BOOTLOADER_VERSION_MAJOR);
			WriteNextResponseByte('0' + BOOTLOADER_VERSION_MINOR);
		}
		else if (Command == 's')
		{
			WriteNextResponseByte(boot_signature_byte_get(4));
			WriteNextResponseByte(boot_signature_byte_get(2));
			WriteNextResponseByte(boot_signature_byte_get(0));		
		}
		else if (Command == 'b')
		{
			WriteNextResponseByte('Y');
				
			/* Send block size to the host */
			WriteNextResponseByte(SPM_PAGESIZE >> 8);
			WriteNextResponseByte(SPM_PAGESIZE & 0xFF);		
		}
		else if (Command == 'e')
		{
			/* Clear the application section of flash */
			for (uint32_t CurrFlashAddress = 0; CurrFlashAddress < BOOT_START_ADDR; CurrFlashAddress++)
			{
				boot_page_erase(CurrFlashAddress);
				boot_spm_busy_wait();
				boot_page_write(CurrFlashAddress);
				boot_spm_busy_wait();

				CurrFlashAddress += SPM_PAGESIZE;
			}
			
			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');		
		}
		else if (Command == 'l')
		{
			/* Set the lock bits to those given by the host */
			boot_lock_bits_set(FetchNextCommandByte());

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');
		}
		else if (Command == 'r')
		{
			WriteNextResponseByte(boot_lock_fuse_bits_get(GET_LOCK_BITS));		
		}
		else if (Command == 'F')
		{
			WriteNextResponseByte(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
		}
		else if (Command == 'N')
		{
			WriteNextResponseByte(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));		
		}
		else if (Command == 'Q')
		{
			WriteNextResponseByte(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));		
		}
		else if ((Command == 'C') || (Command == 'c'))
		{
			if (Command == 'c')
			{
				/* Increment the address if the second byte is being written */
				CurrAddress++;
			}
			
			/* Write the high byte to the current flash page */
			boot_page_fill(((uint32_t)CurrAddress << 1), FetchNextCommandByte());
			
			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');		
		}
		else if (Command == 'm')
		{
			/* Commit the flash page to memory */
			boot_page_write((uint32_t)CurrAddress << 1);
			
			/* Wait until write operation has completed */
			boot_spm_busy_wait();

			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');		
		}
		else if ((Command == 'B') || (Command == 'g'))
		{
			/* Delegate the block write/read to a seperate function for clarity */
			ProgramReadMemoryBlock(Command);
		}
		else if (Command == 'R')
		{
			uint16_t ProgramWord = pgm_read_word_far(((uint32_t)CurrAddress << 1));
		
			WriteNextResponseByte(ProgramWord >> 8);
			WriteNextResponseByte(ProgramWord & 0xFF);
		}
		else if (Command == 'D')
		{
			/* Read the byte from the endpoint and write it to the EEPROM */
			eeprom_write_byte((uint8_t*)CurrAddress, FetchNextCommandByte());
			
			/* Increment the address after use */			
			CurrAddress++;
	
			/* Send confirmation byte back to the host */
			WriteNextResponseByte('\r');		
		}
		else if (Command == 'd')
		{
			/* Read the EEPROM byte and write it to the endpoint */
			WriteNextResponseByte(eeprom_read_byte((uint8_t*)CurrAddress));

			/* Increment the address after use */
			CurrAddress++;
		}
		else if (Command == 27)
		{
			/* Escape is sync, ignore */
		}
		else
		{
			/* Unknown command, return fail code */
			WriteNextResponseByte('?');
		}

		/* Select the IN endpoint */
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);

		/* Remeber if the endpoint is completely full before clearing it */
		bool IsEndpointFull = !(Endpoint_ReadWriteAllowed());

		/* Send the endpoint data to the host */
		Endpoint_ClearCurrentBank();
		
		/* If a full endpoint's worth of data was sent, we need to send an empty packet afterwards to signal end of transfer */
		if (IsEndpointFull)
		{
			while (!(Endpoint_ReadWriteAllowed()));
			Endpoint_ClearCurrentBank();
		}
		
		/* Select the OUT endpoint */
		Endpoint_SelectEndpoint(CDC_RX_EPNUM);

		/* Acknowledge the command from the host */
		Endpoint_ClearCurrentBank();
	}
}

