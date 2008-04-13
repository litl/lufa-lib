#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the LGPL Licence, Version 3
#

# Makefile to build all the MyUSB Demos. Call with "make all" to
# rebuild all demos.

# Projects are pre-cleaned before each one is built, to ensure any
# custom MyUSB library build options are reflected in the compiled
# code.

all:
	make -C AudioInput clean
	make -C AudioInput all

	make -C AudioOutput clean
	make -C AudioOutput all

	make -C CDC clean
	make -C CDC all

	make -C Joystick clean
	make -C Joystick all

	make -C Keyboard clean
	make -C Keyboard all

	make -C KeyboardViaInt clean
	make -C KeyboardViaInt all

	make -C KeyboardHost clean
	make -C KeyboardHost all
	
	make -C KeyboardHostWithParser clean
	make -C KeyboardHostWithParser all

	make -C KeyboardHostViaInt clean
	make -C KeyboardHostViaInt all

	make -C Magstripe clean
	make -C Magstripe all

	make -C MassStorage clean
	make -C MassStorage all

	make -C MassStorageHost clean
	make -C MassStorageHost all

	make -C Mouse clean
	make -C Mouse all

	make -C MouseFullInt clean
	make -C MouseFullInt all

	make -C MouseViaInt clean
	make -C MouseViaInt all
	
	make -C MouseHost clean
	make -C MouseHost all

	make -C MouseHostWithParser clean
	make -C MouseHostWithParser all

	make -C MouseHostViaInt clean
	make -C MouseHostViaInt all

	make -C TestApp clean
	make -C TestApp all

	make -C USBtoSerial clean
	make -C USBtoSerial all

clean:
	make -C AudioInput clean
	make -C AudioOutput clean
	make -C CDC clean
	make -C Joystick clean
	make -C Keyboard clean
	make -C KeyboardViaInt clean
	make -C KeyboardHost clean
	make -C KeyboardHostWithParser clean
	make -C KeyboardHostViaInt clean
	make -C Magstripe clean
	make -C MassStorage clean
	make -C MassStorageHost clean
	make -C Mouse clean
	make -C MouseFullInt clean
	make -C MouseViaInt clean
	make -C MouseHost clean
	make -C MouseHostWithParser clean
	make -C MouseHostViaInt clean
	make -C TestApp clean
	make -C USBtoSerial clean
	
clean_list:
	make -C AudioInput clean_list
	make -C AudioOutput clean_list
	make -C CDC clean_list
	make -C Joystick clean_list
	make -C Keyboard clean_list
	make -C KeyboardInt clean_list
	make -C KeyboardHost clean_list
	make -C KeyboardHostWithParser clean_list
	make -C KeyboardHostViaInt clean_list
	make -C Magstripe clean_list
	make -C MassStorage clean_list
	make -C MassStorageHost clean_list
	make -C Mouse clean_list
	make -C MouseFullInt clean_list
	make -C MouseViaInt clean_list
	make -C MouseHost clean_list
	make -C MouseHostWithParser clean_list
	make -C MouseHostViaInt clean_list
	make -C TestApp clean_list
	make -C USBtoSerial clean_list
