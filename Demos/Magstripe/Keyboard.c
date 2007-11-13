/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Demonstration application for a TTL magnetic stripe reader (such as the
	Omron V3B-4K) by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

/*
	This demo uses a keyboard HID driver to communicate the data collected
	a TTL magnetic stripe reader to the connected computer.  The raw
	bitstream obtained from the magnetic stripe reader is "typed" through
	the keyboard driver as 0's and 1's.  After every card swipe, the demo
	will send a Return key.

	This demo relies on the keyboard demo to compile.  The keyboard demo
	must be located at ../Keyboard.
*/

#include "Keyboard.h"

#define MAG_DATA_PIN	PORTC0
#define MAG_CLOCK_PIN	PORTC3
#define MAG_CLS_PIN	PORTC4
#define MAG_PIN		PINC
#define MAG_DDR		DDRC
#define MAG_PORT	PORTC
#include <MyUSB/Drivers/USBKEY/Magstripe.h>  // magstripe driver for the USBKEY

#include <util/delay.h> // _delay_ms()

#define DATA_LEN 1024
uint8_t stripe_data[DATA_LEN];
uint16_t stripe_idx = 0;

TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Keyboard_Report_ID,
};

TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Keyboard_Report_ID  , TaskName: USB_Keyboard_Report  , TaskStatus: TASK_RUN  },
};

void Keyboard_SendReport(USB_KeyboardReport_Data_t KeyboardReportData);

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Magstripe_Init();
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_HIGHSPEED);

	/* Scheduling */
	Scheduler_Start(); // Scheduler never returns, so put this last
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

TASK(USB_Keyboard_Report)
{
	USB_KeyboardReport_Data_t KeyboardReportData = {Modifier: 0, KeyCode: 0};
	uint8_t                   MagStatus_LCL      = Magstripe_GetStatus();
	uint32_t                  num_entries        = 0;

	if ( (MagStatus_LCL & MAG_CLS) != MAG_CLS ) {
		return;
	}

	// while card present, process card
	while ( (MagStatus_LCL & MAG_CLS) == MAG_CLS ) {
		do {
			MagStatus_LCL = Magstripe_GetStatus();
			if ((MagStatus_LCL & MAG_CLS) != MAG_CLS) {
				goto loop_end;
			}
		} while ( (MagStatus_LCL & MAG_CLOCK) != 0);

		if ( (MagStatus_LCL & MAG_DATA) == 0)
		  stripe_data[stripe_idx] = 39; // 0
		else
		  stripe_data[stripe_idx] = 30; // 1

		stripe_idx++;
		if (stripe_idx >= DATA_LEN) {
			stripe_idx = 0;
		}
		num_entries++;

		do {
			MagStatus_LCL = Magstripe_GetStatus();
			if ((MagStatus_LCL & MAG_CLS) != MAG_CLS) {
				goto loop_end;
			}
		} while ( (MagStatus_LCL & MAG_CLOCK) != MAG_CLOCK);
	}
loop_end:

	for (int i = 0; i < stripe_idx; i++) {
		KeyboardReportData.KeyCode = stripe_data[i];
		Keyboard_SendReport(KeyboardReportData);
		_delay_ms(5);
		KeyboardReportData.KeyCode = 0;
		Keyboard_SendReport(KeyboardReportData);
		_delay_ms(5);
	}
	KeyboardReportData.KeyCode = 40; // Enter
	Keyboard_SendReport(KeyboardReportData);
	_delay_ms(5);
	KeyboardReportData.KeyCode = 0;
	Keyboard_SendReport(KeyboardReportData);

	stripe_idx = 0;
}

void Keyboard_SendReport(USB_KeyboardReport_Data_t KeyboardReportData)
{
	if (USB_IsConnected && USB_IsInitialized)
	{
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		if (Endpoint_ReadWriteAllowed())
		{
			USB_Device_Write_Byte(KeyboardReportData.Modifier);
			USB_Device_Write_Byte(KeyboardReportData.KeyCode);
			
			Endpoint_In_Clear();
		}
	}
}
