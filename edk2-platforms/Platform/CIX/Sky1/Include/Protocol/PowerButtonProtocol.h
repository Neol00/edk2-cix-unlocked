#ifndef _POWER_BUTTON_PROTOCOL_H_
#define _POWER_BUTTON_PROTOCOL_H_

typedef
  EFI_STATUS
(EFIAPI *ENABLE_POWER_BUTTON_DETECT)(
  void
  );

typedef
  VOID
(EFIAPI *DISABLE_POWER_BUTTON_DETECT)(
  void
  );

typedef struct  {
  ENABLE_POWER_BUTTON_DETECT     EnablePowerButtonDetect;
  DISABLE_POWER_BUTTON_DETECT    DisablePowerButtonDetect;
} CIX_POWER_BUTTON_PROTOCOL;

#endif
