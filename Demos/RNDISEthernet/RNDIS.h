/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _RNDIS_H_
#define _RNDIS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <string.h>

		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>
		
		#include "RNDISEthernet.h"

	/* Macros: */
		#define RNDIS_VERSION_MAJOR             0x01
		#define RNDIS_VERSION_MINOR             0x00
	
		#define SEND_ENCAPSULATED_COMMAND       0x00
		#define GET_ENCAPSULATED_RESPONSE       0x01
		
		#define RESPONSE_AVAILABLE              0x01
		
		#define REMOTE_NDIS_INITIALIZE_MSG      0x00000002UL
		#define REMOTE_NDIS_HALT_MSG            0x00000003UL
		#define REMOTE_NDIS_QUERY_MSG           0x00000004UL
		#define REMOTE_NDIS_SET_MSG             0x00000005UL
		#define REMOTE_NDIS_RESET_MSG           0x00000006UL
		#define REMOTE_NDIS_INDICATE_STATUS_MSG 0x00000007UL
		#define REMOTE_NDIS_KEEPALIVE_MSG       0x00000008UL

		#define REMOTE_NDIS_INITIALIZE_CMPLT    0x80000002UL
		#define REMOTE_NDIS_QUERY_CMPLT         0x80000004UL
		#define REMOTE_NDIS_SET_CMPLT           0x80000005UL
		#define REMOTE_NDIS_RESET_CMPLT         0x80000006UL
		#define REMOTE_NDIS_KEEPALIVE_CMPLT     0x80000008UL
		
		#define RNDIS_STATUS_SUCCESS            0x00000000UL
		#define RNDIS_STATUS_FAILURE            0xC0000001UL
		#define RNDIS_STATUS_INVALID_DATA       0xC0010015UL
		#define RNDIS_STATUS_NOT_SUPPORTED      0xC00000BBUL
		#define RNDIS_STATUS_MEDIA_CONNECT      0x4001000BUL
		#define RNDIS_STATUS_MEDIA_DISCONNECT   0x4001000CUL
		
		#define RNDIS_MEDIUM_802_3              0x00000000UL
		
		#define RNDIS_DF_CONNECTIONLESS	        0x00000001UL
		#define RNDIS_DF_CONNECTION_ORIENTED    0x00000002UL
		
		#define OID_GEN_SUPPORTED_LIST          0x00010101 // DONE
		#define OID_GEN_HARDWARE_STATUS         0x00010102 // DONE
		#define OID_GEN_MEDIA_SUPPORTED         0x00010103 // DONE
		#define OID_GEN_MEDIA_IN_USE            0x00010104 // DONE
		#define OID_GEN_MAXIMUM_FRAME_SIZE      0x00010106 // DONE
		#define OID_GEN_MAXIMUM_TOTAL_SIZE      0x00010111 // DONE
		#define OID_GEN_LINK_SPEED              0x00010107 // DONE
		#define OID_GEN_TRANSMIT_BLOCK_SIZE     0x0001010A
		#define OID_GEN_RECEIVE_BLOCK_SIZE      0x0001010B
		#define OID_GEN_VENDOR_ID               0x0001010C // DONE
		#define OID_GEN_VENDOR_DESCRIPTION      0x0001010D // DONE
		#define OID_GEN_CURRENT_PACKET_FILTER   0x0001010E // DONE
		#define OID_GEN_MAXIMUM_TOTAL_SIZE      0x00010111 // DONE
		#define OID_GEN_MEDIA_CONNECT_STATUS    0x00010114 // DONE
		#define OID_GEN_XMIT_OK                 0x00020101 // DONE
		#define OID_GEN_RCV_OK                  0x00020102 // DONE
		#define OID_GEN_XMIT_ERROR              0x00020103 // DONE
		#define OID_GEN_RCV_ERROR               0x00020104 // DONE
		#define OID_GEN_RCV_NO_BUFFER           0x00020105 // DONE
		#define OID_802_3_PERMANENT_ADDRESS     0x01010101 // DONE
		#define OID_802_3_CURRENT_ADDRESS       0x01010102 // DONE
		#define OID_802_3_MULTICAST_LIST        0x01010103 // DONE
		#define OID_802_3_MAXIMUM_LIST_SIZE     0x01010104 // DONE
		#define OID_802_3_RCV_ERROR_ALIGNMENT   0x01020101 // DONE
		#define OID_802_3_XMIT_ONE_COLLISION    0x01020102 // DONE
		#define OID_802_3_XMIT_MORE_COLLISIONS  0x01020103 // DONE
		
		#define ETHERNET_FRAME_SIZE             1522

	/* Enums: */
		enum RNDIS_States_t
		{
			RNDIS_Uninitialized    = 0,
			RNDIS_Initialized      = 1,
			RNDIS_Data_Initialized = 2,
		};

		enum NDIS_HARDWARE_STATUS_t
		{
			NdisHardwareStatusReady,
			NdisHardwareStatusInitializing,
			NdisHardwareStatusReset,
			NdisHardwareStatusClosing,
			NdisHardwareStatusNotReady
		};

	/* Type Defines: */	
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t DataOffset;
			uint32_t DataLength;
			uint32_t OOBDataOffset;
			uint32_t OOBDataLength;
			uint32_t NumOOBDataElements;
			uint32_t PerPacketInfoOffset;
			uint32_t PerPacketInfoLength;
			uint32_t VcHandle;
			uint32_t Reserved;
		} RNDIS_PACKET_MSG_t;

		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
		} RNDIS_Message_Header_t;
	
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			
			uint32_t MajorVersion;
			uint32_t MinorVersion;
			uint32_t MaxTransferSize;
		} RNDIS_INITIALIZE_MSG_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			uint32_t Status;
			
			uint32_t MajorVersion;
			uint32_t MinorVersion;
			uint32_t DeviceFlags;
			uint32_t Medium;
			uint32_t MaxPacketsPerTransfer;
			uint32_t MaxTransferSize;
			uint32_t PacketAlignmentFactor;
			uint32_t AFListOffset;
			uint32_t AFListSize;
		} RNDIS_INITIALIZE_CMPLT_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
		} RNDIS_KEEPALIVE_MSG_t;

		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			uint32_t Status;
		} RNDIS_KEEPALIVE_CMPLT_t;

		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t Status;

			uint32_t AddressingReset;
		} RNDIS_RESET_CMPLT_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			
			uint32_t Oid;
			uint32_t InformationBufferLength;
			uint32_t InformationBufferOffset;
			uint32_t DeviceVcHandle;
		} RNDIS_SET_MSG_t;

		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			uint32_t Status;
		} RNDIS_SET_CMPLT_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			
			uint32_t Oid;
			uint32_t InformationBufferLength;
			uint32_t InformationBufferOffset;
			uint32_t DeviceVcHandle;
		} RNDIS_QUERY_MSG_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t RequestId;
			uint32_t Status;
			
			uint32_t InformationBufferLength;
			uint32_t InformationBufferOffset;
		} RNDIS_QUERY_CMPLT_t;
		
		typedef struct
		{
			uint32_t MessageType;
			uint32_t MessageLength;
			uint32_t Status;
			uint32_t StatusBufferLength;
			uint32_t StatusBufferOffset;
		} RNDIS_INDICATE_STATUS_MSG_t;
		
		typedef struct
		{
			uint32_t DiagStatus;
			uint32_t ErrorOffset;	
		} RNDIS_Diagnostic_Info_t;
		
	/* External Variables: */
		extern RNDIS_Message_Header_t* MessageHeader;

		extern bool                    ResponseReady;
		extern uint8_t                 CurrRNDISState;
	
	/* Function Prototypes: */
		void ProcessRNDISControlMessage(void);

		#if defined(INCLUDE_FROM_RNDIS_C)
			static bool ProcessNDISQuery(uint32_t OId, void* QueryData, uint16_t QuerySize,
										 void* ResponseData, uint16_t* ResponseSize);
			static bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize);	
		#endif
		
#endif
