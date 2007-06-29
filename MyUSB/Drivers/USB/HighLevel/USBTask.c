#include "../USB.h"

#include <util/delay.h> // TEMP
#include "../../USBKEY/Bicolour.h" // TEMP

TASK(USB_ManagementTask)
{
	if (USB_Get_VBUS_Status())
	{
		Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
		
		for (uint8_t i=0; i<100; i++)
		  _delay_ms(10);
	}
}
