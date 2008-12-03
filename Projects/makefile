#
#             LUFA Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#

# Makefile to build all the LUFA Projects. Call with "make all" to
# rebuild all projects.

# Projects are pre-cleaned before each one is built, to ensure any
# custom LUFA library build options are reflected in the compiled
# code.

all:
	make -C Magstripe clean
	make -C Magstripe all

clean:
	make -C Magstripe clean
	
clean_list:
	make -C Magstripe clean_list
