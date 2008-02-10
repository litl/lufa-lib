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

#define INCLUDE_FROM_BOOTLOADER_C
#include "Bootloader.h"

#if defined(SECURE_MODE)
bool      IsSecure      = true;
#endif

bool      RunBootloader = true;
uint8_t   DFU_State     = appIDLE;
uint8_t   DFU_Status    = OK;
uint8_t   CommandData[ENDPOINT_CONTROLEP_SIZE];
uint8_t   DataSize;
FuncPtr_t AppStartPtr   = 0x0000;

int main (void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
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
	uint8_t* CommandDataPtr  = (uint8_t*)&CommandData;

	Endpoint_Ignore_Word();
	Endpoint_Ignore_Word();

	DataSize = Endpoint_Read_Word_LE();

	/* Processing request - status LED orange */
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);

	switch (Request)
	{
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
		case DFU_DNLOAD:
			if (DFU_State == dfuERROR)
				return;
				
			Endpoint_ClearSetupReceived();
			
			if (DataSize)
			{
				while (!(Endpoint_Setup_Out_IsReceived()));
			
				for (uint16_t DataByte = 0; DataByte < DataSize; DataByte++)
				  *(CommandDataPtr++) = Endpoint_Read_Byte();
				
				Endpoint_Setup_Out_Clear();
			}

			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));
			
			ProcessBootloaderCommand();
				
			break;
		case DFU_UPLOAD:
			if (DFU_State == dfuERROR)
				return;

			Endpoint_ClearSetupReceived();

			for (uint16_t DataByte = 0; DataByte < DataSize; DataByte++)
			{
				Endpoint_Write_Byte(*(CommandDataPtr++));

				if (DataByte == DataSize)
				  break;
			}

			Endpoint_Setup_In_Clear();

			while (!(Endpoint_Setup_Out_IsReceived()));
			Endpoint_Setup_Out_Clear();			
		
			break;
	}

	/* End of request processing - status LED green */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

static void ProcessBootloaderCommand(void)
{
	#if defined(SECURE_MODE)
	/* Check if device is in secure mode, but erase command is not being issued */
	if (IsSecure && ((CommandData[0] != COMMAND_WRITE) ||
	                 (CommandData[1] != 0x00)          ||
		             (CommandData[2] != 0xFF)))
	{
		/* Set the state and status variables to indicate the error */
		DFU_State  = dfuERROR;
		DFU_Status = errCHECK_ERASED;
		
		/* Don't process the command */
		return;
	}
	#endif

	/* Dispatch the required command processing routine based on the command type */
	if (CommandData[0] == COMMAND_WRITE)
	  ProcessWriteCommand();
	else if (CommandData[0] == COMMAND_READ)
	  ProcessReadCommand();
}

static void ProcessWriteCommand(void)
{
	switch (CommandData[1])
	{
		case 0x03:                            // Start application command
			if (CommandData[2] == 0x00)       // Hardware reset
			{
				if (!(DataSize))              // Empty data request starts the app
				{
					wdt_enable(WDTO_15MS);
					for (;;);
				}
			}
			else                              // Jump to address
			{
				uint16_t JumpAddress = ((CommandData[3] << 8) | CommandData[4]);

				if (!(DataSize))              // Empty data request starts the app
				  RunBootloader = false;
				else
				  AppStartPtr = (FuncPtr_t)JumpAddress;
			}
			
			break;
		case 0x00:
			if (CommandData[2] == 0xFF)       // Erase flash
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
	DataSize = 1;

	switch (CommandData[1])
	{
		case 0x00:                            // Read bootloader info command
			if (CommandData[2] == 0x00)       // Bootloader version
			  CommandData[0] = BOOTLOADER_VERSION;
			else                              // Boot IDs
			  CommandData[0] = 0x00;
		
			break;
		case 0x01:                            // Read chip info command
			if (CommandData[2] == 0x30)       // Sig Byte 1
			  CommandData[0] = ReadSigByte(0);
			else if (CommandData[2] == 0x31)  // Sig Byte 2
			  CommandData[0] = ReadSigByte(2);
			else if (CommandData[2] == 0x60)  // Sig Byte 3
			  CommandData[0] = ReadSigByte(4);
			
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
