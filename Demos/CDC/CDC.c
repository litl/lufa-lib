/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Communications Device Class demonstration application.
	This gives a simple reference application for implementing
	a CDC device acting as a virtual serial port. Joystick
	actions on the USBKEY board are transmitted to the host
	as strings.
	
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

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	CDC_Task_ID
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: CDC_Task_ID             , TaskName: CDC_Task             , TaskStatus: TASK_RUN  },
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
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, ENDPOINT_TYPE_BULK,
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
			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  Endpoint_Write_Byte(*(LineCodingData++));	
			
			Endpoint_ClearSetupRecieved();

			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
			
			while (!(Endpoint_Out_IsRecieved()));
			Endpoint_Out_Clear();

			break;
		case SET_LINE_CODING:
			Endpoint_ClearSetupRecieved();
			while (!(Endpoint_Out_IsRecieved()));

			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  *(LineCodingData++) = Endpoint_Read_Byte();

			Endpoint_Out_Clear();

			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
	
			break;
		case SET_CONTROL_LINE_STATE:
			Endpoint_ClearSetupRecieved();
			
			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
	
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
		Endpoint_In_Clear();
	}
}
