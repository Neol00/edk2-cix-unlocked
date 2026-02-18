#ifndef __NETWORK_STACK_SETUP_H__
#define __NETWORK_STACK_SETUP_H__

#define NETWORK_STACK_VAR  L"NetworkStackVar"
#define NETWORK_STACK_SETUP_VARIABLE_GUID \
  { 0x8c146f33, 0xd931, 0x497c, { 0x8c, 0x2e, 0x47, 0x96, 0xdc, 0xc5, 0xfd, 0xda }}

#pragma pack(1)
typedef struct {
  UINT8    Enable;
  UINT8    Ipv4Pxe;
  UINT8    Ipv6Pxe;
  UINT8    Ipv4Http;
  UINT8    Ipv6Http;
} NETWORK_STACK;
#pragma pack()

extern EFI_GUID  gEfiNetworkStackSetupGuid;

#endif
