
linux.efi is a dummy file.
These dummy files needs to be replaced by building the Linux Kernel with an Integrated Initrd.

1.  Follow u-root https://github.com/u-root/u-root#readme to compile an initrd
2.  Follow directions on http://osresearch.net/Building/ to integrate initrd and compile the heads kernel
3.  Copy bzimage with integrated initrd to LinuxBoot/LinuxBinaries/linux.efi

