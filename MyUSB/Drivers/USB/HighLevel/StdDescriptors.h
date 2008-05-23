/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Standard USB device descriptor defines and retrieval routines, for USB devices. This module contains
 *  strucutures and macros for the easy creation of standard USB descriptors in USB device projects.
 */

#ifndef __USB_DESCRIPTORS_H__
#define __USB_DESCRIPTORS_H__

	/* Includes: */
		#include <avr/pgmspace.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"
		#include "../LowLevel/USBMode.h"
		#include "Events.h"
		
		#if defined(USB_CAN_BE_DEVICE)
			#include "../LowLevel/Device.h"
		#endif

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Indicates that a given descriptor string index is NULL, i.e. has no associated string descriptor
			 *  describing the descriptor.
			 */
			#define NO_DESCRIPTOR_STRING              0
			
			/** Macro to calculate the power value for the device descriptor, from a given number of milliamps. */
			#define USB_CONFIG_POWER_MA(x)            (x >> 1)

			/** Macro to calculate the Unicode length of a string with a given number of Unicode characters.
			 *  Should be used in string descriptor's headers for giving the string descriptor's byte length.
			 */
			#define USB_STRING_LEN(x)                 (sizeof(USB_Descriptor_Header_t) + (x << 1))
			
			/** String language ID for the English language. Should be used in USB_Descriptor_Language_t descriptors
			 *  to indicate that the English language is supported by the device in its string descriptors.
			 */
			#define LANGUAGE_ID_ENG                   0x0409

			/** Can be masked with an endpoint address for a USB_Descriptor_Endpoint_t endpoint descriptor's
			 *  EndpointAddress value to indicate to the host that the endpoint is of the IN direction (i.e, from
			 *  device to host).
			 */
			#define ENDPOINT_DESCRIPTOR_DIR_IN        0x80

			/** Can be masked with an endpoint address for a USB_Descriptor_Endpoint_t endpoint descriptor's
			 *  EndpointAddress value to indicate to the host that the endpoint is of the OUT direction (i.e, from
			 *  host to device).
			 */
			#define ENDPOINT_DESCRIPTOR_DIR_OUT       0x00		

			/** Can be masked with other configuration descriptor attributes for a USB_Descriptor_Configuration_Header_t
			 *  descriptor's ConfigAttributes value to indicate that the specified configuration can draw its power
			 *  from the host's VBUS line.
			 */
			#define USB_CONFIG_ATTR_BUSPOWERED        0b10000000

			/** Can be masked with other configuration descriptor attributes for a USB_Descriptor_Configuration_Header_t
			 *  descriptor's ConfigAttributes value to indicate that the specified configuration can draw its power
			 *  from the device's own power source.
			 */
			#define USB_CONFIG_ATTR_SELFPOWERED       0b11000000

			/** Can be masked with other configuration descriptor attributes for a USB_Descriptor_Configuration_Header_t
			 *  descriptor's ConfigAttributes value to indicate that the specified configuration supports the
			 *  remote wakeup feature of the USB standard, allowing a suspended USB device to wake up the host upon
			 *  request.
			 */
			#define USB_CONFIG_ATTR_REMOTEWAKEUP      0b10100000

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is not synchronized.
			 *
			 *  \see The USB specification for more details on the possible Endpoint attributes.
			 */
			#define ENDPOINT_ATTR_NO_SYNC             (0b00 << 2)

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is asynchronous.
			 *
			 *  \see The USB specification for more details on the possible Endpoint attributes.
			 */
			#define ENDPOINT_ATTR_ASYNC               (0b01 << 2)

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is adaptive.
			 *
			 *  \see The USB specification for more details on the possible Endpoint attributes.
			 */
			#define ENDPOINT_ATTR_ADAPTIVE            (0b10 << 2)

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is synchronized.
			 *
			 *  \see The USB specification for more details on the possible Endpoint attributes.
			 */
			#define ENDPOINT_ATTR_SYNC                (0b11 << 2)
			
			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is used for data transfers.
			 *
			 *  \see The USB specification for more details on the possible Endpoint usage attributes.
			 */
			#define ENDPOINT_USAGE_DATA               (0b00 << 4)

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is used for feedback.
			 *
			 *  \see The USB specification for more details on the possible Endpoint usage attributes.
			 */
			#define ENDPOINT_USAGE_FEEDBACK           (0b01 << 4)

			/** Can be masked with other endpoint descriptor attributes for a USB_Descriptor_Endpoint_t descriptor's
			 *  Attributes value to indicate that the specified endpoint is used for implicit feedback.
			 *
			 *  \see The USB specification for more details on the possible Endpoint usage attributes.
			 */
			#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (0b10 << 4)

			/** Gives a void pointer to the specified descriptor (of any type). */
			#define DESCRIPTOR_ADDRESS(Descriptor)    ((void*)&Descriptor)

		/* Events: */
			#if defined(USB_CAN_BE_DEVICE) || defined(__DOXYGEN__)
				/** This module raises the Device Error event while in device mode, if the USB_GetDescriptor()
				 *  routine is not hooked in the user application to properly return descriptors to the library.
				 *
				 *  \see Events.h for more information on this event.
				 */
				RAISES_EVENT(USB_DeviceError);
			#endif
			
		/* Enums: */
			/** Enum for the possible standard descriptor types, as given in each descriptor's header. */
			enum DescriptorTypes_t
			{
				DTYPE_Device            = 1, /**< Indicates that the descriptor is a device descriptor. */
				DTYPE_Configuration     = 2, /**< Indicates that the descriptor is a configuration descriptor. */
				DTYPE_String            = 3, /**< Indicates that the descriptor is a string descriptor. */
				DTYPE_Interface         = 4, /**< Indicates that the descriptor is an interface descriptor. */
				DTYPE_Endpoint          = 5, /**< Indicates that the descriptor is an endpoint descriptor. */
				DTYPE_DeviceQualifier   = 6, /**< Indicates that the descriptor is a device qualifier descriptor. */
				DTYPE_Other             = 7, /**< Indicates that the descriptor is of other type. */
				DTYPE_InterfacePower    = 8, /**< Indicates that the descriptor is an interface power descriptor. */
			};

		/* Type Defines: */
			/** Type define for all descriptor's header, indicating the descriptor's length and type. */
			typedef struct
			{
				uint8_t Size; /**< Size of the descriptor, in bytes. */
				uint8_t Type; /**< Type of the descriptor, either a value in DescriptorTypes_t or a value
				                   given by the specific class. */
			} USB_Descriptor_Header_t;
			
			/** Type define for a standard device descriptor.
			 *
			 *  \note The structure entities have more verbose names than those in the USB 2.0 specification,
			 *        but are identical in size and function. They appear in the same order as in the specification,
			 *        thus their exact function can be determined from the USB 2.0 specification directly.
			 */
			typedef struct
			{
				USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

				uint16_t                USBSpecification; /**< BCD of the supported USB specification. */
				uint8_t                 Class; /**< USB device class. */
				uint8_t                 SubClass; /**< USB device subclass. */
				uint8_t                 Protocol; /**< USB device protocol. */
				
				uint8_t                 Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */
				
				uint16_t                VendorID; /**< Vendor ID for the USB product. */
				uint16_t                ProductID; /**< Unique product ID for the USB product. */
				uint16_t                ReleaseNumber; /**< Product release (version) number. */
				
				uint8_t                 ManafacturerStrIndex; /**< String index for the manafacturer's name. The
                                                               *   host will request this string via a seperate
			                                                   *   control request for the string descriptor.
				                                               *
				                                               *   \note If no string supplied, use NO_DESCRIPTOR_STRING. */
				uint8_t                 ProductStrIndex; /**< String index for the product name/details.
				                                          *
				                                          *  \see ManafacturerStrIndex structure entry.
				                                          */
				uint8_t                 SerialNumStrIndex; /**< String index for the product's globally unique hexadecimal
				                                            *   serial number, in uppercase Unicoded ASCII.
				                                            *
				                                            *  \see ManafacturerStrIndex structure entry.
				                                            */

				uint8_t                 NumberOfConfigurations; /**< Total number of configurations supported by
				                                                 *   the device.
				                                                 */
			} USB_Descriptor_Device_t;

			/** Type define for a standard configuration descriptor.
			 *
			 *  \note The structure entities have more verbose names than those in the USB 2.0 specification,
			 *        but are identical in size and function. They appear in the same order as in the specification,
			 *        thus their exact function can be determined from the USB 2.0 specification directly.
			 */
			typedef struct
			{
				USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
			
				uint16_t                TotalConfigurationSize; /**< Size of the configuration descriptor header,
				                                                 *   and all sub descriptors inside the configuration.
				                                                 */
				uint8_t                 TotalInterfaces; /**< Total number of interfaces in the configuration. */

				uint8_t                 ConfigurationNumber; /**< Configuration index of the current configuration. */
				uint8_t                 ConfigurationStrIndex; /**< Index of a string descriptor describing the configuration. */
				
				uint8_t                 ConfigAttributes; /**< Configuration attributes, comprised of a mask of zero or
				                                           *   more USB_CONFIG_ATTR_* masks.
				                                           */
				
				uint8_t                 MaxPowerConsumption; /**< Maximum power consumption of the device while in the
				                                              *   current configuration, calculated by the USB_CONFIG_POWER_MA()
				                                              *   macro.
				                                              */
			} USB_Descriptor_Configuration_Header_t;

			/** Type define for a standard interface descriptor.
			 *
			 *  \note The structure entities have more verbose names than those in the USB 2.0 specification,
			 *        but are identical in size and function. They appear in the same order as in the specification,
			 *        thus their exact function can be determined from the USB 2.0 specification directly.
			 */
			typedef struct
			{
				USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

				uint8_t                 InterfaceNumber; /**< Index of the interface in the current configuration. */
				uint8_t                 AlternateSetting; /**< Alternate setting for the interface number. The same
				                                           *   interface number can have multiple alternate settings
				                                           *   with different endpoint configurations, which can be
				                                           *   selected by the host.
				                                           */
				uint8_t                 TotalEndpoints; /**< Total number of endpoints in the interface. */
				
				uint8_t                 Class; /**< Interface class ID. */
				uint8_t                 SubClass; /**< Interface subclass ID. */
				uint8_t                 Protocol; /**< Interface protocol ID. */

				uint8_t                 InterfaceStrIndex; /**< Index of the string descriptor describing the
				                                            *   interface.
				                                            */
			} USB_Descriptor_Interface_t;

			/** Type define for a standard endpoint descriptor.
			 *
			 *  \note The structure entities have more verbose names than those in the USB 2.0 specification,
			 *        but are identical in size and function. They appear in the same order as in the specification,
			 *        thus their exact function can be determined from the USB 2.0 specification directly.
			 */
			typedef struct
			{
				USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

				uint8_t                 EndpointAddress; /**< Logical address of the endpoint within the device
				                                          *   for the current configuration, including direction
				                                          *   mask.
				                                          */
				uint8_t                 Attributes; /**< Endpoint attributes, comprised of a mask of the
				                                     *   endpoint type (EP_TYPE_*) and attributes (ENDPOINT_ATTR_*)
				                                     *   masks.
				                                     */
				uint16_t                EndpointSize; /**< Size of the endpoint bank, in bytes. This indicates the
				                                       *   maximum packet size that the endpoint can receive at a time.
				                                       */
				
				uint8_t                 PollingIntervalMS; /**< Polling interval in milliseconds for the endpont
				                                            *   if it is an INTERRUPT or ISOCHRONOUS type.
				                                            */
			} USB_Descriptor_Endpoint_t;

			/** Type define for a standard string descriptor. Unlike other standard descriptors, the length
			 *  of the descriptor for placement in the descriptor header must be determined by the USB_STRING_LEN()
			 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
			 *
			 *  This structure should also be used for string index 0, which contains the supported language IDs for
			 *  the device as an array.
			 *
			 *  \note The structure entities have more verbose names than those in the USB 2.0 specification,
			 *        but are identical in size and function. They appear in the same order as in the specification,
			 *        thus their exact function can be determined from the USB 2.0 specification directly.
			 */
			typedef struct
			{
				USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
				
				uint16_t                UnicodeString[]; /**< String data, as unicode characters (alternatively,
				                                          *   string language IDs). If normal ASCII characters are
				                                          *   to be used, they must be added as an array of characters
				                                          *   rather than a normal C string so that they are widened to
				                                          *   Unicode size. */
			} USB_Descriptor_String_t;

		/* Function Prototypes: */
			/** Function to retrieve a given descriptor's size and memory location from the given descriptor
			 *  type value, index and language ID. This function must be overridden in the user application
			 *  (added with full, identical prototype and name) so that the library can call it to retrieve
			 *  descriptor data.
			 *
			 *  \param Type            The type of the descriptor to retrieve. This may be one of the standard types defined
			 *                         in the DescriptorTypes_t enum, or may be a class-specific descriptor type value.
			 *  \param Index           Index of the descriptor to retrieve, when more than one of the given descriptor
			 *                         types exist. For example, the descriptor type may be DTYPE_String, whereupon the
			 *                         Index parameter will give the index number of the string to retreive.
			 *  \param LanguageID      The language ID of the string to return. On devices supporting multiple languages,
			 *                         this value selects the desired language of the string descriptor. For descriptors
			 *                         other than string descriptors, this value is set to 0.
			 *  \param DescriptorAddr  Pointer to the descriptor in memory. This should be set by the routine to
			 *                         the location of the descriptor, found by the DESCRIPTOR_ADDRESS macro.
			 *  \param DescriptorSize  Pointer to a variable storing the size of the requested descriptor. This
			 *                         should be set by the routine to the size in bytes of the descriptor.
			 *
			 *  \return Boolean true if the requested descriptor exists, false otherwise
			 */
			bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
			                       void** const DescriptorAddr, uint16_t* const DescriptorSize)
			                       ATTR_WARN_UNUSED_RESULT ATTR_WEAK ATTR_NON_NULL_PTR_ARG(4, 5);

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
