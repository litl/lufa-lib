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
		/* Read in the bootloader command (first byte sent from host) */
		uint8_t Command = Endpoint_Read_Byte();

		/* Select the IN endpoint */
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);

		/* Wait until host ready for more data */
		while (!(Endpoint_ReadWriteAllowed()));

		switch (Command)
		{
			case 'E': // Exit Bootloader
				RunBootloader = false;
			case 'L': // Leave Programming Mode
			case 'P': // Enter Programming Mode
			case 'T': // Set Device Type
				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'x': // Set LED
				/* Turn on the secondary LED */
				LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3);

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'y': // Clear LED
				/* Turn off the secondary LED */
				LEDs_SetAllLEDs(LEDS_LED2);

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 't': // Get Supported Part Codes
				/* Return ATMEGA128 part code - this is only to allow AVRProg to use the bootloader */
				Endpoint_Write_Byte(0x44);
				Endpoint_Write_Byte(0x00);
				break;
			case 'a': // Check Auto-Increment
				/* Indicate auto-address increment is supported */
				Endpoint_Write_Byte('Y');
				break;
			case 'A': // Set Address
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);

				/* Set the current address to that given by the host */
				CurrAddress = Endpoint_Read_Word_LE();

				Endpoint_SelectEndpoint(CDC_TX_EPNUM);

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'p': // Get Programmer Type
				/* Indicate serial programmer back to the host */
				Endpoint_Write_Byte('S');
				break;
			case 'S': // Get Software Identifier
				/* Write the 7-byte software identifier to the endpoint */
				for (uint8_t CurrByte = 0; CurrByte < sizeof(SOFTWARE_IDENTIFIER); CurrByte++)
				  Endpoint_Write_Byte(SOFTWARE_IDENTIFIER[CurrByte]);

				break;
			case 'V': // Get Software Version
				Endpoint_Write_Word_LE((BOOTLOADER_VERSION_MAJOR << 8) | BOOTLOADER_VERSION_MINOR);
				break;
			case 's': // Get Signature Bytes
				Endpoint_Write_Byte(boot_signature_byte_get(0));
				Endpoint_Write_Byte(boot_signature_byte_get(2));
				Endpoint_Write_Byte(boot_signature_byte_get(4));
				
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

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'l': // Write Lock Bits
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);

				/* Set the lock bits to those given by the host */
				boot_lock_bits_set(Endpoint_Read_Byte());

				Endpoint_SelectEndpoint(CDC_TX_EPNUM);

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'r': // Read Lock Bits
				Endpoint_Write_Byte(boot_lock_fuse_bits_get(GET_LOCK_BITS));
				break;
			case 'F': // Read Fuse Bits
				Endpoint_Write_Byte(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
				break;
			case 'N': // Read High Fuse Bits
				Endpoint_Write_Byte(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
				break;
			case 'Q': // Read Extended Fuse Bits
				Endpoint_Write_Byte(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
				break;
			case 'b': // Check Block Support
				/* Indicate to the host that block mode is supported */
				Endpoint_Write_Byte('Y');
				
				/* Send block size to the host */
				Endpoint_Write_Byte(SPM_PAGESIZE >> 8);
				Endpoint_Write_Byte(SPM_PAGESIZE & 0xFF);
				break;
			case 'C': // Write Program Memory, Low Byte
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);
				
				/* Write the low byte to the current flash page */
				boot_page_fill(((uint32_t)CurrAddress << 1), Endpoint_Read_Byte());
				
				Endpoint_SelectEndpoint(CDC_TX_EPNUM);
				
				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'c': // Write Program Memory, High Byte
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);
				
				/* Write the high byte to the current flash page */
				boot_page_fill(((uint32_t)CurrAddress << 1) + 1, Endpoint_Read_Byte());
				
				/* Increment the address after use */			
				CurrAddress++;
				
				Endpoint_SelectEndpoint(CDC_TX_EPNUM);
				
				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'm': // Issue Page Write
				/* Commit the flash page to memory */
				boot_page_write((uint32_t)CurrAddress << 1);
				
				/* Wait until write operation has completed */
				boot_spm_busy_wait();
				
				/* Re-enable RWW section after write command */
				boot_rww_enable();

				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'B': // Start Block FLASH/EEPROM Load
			case 'g': // Start Block FLASH/EEPROM Read
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);
				
				uint16_t BlockSize  = Endpoint_Read_Word_LE();
				char     MemoryType = Endpoint_Read_Byte();
				
				if ((MemoryType == 'E') || (MemoryType == 'F'))
				{
					/* Check if command is to read memory */
					if (Command == 'g')
					{
						Endpoint_SelectEndpoint(CDC_TX_EPNUM);

						while (BlockSize--)
						{
							if (Endpoint_BytesInEndpoint() == CDC_TXRX_EPSIZE)
							{
								/* Clear the endpoint bank */
								Endpoint_FIFOCON_Clear();
									
								/* Wait until ready to write next packet */
								while (!(Endpoint_ReadWriteAllowed()));
							}

							if (MemoryType == 'E')
							{
								/* Read the next EEPROM byte into the endpoint */
								Endpoint_Write_Byte(eeprom_read_byte(CurrAddress));
									
								/* Increment the address counter after use */
								CurrAddress++;
							}
							else
							{
								/* Read the next FLASH word from the current FLASH page */
								Endpoint_Write_Word_LE(pgm_read_word_far(((uint32_t)CurrAddress << 1)));
								
								/* Increment the address counter by one word size after use */
								CurrAddress += 2;							
							}
						}
					}
					else
					{
						while (BlockSize--)
						{
							if (!(Endpoint_BytesInEndpoint()))
							{
								/* Clear the endpoint bank */
								Endpoint_FIFOCON_Clear();
								
								/* Wait until next packet of data has been received */
								while (!(Endpoint_ReadWriteAllowed()));
							}
							
							if (MemoryType == 'E')
							{
								/* Write the next EEPROM byte from the endpoint */
								eeprom_write_byte(CurrAddress, Endpoint_Read_Byte());
								
								/* Increment the address counter after use */
								CurrAddress++;
							}
							else
							{
								/* Write the next FLASH word to the current FLASH page */
								boot_page_fill(((uint32_t)CurrAddress << 1), Endpoint_Read_Word_LE());
								
								/* Increment the address counter by one word size after use */
								CurrAddress += 2;
							}
						}

						/* In in FLASH programming mode, commit the page after writing */
						if (MemoryType == 'F')
						{
							/* Commit the flash page to memory */
							boot_page_write((uint32_t)CurrAddress << 1);
							
							/* Wait until write operation has completed */
							boot_spm_busy_wait();
							
							/* Re-enable RWW section after write command */
							boot_rww_enable();
						}
					
						Endpoint_SelectEndpoint(CDC_TX_EPNUM);
							
						/* Send response byte back to the host */
						Endpoint_Write_Byte('\r');					
					}
				}
				else
				{
					Endpoint_SelectEndpoint(CDC_TX_EPNUM);
						
					/* Send error byte back to the host */
					Endpoint_Write_Byte('?');
				}

				break;
			case 'R': // Read Program Memory
				Endpoint_Write_Word_LE(pgm_read_word_far(((uint32_t)CurrAddress << 1)));
				break;
			case 'D': // Write EEPROM Memory
				Endpoint_SelectEndpoint(CDC_RX_EPNUM);

				/* Read the byte from the endpoint and write it to the EEPROM */
				eeprom_write_byte((uint8_t*)CurrAddress, Endpoint_Read_Byte());
			
				/* Increment the address after use */			
				CurrAddress++;
	
				Endpoint_SelectEndpoint(CDC_TX_EPNUM);
				
				/* Send confirmation byte back to the host */
				Endpoint_Write_Byte('\r');
				break;
			case 'd': // Read EEPROM Memory
				/* Read the EEPROM byte and write it to the endpoint */
				Endpoint_Write_Byte(eeprom_read_byte((uint8_t*)CurrAddress));

				/* Increment the address after use */
				CurrAddress++;
				break;				
			default:
				/* Send unknown command byte back to the host */
				Endpoint_Write_Byte('?');
		}

		/* Send the endpoint data to the host */
		Endpoint_FIFOCON_Clear();
	}
}
