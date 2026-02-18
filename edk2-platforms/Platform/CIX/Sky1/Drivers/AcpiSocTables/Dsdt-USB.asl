/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (XHC0)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x00)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_DRD_ENABLE_OFFSET) && (\_SB.GETV(ARV_USB3_TYPEC_DRD_ROLE_OFFSET)==0)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEC_DRD_CONTROLLER_BASE, USB_XHCI_TYPEC_DRD_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEC_U3_TYPEC_DRD_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (XHC1)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x01)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST0_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEC_HOST0_CONTROLLER_BASE, USB_XHCI_TYPEC_HOST0_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEC_HOST0_U3_TYPEC_HOST0_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (XHC2)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x02)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST1_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEC_HOST1_CONTROLLER_BASE, USB_XHCI_TYPEC_HOST1_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEC_HOST1_U3_TYPEC_HOST1_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (XHC3)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x03)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST2_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEC_HOST2_CONTROLLER_BASE, USB_XHCI_TYPEC_HOST2_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEC_HOST2_U3_TYPEC_HOST2_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}


Device (XHC4)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x04)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEA_DRD0_ENABLE_OFFSET) && (\_SB.GETV(ARV_USB3_TYPEA_DRD0_ROLE_OFFSET)==0)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEA_DRD0_CONTROLLER_BASE, USB_XHCI_TYPEA_DRD0_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEA_CTL0_U3_TYPEA_CTL0_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (XHC5)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x05)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB3_TYPEA_DRD1_ENABLE_OFFSET) && (\_SB.GETV(ARV_USB3_TYPEA_DRD1_ROLE_OFFSET)==0)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_XHCI_TYPEA_DRD1_CONTROLLER_BASE, USB_XHCI_TYPEA_DRD1_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB3_TYPEA_CTL1_U3_TYPEA_CTL1_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (USB0)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x06)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB2_HOST0_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_EHCI_HOST0_CONTROLLER_BASE, USB_EHCI_HOST0_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB2_HOST0_U2_HOST0_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (USB1)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x07)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB2_HOST1_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_EHCI_HOST1_CONTROLLER_BASE, USB_EHCI_HOST1_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB2_HOST1_U2_HOST1_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (USB2)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x08)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB2_HOST2_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_EHCI_HOST2_CONTROLLER_BASE, USB_EHCI_HOST2_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB2_HOST2_U2_HOST2_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}

Device (USB3)
{
  Name (_HID, "PNP0D10")
  Name (_UID, 0x09)
  Name (_CCA, 0x00)

  Method (_STA)
  {
    If(\_SB.GETV(ARV_USB2_HOST3_ENABLE_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Method(_CRS, 0x0, Serialized) {
    Name(RBUF, ResourceTemplate() {
      Memory32Fixed(ReadWrite, USB_EHCI_HOST3_CONTROLLER_BASE, USB_EHCI_HOST3_CONTROLLER_SIZE)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) {USB2_HOST3_U2_HOST3_IRQ_INTERRUPT_ID}
      })
    Return(RBUF)
  }
}
