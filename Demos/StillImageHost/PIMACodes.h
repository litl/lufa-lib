/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _PIMA_CODES_H_

	/* Macros: */
		#define PIMA_OPERATION_GETDEVICEINFO         0x1001
		#define PIMA_OPERATION_OPENSESSION           0x1002
		#define PIMA_OPERATION_CLOSESESSION          0x1003
		
		#define PIMA_RESPONSE_OK                     0x2001
		#define PIMA_RESPONSE_GENERALERROR           0x2002
		#define PIMA_RESPONSE_SESSIONNOTOPEN         0x2003
		#define PIMA_RESPONSE_INVALIDTRANSACTIONID   0x2004
		#define PIMA_RESPONSE_OPERATIONNOTSUPPORTED  0x2005
		#define PIMA_RESPONSE_PARAMETERNOTSUPPORTED  0x2006
		

#endif
