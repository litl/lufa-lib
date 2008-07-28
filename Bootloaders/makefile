#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#

# Makefile to build all the MyUSB USB Bootloaders. Call with "make all" to
# rebuild all bootloaders.

# Bootloaders are pre-cleaned before each one is built, to ensure any
# custom MyUSB library build options are reflected in the compiled
# code.

all:
	make -C 'DFU Class/' clean
	make -C 'DFU Class/' all

	make -C 'CDC Class/' clean
	make -C 'CDC Class/' all

clean:
	make -C 'DFU Class/' clean
	make -C 'CDC Class/' clean

clean_list:
	make -C 'DFU Class/' clean_list
	make -C 'CDC Class/' clean_list
