#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to build the MyUSB Demos. Call with "make all" to rebuild
# all demos.

# Projects are pre-cleaned before each one is built, to ensure any
# custom MyUSB library build options are reflected in the compiled
# code.

CLEANLIB = make -C ../MyUSB/ clean

all:
	$(CLEANLIB)
	make -C TestApp all
	$(CLEANLIB)
	make -C Mouse all
	$(CLEANLIB)
	make -C Keyboard all
	
clean:
	make -C TestApp clean
	make -C Mouse clean
	make -C Keyboard clean