/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	MyUSB USB DFU Bootloader. This bootloader enumerates to the host
	as a DFU Class device, allowing for DFU-compatible programming
	software to load firmware onto the AVR.
	
	This bootloader is compatible with Atmel's FLIP application.
	However, it requires the use of Atmel's DFU drivers. You will
	need to install Atmel's DFU drivers prior to using this bootloader.
	
	As an open-source option, this bootloader is also compatible
	with the Linux Atmel USB DFU Programmer software, available
	for download at http://sourceforge.net/projects/dfu-programmer/.
	
    If SECURE_MODE is defined as true, upon startup the bootloader will
	be locked, with only the chip erase function avaliable (similar to
    Atmel's DFU bootloader). If SECURE_MODE is defined as false, all
	functions are usable on startup without the prerequisite firmware
	erase.
	
	NOTE: This device spoofs Atmel's DFU Bootloader USB VID and PID.
	      If you do not wish to use Atmel's ID codes, please manually
		  change them in Descriptors.c and alter your driver's INF
		  file accordingly.
*/

#define SECURE_MODE           false

#define INCLUDE_FROM_BOOTLOADER_C
#include "BootloaderDFU.h"

bool          IsSecure      = SECURE_MODE;
bool          RunBootloader = true;

uint8_t       DFU_State     = dfuIDLE;
uint8_t       DFU_Status    = OK;

DFU_Command_t SentCommand;
uint8_t       ResponseByte;

AppPtr_t      AppStartPtr   = 0x0000;

uint8_t       Flash64KBPage = 0;
uint16_t      StartAddr     = 0x0000;
uint16_t      EndAddr       = 0x0000;

int main (void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);
	
	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	/* Hardware initialization */
	LEDs_Init();
	
	/* Indicate USB not connected */
	LEDs_SetAllLEDs(LEDS_LED1);

	/* Initialize the USB subsystem */
	USB_Init();

	/* Run the USB management task while the bootloader is supposed to be running */
	while (RunBootloader)
	  USB_USBTask();
	
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
	AppStartPtr();
}

EVENT_HANDLER(USB_Connect)
{	
	/* Indicate USB connected */
	LEDs_SetAllLEDs(LEDS_LED2);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Upon disconnection, run user application */
	RunBootloader = false;
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	Endpoint_Ignore_Word();
	Endpoint_Ignore_Word();

	SentCommand.DataSize = Endpoint_Read_Word_LE();

	/* Indicate processing request */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3);

	switch (Request)
	{
		case DFU_DNLOAD:
			Endpoint_ClearSetupReceived();
			
			/* If the request has a data stage, load it into the command struct */
			if (SentCommand.DataSize)
			{
				while (!(Endpoint_Setup_Out_IsReceived()));

				/* First byte of the data stage is the DNLOAD request's command */
				SentCommand.Command = Endpoint_Read_Byte();
					
				/* One byte of the data stage is the command, so subtract it from the total data bytes */
				SentCommand.DataSize--;
					
				/* Load in the rest of the data stage as command parameters */
				for (uint16_t DataByte = 0; (DataByte < sizeof(SentCommand.Data)) &&
				     Endpoint_BytesInEndpoint(); DataByte++)
				{
					SentCommand.Data[DataByte] = Endpoint_Read_Byte();
					SentCommand.DataSize--;
				}
				
				/* Process the command */
				ProcessBootloaderCommand();
			}
			
			/* Check if currently downloading firmware */
			if (DFU_State == dfuDNLOAD_IDLE)
			{									
				if (!(SentCommand.DataSize))
				{
					DFU_State = dfuIDLE;
				}
				else
				{
					/* Subtract number of filler and suffix bytes from the total bytes remaining */
					SentCommand.DataSize -= (DFU_FILLER_BYTES_SIZE + DFU_FILE_SUFFIX_SIZE);
				
					/* Throw away the filler bytes before the start of the firmware */
					DiscardFillerBytes(DFU_FILLER_BYTES_SIZE);
					
					uint16_t TransfersRemaining = ((EndAddr - StartAddr) + 1);

					uint16_t BytesInFlashPage = 0;

					while (TransfersRemaining && SentCommand.DataSize)
					{
						/* Check if endpoint is empty - if so clear it and wait until ready for next packet */
						if (!(Endpoint_BytesInEndpoint()))
						{
							Endpoint_Setup_Out_Clear();
							while (!(Endpoint_Setup_Out_IsReceived()));
						}

						/* Default to one byte processed per write command */
						uint8_t BytesProcessed = 1;

						if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00))        // Write flash
						{
							union
							{
								uint16_t Words[2];
								uint32_t Long;
							} CurrFlashAddress = {Words: {StartAddr, Flash64KBPage}};
								
							/* Write the next word into the current flash page */
							boot_page_fill((CurrFlashAddress.Long + BytesInFlashPage), Endpoint_Read_Word_LE());

							/* Increment the counter for the number of bytes in the current page */
							BytesInFlashPage += 2;
							
							/* Two bytes have been processed for flash writes, not one */
							BytesProcessed    = 2;

							/* See if an entire page has been written to the flash page buffer */
							if (BytesInFlashPage < SPM_PAGESIZE)
							{
								/* Commit the flash page to memory */
								boot_page_write(CurrFlashAddress.Long);
								boot_spm_busy_wait();
								
								BytesInFlashPage = 0;
							}
						}
						else                                                   // Write EEPROM
						{
							/* Read the byte from the USB interface and write to to the EEPROM */
							eeprom_write_byte((uint8_t*)StartAddr, Endpoint_Read_Byte());
						}
					
						/* Adjust counters */
						SentCommand.DataSize -= BytesProcessed;
						TransfersRemaining   -= BytesProcessed;
						StartAddr            += BytesProcessed;
					}

					/* Re-enable the RWW section of flash in case it was written to */
					boot_rww_enable();

					/* Throw away the currently unused DFU file suffix */
					DiscardFillerBytes(DFU_FILE_SUFFIX_SIZE);
				}			
			}

			Endpoint_Setup_Out_Clear();

			/* Send ZLP to the host to acknowedge the request */
			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));
				
			break;
		case DFU_UPLOAD:
			Endpoint_ClearSetupReceived();

			while (!(Endpoint_Setup_In_IsReady()));

			if (DFU_State != dfuUPLOAD_IDLE)
			{
				/* Idle state upload - send response to last issued command */
				Endpoint_Write_Byte(ResponseByte);
			}
			else
			{
				/* Determine the number of transfers remaining in the current block */
				uint16_t TransfersRemaining = ((EndAddr - StartAddr) + 1);

				if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00) ||              // Read FLASH
				    IS_ONEBYTE_COMMAND(SentCommand.Data, 0x02))                // Read EEPROM
				{
					while (TransfersRemaining && SentCommand.DataSize)
					{
						/* Check if endpoint is full - if so clear it and wait until ready for next packet */
						if (Endpoint_BytesInEndpoint() == CONTROL_ENDPOINT_SIZE)
						{
							Endpoint_Setup_In_Clear();
							while (!(Endpoint_Setup_In_IsReady()));
						}

						/* Default to one byte processed per read command */
						uint8_t BytesProcessed = 1;

						if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00))        // Read FLASH
						{
							/* Read the flash word and send it via USB to the host */
							#if defined(RAMPZ)
								/* Create far flash psudo-pointer from address and 64KB flash page values */
								union
								{
									uint16_t Words[2];
									uint32_t Long;
								} CurrFlashAddress = {Words: {StartAddr, Flash64KBPage}};

								Endpoint_Write_Word_LE(pgm_read_word_far(CurrFlashAddress.Long));
							#else
								Endpoint_Write_Word_LE(pgm_read_word(StartAddr));							
							#endif
							
							/* Two bytes have been processed for flash writes, not one */
							BytesProcessed    = 2;				
						}
						else                                                   // Read EEPROM
						{
							/* Read the EEPROM byte and send it via USB to the host */
							Endpoint_Write_Byte(eeprom_read_byte((uint8_t*)StartAddr));				
						}
					
						/* Adjust counters */
						SentCommand.DataSize -= BytesProcessed;
						TransfersRemaining   -= BytesProcessed;
						StartAddr            += BytesProcessed;							
					}
				}
				else if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x01))           // Blank Check
				{
					/* Blank checking is performed in the DFU_DNLOAD request - if we get here we've told the host
					   that the memory isn't blank, and the host is requesting the first non-blank address */
					Endpoint_Write_Word_LE(StartAddr);
					
					/* Return to idle state */
					DFU_State = dfuIDLE;
				}
				
				/* Return to idle state if a short frame was sent (last packet of data) */
				if (!(TransfersRemaining))
				  DFU_State = dfuIDLE;
			}

			Endpoint_Setup_In_Clear();

			/* Send ZLP to the host to acknowedge the request */
			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();

			break;
		case DFU_GETSTATUS:
			Endpoint_ClearSetupReceived();
			
			/* Write 8-bit status value */
			Endpoint_Write_Byte(DFU_Status);
			
			/* Write 24-bit poll timeout value */
			Endpoint_Write_Byte(0);
			Endpoint_Write_Word_LE(0);
			
			/* Write 8-bit state value */
			Endpoint_Write_Byte(DFU_State);

			/* Write 8-bit state string ID number */
			Endpoint_Write_Byte(0);

			Endpoint_Setup_In_Clear();
			
			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();
	
			break;		
		case DFU_CLRSTATUS:
			Endpoint_ClearSetupReceived();
			
			/* Reset the status value variable to the default OK status */
			DFU_Status = OK;
			
			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));

			break;
		case DFU_GETSTATE:
			Endpoint_ClearSetupReceived();
			
			/* Write the current device state to the endpoint */
			Endpoint_Write_Byte(DFU_State);
		
			Endpoint_Setup_In_Clear();
			
			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();

			break;
		case DFU_ABORT:
			Endpoint_ClearSetupReceived();
			
			/* Reset the current state variable to the default idle state */
			DFU_State = dfuIDLE;
			
			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));

			break;
	}

	/* Indicate command processing finished */
	LEDs_SetAllLEDs(LEDS_LED2);
}

static void DiscardFillerBytes(uint8_t FillerBytes)
{
	while (FillerBytes--)
	{
		if (!(Endpoint_BytesInEndpoint()))
		{
			Endpoint_Setup_Out_Clear();

			/* Wait until next data packet received */
			while (!(Endpoint_Setup_Out_IsReceived()));
		}

		Endpoint_Ignore_Byte();						
	}
}

static void ProcessBootloaderCommand(void)
{
	/* Check if device is in secure mode */
	if (IsSecure)
	{
		/* Don't process command unless it is a READ or chip erase command */
		if (!(((SentCommand.Command == COMMAND_WRITE)             &&
		        IS_TWOBYTE_COMMAND(SentCommand.Data, 0x00, 0xFF)) ||
			   (SentCommand.Command == COMMAND_READ)))
		{
			/* Set the state and status variables to indicate the error */
			DFU_State  = dfuERROR;
			DFU_Status = errWRITE;
			
			/* Stall command */
			Endpoint_StallTransaction();
			
			/* Don't process the command */
			return;
		}
	}

	/* Dispatch the required command processing routine based on the command type */
	switch (SentCommand.Command)
	{
		case COMMAND_PROG_START:
			ProcessMemProgCommand();
			break;
		case COMMAND_DISP_DATA:
			ProcessMemReadCommand();
			break;
		case COMMAND_WRITE:
			ProcessWriteCommand();
			break;
		case COMMAND_READ:
			ProcessReadCommand();
			break;
		case COMMAND_CHANGE_BASE_ADDR:
			if (IS_TWOBYTE_COMMAND(SentCommand.Data, 0x03, 0x00))              // Set 64KB flash page command
			  Flash64KBPage = SentCommand.Data[2];

			break;
	}
}

static void LoadStartEndAddresses(void)
{
	union
	{
		uint8_t  Bytes[2];
		uint16_t Word;
	} Address[2] = {{Bytes: {SentCommand.Data[2], SentCommand.Data[1]}},
	                {Bytes: {SentCommand.Data[4], SentCommand.Data[3]}}};
		
	/* Load in the start and ending read addresses from the sent data packet */
	StartAddr = Address[0].Word;
	EndAddr   = Address[1].Word;
}

static void ProcessMemProgCommand(void)
{
	if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00) ||                          // Write FLASH command
		IS_ONEBYTE_COMMAND(SentCommand.Data, 0x01))                            // Write EEPROM command
	{
		/* Load in the start and ending read addresses */
		LoadStartEndAddresses();
		
		/* Set the state so that the next DNLOAD requests reads in the firmware */
		DFU_State = dfuDNLOAD_IDLE;
	}
}

static void ProcessMemReadCommand(void)
{
	if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00) ||                          // Read FLASH command
        IS_ONEBYTE_COMMAND(SentCommand.Data, 0x02))                            // Read EEPROM command
	{
		/* Load in the start and ending read addresses */
		LoadStartEndAddresses();

		/* Set the state so that the next UPLOAD requests read out the firmware */
		DFU_State = dfuUPLOAD_IDLE;
	}
	else if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x01))                       // Blank check FLASH command
	{
		uint32_t CurrFlashAddress = 0;

		while (CurrFlashAddress < BOOT_START_ADDR)
		{
			/* Check if the current byte is not blank */
			#if defined(RAMPZ)
			if (pgm_read_byte_far(CurrFlashAddress) != 0xFF)
			#else
			if (pgm_read_byte(CurrFlashAddress) != 0xFF)
			#endif
			{
				/* Set state and status variables to the appropriate error values */
				DFU_State  = dfuERROR;
				DFU_Status = errCHECK_ERASED;

				break;
			}

			CurrFlashAddress++;
		}
	}
}

static void ProcessWriteCommand(void)
{
	if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x03))                            // Start application
	{
		/* Check if empty request data array - an empty request after a filled request retains the
		   previous valid request data, but initializes the reset */
		if (!(SentCommand.DataSize))
		{
			if (SentCommand.Data[1] == 0x00)                                   // Start via watchdog
			{
				/* Start the watchdog, enter infinite loop to reset the AVR */
				wdt_enable(WDTO_250MS);
				for (;;);
			}
			else                                                                // Start via jump
			{
				/* Load in the jump address into the application start address pointer */
				union
				{
					uint8_t  Bytes[2];
					AppPtr_t FuncPtr;
				} Address = {Bytes: {SentCommand.Data[4], SentCommand.Data[3]}};

				AppStartPtr = Address.FuncPtr;
				
				/* Set the flag to terminate the bootloader at next opportunity */
				RunBootloader = false;			
			}
		}
	}
	else if (IS_TWOBYTE_COMMAND(SentCommand.Data, 0x00, 0xFF))                 // Erase flash
	{
		uint32_t CurrFlashAddress = 0;

		/* Clear the application section of flash */
		while (CurrFlashAddress < BOOT_START_ADDR)
		{
			boot_page_erase(CurrFlashAddress);
			boot_spm_busy_wait();
			boot_page_write(CurrFlashAddress);
			boot_spm_busy_wait();

			CurrFlashAddress += SPM_PAGESIZE;
		}

		/* Re-enable the RWW section of flash as writing to the flash locks it out */
		boot_rww_enable();
					
		/* Memory has been erased, reset the security bit so that programming/reading is allowed */
		IsSecure = false;
	}
}

static void ProcessReadCommand(void)
{
	const uint8_t BootloaderInfo[3] = {BOOTLOADER_VERSION, BOOTLOADER_ID_BYTE1, BOOTLOADER_ID_BYTE2};

	uint8_t CommandResponse = 0x00;

	if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x00))                            // Read bootloader info
	{
		CommandResponse = BootloaderInfo[SentCommand.Data[1]];
	}
	else if (IS_ONEBYTE_COMMAND(SentCommand.Data, 0x01))                       // Read signature byte
	{
		if (SentCommand.Data[1] == 0x30)                                       // Read byte 1
		  CommandResponse = boot_signature_byte_get(0);
		else if (SentCommand.Data[1] == 0x31)                                  // Read byte 2
		  CommandResponse = boot_signature_byte_get(2);
		else if (SentCommand.Data[1] == 0x60)                                  // Read byte 3
		  CommandResponse = boot_signature_byte_get(4);
	}
	
	ResponseByte = CommandResponse;
}
