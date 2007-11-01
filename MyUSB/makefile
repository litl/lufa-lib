#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to run various helper applications on the library.
# NB: This makefile does not compile the library itself, or
# the Helper Applications. This makefile is designed to generate
# library information from the already compiled tools only.

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
	@echo Dumping library method signatures...
	java -classpath HelperApps/MethodSig MethodSig $(MYUSBFILES) > FunctionSignatures.txt
	
clean:
	rm -f FunctionSignatures.txt