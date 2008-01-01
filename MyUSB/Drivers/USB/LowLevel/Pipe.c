/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#if ((defined(__AVR_AT90USB1286__) || (defined(__AVR_AT90USB646__))) && !(defined(USB_DEVICE_ONLY)))
	#define USB_DEVICE_ONLY
#endif

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
#include "Pipe.h"

uint8_t USB_ControlPipeSize = PIPE_CONTROLPIPE_DEFAULT_SIZE;

bool Pipe_ConfigurePipe_P(const uint8_t PipeNum,
                          const uint8_t UPCFG0Xdata,
                          const uint8_t UPCFG1Xdata)
{
	Pipe_SelectPipe(PipeNum & PIPE_PIPENUM_MASK);
	Pipe_EnablePipe();
	
	UPCFG0X = UPCFG0Xdata;
	UPCFG1X = ((UPCFG1X & (1 << ALLOC)) | UPCFG1Xdata);
	UPCFG2X = 0;
	
	Pipe_AllocateMemory();
	
	return Pipe_IsConfigured();
}

void Pipe_ClearPipes(void)
{
	for (uint8_t PNum = 0; PNum < PIPE_MAXPIPES; PNum++)
	{
		Pipe_ResetPipe(PNum);
		Pipe_SelectPipe(PNum);
		UPIENX = 0;
		Pipe_ClearError();
		Pipe_ClearErrorFlags();
		Pipe_DeallocateMemory();
		Pipe_DisablePipe();
	}
}
#endif
