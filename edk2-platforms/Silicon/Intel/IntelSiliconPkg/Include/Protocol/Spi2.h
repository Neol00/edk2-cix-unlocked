/** @file
  This file defines the PCH SPI 2 Protocol which implements the
  Intel(R) PCH SPI Host Controller Compatibility Interface.

  This SPI Protocol differs from the PCH SPI 1 Protocol interface
  primarily by identifying SPI flash regions by GUID instead
  of numeric values.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  Copyright (c) Microsoft Corporation.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_SPI2_PROTOCOL_H_
#define _PCH_SPI2_PROTOCOL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                   gPchSpi2ProtocolGuid;
extern EFI_GUID                   gPchSmmSpi2ProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_SPI2_PROTOCOL  PCH_SPI2_PROTOCOL;

//
// Protocol member functions
//

/**
  Read data from the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] FlashRegionGuid      The Flash Region GUID for flash cycle which corresponds to the type in the descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer              The Pointer to caller-allocated buffer containing the dada received.
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_READ) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     EFI_GUID           *FlashRegionGuid,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  );

/**
  Write data to the flash part. Remark: Erase may be needed before write to the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] FlashRegionGuid      The Flash Region GUID for flash cycle which corresponds to the type in the descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[in] Buffer               Pointer to caller-allocated buffer containing the data sent during the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_WRITE) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     EFI_GUID           *FlashRegionGuid,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN     UINT8              *Buffer
  );

/**
  Erase some area on the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] FlashRegionGuid      The Flash Region GUID for flash cycle which corresponds to the type in the descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_ERASE) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     EFI_GUID           *FlashRegionGuid,
  IN     UINT32             Address,
  IN     UINT32             ByteCount
  );

/**
  Read SFDP data from the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] ComponentNumber      The Component Number for chip select
  @param[in] Address              The starting byte address for SFDP data read.
  @param[in] ByteCount            Number of bytes in SFDP data portion of the SPI cycle
  @param[out] SfdpData            The Pointer to caller-allocated buffer containing the SFDP data received
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_READ_SFDP) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT8              ComponentNumber,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SfdpData
  );

/**
  Read Jedec Id from the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] ComponentNumber      The Component Number for chip select
  @param[in] ByteCount            Number of bytes in JedecId data portion of the SPI cycle, the data size is 3 typically
  @param[out] JedecId             The Pointer to caller-allocated buffer containing JEDEC ID received
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_READ_JEDEC_ID) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *JedecId
  );

/**
  Write the status register in the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] ByteCount            Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[in] StatusValue          The Pointer to caller-allocated buffer containing the value of Status register writing

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_WRITE_STATUS) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT32             ByteCount,
  IN     UINT8              *StatusValue
  );

/**
  Read status register in the flash part.

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] ByteCount            Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[out] StatusValue         The Pointer to caller-allocated buffer containing the value of Status register received.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_FLASH_READ_STATUS) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT32             ByteCount,
  OUT    UINT8              *StatusValue
  );

/**
  Get the SPI region base and size, based on the enum type

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] FlashRegionGuid      The Flash Region GUID for flash cycle which corresponds to the type in the descriptor.
  @param[out] BaseAddress         The Flash Linear Address for the Region 'n' Base
  @param[out] RegionSize          The size for the Region 'n'

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_DEVICE_ERROR        The region is not used
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_GET_REGION_ADDRESS) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     EFI_GUID           *FlashRegionGuid,
  OUT    UINT32             *BaseAddress,
  OUT    UINT32             *RegionSize
  );

/**
  Read PCH Soft Strap Values

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] SoftStrapAddr        PCH Soft Strap address offset from FPSBA.
  @param[in] ByteCount            Number of bytes in SoftStrap data portion of the SPI cycle
  @param[out] SoftStrapValue      The Pointer to caller-allocated buffer containing PCH Soft Strap Value.
                                  If the value of ByteCount is 0, the data type of SoftStrapValue should be UINT16 and SoftStrapValue will be PCH Soft Strap Length
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_READ_PCH_SOFTSTRAP) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT32             SoftStrapAddr,
  IN     UINT32             ByteCount,
  OUT    VOID               *SoftStrapValue
  );

/**
  Read CPU Soft Strap Values

  @param[in] This                 Pointer to the PCH_SPI2_PROTOCOL instance.
  @param[in] SoftStrapAddr        CPU Soft Strap address offset from FCPUSBA.
  @param[in] ByteCount            Number of bytes in SoftStrap data portion of the SPI cycle.
  @param[out] SoftStrapValue      The Pointer to caller-allocated buffer containing CPU Soft Strap Value.
                                  If the value of ByteCount is 0, the data type of SoftStrapValue should be UINT16 and SoftStrapValue will be PCH Soft Strap Length
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SPI_READ_CPU_SOFTSTRAP) (
  IN     PCH_SPI2_PROTOCOL  *This,
  IN     UINT32             SoftStrapAddr,
  IN     UINT32             ByteCount,
  OUT    VOID               *SoftStrapValue
  );

/**
  These protocols/PPI allows a platform module to perform SPI operations through the
  Intel PCH SPI Host Controller Interface.

**/
struct _PCH_SPI2_PROTOCOL {
  /**
    This member specifies the revision of this structure. This field is used to
    indicate backwards compatible changes to the protocol.
  **/
  UINT8                             Revision;
  PCH_SPI_FLASH_READ                FlashRead;          ///< Read data from the flash part.
  PCH_SPI_FLASH_WRITE               FlashWrite;         ///< Write data to the flash part. Remark: Erase may be needed before write to the flash part.
  PCH_SPI_FLASH_ERASE               FlashErase;         ///< Erase some area on the flash part.
  PCH_SPI_FLASH_READ_SFDP           FlashReadSfdp;      ///< Read SFDP data from the flash part.
  PCH_SPI_FLASH_READ_JEDEC_ID       FlashReadJedecId;   ///< Read Jedec Id from the flash part.
  PCH_SPI_FLASH_WRITE_STATUS        FlashWriteStatus;   ///< Write the status register in the flash part.
  PCH_SPI_FLASH_READ_STATUS         FlashReadStatus;    ///< Read status register in the flash part.
  PCH_SPI_GET_REGION_ADDRESS        GetRegionAddress;   ///< Get the SPI region base and size
  PCH_SPI_READ_PCH_SOFTSTRAP        ReadPchSoftStrap;   ///< Read PCH Soft Strap Values
  PCH_SPI_READ_CPU_SOFTSTRAP        ReadCpuSoftStrap;   ///< Read CPU Soft Strap Values
};

/**
  PCH SPI PPI/PROTOCOL revision number

  Revision 1:   Initial version
  Revision 2:   Identify regions by GUID

**/
#define PCH_SPI_SERVICES_REVISION       2

#endif
