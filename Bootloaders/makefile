#
#             LUFA Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#

# Makefile to build all the LUFA USB Bootloaders. Call with "make all" to
# rebuild all bootloaders.

# Bootloaders are pre-cleaned before each one is built, to ensure any
# custom LUFA library build options are reflected in the compiled
# code.

all:
	make -C 'DFU/' clean
	make -C 'DFU/' all

	make -C 'CDC/' clean
	make -C 'CDC/' all

clean:
	make -C 'DFU/' clean
	make -C 'CDC/' clean

clean_list:
	make -C 'DFU/' clean_list
	make -C 'CDC/' clean_list
