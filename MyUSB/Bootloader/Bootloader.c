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
	
	*** WORK IN PROGRESS - NOT CURRENTLY FUNCTIONING ***
*/

#define INCLUDE_FROM_BOOTLOADER_C
#include "Bootloader.h"

bool      RunBootloader = true;
uint8_t   DFU_State     = appIDLE;
uint8_t   DFU_Status    = OK;
uint8_t   CommandData[ENDPOINT_CONTROLEP_SIZE];
uint8_t   DataSize;
FuncPtr_t AppStartPtr   = 0x0000;

int main (void)
{
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	Bicolour_Init();
	
	Bicolour_SetLeds(BICOLOUR_LED1_RED);

	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	while (RunBootloader)
	  USB_USBTask();
	
	USB_ShutDown();
	
	MCUCR = (1 << IVCE);
	MCUCR = 0;
	
	boot_rww_enable();
	AppStartPtr();
}

EVENT_HANDLER(USB_Connect)
{
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	RunBootloader = false;
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	Endpoint_Ignore_Word();
	Endpoint_Ignore_Word();

	uint16_t TransactionSize = Endpoint_Read_Word_LE();
	uint8_t* CommandDataPtr  = (uint8_t*)&CommandData;

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
			
			while (!(Endpoint_Setup_Out_IsReceived()));
		
			for (uint16_t DataByte = 0; DataByte < TransactionSize; DataByte++)
			  *(CommandDataPtr++) = Endpoint_Read_Byte();
			
			DataSize = (TransactionSize - sizeof(uint8_t));
			
			Endpoint_Setup_Out_Clear();

			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));
			
			ProcessBootloaderCommand();
				
			break;
		case DFU_UPLOAD:
			if (DFU_State == dfuERROR)
				return;

			Endpoint_ClearSetupReceived();

			for (uint16_t DataByte = 0; DataByte < TransactionSize; DataByte++)
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

	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

static void ProcessBootloaderCommand(void)
{
	switch (CommandData[0])
	{
		case COMMAND_WRITE:
			if (CommandData[1] == 0x03)           // Start application command
			{
				if (CommandData[2] == 0x00)       // Hardware reset
				{
					wdt_enable(WDTO_15MS);
					for (;;);
				}
				else                              // Jump to address
				{
					uint16_t JumpAddress = ((CommandData[3] << 8) | CommandData[4]);

					AppStartPtr = (FuncPtr_t)JumpAddress;
					RunBootloader = false;
				}
			}
			else if (CommandData[1] == 0x00)
			{
				if (CommandData[2] == 0xFF)       // Erase flash
				  EraseFlash();
			}
			
			break;
		case COMMAND_READ:
			if (CommandData[1] == 0x00)           // Read bootloader info command
			{
				DataSize = 1;

				if (CommandData[2] == 0x00)       // Bootloader version
				  CommandData[0] = BOOTLOADER_VERSION;
				else                              // Boot IDs
				  CommandData[0] = 0x00;
			}
			else if (CommandData[1] == 0x01)      // Read chip info command
			{
				DataSize = 1;
				
				uint8_t SigVal = 0;
				
				if (CommandData[2] == 0x30)       // Sig Byte 1
				  SigVal = ReadSigByte(0);
				else if (CommandData[2] == 0x31)  // Sig Byte 2
				  SigVal = ReadSigByte(2);
				else if (CommandData[2] == 0x60)  // Sig Byte 3
				  SigVal = ReadSigByte(4);

				CommandData[0] = SigVal;
			}
			
			break;
	}
}

static void EraseFlash(void)
{
	uint32_t FlashAddr = 0;

	while (FlashAddr < BOOT_START_ADDR)
	{
		boot_page_erase(FlashAddr);
		boot_spm_busy_wait();
		boot_page_write(FlashAddr);
		boot_spm_busy_wait();

		FlashAddr += SPM_PAGESIZE;
	}
}
