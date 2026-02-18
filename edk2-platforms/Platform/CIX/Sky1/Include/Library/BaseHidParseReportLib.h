#/** @file BaseHidParseReportLib.h
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/
#include <IndustryStandard/Usb.h>

#ifndef _BASE_PARSE_REPORT_LIB_H
#define _BASE_PARSE_REPORT_LIB_H

// ----------------------------------------------------------------------------
// HID usage pages
// ----------------------------------------------------------------------------
#define HID_UP_GENDESK    0x01
#define HID_UP_KEYBOARD   0x07
#define HID_UP_LED        0x08
#define HID_UP_BUTTON     0x09
#define HID_UP_CONSUMER   0x0C
#define HID_UP_DIGITIZER  0x0D
#define HID_UP_FIDO       0xF1D0                   // Fast IDentity Online Alliance

#define HID_GENDESK_POINTER   0x01
#define HID_GENDESK_MOUSE     0x02
#define HID_GENDESK_KEYBOARD  0x06
#define HID_GENDESK_KEYPAD    0x07
#define HID_GENDESK_X         0x30
#define HID_GENDESK_Y         0x31
#define HID_GENDESK_Z         0x32
#define HID_GENDESK_WHEEL     0x38

#define HID_KEYBOARD_CAPS_LOCK    0x39
#define HID_KEYBOARD_SYS_REQ_KEY  0x46
#define HID_KEYBOARD_SCROLL_LOCK  0x47
#define HID_KEYBOARD_NUM_LOCK     0x53
#define HID_KEYBOARD_MEMU_KEY     0x65
#define HID_KEYBOARD_LEFT_CTRL    0xE0
#define HID_KEYBOARD_LEFT_SHIFT   0xE1
#define HID_KEYBOARD_LEFT_ALT     0xE2
#define HID_KEYBOARD_LEFT_GUI     0xE3
#define HID_KEYBOARD_RIGHT_CTRL   0xE4
#define HID_KEYBOARD_RIGHT_SHIFT  0xE5
#define HID_KEYBOARD_RIGHT_ALT    0xE6
#define HID_KEYBOARD_RIGHT_GUI    0xE7

#define HID_LED_NUM_LOCK     0x01
#define HID_LED_CAP_LOCK     0x02
#define HID_LED_SCROLL_LOCK  0x03

#define HID_BUTTON_BUTTON1  0x01

#define HID_DIGITIZER_DIGITIZER           0x01
#define HID_DIGITIZER_PEN                 0x02
#define HID_DIGITIZER_LIGHT_PEN           0x03
#define HID_DIGITIZER_TOUCH_SCREEN        0x04
#define HID_DIGITIZER_TOUCH_PAD           0x05
#define HID_DIGITIZER_MULTIPLE_POINT      0x0C
#define HID_DIGITIZER_CONFIGURATION       0x0E
#define HID_DIGITIZER_FINGER              0x22
#define HID_DIGITIZER_DEV_SETTINGS        0x23
#define HID_DIGITIZER_TIP_PRESSURE        0x30
#define HID_DIGITIZER_IN_RANGE            0x32
#define HID_DIGITIZER_TIP_SWITCH          0x42
#define HID_DIGITIZER_TIP_SWITCH2         0x43
#define HID_DIGITIZER_BARREL_SWITCH       0x44
#define HID_DIGITIZER_CONFIDENCE          0x47
#define HID_DIGITIZER_WIDTH               0x48
#define HID_DIGITIZER_HEIGHT              0x49
#define HID_DIGITIZER_CONTACT_IDENTIFIER  0x51
#define HID_DIGITIZER_DEVICE_MODE         0x52
#define HID_DIGITIZER_DEVICE_IDENTIFIER   0x53
#define HID_DIGITIZER_CONTACT_COUNT       0x54
#define HID_DIGITIZER_CONTACT_COUNT_MAX   0x55
#define HID_DIGITIZER_SCAN_TIME           0x56
#define HID_DIGITIZER_SURFACE_SWITCH      0x57
#define HID_DIGITIZER_BUTTON_SWITCH       0x58
#define HID_DIGITIZER_BUTTONTYPE          0x59
#define HID_DIGITIZER_BARRELSWITCH2       0x5A
#define HID_DIGITIZER_TOOLSERIALNUMBER    0x5B

#define HID_DG_DEV_MODE_MOUSE         0x00
#define HID_DG_DEV_MODE_SINGLE_INPUT  0x01
#define HID_DG_DEV_MODE_MULTI_INPUT   0x02

#define HID_FIDO_U2F_AD              0x01           // U2F Authenticator Device
#define HID_FIDO_INPUT_REPORT_DATA   0x20
#define HID_FIDO_OUTPUT_REPORT_DATA  0x21

// ----------------------------------------------------------------------------
// HID Report define start
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// HID USAGE Macro define
// ----------------------------------------------------------------------------
#define GET_USAGE_PAGE(U)   (U >> 16)
#define GET_USAGE_ID(U)     (U & 0xFFFF)
#define SET_USAGE(UP, UID)  ((UP << 16) | (UID))

#define HID_REPORT_LIST_FOR_EACH_SAFE(Entry, NextEntry, ListHead) \
    for(Entry = (ListHead)->ForwardLink, NextEntry = Entry->ForwardLink; \
            Entry != (ListHead); \
            Entry = NextEntry, NextEntry = Entry->ForwardLink \
            )

typedef enum {
  HID_DEV_TYPE_KEYBOARD = 0,
  HID_DEV_TYPE_MOUSE,
  HID_DEV_TYPE_POINT,
  HID_DEV_TYPE_UNSUPPORTED
} HID_DEV_TYPE_ENUM;

typedef enum {
  INPUT_REPORT = 0,
  OUTPUT_REPORT,
  FEATURE_REPORT,
  HID_REPORT_TYPES_MAX
} HID_REPORT_TYPE_ENUM;

typedef union {
  UINT8     Uint8;
  UINT16    Uint16;
  UINT32    Uint32;
  INT8      Int8;
  INT16     Int16;
  INT32     Int32;
  UINT8     *LongData;
} HID_DATA;

typedef struct {
  UINT16      Format;
  UINT8       Size;
  UINT8       Type;
  UINT8       Tag;
  HID_DATA    Data;
} HID_ITEM;

typedef struct {
  UINT16    UsagePage;
  INT32     LogicMin;
  INT32     LogicMax;
  INT32     PhysicalMin;
  INT32     PhysicalMax;
  UINT32    UnitExponent;
  UINT16    Unit;
  UINT8     ReportId;
  UINT16    ReportSize;
  UINT16    ReportCount;
} HID_GLOBAL;

//
// Max Usage count currently define in usage page 0x0C (Consumer page)
//
#define HID_MAX_USAGES  0x29D
typedef struct {
  UINT32    Usage[HID_MAX_USAGES];
  UINT16    UsageIndex;
  UINT32    UsageMin;
  UINT32    UsageMax;

  UINT32    DesignatorIndex;
  UINT32    DesignatorMin;
  UINT32    DesignatorMax;

  UINT32    StringIndex;
  UINT32    StringMin;
  UINT32    StringMax;
  UINT32    Delimiter;
} HID_LOCAL;

typedef struct {
  UINT16    Type;
  UINT32    Usage;
} HID_COLLECTION;

typedef struct {
  UINTN                   FieldIdx;
  UINT8                   ReportFlag;
  HID_REPORT_TYPE_ENUM    ReportType;
  UINT32                  ReportOffset;
  UINT16                  ReportSize;
  UINT16                  ReportCount;

  INT32                   LogicMin;
  INT32                   LogicMax;
  INT32                   PhysicalMin;
  INT32                   PhysicalMax;
  UINT32                  UnitExponent;
  UINT16                  Unit;

  UINT16                  MaxUsage;
  UINT32                  *Usage;
} HID_REPORT_FIELD;

#define HID_MAX_FIELDS      256
#define HID_MAX_REPORT_IDS  256

typedef struct {
  UINT32                  Signature;
  LIST_ENTRY              List;
  UINT32                  TopLvlColUsage;
  HID_DEV_TYPE_ENUM       HidDevType;
  UINT8                   ReportId;
  HID_REPORT_TYPE_ENUM    ReportType;
  UINT32                  ReportTotalSize;
  UINT16                  FieldCount;
  HID_REPORT_FIELD        *Fields[HID_MAX_FIELDS];
} HID_REPORT;

#define HID_REPORT_SIGNATURE  SIGNATURE_32('h','r','p','t')
#define HID_REPORT_FROM_LIST_NODE(a) \
    CR(a, HID_REPORT, List, HID_REPORT_SIGNATURE)

typedef struct {
  LIST_ENTRY    ReportList[HID_REPORT_TYPES_MAX];
  BOOLEAN       ReportNumbered[HID_REPORT_TYPES_MAX];
} HID_REPORT_LIST;

#define HID_GLOBAL_STACK_SIZE      8
#define HID_COLLECTION_STACK_SIZE  8
typedef struct {
  HID_GLOBAL         Global;
  HID_GLOBAL         GlobalStack[HID_GLOBAL_STACK_SIZE];
  UINT32             GlobalStackPtr;

  HID_LOCAL          Local;
  HID_COLLECTION     CollectionStack[HID_COLLECTION_STACK_SIZE];
  UINT32             CollectionStackPtr;

  HID_REPORT_LIST    *HidReportList;
} HID_PARSER;

typedef struct {
  CONST   UINT32       Usage;
  HID_DEV_TYPE_ENUM    HidDevType;
} HID_DEVICE_USAGE_TABLE;

/**
    Parse the report descriptor
    @param  ReportDescriptor        Pointer to hid report descriptor
    @param  ReportLength            Length of report descriptor
    @param  HidReportEnum           Enumerate the Hid report

    @retval EFI_SUCCESS             HID parser success.
    @retval EFI_INVALID_PARAMETER   Failed to parser HID report.
    @retval EFI_OUT_OF_RESOURCES    Failed to allocate HID_PARSER instance.

**/
EFI_STATUS
EFIAPI
HidParseReportDescriptor (
  IN        UINT8            *ReportDescriptor,
  IN        UINTN            ReportLength,
  IN OUT    HID_REPORT_LIST  *HidReportList
  );

/**

    Get the data offset for this usage in the specified report field description.

    @param[in]  Field      To prepare query report field.
    @param[in]  Usage      To query usage data.

    @retval EFI_SUCCESS    Success

**/
UINTN
EFIAPI
HidGetUsageOffset (
  IN      HID_REPORT_FIELD  *Field,
  IN      UINT32            Usage
  );

/**

    Get the entire hid report filed based on the specified usage id.

    @param[in]  HidReport  The instance of HID_REPORT, that contains many report fileds.
    @param[in]  Usage      To query usage data.

    @retval HID_REPORT_FIELD*  A pointer to points HID_REPORT_FIELD instance.
    @retval NULL               Can not find match report field.

**/
HID_REPORT_FIELD *
EFIAPI
HidGetReportFieldByUsage (
  IN      HID_REPORT  *HidReport,
  IN      UINT32      Usage
  );

/**

    Get report field value is based on the report field description.

    @param[in]  ReportData       Data payload
    @param[in]  BitOffset        The data offset in bit
    @param[in]  BitLength        The data length

    @retval UINTN The value of HID report field.


**/
UINTN
EFIAPI
HidGetReportFieldData (
  IN      UINT8  *ReportData,
  IN      UINTN  BitOffset,
  IN      UINTN  BitLength
  );

/**

    Get entire report from report list by search report id and search type.

    @param[in]  ReportData       Data payload
    @param[in]  BitOffset        The data offset in bit
    @param[in]  BitLength        The data length

    @retval UINTN The value of HID report field.


**/
HID_REPORT *
EFIAPI
HidGetReportById (
  IN      HID_REPORT_LIST       *HidReportList,
  IN      UINT8                 ReportId,
  IN      HID_REPORT_TYPE_ENUM  ReportType
  );

/**

    Set report field value is based on the report field description.

    @param[in]  ReportData       Data payload
    @param[in]  BitOffset        The data offset in bit
    @param[in]  BitLength        The data length
    @param[in]  NewValue         To assign value to corresponding report field

    @retval VOID


**/
VOID
EFIAPI
HidSetReportFieldData (
  IN      UINT8   *ReportData,
  IN      UINT32  BitOffset,
  IN      UINT32  BitLength,
  IN      UINTN   NewValue
  );

/**
    Free the resource allocated by HID_REPORT_LIST instance.

    @param[in] ReportEnum  A pointer to points HID_REPORT_LIST instance.

    @retval VOID

**/
VOID
HidReportFreeResource (
  IN       HID_REPORT_LIST  *ReportList
  );

/**
    Dump the hid report field information.

    @param[in] Field  A pointer to points HID_REPORT_FIELD instance.

    @retval VOID

**/
VOID
HidReportFieldDump (
  IN      HID_REPORT_FIELD  *Field
  );

/**
    Dump the list of entire hid report information.

    @param[in] ReportEnum  A pointer to points HID_REPORT_LIST instance.

    @retval VOID

**/
VOID
HidReportListDump (
  IN      HID_REPORT_LIST       *HidReportList,
  IN      HID_REPORT_TYPE_ENUM  ReportType
  );

/**
    Dump the entire hid report information.

    @param[in] ReportEnum  A pointer to points HID_REPORT_LIST instance.

    @retval VOID

**/
VOID
EFIAPI
HidReportDump (
  IN      HID_REPORT  *HidReport
  );

#endif
