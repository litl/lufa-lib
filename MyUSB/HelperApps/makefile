#
#             MyUSB Library
#     Copyright (C) Dean Camera, 2007.
#              
#  dean [at] fourwalledcubicle [dot] com
#      www.fourwalledcubicle.com
#
# Released under the GPL Licence, Version 3
#

# Makefile to build the MyUSB Helper Applications. Call with "make all"
# to rebuild all applications. Compilation requires the installation of
# the Java SDK, avalialiable online from the Sun website.

APPDIRS = CheckLibEventHooks \
          MethodSig

all: start build end

start:
	@echo Building Helper Apps...
	@echo
	
build:
	javac $(APPDIRS:%=-classpath ./%/) $(APPDIRS:%=%/*.java)

end:
	@echo
	@echo Helper Apps Build Complete.

clean:
	rm -f $(APPDIRS:%=%/*.class)