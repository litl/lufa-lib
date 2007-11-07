#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to build and run various helper applications on the library.

MYUSBFILES = ./Scheduler/Scheduler.c                     \
             ./Drivers/USBKEY/Temperature.c              \
             ./Drivers/USB1287/Serial_Stream.c           \
             ./Drivers/USB/LowLevel/LowLevel.c           \
             ./Drivers/USB/LowLevel/Endpoint.c           \
             ./Drivers/USB/LowLevel/Pipe.c               \
             ./Drivers/USB/LowLevel/DevChapter9.c        \
             ./Drivers/USB/LowLevel/HostChapter9.c       \
             ./Drivers/USB/LowLevel/Host.c               \
             ./Drivers/USB/HighLevel/USBTask.c           \
             ./Drivers/USB/HighLevel/USBInterrupt.c      \
             ./Drivers/USB/HighLevel/Events.c            \
             ./Drivers/USB/HighLevel/StdDescriptors.c    \

all:
	make -C HelperApps $@

	@echo
	@echo Dumping library method signatures...
	java -classpath HelperApps/MethodSig MethodSig $(MYUSBFILES) > FunctionSignatures.txt

cleanlib:
	rm -f $(MYUSBFILES:%.c=%.o)

clean:
	make -C HelperApps $@

	rm -f $(MYUSBFILES:%.c=%.o)

	rm -f FunctionSignatures.txt