#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to build the MyUSB Helper Applications and Demos.
# Call with "make all" to rebuild everything.

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