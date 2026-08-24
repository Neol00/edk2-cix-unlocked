/** @file
  Boot-time sweeper for Linux efi-pstore crash dump variables.

  On every kernel crash Linux dumps the dmesg buffer into UEFI variables
  named "dump-type*" under LINUX_EFI_CRASH_GUID. If the OS never drains
  them (systemd-pstore.service disabled or missing), repeated crashes
  fill the SPI NOR variable store until SetVariable starts failing with
  EFI_OUT_OF_RESOURCES and boot slows down.

  When free variable-store space drops below a threshold, delete the
  oldest crash dumps until enough space is recovered. Only variables
  under the Linux crash vendor GUID are ever touched, so nothing the OS
  or firmware relies on long-term can be lost.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PlatformBm.h"

//
// Sweep when free space in the common variable store falls below this
// percentage, and keep deleting dumps until it is back above it.
//
#define PSTORE_SWEEP_MIN_FREE_PERCENT  50

//
// LINUX_EFI_CRASH_GUID, the vendor GUID Linux's efi-pstore uses for its
// "dump-type*" crash dump variables (include/linux/efi.h).
//
STATIC CONST EFI_GUID  mLinuxEfiCrashGuid = {
  0xcbb219d7, 0x3a05, 0x4e51, { 0x93, 0x75, 0x3c, 0x8b, 0xa6, 0x4e, 0xae, 0xea }
};

STATIC CONST CHAR16  mDumpNamePrefix[] = L"dump-type";

typedef struct {
  CHAR16    *Name;
  UINT64    Timestamp;
} PSTORE_DUMP_ENTRY;

/**
  Return the free space in the common variable store as a percentage,
  or 100 if QueryVariableInfo is unavailable (nothing to sweep then).
**/
STATIC
UINTN
GetVariableStoreFreePercent (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT64      MaximumVariableStorageSize;
  UINT64      RemainingVariableStorageSize;
  UINT64      MaximumVariableSize;

  Status = gRT->QueryVariableInfo (
                  EFI_VARIABLE_NON_VOLATILE |
                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                  EFI_VARIABLE_RUNTIME_ACCESS,
                  &MaximumVariableStorageSize,
                  &RemainingVariableStorageSize,
                  &MaximumVariableSize
                  );
  if (EFI_ERROR (Status) || (MaximumVariableStorageSize == 0)) {
    return 100;
  }

  return (UINTN)DivU64x64Remainder (
                  MultU64x32 (RemainingVariableStorageSize, 100),
                  MaximumVariableStorageSize,
                  NULL
                  );
}

/**
  Extract the pstore timestamp from a dump variable name.

  efi-pstore names records "dump-type<type>-<part>[-<count>]-<timestamp>-<C|D>",
  so the timestamp is the second-to-last '-'-separated field. Returns 0 if
  the name does not parse, which sorts the record as oldest.
**/
STATIC
UINT64
DumpNameToTimestamp (
  IN CONST CHAR16  *Name
  )
{
  CONST CHAR16  *Pos;
  CONST CHAR16  *LastField;
  CONST CHAR16  *SecondLastField;

  LastField       = NULL;
  SecondLastField = NULL;
  for (Pos = Name; *Pos != L'\0'; Pos++) {
    if (*Pos == L'-') {
      SecondLastField = LastField;
      LastField       = Pos + 1;
    }
  }

  if (SecondLastField == NULL) {
    return 0;
  }

  return StrDecimalToUint64 (SecondLastField);
}

/**
  Collect the names of all Linux crash dump variables.

  @param[out] Entries  Allocated array of dump entries; caller frees the
                       array and each Name. NULL if none found.

  @return Number of entries collected.
**/
STATIC
UINTN
CollectDumpVariables (
  OUT PSTORE_DUMP_ENTRY  **Entries
  )
{
  EFI_STATUS         Status;
  CHAR16             *Name;
  UINTN              NameBufferSize;
  UINTN              NameSize;
  EFI_GUID           Guid;
  PSTORE_DUMP_ENTRY  *List;
  UINTN              Count;
  UINTN              Capacity;

  *Entries = NULL;

  NameBufferSize = 64 * sizeof (CHAR16);
  Name           = AllocateZeroPool (NameBufferSize);
  if (Name == NULL) {
    return 0;
  }

  List     = NULL;
  Count    = 0;
  Capacity = 0;

  for ( ; ;) {
    NameSize = NameBufferSize;
    Status   = gRT->GetNextVariableName (&NameSize, Name, &Guid);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Name = ReallocatePool (NameBufferSize, NameSize, Name);
      if (Name == NULL) {
        break;
      }

      NameBufferSize = NameSize;
      NameSize       = NameBufferSize;
      Status         = gRT->GetNextVariableName (&NameSize, Name, &Guid);
    }

    if (EFI_ERROR (Status)) {
      break;
    }

    if (!CompareGuid (&Guid, &mLinuxEfiCrashGuid) ||
        (StrnCmp (Name, mDumpNamePrefix, ARRAY_SIZE (mDumpNamePrefix) - 1) != 0))
    {
      continue;
    }

    if (Count == Capacity) {
      Capacity = (Capacity == 0) ? 32 : Capacity * 2;
      List     = ReallocatePool (
                   Count * sizeof (PSTORE_DUMP_ENTRY),
                   Capacity * sizeof (PSTORE_DUMP_ENTRY),
                   List
                   );
      if (List == NULL) {
        Count = 0;
        break;
      }
    }

    List[Count].Name = AllocateCopyPool (StrSize (Name), Name);
    if (List[Count].Name == NULL) {
      break;
    }

    List[Count].Timestamp = DumpNameToTimestamp (Name);
    Count++;
  }

  if (Name != NULL) {
    FreePool (Name);
  }

  *Entries = List;
  return Count;
}

/**
  Sort dump entries oldest-first by pstore timestamp (insertion sort;
  the list is at most a few hundred entries).
**/
STATIC
VOID
SortDumpsOldestFirst (
  IN OUT PSTORE_DUMP_ENTRY  *Entries,
  IN     UINTN              Count
  )
{
  UINTN              Index;
  UINTN              Prev;
  PSTORE_DUMP_ENTRY  Key;

  for (Index = 1; Index < Count; Index++) {
    Key  = Entries[Index];
    Prev = Index;
    while ((Prev > 0) && (Entries[Prev - 1].Timestamp > Key.Timestamp)) {
      Entries[Prev] = Entries[Prev - 1];
      Prev--;
    }

    Entries[Prev] = Key;
  }
}

/**
  If the variable store is running low on space, delete Linux efi-pstore
  crash dump variables, oldest first, until enough space is free again.
**/
VOID
SweepLinuxCrashDumpVariables (
  VOID
  )
{
  PSTORE_DUMP_ENTRY  *Dumps;
  UINTN              Count;
  UINTN              Index;
  UINTN              Deleted;
  UINTN              FreePercent;
  EFI_STATUS         Status;

  FreePercent = GetVariableStoreFreePercent ();
  if (FreePercent >= PSTORE_SWEEP_MIN_FREE_PERCENT) {
    return;
  }

  DEBUG ((
    DEBUG_WARN,
    "%a: variable store %u%% free (< %u%%), sweeping Linux crash dumps\n",
    __func__,
    FreePercent,
    PSTORE_SWEEP_MIN_FREE_PERCENT
    ));

  Count = CollectDumpVariables (&Dumps);
  if (Count == 0) {
    DEBUG ((DEBUG_WARN, "%a: store is low but holds no crash dumps\n", __func__));
    return;
  }

  SortDumpsOldestFirst (Dumps, Count);

  Deleted = 0;
  for (Index = 0; Index < Count; Index++) {
    Status = gRT->SetVariable (
                    Dumps[Index].Name,
                    (EFI_GUID *)&mLinuxEfiCrashGuid,
                    0,
                    0,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_WARN,
        "%a: failed to delete %s: %r\n",
        __func__,
        Dumps[Index].Name,
        Status
        ));
      continue;
    }

    Deleted++;
    if (GetVariableStoreFreePercent () >= PSTORE_SWEEP_MIN_FREE_PERCENT) {
      break;
    }
  }

  DEBUG ((
    DEBUG_WARN,
    "%a: deleted %u of %u crash dump variables, store now %u%% free\n",
    __func__,
    Deleted,
    Count,
    GetVariableStoreFreePercent ()
    ));

  for (Index = 0; Index < Count; Index++) {
    FreePool (Dumps[Index].Name);
  }

  FreePool (Dumps);
}
