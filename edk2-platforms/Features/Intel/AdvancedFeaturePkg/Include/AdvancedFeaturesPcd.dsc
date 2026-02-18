## @file
#  DSC file for defining Pcd of advanced features.
#
#  This file is intended to be included into another package so advanced features
#  can be conditionally built by enabling the respective feature via its FeaturePCD.
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# The section references the package DEC files,
# it allow a FeaturePCD to be used in a conditional statement
#
[Packages]
  MdePkg/MdePkg.dec

  #
  # Debugging features
  #
  AcpiDebugFeaturePkg/AcpiDebugFeaturePkg.dec
  BeepDebugFeaturePkg/BeepDebugFeaturePkg.dec
  PostCodeDebugFeaturePkg/PostCodeDebugFeaturePkg.dec
  Usb3DebugFeaturePkg/Usb3DebugFeaturePkg.dec

  #
  # Networking features
  #
  NetworkFeaturePkg/NetworkFeaturePkg.dec

  #
  # OutOfBandManagement features
  #
  IpmiFeaturePkg/IpmiFeaturePkg.dec
  SpcrFeaturePkg/SpcrFeaturePkg.dec
  AsfFeaturePkg/AsfFeaturePkg.dec

  #
  # PowerManagement features
  #
  S3FeaturePkg/S3FeaturePkg.dec

  #
  # SystemInformation features
  #
  SmbiosFeaturePkg/SmbiosFeaturePkg.dec

  #
  # UserInterface features
  #
  LogoFeaturePkg/LogoFeaturePkg.dec
  UserAuthFeaturePkg/UserAuthFeaturePkg.dec
  VirtualKeyboardFeaturePkg/VirtualKeyboardFeaturePkg.dec

  #
  # Individual features
  #
  PlatformPayloadFeaturePkg/PlatformPayloadFeaturePkg.dec

#
# The section below sets all PCDs to FALSE in this DSC file so the feature is not enabled by default.
# Board can set PCDs to TRUE in its DSC file to enable a subset of advanced features
#
[PcdsFeatureFlag]
  gAcpiDebugFeaturePkgTokenSpaceGuid.PcdAcpiDebugFeatureEnable              |FALSE
  gAcpiDebugFeaturePkgTokenSpaceGuid.PcdUseSmmVersion                       |FALSE
  gBeepDebugFeaturePkgTokenSpaceGuid.PcdBeepDebugFeatureEnable              |FALSE
  gPostCodeDebugFeaturePkgTokenSpaceGuid.PcdPostCodeDebugFeatureEnable      |FALSE
  gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugFeatureEnable              |FALSE

  gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable                  |FALSE

  gIpmiFeaturePkgTokenSpaceGuid.PcdIpmiFeatureEnable                        |FALSE
  gSpcrFeaturePkgTokenSpaceGuid.PcdSpcrFeatureEnable                        |FALSE
  gAsfFeaturePkgTokenSpaceGuid.PcdAsfFeatureEnable                          |FALSE

  gS3FeaturePkgTokenSpaceGuid.PcdS3FeatureEnable                            |FALSE

  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosFeatureEnable                    |FALSE

  gLogoFeaturePkgTokenSpaceGuid.PcdLogoFeatureEnable                        |FALSE
  gUserAuthFeaturePkgTokenSpaceGuid.PcdUserAuthenticationFeatureEnable      |FALSE
  gVirtualKeyboardFeaturePkgTokenSpaceGuid.PcdVirtualKeyboardFeatureEnable  |FALSE

  gPlatformPayloadFeaturePkgTokenSpaceGuid.PcdPlatformPayloadFeatureEnable  |FALSE

#
# There seems to be some build parsing odd behavior that requires this PCD to be specified even though
# the *.fdf that consumes it is dependent on the feature flag.
# This section is to ensure that boards have these PCD instantiated.
#
[PcdsFeatureFlag]
  gLogoFeaturePkgTokenSpaceGuid.PcdJpgEnable                              |FALSE
