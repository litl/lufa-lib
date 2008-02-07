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

#include "Bootloader.h"

bool    RunBootloader = true;
uint8_t DFU_State     = appIDLE;
uint8_t DFU_Status    = OK;

int main (void)
{
	/* Disable clock division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	Bicolour_Init();
	SerialStream_Init(9600);
	
	Bicolour_SetLeds(BICOLOUR_LED1_RED);

	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	while (RunBootloader)
	  USB_USBTask();
	
	USB_ShutDown();
	
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);
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
	uint16_t BlockNum = Endpoint_Read_Word_LE();
	Endpoint_Ignore_Word();

	printf("Sent: %d\r\n", Request);

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
			Endpoint_ClearSetupReceived();
			
			uint8_t  BlockData[ENDPOINT_CONTROLEP_SIZE];
			uint16_t TransactionSize = Endpoint_Read_Word_LE();

			while (!(Endpoint_Setup_Out_IsReceived()));
		
			for (uint16_t DataByte = 0; DataByte < TransactionSize; DataByte++)
			  BlockData[DataByte] = Endpoint_Read_Word();
			
			Endpoint_Setup_Out_Clear();

			Endpoint_Setup_In_Clear();
			while (!(Endpoint_Setup_In_IsReady()));
				
			break;
		default:
			printf("Unused: %d\r\n", Request);
			Bicolour_SetLeds(BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE);
	}
	
	printf("Done: %d\r\n", Request);
}
