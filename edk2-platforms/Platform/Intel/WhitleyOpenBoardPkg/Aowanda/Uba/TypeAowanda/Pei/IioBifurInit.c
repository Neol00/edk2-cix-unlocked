/** @file
  IIO Config Update.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"
#include <Library/UbaIioConfigLib.h>
#include <IioPlatformData.h>

typedef enum {
  Iio_Socket0 = 0,
  Iio_Socket1,
  Iio_Socket2,
  Iio_Socket3,
  Iio_Socket4,
  Iio_Socket5,
  Iio_Socket6,
  Iio_Socket7
} IIO_SOCKETS;

typedef enum {
  Iio_Iou0 = 0,
  Iio_Iou1,
  Iio_Iou2,
  Iio_Iou3,
  Iio_Iou4,
  Iio_IouMax
} IIO_IOUS;

typedef enum {
  VPP_PORT_0 = 0,
  VPP_PORT_1,
  VPP_PORT_2,
  VPP_PORT_3
} VPP_PORT;

#define ENABLE   1
#define DISABLE  0

#define SPLS_1X  0

static IIO_BIFURCATION_DATA_ENTRY_EX  IioBifurcationTable[] =
{
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxxxxx16, VPP_PORT_MAX, SMB_ADDR_MAX, SMB_ADDR_MAX, SMB_DATA_MAX  },
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16, VPP_PORT_MAX, SMB_ADDR_MAX, SMB_ADDR_MAX, SMB_DATA_MAX  },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16, VPP_PORT_MAX, SMB_ADDR_MAX, SMB_ADDR_MAX, SMB_DATA_MAX  },
  { Iio_Socket0, Iio_Iou3, IIO_BIFURCATE_xxxxxx16, VPP_PORT_MAX, SMB_ADDR_MAX, SMB_ADDR_MAX, SMB_DATA_MAX  },
  { Iio_Socket0, Iio_Iou4, IIO_BIFURCATE_xxxxxx16, VPP_PORT_MAX, SMB_ADDR_MAX, SMB_ADDR_MAX, SMB_DATA_MAX  }
};

static IIO_SLOT_CONFIG_DATA_ENTRY_EX  IioSlotTable[] = {
  // Port Index  | Slot       |Interlock |power       |Power        |Hotplug  |Vpp Port      |Vpp Addr      |PCIeSSD  |PCIeSSD       |PCIeSSD       |Hidden    |Common   |  SRIS   |Uplink   |Retimer  |Retimer       |Retimer       |Retimer    |Mux           |Mux           |ExtnCard |ExtnCard      |ExtnCard      |ExtnCard |ExtnCard Retimer|ExtnCard Retimer|ExtnCard |ExtnCard Hotplug|ExtnCard Hotplug|Max Retimer|
  //             |            |          |Limit Scale |Limit Value  |Cap      |              |              |Cap      |Port          |Address       |          |Clock    |         |Port     |         |Address       |Channel       |Width      |Address       |Channel       |Support  |SMBus Port    |SMBus Addr    |Retimer  |SMBus Address   |Width           |Hotplug  |Vpp Port        |Vpp Address     |           |
  {SOCKET_0_INDEX +
    PORT_1A_INDEX, 1          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_1B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_1C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

   {SOCKET_0_INDEX +
    PORT_1D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_2A_INDEX, 2          , DISABLE ,     SPLS_1X ,          25 , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_2B_INDEX, NO_SLT_IMP , DISABLE ,     SPLS_1X ,          25 , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_2C_INDEX, NO_SLT_IMP , DISABLE ,     SPLS_1X ,          25 , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_2D_INDEX, NO_SLT_IMP , DISABLE ,     SPLS_1X ,          25 , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_3A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_3B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_3C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
   {SOCKET_0_INDEX +
    PORT_3D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_4A_INDEX, 0x30       , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_4B_INDEX, 0x31       , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x40         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_4C_INDEX, 0x32       , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x42         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_4D_INDEX, 0x33       , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x42         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , DISABLE , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },

  {SOCKET_0_INDEX +
    PORT_5A_INDEX, 4          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , ENABLE  , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_5B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , ENABLE  , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_5C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , ENABLE  , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      },
  {SOCKET_0_INDEX +
    PORT_5D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE , ENABLE  , DISABLE , DISABLE , DISABLE , SMB_ADDR_MAX , SMB_DATA_MAX , NOT_EXIST , SMB_ADDR_MAX , SMB_DATA_MAX , ENABLE  , VPP_PORT_MAX , SMB_ADDR_MAX , DISABLE , SMB_ADDR_MAX   , NOT_EXIST      , DISABLE , VPP_PORT_MAX   , SMB_ADDR_MAX   , 0x0      }

};

EFI_STATUS
UpdateAowandaIioConfig (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
  return EFI_SUCCESS;
}

PLATFORM_IIO_CONFIG_UPDATE_TABLE_EX  TypeAowandaIioConfigTable =
{
  PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE,
  PLATFORM_IIO_CONFIG_UPDATE_VERSION_2,

  IioBifurcationTable,
  sizeof (IioBifurcationTable),
  UpdateAowandaIioConfig,
  IioSlotTable,
  sizeof (IioSlotTable)
};

/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
TypeAowandaIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  )
{
  EFI_STATUS  Status;

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformIioConfigDataGuid,
                                  &TypeAowandaIioConfigTable,
                                  sizeof (TypeAowandaIioConfigTable)
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformIioConfigDataGuid_1,
                                  &TypeAowandaIioConfigTable,
                                  sizeof (TypeAowandaIioConfigTable)
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformIioConfigDataGuid_2,
                                  &TypeAowandaIioConfigTable,
                                  sizeof (TypeAowandaIioConfigTable)
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformIioConfigDataGuid_3,
                                  &TypeAowandaIioConfigTable,
                                  sizeof (TypeAowandaIioConfigTable)
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
