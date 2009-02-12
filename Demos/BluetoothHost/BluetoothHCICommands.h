/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _BLUETOOTH_HCICOMMANDS_H_
#define _BLUETOOTH_HCICOMMANDS_H_

	/* Macros: */
		#define OGF_LINK_CONTROL                               0x01
		#define OGF_CTRLR_BASEBAND                             0x03

		#define OCF_LINK_CONTROL_INQUIRY                       0x0001
		#define OCF_LINK_CONTROL_INQUIRY_CANCEL                0x0002
		#define OCF_LINK_CONTROL_PERIODIC_INQUIRY              0x0003
		#define OCF_LINK_CONTROL_EXIT_PERIODIC_INQUIRY         0x0004
		#define OCF_LINK_CONTROL_CREATE_CONNECTION             0x0005
		#define OCF_LINK_CONTROL_DISCONNECT                    0x0006
		#define OCF_LINK_CONTROL_CREATE_CONNECTION_CANCEL      0x0008
		#define OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST     0x0009
		#define OCF_LINK_CONTROL_REJECT_CONNECTION_REQUEST     0x000A
		#define OCF_LINK_CONTROL_LINK_KEY_REQUEST_REPLY        0x000B
		#define OCF_LINK_CONTROL_LINK_KEY_REQUEST_NEG_REPLY    0x000C
		#define OCF_LINK_CONTROL_PIN_CODE_REQUEST_REPLY        0x000D
		#define OCF_LINK_CONTROL_PIN_CODE_REQUEST_NEG_REPLY    0x000E
		#define OCF_LINK_CONTROL_CHANGE_CONNECTION_PACKET_TYPE 0x000F
		#define OCF_LINK_CONTROL_REMOTE_NAME_REQUEST           0x0019
		#define OCF_CTRLR_SET_EVENT_MASK                       0x0001
		#define OCF_CTRLR_BASEBAND_RESET                       0x0003
		#define OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME            0x0013
		#define OCF_CTRLR_BASEBAND_READ_LOCAL_NAME             0x0014
		#define OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE           0x001A
		#define OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE       0x0024
		
		#define EVENT_COMMAND_STATUS                           0x0F
		#define EVENT_COMMAND_COMPLETE                         0x0E
		#define EVENT_CONNECTION_COMPLETE                      0x03
		#define EVENT_CONNECTION_REQUEST                       0x04
		#define EVENT_DISCONNECTION_COMPLETE                   0x05
		#define EVENT_REMOTE_NAME_REQUEST_COMPLETE             0x07
		

		#define ERROR_LIMITED_RESOURCES                        0x0D
		
	/* Enums: */
		enum Bluetooth_ScanEnable_Modes_t
		{
			NoScansEnabled            = 0,
			InquiryScanOnly           = 1,
			PageScanOnly              = 2,
			InquiryAndPageScans       = 3,
		};
		
#endif
