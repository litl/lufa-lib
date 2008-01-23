/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Communications Device Class demonstration application.
	This gives a simple reference application for implementing
	a CDC device acting as a virtual serial port. Joystick
	actions on the USBKEY board are transmitted to the host
	as strings. The device does not respond to serial data
	sent from the host.
	
	Before running, you will need to install the INF file that
	is located in the CDC project directory. This will enable
	Windows to use its inbuilt CDC drivers, negating the need
	for special Windows drivers for the device. To install,
	right-click the .INF file and choose the Install option.
*/

#include "CDC.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB CDC App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ Task: CDC_Task             , TaskStatus: TASK_RUN  },
};

/* Globals: */
CDC_Line_Coding_t LineCoding = { BaudRateBPS: 9600,
                                 CharFormat:  OneStopBit,
                                 ParityType:  Parity_None,
                                 DataBits:    8            };

char JoystickUpString[]      PROGMEM = "Joystick Up\r\n";
char JoystickDownString[]    PROGMEM = "Joystick Down\r\n";
char JoystickLeftString[]    PROGMEM = "Joystick Left\r\n";
char JoystickRightString[]   PROGMEM = "Joystick Right\r\n";
char JoystickPressedString[] PROGMEM = "Joystick Pressed\r\n";

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
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

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	uint8_t* LineCodingData = (uint8_t*)&LineCoding;

	Endpoint_Ignore_Word();

	/* Process CDC specific control requests */
	switch (Request)
	{
		case GET_LINE_CODING:
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
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
	char*       ReportString    = NULL;
	uint8_t     JoyStatus_LCL   = Joystick_GetStatus();
	static bool ActionSent      = false;

	/* Determine if a joystick action has occured */
	if (JoyStatus_LCL & JOY_UP)
	  ReportString = JoystickUpString;
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportString = JoystickDownString;
	
	if (JoyStatus_LCL & JOY_LEFT)
	  ReportString = JoystickLeftString;
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportString = JoystickRightString;
	
	if (JoyStatus_LCL & JOY_PRESS)
	  ReportString = JoystickPressedString;

	/* Flag management - Only allow one string to be sent per action */
	if (ReportString == NULL)
	{
		ActionSent = false;
	}
	else if (ActionSent == false)
	{
		ActionSent = true;
		SendStringViaCDC(ReportString);
	}

	/* Select the Serial Rx Endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	/* Throw away any recieved data from the host */
	if (Endpoint_ReadWriteAllowed())
	  Endpoint_FIFOCON_Clear();
}

void SendStringViaCDC(char* FlashString)
{
	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		char StringByte;

		/* Select the Serial Tx Endpoint */
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);

		/* Wait until Serial Tx Endpoint Ready for Read/Write */
		while (!(Endpoint_ReadWriteAllowed()));
		
		/* Write the String to the Endpoint */
		while ((StringByte = pgm_read_byte(FlashString++)) != 0x00)
		  Endpoint_Write_Byte(StringByte);
	  
		/* Send the data */
		Endpoint_FIFOCON_Clear();
	}
}
