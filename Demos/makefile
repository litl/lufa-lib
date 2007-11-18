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

all:
	make -C TestApp clean
	make -C TestApp all

	make -C Mouse clean
	make -C Mouse all

	make -C Keyboard clean
	make -C Keyboard all

	make -C Magstripe clean
	make -C Magstripe all

	make -C MouseHost clean
	make -C MouseHost all

	make -C KeyboardHost clean
	make -C KeyboardHost all
	
clean:
	make -C TestApp clean
	make -C Mouse clean
	make -C Keyboard clean
	make -C Magstripe clean
	make -C MouseHost clean
	make -C KeyboardHost clean
