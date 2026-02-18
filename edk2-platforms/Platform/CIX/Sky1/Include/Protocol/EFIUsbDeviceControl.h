/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef __USB_DEVICE_CONTROL_PROTOCOL_H__
#define __USB_DEVICE_CONTROL_PROTOCOL_H__

#include <Protocol/UsbIo.h>
#include <Protocol/UsbFunctionIo.h>
#include <UsbCommon.h>

#define FS_CONFIG_INDEX  0
#define HS_CONFIG_INDEX  1
#define SS_CONFIG_INDEX  2

typedef struct _CONFIG_NOT_SS_DESC {
  EFI_USB_CONFIG_DESCRIPTOR       ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR     EndpointDescriptorIn;
  EFI_USB_ENDPOINT_DESCRIPTOR     EndpointDescriptorOut;
} CONFIG_NOT_SS_DESC;

typedef struct _CONFIG_SS_DESC {
  EFI_USB_CONFIG_DESCRIPTOR       ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR     EndpointDescriptorIn;
  USB_SS_ENDPOINT_COMP_DES        EndpointDescriptorCompIn;
  EFI_USB_ENDPOINT_DESCRIPTOR     EndpointDescriptorOut;
  USB_SS_ENDPOINT_COMP_DES        EndpointDescriptorCompOut;
} CONFIG_SS_DESC;

typedef struct _FN_MESSAGE {
  EFI_USBFN_MESSAGE            Type;
  EFI_USBFN_MESSAGE_PAYLOAD    Playload;
} FN_MESSAGE;

#define EFI_USB_DEVICE_CONTROL_PROTOCAL_VERSION  0x0000000000000001
typedef struct _EFI_USB_DEVICE_CONTROL_PROTOCOL EFI_USB_DEVICE_CONTROL_PROTOCOL;

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_START) (
  IN VOID  **Descriptors
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_GET_DES) (
  IN OUT VOID  **ConfigDesc,
  IN UINT8     SpeedIndex
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_ALLOC_REQUEST_BUFFER) (
  IN   UINTN  Size,
  OUT  VOID   **Buffer
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_SEND)(
  IN       UINT8  EndpointIndex,
  IN       UINTN  Size,
  IN       VOID   *Buffer
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_FREE_REQUEST_BUFFER)(
  IN  VOID  **Buffer
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_HANDLE_EVENT) (
  OUT       FN_MESSAGE  *Message
  );

typedef
EFI_STATUS
(*USB_DEVICE_CONTROL_STOP) (
  VOID
  );

/*
 * Usb Device Control Protocal Interface
 */
struct _EFI_USB_DEVICE_CONTROL_PROTOCOL {
  UINTN                                      Revision;
  USB_DEVICE_CONTROL_START                   ControlStart;
  USB_DEVICE_CONTROL_GET_DES                 ControlGetDes;
  USB_DEVICE_CONTROL_ALLOC_REQUEST_BUFFER    AllocRequestBuffer;
  USB_DEVICE_CONTROL_FREE_REQUEST_BUFFER     FreeRequestBuffer;
  USB_DEVICE_CONTROL_SEND                    Send;
  USB_DEVICE_CONTROL_HANDLE_EVENT            HandleEvent;
  USB_DEVICE_CONTROL_STOP                    ControlStop;
};

#endif
