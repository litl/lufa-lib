/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	MyUSB USB Bootloader. This bootloader enumerates to the host
	as a DFU Class device, allowing for DFU-compatible programming
	software to load firmware onto the AVR.
	
	This bootloader is compatible with Atmel's FLIP application.
	However, it requires the use of Atmel's DFU drivers. You will
	need to install Atmel's FDU drivers prior to using this bootloader.
	
	As an open-source option, this bootloader is also compatible
	with the Linux Atmel USB DFU Programmer software, avaliable
	for download at http://sourceforge.net/projects/dfu-programmer/.
	
    If SECURE_MODE is defined, upon startup the bootloader will be
    locked, with only the chip erase function avaliable (similar to
    Atmel's DFU bootloader). If SECURE_MODE is undefined, all functions
    are usable on startup without the prerequisite firmware erase.
    
	*** WORK IN PROGRESS - NOT CURRENTLY FUNCTIONING ***
*/

//#define SECURE_MODE

#define INCLUDE_FROM_BOOTLOADER_C
#include "Bootloader.h"

#if defined(SECURE_MODE)
bool          IsSecure      = true;
#endif

bool          RunBootloader = true;
uint8_t       DFU_State     = appIDLE;
uint8_t       DFU_Status    = OK;

DFU_Command_t SentCommand;
uint8_t       ResponseByte;
uint8_t       Operation     = OP_SINGLE_BYTE_RESPONSE;

FuncPtr_t     AppStartPtr   = 0x0000;

uint8_t       Flash64KBPage = 0;
uint16_t      StartAddr     = 0x0000;
uint16_t      EndAddr       = 0x0000;

int main (void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	/* Hardware initialization */
	Bicolour_Init();
	
	/* Staus LED set to red by default */
	Bicolour_SetLeds(BICOLOUR_LED1_RED);

	/* Initialize the USB subsystem */
	USB_Init();

	/* Run the USB management task while the bootload is supposed to be running */
	while (RunBootloader)
	  USB_USBTask();
	
	/* Shut down the USB subsystem */
	USB_ShutDown();
	
	/* Relocate the interrupt vector table back to the application section */
	MCUCR = (1 << IVCE);
	MCUCR = 0;

	/* Reset any used hardware ports back to their defaults */
	PORTD = 0;
	DDRD = 0;
	
	/* Start the user application */
	boot_rww_enable();
	AppStartPtr();
}

EVENT_HANDLER(USB_Connect)
{
	/* Status LED green when USB connected */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Upon disconnection, run user application */
	RunBootloader = false;
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	uint8_t* CommandDataPtr = (uint8_t*)&SentCommand.Data;

	Endpoint_Ignore_Word();
	Endpoint_Ignore_Word();

	SentCommand.DataSize = Endpoint_Read_Word_LE();

	/* Processing request - status LED orange */
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);

	switch (Request)
	{
		case DFU_DNLOAD:
			Endpoint_ClearSetupReceived();
			
			if (Operation == OP_SINGLE_BYTE_RESPONSE)
			{
				if (SentCommand.DataSize)
				{
					while (!(Endpoint_Setup_Out_IsReceived()));

					SentCommand.Command = Endpoint_Read_Byte();
					SentCommand.DataSize--;
					
					for (uint16_t DataByte = 0; DataByte < SentCommand.DataSize; DataByte++)
					  *(CommandDataPtr++) = Endpoint_Read_Byte();
					
					Endpoint_Setup_Out_Clear();
				}
			}
			else
			{
				// Prog FE here
			}

			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));
			
			Operation = OP_SINGLE_BYTE_RESPONSE;			

			ProcessBootloaderCommand();
				
			break;
		case DFU_UPLOAD:
			Endpoint_ClearSetupReceived();

			if (Operation == OP_SINGLE_BYTE_RESPONSE)
			{
				Endpoint_Write_Byte(ResponseByte);
			}
			else
			{
				if (Operation == OP_READ_EEPROM)
				{
					uint8_t TBytesRem = ENDPOINT_CONTROLEP_SIZE;
					
					while (StartAddr <= EndAddr)
					{
						Endpoint_Write_Byte(eeprom_read_byte((uint8_t*)StartAddr));
						TBytesRem--;
						StartAddr++;
						
						if (!(TBytesRem))
						{
							Endpoint_Setup_In_Clear();
							while (!(Endpoint_Setup_In_IsReady()));
							
							TBytesRem = ENDPOINT_CONTROLEP_SIZE;
						}
					}
				}
				else if (Operation == OP_READ_FLASH)
				{
					uint8_t TBytesRem = ENDPOINT_CONTROLEP_SIZE;
					
					while (StartAddr <= EndAddr)
					{
						uint32_t CurrAddress = (((uint32_t)Flash64KBPage << 16) | StartAddr);

						Endpoint_Write_Byte(pgm_read_byte_far(CurrAddress));
						TBytesRem--;
						StartAddr++;
						
						if (!(TBytesRem))
						{
							Endpoint_Setup_In_Clear();
							while (!(Endpoint_Setup_In_IsReady()));
							
							TBytesRem = ENDPOINT_CONTROLEP_SIZE;
						}
					}				
				}
				else if (Operation == OP_BLANK_CHECK)
				{
					Endpoint_Write_Word_LE(StartAddr);
				}
			}

			Endpoint_Setup_In_Clear();

			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();			
		
			break;
		case DFU_GETSTATUS:
			Endpoint_ClearSetupReceived();
			
			Endpoint_Write_Byte(DFU_Status);
			
			Endpoint_Write_Byte(0);
			Endpoint_Write_Word_LE(1);
			
			Endpoint_Write_Byte(DFU_State);

			Endpoint_Write_Byte(0);

			Endpoint_Setup_In_Clear();
			
			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();
	
			break;		
		case DFU_CLRSTATUS:
			Endpoint_ClearSetupReceived();
			
			DFU_Status = OK;
			
			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));

			break;
		case DFU_GETSTATE:
			Endpoint_ClearSetupReceived();
			
			Endpoint_Write_Byte(DFU_State);
		
			Endpoint_Setup_In_Clear();
			
			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();

			break;
		case DFU_ABORT:
			Endpoint_ClearSetupReceived();
			
			DFU_State = appIDLE;
			
			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));

			break;
	}

	/* End of request processing - status LED green */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

static void ProcessBootloaderCommand(void)
{
	#if defined(SECURE_MODE)
	/* Check if device is in secure mode, but erase command is not being issued */
	if (IsSecure && ((SentCommand.Command != COMMAND_WRITE) ||
	                 (SentCommand.Data[0] != 0x00)          ||
		             (SentCommand.Data[1] != 0xFF)))
	{
		/* Set the state and status variables to indicate the error */
		DFU_State  = dfuERROR;
		DFU_Status = errCHECK_ERASED;
		
		/* Don't process the command */
		return;
	}
	#endif

	/* Dispatch the required command processing routine based on the command type */
	switch (SentCommand.Command)
	{
		case COMMAND_WRITE:
			ProcessWriteCommand();
			break;
		case COMMAND_READ:
			ProcessReadCommand();
			break;
		case COMMAND_PROG_START:
			ProcessMemProgCommand();
			break;
		case COMMAND_DISP_DATA:
			ProcessMemReadCommand();
			break;
		case COMMAND_CHANGE_BASE_ADDR:
			if (SentCommand.Data[0] == 0x03)
			{
				if (SentCommand.Data[1] == 0x00)   // Set 64KB flash page command
				  Flash64KBPage = SentCommand.Data[2];
			}

			break;
	}
}

static void ProcessMemProgCommand(void)
{
	StartAddr = (((uint16_t)SentCommand.Data[1] << 8) | SentCommand.Data[2]);
	EndAddr   = (((uint16_t)SentCommand.Data[3] << 8) | SentCommand.Data[4]);

	if (SentCommand.Data[0] == 0x00)               // Write FLASH command
	  Operation = OP_WRITE_FLASH;
	else if (SentCommand.Data[0] == 0x01)          // Write EEPROM command
	  Operation = OP_WRITE_EEPROM;
}

static void ProcessMemReadCommand(void)
{
	StartAddr = (((uint16_t)SentCommand.Data[1] << 8) | SentCommand.Data[2]);
	EndAddr   = (((uint16_t)SentCommand.Data[3] << 8) | SentCommand.Data[4]);
	
	if (SentCommand.Data[0] == 0x00)               // Read FLASH command
	{
		Operation = OP_READ_FLASH;
	}
	else if (SentCommand.Data[0] == 0x01)          // Blank check FLASH command
	{
		Operation = OP_BLANK_CHECK;

		do
		{
			uint32_t CurrAddress = (((uint32_t)Flash64KBPage << 16) | StartAddr);
			
			if (pgm_read_byte_far(CurrAddress) != 0xFF)
			{
				DFU_State  = dfuERROR;
				DFU_Status = errCHECK_ERASED;
				break;
			}
			
			StartAddr++;
		}
		while (StartAddr != EndAddr);
	}
	else if (SentCommand.Data[0] == 0x02)          // Read EEPROM command
	{
		Operation = OP_READ_EEPROM;
	}
}

static void ProcessWriteCommand(void)
{
	switch (SentCommand.Data[0])
	{
		case 0x03:                                 // Start application command
			if (SentCommand.Data[1] == 0x00)       // Hardware reset
			{
				if (!(SentCommand.DataSize))       // Empty data request starts the app
				{
					wdt_enable(WDTO_250MS);
					for (;;);
				}
			}
			else                                   // Jump to address
			{
				uint16_t JumpAddress = ((SentCommand.Data[2] << 8) | SentCommand.Data[3]);

				if (!(SentCommand.DataSize))       // Empty data request starts the app
				  RunBootloader = false;
				else
				  AppStartPtr = (FuncPtr_t)JumpAddress;
			}
			
			break;
		case 0x00:
			if (SentCommand.Data[1] == 0xFF)       // Erase flash
			{
				EraseFlash();
						
				#if defined(SECURE_MODE)
				IsSecure = false;
				#endif
			}
			
			break;
	}
}

static void ProcessReadCommand(void)
{
	switch (SentCommand.Data[0])
	{
		case 0x00:                                 // Read bootloader info command
			if (SentCommand.Data[1] == 0x00)       // Bootloader version
			  ResponseByte = BOOTLOADER_VERSION;
			else                                   // Boot IDs
			  ResponseByte = 0x00;
		
			break;
		case 0x01:                                 // Read chip info command
			if (SentCommand.Data[1] == 0x30)       // Sig Byte 1
			  ResponseByte = boot_read_sig_byte(0);
			else if (SentCommand.Data[1] == 0x31)  // Sig Byte 2
			  ResponseByte = boot_read_sig_byte(2);
			else if (SentCommand.Data[1] == 0x60)  // Sig Byte 3
			  ResponseByte = boot_read_sig_byte(4);
			else
			  ResponseByte = 0x00;
			
			boot_spm_busy_wait();
			
			break;
	}
}

static void EraseFlash(void)
{
	uint32_t FlashAddr = 0;

	/* Clear the application section of flash */
	while (FlashAddr < BOOT_START_ADDR)
	{
		boot_page_erase(FlashAddr);
		boot_spm_busy_wait();
		boot_page_write(FlashAddr);
		boot_spm_busy_wait();

		FlashAddr += SPM_PAGESIZE;
	}
}
