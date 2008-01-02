#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2008.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the LGPL Licence, Version 3
#

# Makefile to build the MyUSB library and Demos.
# Call with "make all" to rebuild everything,
# "make clean" to clean everything, and
# "make clean_list" to remove all intermediatary
# files but preserve any binaries.

# It is suggested that for the master build, the --quiet switch
# is passed to make, to remove all the commands from the output.
# This gives a much easier to read report of the entire build
# process.

%:
	@echo Executing \"make $@\" on all MyUSB library elements.
	@echo
	make -C MyUSB/ $@
	make -C Demos/ $@
	@echo
	@echo MyUSB \"make $@\" operation complete.