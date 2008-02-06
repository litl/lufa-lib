#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile for the MyUSB library itself.

MYUSB_USB_FILES =    ./Drivers/USB/LowLevel/LowLevel.c           \
                     ./Drivers/USB/LowLevel/Endpoint.c           \
                     ./Drivers/USB/LowLevel/Pipe.c               \
                     ./Drivers/USB/LowLevel/DevChapter9.c        \
                     ./Drivers/USB/LowLevel/HostChapter9.c       \
                     ./Drivers/USB/LowLevel/Host.c               \
                     ./Drivers/USB/HighLevel/USBTask.c           \
                     ./Drivers/USB/HighLevel/USBInterrupt.c      \
                     ./Drivers/USB/HighLevel/Events.c            \
                     ./Drivers/USB/HighLevel/StdDescriptors.c    \					 
				  
MYUSB_DRIVER_FILES = ./Scheduler/Scheduler.c                     \
                     ./MemoryAllocator/DynAlloc.c                \
                     ./Drivers/USBKEY/Dataflash.c                \
                     ./Drivers/USBKEY/Temperature.c              \
                     ./Drivers/AT90USB_64x_128x/Serial.c         \
                     ./Drivers/AT90USB_64x_128x/Serial_Stream.c  \

all:
	make -C Bootloader/ clean
	make -C Bootloader/ all
	
clean:
	make -C Bootloader/ clean

	rm -f $(MYUSB_USB_FILES:%.c=%.o)
	rm -f $(MYUSB_DRIVER_FILES:%.c=%.o)
	
clean_list:
