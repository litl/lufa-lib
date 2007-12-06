#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to build all the MyUSB Demos. Call with "make all" to
# rebuild all demos.

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

	make -C Joystick clean
	make -C Joystick all

	make -C MouseHost clean
	make -C MouseHost all

	make -C MouseHostViaInt clean
	make -C MouseHostViaInt all

	make -C KeyboardHost clean
	make -C KeyboardHost all
	
	make -C KeyboardHostViaInt clean
	make -C KeyboardHostViaInt all

	make -C CDC clean
	make -C CDC all
	
	make -C USBtoSerial clean
	make -C USBtoSerial all
	
	make -C MassStorage clean
	make -C MassStorage all

clean:
	make -C TestApp clean
	make -C Mouse clean
	make -C Keyboard clean
	make -C Magstripe clean
	make -C Joystick clean
	make -C MouseHost clean
	make -C KeyboardHost clean
	make -C CDC clean
	make -C USBtoSerial clean
	make -C MassStorage clean
	
clean_list:
	make -C TestApp clean_list
	make -C Mouse clean_list
	make -C Keyboard clean_list
	make -C Magstripe clean_list
	make -C Joystick clean_list
	make -C MouseHost clean_list
	make -C MouseHostViaInt clean
	make -C KeyboardHost clean_list
	make -C KeyboardHostViaInt clean_list
	make -C CDC clean_list
	make -C USBtoSerial clean_list
	make -C MassStorage clean_list
