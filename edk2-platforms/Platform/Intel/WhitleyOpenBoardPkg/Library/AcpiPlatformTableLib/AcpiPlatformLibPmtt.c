/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 2016 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "AcpiPlatformLibLocal.h"
#include <Protocol/SmbiosMemInfo.h>

extern struct SystemMemoryMapHob   *mSystemMemoryMap;

/******************************************************************************
 * Definitions.
 ******************************************************************************/
//#define PMTTDEBUG_ENABLED 1
#if PMTTDEBUG_ENABLED
#define PMTTDEBUG(Expr) _DEBUG(Expr)
#else
#define PMTTDEBUG(Expr)
#endif
#ifndef NELEMENTS
#define NELEMENTS(Array) (sizeof(Array)/sizeof((Array)[0]))
#endif


//
// PMTT GUID variables
//
const EFI_GUID  gEfiPmttTypeDieGuid = PMTT_TYPE_DIE_GUID;
const EFI_GUID  gEfiPmttTypeChannelGuid = PMTT_TYPE_CHANNEL_GUID;
const EFI_GUID  gEfiPmttTypeSlotGuid = PMTT_TYPE_SLOT_GUID;

/******************************************************************************
 * Functions.
 ******************************************************************************/

EFI_STATUS
PatchPlatformMemoryTopologyTable (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table
  )
{
  UINT8                                Socket;
  UINT8                                Die;
  UINT8                                Imc;
  UINT8                                Channel;
  UINT8                                ChannelIndex;
  UINT8                                Dimm;
  SMBIOS_DIMM_INFO                     DimmInfo;
  SMBIOS_MEM_INFO_PROTOCOL             *SmbiosInfoProtocol;
  EFI_STATUS                           Status;
  ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE  *PmttTable = (ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE*)Table;
  UINT8                                MaxImc;
  UINT8                                MaxChPerImc;
  UINT8                                DieCnt;

  DYNAMIC_SI_LIBARY_PROTOCOL2         *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  MaxImc = DynamicSiLibraryProtocol2->GetMaxImc ();
  MaxChPerImc = DynamicSiLibraryProtocol2->GetNumChannelPerMc ();

  ASSERT (PmttTable->Header.Signature == ACPI_PMTT_TABLE_SIGNATURE);

  Status = gBS->LocateProtocol (&gSmbiosMemInfoProtocolGuid, NULL, (VOID**) &SmbiosInfoProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ACPI] (PMTT) Cannot locate SmbiosMemInfoProtocol! (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    SmbiosInfoProtocol = NULL;
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

    PmttTable->Socket[Socket].Type      = ACPI_TOP_LEVEL_SOCKET;
    PmttTable->Socket[Socket].SckIdent  = Socket;
    PmttTable->Socket[Socket].Length    = sizeof(PmttTable->Socket[Socket]) - sizeof(ACPI_PMTT_DIE_DEVICE);
    PmttTable->Socket[Socket].Flag      = 0;
    PmttTable->Socket[Socket].NumOfMemoryDevices = 0;

    if (mSystemMemoryMap->Socket[Socket].SocketEnabled) {
      PmttTable->Socket[Socket].Flag |= (PMTT_TOP_LEVEL_AGGREGATOR_DEVICE | PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY);
      PmttTable->NumOfMemoryDevices++;
    } else {
      continue;
    }

    DieCnt =   DynamicSiLibraryProtocol2->GetAcpiDieCount (Socket);
    for (Die = 0; Die < DieCnt; Die++) {

      PmttTable->Socket[Socket].Die[Die].Type = ACPI_TOP_LEVEL_VENDOR_SPECIFIC_DEVICE;
      PmttTable->Socket[Socket].Die[Die].Length = sizeof(PmttTable->Socket[Socket].Die[Die]) - MAX_IMC * sizeof(ACPI_PMTT_IMC_DEVICE);
      PmttTable->Socket[Socket].Die[Die].Flag = 0;
      PmttTable->Socket[Socket].Die[Die].Flag |= PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY;
      PmttTable->Socket[Socket].Die[Die].NumOfMemoryDevices = 0;
      PmttTable->Socket[Socket].Die[Die].DieId = Die;
      CopyGuid (&PmttTable->Socket[Socket].Die[Die].TypeUuid, &gEfiPmttTypeDieGuid);

      PmttTable->Socket[Socket].NumOfMemoryDevices++;

      for (Imc = 0; Imc < MaxImc; Imc++) {

        PmttTable->Socket[Socket].Die[Die].Imc[Imc].Type = ACPI_TOP_LEVEL_IMC;
        PmttTable->Socket[Socket].Die[Die].Imc[Imc].Length = sizeof(PmttTable->Socket[Socket].Die[Die].Imc[Imc]) - MAX_MC_CH * sizeof(ACPI_PMTT_CHANNEL_DEVICE);
        PmttTable->Socket[Socket].Die[Die].Imc[Imc].Flag = 0;
        PmttTable->Socket[Socket].Die[Die].Imc[Imc].NumOfMemoryDevices = 0;
        PmttTable->Socket[Socket].Die[Die].Imc[Imc].ImcId = Imc;

        if (mSystemMemoryMap->Socket[Socket].imcEnabled[Imc]) {
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Flag |= PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY;
          PmttTable->Socket[Socket].Die[Die].NumOfMemoryDevices++;
        } else {
          continue;
        }

        for (Channel = 0; Channel < MaxChPerImc; Channel++) {

          ChannelIndex = MEM_IMCCH_TO_SKTCH(Imc, Channel);
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Type = ACPI_TOP_LEVEL_VENDOR_SPECIFIC_DEVICE;
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Length = sizeof(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel]) - MAX_DIMM * sizeof(ACPI_PMTT_SLOT_DEVICE);
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Flag = 0;
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].NumOfMemoryDevices = 0;
          PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].ChannelId = Channel;
          CopyGuid (&(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].TypeUuid), &gEfiPmttTypeChannelGuid);

          if (mSystemMemoryMap->Socket[Socket].ChannelInfo[ChannelIndex].Enabled) {
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Flag |= PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].NumOfMemoryDevices++;
          } else {
            continue;
          }

          for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
            //
            // Looping through the each DIMM on the IMC.
            //
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Type = ACPI_TOP_LEVEL_VENDOR_SPECIFIC_DEVICE;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Length = sizeof(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm]) - sizeof(ACPI_PMTT_DIMM_DEVICE);
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Flag |= PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].NumOfMemoryDevices = 0;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].SlotId = Dimm;
            CopyGuid (&PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].TypeUuid, &gEfiPmttTypeSlotGuid);

            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Type = PHYSICAL_COMPONENT_IDENTIFIER_TYPE_DIMM;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Length = sizeof(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm);
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.NumOfMemoryDevices = 0;

            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Flag = 0;

            if (!mSystemMemoryMap->Socket[Socket].ChannelInfo[ChannelIndex].DimmInfo[Dimm].Present) {
              continue;
            }

            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Flag |= PMTT_PHYSICAL_ELEMENT_OF_TOPOLOGY;

            if (mSystemMemoryMap->Socket[Socket].ChannelInfo[ChannelIndex].DimmInfo[Dimm].DcpmmPresent) {
              PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Flag |= PMTT_AEP_DIMM;
            }
            //
            // Get SMBIOS handle for the DIMM. If handle not found use FFFFFFFFh.
            //
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.SmbiosHandle = (UINT32)-1;
            if (SmbiosInfoProtocol != NULL) {

              DimmInfo.Socket = Socket;
              DimmInfo.Imc = Imc;
              DimmInfo.Channel = Channel;
              DimmInfo.Dimm = Dimm;
              Status = SmbiosInfoProtocol->SmbiosGetDimmByLocation (SmbiosInfoProtocol, &DimmInfo);
              if (!EFI_ERROR (Status)) {
                PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.SmbiosHandle = DimmInfo.Type17Handle;
              }
            }

            if (!mSystemMemoryMap->Socket[Socket].ChannelInfo[ChannelIndex].DimmInfo[Dimm].Enabled) {
              continue;
            }

            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].NumOfMemoryDevices++;
            PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].NumOfMemoryDevices++;
          } // for (Dimm...)
        } // for (Channel...)
      } // for (Imc...)
    } // for (Die...)
  } // for (Skt...)
  //
  // Dump the strucutre for debug purpose
  //
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) Signature:       0x%08X\n", PmttTable->Header.Signature));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) Length:          %d\n", PmttTable->Header.Length));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) Revision:        %d\n", PmttTable->Header.Revision));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) Checksum:         %d\n", PmttTable->Header.Checksum));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) OemId:           '%c%c%c%c%c%c'\n",
              PmttTable->Header.OemId[0], PmttTable->Header.OemId[1], PmttTable->Header.OemId[2],
              PmttTable->Header.OemId[3], PmttTable->Header.OemId[4], PmttTable->Header.OemId[5]));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) OemTableId:      0x%08X\n", PmttTable->Header.OemTableId));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) OemRevision:     %d\n", PmttTable->Header.OemRevision));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) CreatorId:       0x%08X\n", PmttTable->Header.CreatorId));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) CreatorRevision: %d\n", PmttTable->Header.OemRevision));
  PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) NumOfMemoryDevices: %d\n", PmttTable->NumOfMemoryDevices));

  PMTTDEBUG ((DEBUG_INFO, "\n"));
  for (Socket = 0; Socket < NELEMENTS (PmttTable->Socket); Socket++) {

    PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d] Type:        0x%02X\n", Socket, PmttTable->Socket[Socket].Type));
    PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d] Length:      %d\n", Socket, PmttTable->Socket[Socket].Length));
    PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d] Flags:       0x%04X\n", Socket, PmttTable->Socket[Socket].Flag));
    PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d] SocketId:    %d\n", Socket, PmttTable->Socket[Socket].SckIdent));
    PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d] NumOfMemoryDevices:    %d\n", Socket, PmttTable->Socket[Socket].NumOfMemoryDevices));

    PMTTDEBUG ((DEBUG_INFO, "\n"));
    for (Die = 0; Die < NELEMENTS(PmttTable->Socket[Socket].Die); Die++) {

      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] Type:        0x%02X\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].Type));
      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] Length:      %d\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].Length));
      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] Flags:       0x%04X\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].Flag));
      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] DieId:       %d\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].DieId));
      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] TypeUuid:    %g\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].TypeUuid));
      PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d] NumOfMemoryDevices:       %d\n", Socket, Die, PmttTable->Socket[Socket].Die[Die].NumOfMemoryDevices));

      PMTTDEBUG ((DEBUG_INFO, "\n"));
      for (Imc = 0; Imc < NELEMENTS(PmttTable->Socket[Socket].Die[Die].Imc); Imc++) {

        PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d] Type:                   0x%02X\n", Socket, Die, Imc, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Type));
        PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d] Length:                 %d\n", Socket, Die, Imc, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Length));
        PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d] Flags:                  0x%04X\n", Socket, Die, Imc, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Flag));
        PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d] ImcId:                  %d\n", Socket, Die, Imc, PmttTable->Socket[Socket].Die[Die].Imc[Imc].ImcId));
        PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d] NumOfMemoryDevices:     %d\n", Socket, Die, Imc, PmttTable->Socket[Socket].Die[Die].Imc[Imc].NumOfMemoryDevices));

        PMTTDEBUG ((DEBUG_INFO, "\n"));
        for (Channel = 0; Channel < NELEMENTS(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel); Channel++) {

          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] Type:                   0x%02X\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Type));
          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] Length:                 %d\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Length));
          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] Flags:                  0x%04X\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Flag));
          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] ChannelId:              %d\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].ChannelId));
          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] TypeUuid:               %g\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].TypeUuid));
          PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d] NumOfMemoryDevices:     %d\n", Socket, Die, Imc, Channel, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].NumOfMemoryDevices));

          PMTTDEBUG ((DEBUG_INFO, "\n"));
          for (Dimm = 0; Dimm < NELEMENTS(PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot); Dimm++) {

            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] Type:                   0x%02X\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Type));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] Length:                 %d\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Length));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] Flags:                  0x%04X\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Flag));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] SlotId:                 %d\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].SlotId));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] TypeUuid:               %g\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].TypeUuid));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d] NumOfMemoryDevices:     %d\n", Socket, Die, Imc, Channel, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].NumOfMemoryDevices));

            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d][%d] Type:                0x%02X\n", Socket, Die, Imc, Channel, Dimm, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Type));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d][%d] Length:              %d\n", Socket, Die, Imc, Channel, Dimm, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Length));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d][%d] Flags:               0x%04X\n", Socket, Die, Imc, Channel, Dimm, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.Flag));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d][%d] SmbiosHandle:        0x%08X\n", Socket, Die, Imc, Channel, Dimm, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.SmbiosHandle));
            PMTTDEBUG ((DEBUG_INFO, "[ACPI] (PMTT) [%d][%d][%d][%d][%d][%d] NumOfMemoryDevices:  %d\n", Socket, Die, Imc, Channel, Dimm, Dimm, PmttTable->Socket[Socket].Die[Die].Imc[Imc].Channel[Channel].Slot[Dimm].Dimm.NumOfMemoryDevices));
          } // for (Dimm...)
        } // for (Channel...)
      } // for (Imc...)
    } // for (Die...)
  } // for (Skt...)
  return EFI_SUCCESS;
} // PatchPlatformMemoryTopologyTable ()
