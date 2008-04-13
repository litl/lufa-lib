/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _HIDREPORTDATA_H_
#define _HIDREPORTDATA_H_

	/* Macros: */
		#define DATA_SIZE_MASK           0b00000011
		#define TYPE_MASK                0b00001100
		#define TAG_MASK                 0b11110000

		#define DATA_SIZE_0              0b00000000
		#define DATA_SIZE_1              0b00000001
		#define DATA_SIZE_2              0b00000010
		#define DATA_SIZE_4              0b00000011
		
		#define TYPE_MAIN                0b00000000
		#define TYPE_GLOBAL              0b00000100
		#define TYPE_LOCAL               0b00001000
		
		#define MAIN_TAG_INPUT           0b10000000
		#define MAIN_TAG_OUTPUT          0b10010000
		#define MAIN_TAG_FEATURE         0b10110000
		#define MAIN_TAG_COLLECTION      0b10100000
		#define MAIN_TAG_ENDCOLLECTION   0b11000000
		#define GLOBAL_TAG_USAGEPAGE     0b00000000
		#define GLOBAL_TAG_LOGICALMIN    0b00010000
		#define GLOBAL_TAG_LOGICALMAX    0b00100000
		#define GLOBAL_TAG_PHYSMIN       0b00110000
		#define GLOBAL_TAG_PHYSMAX       0b01000000
		#define GLOBAL_TAG_UNITEXP       0b01010000
		#define GLOBAL_TAG_UNIT          0b01100000
		#define GLOBAL_TAG_REPORTSIZE    0b01110000
		#define GLOBAL_TAG_REPORTID      0b10000000
		#define GLOBAL_TAG_REPORTCOUNT   0b10010000
		#define GLOBAL_TAG_PUSH          0b10100000
		#define GLOBAL_TAG_POP           0b10110000
		#define LOCAL_TAG_USAGE          0b00000000
		#define LOCAL_TAG_USAGEMIN       0b00010000
		#define LOCAL_TAG_USAGEMAX       0b00100000
		
		#define IOF_CONSTANT             (1 << 0)
		#define IOF_DATA                 (0 << 0)
		#define IOF_VARIABLE             (1 << 1)
		#define IOF_ARRAY                (0 << 1)
		#define IOF_RELATIVE             (1 << 2)
		#define IOF_ABSOLUTE             (0 << 2)
		#define IOF_WRAP                 (1 << 3)
		#define IOF_NOWRAP               (0 << 3)
		#define IOF_NONLINEAR            (1 << 4)
		#define IOF_LINEAR               (0 << 4)
		#define IOF_NOPREFERRED          (1 << 5)
		#define IOF_PREFERREDSTATE       (0 << 5)
		#define IOF_NULLSTATE            (1 << 6)
		#define IOF_NONULLPOSITION       (0 << 6)
		#define IOF_BUFFEREDBYTES        (1 << 8)
		#define IOF_BITFIELD             (0 << 8)
		
#endif
