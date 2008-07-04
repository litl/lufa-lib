/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define  INCLUDE_FROM_RNDIS_C
#include "RNDIS.h"

/* Global Variables: */
uint8_t RNDISBuffer[512];

RNDIS_Message_Header_t* MessageHeader = (RNDIS_Message_Header_t*)&RNDISBuffer;

bool                    ResponseReady               = false;
uint8_t                 CurrRNDISState              = RNDIS_Uninitialized;
uint32_t                CurrPacketFilter            = 0;

static char             VendorDescription[] PROGMEM = "MyUSB RNDIS";
static MAC_Address_t    MACAddress          PROGMEM = {{0x40,0x01,0x02,0x03,0x04,0x05}};
static uint32_t         SupportedOIDList[]  PROGMEM =
							{
								OID_GEN_SUPPORTED_LIST,
								OID_GEN_HARDWARE_STATUS,
								OID_GEN_MEDIA_SUPPORTED,
								OID_GEN_MEDIA_IN_USE,
								OID_GEN_MAXIMUM_FRAME_SIZE,
								OID_GEN_MAXIMUM_TOTAL_SIZE,
								OID_GEN_LINK_SPEED,
								OID_GEN_TRANSMIT_BLOCK_SIZE,
								OID_GEN_RECEIVE_BLOCK_SIZE,
								OID_GEN_VENDOR_ID,
								OID_GEN_VENDOR_DESCRIPTION,
								OID_GEN_CURRENT_PACKET_FILTER,
								OID_GEN_MAXIMUM_TOTAL_SIZE,
								OID_GEN_MEDIA_CONNECT_STATUS,
								OID_GEN_XMIT_OK,
								OID_GEN_RCV_OK,
								OID_GEN_XMIT_ERROR,
								OID_GEN_RCV_ERROR,
								OID_GEN_RCV_NO_BUFFER,
								OID_802_3_PERMANENT_ADDRESS,
								OID_802_3_CURRENT_ADDRESS,
								OID_802_3_MULTICAST_LIST,
								OID_802_3_MAXIMUM_LIST_SIZE,
								OID_802_3_RCV_ERROR_ALIGNMENT,
								OID_802_3_XMIT_ONE_COLLISION,
								OID_802_3_XMIT_MORE_COLLISIONS,
							};


void ProcessRNDISControlMessage(void)
{
	/* Note: Only a single buffer is used for both the received message and its response to save SRAM. Because of
	         this, response bytes should be filled in order so that they do not clobber unread data in the buffer. */

	switch (MessageHeader->MessageType)
	{
		case REMOTE_NDIS_INITIALIZE_MSG:
			ResponseReady = true;
			
			RNDIS_INITIALIZE_MSG_t*   INITIALIZE_Message  = (RNDIS_INITIALIZE_MSG_t*)&RNDISBuffer;
			RNDIS_INITIALIZE_CMPLT_t* INITIALIZE_Response = (RNDIS_INITIALIZE_CMPLT_t*)&RNDISBuffer;
			
			INITIALIZE_Response->MessageType           = REMOTE_NDIS_INITIALIZE_CMPLT;
			INITIALIZE_Response->MessageLength         = sizeof(RNDIS_INITIALIZE_CMPLT_t);
			INITIALIZE_Response->RequestId             = INITIALIZE_Message->RequestId;
			INITIALIZE_Response->Status                = REMOTE_NDIS_STATUS_SUCCESS;
			
			INITIALIZE_Response->MajorVersion          = REMOTE_NDIS_VERSION_MAJOR;
			INITIALIZE_Response->MinorVersion          = REMOTE_NDIS_VERSION_MINOR;			
			INITIALIZE_Response->DeviceFlags           = REMOTE_NDIS_DF_CONNECTIONLESS;
			INITIALIZE_Response->Medium                = REMOTE_NDIS_MEDIUM_802_3;
			INITIALIZE_Response->MaxPacketsPerTransfer = 1;
			INITIALIZE_Response->MaxTransferSize       = (sizeof(RNDISBuffer) + ETHERNET_FRAME_SIZE);
			INITIALIZE_Response->PacketAlignmentFactor = 0;
			INITIALIZE_Response->AFListOffset          = 0;
			INITIALIZE_Response->AFListSize            = 0;
			
			CurrRNDISState = RNDIS_Initialized;
		
			break;
		case REMOTE_NDIS_HALT_MSG:
			ResponseReady = false;
			MessageHeader->MessageLength = 0;

			CurrRNDISState = RNDIS_Uninitialized;

			break;
		case REMOTE_NDIS_QUERY_MSG:
			ResponseReady = true;
			
			RNDIS_QUERY_MSG_t*   QUERY_Message  = (RNDIS_QUERY_MSG_t*)&RNDISBuffer;
			RNDIS_QUERY_CMPLT_t* QUERY_Response = (RNDIS_QUERY_CMPLT_t*)&RNDISBuffer;
			uint32_t             Query_Oid      = QUERY_Message->Oid;
						
			void*     QueryData                 = &RNDISBuffer[sizeof(RNDIS_Message_Header_t) +
			                                                   QUERY_Message->InformationBufferOffset];
			void*     ResponseData              = &RNDISBuffer[sizeof(RNDIS_QUERY_CMPLT_t)];		
			uint16_t  ResponseSize;

			QUERY_Response->MessageType         = REMOTE_NDIS_QUERY_CMPLT;
			QUERY_Response->MessageLength       = sizeof(RNDIS_QUERY_CMPLT_t);
						
			if (ProcessNDISQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength,
			                     ResponseData, &ResponseSize))
			{
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_SUCCESS;
				QUERY_Response->MessageLength          += ResponseSize;
							
				QUERY_Response->InformationBufferLength = ResponseSize;
				QUERY_Response->InformationBufferOffset = (sizeof(RNDIS_QUERY_CMPLT_t) - sizeof(RNDIS_Message_Header_t));
			}
			else
			{				
				QUERY_Response->Status                  = REMOTE_NDIS_STATUS_NOT_SUPPORTED;

				QUERY_Response->InformationBufferLength = 0;
				QUERY_Response->InformationBufferOffset = 0;

				printf("UNKQ %08lx:\r\n", Query_Oid);
			}
			
			break;
		case REMOTE_NDIS_SET_MSG:
			ResponseReady = true;
			
			RNDIS_SET_MSG_t*   SET_Message  = (RNDIS_SET_MSG_t*)&RNDISBuffer;
			RNDIS_SET_CMPLT_t* SET_Response = (RNDIS_SET_CMPLT_t*)&RNDISBuffer;
			uint32_t           SET_Oid      = SET_Message->Oid;

			SET_Response->MessageType       = REMOTE_NDIS_SET_CMPLT;
			SET_Response->MessageLength     = sizeof(RNDIS_SET_CMPLT_t);
			SET_Response->RequestId         = SET_Message->RequestId;

			void*     SetData               = &RNDISBuffer[sizeof(RNDIS_Message_Header_t) +
			                                          SET_Message->InformationBufferOffset];
						
			if (ProcessNDISSet(SET_Oid, SetData, SET_Message->InformationBufferLength))
			{
				SET_Response->Status        = REMOTE_NDIS_STATUS_SUCCESS;
			}
			else
			{				
				SET_Response->Status        = REMOTE_NDIS_STATUS_NOT_SUPPORTED;

				printf("UNKS %08lx:\r\n", SET_Oid);
			}

			break;
		case REMOTE_NDIS_RESET_MSG:
			ResponseReady = true;
			
			RNDIS_RESET_CMPLT_t* RESET_Response = (RNDIS_RESET_CMPLT_t*)&RNDISBuffer;

			RESET_Response->MessageType         = REMOTE_NDIS_RESET_CMPLT;
			RESET_Response->MessageLength       = sizeof(RNDIS_RESET_CMPLT_t);
			RESET_Response->Status              = REMOTE_NDIS_STATUS_SUCCESS;

			break;
		case REMOTE_NDIS_KEEPALIVE_MSG:
			ResponseReady = true;
			
			RNDIS_KEEPALIVE_MSG_t*   KEEPALIVE_Message  = (RNDIS_KEEPALIVE_MSG_t*)&RNDISBuffer;
			RNDIS_KEEPALIVE_CMPLT_t* KEEPALIVE_Response = (RNDIS_KEEPALIVE_CMPLT_t*)&RNDISBuffer;

			KEEPALIVE_Response->MessageType     = REMOTE_NDIS_KEEPALIVE_CMPLT;
			KEEPALIVE_Response->MessageLength   = sizeof(RNDIS_KEEPALIVE_CMPLT_t);
			KEEPALIVE_Response->RequestId       = KEEPALIVE_Message->RequestId;
			KEEPALIVE_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;

			break;
	}
}

static bool ProcessNDISQuery(uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize)
{
	switch (OId)
	{
		case OID_GEN_SUPPORTED_LIST:
			*ResponseSize = sizeof(SupportedOIDList);
			
			memcpy_P(ResponseData, SupportedOIDList, sizeof(SupportedOIDList));
			
			return true;
		case OID_GEN_HARDWARE_STATUS:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = NdisHardwareStatusReady;
			
			return true;
		case OID_GEN_MEDIA_SUPPORTED:
		case OID_GEN_MEDIA_IN_USE:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIUM_802_3;
			
			return true;
		case OID_GEN_VENDOR_ID:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = 0x00FFFFFF;
			
			return true;
		case OID_GEN_MAXIMUM_FRAME_SIZE:
		case OID_GEN_TRANSMIT_BLOCK_SIZE:
		case OID_GEN_RECEIVE_BLOCK_SIZE:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = ETHERNET_FRAME_SIZE;
			
			return true;
		case OID_GEN_VENDOR_DESCRIPTION:
			*ResponseSize = sizeof(uint32_t);
			
			memcpy_P(ResponseData, VendorDescription, sizeof(VendorDescription));
			
			return true;
		case OID_GEN_MEDIA_CONNECT_STATUS:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
			
			return true;
		case OID_GEN_LINK_SPEED:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = 100000;

			return true;
		case OID_802_3_PERMANENT_ADDRESS:
		case OID_802_3_CURRENT_ADDRESS:
			*ResponseSize = sizeof(MACAddress);
			
			memcpy_P(ResponseData, &MACAddress, sizeof(MACAddress));

			return true;
		case OID_802_3_MAXIMUM_LIST_SIZE:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = 1;
		
			return true;
		case OID_GEN_CURRENT_PACKET_FILTER:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = CurrPacketFilter;
		
			return true;			
		case OID_GEN_XMIT_OK:
		case OID_GEN_RCV_OK:
		case OID_GEN_XMIT_ERROR:
		case OID_GEN_RCV_ERROR:
		case OID_GEN_RCV_NO_BUFFER:
		case OID_802_3_RCV_ERROR_ALIGNMENT:
		case OID_802_3_XMIT_ONE_COLLISION:
		case OID_802_3_XMIT_MORE_COLLISIONS:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = 0;
		
			return true;
		case OID_GEN_MAXIMUM_TOTAL_SIZE:
			*ResponseSize = sizeof(uint32_t);
			
			*((uint32_t*)ResponseData) = (sizeof(RNDISBuffer) + ETHERNET_FRAME_SIZE);
		
			return true;
		default:
			return false;
	}
}

static bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize)
{
	switch (OId)
	{
		case OID_GEN_CURRENT_PACKET_FILTER:
			CurrPacketFilter = *((uint32_t*)SetData);
		
			CurrRNDISState = ((CurrPacketFilter) ? RNDIS_Data_Initialized : RNDIS_Data_Initialized);
			
			return true;
		case OID_802_3_MULTICAST_LIST:
			return true;
		default:
			return false;
	}
}
